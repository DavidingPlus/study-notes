# MCP:

MCP 模块是 GopherAI 第二版中用于 外部工具调用和多渠道能力扩展 的核心组件。它与 RAG（Retrieval-Augmented Generation）模块类似，都是为了增强 LLM 的功能，但 MCP 更偏向 对接外部服务和插件工具，实现动态信息获取和功能调用。

在当前项目中，MCP 主要实现 天气查询功能，通过自建的 MCP Server 对接 <code><font style="color:#000000;">wttr.in</font></code> API，并在 LLM 调用流程中作为工具集成。

# <font style="color:#000000;">工作流程图:</font>

![1766837878758-4a4fd1ab-1c4d-4b43-ae11-5606d0b2e26d.png](https://cdn.davidingplus.cn/images/2026/09/11/1766837878758-4a4fd1ab-1c4d-4b43-ae11-5606d0b2e26d-377492.png)

# <font style="color:#000000;">代码解析</font>

## <font style="color:#000000;">mcp核心函数解析</font>

我们先来看一下common/mcp/server下的核心函数

通过 `server.NewMCPServer` 初始化，通过 `AddTool` 方法注册了 `get_weather` 工具，提供 `city` 参数供调用者指定查询的城市。每当工具被调用时，回调函数会从请求参数中获取城市名称，并使用封装的 `WeatherAPIClient` 获取天气数据。随后，将获取到的天气信息格式化为文本返回，封装在 `mcp.CallToolResult` 中

**需要注意的是,由于这边远程访问外部服务,服务有可能出现超时请求的错误,从而获取不到结果**

```go
//GetWeather别看有一大堆代码
//本质就是调用http请求获取天气信息
func (c *WeatherAPIClient)(ctx context.Context, city string) (*WeatherResponse, error) {
	apiURL := fmt.Sprintf(
		"https://wttr.in/%s?format=j1&lang=zh",
		city,
	)

	req, err := http.NewRequestWithContext(ctx, http.MethodGet, apiURL, nil)
	if err != nil {
		return nil, fmt.Errorf("create request failed: %w", err)
	}

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		return nil, fmt.Errorf("http request failed: %w", err)
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, fmt.Errorf("read response failed: %w", err)
	}

	var wttrResp WttrResponse
	if err := json.Unmarshal(body, &wttrResp); err != nil {
		return nil, fmt.Errorf("json parse failed: %w", err)
	}

	if len(wttrResp.CurrentCondition) == 0 {
		return nil, fmt.Errorf("no weather data")
	}

	cc := wttrResp.CurrentCondition[0]

	temp, _ := strconv.ParseFloat(cc.TempC, 64)
	humidity, _ := strconv.Atoi(cc.Humidity)
	wind, _ := strconv.ParseFloat(cc.WindspeedKmph, 64)

	location := city
	if len(wttrResp.NearestArea) > 0 &&
		len(wttrResp.NearestArea[0].AreaName) > 0 {
		location = wttrResp.NearestArea[0].AreaName[0].Value
	}

	condition := "未知"
	if len(cc.WeatherDesc) > 0 {
		condition = cc.WeatherDesc[0].Value
	}

	return &WeatherResponse{
		Location:    location,
		Temperature: temp,
		Condition:   condition,
		Humidity:    humidity,
		WindSpeed:   wind,
	}, nil
}

func NewMCPServer() *server.MCPServer {
	weatherClient := NewWeatherAPIClient()

	mcpServer := server.NewMCPServer(
		"weather-query-server",
		"1.0.0",
		server.WithToolCapabilities(true),
		server.WithLogging(),
	)

	mcpServer.AddTool(
		mcp.NewTool(
			"get_weather",
			mcp.WithDescription("获取指定城市的天气信息"),
			mcp.WithString(
				"city",
				mcp.Description("城市名称，如 Beijing、上海"),
				mcp.Required(),
			),
		),
		func(ctx context.Context, request mcp.CallToolRequest) (*mcp.CallToolResult, error) {
			args := request.GetArguments()
			city, ok := args["city"].(string)
			if !ok || city == "" {
				return nil, fmt.Errorf("invalid city argument")
			}

			weather, err := weatherClient.GetWeather(ctx, city)
			if err != nil {
				return nil, err
			}

			resultText := fmt.Sprintf(
				"城市: %s\n温度: %.1f°C\n天气: %s\n湿度: %d%%\n风速: %.1f km/h",
				weather.Location,
				weather.Temperature,
				weather.Condition,
				weather.Humidity,
				weather.WindSpeed,
			)

			return &mcp.CallToolResult{
				Content: []mcp.Content{
					mcp.TextContent{
						Type: "text",
						Text: resultText,
					},
				},
			}, nil
		},
	)

	return mcpServer
}
```

## <font style="color:#000000;">mcp模型调用案例</font>

再来看模型是如何调用mcp服务的

在第一版模型的基础上，又创建了mcp模型类实现了模型的方法，以便在基础模型的基础上提供“远程调用工具”的能力

下面是初始化部分

```go
// MCPModel MCP模型实现，集成MCP服务
type MCPModel struct {
	llm        model.ToolCallingChatModel
	mcpClient  *client.Client
	username   string
	mcpBaseURL string
}

// NewMCPModel 创建MCP模型实例
func NewMCPModel(ctx context.Context, username string) (*MCPModel, error) {
	key := os.Getenv("OPENAI_API_KEY")
	conf := config.GetConfig()
	modelName := conf.RagModelConfig.RagChatModelName
	baseURL := conf.RagModelConfig.RagBaseUrl

	// 创建LLM
	llm, err := openai.NewChatModel(ctx, &openai.ChatModelConfig{
		BaseURL: baseURL,
		Model:   modelName,
		APIKey:  key,
	})
	if err != nil {
		return nil, fmt.Errorf("create mcp model failed: %v", err)
	}

	mcpBaseURL := "http://localhost:8081/mcp"

	return &MCPModel{
		llm:        llm,
		mcpBaseURL: mcpBaseURL,
		username:   username,
	}, nil
}
```

接着是生成响应部分：

```go
// GenerateResponse 生成响应，集成MCP工具
func (m *MCPModel) GenerateResponse(ctx context.Context, messages []*schema.Message) (*schema.Message, error) {
	if len(messages) == 0 {
		return nil, fmt.Errorf("no messages provided")
	}

	// 获取最后一条消息
	lastMessage := messages[len(messages)-1]
	query := lastMessage.Content

	// 第一次调用AI：告诉AI使用固定的JSON格式返回数据
    // 构建相应的提示词，告诉ai让他知道我有哪些能力，让ai来判断
    // 用户的问题是否需要调用这些工具
	firstPrompt := m.buildFirstPrompt(query)
	firstMessages := make([]*schema.Message, len(messages))
	copy(firstMessages, messages)
	firstMessages[len(firstMessages)-1] = &schema.Message{
		Role:    schema.User,
		Content: firstPrompt,
	}

	// 调用LLM生成第一次响应
	firstResp, err := m.llm.Generate(ctx, firstMessages)
	if err != nil {
		return nil, fmt.Errorf("mcp first generate failed: %v", err)
	}
	log.Println("first resp is ", firstResp)
	// 解析AI响应
	aiResult := firstResp.Content
	toolCall, err := m.parseAIResponse(aiResult)
	if err != nil {
		log.Printf("Failed to parse AI response: %v", err)
		return firstResp, nil
	}

	// 情况1：AI不调用工具，直接返回响应
	if !toolCall.IsToolCall {
		log.Println("toolCall IsToolCall is false ", firstResp)
		return firstResp, nil
	}
	log.Println("toolCall IsToolCall is true ", firstResp)
	// 情况2：AI要调用工具
	// 获取MCP客户端
	mcpClient, err := m.getMCPClient(ctx)
	if err != nil {
		log.Printf("MCP client error: %v", err)
		return firstResp, nil
	}

	// 调用MCP工具（通过mcp-go）请求到mcp服务器上
	toolResult, err := m.callMCPTool(ctx, mcpClient, toolCall.ToolName, toolCall.Args)
	if err != nil {
		log.Printf("MCP tool call failed: %v", err)
		return firstResp, nil
	}

	// 第二次调用AI：将工具结果告诉AI
	secondPrompt := m.buildSecondPrompt(query, toolCall.ToolName, toolCall.Args, toolResult)
	secondMessages := make([]*schema.Message, len(messages))
	copy(secondMessages, messages)
	secondMessages[len(secondMessages)-1] = &schema.Message{
		Role:    schema.User,
		Content: secondPrompt,
	}

	// 调用LLM生成最终响应
	finalResp, err := m.llm.Generate(ctx, secondMessages)

	if err != nil {
		return nil, fmt.Errorf("mcp second generate failed: %v", err)
	}
	log.Println("最终响应为：", finalResp)
	return finalResp, nil
}
```


> 更新: 2025-12-28 06:53:04  
> 原文: <https://www.yuque.com/chengxuyuancarl/agcwre/dmzmgx8a3u1xghzu>