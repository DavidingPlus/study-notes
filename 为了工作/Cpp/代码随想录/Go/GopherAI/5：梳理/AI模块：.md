# AI模块：

## 概述

AI模块是GopherAI项目的核心组件，专注于集成多种AI模型和服务，提供智能聊天对话功能。该模块采用模块化设计，**支持OpenAI GPT系列、Ollama本地模型等多种AI后端**，实现统一的接口调用和管理。**核心功能包括多会话管理、消息历史维护、同步/流式响应输出，以及异步消息持久化。**

**模块使用工厂模式创建和管理AI助手实例**，每个用户和会话对应独立的AIHelper对象，确保会话隔离和上下文连续性。单例管理器（GlobalManager）维护全局AI助手映射，支持高效的实例获取和生命周期管理。消息历史在内存中维护，通过RabbitMQ异步存储到数据库，避免阻塞主线程。

**支持两种响应模式**：同步模式一次性返回完整AI回复，适用于短对话；流式模式使用SSE（Server-Sent Events）实时推送内容片段，提升用户体验。模块集成JWT认证，确保只有登录用户可访问AI功能。

**架构设计注重高并发和可扩展性**，AIHelper使用读写锁保护消息历史，支持多线程安全访问。配置驱动的模型选择和API密钥管理，便于部署和维护。该模块不仅服务于聊天功能，还为项目提供了AI集成的标准框架，可扩展到其他AI应用场景。

## 工作流程图：

![1762829114443-1615e319-8713-41b0-b855-46e921074b6f.png](https://cdn.davidingplus.cn/images/2026/09/11/1762829114443-1615e319-8713-41b0-b855-46e921074b6f-709463.png)

* **前端**：AIChat.vue发送问题，支持同步/流式模式选择。
* **路由层**：AI.go路由匹配/chat/\*路径，分发到聊天控制器。
* **中间件层**：jwt.go验证用户Token，确保已登录。
* **控制器层**：session.go控制器（ChatSend/ChatStreamSend等）解析参数，处理会话ID和模型类型。
* **服务层**：session.go服务（CreateSessionAndSendMessage等）管理会话创建、AI回复生成。
* **数据访问层**：session.go DAO创建会话记录，message.go可选消息存储。
* **通用组件**：AIHelper生成回复、AIHelperManager管理实例、AIModelFactory创建模型、RabbitMQ异步存储消息、Redis缓存状态。
* **外部服务**：AI API（如OpenAI/Ollama）提供模型推理。

流程支持新会话创建（生成UUID）和现有会话继续，消息通过RabbitMQ异步持久化，避免阻塞。流式模式用SSE推送实时内容。

## AI助手架构

AI助手模块基于AIHelper结构体实现，采用面向对象的封装设计，为每个会话提供独立的AI交互环境。结构体包含消息历史、模型绑定和会话管理，支持动态配置和扩展。

**核心组件：**

● **AIHelper结构体**：核心类封装单个会话的AI交互逻辑，结构体定义如下：

```go
type AIHelper struct {
    model     AIModel                    // AI模型接口，支持不同模型实现
    messages  []*model.Message           // 消息历史列表，存储用户和AI的对话记录
    mu        sync.RWMutex               // 读写锁，保护消息历史并发访问
    SessionID string                     // 会话唯一标识，用于绑定消息和上下文
    saveFunc  func(*model.Message) (*model.Message, error)  // 消息存储回调函数，默认异步发布到RabbitMQ
}
```

* **消息历史**：内存中维护\[]\*model.Message切片，按时间顺序存储对话消息。每个Message包含SessionID、Content、UserName、IsUser（区分用户/AI消息）和时间戳。历史用于构建AI上下文，确保连续对话的连贯性。
* **模型绑定**：通过AIModel接口动态绑定AI模型，支持运行时切换（如从GPT切换到Ollama）。接口定义GenerateResponse和StreamResponse方法，实现同步和流式生成。
* **会话ID**：字符串类型唯一标识会话，用于消息关联和实例隔离。每个AIHelper实例绑定一个SessionID，确保多会话独立。
* **存储函数**：函数指针类型，默认实现通过RabbitMQ异步发布消息到队列。支持自定义回调，如同步写入数据库，便于测试和扩展。消息发布包含SessionID、Content、UserName、IsUser参数。

### 工作机制：

1. **实例创建**：NewAIHelper构造函数初始化结构体，设置默认saveFunc为RabbitMQ发布。消息列表为空切片，SessionID从参数传入。
2. **消息添加**：AddMessage方法添加新消息到历史，自动调用saveFunc持久化。若Save参数为false，仅内存存储。使用锁保护并发安全。
3. **响应生成**：GenerateResponse/StreamResponse方法构建消息上下文，调用模型接口生成回复。用户消息先添加历史，AI回复后存储。流式模式通过回调实时输出。
4. **历史获取**：GetMessages返回历史副本，避免外部修改。使用读锁确保线程安全。
5. **自定义存储**：SetSaveFunc允许替换存储逻辑，如单元测试中的内存存储。

该架构通过组合模式和策略模式实现灵活性，支持多模型扩展和异步存储，适用于高并发的聊天应用。

## 模型接口

模块定义 <code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">AIModel</font></code> 接口，支持多种 AI 模型实现。当前支持 OpenAI 和 Ollama。

### 接口定义

```go
type AIModel interface {
    GenerateResponse(ctx context.Context, messages []*schema.Message) (*schema.Message, error)
    StreamResponse(ctx context.Context, messages []*schema.Message, cb StreamCallback) (string, error)
    GetModelType() string
}
```

* **GenerateResponse**：同步生成回复。
* **StreamResponse**：流式生成回复，通过回调函数实时输出。
* **GetModelType**：返回模型类型。

### OpenAI 实现

使用 <code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">github.com/cloudwego/eino-ext/components/model/openai</font></code> 库。

* 配置：从环境变量读取 <code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">OPENAI_API_KEY</font></code>、<code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">OPENAI_MODEL_NAME</font></code>、<code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">OPENAI_BASE_URL</font></code>。
* 流式处理：聚合内容并调用回调函数。

### Ollama 实现

使用 <code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">github.com/cloudwego/eino-ext/components/model/ollama</font></code> 库。

* 配置：传入 <code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">baseURL</font></code> 和 <code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">modelName</font></code>。
* 类似 OpenAI，支持本地部署。

## 工厂模式

**AIModelFactory采用工厂模式实现AI模型的创建和管理，支持动态注册和实例化多种AI模型（如OpenAI和Ollama）。工厂使用map存储创建者函数，确保扩展性和解耦。**

**核心实现：**

* **注册创建者**：使用map\[string]ModelCreator存储模型创建函数，键为模型类型字符串（如"1"表示OpenAI，"2"表示Ollama）。ModelCreator定义为func(ctx context.Context, config map\[string]interface{}) (AIModel, error)。
* **创建模型**：CreateAIModel方法根据modelType从map获取创建者，调用函数实例化模型。传入context和配置参数，返回AIModel接口实例。
* **一键创建助手**：CreateAIHelper方法结合工厂和AIHelper创建，直接返回配置好的助手实例。内部调用CreateAIModel获取模型，然后NewAIHelper创建助手。
* **扩展性**：RegisterModel方法允许运行时注册新模型类型，动态扩展支持的AI服务。全局单例工厂通过sync.Once确保线程安全初始化。

**代码示例：**

```go
// 工厂结构体定义
type AIModelFactory struct {
    creators map[string]ModelCreator
    mu       sync.RWMutex
}

// 注册创建者
func (f *AIModelFactory) RegisterModel(modelType string, creator ModelCreator) {
    f.mu.Lock()
    defer f.mu.Unlock()
    f.creators[modelType] = creator
}

// 创建模型
func (f *AIModelFactory) CreateAIModel(ctx context.Context, modelType string, config map[string]interface{}) (AIModel, error) {
    f.mu.RLock()
    creator, exists := f.creators[modelType]
    f.mu.RUnlock()
    if !exists {
        return nil, fmt.Errorf("unknown model type: %s", modelType)
    }
    return creator(ctx, config)
}

// 一键创建助手
func (f *AIModelFactory) CreateAIHelper(ctx context.Context, modelType string, SessionID string, config map[string]interface{}) (*AIHelper, error) {
    model, err := f.CreateAIModel(ctx, modelType, config)
    if err != nil {
        return nil, err
    }
    return NewAIHelper(model, SessionID), nil
}
```

## 管理器

**AIHelperManager采用单例模式管理用户-会话-AIHelper的映射关系，实现实例缓存和生命周期控制。**

**核心实现：**

● **数据结构**：使用map\[string]map\[string]\*AIHelper，外层map键为用户名，内层map键为会话ID，值为AIHelper指针。支持多用户多会话隔离。

● **获取或创建**：GetOrCreateAIHelper方法检查是否存在助手，若无则通过工厂创建并存储。传入用户名、会话ID、模型类型和配置，确保实例唯一性。

● **移除和查询**：RemoveAIHelper删除指定助手，GetAIHelper获取现有实例，GetUserSessions返回用户的所有会话ID列表。

● **全局单例**：GetGlobalManager使用sync.Once返回单例实例，提供统一管理入口。

**代码示例：**

```go
// 管理器结构体定义
type AIHelperManager struct {
    helpers map[string]map[string]*AIHelper  // 用户 -> 会话 -> 助手
    factory *AIModelFactory
    mu      sync.RWMutex
}

// 获取或创建助手
func (m *AIHelperManager) GetOrCreateAIHelper(userName, sessionID, modelType string, config map[string]interface{}) (*AIHelper, error) {
    m.mu.Lock()
    defer m.mu.Unlock()
    
    if m.helpers[userName] == nil {
        m.helpers[userName] = make(map[string]*AIHelper)
    }
    
    if helper, exists := m.helpers[userName][sessionID]; exists {
        return helper, nil
    }
    
    helper, err := m.factory.CreateAIHelper(context.Background(), modelType, sessionID, config)
    if err != nil {
        return nil, err
    }
    
    m.helpers[userName][sessionID] = helper
    return helper, nil
}

// 获取用户会话列表
func (m *AIHelperManager) GetUserSessions(userName string) []string {
    m.mu.RLock()
    defer m.mu.RUnlock()
    
    var sessions []string
    if userSessions, exists := m.helpers[userName]; exists {
        for sessionID := range userSessions {
            sessions = append(sessions, sessionID)
        }
    }
    return sessions
}

// 移除助手
func (m *AIHelperManager) RemoveAIHelper(userName, sessionID string) {
    m.mu.Lock()
    defer m.mu.Unlock()
    
    if userSessions, exists := m.helpers[userName]; exists {
        delete(userSessions, sessionID)
        if len(userSessions) == 0 {
            delete(m.helpers, userName)
        }
    }
}
```

## 会话管理

会话基于 <code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">model.Session</font></code> 模型，存储在 MySQL 中。

* **Session 模型**：

```go
type Session struct {
    ID        string    `gorm:"primaryKey;type:varchar(36)"`
    UserName  string    `gorm:"index;not null"`
    Title     string    `gorm:"type:varchar(100)"`
    CreatedAt time.Time
    UpdatedAt time.Time
    DeletedAt gorm.DeletedAt `gorm:"index"`
}
```

* **创建会话**：使用 UUID 生成唯一 ID，标题为用户首个问题。
* **会话列表**：<code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">GetUserSessionsByUserName</font></code> 返回用户所有会话 ID 和标题。

## 消息处理

消息基于 <code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">model.Message</font></code> 模型，支持用户和 AI 消息区分。

* **Message 模型**：

```go
type Message struct {
    ID        uint      `gorm:"primaryKey;autoIncrement"`
    SessionID string    `gorm:"index;not null;type:varchar(36)"`
    UserName  string    `gorm:"type:varchar(20)"`
    Content   string    `gorm:"type:text"`
    IsUser    bool      `gorm:"not null"`
    CreatedAt time.Time
}
```

* **存储策略**：默认异步推送到 RabbitMQ 队列，由消费者处理持久化到 MySQL。
* **历史查询**：<code><font style="background-color:rgba(255, 255, 255, 0.1) !important;">GetMessagesBySessionID</font></code> 获取会话消息，按时间排序。

<font style="color:rgb(212, 212, 212);"></font>

## API接口请求流程举例与MVC架构

**采用经典MVC（Model-View-Controller）架构设计**，实现前后端分离。前端Vue.js作为View层处理用户界面，后端Go实现Controller、Service和Model层。**以下以POST /chat/send-new-session接口为例，展示完整MVC流程：创建新会话并同步发送消息。**

**MVC架构概述：**

* **Model（模型层）**：定义数据结构和业务实体，如model/session.go的Session结构体、model/message.go的Message结构体。负责数据表示和持久化逻辑。
* **View（视图层）**：前端Vue.js组件，如AIChat.vue，负责渲染UI和处理用户输入，通过API调用与Controller交互。
* **Controller（控制器层）**：如controller/session/session.go，接收HTTP请求，调用Service执行业务逻辑，返回响应。

**完整流程举例（POST /chat/send-new-session）：**

1. **前端View发起请求**：用户在AIChat.vue中输入问题，点击发送。前端构造JSON请求体，调用axios.post('/chat/send-new-session', requestData)，requestData包含question、modelType。
2. **Controller接收并处理**：Gin路由分发到session.CreateSessionAndSendMessage控制器方法。方法执行：
   * 绑定请求参数到CreateSessionAndSendMessageRequest结构体。
   * 从JWT中间件获取userName。
   * 调用service.CreateSessionAndSendMessage(userName, req.UserQuestion, req.ModelType)。

```go
func CreateSessionAndSendMessage(c *gin.Context) {
	req := new(CreateSessionAndSendMessageRequest)
	res := new(CreateSessionAndSendMessageResponse)
	userName := c.GetString("userName") // From JWT middleware
	if err := c.ShouldBindJSON(req); err != nil {
		c.JSON(http.StatusOK, res.CodeOf(code.CodeInvalidParams))
		return
	}
	//内部会创建会话并发送消息，并会将AI回答、当前会话返回
	session_id, aiInformation, code_ := session.CreateSessionAndSendMessage(userName, req.UserQuestion, req.ModelType)

	if code_ != code.CodeSuccess {
		c.JSON(http.StatusOK, res.CodeOf(code_))
		return
	}

	res.Success()
	res.AiInformation = aiInformation
	res.SessionID = session_id
	c.JSON(http.StatusOK, res)
}
```

3. **Service执行业务逻辑**：session.CreateSessionAndSendMessage服务方法执行：
   * 创建新Session实体，调用dao.CreateSession持久化到MySQL。
   * 获取全局AIHelperManager，调用GetOrCreateAIHelper创建AIHelper实例。
   * 调用helper.GenerateResponse生成AI回复（同步模式）。
   * 返回sessionID和aiResponse。

```go
func CreateSessionAndSendMessage(userName string, userQuestion string, modelType string) (string, string, code.Code) {
	//1：创建一个新的会话
	newSession := &model.Session{
		ID:       uuid.New().String(),
		UserName: userName,
		Title:    userQuestion, // 可以根据需求设置标题，这边暂时用用户第一次的问题作为标题
	}
	createdSession, err := session.CreateSession(newSession)
	if err != nil {
		log.Println("CreateSessionAndSendMessage CreateSession error:", err)
		return "", "", code.CodeServerBusy
	}

	//2：获取AIHelper并通过其管理消息
	manager := aihelper.GetGlobalManager()
	config := map[string]interface{}{
		"apiKey": "your-api-key", // TODO: 从配置中获取,目前这个没什么用
                                  // 跟本地模型对接的，只封装了本地模型对应接口
	}
	helper, err := manager.GetOrCreateAIHelper(userName, createdSession.ID, modelType, config)
	if err != nil {
		log.Println("CreateSessionAndSendMessage GetOrCreateAIHelper error:", err)
		return "", "", code.AIModelFail
	}

	//3：生成AI回复
	aiResponse, err_ := helper.GenerateResponse(userName, ctx, userQuestion)
	if err_ != nil {
		log.Println("CreateSessionAndSendMessage GenerateResponse error:", err_)
		return "", "", code.AIModelFail
	}

	return createdSession.ID, aiResponse.Content, code.CodeSuccess
}
```

4. **DAO数据访问**：dao.CreateSession使用GORM插入Session记录到数据库。AIHelper内部通过RabbitMQ异步存储消息到Message表。

```go
func CreateSession(session *model.Session) (*model.Session, error) {
	err := mysql.DB.Create(session).Error
	return session, err
}
```

5. **Controller返回响应**：构造CreateSessionAndSendMessageResponse，包含aiInformation和sessionID，返回JSON响应给前端。

```go
	//内部会创建会话并发送消息，并会将AI回答、当前会话返回
	session_id, aiInformation, code_ := session.CreateSessionAndSendMessage(userName, req.UserQuestion, req.ModelType)

	if code_ != code.CodeSuccess {
		c.JSON(http.StatusOK, res.CodeOf(code_))
		return
	}

	res.Success()
	res.AiInformation = aiInformation
	res.SessionID = session_id
	c.JSON(http.StatusOK, res)
```

6. **前端View更新**：接收响应，更新UI显示AI回复，添加新会话到侧边栏。

### 主要接口

AI模块提供RESTful API，支持同步和流式聊天，AI所有接口需JWT认证。

● GET /chat/sessions：获取用户会话列表。Controller调用service.GetUserSessionsByUserName，DAO查询Session表，返回\[]SessionInfo。

● POST /chat/send-new-session：创建新会话并发送消息（同步）。如上流程，返回AI回复和sessionID。

● POST /chat/send：向现有会话发送消息（同步）。类似流程，但复用现有sessionID。

● POST /chat/history：获取会话历史。Controller调用service.GetChatHistory，AIHelper返回内存消息历史。

● POST /chat/send-stream-new-session：创建新会话并流式发送消息。Controller设置SSE头，调用service.CreateStreamSessionAndSendMessage，流式推送数据。

● POST /chat/send-stream：向现有会话流式发送消息。复用sessionID，流式输出。

### 请求/响应示例

**发送消息请求**（POST /chat/send-new-session）：

```json
{
  "question": "你好，请介绍一下Go语言",
  "modelType": "1"
}
```

<font style="color:rgb(212, 212, 212);">● </font>**<font style="color:rgb(212, 212, 212);">响应</font>**<font style="color:rgb(212, 212, 212);">：</font>

```json
{
  "code": 200,
  "message": "success",
  "data": {
    "Information": "Go语言是由Google开发的开源编程语言，以简洁、高效著称...",
    "sessionId": "550e8400-e29b-41d4-a716-446655440000"
  }
}
```

## **流式响应**

**AI模块支持流式响应，使用Server-Sent Events (SSE)协议实现实时内容推送，提升用户交互体验。流式模式适用于长文本生成，允许前端逐步显示AI回复。**

### SSE 配置：

* **Content-Type设置**：响应头设置为"text/event-stream"，告知浏览器这是SSE流。
* **缓存控制**：设置"Cache-Control: no-cache"禁用缓存，"Connection: keep-alive"保持连接，"Access-Control-Allow-Origin: \*"支持跨域。
* **缓冲禁用**：设置"X-Accel-Buffering: no"防止代理服务器缓冲，确保数据实时到达前端。

```go
	// 设置SSE头
	c.Header("Content-Type", "text/event-stream")
	c.Header("Cache-Control", "no-cache")
	c.Header("Connection", "keep-alive")
	c.Header("Access-Control-Allow-Origin", "*")
	c.Header("X-Accel-Buffering", "no") // 禁止代理缓存
```

### 回调机制：

* **StreamCallback定义**：类型为func(msg string)，在流式生成过程中实时调用。每次接收模型输出片段时，执行回调发送数据。
* **数据格式**：回调内部构造SSE格式字符串"data: " + msg + "\n\n"，通过http.ResponseWriter.Write写入响应流。每个数据块以双换行符结束。
* **刷新机制**：调用http.Flusher.Flush()立即发送数据到客户端，确保低延迟。

### 结束信号：

* **完成标记**：流结束时发送"data: \[DONE]\n\n"，前端据此停止监听并关闭连接。
* **错误处理**：若生成失败，通过SSE发送错误事件，如"event: error\ndata: {"message": "Failed to generate"}\n\n"。

### 实现细节：

* **StreamResponse方法**：在AIHelper中实现，接收StreamCallback参数。首先添加用户消息到历史，锁定读取消息构建上下文。调用model.StreamResponse(ctx, messages, cb)，传入回调函数。
* **回调实现**：内部定义cb函数，接收msg字符串，写入响应流并刷新。支持并发安全，通过Flusher确保顺序发送。
* **控制器集成**：ChatStreamSend控制器设置SSE头，调用session.ChatStreamSend传递ResponseWriter。函数内部获取助手，执行StreamMessageToExistingSession。
* **前端处理**：前端EventSource监听"data"事件，累积内容显示。收到\[DONE]时处理完成逻辑。

该实现确保流式响应高效且可靠，支持中断恢复和错误处理，适用于实时AI对话场景。

## 总结

AI模块通过工厂模式和单例管理器实现灵活的模型集成，支持多会话并发。异步消息存储确保性能，流式响应提升用户体验。模块易扩展，可添加新模型或存储策略。


> 更新: 2025-11-11 10:58:04  
> 原文: <https://www.yuque.com/chengxuyuancarl/agcwre/ywrbfqhn6aqtgc5d>