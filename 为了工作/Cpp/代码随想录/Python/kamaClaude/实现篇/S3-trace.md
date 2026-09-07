# S3-trace

# Trace：让 daemon 的数据流从黑盒变成时间线

| **阶段** | trace |
| --- | --- |
| **分支** | `stage/s3` |
| **本阶段新增** | TraceRecord、TraceWriter、TracingProvider、IPC / EventBus / LLM 三层埋点、`kama trace` CLI |
| **依赖上一阶段** | s2 的 SocketServer、IpcEventBroadcaster、EventBus、AnthropicProvider |

## 本阶段要做什么

s2 结束时，我们已经能把任务跑起来，而且两个终端能同时看到同一份事件流。`runs/<run_id>/events.jsonl` 里有完整的步骤序列：哪步开始了，哪个工具被调用，LLM 用了多少 token。

但这只是**一个视角**——EventBus 内部的视角。

假设你运行了一次任务，得到了错误的结果。你想搞清楚到底哪里出问题：

> * LLM 真的收到了工具调用的结果吗？`messages` 参数里有没有正确的 `tool_result`？
> * LLM 的 `stop_reason` 是 `tool_use` 还是 `end_turn`？它是主动结束还是被截断了？
> * 客户端发来的 `agent.run` 命令，参数有没有被正确解析？还是在 IPC 层就已经出了问题？

`events.jsonl` 回答不了这三个问题。它只记录了 EventBus 发布的高层事件，不包含 IPC 层的 JSON-RPC 原始帧，也不包含发给 Anthropic API 的完整 `messages` 数组和收到的原始响应。

这个阶段要在系统里埋四个观察点，把三个层面的数据流统一汇入一个文件，按时间顺序排成一条时间线：客户端发来什么命令，守护进程回了什么，EventBus 发布了什么事件，LLM API 收到和返回了什么——全部可见，全部有时间戳，全部可以用命令行过滤查看。

***

## 设计：系统级统一时间线

### 一个文件，五个方向

整个系统的数据流可以用五个方向描述完：

| 方向 | 含义 | 所在层 |
| --- | --- | --- |
| `CLIENT→CORE` | 客户端发来的 JSON-RPC 命令 | ipc |
| `CORE→CLIENT` | 守护进程回复的响应或主动推送的事件 | ipc |
| `CORE` | EventBus 内部发布的事件 | event |
| `CORE→LLM` | 发出 Anthropic API 请求 | llm |
| `LLM→CORE` | 收到 Anthropic API 响应 | llm |

这五个方向完整覆盖了 daemon 进程边界内外的所有 I/O。每条记录都有时间戳，按时间顺序追加到 `~/.kama/traces/daemon.jsonl`，整个文件就是系统行为的完整时间线。

![1779882977296-fe4ff2e0-1e47-4ada-8375-5b0cfc7f94c3.png](./img/JbpbBzB7pxnfjXg0/1779882977296-fe4ff2e0-1e47-4ada-8375-5b0cfc7f94c3-820779.png)

### 为什么是一个文件，而不是每个 run 一个文件

看起来把 trace 记录拆到 `runs/<run_id>/trace.jsonl` 更整洁——每次 run 自己管自己的 trace。

但有一个问题：`CLIENT→CORE` 命令在被解析成功之前，`run_id` 还不存在。客户端发来 `agent.run`，守护进程解析出命令、生成 `run_id`、启动 AgentRunner——这三件事有先后顺序，第一条 trace 记录在第三件事之前就必须写出去。

更大的问题是 IPC 命令本身：`core.ping`、`event.subscribe` 这类命令根本就没有关联的 `run_id`，它们是全局性的守护进程行为，不属于任何一次 run。

所以存储的选择很明确：单一的 `daemon.jsonl`，daemon 整个生命周期的全局时间线，`run_id` 是可选字段——有就记，没有就留空。

> 💡 这个决策和 `events.jsonl` 形成了互补：`events.jsonl` 是 per-run 的深度档案，适合分析"这次 run 里 agent 做了什么"；`daemon.jsonl` 是跨层的系统时间线，适合调试"这条命令是怎么一路流转的"。两个文件各有用途，不冲突。

### 非阻塞写入：队列 + drain task

`emit()` 是在 EventBus 回调里被调用的，这个回调在 daemon 的主 asyncio 事件循环里运行。如果 `emit()` 直接调用文件 I/O，哪怕是极短的阻塞（几毫秒），也会卡住事件循环，让所有在等待的协程一起暂停。

解决方案：`emit()` 只把记录放进一个内存队列（`asyncio.Queue.put_nowait`），立即返回；一个独立的 drain task 持续从队列里取出记录，追加写入文件。主事件循环从不等文件 I/O。

```plain
emit() ─→ asyncio.Queue ─→ _drain() ─→ 追加写 daemon.jsonl
           （非阻塞，立即返回）        （独立 task，异步执行）
```

> ⚠️ 这个设计有一个后果：如果 daemon 突然崩溃（SIGKILL），队列里还没来得及写出的记录会丢失。`SIGTERM` 不会丢，因为 `CoreApp.run()` 在关闭时会调用 `trace.stop()`，它等待队列清空再退出。课程项目里这个权衡是合理的——调试时不会 SIGKILL 进程。

***

## 实现

### TraceRecord：时间线里的每一行

每条 trace 记录都是一个 `TraceRecord`：

```python
# core/trace/record.py

class TraceRecord(BaseModel):
    ts: str
    direction: Literal[
        "CLIENT→CORE", "CORE→CLIENT", "CORE", "CORE→LLM", "LLM→CORE"
    ]
    layer: Literal["ipc", "event", "llm"]
    kind: str
    run_id: str | None = None
    step: int | None = None
    client_id: str | None = None
    data: dict[str, Any]
```

`direction` 和 `layer` 的组合决定了这条记录的含义：`layer` 告诉你是哪个子系统，`direction` 告诉你数据在往哪里流。`kind` 是更细的分类：同样是 `CORE→CLIENT` 方向，`response` 是命令的 JSON-RPC 回复，`push` 是 broadcaster 主动推出去的事件推送，两者需要区分。

`data` 是开放的 `dict`——不同埋点塞进去的内容不同，IPC 层放原始帧结构，LLM 层放消息数量和 token 统计，不强制统一 schema，保持灵活。

### TraceWriter：队列 + 后台写入

```python
# core/trace/writer.py

class TraceWriter:
    def __init__(self, path: Path) -> None:
        self._path = path
        self._queue: asyncio.Queue[TraceRecord] = asyncio.Queue()
        self._task: asyncio.Task[None] | None = None

    async def start(self) -> None:
        self._path.parent.mkdir(parents=True, exist_ok=True)
        self._task = asyncio.create_task(self._drain())

    async def stop(self) -> None:
        await self._queue.join()    # 等队列里的记录全部写完
        if self._task is not None:
            self._task.cancel()
            try:
                await self._task
            except asyncio.CancelledError:
                pass

    def emit(self, record: TraceRecord) -> None:
        self._queue.put_nowait(record)    # 同步，不阻塞

    async def _drain(self) -> None:
        with open(self._path, "a") as f:
            while True:
                record = await self._queue.get()
                try:
                    f.write(record.model_dump_json() + "\n")
                    f.flush()
                finally:
                    self._queue.task_done()
```

`stop()` 先 `await self._queue.join()` 再取消 drain task，而不是直接取消。这确保了 daemon 退出时队列里的最后几条记录都能落盘——`task_done()` 在 `finally` 块里调用，即使 `f.write()` 抛异常也会执行，`join()` 不会永久阻塞。

### 埋点 ①：IPC 层——SocketServer

`SocketServer` 在两个地方埋点：收到命令时，以及发出响应时。

**收到命令**（`_handle_line`，解析成功之后）：

```python
# core/transport/socket_server.py（节选）

if self._trace is not None:
    client_id = str(writer.get_extra_info("peername", "<unknown>"))
    self._trace.emit(
        TraceRecord(
            ts=_now(),
            direction="CLIENT→CORE",
            layer="ipc",
            kind="command",
            client_id=client_id,
            data={"method": req.method, "id": req.id, "params": req.params},
        )
    )
```

埋点在 `JsonRpcRequest.model_validate(raw)` 成功之后——只记录合法的请求，解析失败的直接发错误响应，不写 trace（它们不属于"命令"，属于客户端 bug）。

**发出响应**（`_send`，`drain()` 之后）：

```python
async def _send(self, writer: asyncio.StreamWriter, msg: BaseModel) -> None:
    writer.write(msg.model_dump_json().encode() + b"\n")
    await writer.drain()
    if self._trace is not None:
        kind = "error" if isinstance(msg, JsonRpcError) else "response"
        ...
        self._trace.emit(TraceRecord(direction="CORE→CLIENT", kind=kind, ...))
```

注意顺序：先 `drain()` 成功，再写 trace。这保证了 trace 里出现 `CORE→CLIENT response` 就意味着客户端确实收到了响应，不是"即将发送"。

### 埋点 ②：IPC 层——IpcEventBroadcaster

broadcaster 每次成功推送事件之后，写一条 `push` 记录：

```python
# core/transport/ipc_broadcaster.py（节选）

sub.writer.write(envelope.model_dump_json().encode() + b"\n")
await sub.writer.drain()
if self._trace is not None:
    self._trace.emit(
        TraceRecord(
            direction="CORE→CLIENT",
            layer="ipc",
            kind="push",
            run_id=run_id,
            client_id=client_id,
            data={"sub_id": sub.sub_id, "event_type": event_type},
        )
    )
```

`push` 记录里**不包含完整的 event body**，只记录 `sub_id` 和 `event_type`。原因：完整 event 已经在 `CORE event` 记录里了（埋点 ③）；如果 push 也完整记录，同一个 event 有几个订阅者就会在 trace 文件里出现几遍，文件体积翻几倍，查阅时也多余。`push` 记录的意义是"谁（sub\_id/client\_id）收到了这个事件类型"，摘要已经足够。

### 埋点 ③：EventBus 层——CoreApp 订阅者

`CoreApp.run()` 里，trace 作为 EventBus 的普通订阅者挂上去，和 `EventWriter`、`IpcEventBroadcaster` 并列：

```python
# core/app.py（节选）

async def _trace_event_handler(self, event: BaseModel) -> None:
    assert self._trace is not None
    event_dict = event.model_dump()
    self._trace.emit(
        TraceRecord(
            ts=_now(),
            direction="CORE",
            layer="event",
            kind="event",
            run_id=event_dict.get("run_id"),
            data=event_dict,
        )
    )

# run() 里：
self._bus.subscribe(self._trace_event_handler)
```

EventBus 本身不需要改动——trace 只是又一个订阅者。这个设计让 EventBus 保持对 trace 系统的完全无感知，也让 `_trace_event_handler` 可以单独测试和替换，不影响其他订阅者。

### 埋点 ④：LLM 层——TracingProvider

LLM 层的 trace 用了一个不同的思路：不是在 `AnthropicProvider` 里埋点，而是在它外面套一层。

`TracingProvider` 实现了和 `AnthropicProvider` 完全相同的 `LLMProvider` 接口，内部持有一个 `inner` provider 的引用：

```python
# core/trace/provider.py（节选）

class TracingProvider:
    def __init__(self, inner: LLMProvider, trace: TraceWriter, *, include_payload: bool) -> None:
        self._inner = inner
        self._trace = trace
        self._include_payload = include_payload

    async def chat(self, messages, tool_schemas, bus, run_id, *, step=0) -> LlmResponse:
        # 调用前：记录 CORE→LLM
        if self._include_payload:
            call_data = {"messages": messages, "tool_schemas": tool_schemas}
        else:
            call_data = {"message_count": len(messages), "tool_count": len(tool_schemas)}

        self._trace.emit(TraceRecord(
            direction="CORE→LLM", layer="llm", kind="api_call",
            run_id=run_id, step=step, data=call_data,
        ))

        t0 = time.monotonic()
        result = await self._inner.chat(messages, tool_schemas, bus, run_id, step=step)
        latency_ms = int((time.monotonic() - t0) * 1000)

        # 调用后：记录 LLM→CORE
        ...
        self._trace.emit(TraceRecord(
            direction="LLM→CORE", layer="llm", kind="api_response",
            run_id=run_id, step=step, data=resp_data,
        ))
        return result
```

> 💡 **为什么用 wrapper 而不是直接在 AnthropicProvider 里加？** `AnthropicProvider` 是一个具体实现，它应该只关心"怎么调 Anthropic API"。如果 trace 逻辑写进去，以后想换一个 provider，或者想关掉 trace，就得改两个地方。Wrapper 模式让两者完全解耦：trace 禁用时，`AgentRunner` 直接用 `AnthropicProvider`；启用时，在外面套一个 `TracingProvider`，`AnthropicProvider` 本身一行代码都不改。

`include_payload` 控制是否记录完整的 `messages` 数组和响应体。默认开启——调试时能看到 LLM 实际收到的完整 prompt 非常有价值，能直接看出是提示词有问题还是工具结果传递有问题。生产场景（message 可能包含敏感信息，或者 trace 文件体积需要控制）可以关掉，只记录摘要。

`AgentRunner` 在收到非 None 的 `trace` 时，用 `TracingProvider` 包裹真实 provider：

```python
# core/runner.py（节选）

if self._trace is not None:
    provider = TracingProvider(
        provider,
        self._trace,
        include_payload=self._config.trace.include_llm_payload,
    )
```

同时，为了让 TracingProvider 能把步骤编号记录进 `LLM→CORE` 记录，`AgentLoop` 在调 `provider.chat()` 时需要把当前步骤传进去。原来的调用：

```python
response = await self._provider.chat(messages=context.messages, ...)
```

改成：

```python
response = await self._provider.chat(messages=context.messages, ..., step=context.step)
```

`LLMProvider` 协议和 `AnthropicProvider` 的签名同步加上 `*, step: int = 0` 关键字参数。`AnthropicProvider` 不使用这个参数（它不需要知道步骤编号），只是接受它以满足协议约定。

### 配置

`config.toml` 里的 `[trace]` 小节：

```toml
[trace]
enabled = true
file = "~/.kama/traces/daemon.jsonl"
include_llm_payload = true
```

对应的环境变量：`KAMA_TRACE_ENABLED`、`KAMA_TRACE_FILE`、`KAMA_TRACE_INCLUDE_LLM_PAYLOAD`。

### CoreApp 串联

`CoreApp.run()` 是把所有组件接在一起的地方：

```python
# core/app.py（run() 节选）

if self._config.trace.enabled:
    trace_path = Path(self._config.trace.file).expanduser()
    self._trace = TraceWriter(trace_path)
    await self._trace.start()
    self._bus.subscribe(self._trace_event_handler)   # 埋点 ③

self._broadcaster = IpcEventBroadcaster(trace=self._trace)   # 埋点 ②

server = SocketServer(
    self._config.host, self._config.port,
    self._broadcaster,
    trace=self._trace,   # 埋点 ①
)
```

runner 创建时注入 trace：

```python
runner = AgentRunner(self._config, bus=self._bus, trace=self._trace)   # 埋点 ④
```

关闭时等队列清空：

```python
await server.stop()
if self._trace is not None:
    await self._trace.stop()
```

三个埋点（IPC 层两处 + EventBus 层）通过构造函数注入 `TraceWriter`；LLM 层埋点通过 `AgentRunner` 的 `trace` 参数在 runner 内部包装。所有组件都收到同一个 `TraceWriter` 实例，写入的是同一个文件，时间顺序由各 `emit()` 调用时的挂钟时间决定。

### `kama trace`：查看时间线

`kama trace` 命令从 `daemon.jsonl` 读取记录，彩色输出，可以按 run\_id、layer、direction 过滤：

```bash
# 查看所有记录
uv run kama trace

# 只看 LLM 层（发出的请求和收到的响应）
uv run kama trace --layer llm

# 只看某次 run 的记录
uv run kama trace run-20260516-abc123

# 实时跟踪（像 tail -f）
uv run kama trace --follow

# 输出原始 NDJSON，供 jq 处理
uv run kama trace --raw | jq 'select(.direction == "LLM→CORE")'
```

输出格式（按 direction 着色，摘要折叠大字段）：

```plain
10:00:00.001  CLIENT→CORE    command        method=agent.run  goal="总结 README.md 的主要章节"
10:00:00.003  CORE           event          type=run.started
10:00:00.004  CORE→CLIENT    response       run_id=20260516
10:00:00.005  CORE→CLIENT    push           event=run.started  sub=sub-a1b2c3
10:00:00.009  CORE→LLM       api_call       msgs=3  tools=1
10:00:00.851  LLM→CORE       api_response   stop=tool_use  latency=842ms  out_tokens=47
10:00:00.852  CORE           event          type=tool.call_started
10:00:00.856  CORE           event          type=tool.call_finished
10:00:00.857  CORE→LLM       api_call       msgs=5  tools=1
10:00:01.623  LLM→CORE       api_response   stop=end_turn  latency=766ms  out_tokens=89
10:00:01.624  CORE           event          type=run.finished
10:00:01.625  CORE→CLIENT    push           event=run.finished  sub=sub-a1b2c3
```

`_summarize()` 为每种 kind 定义了不同的摘要逻辑：`command` 记录展示 `method` 和 `goal`，`api_call` 记录展示消息数和工具数，`api_response` 记录展示 `stop_reason`、延迟和 `output_tokens`。大字段（完整的 `messages` 数组）只出现在 `--raw` 模式的原始输出里，命令行浏览时不会被淹没。

***

## 验证

### 手动验证

```bash
# 终端 A：启动 daemon
uv run kama-core

# 终端 B：跑一次任务
uv run kama run --goal "用一句话介绍你自己"

# 终端 C：查看时间线
uv run kama trace
```

核心断言——时间线里应该能看到五种 direction 的记录，按时间顺序依次出现：

```plain
CLIENT→CORE   command        method=event.subscribe
CORE→CLIENT   response       ...
CLIENT→CORE   command        method=agent.run
CORE          event          type=run.started
CORE→CLIENT   response       run_id=...
CORE→CLIENT   push           event=run.started
CORE→LLM      api_call       msgs=3  tools=1
LLM→CORE      api_response   stop=end_turn  latency=...ms
CORE          event          type=run.finished
CORE→CLIENT   push           event=run.finished
```

如果看到 `CORE→LLM api_call` 之后紧跟 `LLM→CORE api_response(stop=tool_use)`，说明 LLM 请求了工具调用——能和 events.jsonl 里的 `tool.call_started` 对应上，两个文件互相印证。

### 验证 include\_payload

```bash
# 在 config.toml 里设置 include_llm_payload = true，跑一次任务后：
uv run kama trace --raw | jq 'select(.kind == "api_call") | .data.messages | length'
# 应该输出消息数量（如 3），而不是 null
```

***

## 小结与展望

这个阶段没有改变系统的任何行为，只是在四个位置加了观察窗口。现在你能看到的东西，和 s2 结束时完全不同：

* `events.jsonl` 告诉你 agent **做了什么**（步骤、工具调用、结果）
* `daemon.jsonl` 告诉你**数据怎么流动的**（从哪里来、经过哪些层、耗了多少时间）

两个文件用 `run_id` 关联，一起使用时能建立完整的系统画像。

当前的 trace 系统有几个已知限制：

* **不轮转**：`daemon.jsonl`\*\* 会无限增长\*\*。这在 s3 的配置里预留了 `max_size_mb` 和 `keep_files` 字段，但实现推迟到功能稳定后再补。
* **没有结构化索引**：`kama trace --follow` 是文件末尾轮询，高频场景下有 50ms 延迟；`kama trace <run_id>` 每次都全文扫描。

下一阶段（s3 主线）会引入任务状态机，`task.state_changed` 和 `task.tree_updated` 事件将出现在 `daemon.jsonl` 里的 `CORE event` 记录中——有了 trace 基础设施，调试任务调度问题时可以直接从时间线里看出"哪条命令触发了状态迁移、状态迁移之后发生了什么"，而不用在日志里靠时间戳拼凑。


> 更新: 2026-05-27 19:59:40  
> 原文: <https://www.yuque.com/chengxuyuancarl/nvd81p/sgh4zwb44g6yox2z>