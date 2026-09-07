# RAG:

RAG（Retrieval-Augmented Generation）模块是 GopherAI 第二版中的核心功能组件，专注于提供高效、准确的知识检索和问答服务。该模块基于 Redis 作为向量数据库，实现对上传文档的向量化存储和检索，支持md文档格式和输入方式。模块采用轻量级架构，注重性能优化和资源管理，确保在 Go Web 应用中无缝集成。

模块支持单次文档索引和检索模式，用户可通过 API 上传文档文件。内部实现完整的文档预处理流程，包括文本解析、向量化和存储。检索过程使用 Redis 进行向量检索，输出相关文档的检索结果，返回构建的 RAG 提示词。



对于刚接触rag的人来说，如果直接看代码，这块还是挺复杂的，因为

+ 市面上go相关rag知识库的开源文章非常少
+ 本身代码难理解

我通过查阅eino官方源码，在common/rag目录下，封装好了rag相关的包

其实整个rag包，只做了三件事：

1. 建知识库（Index）
2. 查知识库（Retrieve）
3. 把查到的内容塞进 Prompt



# 工作流程图:




![1766836865086-c897d7a9-ed89-4b5b-9eb5-ccbb3e489292.png](./img/BsbZwysfSrX-_z5c/1766836865086-c897d7a9-ed89-4b5b-9eb5-ccbb3e489292-377314.png)





# 代码解析
## 结构体封装
其中主要是封装了如下两个结构体：

1. RAGIndexer（建库的人）
2. RAGQuery（查库的人）



```go
//RAGIndexer 结构体：封装完整的文档索引逻辑，包含 Embedder 和 Indexer
type RAGIndexer struct {
    //embedding是文本和向量之间转化的桥梁
    embedding embedding.Embedder
    indexer   *redisIndexer.Indexer
}
//RAGQuery 结构体：封装完整的文档检索逻辑，包含 Embedder 和 Retriever
type RAGQuery struct {
    embedding embedding.Embedder
    retriever retriever.Retriever
}
```



我们这一次不从“建库”阶段讲起，而是反向从“查询”阶段来理解整个 RAG 的工作流程。假设公司内部有一个尚未对外发布的产品，名字叫 Apple。虽然这个产品还没有上线，但公司的 AI 系统已经能够回答诸如“Apple 是什么样的产品”之类的问题。



当用户提出问题时，系统并不会直接在 Redis 中进行字符串匹配查询，而是先将用户的问题转换成一个向量表示（Embedding），这个向量可以理解为问题在语义空间中的数学表达。随后，RAGQuery 中的 retriever 会使用该向量，在 Redis 中对名为 vector 的字段执行向量相似度搜索，从而找到语义上最接近的几段文本内容。



需要注意的是，Redis 中的 vector 字段并不是在查询时临时生成的，而是在 Index（建库）阶段就已经提前写入完成的。在 Index 阶段，系统会将原始文档内容进行解析和切分，把每一段文本转换成向量，并将这些向量存储到 Redis 的 vector 字段中。查询阶段所做的事情，本质上只是“拿问题的向量，去找最相似的文本向量”。因此可以理解为，Index 阶段负责把知识转换成 AI 能理解和搜索的形式并存储起来，而 Query 阶段则负责从这些已经向量化的知识中检索出最相关的内容，再交给大模型生成最终答案。



相信大家在看了上面的内容，就知道这两个结构体是用来做什么的了，我们再来看代码

先看存储文档的向量化部分：NewRAGIndexer -> IndexFile

## 索引代码解析:
```go
// 构建知识库索引
// 专业说法：文本解析、文本切块、向量化、存储向量
// 通俗理解：把“人能读的文档”，转换成“AI 能按语义搜索的格式”，并存起来
func NewRAGIndexer(filename, embeddingModel string) (*RAGIndexer, error) {

	// 用于控制整个初始化流程（超时 / 取消等），这里先用默认背景即可
	ctx := context.Background()

	// 从环境变量中读取调用向量模型所需的 API Key
	apiKey := os.Getenv("OPENAI_API_KEY")

	// 向量的维度大小
	dimension := config.GetConfig().RagModelConfig.RagDimension

	// 1. 创建相关配置
	embedConfig := &embeddingArk.EmbeddingConfig{
		BaseURL: config.GetConfig().RagModelConfig.RagBaseUrl, // 向量模型服务地址
		APIKey:  apiKey,                                       // 鉴权信息
		Model:   embeddingModel,                               // 使用哪个向量模型
	}

	// 通过配置创建向量生成器实例
	// 后续所有文本的“向量化”都会通过它完成
    // 文本和向量转换的桥梁
	embedder, err := embeddingArk.NewEmbedder(ctx, embedConfig)
	if err != nil {
		return nil, fmt.Errorf("failed to create embedder: %w", err)
	}

	// 2. 初始化 Redis 中的向量索引结构
	// 可以理解为：先在 Redis 里建好“仓库”，
	// 告诉它以后要存向量，并且每个向量的维度是多少
	if err := redisPkg.InitRedisIndex(ctx, filename, dimension); err != nil {
		return nil, fmt.Errorf("failed to init redis index: %w", err)
	}

	// 获取 Redis 客户端，用于后续数据写入
	rdb := redisPkg.Rdb

	// 3. 配置索引器（定义：文档如何被存进 Redis）
	indexerConfig := &redisIndexer.IndexerConfig{
		Client:    rdb,                                     // Redis 客户端
		KeyPrefix: redis.GenerateIndexNamePrefix(filename), // 不同知识库使用不同前缀，避免冲突
		BatchSize: 10,                                      // 批量处理文档，提高写入效率

		// 定义：一段文档（Document）在 Redis 中该如何存储
		DocumentToHashes: func(ctx context.Context, doc *schema.Document) (*redisIndexer.Hashes, error) {

			// 从文档的元数据中取出来源信息（例如文件名、URL）
			source := ""
			if s, ok := doc.MetaData["source"].(string); ok {
				source = s
			}

			// 构造 Redis 中实际存储的数据结构（Hash）
			return &redisIndexer.Hashes{
				// Redis Key，一般由“知识库名 + 文档块 ID”组成
				Key: fmt.Sprintf("%s:%s", filename, doc.ID),

				// Redis Hash 中的字段
				Field2Value: map[string]redisIndexer.FieldValue{
					// content：原始文本内容
					// EmbedKey 表示：该字段需要先做向量化，
					// 生成的向量会存入名为 "vector" 的字段中
					"content": {Value: doc.Content, EmbedKey: "vector"},

					// metadata：一些辅助信息，不参与向量计算
					"metadata": {Value: source},
				},
			}, nil
		},
	}

	// 将“向量生成器”交给索引器
	// 这样索引器在写入文本时，可以自动完成向量计算
	indexerConfig.Embedding = embedder

	// 4. 创建最终可用的索引器实例
	// 此时索引器已经具备：
	// - 文本 → 向量 的能力
	// - 向量写入 Redis 的能力
	idx, err := redisIndexer.NewIndexer(ctx, indexerConfig)
	if err != nil {
		return nil, fmt.Errorf("failed to create indexer: %w", err)
	}

	// 返回一个封装好的 RAGIndexer，
	// 后续只需要调用它，就可以把文档加入知识库
	return &RAGIndexer{
		embedding: embedder,
		indexer:   idx,
	}, nil
}
```



```go
// IndexFile 读取文件内容并创建向量索引
func (r *RAGIndexer) IndexFile(ctx context.Context, filePath string) error {
	// 读取文件内容
	content, err := os.ReadFile(filePath)
	if err != nil {
		return fmt.Errorf("failed to read file: %w", err)
	}

	// 将文件内容转换为文档
	// TODO: 这里可以根据需要进行文本切块，目前简单处理为一个文档
	doc := &schema.Document{
		ID:      "doc_1", // 可以使用 UUID 或其他唯一标识
		Content: string(content),
		MetaData: map[string]any{
			"source": filePath,
		},
	}

	// 使用 indexer 存储文档（会自动进行向量化）
	_, err = r.indexer.Store(ctx, []*schema.Document{doc})
	if err != nil {
		return fmt.Errorf("failed to store document: %w", err)
	}

	return nil
}
```



其中内部还有用到InitRedisIndex函数

```go
// InitRedisIndex 初始化 Redis 索引，支持按文件名区分
func InitRedisIndex(ctx context.Context, filename string, dimension int) error {
	indexName := GenerateIndexName(filename)

	// 检查索引是否存在
	_, err := Rdb.Do(ctx, "FT.INFO", indexName).Result()
	if err == nil {
		fmt.Println("索引已存在，跳过创建")
		return nil
	}

	// 如果索引不存在，创建新索引
	if !strings.Contains(err.Error(), "Unknown index name") {
		return fmt.Errorf("检查索引失败: %w", err)
	}

	fmt.Println("正在创建 Redis 索引...")

	prefix := GenerateIndexNamePrefix(filename)

	// 创建索引
	createArgs := []interface{}{
        //创建一个新的索引，名字是 indexName
		"FT.CREATE", indexName,
        //
		"ON", "HASH",
		"PREFIX", "1", prefix,
		"SCHEMA",
        //定义字段类型。content 和 metadata 都是文本字段
		"content", "TEXT",
		"metadata", "TEXT",
        //告诉 Redis 这是一个向量字段，存储向量数据。
		"vector", "VECTOR", "FLAT",
		"6",
        //向量里每个数字的类型
		"TYPE", "FLOAT32",
        //向量的维度
		"DIM", dimension,
        //告诉 Redis，查相似向量的时候，用“余弦相似度”去比较两个向量是不是意思相近
		"DISTANCE_METRIC", "COSINE",
	}

	if err := Rdb.Do(ctx, createArgs...).Err(); err != nil {
		return fmt.Errorf("创建索引失败: %w", err)
	}

	fmt.Println("索引创建成功！")
	return nil
}
```



存看完了，下面该讲查了

## 查询代码解析:
```go
// NewRAGQuery 创建 RAG 查询器（用于向量检索和问答）
// 自然语言问题变成向量，然后去 Redis 向量索引里找最相关的文档。
func NewRAGQuery(ctx context.Context, username string) (*RAGQuery, error) {
	cfg := config.GetConfig()
	apiKey := os.Getenv("OPENAI_API_KEY")

	// 创建embedConfig
	embedConfig := &embeddingArk.EmbeddingConfig{
		BaseURL: cfg.RagModelConfig.RagBaseUrl,
		APIKey:  apiKey,
		Model:   cfg.RagModelConfig.RagEmbeddingModel,
	}
    // 而后创建 embedding 模型
    // embedding 是把文本变成向量的工具
	embedder, err := embeddingArk.NewEmbedder(ctx, embedConfig)
	if err != nil {
		return nil, fmt.Errorf("failed to create embedder: %w", err)
	}

	// 获取用户上传的文件名（假设每个用户只有一个文件）
	// 这里需要从用户目录读取文件名
	userDir := fmt.Sprintf("uploads/%s", username)
	files, err := os.ReadDir(userDir)
	if err != nil || len(files) == 0 {
		return nil, fmt.Errorf("no uploaded file found for user %s", username)
	}

	var filename string
	for _, f := range files {
		if !f.IsDir() {
			filename = f.Name()
			break
		}
	}

	if filename == "" {
		return nil, fmt.Errorf("no valid file found for user %s", username)
	}

	// 创建 retriever
	rdb := redisPkg.Rdb
	indexName := redis.GenerateIndexName(filename)

	retrieverConfig := &redisRetriever.RetrieverConfig{
		Client:       rdb,
		Index:        indexName,
		Dialect:      2,
		ReturnFields: []string{"content", "metadata", "distance"},
		TopK:         5,
		VectorField:  "vector",
        // 由于 Redis 存储的文档格式和 Eino 框架内部定义的 schema.Document 格式不同
        // 因此在查询时需要通过 DocumentConverter 进行相应的转换，
        // 将 Redis 返回的数据整理成框架内部可直接使用的 Document 对象。
		DocumentConverter: func(ctx context.Context, doc redisCli.Document) (*schema.Document, error) {
			resp := &schema.Document{
				ID:       doc.ID,
				Content:  "",
				MetaData: map[string]any{},
			}
			for field, val := range doc.Fields {
				if field == "content" {
					resp.Content = val
				} else {
					resp.MetaData[field] = val
				}
			}
			return resp, nil
		},
	}
	retrieverConfig.Embedding = embedder

	rtr, err := redisRetriever.NewRetriever(ctx, retrieverConfig)
	if err != nil {
		return nil, fmt.Errorf("failed to create retriever: %w", err)
	}

	return &RAGQuery{
		embedding: embedder,
		retriever: rtr,
	}, nil
}
```

检索相关文档，并将检索的内容返回

```go
// RetrieveDocuments 检索相关文档
func (r *RAGQuery) RetrieveDocuments(ctx context.Context, query string) ([]*schema.Document, error) {
	docs, err := r.retriever.Retrieve(ctx, query)
	if err != nil {
		return nil, fmt.Errorf("failed to retrieve documents: %w", err)
	}
	return docs, nil
}
```



## rag模型调用案例
rag模型与原先不同的是，多增加了如下的向量化查询和拼接提示词的操作，以便正确返回信息

```go
func (o *AliRAGModel) GenerateResponse(ctx context.Context, messages []*schema.Message) (*schema.Message, error) {
	// 1. 创建 RAG 查询器
	ragQuery, err := rag.NewRAGQuery(ctx, o.username)
	if err != nil {
		log.Printf("Failed to create RAG query (user may not have uploaded file): %v", err)
		// 如果用户没有上传文件，直接使用原始问题
		resp, err := o.llm.Generate(ctx, messages)
		if err != nil {
			return nil, fmt.Errorf("ali rag generate failed: %v", err)
		}
		return resp, nil
	}

	// 2. 获取用户最后一条消息作为查询
	if len(messages) == 0 {
		return nil, fmt.Errorf("no messages provided")
	}
	lastMessage := messages[len(messages)-1]
	query := lastMessage.Content

	// 3. 检索相关文档
	docs, err := ragQuery.RetrieveDocuments(ctx, query)
	if err != nil {
		log.Printf("Failed to retrieve documents: %v", err)
		// 检索失败，使用原始问题
		resp, err := o.llm.Generate(ctx, messages)
		if err != nil {
			return nil, fmt.Errorf("ali rag generate failed: %v", err)
		}
		return resp, nil
	}

	// 4. 构建包含检索结果的提示词
	ragPrompt := rag.BuildRAGPrompt(query, docs)

	// 5. 替换最后一条消息为 RAG 提示词
	ragMessages := make([]*schema.Message, len(messages))
	copy(ragMessages, messages)
	ragMessages[len(ragMessages)-1] = &schema.Message{
		Role:    schema.User,
		Content: ragPrompt,
	}

	// 6. 调用 LLM 生成回答
	resp, err := o.llm.Generate(ctx, ragMessages)
	if err != nil {
		return nil, fmt.Errorf("ali rag generate failed: %v", err)
	}
	return resp, nil
}
```









> 更新: 2026-01-19 14:55:00  
> 原文: <https://www.yuque.com/chengxuyuancarl/agcwre/nlc2ve0ymhpszzrb>