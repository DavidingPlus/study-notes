# S1-Agent 第一次运行

# 第 1 阶段：Agent 第一次运行

| **阶段** | s1 |
| --- | --- |
| **分支** | `stage/s1` |
| **本阶段新增** | LLM 调用、工具系统、Agent 循环、事件流、CLI run 命令 |
| **依赖上一阶段** | s0 的配置管理（`KamaConfig`）、项目骨架 |

## 本阶段要做什么

s0 结束时我们有一个精致的空壳：配置能读，日志能写，守护进程能启动，然后什么都不发生。

s1 要让它第一次真正"做事"。目标是能执行这条命令：

```bash
uv run kama run --goal "总结 README.md 的主要章节"
```

然后在终端看到 agent 实时思考、调工具、输出结果的过程：

```plain
[run] 20260511-161020-abc123
[step 1] planning...
I'll read the README.md file to get its contents.
[tool] read_file {"path": "README.md"}
[tool] read_file ✓  4ms
[step 1] done
[step 2] planning...
# Summary
The README covers the following sections...
[step 2] done
[run] success  2 steps  5.3s
```

与此同时，`runs/20260511-161020-abc123/events.jsonl` 里会留下整个过程的完整记录，每一步 LLM 做了什么、每次工具调用的结果和耗时，全部可查。

实现这个目标需要解决一连串问题：goal 怎么传给 LLM？LLM 要调工具怎么办？工具出错了继续还是终止？循环什么时候停？执行过程怎么实时显示、怎么持久化？

这些问题环环相扣。我们不会在最开始一次性回答所有问题，而是顺着 `kama run` 的执行路径一路往下走，问题出现时再解答。

***

## 设计：顺着执行流走一遍

在写代码之前，先在脑子里把一次完整的 `kama run` 过一遍，看整条链路需要哪些东西。

用户按下回车，命令行解析出 `--goal`，然后呢？

goal 需要被传给 LLM，但不是简单地发一次消息、等一次回复。Agent 要的是多轮对话：LLM 可能说"帮我读一下这个文件"，我们执行，把结果告诉 LLM，LLM 继续思考……这个过程可能反复好几轮，直到 LLM 觉得任务完成了。

所以我们需要：

* 一个地方**维护对话历史**，每轮都追加进去（`ExecutionContext`）
* 一个**驱动循环的控制器**，决定什么时候继续、什么时候停（`AgentLoop`）
* 一个**调用 LLM 的组件**，拿到响应（`AnthropicProvider`）
* 一套**工具系统**，让 LLM 真的能做事（`ToolRegistry` + `ReadFileTool`）
* 一条**观测管道**，把发生的事情广播给终端和日志文件（`EventBus` + `EventWriter` + `StdoutPrinter`）

把这些组装起来的是 `AgentRunner`，它是整条链路的起点。

![1779880447903-a46859b1-f7d2-4021-a0b0-cfb8c88a1297.png](./img/8MR3iZV3po0gVP6S/1779880447903-a46859b1-f7d2-4021-a0b0-cfb8c88a1297-725605.png)

接下来按这张图从上到下实现。

***

## 实现

### 命令行入口

用户输入 `kama run --goal "..."` 回车之后，最先跑起来的是 `cli/main.py`。它用 Python 标准库的 `argparse` 解析命令行参数，然后把控制权交给对应的子命令处理函数：

```python
# cli/main.py（节选）

config = get_config()   # 读取配置（s0 已实现）

if args.command == "run":
    cmd_run(args.goal, config)
```

`get_config()` 负责按优先级加载配置（默认值 → TOML 文件 → `.env` → 环境变量），这是 s0 已经建好的基础设施，这里直接用。

进入 `cmd_run`：

```python
# cli/commands/run.py

def cmd_run(goal: str, config: KamaConfig) -> None:
    printer = StdoutPrinter()
    runner = AgentRunner(config, extra_handlers=[printer.handle])
    try:
        asyncio.run(runner.run(goal))
    except KeyboardInterrupt:
        sys.exit(130)
```

这里做了三件事：创建负责打印终端输出的 `StdoutPrinter`，创建负责组装所有零件的 `AgentRunner`，然后用 `asyncio.run()` 启动异步运行。

`asyncio.run()` 的作用是启动一个事件循环并运行传入的协程。我们的 agent 需要同时等待网络请求（调用 LLM API）和文件 I/O，用异步的方式可以在等待一件事的时候去做另一件事，而不是傻等着。你可以把 `asyncio.run()` 理解为"开始干活"的发令枪。

`KeyboardInterrupt` 对应用户按 Ctrl+C，退出码 130 是 Unix 的约定（128 + SIGINT 信号编号 2），这样调用 `kama` 的脚本就能识别出"是被用户中断的"。

### AgentRunner 把所有零件组装起来

`AgentRunner.run()` 是真正的组装现场。在 `AgentLoop` 开始循环之前，它需要把所有依赖都准备好：

```python
# core/runner.py（节选）

async def run(self, goal: str) -> None:
    # 1. 为这次运行生成唯一 ID，创建对应目录
    run_id = new_run_id()
    run_path = self._runs_dir / run_id
    run_path.mkdir(parents=True, exist_ok=True)

    # 2. 建立事件总线，订阅所有监听者
    bus = EventBus()
    for h in self._extra_handlers:   # StdoutPrinter 从这里进来
        bus.subscribe(h)

    # 3. 准备 LLM、工具注册表、循环控制器
    provider = self._provider or AnthropicProvider(self._config.llm.default_model)
    registry = ToolRegistry()
    registry.register(ReadFileTool())
    loop = AgentLoop(provider, registry, bus)

    # 4. 创建"工作记忆"，goal 在这里成为第一条消息
    context = ExecutionContext(run_id=run_id, goal=goal, max_steps=self._config.agent.max_steps)

    # 5. 打开事件文件，然后正式开始
    async with EventWriter(run_path / "events.jsonl") as writer:
        writer.subscribe(bus)
        await bus.publish(RunStartedEvent(run_id=run_id, goal=goal, ts=_now()))
        ...
        await loop.run(context)
        ...
        await bus.publish(RunFinishedEvent(...))
```

`run_id` 的格式是 `YYYYMMDD-HHMMSS-xxxxxx`（例如 `20260511-161020-abc123`），时间戳让 `ls runs/` 后能一眼认出最近一次运行，6 位随机十六进制保证同一秒内多次启动不会冲突。

注意 `EventWriter` 是用 `async with` 打开的。这保证无论后面发生什么——正常完成、报错、被 Ctrl+C 中断——事件文件都会被正确关闭，不会留下损坏的文件。

### EventBus——让所有人都能听到

在循环开始前，先弄清楚 `EventBus` 是什么，因为后面所有代码都会用到它。

```python
# core/events/bus.py

class EventBus:
    def __init__(self) -> None:
        self._subscribers: list[EventHandler] = []

    def subscribe(self, handler: EventHandler) -> None:
        self._subscribers.append(handler)

    async def publish(self, event: BaseModel) -> None:
        for handler in self._subscribers:
            await handler(event)
```

EventBus 是一个广播中心。任何代码调用 `bus.publish(某个事件)`，所有订阅了这个 bus 的处理函数就会按注册顺序依次收到它。

s1 里有三个订阅者：

* `EventWriter.handle`：把事件序列化成 JSON 行，写入 `events.jsonl`
* `StdoutPrinter.handle`：把事件格式化后打印到终端
* `AgentRunner` 传进来的 `extra_handlers`（目前就是 `StdoutPrinter`，这两条是同一个东西）

![1779880447709-59bb7926-b669-4977-b823-a828b19b35ee.png](./img/8MR3iZV3po0gVP6S/1779880447709-59bb7926-b669-4977-b823-a828b19b35ee-686165.png)

为什么要这样设计，而不是在每个需要打印或记录的地方直接调用？

因为 `AgentLoop` 不应该知道外面有没有终端，有没有文件要写。它只需要"广播一件事发生了"，至于谁关心、怎么处理，是外面的事。这样 `AgentLoop` 的代码保持干净，测试时也可以接一个简单的 mock handler 而不需要真实的文件系统。

**EventWriter：每写一行就立即 flush**

```python
# core/events/writer.py（节选）

async def handle(self, event: BaseModel) -> None:
    try:
        self._file.write(event.model_dump_json() + "\n")
        self._file.flush()   # 立即刷盘，不等缓冲区满
    except (OSError, ValueError) as e:
        logger.error("EventWriter: failed to write event: %s", e)
```

每写一行就立即调用 `flush()`，不做批量缓冲。代价是频繁的磁盘写入，但好处是：如果程序在任何一步崩溃了，已记录的事件不会丢失。`OSError` 在这里不会重新抛出——磁盘满了不应该导致 agent 停止工作。

**事件类型是什么样的**

所有事件都是 pydantic 模型，定义在 `core/bus/events.py` 里：

```python
class RunStartedEvent(BaseModel):
    type: Literal["run.started"] = "run.started"
    run_id: str
    goal: str
    ts: str   # ISO 8601 时间戳，例如 "2026-05-11T16:10:20.001Z"

class LlmTokenEvent(BaseModel):
    type: Literal["llm.token"] = "llm.token"
    run_id: str
    token: str   # LLM 流式输出的单个文本片段
    ts: str

class ToolCallFinishedEvent(BaseModel):
    type: Literal["tool.call_finished"] = "tool.call_finished"
    run_id: str
    tool_use_id: str
    tool_name: str
    elapsed_ms: int
    ts: str
```

每种事件的 `type` 字段是固定的字符串常量（`Literal`），写进 `events.jsonl` 后，读取时可以用 `type` 字段来判断这行是什么类型的事件。s1 共有 11 种事件类型，覆盖 run、step、LLM 调用、工具调用的全部生命周期。

### ExecutionContext -- Agent 的记忆

`ExecutionContext` 是 agent 的工作记忆，在整个循环里被所有组件共享和修改：

```python
# core/context.py

@dataclass
class ExecutionContext:
    run_id: str
    goal: str
    max_steps: int
    messages: list[dict[str, Any]] = field(default_factory=list)
    step: int = 0
    status: str = "running"   # "running" | "success" | "failed"
    reason: str | None = None

    def __post_init__(self) -> None:
        # goal 在初始化时自动变成第一条对话消息
        if not self.messages:
            self.messages.append({"role": "user", "content": self.goal})
```

`messages` 是整个上下文最核心的部分。它的格式和 Anthropic API 要求的完全一致，调用 LLM 时直接把 `context.messages` 传进去，不需要任何转换。

随着循环推进，`messages` 会不断追加新内容：

```python
    def add_assistant_message(self, content: list[Any]) -> None:
        self.messages.append({"role": "assistant", "content": content})

    def add_tool_result(self, tool_use_id: str, content: str, is_error: bool = False) -> None:
        # 工具结果作为 user 消息追加
        # 同一步的多个工具结果必须合并在同一条消息里（Anthropic API 的要求）
        block = {"type": "tool_result", "tool_use_id": tool_use_id, "content": content}
        if is_error:
            block["is_error"] = True

        last = self.messages[-1] if self.messages else None
        if (last and last["role"] == "user"
                and isinstance(last["content"], list)
                and all(b.get("type") == "tool_result" for b in last["content"])):
            last["content"].append(block)
        else:
            self.messages.append({"role": "user", "content": [block]})
```

`add_tool_result` 里有一条 Anthropic 的格式要求：同一步骤里的多个工具调用结果，必须合并在**同一条** `user` 消息里。代码检查最后一条消息是否已经是 tool\_result 类型的 user 消息，是就追加，否则新建一条。

![1779880451253-ec5828e7-06b2-49cc-b9c0-9c2bd257824a.png](./img/8MR3iZV3po0gVP6S/1779880451253-ec5828e7-06b2-49cc-b9c0-9c2bd257824a-301749.png)

### AgentLoop

准备工作做完了，`AgentLoop.run()` 正式开始循环：

```python
# core/loop.py

async def run(self, context: ExecutionContext) -> None:
    while not context.is_done():
        context.step += 1
        await self._bus.publish(StepStartedEvent(...))

        # ── plan：让 LLM 思考下一步 ──────────────────────────
        try:
            response = await self._provider.chat(
                messages=context.messages,
                tool_schemas=self._registry.tool_schemas(),
                bus=self._bus,
                run_id=context.run_id,
            )
        except asyncio.CancelledError:
            context.mark_failed("cancelled")
            raise   # 必须向上传播，见下方说明
        except Exception:
            context.mark_failed("llm_error")
            break

        # ── observe：把 LLM 响应追加到对话历史 ───────────────
        blocks = []
        if response.text:
            blocks.append({"type": "text", "text": response.text})
        for tc in response.tool_calls:
            blocks.append({"type": "tool_use", "id": tc.id, "name": tc.name, "input": tc.input})
        context.add_assistant_message(blocks)

        # ── act：如果 LLM 要求调用工具，执行它 ──────────────
        if response.stop_reason == "tool_use":
            for tc in response.tool_calls:
                result = await invoke_tool(self._registry, tc, self._bus, context.run_id)
                context.add_tool_result(tc.id, result.content, is_error=result.is_error)

        # ── 终止检查 ──────────────────────────────────────────
        if response.stop_reason == "end_turn":
            context.mark_success()
        elif context.step >= context.max_steps:
            context.mark_failed("exceeded_max_steps")

        await self._bus.publish(StepFinishedEvent(...))
```

![1779880450661-74fe78ad-2d06-4a34-9859-347e9aeac915.png](./img/8MR3iZV3po0gVP6S/1779880450661-74fe78ad-2d06-4a34-9859-347e9aeac915-686425.png)

**为什么顺序是 observe → act，而不是 act → observe？**

Anthropic API 的消息格式有严格要求：assistant 的回复必须先出现在历史里，tool result 作为下一条 user 消息紧随其后。如果先执行工具（act）再记录 LLM 响应（observe），消息顺序就乱了，下一次调用 API 会报错。

**工具调用失败了，循环停不停？**

不停。`invoke_tool()` 的契约是永不抛出异常——无论工具执行成功还是失败，都返回一个 `ToolResult`，`is_error` 字段说明是否出错。失败的结果同样通过 `add_tool_result()` 追加进对话历史，让 LLM 看到"这个工具出错了"，然后自己决定怎么办（换路径、报告给用户、还是放弃）。这是 agent 和普通脚本最本质的区别：agent 能从错误中恢复。

`CancelledError`\*\* 为什么必须 re-raise？\*\*

当用户按 Ctrl+C，asyncio 会向正在运行的协程发送 `CancelledError`。如果我们在 `except` 里吞掉它而不 `raise`，asyncio 不知道取消发生了，程序就无法正常退出。我们捕获它唯一的目的是在 re-raise 之前有机会更新 `context.status`，让 `RunFinishedEvent` 能记录正确的终止原因。

循环的终止条件汇总：

| 触发条件 | 结果 |
| --- | --- |
| LLM 返回 `end_turn` | `success` |
| 步数达到 `max_steps`（默认 20） | `failed: exceeded_max_steps` |
| LLM API 报错 | `failed: llm_error` |
| Ctrl+C | `failed: cancelled` |
| 工具执行出错 | **不终止**，错误作为结果送回 LLM |

### AnthropicProvider——和 LLM 对话

每次 `plan` 阶段，`AgentLoop` 调用 `provider.chat()`，把当前对话历史和工具 schema 传进去，拿回 LLM 的响应。

```python
# core/llm/provider.py（节选）

async def chat(self, messages, tool_schemas, bus, run_id) -> LlmResponse:
    # 告诉监听者用了哪个模型
    await bus.publish(LlmModelSelectedEvent(run_id=run_id, model=self._model, strategy="static", ts=_now()))

    # system prompt：告诉 LLM 它是谁、能做什么
    system = [{"type": "text", "text": _SYSTEM_PROMPT, "cache_control": {"type": "ephemeral"}}]

    # 流式调用
    async with self._client.messages.stream(**kwargs) as stream:
        async for text in stream.text_stream:
            await bus.publish(LlmTokenEvent(run_id=run_id, token=text, ts=_now()))
            text_parts.append(text)
        final_message = await stream.get_final_message()
```

"流式调用"的意思是：LLM 生成文本时，不等全部生成完再返回，而是每生成一个片段（token）就立刻发给我们。我们把每个 token 发布为 `LlmTokenEvent`，`StdoutPrinter` 收到后立即打印，这就是终端上看到文字一个个出现的原因。

`cache_control`\*\* 是什么？\*\*

```python
system = [{"type": "text", "text": _SYSTEM_PROMPT, "cache_control": {"type": "ephemeral"}}]

# tool schema 列表最后一项也加上
last = dict(tools[-1])
last["cache_control"] = {"type": "ephemeral"}
```

Anthropic 的 prompt caching 功能：如果相邻两次 API 调用的 system prompt 内容完全相同，第二次可以直接从缓存里取，不用重新处理，消耗的 token 数大幅减少（缓存命中的 token 只收原价的 10%）。

一次 run 里，system prompt 和工具 schema 从头到尾不变，加上 `cache_control` 标记后，从第二步开始每次 LLM 调用都能命中缓存。多步任务里这个节省非常可观。

> 💡 `cache_control`\*\* 只加在列表最后一项\*\*
>
> Anthropic 的 prompt caching 基于前缀匹配——从列表开头扫描，直到遇到带 `cache_control` 的项为止，把这个前缀作为缓存 key。所以标记加在哪个位置，决定了缓存的边界。我们把整个 system prompt（或工具列表）当作一个整体来缓存，所以只需要在最后一项加标记。

流结束后，从 `final_message` 里提取工具调用信息：

```python
    tool_calls = []
    for block in final_message.content:
        if block.type == "tool_use":
            tool_calls.append(
                ToolCallBlock(id=block.id, name=block.name, input=dict(block.input))
            )

    return LlmResponse(
        stop_reason=final_message.stop_reason or "end_turn",
        tool_calls=tool_calls,
        text="".join(text_parts),
    )
```

`stop_reason` 有两个我们关心的值：`"end_turn"`（LLM 认为任务完成）和 `"tool_use"`（LLM 要调用工具）。其他情况（如 `"max_tokens"`）当作 `"end_turn"` 处理。

**为什么用原生 tool\_use，不让 LLM 在文本里描述它想调什么？**

原生 tool\_use 给了我们三样东西：结构化的 JSON 参数（不用自己写解析器）、`tool_use_id`（可以精确追踪每次调用）、server-side 参数校验（格式不对 API 直接报错，而不是运行时出问题）。文本解析容易出错，且随 LLM 版本变化。

### 工具系统——执行 LLM 的请求

`AgentLoop` 拿到 LLM 的 `tool_calls` 列表之后，对每一个调用 `invoke_tool()`。工具系统由三个部分组成。

`BaseTool`**：所有工具的模板**

```python
# core/tools/base.py

@dataclass
class ToolResult:
    content: str              # 工具的输出内容
    is_error: bool = False    # True 表示这是一个错误结果
    error_type: str | None = None

class BaseTool(ABC):
    name: str                 # 工具名，LLM 用这个名字来请求调用
    description: str          # 工具描述，LLM 根据这个判断要不要用这个工具
    input_schema: dict[str, object]   # 参数格式定义

    @abstractmethod
    async def invoke(self, params: dict[str, object]) -> ToolResult: ...
```

`ToolResult` 的 `is_error` 标志很关键：工具失败不抛异常，而是返回带错误标记的结果。这样调用方的代码不需要 try/except，直接把结果追加到上下文，循环继续。

`ToolRegistry`**：工具的注册表**

```python
# core/tools/registry.py

def tool_schemas(self) -> list[dict[str, object]]:
    return [
        {"name": tool.name, "description": tool.description, "input_schema": tool.input_schema}
        for tool in self._tools.values()
    ]
```

`tool_schemas()` 的输出就是 Anthropic API `tools` 参数的格式——直接传给 `AnthropicProvider`，不需要任何转换。LLM 靠这个 schema 知道有哪些工具可以用、每个工具需要什么参数。

`invoke_tool()`**：安全地调用工具**

```python
# core/tools/invocation.py（节选）

async def invoke_tool(registry, tool_call, bus, run_id, timeout=10.0) -> ToolResult:
    await bus.publish(ToolCallStartedEvent(...))

    # 查找工具
    tool = registry.get(tool_call.name)
    if tool is None:
        return await _fail(bus, ..., "runtime_error", f"unknown tool: {tool_call.name}")

    # 检查必填参数
    required = cast(list[str], tool.input_schema.get("required", []))
    missing = [p for p in required if p not in tool_call.input]
    if missing:
        return await _fail(bus, ..., "schema_error", f"missing required parameters: {', '.join(missing)}")

    # 执行工具，最多等 10 秒
    try:
        result = await asyncio.wait_for(tool.invoke(dict(tool_call.input)), timeout=timeout)
        await bus.publish(ToolCallFinishedEvent(...))
        return result
    except TimeoutError:
        return await _fail(bus, ..., "timeout", f"tool timed out after {timeout}s")
    except Exception as exc:
        return await _fail(bus, ..., "runtime_error", str(exc))
```

`asyncio.wait_for(coro, timeout=10.0)` 是 asyncio 提供的超时机制：如果 `tool.invoke()` 超过 10 秒还没返回，直接取消并抛出 `TimeoutError`。我们捕获它，通过 `_fail()` 转成带错误标记的 `ToolResult`。

所有错误路径——工具不存在、参数缺失、超时、运行时崩溃——都走 `_fail()` 辅助函数，它做两件事：发布 `ToolCallFailedEvent`（让 EventBus 的订阅者知道失败了），返回 `ToolResult(is_error=True)`。

**s1 的内建工具：**`ReadFileTool`

```python
# core/tools/builtin/read_file.py

_MAX_BYTES = 512 * 1024   # 512 KB

async def invoke(self, params: dict[str, object]) -> ToolResult:
    path_str = str(params["path"])

    if ".." in Path(path_str).parts:
        raise PermissionError(f"path traversal not allowed: {path_str}")

    raw = Path(path_str).read_bytes()
    truncated = len(raw) > _MAX_BYTES
    text = raw[:_MAX_BYTES].decode("utf-8", errors="replace")
    if truncated:
        text += "\n[truncated]"
    return ToolResult(content=text)
```

三个安全边界：路径不能包含 `..`（防止读取上层目录之外的文件）；文件超过 512KB 时截断而不是报错（追加 `[truncated]` 让 LLM 知道内容不完整）；路径相对于当前工作目录。

这个工具抛出 `PermissionError` 后会被 `invoke_tool()` 的 `except Exception` 捕获并转成错误结果，流程照常继续。

![1779880450730-a9da06ed-d02e-47d7-8177-ad042b798478.png](./img/8MR3iZV3po0gVP6S/1779880450730-a9da06ed-d02e-47d7-8177-ad042b798478-424936.png)

### 回到 AgentRunner：收尾工作

`loop.run()` 返回之后（正常结束或被中断），`AgentRunner` 负责收尾：

```python
        cancelled = False
        try:
            await loop.run(context)
        except asyncio.CancelledError:
            cancelled = True
            if not context.is_done():
                context.mark_failed("cancelled")

        await bus.publish(RunFinishedEvent(
            run_id=run_id,
            status=context.status,
            reason=context.reason,
            steps=context.step,
            ts=_now(),
        ))

    if cancelled:   # EventWriter 的 async with 已经结束（文件已关闭），现在才能 re-raise
        raise asyncio.CancelledError()
```

`RunFinishedEvent` 在所有情况下都会发布：正常完成、超步数失败、Ctrl+C 取消。保证 `events.jsonl` 里永远有头有尾，不会出现"有 `run.started` 但找不到 `run.finished`"的幽灵 run。

re-raise `CancelledError` 放在 `async with EventWriter` 块的外面，是因为要等文件关闭之后再抛出，防止文件处于打开状态时进程被终止。

### StdoutPrinter：终端的实时输出

`StdoutPrinter` 订阅 EventBus，把它关心的事件格式化打印出来：

```python
# cli/commands/run.py（节选）

class StdoutPrinter:
    def __init__(self) -> None:
        self._inline = False   # 记录当前是否在 LLM token 流式输出的中途

    def _ensure_newline(self) -> None:
        if self._inline:
            print()            # 补一个换行
            self._inline = False

    async def handle(self, event: BaseModel) -> None:
        if isinstance(event, LlmTokenEvent):
            print(event.token, end="", flush=True)
            self._inline = True   # 还没换行，记录下来

        elif isinstance(event, ToolCallStartedEvent):
            self._ensure_newline()   # LLM 流式输出可能没有换行，先补上
            print(f"[tool] {event.tool_name} {json.dumps(event.params, ensure_ascii=False)}")

        elif isinstance(event, RunFinishedEvent):
            self._ensure_newline()
            elapsed = time.monotonic() - self._run_start
            print(f"[run] {event.status}  {event.steps} steps  {elapsed:.1f}s")
```

`_inline` 标志处理一个细节：LLM 流式输出时，每个 token 用 `print(token, end="")` 打印，不换行。如果这时候出现了工具调用，需要先打一个换行，再打 `[tool] read_file ...`，否则两段输出会挤在同一行。`_ensure_newline()` 负责做这件事。

***

## 验证

### 手动运行

```bash
uv run kama run --goal "总结 README.md 的主要章节"
```

运行完看看 `events.jsonl`：

```bash
cat runs/$(ls -t runs | head -1)/events.jsonl | python -m json.tool --no-ensure-ascii
```

如果第二步的 `llm.usage` 里 `cache_read_input_tokens > 0`，说明 prompt caching 在正常工作。

***

## 小结与展望

这个阶段让 agent 第一次能做事了。从用户输入 `kama run --goal` 到 `events.jsonl` 落盘，整条链路跑通，每一步都有事件记录，可以回溯。

**s2 做什么**

`AgentRunner` 现在跑在 CLI 进程里。s2 要把它搬进 `kama-core` 守护进程，`kama run` 变成客户端，通过 socket 发指令、订阅事件流。EventBus 会多一个订阅者——把事件序列化发到 socket 的 handler。现有的所有模块接口不需要动，s1 建好的地基直接复用。


> 更新: 2026-05-27 19:17:33  
> 原文: <https://www.yuque.com/chengxuyuancarl/nvd81p/mkz2qcrkrfa9y0gc>