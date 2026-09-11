# Agent 开发实践（六）：Agent Loop 的第一次落地

在上一章中，我们通过 `JChatMindV1` 实现了一个最小可用的聊天 Agent。

它已经具备了三个重要特征：

* 模型能力通过 `ChatClient` 接入
* 对话上下文由 `ChatMemory` 显式维护
* 对话是有 session、有状态的

但如果你回头看 V1 的实现，会发现一个明显的限制：

> **模型只能“说话”，不能“做事”。**

一旦用户的问题涉及外部世界（日期、天气、数据库、接口），V1 就已经走到了尽头。

这一章的目标，就是在 **不推翻 V1 结构** 的前提下，引入 Agent Loop，让 Agent 开始具备：

* 判断是否需要外部能力
* 主动发起工具调用
* 根据工具结果继续推进任务

## 一、从“聊天”到“做事”，差的是什么？

很多人第一次接触 Agent 时，会以为核心在于“多聪明的模型”。但从工程视角看，真正的分水岭其实只有一个问题：

> **这一次模型回复，是不是最终答案？**

如果不是，那么系统必须介入，推动下一步。

JChatMindV2 的全部设计，都是围绕这个问题展开的。

## 二、V2 相比 V1，多了哪些关键组件

我们先从类结构层面看 V2 的变化。

```java
public class JChatMindV2 extends JChatMindV1 {
    protected List<ToolCallback> availableTools;
    protected ToolCallingManager toolCallingManager;
    protected ChatOptions chatOptions;
    protected ChatResponse lastChatResponse;
}
```

相比 V1，新增的字段如下：

* **availableTools**：Agent 当前可使用的外部能力
* **ToolCallingManager**：统一执行工具调用的协调器
* **ChatOptions**：显式控制模型行为
* **lastChatResponse**：保存最近一次模型输出，作为执行阶段的输入

## 三、Agent Loop 的执行流程控制

在构造函数中，我们关闭了 Spring AI 的自动工具执行：

```java
this.chatOptions = DefaultToolCallingChatOptions.builder()
        .internalToolExecutionEnabled(false)
        .build();
```

Spring AI 原生支持 **模型 → 自动执行工具 → 回填结果** 的一条龙流程。

但我们在这里选择了**手动接管执行权**。

原因是**Agent Loop 不是简单的“工具调用”，而是“对执行过程的控制”。**

一旦工具被自动执行，Agent 将失去：

* 记录执行轨迹的能力
* 插入中断、回滚、重试的机会
* 对“是否继续”的判断权

所以在 V2 中，我们宁愿多写一点代码，也要把 **“执行” 留在 Agent 内部**。

现在来看 Agent Loop 的具体功能模块划分：

**功能划分概览：**

* \*\*Think（决策模块）\*\*负责分析当前对话上下文与系统状态，判断下一步行动策略
* \*\*LLMs（大语言模型）\*\*根据 Think 阶段给定的决策约束与上下文信息，生成结构化的推理结果，包括普通回复或带有工具调用描述的输出。
* \*\*Execute（执行器）\*\*负责解析并执行 LLMs 生成的工具调用请求，将真实执行结果以标准化形式回注到对话历史中。

**三个部分之间交互预览：**

* **Think → LLMs**：生成策略与行为规划
* **LLMs → Execute**：输出动作描述与工具调用请求
* **Execute → Think**：反馈执行结果并更新系统状态

这个循环会持续运行，直到任务被明确标记为完成，或者超过循环最高次数限制。

![1766649802351-e337b085-526d-405f-a86f-2a709325aaaf.png](https://cdn.davidingplus.cn/images/2026/09/11/1766649802351-e337b085-526d-405f-a86f-2a709325aaaf-077052.png)

## 四、Think 阶段：让模型决定“下一步干什么”

Agent Loop 的第一步，是 **Think**。

```java
protected boolean think() {
    String thinkPrompt = """
        现在你是一个智能的「决策模块」。
        请根据当前对话上下文，决定下一步的动作。
        如果需要调用工具来完成任务，请调用相应的工具。
        """;
```

这里非常重要的一点是：**Think 阶段的目标不是“给用户回答”，而是“做决策”。**

接下来是模型调用：

```java
this.lastChatResponse = chatClient
        .prompt(prompt)
        .system(thinkPrompt)
        .toolCallbacks(availableTools.toArray(new ToolCallback[0]))
        .call()
        .chatClientResponse()
        .chatResponse();
```

这里发生的三件事：

1. 使用完整的上下文（chatMemory）
2. 暴露所有可用工具
3. 获取**完整 ChatResponse**，而不是直接取文本，因为 ChatResponse 包含了工具调用的内容

## 五、ToolCall 出现时，为什么不立刻写入 memory？

Think 阶段结束后，这里做了一个预防 bug 的处理：

```java
if (toolCalls.isEmpty()) {
    chatMemory.add(sessionId, output);
}
```

也就是说：

* **没有工具调用 → 这是最终回复 → 写入 memory**
* **有工具调用 → 暂不写入 → 交给 execute() 统一处理**

这一点非常重要。如果把“带 tool\_calls 的 AssistantMessage”提前写入 memory，而后面工具执行失败或缺失，会导致上下文不一致。

> 这个问题只有在真实写代码的时候遇到

## 六、Execute 阶段：系统真正“动手”的地方

Execute 阶段，是 Agent 从“会想”走向“会做”的关键步骤。

```java
ToolExecutionResult toolExecutionResult =
        toolCallingManager.executeToolCalls(prompt, this.lastChatResponse);

ToolResponseMessage toolResponseMessage = (ToolResponseMessage) toolExecutionResult
                .conversationHistory()
                .get(toolExecutionResult.conversationHistory().size() - 1);
```

这一行代码背后，发生了完整的工具调用链路：

1. 取出带 tool\_calls 的 AssistantMessage
2. 根据 name + arguments 执行工具
3. 生成 ToolResponseMessage
4. 返回完整的新对话历史

## 七、Step：Think + Execute 的最小循环单元

```java
protected void step() {
    if (think()) {
        execute();
    } else {
        agentState = AgentState.FINISHED;
    }
}
```

> **一次 step = 一次决策 +（可选的）一次执行**

## 八、chat() 方法，已经不再是“一次调用”

在 V2 中，`chat()` 方法已经发生了语义变化：

```java
for (int i = 0; i < MAX_STEPS && agentState != AgentState.FINISHED; i++) {
    step();
}
```

这不再是“问一句，答一句”，

而是：

> **给 Agent 一个目标，让它自己跑完。**

MAX\_STEPS 的存在，也清晰地表达了一个工程态度：

* Agent 不应该无限循环
* 模型的推理深度是有上限的
* 系统必须兜底

## 九、到这里，我们已经真正拥有了一个 Agent Loop

到 JChatMindV2 为止，我们已经完成了 Agent 系统中最困难的一步：

* 模型不再直接回答问题
* 模型开始参与“下一步该做什么”的决策
* 系统与模型形成了明确的职责分工

后续无论是：

* 引入 Planning
* 加入失败重试
* 接入 RAG
* 做执行轨迹可视化

都只是 **在这个 Loop 上继续加层**。

## 十、测试

我们依旧写一个测试函数，测试 Loop 和工具调用有没有问题

```java
    @Autowired
    @Qualifier("deepseek-chat")
    private ChatClient chatClient;

    @Autowired
    private CityTool cityTool;

    @Autowired
    private DateTool dateTool;

    @Autowired
    private WeatherTool weatherTool;

    @Test
    public void testToolCalling() {
        // 准备工具回调
        ToolCallback[] toolCallbacks = MethodToolCallbackProvider.builder()
                .toolObjects(cityTool, dateTool, weatherTool)
                .build()
                .getToolCallbacks();

        // 创建 V2 实例
        JChatMindV2 agent = new JChatMindV2(
                "test-agent-v2",
                "测试 Agent V2",
                "你是一个智能助手，可以帮助用户查询天气、日期和城市信息。",
                chatClient,
                20,
                "test-session-v2",
                Arrays.asList(toolCallbacks)
        );

        // 测试需要调用工具的对话
        String userInput = "今天的天气怎么样？";
        String response = agent.chat(userInput);

        // 验证回复不为空
        assertNotNull(response);
        assertTrue(response.length() > 0);

        System.out.println("用户输入: " + userInput);
        System.out.println("AI 回复: " + response);
        System.out.println("对话历史长度: " + agent.getConversationHistory().size());
    }
```

测试结果如下：

![1766489289869-2a9c9ee8-13c9-49b5-9621-b33972065eab.png](https://cdn.davidingplus.cn/images/2026/09/11/1766489289869-2a9c9ee8-13c9-49b5-9621-b33972065eab-979831.png)


> 更新: 2025-12-25 16:25:01  
> 原文: <https://www.yuque.com/chengxuyuancarl/zsqzgh/yrwv7f5ywgotbubl>