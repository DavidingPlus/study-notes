# Agent 开发实践（一）：从最原始的大模型调用开始

在开始 Agent 开发时，如果一上来就直接使用 Spring AI、LangChain4j 这类框架，确实可以很快跑出效果。但这种“快”，往往是以牺牲理解为代价的。

如果你没有亲眼看过一次真实的大模型请求是如何发出的，也没有亲手处理过一次响应中的字段变化，那么在使用 Agent 框架时，很容易只停留在“会用”的层面，而很难回答一个更关键的问题：

**Agent 开发框架，究竟替我做了哪些事？**

因此，在进入 Agent 系统设计之前，非常有必要先回到最底层，用最原始的 API 调用方式，完整地走一遍模型交互流程。只有当你真正看过 messages 是如何被拼接的、工具调用信息是如何在网络中往返的、上下文是如何一步步增长和裁剪的，再回头看 Agent、Tool Calling、RAG 这些概念时，很多原本抽象的设计都会突然变得具体起来。

本章不会引入任何框架，而是通过三个逐步递进的小实验，亲手跑完一条“最小可理解”的模型交互链路：

* 先完成一次最基本的单轮对话
* 再实现一个手动维护上下文的多轮对话
* 最后跑通一次完整的工具调用流程

示例代码统一使用 JavaScript，这样你可以直接在浏览器开发者工具中，看到真实的请求与响应数据。

## 一、最基本的单轮对话：看清一次请求和一次响应

我们从最简单的情况开始：如何向模型发送一次请求，并拿到一次回答。

在这个阶段，我们只关心一件事：**模型 API 在网络中，究竟长什么样子。**

下面是一段最基础的大模型调用示例，用于演示一次完整的请求与响应流程\*\*（请注意，示例中的 API Key 仅用于结构演示，真实项目中不要写在前端）\*\*：

```javascript
const apiKey = "YOUR_API_KEY";
const baseUrl = "https://api.deepseek.com";
const model = "deepseek-chat";

const response = await fetch(`${baseUrl}/v1/chat/completions`, {
  method: "POST",
  headers: {
    "Content-Type": "application/json",
    "Authorization": `Bearer ${apiKey}`
  },
  body: JSON.stringify({
    model,
    messages: [
      { role: "user", content: "生命的意义是什么？" }
    ]
  })
});

const data = await response.json();
console.log(data.choices[0].message.content);
```

为了更直观地观察请求过程，我们使用  [example1.html](https://github.com/youngyangyang04/JChatMind/blob/dd4f39024c828b5dd0331798c8bf08c70cc79427/examples/example1.html) 页面进行测试：

![1766352126834-feea8126-60c3-49fb-8442-92002a862cd9.png](https://cdn.davidingplus.cn/images/2026/09/11/1766352126834-feea8126-60c3-49fb-8442-92002a862cd9-589505.png)

打开浏览器开发者工具，切换到 Network 面板，你会看到请求体大致如下：

![1765955320365-ed9acea7-6e1e-4789-a6f8-4aca403f9ecf.png](https://cdn.davidingplus.cn/images/2026/09/11/1765955320365-ed9acea7-6e1e-4789-a6f8-4aca403f9ecf-387856.png)![1765955271320-5a1b05b7-57df-4797-ae18-02f6a06b865c.png](https://cdn.davidingplus.cn/images/2026/09/11/1765955271320-5a1b05b7-57df-4797-ae18-02f6a06b865c-414749.png)

```javascript
{
  "model": "deepseek-chat",
  "messages": [
    {
      "role": "user",
      "content": "生命的意义是什么？"
    }
  ]
}
```

这里有一个非常重要的认知点：

**模型看到的，从来不是“当前这一句话”，而是你发送给它的整个 messages。**

在这个示例中，我们只有一条 user 消息，所以 messages 很短。但在真实聊天产品中，之所以能够“记住上下文”，并不是模型在记忆，而是系统在每一轮请求中，把历史对话完整地重新发给了模型。

**再来看响应体。模型返回的内容并不是一个简单字符串，而是被包装在一个标准的 message 结构中：**

```javascript
{
    "id": "e6ca0fc0-a7d0-4504-8d40-0c7ebf2724a7",
    "object": "chat.completion",
    "created": 1765955019,
    "model": "deepseek-chat",
    "choices": [
        {
            "index": 0,
            "message": {
                "role": "assistant",
                "content": "这是一个古老而深刻的问题，无数哲学家、科学家、艺术家和普通人都在探索。没有一个标准答案，但可以从多个角度来思考：\n\n---\n\n### **1. 哲学与宗教视角**\n- **存在主义**（如萨特、加缪）：生命本身没有预设的意义，意义是每个人通过自由选择和行动创造的。就像一块空白的画布，由你赋予色彩。\n- **宗教观点**：许多宗教认为生命的意义与更高的神圣目的相关（如服务上帝、修行解脱、因果轮回等）。\n- **东方哲学**（如佛教、道家）：生命的意义在于超越痛苦、回归自然、体验“道”或达到觉悟。\n\n---\n\n### **2. 科学视角**\n- **进化生物学**：生命是自然演化的产物，其“意义”可能是生存、繁衍、传递基因，并在适应中延续物种。\n- **物理学与宇宙学**：从宇宙尺度看，生命可能是物质复杂化过程中的偶然现象，但人类的存在让宇宙有了自我认知的可能（如“人类是宇宙认识自己的方式”）。\n\n---\n\n### **3. 人文与心理学视角**\n- **追求幸福与连接**：心理学认为意义常来自爱、人际关系、创造、贡献感（如维克多·弗兰克尔的意义疗法）。\n- **自我实现**：马斯洛需求理论中，人类在满足基本需求后，会追求成长、潜能发挥和超越自我。\n\n---\n\n### **4. 个人实践的视角**\n- **创造与影响**：通过艺术、科学、帮助他人，留下对世界积极的痕迹。\n- **体验与成长**：生命的意义可能在于体验世界的美好与痛苦，在过程中成为更完整的自己。\n- **爱与关系**：与家人、朋友、社群的深度连接，往往是意义感的核心来源。\n\n---\n\n### **5. 一个可能的思考框架**\n你可以问自己：\n- **什么让我感到充实和投入？**\n- **我希望被他人如何记住？**\n- **如果生命有限，我最在意的是什么？**\n\n---\n\n### **最后**\n或许生命的意义**不是一个待发现的谜底，而是一个持续构建的过程**。它可能存在于你与世界的互动中，在你选择珍惜的事物里，甚至在你面对虚无时依然选择热爱生活的勇气中。\n\n就像有人问登山者“为什么要登山”，回答可能是：“因为山在那里。”  \n生命的意义，或许就藏在**你赋予它的每一刻真实体验里**。\n\n你如何看待这个问题？你的生活中哪些事物让你感到有意义？ 🌱"
            },
            "logprobs": null,
            "finish_reason": "stop"
        }
    ],
    "usage": {
        "prompt_tokens": 8,
        "completion_tokens": 507,
        "total_tokens": 515,
        "prompt_tokens_details": {
            "cached_tokens": 0
        },
        "prompt_cache_hit_tokens": 0,
        "prompt_cache_miss_tokens": 8
    },
    "system_fingerprint": "fp_eaab8d114b_prod0820_fp8_kvcache"
}
```

这里的 `role` 是 `assistant`，`content` 才是真正的生成文本。这一点非常关键，因为它意味着：**模型的输出，本身就可以被直接追加回 messages，作为下一轮对话的上下文。**

另外两个在工程上非常常用的字段是：

* `finish_reason`：模型为什么结束生成
* `usage`：本次调用消耗的 token 数量

它们在后续的流式输出、工具调用和计费控制中，都会频繁出现。

到这里，你已经完成了第一步：亲眼看清了一次“输入 → 模型 → 输出”的真实数据结构。

> `choices` 是什么？为什么是个数组？
>
> 在当前这个示例中，我们的请求结果被放到了一个被称之为 `choices` 的数组内。
>
> 如果你用过 AI 聊天产品，你可能会碰到，有时候你问 AI 一个问题，它有时候会同时返回两个结果，然后在回复的结尾，问你更喜欢哪个回复。
>
> 所以模型其实会输出一个候选集合，不过我们简单起见，每次我们默认选择数组的第一个结果就可以。

## 二、多轮对话：所谓“记忆”，其实是你在反复重发历史

如果你现在再向模型发送一个新的问题，但不携带任何历史消息，模型并不会知道你之前问过什么。这是很多初学者最容易产生误解的地方。

聊天产品里的“上下文记忆”，并不是模型在记忆，而是系统在做一件非常朴素的事情：**把历史对话整理好，再一次性发给模型。**

下面是一个最常见的多轮对话实现方式，通过一个数组手动维护上下文：

```javascript
let conversationHistory = [];

function addMessage(role, content) {
  conversationHistory.push({ role, content });
}

async function sendMessage(message) {
  addMessage("user", message);

  const response = await fetch(`${baseUrl}/v1/chat/completions`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      "Authorization": `Bearer ${apiKey}`
    },
    body: JSON.stringify({
      model,
      messages: conversationHistory
    })
  });

  const data = await response.json();
  const assistantMessage = data.choices[0].message.content;
  addMessage("assistant", assistantMessage);
}
```

在 [example2.html](https://github.com/youngyangyang04/JChatMind/blob/dd4f39024c828b5dd0331798c8bf08c70cc79427/examples/example2.html) 页面中，你可以直观地看到 messages 随着对话不断增长。

![1766352819186-ca70a348-43ef-4183-b444-428898c03bf6.png](https://cdn.davidingplus.cn/images/2026/09/11/1766352819186-ca70a348-43ef-4183-b444-428898c03bf6-947358.png)

此时你应该能够得出一个非常重要的结论：

**多轮对话的“记忆感”，完全来自系统对 messages 的管理，而不是模型本身的能力。**

理解了这一点，你就已经在为后面的 Agent、Tool Calling 打基础了。因为所有更复杂的行为，本质上都是在 messages 之上演进的。

## 三、role 的意义：它不是给人看的，而是给模型看的

每一条 message 看起来都很简单：

```javascript
{ "role": "...", "content": "..." }
```

但 role 并不是一个展示标签，而是模型理解上下文时最关键的信号。

常见的 role 包括 user、assistant、system 和 tool。

* user 表示用户输入，assistant 表示模型输出，它们共同构成对话历史。
* system 用来描述全局规则、角色设定或行为约束，它更像是一段背景说明，而不是一次发言。
* tool 则是工具执行结果的专用角色，用来告诉模型：“这段内容不是用户说的，而是某个工具的真实执行结果。”

这一点非常重要。如果你把工具执行结果当作 user 消息发回模型，模型很可能会直接理解错上下文，甚至报错。

理解 role 的真正用途之后，你会发现：**多轮对话、工具调用、Agent Loop，其实都是围绕 messages 和 role 在不断升级。**

## 四、工具调用：模型负责判断，系统负责执行

接下来，我们通过一个查询天气的示例，跑通一次完整的工具调用链路。

当用户问“今天我这里的天气怎么样”时，模型本身并不知道你的城市、不知道今天的日期，也无法访问真实天气接口。这种问题，不可能通过一次生成直接完成。

因此，我们需要为系统定义一组“工具”，并把这些能力告诉模型。

```javascript
const tools = [
  {
    type: "function",
    function: {
      name: "get_current_date",
      description: "查询今天的日期",
      parameters: { type: "object", properties: {}, required: [] }
    }
  },
  {
    type: "function",
    function: {
      name: "get_current_city",
      description: "查询当前所在的城市",
      parameters: { type: "object", properties: {}, required: [] }
    }
  },
  {
    type: "function",
    function: {
      name: "get_weather",
      description: "根据日期和城市查询天气信息",
      parameters: {
        type: "object",
        properties: {
          date: { type: "string" },
          city: { type: "string" }
        },
        required: ["date", "city"]
      }
    }
  }
];
```

在请求中，将 tools 一并发送给模型，并允许模型自动决定是否调用工具：

```javascript
body: JSON.stringify({
  model,
  messages,
  tools,
  tool_choice: "auto"
})
```

![1766353307244-26513952-3b6f-4d9c-b7df-babcac936e9c.png](https://cdn.davidingplus.cn/images/2026/09/11/1766353307244-26513952-3b6f-4d9c-b7df-babcac936e9c-793452.png)

模型在第一次响应中，并不会直接回答天气，而是返回它“希望调用的工具”。

系统解析这些 tool\_calls，执行对应的真实代码，并把执行结果以 **tool message** 的形式追加到 messages 中，再次发送给模型。

![1766353380829-077d09fc-f01a-4364-a7c6-1ca62aac6ac1.png](https://cdn.davidingplus.cn/images/2026/09/11/1766353380829-077d09fc-f01a-4364-a7c6-1ca62aac6ac1-215887.png)

模型拿到新的信息后，继续判断下一步需要什么工具。

这个过程往往会发生多次，直到信息足够，模型才会生成最终的自然语言回答。

在 [example3.html](https://github.com/youngyangyang04/JChatMind/blob/dd4f39024c828b5dd0331798c8bf08c70cc79427/examples/example3.html) 中，你可以完整看到这一往返过程。

如果你回头看整个流程，会发现一件非常关键的事情：

* 模型从头到尾都没有“直接做事”
* 它只是不断判断：现在还缺什么信息
* 每一步缺失的信息，都由系统通过工具补齐

这正是工具调用的本质协作方式：

**模型负责决策，系统负责执行，而 messages 记录全过程。**

## 五、到这里，你已经手写了一个“最小 Agent”

虽然我们还没有定义 Agent 类，也没有引入任何框架，但你已经亲手跑完了一条具备以下特征的流程：

* 有明确目标
* 有中间状态
* 能根据结果继续推进
* 能与真实系统交互

这已经不再是一次简单的问答，而是一个**最小可运行的 Agent 行为闭环**。

当你理解了这一点，再回头看 Spring AI 这类框架时，就会发现它们并不是在“让模型更聪明”，而是在帮你把这些步骤系统化、标准化、可观测化。

## 六、为什么这一步是 Agent 开发的必经之路

在这一章中，我们刻意绕开了所有框架，只用最原始的 API，亲手走完了一次模型交互的完整链路。

你已经清楚地看到：

* 上下文不是模型记住的，而是系统拼接的
* 工具不是模型执行的，而是系统代劳的
* 多次请求的往返，本身就构成了任务推进的最小循环

当这些认知真正建立起来之后，Agent 系统就不再神秘了。

接下来要做的事情也非常自然：当任务更复杂、工具更多、对话更长时，我们该如何控制这个循环，避免失控，并让它在工程上可维护？


> 更新: 2026-03-04 17:32:32  
> 原文: <https://www.yuque.com/chengxuyuancarl/zsqzgh/vhuo40gq26q7agl5>