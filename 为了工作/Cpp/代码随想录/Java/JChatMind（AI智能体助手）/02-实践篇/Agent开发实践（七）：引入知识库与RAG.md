# Agent 开发实践（七）：引入知识库与 RAG

通过前面的 JChatMindV1 和 JChatMindV2 版本的代码，我们已经完成了以下功能：

* 可以通过工具调用，与真实系统发生交互
* 不再只执行一次模型调用，而是运行在一个可控的 Agent Loop 中

但很快就会遇到一个新的问题，很多任务并不是缺一个“实时数据”，而是缺一段**系统内部知识**，例，项目文档、设计说明、业务规则、数据库结构说明等。

这些信息既不适合写死在提示词里，也不可能指望模型“凭记忆猜对”，它们更像是一本**只属于当前系统的说明书**。

RAG（Retrieval Augmented Generation）的作用，正是在这里显现出来的。

我们前面说过：**RAG 不是模型能力，而是系统能力。**

模型本身并不会“学会”你的文档，它只是被系统在合适的时机，提供了一小段**与当前问题最相关的背景信息**。

因此，在 JChatMind 中，我们并没有把 RAG 设计成某种特殊的模型模式，而是**把 RAG 当成一个工具（Tool）。**

在 Agent 的世界里，RAG 和“查天气”“查日期”没有本质上的区别：

* 都是模型发现自己不知道
* 都是通过工具向系统请求信息
* 都是拿到结果后继续推理

## 一、Agent 视角下的一次 RAG 调用

从 Agent 的角度看，一次完整的 RAG 流程如下：

```plain
用户提出问题
  ↓
Agent 判断：我是否需要查阅知识库？
  ↓
Agent 调用 KnowledgeTool
  ↓
系统执行向量检索
  ↓
返回相关文档内容
  ↓
Agent 基于内容生成最终回答
```

Agent 最关心的一件事，是**现在缺的背景信息。**

## 二、知识库与 RAG 两条主线

本章内容可以简单划分为两条主线：

* 将内容放进知识库
* 从知识库中搜索内容

![1766664366460-f7b46457-5bdf-45fc-a6e4-623de0d57cd8.png](./img/VwoiGMa_U9ZMV70m/1766664366460-f7b46457-5bdf-45fc-a6e4-623de0d57cd8-220461.png)

### 将内容放入知识库流程

* 将文档进行拆分
* 拆分后的每一个 chunk，送入**向量模型**（图中以 `bge-m3` 为例）进行 embedding
* 向量化后的结果统一存储到向量数据库中

至此，**知识库构建完成**。模型本身并没有“记住”这些知识，但系统已经具备了**随用随取的能力**。

### 从知识库查询内容流程

* 用户提问（往往缺少背景信息）
* 对用户问题进行 Embedding
* 使用用户问题的向量在向量数据库中执行相似度检索
* 检索到的 `Top-K` 条 chunk，会被作为**补充上下文**，与用户原始问题一起发送给大模型
* 模型基于用户问题和检索到的知识片段生成最终回答并返回给用户。

这一步的本质是：**RAG 负责“找对信息”，AI 负责“说清楚信息”。**

## 三、为什么文档必须进行拆分

> **不要一上来就做 embedding，先把文档结构搞清楚。**

在真实系统中，知识库往往由完整文档组成，如果我们直接把整篇文档交给模型，会立刻遇到几个现实问题：

* 文档可能很长，超过上下文限制
* 每次问题只和其中很小一部分相关
* token 成本和干扰信息都不可控

因此，第一步必须是**拆文档**。

拆分文档的方式很多：

* 按照段落拆
* 按照固定长度拆
* .....

在 JChatMind 中，我们采用了一种比较稳定的方法：**按 Markdown 的章节标题拆分。**

每一个一级标题，对应一个可被独立检索的语义单元。

> 这也是 RAG 麻烦的地方，需要应对不同的文档，采用不同的拆分策略。因为当前的项目的定位是入门级项目，我们预设文档的格式后，针对格式进行拆分，体验 RAG 的整个过程就好。

我们准备的知识库的文档是这种格式的（eshop.md，在知识星球内部获取）

````markdown
# 电商系统（eshop）数据库设计文档（PostgreSQL）

> 说明：
>
> * 主键统一使用 `UUID + gen_random_uuid()`
> * 所有时间字段使用 `TIMESTAMPTZ`
> * 文本型扩展信息统一使用 `JSONB`
> * 评论相关表：`t_comment` / `t_comment_topic` / `t_comment_topic_mapping` / `t_comment_summary_daily`

## 1. 设计目标

1. 支撑电商核心业务：用户、商品、订单、支付、发货。
2. 支持用户对商品的评论与多维度分析（评价内容、评分等）。

## 2. 表清单

| 表名                          | 说明                 |
| --------------------------- | ------------------ |
| `t_app_user`                  | 用户表                |
| `t_role`                      | 角色表（管理员、客服等）       |
| `t_user_role`                 | 用户-角色关联表           |
| `t_product_category`          | 商品类目               |
| `t_product`                   | 商品主表               |
| `t_product_category_relation` | 商品-类目多对多关联         |
| `t_order_header`              | 订单主表               |
| `t_order_item`                | 订单明细               |
| `t_payment`                   | 支付记录               |
| `t_shipment`                  | 发货记录               |
| `t_comment_topic`             | 评论话题定义（“物流”、“质量”等） |
| `t_comment`                   | **评论表，核心分析对象**     |
| `t_comment_topic_mapping`     | 评论-话题关联表（多对多）      |
| `t_comment_summary_daily`     | 评论日汇总表，用于加速统计      |
| `t_system_kv`                 | 系统配置 / 开关表         |

---

## 3. 表结构设计（分表说明 + DDL）

### 3.1 用户表 `t_app_user`

**用途**：
存储系统用户（买家、卖家、运营、客服等）的基础信息。

**字段要点**：

* `id`: 用户主键 UUID
* `email`: 登录邮箱，唯一
* `phone`: 手机号
* `display_name`: 显示昵称
* `status`: 用户状态（active / blocked / deleted）
* `metadata`: 扩展信息（渠道、终端等）

```sql
CREATE TABLE t_app_user (
    id              UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    email           VARCHAR(255) UNIQUE NOT NULL,
    phone           VARCHAR(32),
    display_name    VARCHAR(128) NOT NULL,
    password_hash   VARCHAR(255) NOT NULL,
    status          VARCHAR(32) NOT NULL DEFAULT 'active', -- active / blocked / deleted
    metadata        JSONB,
    created_at      TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at      TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

COMMENT ON TABLE t_app_user IS '用户表：存储系统用户的基础信息';
COMMENT ON COLUMN t_app_user.email IS '登录邮箱，唯一';
COMMENT ON COLUMN t_app_user.display_name IS '用户显示昵称';
COMMENT ON COLUMN t_app_user.status IS '用户状态：active / blocked / deleted';
````

### 3.2 角色表 `t_role`

**用途**：

定义系统角色，如管理员、客服、运营等。

```sql
CREATE TABLE t_role (
    id          UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    code        VARCHAR(64) UNIQUE NOT NULL,
    name        VARCHAR(128) NOT NULL,
    description TEXT,
    created_at  TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at  TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

COMMENT ON TABLE t_role IS '角色表：定义系统中的角色（管理员、客服等）';
```

````

像这样的文档格式一般如下：

![1766472628540-4eea0098-1f8c-4cd5-9b5f-409b399c86e3.png](./img/VwoiGMa_U9ZMV70m/1766472628540-4eea0098-1f8c-4cd5-9b5f-409b399c86e3-674741.png)

每篇文档都可以为拆分一块块的内容。

每一块又分为两个部分：

+ 标题
+ 内容

就像下面这个示例一样。

```markdown
### 3.1 用户表 `t_app_user`

**用途**：
存储系统用户（买家、卖家、运营、客服等）的基础信息。

**字段要点**：

* `id`: 用户主键 UUID
* `email`: 登录邮箱，唯一
* `phone`: 手机号
* `display_name`: 显示昵称
* `status`: 用户状态（active / blocked / deleted）
* `metadata`: 扩展信息（渠道、终端等）

```sql
CREATE TABLE t_app_user (
    id              UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    email           VARCHAR(255) UNIQUE NOT NULL,
    phone           VARCHAR(32),
    display_name    VARCHAR(128) NOT NULL,
    password_hash   VARCHAR(255) NOT NULL,
    status          VARCHAR(32) NOT NULL DEFAULT 'active', -- active / blocked / deleted
    metadata        JSONB,
    created_at      TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at      TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

COMMENT ON TABLE t_app_user IS '用户表：存储系统用户的基础信息';
COMMENT ON COLUMN t_app_user.email IS '登录邮箱，唯一';
COMMENT ON COLUMN t_app_user.display_name IS '用户显示昵称';
COMMENT ON COLUMN t_app_user.status IS '用户状态：active / blocked / deleted';
````

标题：用户表 `t_app_user`

内容：用户表的详情。

按照这种拆分方式，每篇文档我们都拆分成下面的结构：

![1766472768649-38282004-0887-45c4-a7a7-4ebe51c75e6a.png](./img/VwoiGMa_U9ZMV70m/1766472768649-38282004-0887-45c4-a7a7-4ebe51c75e6a-367541.png)

然后我们再将标题进行 embedding 处理，通过向量模型将其转成向量，保持文本不变：

![1766472898168-a1e62e97-bb2f-4c60-830f-b8dbcafba659.png](./img/VwoiGMa_U9ZMV70m/1766472898168-a1e62e97-bb2f-4c60-830f-b8dbcafba659-184537.png)

然后我们将标题的向量和标题下的内容存入 chunk\_bge\_m3 表中：

embedding 字段存储向量，content 存文本。

具体实现方式是我们通过解析 AST 的方式，提取所有标题，并将每个标题下面的内容视为一个完整章节。

简化后的逻辑可以概括为：

* 找到所有标题节点
* 从当前标题开始，收集直到下一个标题之间的内容
* 得到一组 `(title, content)` 对

这样拆出来的每一块，都具有非常明确的语义边界，它们既不会太碎，也不会太大，恰好适合作为 RAG 的最小单元。

```java
public interface MarkdownParserService {
    /**
     * 解析 Markdown 文件，提取标题和对应的内容
     */
    List<MarkdownSection> parseMarkdown(InputStream inputStream);
    
    @Data
    @AllArgsConstructor
    @ToString
    class MarkdownSection {
        private String title;
        private String content;
    }
}
```

解析入口：

```java
public List<MarkdownSection> parseMarkdown(InputStream inputStream) {
    try {
        // 读取文件内容
        originalMarkdownContent = new String(inputStream.readAllBytes(), StandardCharsets.UTF_8);
        
        // 解析 Markdown
        Document document = parser.parse(originalMarkdownContent);
        
        // 提取标题和内容
        List<MarkdownSection> sections = new ArrayList<>();
        extractSections(document, sections);
        
        log.info("解析 Markdown 完成，共提取 {} 个章节", sections.size());
        return sections;
    } catch (Exception e) {
        log.error("解析 Markdown 失败", e);
        throw new RuntimeException("解析 Markdown 失败: " + e.getMessage(), e);
    }
}
```

对 titie 生成 embedding，并持久化到数据库中：

```java
for (MarkdownParserService.MarkdownSection section : sections) {
    String title = section.getTitle();
    String content = section.getContent();

    if (title == null || title.trim().isEmpty()) {
        continue;
    }

    // 对标题进行 embedding
    float[] embedding = ragService.embed(title);

    // 创建 ChunkBgeM3 实体
    ChunkBgeM3 chunk = ChunkBgeM3.builder()
            .kbId(kbId)
            .docId(documentId)
            .content(content != null ? content : "")
            .metadata(null) // 可以存储标题信息到 metadata
            .embedding(embedding)
            .createdAt(now)
            .updatedAt(now)
            .build();

    chunkBgeM3Mapper.insert(chunk);
}
```

简单看一下 `ragService.embed()` 方法，和调用 LLM 类似，都是发送一个 HTTP 请求，然后接受调用结果。

在这里我们是调用本地部署的 bge-m3 模型，将 title 转成向量：

```java
private final WebClient webClient;

@Override
public float[] embed(String text) {
    return doEmbed(text);
}

private float[] doEmbed(String text) {
    EmbeddingResponse resp = webClient.post()
            .uri("/api/embeddings")
            .bodyValue(Map.of(
                    "model", "bge-m3",
                    "prompt", text
            ))
            .retrieve()
            .bodyToMono(EmbeddingResponse.class)
            .block();
    Assert.notNull(resp, "Embedding response cannot be null");
    return resp.getEmbedding();
}
```

## 四、为什么只对「标题」生成 Embedding

我们并没有对完整内容生成向量，而是只对**章节标题**生成 Embedding：

而章节的具体内容，则原样存储，用于最终返回给 Agent。

这个设计背后，有几个非常现实的考虑。

1. 标题本身就是作者对这一段内容的高度概括，它天然就是一段“语义标签”。
2. 标题长度稳定、token 数量少，Embedding 成本低且效果稳定。
3. 真正需要交给模型“阅读”的，是内容，而不是标题。

换句话说：\*\*标题负责“找”，内容负责“看”，\*\*这是一个非常适合 Agent 使用场景的折中方案。

最终的向量并没有被存入某个独立的向量数据库，而是直接使用 PostgreSQL + pgvector：

```plain
embedding VECTOR(1024) NOT NULL
```

这样做的原因并不复杂：

* 不需要引入额外的基础设施
* 向量数据和业务数据在同一数据库中
* 查询路径清晰、可调试、可观测

通过 pgvector 提供的 `<->` 操作符，我们可以非常直接地完成 Top-K 相似度搜索。

## 五、相似度搜索的核心逻辑

一次完整的相似度搜索流程如下：

1. 将查询文本生成 Embedding
2. 使用该向量在指定知识库范围内执行相似度排序
3. 返回最相关的若干个章节内容

对应的 SQL 查询也非常直观：

```sql
<select id="similaritySearch" resultMap="BaseResultMap">
    <![CDATA[
    SELECT id,
           kb_id,
           doc_id,
           content,
           metadata,
           embedding,
           created_at,
           updated_at
    FROM chunk_bge_m3
    WHERE kb_id = CAST(#{kbId} AS uuid)
    ORDER BY embedding <-> #{vectorLiteral}::vector
    LIMIT #{limit}
    ]]>
</select>
```

在这个过程中，embedding 只是检索的“索引键”，真正返回给 Agent 的，始终是原始文档内容。

## 六、KnowledgeTool：RAG 在 Agent 世界中的入口

Agent 并不会直接调用 `RagService`，它只认识一个工具：

```java
@Tool(
    name = "KnowledgeTool",
    description = "从知识库中执行语义检索"
)
public String knowledgeQuery(String kbsId, String query) {
    List<String> results = ragService.similaritySearch(kbsId, query);
    return String.join("\n", results);
}
```

为了让 Agent 在 Think 阶段能够正确判断是否要使用 RAG，我们会在提示词中明确告诉它：

* 当前允许访问哪些知识库
* 每个知识库的大致用途

这些信息并不会被模型“记住”，但会在当前决策中起到非常关键的引导作用。

一旦模型意识到：“这个问题我可以通过知识库解决”，它就会主动选择调用 `KnowledgeTool`。


> 更新: 2025-12-25 20:32:55  
> 原文: <https://www.yuque.com/chengxuyuancarl/zsqzgh/ud4okzhfzhvhpz1c>