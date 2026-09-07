# Agent 开发实践（三）：数据模型设计

在前面的章节中，我们已经从概念和最小实践层面，完整走过了一次 Agent 的工作方式：模型负责生成与判断，系统负责执行与组织，而 Agent 的价值，体现在“如何把一次次模型调用，组织成一个可推进的过程”。

当这些行为真正落地到工程中时，就会遇到一个无法回避的问题：

**这些状态，存在哪里？**

## **一、先划清三类数据：这是整个设计的起点**
在看具体表结构之前，我们先明确一个非常重要的设计前提。在 Agent 系统中，所有需要持久化的数据，基本都可以归为三类：

第一类，是Agent 与对话状态本身，它们描述的是：谁在和模型交互，这次对话进行到哪一步了。

第二类，是对话过程中产生的消息记录，包括用户输入、模型输出、工具执行结果，以及中间的各种元信息。

第三类，是 Agent 需要“按需查阅”的系统知识。这些知识不属于模型本身，而是通过 RAG 的方式，在需要时提供给模型。

## 二、agent 表
```sql
CREATE TABLE agent (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    
    name TEXT NOT NULL,                    -- Agent 名称
    description TEXT,                      -- 描述（用户可见）
    system_prompt TEXT,                    -- 系统指令
    model TEXT,                            -- 默认使用的模型
    allowed_tools JSONB,                   -- 允许使用的工具列表
    allowed_kbs JSONB,                     -- 允许访问的知识库
    chat_options JSONB,                    -- 其它配置项（温度、top_p、最大token）
    
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);
```

在很多简单示例中，Agent 往往只是代码里的一个类，system prompt 写死在代码里，能用哪些工具、访问哪些知识，也全靠开发者约定。

但只要系统稍微复杂一点，这种方式就会立刻失控。

因此，这套设计的第一步，就是**把 Agent 本身从代码中抽离出来**，变成一条明确的数据记录。

Agent 表只描述三类东西：  
它是谁（name、description），  
它的行为基调是什么（system_prompt），  
以及它的能力边界在哪里（model、allowed_tools、allowed_kbs、chat_options）。

这些信息都是“长期稳定的配置”，而不是运行时状态。  
这样做的好处是：Agent 可以被创建、修改、禁用，而不需要改代码；系统也可以在运行时，清楚地知道“当前这个 Agent 能做什么，不能做什么”。

## 三、chat_session 表
```sql
CREATE TABLE chat_session (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    agent_id UUID REFERENCES agent(id) ON DELETE SET NULL,  -- 绑定的 Agent
    
    title TEXT,                          -- 自动生成的标题
    metadata JSONB,                      -- 扩展（例如输入语言、设备类型）
  
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);
```

一次对话是成一个过程，而不是一次请求，它本身不承载太多信息，只起到一个锚点的作用：把这次对话绑定到某一个 Agent，并为后续所有行为提供归属。

## 四、chat_message 表
```sql
CREATE TABLE chat_message (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    session_id UUID NOT NULL REFERENCES chat_session(id) ON DELETE CASCADE,

    role TEXT NOT NULL,                      -- user / assistant / system / tool
    content TEXT,                            -- 主体内容
    metadata JSONB,                          -- 工具调用、RAG 片段、模型参数等
    
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);
```

这张表的设计思路非常简单，但非常重要：**对话过程中出现的所有语义片段，都应该是可追溯的。**

不论是用户输入、模型输出、system 规则，还是工具执行结果，它们在语义上是不同的，但在时间线上是连续的。

因此，这里不区分“聊天消息”和“系统消息”，而是统一抽象为 message，通过 role 来区分语义角色。

content 存的是模型真正“说的话”，metadata 存的是那些不适合直接拼进 prompt，但对系统来说非常重要的结构化信息，比如工具参数、RAG 命中内容、模型选项等。

这样一来，系统在任何时刻，都可以根据 session_id，把完整上下文重新拼出来，记忆功能可以根据这个实现。

## 五、knowledge_base 表
```sql
CREATE TABLE knowledge_base (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    name TEXT NOT NULL,
    description TEXT,
    metadata JSONB,                         -- 业务属性，如行业/标签

    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);
```

Agent 系统里非常容易混淆的一点是：哪些信息是模型本来就知道的，哪些是系统必须额外提供的。

这套设计里，knowledge_base 的存在，正是为了划清这条边界。

知识库里的内容，并不是为了“教会模型”，而是为了在需要时，通过 RAG 的方式，把相关背景信息临时提供给模型。

它代表的是**系统私有知识**，而不是模型记忆的一部分。

## 六、document 表
```sql
CREATE TABLE document (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    kb_id UUID NOT NULL REFERENCES knowledge_base(id) ON DELETE CASCADE,

    filename TEXT NOT NULL,
    filetype TEXT,                          -- pdf / md / txt 等
    size BIGINT,                            -- 文件大小
    metadata JSONB,                         -- 页数、上传方式、解析参数等

    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);
```

ddocument 并不直接服务于模型，它管理的是**原始资料这一层**，一个文件是什么、从哪里来的、以什么方式被解析过。

## 七、chunk_bge_m3
```sql
CREATE TABLE chunk_bge_m3 (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    kb_id UUID NOT NULL REFERENCES knowledge_base(id) ON DELETE CASCADE,
    doc_id UUID NOT NULL REFERENCES document(id) ON DELETE CASCADE,

    content TEXT NOT NULL,                  -- 切片后的文本内容
    metadata JSONB,                         -- 页码、段落号、chunk index 等

    embedding VECTOR(1024) NOT NULL,        -- bge_m3 模型是 1024 维的向量

    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);

-- 给向量加索引
CREATE INDEX idx_chunk_embedding
ON chunk_bge_m3
USING ivfflat (embedding vector_l2_ops)
WITH (lists = 100);
```

模型并不会直接阅读整份文档。

在 RAG 中，模型看到的永远是一些被切分后的、语义相对完整的小片段，chunk 正是这些片段的载体。

每一条 chunk 都包含三类关键信息：可读的文本内容、用于检索的向量表示，以及描述来源的元信息。

这里有一个非常重要的设计原则：**向量只负责“找”，文本才负责“看”。**

embedding 只用于计算相似度，从不直接交给模型，真正拼进 prompt 的，是 chunk 中的 content。

**这里加了 bge_m3 的后缀**，表示这张表的向量是通过 **bge_m3 模型生成的。**



> 更新: 2025-12-22 15:44:31  
> 原文: <https://www.yuque.com/chengxuyuancarl/zsqzgh/bldffmrd9q6giplm>