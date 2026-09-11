# Agent 开发实践（二）：开发环境搭建

## 一、开发环境要求

在开始构建 **JChatMind** 之前，请确保你的本地环境满足以下要求。

### 1、JDK 17+

* 推荐版本：**JDK 17（LTS）**
* 原因：
  * Spring Boot 3.x 的最低要求（需要支持 Spring AI）

### 2、PostgreSQL 14+

> Windows 下安装 pgvector 插件比较困难，建议 Windows 用户通过 docker 使用 PostgreSQL。
>
> docker 拉取 pgvector 的 image 如果因为网络问题失败，推荐使用这个镜像网站： <https://docker.aityp.com/>

* 作为核心数据库
* 用于：
  * 会话数据
  * Agent 状态
  * RAG 知识库
  * 向量检索（pgvector）
* 原因：
  * 需要支持向量检索，MySQL 不支持向量，所以采用 PostgreSQL

SQL文件下载，在链接的附件里：<https://t.zsxq.com/QOwB0>

### 3、IDE（推荐）

以下任意一种即可：

* **IntelliJ IDEA（推荐）**
  * 对 Spring Boot / Maven / SQL / Debug 支持最好
  * 自带 maven
* **Cursor**
  * 适合边写代码边和 Agent 协作

### 4、NodeJS 22+

* 用于前端界面（React / Vite）

### 5、Ollama

* 用于本地运行 embedding 模型
* 主要用途：
  * 向量生成（Embedding）

### 6、mitmproxy（可选）

* 用途：
  * 抓取 Java 发出的 HTTP 请求
  * 查看真实的：
    * Prompt 内容
    * Tool Calling 参数
    * 模型返回结构
* 特别适合：
  * 调试 Agent 决策异常
  * 理解 Spring AI / 模型 SDK 的真实行为

### 2. Docker（可选）

* 用途：
  * 快速启动 PostgreSQL
  * 部署 pgvector

## 二、配置步骤

### 下载源码

```bash
git clone git@github.com:youngyangyang04/JChatMind.git
```

### 创建数据库

```sql
CREATE DATABASE jchatmind;
```

### 建表

```sql
\i /absolute/path/to/jchatmind.sql
\i /absolute/path/to/eshop.sql
\i /absolute/path/to/eshop_data.sql
```

数据库脚本说明：

* <code>**jchatmind.sql**</code>\
  项目的**主业务数据库脚本**，用于创建并维护 JChatMind 系统自身所需的核心表结构（如 Agent、会话、消息、知识库等）。
* <code>**eshop.sql**</code>\
  专门用于 **AI 测试与演示 SQL 能力的示例数据库结构**。\
  出于多数据源配置复杂度的考虑，`eshop` 并未单独部署数据库实例，而是**在物理层面与 **<code>**jchatmind**</code>** 共用同一个 PostgreSQL 数据库**。\
  在逻辑层面，通过统一的 <code>**t_**</code>\*\* 前缀\*\* 对 `eshop` 相关表进行区分，以避免与主业务表产生命名冲突。
* <code>**eshop_data.sql**</code>\
  `eshop` 示例数据库的**模拟业务数据脚本**，用于为 AI 生成、执行和分析 SQL 提供可操作的数据环境。

建表结果最终如下：

![1766483416686-0314cb03-207a-4d68-9b7e-f7cbcbd21cd9.png](https://cdn.davidingplus.cn/images/2026/09/11/1766483416686-0314cb03-207a-4d68-9b7e-f7cbcbd21cd9-039235.png)

### 修改配置

编辑`jchatmind/src/main/resources/application.yaml` 配置文件

#### 配置数据库连接

```yaml
spring:
  datasource:
    url: jdbc:postgresql://localhost:5432/jchatmind
    username: postgres
    password: 123456  # 修改为你的密码
    driver-class-name: org.postgresql.Driver
```

#### 配置模型的 API Key

```yaml
spring:
  ai:
    deepseek:
      api-key: your-deepseek-api-key  # 替换为你的 DeepSeek API Key
      base-url: https://api.deepseek.com
      chat:
        options:
          model: deepseek-chat
    zhipuai:
      api-key: your-zhipuai-api-key    # 替换为你的智谱AI API Key
      base-url: https://open.bigmodel.cn/api/paas
      chat:
        options:
          model: glm-4.6
```

**怎么获取 API Key？**

* **DeepSeek**: <https://platform.deepseek.com/>
* **智谱 AI**：[https://open.bigmodel.cn](https://open.bigmodel.cn/)

#### 配置邮件服务（QQ 邮箱）

```yaml
spring:
  mail:
    host: smtp.qq.com
    port: 587
    username: your-email@qq.com
    password: your-email-password  # QQ 邮箱授权码
    default-encoding: UTF-8
    properties:
      mail:
        smtp:
          auth: true
          starttls:
            enable: true
            required: true
```

### 本地部署 bge-m3 模型

拉取模型：

```sql
ollama pull bge-m3
```

查看拉取结果：

```sql
ollama list
```

拉取成功结果：

```sql
NAME             ID              SIZE      MODIFIED
bge-m3:latest    790764642607    1.2 GB    3 weeks ago
```

## 三、启动项目

### 启动前端

#### 下载依赖

```bash
cd JChatMind/ui
npm install
```

#### 运行项目

```bash
npm run dev
```

### 启动后端

#### 命令行方式

```bash
cd JChatMind/jchatmind
mvn spring-boot:run
```

#### UI 界面

略


> 更新: 2026-03-25 09:21:35  
> 原文: <https://www.yuque.com/chengxuyuancarl/zsqzgh/zwu0wpdaf61g137l>