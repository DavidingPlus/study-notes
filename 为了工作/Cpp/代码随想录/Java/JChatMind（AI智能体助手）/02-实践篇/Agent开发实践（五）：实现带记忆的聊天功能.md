# Agent 开发实践（五）：实现带记忆的聊天功能

前面的章节我们已经完成了模型接入、多模型管理，以及 Agent 与模型的解耦设计。

这一章我们通过一个最小可运行的 Demo：`JChatMindV1.java`，完整展示：

* Agent 如何调用模型
* 对话上下文是如何被维护的
* 系统提示词在什么时候生效
* 多轮对话为什么“看起来有记忆”

## 一、一个最小 Agent 需要哪些东西

我们从 `JChatMindV1` 这个类开始看。

这是一个 **V1 版本的最小 Agent 实现**，目标非常明确：**集成 LLM，能够完成最基本的聊天功能。**

在这个版本中，我们只保留了 Agent 的基本功能。

```java
public class JChatMindV1 {
    protected String name;
    protected String description;
    protected String systemPrompt;
    protected ChatClient chatClient;
    protected ChatMemory chatMemory;
    protected AgentState agentState;
    protected String sessionId;
}
```

这几个字段用途分别为：

* **ChatClient**：模型能力的入口
* **ChatMemory**：会话上下文
* **systemPrompt**：系统提示词
* **sessionId**：会话 ID
* **agentState**：当前 Agent 的状态

在构造函数中，`JChatMindV1` 做了三件事情。

```java
this.chatMemory = MessageWindowChatMemory.builder()
        .maxMessages(maxMessages != null ? maxMessages : DEFAULT_MAX_MESSAGES)
        .build();
```

第一，初始化 `ChatMemory`，并限制最多保存的消息数量。

这一步对应的是我们在前面反复强调的一个事实：

**模型不会记忆上下文，上下文是系统帮它记住的。**

`MessageWindowChatMemory` 是 \*\*Spring AI \*\*提供的一个简单实现，用“滑动窗口”的方式保留最近 N 条消息。

```java
if (StringUtils.hasLength(systemPrompt)) {
    this.chatMemory.add(this.sessionId, new SystemMessage(systemPrompt));
}
```

第二，在对话一开始，就把 `systemPrompt` 放进记忆中。

这里非常重要的一点是：**system message 不是每一轮都加，而是在会话开始时加一次。**

从模型视角看，这相当于在整个对话期间，都存在一个“隐形的背景规则”。

```java
this.agentState = AgentState.IDLE;
```

第三，初始化 Agent 状态。

虽然在 V1 版本中，状态还没有复杂逻辑，但这个字段的存在非常重要。

## 二、一次最完整的聊天流程

接下来，我们重点看 `chat(String userInput)`这个方法。

系统在与模型交互时，会将历史聊天上下文一并带上。

在交互完毕后，模型的输出也会加入到历史聊天上下文中。

![1766655510033-cd723ec3-04f0-492c-8a34-4192cfe99149.png](./img/wKf0YikMBi1zakFm/1766655510033-cd723ec3-04f0-492c-8a34-4192cfe99149-432170.png)

### 状态检查与输入校验

```java
Assert.notNull(userInput, "用户输入不能为空");

if (agentState != AgentState.IDLE) {
    throw new IllegalStateException("Agent 状态不是 IDLE");
}
```

这里体现的是一个非常基础、但非常重要的工程习惯：

> **Agent 是有状态的，不是一个随便可重入的方法。**

### 把用户输入写入上下文

```java
UserMessage userMessage = new UserMessage(userInput);
chatMemory.add(sessionId, userMessage);
```

不是把 `userInput` 直接发给模型，而是**先写入 ChatMemory**。

这意味着：

* 当前输入会成为上下文的一部分
* 下一轮对话，模型还能“看到”它

这正是多轮对话产生“记忆感”的根本原因。

### 构建 Prompt，并调用模型

```java
Prompt prompt = Prompt.builder()
        .messages(chatMemory.get(sessionId))
        .build();

ChatResponse response = chatClient
        .prompt(prompt)
        .call()
        .chatResponse();
```

到这里，前面几章的内容开始真正“对齐”了。

你会发现：

* Prompt 里并没有魔法
* 只是把当前 session 下的所有 messages 原样交给模型

Spring AI 并没有替你“自动管理上下文”，它只是帮你把 **“一次模型调用”** 包装得更干净。

### 处理模型回复，并写回上下文

```java
AssistantMessage assistantMessage = response.getResult().getOutput();
String aiResponse = assistantMessage.getText();

chatMemory.add(sessionId, assistantMessage);
```

这一段逻辑，和前面处理用户输入是完全对称的：

* 用户说的话 → UserMessage → 进 memory
* 模型说的话 → AssistantMessage → 进 memory

从模型的角度看，它并不区分“旧消息”和“新消息”，它看到的永远只是：**一段完整的 messages 列表**。

## 三、测试

写一个测试函数：

```java
@Autowired
@Qualifier("deepseek-chat")
private ChatClient chatClient;

@Test
public void testMultiTurnConversation() {
    // 创建 V1 实例
    JChatMindV1 agent = new JChatMindV1(
        "test-agent-v1",
        "测试 Agent V1",
        "",
        chatClient,
        20,
        "test-session-v1-multi"
    );

    // 第一轮对话
    String response1 = agent.chat("我的名字叫做张三");
    assertNotNull(response1);
    System.out.println("第一轮 - [用户]: 我的名字叫做张三？");
    System.out.println("第一轮 - [AI]: " + response1);

    // 第二轮对话（测试上下文记忆）
    String response2 = agent.chat("我的名字叫做什么？");
    assertNotNull(response2);
    System.out.println("第二轮 - [用户]: 我的名字叫做什么？");
    System.out.println("第二轮 - [AI]: " + response2);

    // 验证对话历史包含多轮对话
    assertTrue(agent.getConversationHistory().size() >= 4); // 至少包含：系统消息 + 用户消息1 + AI回复1 + 用户消息2 + AI回复2
}
```

查看测试效果：

![1766489108572-56e9a257-0a49-4644-9421-55b4e0b21edb.png](./img/wKf0YikMBi1zakFm/1766489108572-56e9a257-0a49-4644-9421-55b4e0b21edb-251748.png)


> 更新: 2025-12-25 17:39:10  
> 原文: <https://www.yuque.com/chengxuyuancarl/zsqzgh/gwuggsb0edao1wyy>