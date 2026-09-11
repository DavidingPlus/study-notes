# S7-Skills、Subagents 与 MCP

# 第 7 阶段：Skills、Subagents 与 MCP

| **阶段** | s7 |
| --- | --- |
| **分支** | `stage/s7` |
| **本阶段新增** | Skills 斜杠命令、子 Agent、角色配置、后台 agent\_result、MCP 外部工具接入 |
| **依赖上一阶段** | s6 的 SessionManager、AgentRunner、TUI、权限系统和上下文治理 |

## 本阶段要做什么

s6 之后，单个 agent 已经能长期会话，能安全调用工具，能压缩上下文，还能看到项目级 context。

但它仍然是**一个 agent 在一条上下文里做所有事**。

这在小任务里没问题。一旦任务变成：

```latex
重构 core/runner.py：先分析影响范围，再改代码，最后做审查
```

单 agent 模型就开始别扭。分析阶段应该尽量只读；执行阶段需要写文件；审查阶段要挑问题，最好不要继续改。三个阶段的目标、语气、工具权限都不一样。把它们塞进同一个 system prompt，只能靠 LLM 自己切换角色。

另一个边界是工具。内建工具再多，也不可能覆盖所有外部系统：数据库、公司知识库、GitHub、内部 API。我们不应该为了每个外部服务都改 kama 源码。

s7 引入三件事来突破这些边界：

* **Skills**：用户用 `/review ...`、`/orchestrate ...` 触发预定义工作流。
* **Subagents**：父 agent 可以派生隔离的子 agent，把分析、执行、审查拆开跑。
* **MCP**：daemon 启动时连接外部工具服务器，把它们包装成普通 ToolRegistry 工具。

这一章顺着一次命令走：

```latex
/orchestrate 对 core/runner.py 做一轮重构分析和代码审查
```

路径是：`SessionManager` 识别斜杠命令 → `SkillLoader` 渲染 prompt 和工具白名单 → `AgentRunner` 构造受限 registry → 父 Agent 调 `spawn_agent` → 子 Agent 按角色运行 → 事件桥回 TUI → 必要时通过 MCP 工具访问外部能力。

![1779890315850-ae4cb8eb-85b7-4b60-8f4b-fd0e99f42f52.png](https://cdn.davidingplus.cn/images/2026/09/11/1779890315850-ae4cb8eb-85b7-4b60-8f4b-fd0e99f42f52-096682.png)

***

## 斜杠命令：把用户输入变成工作流

用户输入 `/orchestrate ...` 后，消息仍然走 s4 的 `session.send_message`。第一个变化点在 `SessionManager.send_message()`：

```python
# core/session/manager.py（节选）

goal = content
system_prompt_override: str | None = None
tool_whitelist: list[str] | None = None

if content.startswith("/"):
    parts = content[1:].split(None, 1)
    skill_name = parts[0]
    arguments = parts[1] if len(parts) > 1 else ""
    skill = self._skill_loader.resolve(skill_name)
    if skill is not None:
        goal = self._skill_loader.render_prompt(skill, arguments)
        system_prompt_override = skill.system_prompt_template
        tool_whitelist = skill.allowed_tools or None
        await self._bus.publish(SkillInvokedEvent(...))
```

找不到 skill 时，输入会被当作普通用户消息。找到了，`goal` 会变成渲染后的 skill prompt，`system_prompt_override` 和 `tool_whitelist` 一起传给 runner。

skill 文件是带 frontmatter 的 Markdown：

```markdown
---
name: orchestrate
description: 用 planner → executor → reviewer 工作流完成复杂任务
allowed_tools:
  - spawn_agent
  - agent_result
  - task_create
  - task_update
  - task_list
---
你是 multi-agent 协调者。请完成以下目标：

$ARGUMENTS

请先派生 planner，再根据计划派生 executor，最后派生 reviewer。
```

`SkillLoader` 做的事情很克制：

```python
def render_prompt(self, skill: Skill, arguments: str) -> str:
    return skill.system_prompt_template.replace("$ARGUMENTS", arguments)
```

skill 本质上是一个可复用的 system prompt 模板，再加一份工具白名单。

查找顺序是三级：

```latex
.kama/skills/<name>.md
~/.kama/skills/<name>.md
内建 core/skills/builtin/<name>.md
```

项目本地优先，方便一个仓库覆盖内建 `/review` 或新增自己的 `/deploy`。

***

## 工具白名单：协调者不能越界

`SessionManager` 把 `tool_whitelist` 传给 `AgentRunner.run_and_capture()`。runner 构造 registry 时，每个工具都要过一遍 `_ok()`：

```python
allowed: set[str] | None = set(tool_whitelist) if tool_whitelist else None

def _ok(name: str) -> bool:
    return allowed is None or name in allowed

for t in [ReadFileTool(), BashTool(), WriteFileTool(), ListDirTool()]:
    if _ok(t.name):
        registry.register(t)
```

这不是在 prompt 里劝 LLM "不要用 bash"，而是 registry 里根本不给它 bash。LLM 看不到工具 schema，自然无法调用。

对 `/orchestrate` 来说，父 agent 的角色是协调者。它应该拆任务、派生子 agent、收集结果，而不是自己读文件或改代码。所以它的白名单里有 `spawn_agent`、`agent_result`、任务工具，没有 `read_file`、`bash`、`write_file`。

`system_prompt_override` 则替换默认 system prompt：

```python
def system_prompt(self, base: str) -> str:
    parts = [self.system_prompt_override if self.system_prompt_override else base]
    ...
    return "".join(parts)
```

为什么是覆盖，不是追加？因为默认 prompt 和 skill prompt 可能定义不同身份。一个说"你是通用助手"，另一个说"你是协调者"，追加会制造冲突。

***

## spawn\_agent：派生一个干净的子 Agent

父 agent 拿到 orchestrate prompt 后，会调用 `spawn_agent`：

```json
{
  "description": "规划重构",
  "subagent_type": "planner",
  "prompt": "分析 core/runner.py 的影响范围，只读，不要修改文件"
}
```

`SpawnAgentTool` 的参数模型：

```python
class SpawnAgentParams(BaseModel):
    description: str
    prompt: str
    run_in_background: bool = False
    subagent_type: str = ""
```

真正创建子 agent 时，它会生成新的 run\_id、新的 ExecutionContext、新的 EventBus：

```python
child_run_id = new_run_id()
child_context = ExecutionContext(
    run_id=child_run_id,
    goal=p.prompt,
    max_steps=self._max_steps,
    system_prompt_override=profile.system_prompt if profile else None,
)

child_bus = EventBus()
child_registry = self._build_child_registry(child_bus, child_run_id, profile)
child_loop = AgentLoop(self._provider, child_registry, child_bus, ...)
```

最重要的一点：**子 agent 不继承父 agent 的 messages 历史**。

它的上下文只有 `prompt` 变成的那条 user 消息，加上角色配置里的 system prompt。父 agent 如果希望子 agent 知道某个文件路径、约束、目标，就必须写进 `prompt`。

这是一种隔离。子任务越明确，子 agent 越不容易被父级对话里的无关内容干扰。

`SpawnAgentTool` 还限制最大嵌套深度：

```python
if self._depth >= 2:
    return ToolResult(
        content="Subagent nesting limit (2) reached; cannot spawn further subagents.",
        is_error=True,
    )
```

没有这个限制，LLM 可能不断派生子 agent，形成不可控的递归。

> 💡 子 agent 复用同一个 provider 实例，但 messages 是隔离的。连接和客户端对象可以复用，认知上下文不能混在一起。

***

## 角色配置：planner、executor、reviewer

`subagent_type="planner"` 会触发 `AgentProfileLoader` 查找角色配置：

```toml
[agent]
description = "规划 agent：分析目标并拆解任务"
system_prompt = """
你是规划专家。只分析和拆解，不修改文件。
"""
allowed_tools = ["read_file", "list_dir", "task_create", "task_update"]
```

角色配置也是三级查找：

```latex
.kama/agents/<name>.toml
~/.kama/agents/<name>.toml
内建 core/agents/builtin/<name>.toml
```

子 agent registry 按角色 allowed\_tools 过滤：

```python
allowed = set(profile.allowed_tools) if profile and profile.allowed_tools else None

def _allowed(name: str) -> bool:
    return allowed is None or name in allowed

for t in [ReadFileTool(), BashTool(), WriteFileTool(), ListDirTool()]:
    if _allowed(t.name):
        registry.register(t)
```

三个典型角色的边界是：

| 角色 | 主要职责 | 工具边界 |
| --- | --- | --- |
| planner | 读上下文、拆任务、制定计划 | 只读和 task 工具 |
| executor | 按计划修改代码、运行测试 | bash/read/write/list/task |
| reviewer | 复查结果、指出风险 | 只读查询，尽量不写 |

这不是纯 prompt 约束。工具白名单让 planner 根本拿不到 `write_file`，系统层面降低越界概率。

***

## 子 Agent 的事件怎么回到 TUI

子 agent 有自己的 `child_bus`，但 TUI 订阅的是父 run 的事件流。要让 TUI 看到子 agent 的 token 和工具调用，需要事件桥：

```python
child_bus = EventBus()

async def _bridge(event: BaseModel) -> None:
    await self._parent_bus.publish(event)

child_bus.subscribe(_bridge)
```

子 agent 自己的事件先发布到 `child_bus`，再由 `_bridge` 重新发布到父 bus。父 bus 上的 IPC broadcaster 会把事件推给 TUI。

同时，父 bus 还会发布专门的子 agent 生命周期事件：

```python
await self._parent_bus.publish(
    SubagentStartedEvent(
        run_id=child_run_id,
        parent_run_id=self._parent_run_id,
        description=p.description,
        ts=_now(),
    )
)
```

结束时发布 `SubagentFinishedEvent`。TUI 用这些事件做缩进和层级显示：哪个 run 是父 run，哪个 run 是子 run。

![1779890315846-c5dabac2-8874-47bf-8d9d-a48f6e757485.png](https://cdn.davidingplus.cn/images/2026/09/11/1779890315846-c5dabac2-8874-47bf-8d9d-a48f6e757485-029608.png)

前台子 agent 会一直跑到结束，然后把 `child_context.result` 包成 ToolResult 返回给父 LLM。父 LLM 再根据结果决定下一步，比如派生 executor。

***

## 后台子 Agent 与 agent\_result

有些子任务可以并行。比如 planner 拆出三个互不相关的代码区域，父 agent 可以同时派生三个 reviewer。

这时 `spawn_agent` 可以设置：

```json
{"run_in_background": true}
```

后台模式下，工具不会等待子 agent 完成：

```python
task = asyncio.create_task(
    self._run_background(child_loop, child_context, child_bus, ...)
)
self._task_registry.register(child_run_id, task, child_context)
return ToolResult(
    content=(
        f"Subagent started in background. run_id={child_run_id}. "
        f"Use agent_result(run_id='{child_run_id}') to retrieve result."
    )
)
```

`BackgroundTaskRegistry` 保存 `run_id → (asyncio.Task, ExecutionContext)`。父 agent 后面调用 `agent_result(run_id=...)`：

```python
if not task.done():
    return ToolResult(content="still running")
if task.cancelled():
    return ToolResult(content="Subagent was cancelled.", is_error=True)
exc = task.exception()
if exc is not None:
    return ToolResult(content=f"Subagent raised an exception: {exc}", is_error=True)
return ToolResult(content=context.result or "Subagent completed with no text result.")
```

`task.done()` 是非阻塞检查。父 agent 可以继续做别的事，隔一会再查询结果。

注意：后台子 agent 的事件仍然通过 bridge 推给 TUI。父 agent 不等待它，不代表用户看不到它。

***

## MCP：把外部工具接进 ToolRegistry

Skills 和 subagents 解决的是"怎么组织 agent 工作"。MCP 解决的是"工具从哪里来"。

daemon 启动时，`CoreApp` 创建 `McpServerManager`，根据配置连接外部 server：

```python
self._mcp_manager = McpServerManager()
if self._config.mcp.servers:
    await self._mcp_manager.start_all(self._config.mcp.servers)
```

`start_all()` 逐个连接 server，发现工具，再包装成 `McpTool`：

```python
client = await self._connect(cfg)
tool_defs = await client.list_tools()
for tool_def in tool_defs:
    self._tools.append(McpTool(client, cfg.name, tool_def))
self._clients[cfg.name] = client
```

stdio server 会作为子进程启动，tcp server 会连接已有进程：

```toml
[[mcp.servers]]
name = "filesystem"
transport = "stdio"
command = "npx"
args = ["-y", "@modelcontextprotocol/server-filesystem", "/tmp"]

[[mcp.servers]]
name = "internal-kb"
transport = "tcp"
host = "10.0.0.5"
port = 3000
```

`McpTool` 让外部工具看起来像内建工具：

```python
class McpTool(BaseTool):
    def __init__(self, client, server_name, tool_def):
        self.name = f"{server_name}__{tool_def.name}"
        self.description = tool_def.description or f"MCP tool from {server_name}"
        self.input_schema = tool_def.input_schema or {"type": "object", "properties": {}}

    async def invoke(self, params):
        try:
            content = await self._client.call_tool(self._tool_def.name, dict(params))
            return ToolResult(content=content)
        except McpServerUnavailableError:
            return ToolResult(
                content=f"mcp server '{self._server_name}' unavailable",
                is_error=True,
                error_type="runtime_error",
            )
```

工具名带 server 前缀，比如 `filesystem__read_file`。这样不会和内建 `read_file` 冲突，也能让 LLM 看出工具来自哪个 server。

每次 run 构造 registry 时，runner 注入已发现的 MCP 工具：

```python
if self._mcp_manager is not None:
    for mcp_tool in self._mcp_manager.get_tools():
        if _ok(mcp_tool.name):
            registry.register(mcp_tool)
```

MCP 工具走同一条 `invoke_tool()` 路径，所以 s5 的权限、失败分类、TUI 展示都能复用。server 不可用时，`McpTool` 返回 `is_error=True`，AgentLoop 不需要特殊分支。

![1779890315896-a99f87a9-27f5-48b1-9458-987821141c0c.png](https://cdn.davidingplus.cn/images/2026/09/11/1779890315896-a99f87a9-27f5-48b1-9458-987821141c0c-903311.png)

***

## 验证

手动验证 skill：

```bash
uv run kama-core
uv run kama-tui
```

在 TUI 输入：

```latex
/review KamaClaude/src/kama_claude/core/loop.py
```

看事件流里是否出现 `skill.invoked`，并确认这次 run 使用的是 review skill 允许的工具。

手动验证 subagent：

```latex
/orchestrate 对 KamaClaude/src/kama_claude/core/runner.py 做一次重构风险分析
```

TUI 应该能看到子 agent 开始和结束事件，子 agent 工具调用会有缩进或层级提示。`~/.kama/sessions/<sid>/runs/` 下也会出现子 run 的 `events.jsonl`。

手动验证 MCP：在 `~/.kama/config.toml` 配置一个 MCP server，重启 daemon，观察日志中 server connected 和 discovered tools。然后让 agent 使用对应 `server__tool` 名称的工具。

***

## 小结与展望

s7 把系统从"一个 agent 使用内建工具"推进到"可组织、可派生、可扩展"：

* Skills 把常用工作流固化成斜杠命令，并能限制父 agent 的工具集。
* Subagents 把复杂任务拆给隔离上下文里的子 agent，每个子 agent 可以有自己的角色和工具边界。
* 事件桥让子 agent 的输出仍然进入同一条 TUI 事件流。
* 后台 subagent 和 `agent_result` 让并行任务成为可能。
* MCP 把外部工具服务器接进同一套 ToolRegistry/invoke\_tool/EventBus 链路。


> 更新: 2026-05-28 14:11:49  
> 原文: <https://www.yuque.com/chengxuyuancarl/nvd81p/ar9vunb2b580gqnw>