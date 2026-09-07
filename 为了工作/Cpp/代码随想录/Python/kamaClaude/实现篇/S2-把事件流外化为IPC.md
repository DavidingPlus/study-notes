# S2-把事件流外化为 IPC

# 第 2 阶段：把事件流外化为 IPC

| **阶段** | s2 |
| --- | --- |
| **分支** | `stage/s2` |
| **本阶段新增** | daemon 执行 agent.run、IPC 事件推送、事件订阅与回放、SocketClient、TUI 雏形、daemon 生命周期管理 |
| **依赖上一阶段** | s1 的 AgentRunner、AgentLoop、EventBus、EventWriter |

## 本阶段要做什么

s1 结束时，`kama run --goal "..."` 已经能跑通：agent 会调用 LLM、执行工具、把事件写进 `events.jsonl`，CLI 也能实时打印过程。但这条链路有一个结构性问题：它仍然发生在 CLI 进程里。

也就是说，s1 的 `EventBus` 是进程内对象，`AgentRunner` 也是由 CLI 直接创建。你打开两个终端，想让一个 TUI 观察、另一个 CLI 触发任务，表面上 daemon 已经存在：

```bash
# 终端 A
uv run kama-core
```

再触发一次 run：

```bash
# 终端 B
uv run kama run --goal "总结 README.md 的主要章节"
```

任务完成，结果输出在终端 B。但守护进程那边没有任何反应，因为这次 run 根本没经过 daemon。

原因很直接：s1 的 `kama run` 在**它自己的进程**里直接跑 `AgentRunner`，任务跑完进程就退出，守护进程从头到尾只是个旁观者：

```plain
s1 的实际结构：

kama-core（守护进程）  ← 只能响应 core.ping，与 agent 无关

kama run（CLI 进程）
  └─ AgentRunner.run()  ← agent 在这里跑完就消失
```

s2 要修正的根本问题不是“增加一个 TUI”，也不只是“把 `AgentRunner.run()` 换个地方调用”。真正的目标是：**把 s1 的进程内事件总线外化为 IPC，让 Core daemon 成为唯一的 agent 执行体，CLI 和 TUI 都通过同一份协议消费同一份实时事件流**。

这会解决三个具体限制：

* 多客户端可以同时消费同一份 run 事件，CLI 和 TUI 不再各跑各的。
* S0 建好的 daemon、SocketServer 和 JSON-RPC 协议开始承载真正的 agent 命令，而不是只响应 `core.ping`。
* 客户端断开后可以通过 `events.jsonl` 回放历史，再接续实时事件流。

所以 s2 的主线是 `kama run --goal "..."` 的双进程版：CLI 发送 `event.subscribe` 和 `agent.run`，daemon 在后台运行 AgentRunner，EventBus 把事件同时交给 EventWriter 和 IpcEventBroadcaster，所有客户端从同一条 IPC 事件流里观察进度。

![1779880678758-3a5f1292-031a-479d-935f-3b44ddc7d916.png](./img/hsPmMR-74SOHzZb5/1779880678758-3a5f1292-031a-479d-935f-3b44ddc7d916-159921.png)

***

## 执行路径概览

先把 s2 里 `kama run --goal "..."` 的完整执行路径过一遍。后面的章节会按这条路径展开。

**客户端侧（**`kama`\*\* 进程）：\*\*

1. 连接守护进程的 TCP 端口（默认 `127.0.0.1:7437`）
2. 发送 `event.subscribe` 命令：告诉守护进程想订阅哪些事件类型
3. 发送 `agent.run` 命令，附上 goal
4. 守护进程立刻回复一个 `run_id`（不等任务完成）
5. 继续监听同一条连接，接收守护进程主动推过来的事件
6. 每收到一个事件，`StdoutPrinter` 格式化打印
7. 收到 `run.finished`，关闭连接，退出

**守护进程侧（**`kama-core`\*\* 进程）：\*\*

1. 收到 `event.subscribe`，把这条连接的 TCP 写端登记到 broadcaster
2. 收到 `agent.run`，生成 `run_id`，后台启动 `AgentRunner.run()`，立即回复 `run_id`
3. `AgentRunner` 在后台运行，向全局 `EventBus` 发布事件
4. `IpcEventBroadcaster` 订阅了这个 bus，把每个事件推给所有登记的客户端

s1 的组件（AgentRunner、AgentLoop、EventBus、EventWriter）都还在。变化在边界上：`EventBus` 从单个 CLI 进程里的局部对象，变成 daemon 生命周期内的全局事件源；`IpcEventBroadcaster` 成为它的订阅者，把事件推给 TCP 客户端；CLI 和 TUI 都通过 `SocketClient` 连接进来。

***

## CLI 只负责发命令和消费事件

s1 的 `cmd_run` 直接调用 `asyncio.run(runner.run(goal))`，agent 就在这个进程里跑。s2 的 `cmd_run` 改成了通过网络触发：

```python
# cli/commands/run.py（节选）

async def _run_async(goal: str, config: KamaConfig) -> int:
    client = SocketClient(config.host, config.port)
    try:
        await client.connect()
    except (ConnectionRefusedError, OSError):
        print(f"error: core not running ({config.host}:{config.port})", file=sys.stderr)
        return 1

    printer = StdoutPrinter()
    finished = asyncio.Event()
    exit_code = 0

    async def on_event(event: dict[str, Any]) -> None:
        nonlocal exit_code
        await printer.handle(event)
        if event.get("type") == "run.finished":
            exit_code = 0 if event.get("status") == "success" else 1
            finished.set()

    client.on_event(on_event)
    loop_task = asyncio.create_task(client.run_event_loop())

    await client.send_command("event.subscribe", {
        "topics": ["run.*", "step.*", "tool.*", "llm.token", "llm.usage"],
        "scope": "global",
    })
    await client.send_command("agent.run", {"goal": goal})
    await finished.wait()
    ...
```

`AgentRunner` 消失了，换成了 `SocketClient`。`StdoutPrinter` 还在，但现在处理的是 `dict`，而不是 s1 里的 pydantic Event 对象——事件经过网络传输之后被反序列化为普通 Python 字典。

`asyncio.Event()` 是 asyncio 里的信号量：`finished.set()` 把它置位，`await finished.wait()` 会阻塞到置位为止。这里用来等 `run.finished` 事件：收到了就置位，`_run_async` 就能退出。

**顺序很关键**：`event.subscribe` 必须在 `agent.run` 之前发送。如果先发 `agent.run`，守护进程立刻开始跑，`run.started` 可能在你登记订阅之前就已经推出去了，客户端会错过。先订阅再触发，不丢第一个事件。

### SocketClient：一条连接，两种消息

`SocketClient` 是客户端侧的连接抽象。连接守护进程后，同一条 TCP 流上会出现两种消息：

* **命令响应**：JSON-RPC 2.0 格式，带 `"jsonrpc": "2.0"` 和 `"id"` 字段，对应之前发出的某个请求
* **事件推送**：带 `"kind": "event"` 字段，是守护进程主动发来的，没有 `id`

实际的流大概长这样：

```plain
# 两种消息混合出现在同一条 TCP 流里（按时间顺序）：

{"jsonrpc":"2.0","id":"req-1","result":{"subscription_id":"sub-abc","replayed_count":0}}
{"jsonrpc":"2.0","id":"req-2","result":{"run_id":"20260515-abc"}}
{"kind":"event","event":{"type":"run.started","run_id":"20260515-abc",...}}
{"kind":"event","event":{"type":"step.started","step":1,...}}
{"kind":"event","event":{"type":"llm.token","token":"I'll",...}}
{"kind":"event","event":{"type":"llm.token","token":" read",...}}
```

![1779880679412-126bf4b0-2eed-4a2e-b0e2-e249412a0b12.png](./img/hsPmMR-74SOHzZb5/1779880679412-126bf4b0-2eed-4a2e-b0e2-e249412a0b12-730710.png)

为什么放在同一条连接里，而不是单独开一条连接专门推事件？分成两条连接需要管理两个生命周期，断开时要两边同步清理，还要有机制把两条连接绑定到同一个"会话"——实现更复杂，收益不明显。

`SocketClient._dispatch()` 是解析这个混合流的路由器：

```python
# core/transport/socket_client.py（节选）

async def _dispatch(self, line: bytes) -> None:
    msg = json.loads(line)

    if "jsonrpc" in msg:
        # 命令响应：找到等待它的 Future，完成它
        req_id = msg.get("id")
        if req_id and req_id in self._pending:
            fut = self._pending.pop(req_id)
            if "error" in msg:
                err = msg["error"]
                fut.set_exception(IpcError(err["code"], err["message"]))
            else:
                fut.set_result(msg.get("result") or {})

    elif msg.get("kind") == "event":
        # 事件推送：调用所有注册的事件处理器
        event_data = msg.get("event", {})
        for handler in self._event_handlers:
            await handler(event_data)
```

`_pending` 是一个字典，键是请求 ID，值是一个 `Future`。`send_command()` 发出请求之前，创建一个空的 `Future` 存进去，然后 `await fut` 挂起等待；等 `_dispatch()` 解析到对应 ID 的响应，调用 `fut.set_result()`，挂起的地方就被唤醒，拿到结果继续执行。这是 asyncio 里"等一个还没到的值"的标准做法。

`send_command()` 和 `run_event_loop()` 在同一个事件循环里并发运行：前者在等 Future 时不阻塞，后者持续读取服务器消息；读到命令响应，`_dispatch()` 完成 Future，`send_command()` 就被唤醒。两者通过 asyncio 的调度机制交替执行。

***

## 守护进程接管 agent.run

s1 的守护进程只有一个 handler：`core.ping`。s2 在 `CoreApp` 里新增了两个：`event.subscribe` 和 `agent.run`。

`CoreApp.__init__` 里建立了守护进程的"神经中枢"：

```python
# core/app.py（节选）

class CoreApp:
    def __init__(self) -> None:
        self._bus = EventBus()
        self._broadcaster = IpcEventBroadcaster()
        self._bus.subscribe(self._broadcaster.handle)   # broadcaster 挂到总线
        self._current_run_task: asyncio.Task[None] | None = None
```

这两行接线是整个 s2 的关键：`IpcEventBroadcaster` 作为 `EventBus` 的订阅者挂上去，和 s1 的 `EventWriter` 并列。之后 `AgentRunner` 发布任何事件，`EventWriter` 写文件，`broadcaster` 推网络，两件事同时发生：

```plain
bus.publish(event)
    ├─ EventWriter.handle(event)          → 写入 events.jsonl
    └─ IpcEventBroadcaster.handle(event)  → 推给所有 TCP 客户端
```

`AgentRunner` 和 `AgentLoop` 对外面有没有客户端连着完全无感知，它们只管向 bus 发布事件。

![1779880679481-52ef9b2d-afcf-4615-9ab5-1d24ca2ecdf9.png](./img/hsPmMR-74SOHzZb5/1779880679481-52ef9b2d-afcf-4615-9ab5-1d24ca2ecdf9-441973.png)

### event.subscribe 命令

```python
# core/app.py（节选）

async def _subscribe_handler(self, params: dict[str, Any]) -> EventSubscribeResult:
    cmd = EventSubscribeCommand.model_validate(params)
    writer = get_connection_writer()   # 拿到当前连接的 TCP 写端

    replayed_count = 0
    if cmd.replay_from_run is not None:
        replayed_count = await self._replay_events(cmd.replay_from_run, writer, cmd.topics)

    sub_id = self._broadcaster.subscribe(writer, cmd.topics, cmd.scope)
    return EventSubscribeResult(subscription_id=sub_id, replayed_count=replayed_count)
```

这里有一个问题：`_subscribe_handler` 需要当前连接的 TCP `writer`，这样才能把这条连接注册到 broadcaster。但 `SocketServer` 的所有 handler 签名统一是 `async def handler(params: dict) -> Any`，没有 `writer` 参数。如果为了这一个 handler 改签名，`core.ping` 等所有 handler 都要跟着变，接口就不统一了。

解决方案是 `ContextVar`。Python asyncio 里，每个协程有自己的"上下文"，`ContextVar` 是这个上下文里的一个槽位——100 个连接同时在处理，每个协程读到的 `_writer_var` 是自己那条连接的 writer，互不干扰：

```python
# core/transport/socket_server.py（节选）

_writer_var: ContextVar[asyncio.StreamWriter] = ContextVar("_writer_var")

def get_connection_writer() -> asyncio.StreamWriter:
    return _writer_var.get()

# 在 _handle_line 里，调用 handler 之前：
_writer_var.set(writer)
result = await handler(req.params)   # handler 里调 get_connection_writer() 就能拿到 writer
```

### agent.run 命令

```python
# core/app.py（节选）

async def _agent_run_handler(self, params: dict[str, Any]) -> AgentRunResult:
    cmd = AgentRunCommand.model_validate(params)

    if self._current_run_task and not self._current_run_task.done():
        raise RuntimeError("a run is already in progress")

    run_id = new_run_id()
    runner = AgentRunner(self._config, bus=self._bus)
    self._current_run_task = asyncio.create_task(
        runner.run(cmd.goal, run_id=run_id)   # 后台运行，不等它完成
    )
    return AgentRunResult(run_id=run_id)      # 立刻返回
```

`asyncio.create_task()` 把协程"扔到后台"：当前代码不等它，继续往下跑。handler 立刻返回 `run_id` 给客户端，客户端知道任务已经开始，就可以等事件了。

`AgentRunner` 接受一个可选的 `bus` 参数——传入守护进程的全局 bus，agent 发布的事件就会流经 broadcaster，推给所有订阅客户端。不传 `bus` 时 AgentRunner 自己创建一个本地 bus，行为和 s1 一模一样，现有测试不受影响。

还有一个顺序上的细节：在 `AgentRunner.run()` 内部，`run.started` 是在初始化 LLM provider **之前**发布的：

```python
async with EventWriter(run_path / "events.jsonl") as writer:
    writer.subscribe(bus)
    await bus.publish(RunStartedEvent(...))   # 先推送给客户端

    provider = AnthropicProvider(...)         # 然后才初始化 LLM
    loop = AgentLoop(provider, registry, bus)
    await loop.run(context)
```

这样即使 LLM provider 初始化失败，客户端也已经收到了 `run.started`，而不是一直等待什么都不知道。

***

## IPC 事件广播

`IpcEventBroadcaster` 是 s2 的核心新模块。每次有客户端发 `event.subscribe`，它就记下这条连接的信息：

```python
# core/transport/ipc_broadcaster.py（节选）

@dataclass
class _Subscription:
    sub_id: str
    writer: asyncio.StreamWriter   # TCP 连接的写端
    topics: list[str]              # 如 ["run.*", "step.*", "llm.token"]
    scope: str                     # "global" 或 "run:<run_id>"
```

EventBus 发布事件时，`handle()` 被调用，遍历订阅列表，过滤 topic 和 scope，把匹配的事件推给对应客户端：

```python
# core/transport/ipc_broadcaster.py（节选）

async def handle(self, event: BaseModel) -> None:
    event_dict = event.model_dump()
    event_type = event_dict.get("type", "")
    run_id = event_dict.get("run_id")

    dead: list[asyncio.StreamWriter] = []

    for sub in list(self._subscriptions):
        if not self._matches_topic(event_type, sub.topics):
            continue
        if not self._matches_scope(run_id, sub.scope):
            continue
        try:
            envelope = EventPushEnvelope(event=event_dict)
            sub.writer.write(envelope.model_dump_json().encode() + b"\n")
            await sub.writer.drain()
        except (ConnectionResetError, BrokenPipeError, OSError):
            dead.append(sub.writer)   # 先记下，fan-out 完再清理

    for writer in dead:
        self.unsubscribe(writer)
```

Topic 过滤用 `fnmatch`——`"step.*"` 匹配 `"step.started"` 和 `"step.finished"`，`"tool.*"` 匹配所有 tool 事件。客户端只会收到自己声明感兴趣的事件类型，不相关的过滤掉不发。

Scope 过滤：`"global"` 接收所有 run 的事件，`"run:<run_id>"` 只接收特定 run 的事件。CLI 用 `"global"`，这样它触发的那次 run 的事件能收到。

```python
@staticmethod
def _matches_scope(run_id: str | None, scope: str) -> bool:
    if scope == "global":
        return True
    if scope.startswith("run:"):
        return run_id == scope[4:]
    return False
```

死连接（客户端已断开但 broadcaster 还不知道）在推送时触发 `BrokenPipeError`。处理方式是先把出问题的 writer 记进 `dead` 列表，fan-out 全部完成后再统一清理——不能在遍历 `self._subscriptions` 的过程中修改它，否则会跳过某些订阅者。

![1779880680988-569b91e7-81dc-48e8-ad9a-d0a16208fb2f.png](./img/hsPmMR-74SOHzZb5/1779880680988-569b91e7-81dc-48e8-ad9a-d0a16208fb2f-255674.png)

客户端主动断开时，`SocketServer` 在连接处理函数的 `finally` 块里调用 `broadcaster.unsubscribe(writer)`，立刻清掉这条连接的订阅，不用等到下次推送失败才发现：

```python
# core/transport/socket_server.py（节选）

async def _handle_connection(self, reader, writer):
    try:
        await self._read_loop(reader, writer)
    finally:
        if self._broadcaster is not None:
            self._broadcaster.unsubscribe(writer)   # 断开时清理订阅
        writer.close()
```

***

## 事件回放

`event.subscribe` 有一个可选参数 `replay_from_run`：

```json
{
  "topics": ["run.*", "step.*", "tool.*"],
  "scope": "global",
  "replay_from_run": "20260515-abc"
}
```

带上这个参数，守护进程在建立实时订阅之前，先从对应的 `events.jsonl` 推送历史事件：

```python
# core/app.py（节选）

async def _replay_events(self, run_id, writer, topics) -> int:
    path = events_file(run_id)   # ~/.kama/runs/<run_id>/events.jsonl
    if not path.exists():
        return 0

    count = 0
    for line in path.read_text().splitlines():
        event = json.loads(line)
        event_type = event.get("type", "")
        if not any(fnmatch.fnmatch(event_type, p) for p in topics):
            continue
        envelope = EventPushEnvelope(event=event)
        writer.write(envelope.model_dump_json().encode() + b"\n")
        count += 1

    if count:
        await writer.drain()
    return count
```

历史事件和实时事件用同样的 `EventPushEnvelope` 格式发出，客户端不需要区分——先收到一批历史，然后无缝接到实时流上。`EventSubscribeResult.replayed_count` 告诉客户端回放了多少条。

![1779880679572-e396e644-c92d-44bc-be43-e9c4ebb2fd3b.png](./img/hsPmMR-74SOHzZb5/1779880679572-e396e644-c92d-44bc-be43-e9c4ebb2fd3b-672304.png)

`kama-tui` 支持 `--replay` 参数来触发这个流程：

```bash
uv run kama-tui --replay 20260515-abc
```

***

## TUI 客户端

TUI 用 [Textual](https://textual.textualize.io/) 框架实现，底层和 CLI 一样——`SocketClient` 连接守护进程，订阅事件——只是呈现层换成了交互式终端界面。

![1779880680588-d76eefe2-af56-47ee-abf0-c85152db0070.png](./img/hsPmMR-74SOHzZb5/1779880680588-d76eefe2-af56-47ee-abf0-c85152db0070-434757.png)

布局：顶部一行状态栏，剩余空间是可滚动的富文本日志区。

```plain
┌──────────────────────────────────────────┐
│ ● connected  127.0.0.1:7437              │  ← 状态栏（1 行）
├──────────────────────────────────────────┤
│ ▶ run  20260515-abc  总结 README.md      │
│   step 1  planning...                    │  ← RichLog（剩余空间，可滚动）
│   I'll read the README to get started.   │
│   tool  read_file  {"path":"README.md"}  │
│   tool  read_file ✓  4ms                 │
└──────────────────────────────────────────┘
```

连接逻辑在 `_socket_loop()` 里，`while True` 驱动：连接失败等 2 秒重试，连接成功后订阅事件、等到断开，断开后再等 2 秒重试：

```python
# tui/app.py（节选）

async def _socket_loop(self) -> None:
    while True:
        client = SocketClient(self._host, self._port)
        try:
            await client.connect()
        except (ConnectionRefusedError, OSError):
            status.update("● not connected — retrying in 2s")
            await asyncio.sleep(2)
            continue

        status.update(f"● connected  {self._host}:{self._port}")
        loop_task = asyncio.create_task(client.run_event_loop())
        client.on_event(lambda e: self._handle_event(e, log))

        try:
            await client.send_command("event.subscribe", {...})
            await loop_task   # 阻塞到连接断开
        finally:
            self._flush_tokens(log)
            await client.close()

        status.update("● disconnected — retrying in 2s")
        await asyncio.sleep(2)
```

这个循环以 Textual worker 的形式启动，而不是 `asyncio.create_task`：

```python
def on_mount(self) -> None:
    self.run_worker(self._socket_loop(), exclusive=True, name="socket")
```

Textual 的 `run_worker` 把协程集成在 Textual 自己的事件循环里，worker 协程直接调用 `log.write()` 或 `status.update()` 是安全的，不需要额外的线程同步机制。

**Token 缓冲**

LLM 流式生成时，`llm.token` 事件密集出现，每个只有一两个字符。每个 token 单独调一次 `RichLog.write()` 会导致屏幕频繁闪烁。

解决方案：收到 `llm.token` 只追加到内部字符串，不写屏；等到下一个非 token 事件来时，先把积攒的内容整体写入一行：

```python
# tui/app.py（节选）

def _handle_event(self, event: dict[str, Any], log: RichLog) -> None:
    t = event.get("type", "")

    if t == "llm.token":
        self._token_buf += event.get("token", "")
        return   # 不写屏

    self._flush_tokens(log)   # 非 token 事件来了，先把缓冲区写出去

    if t == "run.started":
        log.write(f"[bold blue]▶ run[/bold blue]  {event.get('run_id')}  {event.get('goal')}")
    elif t == "run.finished":
        s = event.get("status", "")
        color = "green" if s == "success" else "red"
        log.write(f"[{color}]■ run[/{color}]  {s}  {event.get('steps')} steps")
    ...
```

用户看到的效果：LLM 生成的一整段文字显示为一行，工具调用和步骤边界各自成行，有层次感，不闪烁。

***

## 验证

### 单元测试

```bash
uv run pytest tests/unit -v
```

106 个测试，约 0.7 秒。s2 新增的几个重点测试文件：

`test_ipc_broadcaster.py`：用 `asyncio.start_server` 在随机端口起临时服务器，测 topic 过滤（`"run.*"` 匹配 `"run.started"` 、不匹配 `"step.started"`）、scope 过滤、unsubscribe、死连接清理。全部不需要真实守护进程。

`test_socket_client.py`：同样起临时服务器，测命令响应路由（Future 被正确 resolve）、`IpcError` 抛出（`"error"` 字段触发 `fut.set_exception`）、事件推送回调、连接断开时所有 pending Future 被 cancel。

`test_tui_app.py`：用 `_FakeLog`（一个只有 `write()` 方法的简单对象）替代真实的 `RichLog`，绕开 Textual 的渲染层，直接测 `_handle_event()` 的逻辑——token 缓冲、flush 时机、颜色标记。

### 集成测试

```bash
uv run pytest tests/integration/test_s2_dual_process.py -v
```

三个测试，在真实守护进程上运行，不需要 `ANTHROPIC_API_KEY`（`run.started` 在 LLM provider 初始化之前就推出来了）。

Fixture `running_daemon` 在随机端口启动真实守护进程，轮询等待连接就绪，测试结束后发 SIGTERM 关闭：

```python
@pytest.fixture
async def running_daemon(free_port: int):
    proc = subprocess.Popen(
        [sys.executable, "-m", "kama_claude.core"],
        env={**os.environ, "KAMA_PORT": str(free_port), "KAMA_LOG_LEVEL": "WARNING"},
    )
    deadline = time.monotonic() + 3.0
    while time.monotonic() < deadline:
        await asyncio.sleep(0.05)
        try:
            _, w = await asyncio.open_connection("127.0.0.1", free_port)
            w.close(); break
        except (ConnectionRefusedError, OSError):
            pass
    yield proc
    proc.terminate(); proc.wait(timeout=2)
```

**test 1**：发 `agent.run`，5 秒内收到 `run.started`，且事件里的 `run_id` 与命令返回的一致。

**test 2**：两个客户端同时订阅，一个触发 `agent.run`，验证两个客户端都收到 `run.started`。用 `asyncio.gather` 并行等待两个事件：

```python
await asyncio.wait_for(
    asyncio.gather(event1.wait(), event2.wait()),
    timeout=5.0,
)
```

**test 3**：client1 触发 run、等到 `run.started` 后断开；client2 用 `replay_from_run=run_id` 重连，验证 `replayed_count > 0`——确认事件落盘和回放的完整闭环。

### 手动验证

```bash
# 终端 A：前台启动守护进程
uv run kama-core

# 终端 B：打开 TUI（先开）
uv run kama-tui

# 终端 C：触发一次 run
uv run kama run --goal "用一句话介绍你自己"
```

终端 B 的 TUI 和终端 C 的 CLI 应该同时滚动出事件流——这是 s1 做不到的，是 s2 的核心交付。

***

## 小结与展望

s2 的核心是把 s1 的进程内事件流变成 daemon 对外提供的 IPC 事件流。`AgentRunner` 开始在 Core daemon 中运行，CLI 和 TUI 都通过 `SocketClient` 连接 daemon，先订阅事件，再发送 `agent.run`，随后从同一条 NDJSON 连接中接收命令响应和事件推送。

这个阶段新增了几条关键边界：

* **daemon 是唯一执行体**：`kama run` 不再直接创建 `AgentRunner`，而是发送 `agent.run` 命令。
* **EventBus 变成 daemon 级事件源**：`EventWriter` 继续写 `events.jsonl`，`IpcEventBroadcaster` 同时把事件推给 TCP 客户端。
* **客户端共享同一套 IPC 代码**：CLI 和 TUI 都复用 `SocketClient`，请求-响应用 `Future` 配对，事件推送用回调分发。
* **断线后可以回放历史**：`event.subscribe` 支持 `replay_from_run`，客户端能先读取 `events.jsonl`，再接续实时流。

目前的限制也很明确：s2 只验证双进程架构和事件流外化，不处理多任务调度。daemon 同一时刻只支持一个活跃 run，新请求会被拒绝；s3 会在这个 IPC 基础上引入任务状态机和更完整的任务视图。


> 更新: 2026-05-27 19:18:06  
> 原文: <https://www.yuque.com/chengxuyuancarl/nvd81p/aymaqgmsmyk1k58c>