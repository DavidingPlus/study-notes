# Agent 开发实践（四）：Spring AI 集成与多模型支持

在前面的章节中，我们已经通过手写大模型 API 的方式，完整理解了模型调用、上下文管理和工具调用的真实流程。\
从这一章开始，我们正式进入工程阶段。

问题也随之发生了变化：

当系统中不止有一次模型调用、不止一个 Agent、不止一个模型时，我们该如何把模型能力接入系统，并让它成为一种**可配置、可切换、可扩展的基础设施**？

这一章要解决的，正是这个问题。

## 一、Spring AI 在 Agent 系统中的角色

在没有框架的情况下，每接入一个模型，你都需要自己处理：

* HTTP 请求构造
* 鉴权与重试
* 不同模型的参数差异
* 工具调用协议差异

这些工作并不困难，但**它们不属于 Agent 的核心逻辑**。

Spring AI 的价值在于它把“如何和模型打交道”这件事，抽象成了一套统一接口，让你可以把精力放在 **Agent 的行为设计** 上，而不是模型厂商的细节上。

在 Agent 系统中，我们只关心一件事：**给我一个可以用来和模型对话的对象。**

**而这个对象就是** Spring AI 中 `ChatClient`。

## 二、Spring AI 的接入方式

在 Spring Boot 项目中，接入 Spring AI 非常简单。

首先，通过 BOM 管理版本，避免依赖冲突：

```xml
<dependencyManagement>
  <dependencies>
    <dependency>
      <groupId>org.springframework.ai</groupId>
      <artifactId>spring-ai-bom</artifactId>
      <version>1.1.0</version>
      <type>pom</type>
      <scope>import</scope>
    </dependency>
  </dependencies>
</dependencyManagement>
```

然后，根据不同的厂商，引入不同的 starter。

> deepseek 和 智谱 AI 的 starter 案例：

```xml
<dependencies>
    <dependency>
        <groupId>org.springframework.ai</groupId>
        <artifactId>spring-ai-starter-model-deepseek</artifactId>
    </dependency>

    <dependency>
        <groupId>org.springframework.ai</groupId>
        <artifactId>spring-ai-starter-model-zhipuai</artifactId>
    </dependency>
</dependencies>
```

最后，在 `application.yaml` 中配置模型参数：

```yaml
spring:
  ai:
    deepseek:
      api-key: ${DEEPSEEK_API_KEY}
      chat:
        options:
          model: deepseek-chat

    zhipuai:
      api-key: ${ZHIPU_API_KEY}
      chat:
        options:
          model: glm-4.6
```

到这里，模型已经被 Spring AI 自动装配成了 `ChatModel` Bean。

接下来，我们需要在系统里面使用模型。

## 三、一个模型 = 一个 ChatClient

在 Agent 系统中，我们约定，每一个可用的模型，对应一个 ChatClient 对象。

ChatClient 是对“模型能力”的封装，它屏蔽了模型调用的细节，只暴露统一的对话接口。

我们为每个模型创建一个独立的 ChatClient Bean：

```java
@Configuration
public class MultiChatClientConfig {

    @Bean("deepseek-chat")
    public ChatClient deepSeekChatClient(DeepSeekChatModel model) {
        return ChatClient.create(model);
    }

    @Bean("glm-4.6")
    public ChatClient zhiPuChatClient(ZhiPuAiChatModel model) {
        return ChatClient.create(model);
    }
}
```

## 四、 ChatClientRegistry：模型调度的核心

当系统中存在多个 ChatClient 后，下一个问题自然出现了，在运行时，如何根据 Agent 的配置，选择正确的模型？

答案是：**不要写 if / switch，而是交给 Spring。**

![1766644072294-8366adc2-5623-4d0a-832c-35a08c17e2da.png](./img/K6O2KxVwenEnDvLf/1766644072294-8366adc2-5623-4d0a-832c-35a08c17e2da-877307.png)

Spring 会自动把所有 `ChatClient` 类型的 Bean 注入到一个 Map 中，Key 为 Bean 名称，Value 为实例本身。

```java
@Component
public class ChatClientRegistry {

    private final Map<String, ChatClient> registry;

    public ChatClientRegistry(Map<String, ChatClient> registry) {
        this.registry = registry;
    }

    public ChatClient get(String modelName) {
        return registry.get(modelName);
    }
}
```

这一小段代码，实际上解决了一个非常重要的工程问题：

* 新增模型，不需要改任何调度代码
* 模型注册是声明式的，而不是控制流式的
* 模型选择完全由数据驱动

在 Agent 系统中，这种“注册表模式”是多模型支持的核心。

## 五、Agent 如何绑定模型能力

在前面的数据库设计中，Agent 表中包含一个 `model` 字段，用来描述该 Agent 默认使用的模型。

当系统真正创建 Agent 的运行实例时，会发生这样一件事：

```java
ChatClient chatClient = chatClientRegistry.get(agent.getModel());

if (chatClient == null) {
    throw new IllegalStateException(
        "未找到对应的模型：" + agent.getModel()
    );
}
```

到这里，我们完成了模型的动态切换能力：

* Agent 只依赖 `ChatClient` 接口
* Agent 完全不知道模型来自哪家厂商
* 模型的切换，不需要修改 Agent 代码

**模型已经从“写死的依赖”，变成了运行时注入的能力。**

## 六、模型已经成为系统基础设施

到这一章为止，我们完成了一件非常关键的事情：

* 模型被统一抽象为 ChatClient
* 多模型可以在系统中并存
* Agent 可以在运行时自由选择模型
* 模型接入与 Agent 行为彻底解耦

此时，模型已经不再是“某个 API Key 对应的外部服务”，而是系统中的一种**基础能力组件**。


> 更新: 2025-12-25 17:17:50  
> 原文: <https://www.yuque.com/chengxuyuancarl/zsqzgh/iaqwg8sehg56zauh>