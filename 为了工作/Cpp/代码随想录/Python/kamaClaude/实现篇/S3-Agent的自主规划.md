# S3-Agent 的自主规划

# 第 3 阶段：Agent 的自主规划与终端可视化

| **阶段** | s3 |
| --- | --- |
| **分支** | `stage/s3` |
| **本阶段新增** | Agent 内部任务系统、扩展工具集（bash / write\_file / list\_dir）、TUI 终端化改版 |
| **依赖上一阶段** | s2 的守护进程架构、EventBus、IPC 订阅机制 |

## 本阶段要做什么

s2 结束时，整个系统架构已经完成：`kama-core` 守护进程持续运行，`kama run` 发送目标、订阅事件，TUI 实时显示进度。

但它有一个根本性的局限：**agent 只能执行用户分解好的单条指令**。

如果你的目标是"分析这个 Python 项目的代码风格并写一份报告"，你不能直接把这句话扔给 agent。它会在一次对话循环里把所有事情混在一起做，结果很可能是乱七八糟的。你需要手动拆解：先让它分析目录结构，再逐一读文件，再总结……这些步骤完全由人来排列和触发。

s2 的任务系统（`task_create` / `task_update`）确实存在，但它暴露在 IPC 层上，是用户通过客户端操控的外部接口。问题在于：任务分解本质上是一件认知性工作，LLM 在这件事上比人更系统、更不容易漏项。为什么不让 agent 自己来？

s3 做了一个根本性的翻转：**把任务系统从用户可见的控制接口，变成 agent 的私有认知工具**。用户只给一个目标，agent 自己决定要不要拆解、拆成什么、按什么顺序做。从用户视角看，命令没有变：

```bash
uv run kama run --goal "分析代码库并写报告"
```

变的是 agent 在这个目标下的行为模式——它现在会先主动规划任务，然后按计划执行。

***

## 守护进程启动 run

用户的命令经过 CLI → SocketClient → daemon 这条 s2 已经建好的链路，到达 `CoreApp._agent_run_handler`：

```python
# core/app.py

async def _agent_run_handler(self, params: dict[str, Any]) -> AgentRunResult:
    cmd = AgentRunCommand.model_validate(params)
    run_id = new_run_id()
    runner = AgentRunner(self._config, bus=self._bus, trace=self._trace)
    run_task = asyncio.create_task(runner.run(cmd.goal, run_id=run_id))
    self._running_runs.add(run_task)
    run_task.add_done_callback(self._running_runs.discard)
    return AgentRunResult(run_id=run_id)
```

这里有一个重要的细节：`run_id` 在 handler 里生成，然后传入 `runner.run()`。这样 `AgentRunResult` 能立刻把 `run_id` 返回给客户端——TUI 在 agent 跑完之前就拿到了 run\_id，可以提前准备订阅，不会错过 `run.started` 事件。

`asyncio.create_task()` 把整个 run 甩到后台异步执行，handler 立即返回，不阻塞 socket 服务器接收下一条命令。`_running_runs` 维护一个活跃 task 的集合，shutdown 时逐个 cancel，保证进程不会在 agent 还在跑的时候直接退出。

***

## AgentRunner 接入 TaskManager

进入 `AgentRunner.run_and_capture()`，s3 在这里加了两行关键代码：

```python
# core/runner.py

async def run_and_capture(self, goal: str, *, run_id: str | None = None) -> RunOutcome:
    run_id = run_id or new_run_id()
    run_path = self._runs_dir / run_id
    run_path.mkdir(parents=True, exist_ok=True)

    task_manager = TaskManager(run_path / ".tasks")   # [new]
    ...
    registry = self._build_registry(task_manager)      # [new]
    loop = AgentLoop(provider, registry, bus)
    ...
```

`TaskManager` 的初始化路径是 `runs/<run_id>/.tasks/`——每个 run 的任务数据完全隔离，互不干扰，可以随时回溯某次 run 的完整规划记录。

`_build_registry(task_manager)` 把 4 个任务工具和这个 `task_manager` 实例一起注册进工具注册表：

```python
# core/runner.py

def _build_registry(self, task_manager: TaskManager) -> ToolRegistry:
    registry = ToolRegistry()
    for t in [ReadFileTool(), BashTool(), WriteFileTool(), ListDirTool()]:
        registry.register(t)
    for t in [
        TaskCreateTool(task_manager),
        TaskUpdateTool(task_manager),
        TaskListTool(task_manager),
        TaskGetTool(task_manager),
    ]:
        registry.register(t)
    return registry
```

所有 4 个任务工具共享同一个 `task_manager` 实例。这是关键：`task_create` 写入的文件，`task_update` 和 `task_list` 能读到，因为它们操作的是同一个 `.tasks/` 目录下的同一批 JSON 文件。

***

## TaskManager：同步的文件 CRUD

`TaskManager` 是 s3 任务系统的核心，但它有意做得很简单：

```python
# core/task/manager.py

class TaskManager:
    def __init__(self, tasks_dir: Path) -> None:
        self._dir = tasks_dir
        self._dir.mkdir(parents=True, exist_ok=True)
        self._next_id = self._max_id() + 1

    def create(self, subject: str, description: str = "", blocked_by: list[int] | None = None) -> Task:
        ...
        task = Task(id=self._next_id, subject=subject, ..., blocked_by=list(blocked_by or []))
        self._save(task)        # 直接写 JSON 文件
        self._next_id += 1
        return task

    def update(self, task_id: int, *, status, add_blocked_by, remove_blocked_by) -> Task:
        task = self._load(task_id)     # 读文件
        if status == "completed":
            self._clear_dependency(task_id)    # 自动解除其他任务的阻塞
        ...
        self._save(task)        # 写回文件
        return task
```

`TaskManager` 没有异步方法，没有 EventBus 依赖，没有状态机。它是**纯粹的同步文件 CRUD 层**，每次操作就是读一个 JSON 文件、改一改、写回去。

为什么不用数据库？任务数量通常是个位数到十几个，文件 I/O 的开销完全可以忽略。用文件的好处是：任务的完整历史可以直接用 `ls` 和 `cat` 查看，不需要任何工具，调试非常方便。

> 💡 **ID 用整数，不用 UUID**
>
> 工具 schema 里 `task_id` 是 `integer`，调用 `task_update` 时参数是 `{"task_id": 1}`。如果用 UUID，LLM 需要先记住完整的随机字符串，再在下一个工具调用里原样复述，出错概率高得多。整数 ID 在对话历史里不占位置，LLM 几乎不会用错。

每个任务持久化为一个独立 JSON 文件：

```json
{
  "id": 1,
  "subject": "分析目录结构",
  "description": "了解整体布局，找出核心模块",
  "status": "completed",
  "blocked_by": [],
  "created_at": "2026-05-19T10:00:01Z",
  "updated_at": "2026-05-19T10:00:45Z"
}
```

状态只有三种：`pending`（等待）、`in_progress`（进行中）、`completed`（完成）。没有 pause、retry、cancel——这些是流程控制逻辑，不是任务本身的状态。agent 如果需要重试，直接用工具再做一遍就好了。

![1779885097737-85d8005e-2c7b-490e-a56e-2316d76fdf13.png](https://cdn.davidingplus.cn/images/2026/09/11/1779885097737-85d8005e-2c7b-490e-a56e-2316d76fdf13-288174.png)

### `blocked_by` 的自动级联

任务依赖关系用 `blocked_by` 字段表示：`task_2.blocked_by = [1]` 意味着任务 2 在等待任务 1 完成才能开始。

当 LLM 把任务 1 标记为 `completed`，`_clear_dependency(1)` 会扫描 `.tasks/` 目录下所有文件，把 `blocked_by` 里含有 `1` 的条目全部移除——不需要 LLM 手动去更新依赖关系，直接就解锁了。

```python
# core/task/manager.py

def _clear_dependency(self, completed_id: int) -> None:
    for f in self._dir.glob("task_*.json"):
        data = json.loads(f.read_text())
        blocked = [int(x) for x in data.get("blocked_by", [])]
        if completed_id in blocked:
            data["blocked_by"] = [x for x in blocked if x != completed_id]
            data["updated_at"] = _now()
            f.write_text(json.dumps(data, indent=2, ensure_ascii=False))
```

这个设计让 LLM 的操作序列非常自然：创建任务时声明依赖，完成任务时只需更新自己的状态，其他任务的解锁是自动发生的副作用。

***

## 工具集扩展到 8 个

s3 的 `ToolRegistry` 里注册了 8 个工具。LLM 在每次 plan 阶段都能看到它们全部的 schema，自主决定使用哪些、按什么顺序。

**任务工具（4 个）：**

| 工具 | 作用 |
| --- | --- |
| `task_create` | 创建新任务，可选设置 `blocked_by` 依赖 |
| `task_update` | 更新状态（pending / in\_progress / completed）或调整依赖 |
| `task_list` | 列出所有任务的当前状态，返回格式化摘要 |
| `task_get` | 获取单个任务的完整 JSON |

`task_list` 的返回格式是专为 LLM 设计的紧凑表示：

```plain
[ ] #1: 分析目录结构
[>] #2: 读取核心模块代码 (blocked by: [])
[x] #3: 分析代码风格
[ ] #4: 写报告 (blocked by: [2, 3])
```

`[ ]` 是 pending，`[>]` 是 in\_progress，`[x]` 是 completed。LLM 一眼就能判断哪些任务可以开始、哪些还在等待。

**执行工具（4 个）：**

| 工具 | 作用 |
| --- | --- |
| `read_file` | 读取文件内容，最大 512 KB，自动截断 |
| `write_file` | 写文件，自动创建父目录，阻止路径穿越 |
| `list_dir` | 列出目录结构，可控制递归深度 |
| `bash` | 执行 shell 命令，合并 stdout/stderr，64 KB 输出上限 |

s1 和 s2 只有 `read_file`。s3 加入 `bash`、`write_file`、`list_dir` 之后，agent 能做的事彻底不同了：它可以运行测试、调用命令行工具、写入文件、遍历目录。对于代码分析、文件生成这类任务，这几个工具的组合基本已经够用。

`bash` 工具的实现：

```python
# core/tools/builtin/bash.py

async def invoke(self, params: dict[str, object]) -> ToolResult:
    p = BashParams.model_validate(params)
    proc = await asyncio.create_subprocess_shell(
        p.command,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.STDOUT,
    )
    try:
        stdout_bytes, _ = await asyncio.wait_for(proc.communicate(), timeout=p.timeout)
    except TimeoutError:
        proc.kill()
        await proc.communicate()
        return ToolResult(content=f"[timeout after {p.timeout}s]", is_error=True, error_type="timeout")
    ...
    if proc.returncode != 0:
        return ToolResult(content=f"[exit {proc.returncode}]\n{output}", is_error=True, ...)
    return ToolResult(content=output or "[no output]")
```

`asyncio.create_subprocess_shell` 执行任意 shell 命令，`asyncio.wait_for` 控制超时（默认 60 秒，最大允许 120 秒），超时后 `proc.kill()` 强制终止。非零退出码返回 `is_error=True` 的 `ToolResult`，让 LLM 知道命令失败了。

> ⚠️ `bash`\*\* 工具的安全边界\*\*
>
> 没有沙箱，没有权限限制——这是 s3 的已知局限，适合本地开发场景，生产部署需要额外的隔离层。

***

## LLM 拿到任务工具之后

这是整个 s3 最核心的一步，但没有新代码——完全靠 s1 建好的 `AgentLoop + AnthropicProvider` 机制，加上新的工具 schema。

`AgentLoop.run()` 的循环体没有变：

```plain
while not context.is_done():
    → plan:    provider.chat(messages, tool_schemas, ...)
    → observe: 把 LLM 响应追加进 context.messages
    → act:     对每个 tool_call 执行 invoke_tool()
    → 终止检查
```

变的是 `tool_schemas` 的内容——现在包含了 `task_create`、`task_update` 等 4 个任务工具的 schema。LLM 读到这些 schema 后，会根据 system prompt 的指示和它自己对"什么时候应该拆解任务"的判断，决定要不要在第一步就调用 `task_create`。

一次典型的多任务 run，`context.messages` 的演变是这样的：

```plain
step 1:
  → LLM: [task_create("分析目录结构"), task_create("读取核心模块", blocked_by=[1]), ...]
  → context.messages 追加 assistant 消息（tool_use blocks）
  → invoke_tool 依次执行，任务文件写入 .tasks/
  → context.messages 追加 user 消息（tool_result blocks）

step 2:
  → LLM: [task_update(1, "in_progress"), list_dir(".")]
  → ...

step 3:
  → LLM: [read_file("src/core/loop.py"), ...]
  → task_update(1, "completed")  ← 触发 _clear_dependency，任务 2 的 blocked_by 被清空

step N:
  → LLM: [write_file("/tmp/report.md", content="...")]
  → task_update(N, "completed")
  → end_turn
```

![1779885097937-7c023912-0148-47c3-86b7-7484f13b4de5.png](https://cdn.davidingplus.cn/images/2026/09/11/1779885097937-7c023912-0148-47c3-86b7-7484f13b4de5-067317.png)

这里有一个设计决策：**任务工具调用和普通工具调用在事件流上完全一样**。LLM 调用 `task_create` 产生的是 `tool.call_started` + `tool.call_finished` 事件，跟调用 `bash` 或 `read_file` 完全相同的格式。TUI 不需要知道"这是任务操作"——它只是看到一个工具调用，显示一个工具调用块。

任务系统是 agent 的认知行为，不是系统的控制操作，把它暴露成独立的事件类型反而会让 TUI 变得复杂。

***

## TUI 改版：单列终端滚动流

现在切换到用户视角。`kama-tui` 里有什么变化？

![1779886751589-78b76af5-5404-4d21-a698-6edf9cb87f1a.png](https://cdn.davidingplus.cn/images/2026/09/11/1779886751589-78b76af5-5404-4d21-a698-6edf9cb87f1a-795346.png)

整个界面就是一个 `VerticalScroll` 容器，事件进来时动态追加 widget，始终自动滚动到底部。用户按 `ctrl + q` 退出。

***

## LLM 流式输出的原地累积

`KamaTuiApp` 用 `_handle_event_inner` 路由所有到来的事件。收到 `llm.token` 时的处理是这样的：

```python
# tui/app.py

def _handle_event_inner(self, event: dict[str, Any]) -> None:
    t = event.get("type", "")

    if t == "llm.token":
        token = event.get("token", "")
        if self._current_llm is None:
            llm_block = LLMStreamBlock()
            self._append(llm_block)
            self._current_llm = llm_block
        self._current_llm.append_token(token)
        return

    self._break_llm()   # 任何非 token 事件都先结束当前 LLM 块
    ...
```

`LLMStreamBlock` 是一个 `Static` 子类，在同一个 widget 里累积所有 token：

```python
# tui/app.py

class LLMStreamBlock(Static):
    def __init__(self) -> None:
        super().__init__("")
        self._text = ""
        self._finalized = False

    def append_token(self, token: str) -> None:
        self._text += token
        self.update(self._text)

    def finalize_markdown(self) -> None:
        self._finalized = True
        if self._text.strip():
            self.update(Markdown(self._text, code_theme="monokai"))
```

每到一个 token，`self._text` 追加，然后用新字符串刷新 widget 显示。流结束时（收到非 token 事件），`finalize_markdown()` 把累积的文本整体渲染成 Markdown，代码块、列表、粗体都正确显示。

> 💡 **为什么不每个 token 追加一个 widget？**
>
> 如果每个 token 都 `mount` 一个新 widget，一次 LLM 回复可能产生几百个 widget 对象，Textual 的布局引擎需要反复计算整个 widget 树的高度和位置，帧率会显著下降，长输出时肉眼可见地卡顿。`update()` 在原地替换内容，布局引擎只需要重绘这一个 widget，代价小得多。

`_current_llm` 是当前"活跃"的 `LLMStreamBlock` 引用。当收到任何非 token 事件时，`_break_llm()` 调用 `finalize_markdown()` 并把引用置为 `None`：下一个 token 到来时会新建一个 `LLMStreamBlock`，形成一个新的文字块。LLM 在不同 step 里的思考内容在视觉上是分隔的，不会混在一起。

***

## 工具调用块的折叠展开

工具调用的显示要解决一个矛盾：工具调用很频繁，但你大多数时候只关心它成功了没有，不需要看完整的参数和输出。把所有内容默认展开会淹没 LLM 的思考文字；全部折叠又让你没有办法深入了解某次调用的细节。

s3 的解法：默认折叠，点击展开。

```python
# tui/app.py

class ToolCallBlock(Widget):
    DEFAULT_CSS = """
    ToolCallBlock { height: auto; padding: 0 2; color: $text-muted; }
    ToolCallBlock > .detail { display: none; padding: 0 2 0 4; color: $text-muted; }
    ToolCallBlock.expanded > .detail { display: block; }
    """

    def compose(self) -> ComposeResult:
        yield Static(self._summary(), classes="summary")
        yield Static("", classes="detail")

    def on_click(self) -> None:
        if not self._finished:
            return
        if "expanded" in self.classes:
            self.remove_class("expanded")
        else:
            detail = self.query_one(".detail", Static)
            detail.update(
                f"[dim]params[/dim]\n{self._params_full}\n\n"
                f"[dim]output[/dim]\n{self._output}\n\n"
                f"[dim]elapsed:[/dim] {self._elapsed_ms}ms"
            )
            self.add_class("expanded")
```

`.detail` 这个子 widget 默认是 `display: none`——存在于 DOM 里，但不占空间、不显示。给父 widget 加上 `expanded` 类后，CSS 规则 `ToolCallBlock.expanded > .detail { display: block; }` 立即生效，detail 出现。

折叠/展开是纯 CSS 切换，不需要 `mount`/`remove` widget，也不需要重新布局整棵树，只是修改显示属性。点击行为很流畅。

工具出错时，摘要行的颜色变红，折叠状态下就能看到出了什么问题——不需要展开细节。

工具输出通过 `ToolCallFinishedEvent.output` 字段传递给 TUI。s3 在这个 event 上新增了 `output: str = ""` 字段，`invoke_tool()` 在 publish 之前把 `result.content` 塞进去，这样 TUI 能直接从事件里拿到工具输出，不需要回查 `events.jsonl`。

***

## 验证

启动守护进程：

```bash
uv run kama-core
```

启动 TUI：

```bash
uv run kama-tui
```

触发一个足够复杂的目标（让 agent 有理由拆解）：

```bash
uv run kama run --goal "列出当前目录下所有 Python 文件，统计总行数，然后把结果写入 /tmp/py_stats.txt"
```

在 TUI 里观察：

* LLM 的思考文字实时出现，每个 step 独立一块
* `tool task_create` 行出现，完成后显示结果行
* `tool bash` 行出现，点击后展开完整命令和输出
* 最后显示 run 完成状态

## 小结与展望

s3 完成了**任务系统的内化**。它让 agent 从"执行用户给定步骤的工具"变成了"能够自主规划的 agent"。`TaskManager` 刻意保持极简——同步、纯文件、不发事件，任务是 agent 的认知状态，不是系统的运行时状态，两者不应该耦合。任务工具和普通工具在事件流上无差别，TUI 不需要为任务系统单独建模。

**当前的局限**

`blocked_by` 是 agent 自己声明的，系统不强制检查——LLM 完全可以在任务 1 还没完成时就开始任务 2，任务依赖是"建议性的"而不是"执行性的"。这对于当前的单线程 agent 来说不是问题（它一次只做一件事），但如果未来要并行执行任务，就需要真正的依赖检查。


> 更新: 2026-06-10 17:43:34  
> 原文: <https://www.yuque.com/chengxuyuancarl/nvd81p/vduokwbgevrbxzqq>