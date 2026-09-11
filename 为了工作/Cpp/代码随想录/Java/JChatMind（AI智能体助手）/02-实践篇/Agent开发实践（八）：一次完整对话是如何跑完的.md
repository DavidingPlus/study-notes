# Agent 开发实践（八）：一次完整对话是如何跑完的

在前面的章节中，我们分别拆开讲了 Agent、Tool、RAG 等关键模块，但如果把它们放在一起看，仍然会有一个很现实的问题：

**当用户在前端输入一句话之后，系统内部究竟发生了什么？**

这一章不再介绍新组件，而是**把一次完整对话从头到尾走一遍**。

我们关心的不是“某个类做了什么”，而是：

* 请求从哪里进来
* 在哪里被转交
* Agent 是在什么时候被真正启动的
* 消息又是如何被一点点推回前端的

以下是一个简单时序图，能够简单描述一次交互示例：

![1766477226528-e7dcbba3-fba8-4810-b65c-6b1ae91e7436.png](https://cdn.davidingplus.cn/images/2026/09/11/1766477226528-e7dcbba3-fba8-4810-b65c-6b1ae91e7436-260851.png)

## 一、从前端的一次输入开始

整个流程的起点非常普通，用户在聊天输入框中敲下一段文字，按下回车。

在前端看来，这一步并没有任何“Agent”的概念，它只做三件事：

1. 如果当前还没有会话，先创建一个会话
2. 创建一条 **用户消息**
3. 把这条消息立刻渲染到聊天列表中

也就是说，**前端不会等待 AI 的任何结果**。

这是一个非常重要的设计点，用户输入的那一刻，系统就应该“立刻有反应”。

## 二、后端接收消息

当 `/api/chat-messages` 被调用时，后端 Controller 并不会启动 Agent，也不会去调用模型，而只是做两件事：

* 把用户消息持久化到数据库
* 发布一个“聊天事件”

这个事件里包含三样东西：

* agentId
* sessionId
* 用户输入内容

到这里，HTTP 请求就结束了。

```java
@Override
public CreateChatMessageResponse createChatMessage(CreateChatMessageRequest request) {
    ChatMessage chatMessage = doCreateChatMessage(request);
    // 发布聊天通知事件
    publisher.publishEvent(new ChatEvent(
                    request.getAgentId(),
                    chatMessage.getSessionId(),
                    chatMessage.getContent()
            )
    );
    // 返回生成的 chatMessageId
    return CreateChatMessageResponse.builder()
            .chatMessageId(chatMessage.getId())
            .build();
}
```

**Agent 还没有启动，但用户已经得到了响应。**

这一步的意义在于**把“消息创建”和“Agent 执行”彻底解耦。**

## 三、事件，才是真正的分水岭

Agent 启动，是从 `ChatEvent` 被监听到的那一刻才开始的。

事件监听器并不运行在 Web 线程中，而是一个异步任务。这意味着无论 Agent 跑多久，无论中途调用多少功能，都不会影响用户继续操作页面，监听器拿到事件之后，只做一件事：**创建一个 Agent 实例，并调用 **<code>**run()**</code>**。**

```java
@Component
@AllArgsConstructor
public class ChatEventListener {
    private final JChatMindFactory jChatMindFactory;
    @Async
    @EventListener
    public void handle(ChatEvent event) {
        // 创建一个 Agent 实例处理聊天事件
        JChatMind jChatMind = jChatMindFactory.create(event.getAgentId(), event.getSessionId());
        jChatMind.run();
    }
}
```

从这一刻起，系统正式启动了一个 Agent 对象。

## 四、Agent 是“现场组装”的

> **Agent 不是一个单例，也不是长期存活的对象。**

在创建过程中，系统会做几件非常重要的事情：

* 从数据库加载 Agent 的配置
* 根据配置选择对应的 ChatClient
* 从数据库中恢复最近的对话历史
* 解析当前 Agent 允许使用的工具
* 解析当前 Agent 允许访问的知识库

也就是说，这个 Agent 一出生，就已经拥有了完整的上下文环境。

```java
public JChatMind create(String agentId, String chatSessionId) {
    Agent agent = loadAgent(agentId);
    AgentDTO agentConfig = toAgentConfig(agent);
    List<Message> memory = loadMemory(chatSessionId);

    // 解析 agent 的支持的知识库
    List<KnowledgeBaseDTO> knowledgeBases = resolveRuntimeKnowledgeBases(agentConfig);
    // 解析 agent 支持的工具调用
    List<Tool> runtimeTools = resolveRuntimeTools(agentConfig);
    // 将工具调用转换成 ToolCallback 的形式
    List<ToolCallback> toolCallbacks = buildToolCallbacks(runtimeTools);

    return buildAgentRuntime(
            agent,
            memory,
            knowledgeBases,
            toolCallbacks,
            chatSessionId
    );
}

private JChatMind buildAgentRuntime(
        Agent agent,
        List<Message> memory,
        List<KnowledgeBaseDTO> knowledgeBases,
        List<ToolCallback> toolCallbacks,
        String chatSessionId
) {
    ChatClient chatClient = chatClientRegistry.get(agent.getModel());
    if (Objects.isNull(chatClient)) {
        throw new IllegalStateException("未找到对应的 ChatClient: " + agent.getModel());
    }
    return new JChatMind(
            agent.getId(),
            agent.getName(),
            agent.getDescription(),
            agent.getSystemPrompt(),
            chatClient,
            agentConfig.getChatOptions().getMessageLength(),
            memory,
            toolCallbacks,
            knowledgeBases,
            chatSessionId,
            sseService,
            chatMessageFacadeService,
            chatMessageConverter
    );
}
```

## 五、Think：决定“接下来该干嘛”

在 `think()` 阶段，Agent 会做一件非常明确的事情：**判断下一步是否需要调用工具。**

此时系统会构造一个 Prompt，其中包含：

* 当前的对话历史
* Agent 的系统提示词
* 可用工具列表
* 可访问的知识库说明

模型的输出，可能是一段普通的 Assistant 回复，或者一个（或多个）Tool Call。

不管是哪一种，这个输出都会被立刻持久化到数据库和放入“待推送列表”，然后通过 SSE 推送给前端。

## 六、Execute：工具才是“动手的人”

如果 Think 阶段发现了工具调用，Agent 会进入 Execute 阶段。

在这个阶段系统会根据 Tool Call 的描述，直接执行对应工具。

工具执行完成后，会生成 ToolResponseMessage。这些工具返回结果同样会被持久化，通过 SSE 推送到前端，写入对话历史。

```java
private void execute() {
    Assert.notNull(this.lastChatResponse, "Last chat client response cannot be null");

    if (!this.lastChatResponse.hasToolCalls()) {
        return;
    }

    Prompt prompt = Prompt.builder()
            .messages(this.chatMemory.get(this.chatSessionId))
            .chatOptions(this.chatOptions)
            .build();

    ToolExecutionResult toolExecutionResult = toolCallingManager.executeToolCalls(prompt, this.lastChatResponse);

    this.chatMemory.clear(this.chatSessionId);
    this.chatMemory.add(this.chatSessionId, toolExecutionResult.conversationHistory());

    ToolResponseMessage toolResponseMessage = (ToolResponseMessage) toolExecutionResult
            .conversationHistory()
            .get(toolExecutionResult.conversationHistory().size() - 1);

    String collect = toolResponseMessage.getResponses()
            .stream()
            .map(resp -> "工具" + resp.name() + "的返回结果为：" + resp.responseData())
            .collect(Collectors.joining("\n"));

    log.info("工具调用结果：{}", collect);

    // 保存工具调用
    saveMessage(toolResponseMessage);
    refreshPendingMessages();

    if (toolResponseMessage.getResponses()
            .stream()
            .anyMatch(resp -> resp.name().equals("terminate"))) {
        this.agentState = AgentState.FINISHED;
        log.info("任务结束");
    }
}
```

对 Agent 来说，工具返回的结果就是“新的上下文”。

## 七、消息并不是立刻推的

Agent 在运行过程中，并不会每生成一条消息就立刻推送，而是先统一保存到 `pendingChatMessages`，在合适的时机一次性推送。

```java
private void refreshPendingMessages() {
    for (ChatMessageDTO message : pendingChatMessages) {
        ChatMessageVO vo = chatMessageConverter.toVO(message);
        SseMessage sseMessage = SseMessage.builder()
                .type(SseMessage.Type.AI_GENERATED_CONTENT)
                .payload(SseMessage.Payload.builder()
                        .message(vo)
                        .build())
                .metadata(SseMessage.Metadata.builder()
                        .chatMessageId(message.getId())
                        .build())
                .build();
        sseService.send(this.chatSessionId, sseMessage);
    }
    pendingChatMessages.clear();
}
```

这样做的好处很明显：

* 避免高频 SSE 发送
* 保证消息顺序

另外一个原因是我们不能保证每次循环，模型只调用一个工具调用。

## 八、SSE 的作用

SSE 在这里承担的角色非常清晰：

* 不负责业务
* 不负责状态
* 只负责把消息“推过去”

后端只要知道一个 `chatSessionId`，就可以把消息精准地送到对应的前端页面。

而前端只需要监听不同的 `type`：

* 状态类消息 → 更新加载动画
* 内容类消息 → 插入聊天列表

两边并不共享任何内部结构，只通过事件类型和 payload 通信。

## 九、前端并不知道 Agent 的存在

站在前端视角看，这一整套流程非常“干净”。

它只知道三件事：

1. 我发送了一条消息
2. 服务端在不断推送新消息
3. 最终会告诉我“结束了”

至于：

* Think / Execute
* Tool / RAG
* Agent Loop

这些都被完整地隐藏在系统内部。


> 更新: 2025-12-23 20:02:23  
> 原文: <https://www.yuque.com/chengxuyuancarl/zsqzgh/xlhprbyxlq8b27p8>