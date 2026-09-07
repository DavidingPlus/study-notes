# S4-把 Agent 变成会话伙伴

# 第 4 阶段：把 Agent 变成会话伙伴

| **阶段** | s4 |
| --- | --- |
| **分支** | `stage/s4` |
| **本阶段新增** | Session 容器、thread/notes 分层记忆、`note_save` 工具、`kama chat`、TUI 输入框 |
| **依赖上一阶段** | s3 的 AgentRunner、ToolRegistry、任务工具和终端 TUI；s2 的 IPC 事件流 |

## 本阶段要做什么

s3 结束时，agent 已经不只是"读一个文件、回答一个问题"了。它能自己创建任务、更新任务、运行 shell、写文件，甚至在 TUI 里把执行过程完整展示出来。

但它仍然有一个很硬的边界：**一次 run 结束之后，下一次 run 什么都不记得**。

试一下这个交互：

```bash
uv run kama run --goal "项目用什么 Python 版本？"
```

agent 读了 `pyproject.toml`，回答："Python 3.12"。这时你接着想说：

```latex
那写一个适合该版本的新特性 demo
```

在 s3 里，这句话没有地方可说。你只能重新发一条 `kama run`：

```bash
uv run kama run --goal "写一个适合该版本的新特性 demo"
```

问题来了：这里的"该版本"指什么？上一轮的答案已经随着进程结束一起消失了。agent 要么重新读 `pyproject.toml`，要么猜错。

s4 要解决的就是这个问题：让多个 run 共享同一个 **Session**。s4 给 `kama-tui` 底部加了输入框，用户不再每次都发一次独立命令，而是在 TUI 里进入持续会话：

```bash
uv run kama-tui
```

TUI 启动后自动创建 chat session，底部输入框就绪。在输入框里输入第一条消息：

```plain
项目用什么 Python 版本？
```

按 Enter，输出区域流式展示工具调用和 agent 回答。完成后输入框重新激活，继续输入：

```plain
写一个适合该版本的新特性 demo
```

这一次 agent 不需要重新读 `pyproject.toml`。它从上一轮的 thread 里已经知道答案，直接开始写。

第二轮里，agent 不需要重新理解"该版本"。它能看到第一轮的对话、工具调用结果，也能看到自己主动保存的关键笔记。这就是 s4 的核心：**把 agent 从一次性任务执行者，变成能连续协作的会话伙伴**。

这一章的主线就是一次 `kama-tui` 会话：用户启动 TUI，daemon 创建 session，用户在输入框发送第一条消息，AgentRunner 读取 thread 和 notes，run 结束后写回记忆，然后第二条消息复用这些记忆。

![1779887730297-61187954-ce96-4f26-aa60-02b37ed1f683.png](./img/QvMgiAVBDfbWK5Yf/1779887730297-61187954-ce96-4f26-aa60-02b37ed1f683-362521.png)

***

## `kama chat`：session 协议的 CLI 参考实现

TUI 输入框背后走的协议和 `kama chat` 完全相同。先看 `kama chat` 的实现，它更简洁，能让 session 协议的脉络看得更清楚。

s2 和 s3 的 `kama run` 是一次性命令：发 goal，等 run.finished，然后退出。`kama chat` 不退出，它反复做三件事：

1. 读用户输入
2. 发给 daemon
3. 打印 daemon 推回来的事件

入口在 `cli/commands/chat.py`：

```python
# cli/commands/chat.py（节选）

async def _chat_async(config: KamaConfig) -> int:
    client = SocketClient(config.host, config.port)
    await client.connect()

    printer = ChatPrinter()
    client.on_event(printer.handle)
    loop_task = asyncio.create_task(client.run_event_loop())

    await client.send_command("event.subscribe", {
        "topics": ["session.*", "run.*", "step.*", "tool.*", "llm.*"],
    })
    created = await client.send_command("session.create", {"mode": "chat"})
    session_id = str(created["session_id"])
    print(f"[session: {session_id}]")

    while True:
        line = await _readline("> ")
        if not line.strip():
            continue
        await client.send_command("session.send_message", {
            "session_id": session_id,
            "content": line,
        })
```

这里沿用了 s2 的 `SocketClient`：一条 TCP 连接上既发命令，也收事件。不同的是，`kama chat` 先发 `session.create`，拿到一个 `session_id`，后续每条用户输入都发 `session.send_message`。TUI 启动时做的是完全一样的事，只是把 `input()` 换成了底部输入框。

注意顺序：**先订阅事件，再创建 session**。如果反过来，daemon 可能已经广播了 `session.created`，客户端才开始订阅，第一条事件就丢了。s2 里 `kama run` 已经有这个经验：先 `event.subscribe`，再触发真正的动作。

还有一个小但关键的细节：读 stdin 不能直接 `input()`。

```python
async def _readline(prompt: str) -> str:
    loop = asyncio.get_running_loop()
    return await loop.run_in_executor(None, input, prompt)
```

`input()` 是阻塞调用。如果直接在 async 函数里调用它，整个事件循环会停住；用户还没敲完下一句话时，daemon 推过来的 `llm.token` 也打印不出来。把 `input` 放到 executor 线程里，主事件循环就能继续收事件，终端才能一边等待输入、一边流式输出。

`ChatPrinter` 做的事情很少：收到 `llm.token` 就内联打印，收到 `tool.call_started` 就换行打印工具调用，收到 `session.waiting_for_input` 就提示 `[waiting for input]`。它不保存任何会话状态。

> 💡 两个客户端都只是"眼睛和耳朵"。真正的 session 状态、thread、notes、run 调度都在 daemon 里。TUI 和 CLI 共享完全相同的协议，不各自维护状态。

***

## SessionManager：daemon 里的会话入口

`SocketServer` 收到 `session.create` 后，最终会走到 `CoreApp` 的 handler：

```python
# core/app.py（节选）

async def _session_create_handler(self, params: dict[str, Any]) -> SessionCreateResult:
    cmd = SessionCreateCommand.model_validate(params)
    session = await self._sessions.create(mode=cmd.mode, title=cmd.title)
    return SessionCreateResult(session_id=session.id, status=session.status)
```

这里的 `_sessions` 就是 s4 新增的 `SessionManager`。创建 session 时，它生成一个 `sess-` 开头的 ID，写一份 `meta.json`，再发布 `session.created`：

```python
# core/session/manager.py（节选）

async def create(self, mode: SessionMode, title: str = "") -> Session:
    sid = f"sess-{uuid.uuid4().hex[:12]}"
    ts = _now()
    session = Session(
        id=sid,
        mode=mode,
        status="active",
        title=title,
        created_at=ts,
        updated_at=ts,
        run_ids=[],
    )
    self._sessions[sid] = session
    self._locks[sid] = asyncio.Lock()
    self._store.write_meta(session)
    await self._bus.publish(SessionCreatedEvent(session_id=sid, mode=mode, ts=ts))
    return session
```

磁盘上，一个 session 是这样的：

```latex
~/.kama/sessions/sess-9f3a2c1b8d04/
  meta.json
  thread.jsonl
  notes.md
  runs/
    20260519-103012-a1b2c3/
      events.jsonl
      .tasks/
```

`meta.json` 是 session 档案：ID、模式、状态、标题、创建时间、更新时间、包含哪些 run。`runs/` 下面仍然是 s1/s2/s3 熟悉的 run 目录，只是它们现在归属到一个 session 下。

为什么不继续把 run 写到当前项目的 `runs/` 目录？

s2 之后 daemon 是一个全局服务。你可以从任意工作目录连接它。如果数据还散在各个项目目录里，session 就很难统一管理。s4 把会话数据集中到 `~/.kama/sessions/`，和 `~/.kama/config.toml`、`~/.kama/logs/` 放在同一个用户级空间里。

> 💡 s4 开始，run 不再是顶层概念。run 是 session 里的一个回合，session 才是用户连续协作的容器。

***

## 第一条消息：先写 thread，再启动 run

用户在 chat 里输入：

```latex
项目用什么 Python 版本？
```

CLI 发出：

```json
{"method":"session.send_message","params":{"session_id":"sess-...","content":"项目用什么 Python 版本？"}}
```

daemon 侧的主逻辑在 `SessionManager.send_message`：

```python
# core/session/manager.py（节选）

async def send_message(self, sid: str, content: str, *, run_id: str | None = None) -> str:
    session = self._get_session(sid)
    lock = self._locks[sid]
    if lock.locked():
        raise HandlerError(SESSION_BUSY, "session busy")

    async with lock:
        if session.status == "closed":
            raise HandlerError(SESSION_CLOSED, "session already closed")

        if session.status == "waiting_for_input":
            await self._bus.publish(SessionResumedEvent(session_id=sid, ts=_now()))

        self._store.append_message(sid, "user", content)
        await self._bus.publish(SessionMessageReceivedEvent(...))

        run_id = run_id or new_run_id()
        session.run_ids.append(run_id)
        self._store.write_meta(session)

        runner = self._runner_factory()
        await runner.run_and_capture(
            content,
            run_id=run_id,
            session=session,
            store=self._store,
        )
        ...
```

最重要的是这句：

```python
self._store.append_message(sid, "user", content)
```

**用户消息必须先写进 **`thread.jsonl`**，再启动 AgentRunner。**

原因很直接：AgentRunner 接下来会读取整个 thread 作为 LLM 的 messages 前缀。如果这条 user 消息还没写进去，LLM 就看不到用户本轮到底问了什么。

这里也引入了每个 session 一把 `asyncio.Lock`。如果同一个 session 正在跑，用户又发来第二条消息，系统直接返回 `session busy`，而不是排队。

为什么不排队？因为 agent 的下一轮应该建立在上一轮完整结果上。上一轮还没结束时，thread 和 notes 都没稳定，提前排队只会制造更难理解的状态。busy 是更清晰的语义：等 `[waiting for input]` 再发下一句。

***

## AgentRunner：从 session 里恢复上下文

`SessionManager` 把 `session` 和 `store` 传给 `AgentRunner.run_and_capture`。runner 看到这两个参数，就不再创建一个孤立 run，而是从 session 里恢复上下文：

```python
# core/runner.py（节选）

async def run_and_capture(self, goal, *, run_id=None, session=None, store=None):
    run_id = run_id or new_run_id()
    if session is not None and store is not None:
        run_path = store.runs_dir(session.id) / run_id
        history = store.read_messages(session.id)
        notes = store.read_notes(session.id)
    else:
        run_path = self._runs_dir / run_id
        history = [{"role": "user", "content": goal}]
        notes = ""

    context = ExecutionContext(
        run_id=run_id,
        goal=goal,
        max_steps=self._config.agent.max_steps,
        prefill_messages=history,
        session_notes=notes,
    )
```

这里出现了 s4 的两层记忆：

* `thread.jsonl` 读出来变成 `history`，作为 `messages` 前缀。
* `notes.md` 读出来变成 `notes`，注入 system prompt。

这两层听起来都叫"记忆"，但职责不一样。

`thread.jsonl` 记录的是**完整对话过程**：用户说了什么，assistant 返回了什么，工具调用了什么，工具结果是什么。它回答的是"上一轮发生过什么"。

`notes.md` 记录的是 agent 主动保存的**长期事实和决策**。它回答的是"以后应该记住什么"。

***

## thread.jsonl：完整回放，而不是最近 K 轮

`SessionStore.read_messages` 读出来的不是给人看的日志，而是可以直接发给 Anthropic API 的 messages：

```python
# core/session/store.py（节选）

def read_messages(self, sid: str) -> list[dict[str, Any]]:
    path = self.session_dir(sid) / "thread.jsonl"
    if not path.exists():
        return []

    messages = []
    for line_no, line in enumerate(path.read_text().splitlines(), start=1):
        if not line:
            continue
        try:
            row = json.loads(line)
        except json.JSONDecodeError:
            logger.warning("skip broken thread row sid=%s line=%s", sid, line_no)
            continue
        role = row.get("role")
        if role not in ("user", "assistant"):
            continue
        messages.append({"role": role, "content": row.get("content", "")})

    return self._trim_orphan_tool_use(messages)
```

注意最后返回前调用了 `_trim_orphan_tool_use`。Anthropic 的消息格式要求 `tool_use` 后面必须有匹配的 `tool_result`。如果某次 run 在工具调用中途崩了，thread 里可能留下半截 tool\_use。下次读取时把孤儿 tool\_use 裁掉，可以避免 API 直接报 `messages.invalid`。

第一轮结束后，`thread.jsonl` 可能长这样：

```plain
{"role":"user","content":"项目用什么 Python 版本？"}
{"role":"assistant","content":[
  {"type":"text","text":"我先看 pyproject.toml。"},
  {"type":"tool_use","id":"toolu_01","name":"read_file","input":{"path":"pyproject.toml"}}
]}
{"role":"user","content":[
  {"type":"tool_result","tool_use_id":"toolu_01","content":"requires-python = \">=3.12\""}
]}
{"role":"assistant","content":[
  {"type":"text","text":"项目使用 Python 3.12。"}
]}
```

s4 的选择是：**每次新 run 启动时完整回放整个 thread，不做最近 K 轮截断。**

一开始你可能会想，完整回放太浪费 token 了，为什么不只取最近 5 轮？

问题在于滑动窗口看起来省钱，实际会破坏 agent 的连续性：

* 它可能截断 `tool_use` 和 `tool_result` 的配对，直接让 Anthropic API 拒绝请求。
* 它会丢掉旧工具结果，而旧工具结果可能正是下一轮回答的依据。
* 它把"哪些历史重要"这个判断提前交给工程代码，但这件事通常模型自己更擅长。

s1 已经接入了 Anthropic prompt caching。完整 thread 的旧前缀在下一轮大概率命中缓存，`cache_read_input_tokens` 会承担大头。我们用缓存降低成本，而不是用工程截断破坏语义。

> 💡 完整回放不是偷懒，而是现代 agent 会话的基础策略：保留完整消息结构，让模型看到真实过程，再靠 prompt caching 控制成本。

run 结束时，runner 只把本轮新增的 messages 追加回 thread：

```python
# core/runner.py（节选）

if session is not None and store is not None:
    store.append_messages(session.id, context.messages[prefill_len:], run_id=run_id)
```

`prefill_len` 是 run 开始时历史 messages 的长度。run 跑完后，`context.messages[prefill_len:]` 就是本轮新产生的 assistant 消息、tool\_result 消息和最终回答。

***

## notes.md：agent 自己给未来留笔记

只有 thread 够不够？s4 阶段短对话里，确实看起来够。

第一轮 thread 已经记录了 `pyproject.toml` 里的 Python 版本，第二轮完整回放时 LLM 能看到。那为什么还要 `notes.md`？

因为 thread 是"历史流水"，notes 是"事实层"。s6 会引入 compact：当 thread 过长时，旧消息会被压缩成摘要。摘要可能漏掉某个细节，但 notes 不参与 compact，会原样注入 system prompt。

s4 先把这条契约建立起来：agent 在知道某件事以后，可以主动调用 `note_save` 记录它。

```python
# core/tools/builtin/note_save.py（节选）

class NoteSaveTool(BaseTool):
    name = "note_save"
    description = (
        "Save a fact or decision to the session's notes. "
        "These notes will be visible to you in future turns of this session."
    )
    input_schema = {
        "type": "object",
        "properties": {
            "content": {"type": "string"},
        },
        "required": ["content"],
    }

    def __init__(self, store, session_id, run_id):
        self._store = store
        self._sid = session_id
        self._run_id = run_id

    async def invoke(self, params):
        content = str(params["content"]).strip()
        if not content:
            return ToolResult(content="empty content", is_error=True)
        self._store.append_note(self._sid, content, self._run_id)
        return ToolResult(content="saved")
```

这个工具只在 session run 里注册：

```python
if session is not None and store is not None and run_id is not None:
    registry.register(NoteSaveTool(store, session.id, run_id))
```

没有 session 的普通 run 不应该看到 `note_save`。因为 `note_save` 的语义就是"写当前 session 的 notes"，没有 session\_id 就没有写入目标。

为什么不在每次 run 结束后自动总结，把摘要写入 notes？

自动总结有三个问题：

* 多一次 LLM 调用，增加延迟和成本。
* 事后总结是在猜哪些内容未来重要。
* 工具调用是显式事件，能被 events.jsonl 记录，也能在 TUI 里看到。

让 agent 自己调用 `note_save`，等于让它在做决策的当下标记"这条以后还会用"。这比事后猜测更可靠。

***

## notes 怎么进入 LLM：注入 system prompt

`ExecutionContext` 收到 `session_notes` 后，不把它塞进 messages，而是拼进 system prompt：

```python
# core/context.py（节选）

def system_prompt(self, base: str) -> str:
    if not self.session_notes.strip():
        return base
    return (
        base
        + "\n\n## Session Notes\n"
        + self.session_notes.strip()
        + "\n\nRemember important durable facts by calling note_save."
    )
```

为什么放 system，而不是伪造一条 user 消息？

notes 不是某一轮用户说的话，它是会话层的持久上下文。放进 system prompt，能让模型把它当作当前会话背景，而不是误以为用户刚刚又说了一遍。

`AgentLoop` 调 LLM 时，把这个动态 system 传给 provider：

```python
response = await self._provider.chat(
    messages=context.messages,
    tool_schemas=self._registry.tool_schemas(),
    bus=self._bus,
    run_id=context.run_id,
    step=context.step,
    system=context.system_prompt(BASE_SYSTEM_PROMPT),
)
```

这里有一个缓存上的取舍。notes 一变，system prompt 也变，下一次 prompt cache 会重建一次。这个代价可以接受，因为 notes 不应该每一步都变；而当 notes 真的变了，让模型立即看到新事实比缓存命中更重要。

> ⚠️ notes 是本地 agent 写入的受信任上下文。s4 不处理 notes 里的 prompt injection。后续权限和上下文治理阶段会继续收紧边界。

***

## 第二轮：记忆开始生效

第一轮结束后，chat 进入等待状态：

```python
if session.mode == "one_shot":
    session.status = "closed"
    await self._bus.publish(SessionClosedEvent(...))
else:
    session.status = "waiting_for_input"
    await self._bus.publish(SessionWaitingForInputEvent(...))
```

CLI 收到 `session.waiting_for_input`，打印：

```latex
[waiting for input]
```

用户输入第二句：

```latex
写一个适合该版本的新特性 demo
```

这一次 `send_message` 还是同样的路径：先追加 user 消息，再启动 run。但 `AgentRunner` 读到的 `history` 已经不是空的了：

```python
messages = [
    {"role": "user", "content": "项目用什么 Python 版本？"},
    {"role": "assistant", "content": [text_block, tool_use(read_file)]},
    {"role": "user", "content": [tool_result("requires-python >=3.12")]},
    {"role": "assistant", "content": [text_block("项目使用 Python 3.12")]},
    {"role": "user", "content": "写一个适合该版本的新特性 demo"},
]
```

同时 system prompt 里还有：

```markdown
## Session Notes
Project uses Python 3.12.
```

LLM 现在同时看到两件事：

* thread 里有完整证据：上一轮读了 `pyproject.toml`，工具结果写着 Python 3.12。
* notes 里有整理后的事实：项目使用 Python 3.12。

所以它可以直接开始写 demo，而不是重新读取 `pyproject.toml`。

![1779887731469-856554bc-5f40-409d-9bee-0b4121eda0fc.png](./img/QvMgiAVBDfbWK5Yf/1779887731469-856554bc-5f40-409d-9bee-0b4121eda0fc-136301.png)

这就是 s4 的 payoff：多轮不是把上一轮终态文本拼到 prompt 里，而是把上一轮完整 API 消息流和主动笔记都带回来。

***

## `kama run` 仍然可用：one\_shot session

s4 引入 session 后，还有一个兼容问题：原来的 `kama run --goal ...` 怎么办？

我们不希望老命令突然变成另一套执行路径。s4 的做法是：让 `agent.run` 在 daemon 内部创建一个 `one_shot` session，然后调用同一个 `SessionManager.send_message`。

```python
# core/app.py（节选）

async def _agent_run_handler(self, params):
    cmd = AgentRunCommand.model_validate(params)
    session = await self._sessions.create(mode="one_shot", title=cmd.goal[:40])
    run_id = new_run_id()
    run_task = asyncio.create_task(
        self._sessions.send_message(session.id, cmd.goal, run_id=run_id)
    )
    self._running_runs.add(run_task)
    run_task.add_done_callback(self._running_runs.discard)
    return AgentRunResult(run_id=run_id)
```

one\_shot 和 chat 的区别只在 run 结束之后：

* `chat`：进入 `waiting_for_input`，继续等下一条消息。
* `one_shot`：进入 `closed`，行为上仍然像一次性 run。

这样 `kama run`、TUI 里已有的订阅逻辑、events.jsonl 都不用重写。只是底层存储统一到了 `~/.kama/sessions/<sid>/runs/<run_id>/`。

> 💡 这是渐进迁移。强制所有客户端立刻改成 `session.create + session.send_message` 也能做，但会同时改 CLI、TUI、测试和用户习惯。one\_shot session 让新模型先落地，旧入口继续工作。

***

## TUI：从只读窗口变成可输入会话

s3 的 TUI 只能看。它订阅事件，展示 run 过程，但用户要继续对话还得回 CLI。

s4 给 TUI 底部加了输入框。输入框提交后，走的就是上面介绍过的 `session.send_message`，和 `kama chat` 底层完全相同。

这里没有直接用 Textual 的 `Input`，而是用 `TextArea` 包了一层：

```python
# tui/app.py（节选）

class ChatTextArea(TextArea):
    class Submitted(Message):
        def __init__(self, area):
            self.text_area = area
            self.value = area.text
            super().__init__()

    async def _on_key(self, event):
        key = event.key
        if key == "enter":
            event.stop()
            event.prevent_default()
            if self.text.strip():
                self.post_message(self.Submitted(self))
            return
        if key in ("alt+enter", "shift+enter", "ctrl+j", "super+enter"):
            event.stop()
            event.prevent_default()
            if not self.read_only:
                self.insert("\n")
            return
        await super()._on_key(event)
```

选择 `TextArea` 是因为用户可能输入多行内容。语义是：

* Enter：提交
* Shift/Alt/Cmd+Enter：换行

如果用默认 `TextArea` 行为，Enter 会插入换行；如果用 `Input`，又没有多行能力。这里定制按键处理，就是为了让 TUI 更像一个真正的 chat 输入框。

输入提交后，TUI 也会在 run 进行中禁用输入框，等收到 `session.waiting_for_input` 再重新启用。这和 CLI 的 `[waiting for input]` 是同一个状态，只是呈现方式不同。

***

## 验证

启动 daemon：

```bash
uv run kama-core
```

另一个终端启动 TUI：

```bash
uv run kama-tui
```

TUI 底部输入框就绪后，依次发送两条消息：

```plain
项目的 Python 版本是多少？看 pyproject.toml
```

等 agent 回答完成、输入框重新激活，再发：

```plain
写一个适合该版本的新特性 demo 到 /tmp/demo.py
```

然后看 session 目录：

```bash
ls ~/.kama/sessions/sess-*/
# meta.json  notes.md  thread.jsonl  runs/

cat ~/.kama/sessions/sess-*/notes.md

cat ~/.kama/sessions/sess-*/thread.jsonl | python -c "
import json, sys
for line in sys.stdin:
    msg = json.loads(line)
    print(msg['role'], '|', str(msg['content'])[:120])
"
```

验收点不是"目录存在"这么简单，而是三件事：

* `thread.jsonl` 里有完整的 `tool_use` / `tool_result` 块，不只是最终文本。
* `notes.md` 里有 agent 通过 `note_save` 写下的关键事实。
* 第二轮 run 不需要重新读取 `pyproject.toml` 来理解"该版本"。

可以检查第二个 run 的事件：

```bash
cat ~/.kama/sessions/sess-*/runs/<run-2-id>/events.jsonl | grep '"tool_name":"read_file"'
```

如果这里又出现读取 `pyproject.toml`，说明记忆没有真正生效；如果它直接写 demo 或回答，说明 thread + notes 的路径跑通了。

***

## 小结与展望

s4 把系统的核心单位从 run 提升到了 session。

s1 让 agent 第一次跑起来；s2 把执行搬进 daemon，并把事件流外化为 IPC；s3 让 agent 能自主规划任务。到了 s4，agent 终于能接住上一轮的上下文，继续对话。

这个阶段真正确立的是一套跨 run 记忆模型：

* `thread.jsonl` 保存完整 API 消息流，保证下一轮能看到真实过程。
* `notes.md` 保存 agent 主动 curated 的关键事实，作为后续 compact 不会丢的事实层。
* `SessionManager` 把多个 run 组织到同一个会话里，负责状态、锁、事件和存储。
* TUI 输入框和 `kama chat` 走同一个 `session.send_message`，两个客户端共享完全相同的 daemon 逻辑，没有两套状态。


> 更新: 2026-05-27 21:16:01  
> 原文: <https://www.yuque.com/chengxuyuancarl/nvd81p/szyxrh3tae3rtwef>