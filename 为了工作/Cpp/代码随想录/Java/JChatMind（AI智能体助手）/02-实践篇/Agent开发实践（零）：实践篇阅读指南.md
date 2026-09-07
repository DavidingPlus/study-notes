# Agent 开发实践（零）：实践篇阅读指南

本章节用于说明【Agent 开发实践篇】的整体结构。

本文档的核心关注点在 **Agent 相关设计与实现**，系统中常规的 CRUD、业务接口与页面逻辑不作为重点内容进行展开。

**<font style="color:#DF2A3F;">文档主要在于讲解实现思路，并附核心代码讲解，其余具体实现细节请参考项目源码。</font>**

## 第一节：从最原始的大模型调用开始

[Agent 开发实践（一）：从最原始的大模型调用开始](https://www.yuque.com/chengxuyuancarl/zsqzgh/vhuo40gq26q7agl5)

第一节是用于介绍从最基础的 API 交互之间数据流转。

**这一节是 【Agent 开发实践篇】最重要的一节。**

本节通过 `example1.html`、`example2.html`、`example3.html` 三个示例。

依次展示：

* 基础聊天请求与响应的组织方式
* 多角色消息在对话中的结构与作用
* 工具调用的定义、触发与结果回传过程

理解了上述内容，就已经拥有不依靠框架，自行开发一个 Agent 系统的能力了。比如 OpenManus 那样的项目。

## 第二节：开发环境搭建

[Agent 开发实践（二）：开发环境搭建](https://www.yuque.com/chengxuyuancarl/zsqzgh/zwu0wpdaf61g137l)

本节内容主要围绕运行项目所需的软件版本、依赖组件及基础配置展开，不涉及系统内部实现逻辑，可作为环境准备与项目启动的参考。

## 第三节：数据模型设计

[Agent 开发实践（三）：数据模型设计](https://www.yuque.com/chengxuyuancarl/zsqzgh/bldffmrd9q6giplm)

第三节介绍系统中与 Agent 运行相关的数据库结构设计。

内容包括：

* Agent、会话、消息等核心数据结构的设计方式
* Agent 运行状态与上下文信息的存储形式
* 后续 RAG 与工具能力在数据层面的支撑结构

本节重点在于说明设计思路，而非数据库操作细节。

## 第四节：Spring AI 集成与多模型支持

[Agent 开发实践（四）：Spring AI 集成与多模型支持](https://www.yuque.com/chengxuyuancarl/zsqzgh/iaqwg8sehg56zauh)

本节关注点包括：

* 模型能力的抽象方式
* 多模型共存与切换的设计思路
* 系统对具体模型实现的解耦方式

该部分不针对某一具体模型展开，而是介绍整体集成策略。

## 第五节 + 第六节

[Agent 开发实践（五）：实现带记忆的聊天功能](https://www.yuque.com/chengxuyuancarl/zsqzgh/gwuggsb0edao1wyy)

[Agent 开发实践（六）：Agent Loop 的第一次落地](https://www.yuque.com/chengxuyuancarl/zsqzgh/yrwv7f5ywgotbubl)

第五节与第六节通过分析 `JChatMindV1.java` 与 `JChatMindV2.java`，介绍 Agent 的核心运行流程和基础聊天实现。

在这一部分中，有意弱化 `JChatMind` 与 `JChatMindFactory` 中较为复杂的封装与配置逻辑，仅关注：

* Agent 的初始化过程
* Agent Loop 的执行结构
* 聊天流程与工具调用在 Loop 中的位置

## 第七节：引入知识库和 RAG

[Agent 开发实践（七）：引入知识库与 RAG](https://www.yuque.com/chengxuyuancarl/zsqzgh/ud4okzhfzhvhpz1c)

第七节用于介绍系统中 RAG（Retrieval-Augmented Generation）的实现方式。

内容包括：

* 知识检索能力在系统中的位置
* RAG 如何作为 Agent 的一种外部能力被调用
* 向量检索与结果注入的整体流程

## 第八节：一次完整对话是如何跑完的

[Agent 开发实践（八）：一次完整对话是如何跑完的](https://www.yuque.com/chengxuyuancarl/zsqzgh/xlhprbyxlq8b27p8)

第八节对系统中的一次完整交互流程进行整体分析。

该流程涵盖：

* 前端请求的发起
* 后端消息创建与事件触发
* Agent 执行过程
* 执行结果返回前端的方式

用于从系统整体视角理解 Agent 与前端之间的协作关系。


> 更新: 2025-12-23 19:18:10  
> 原文: <https://www.yuque.com/chengxuyuancarl/zsqzgh/yqge43oabap0392z>