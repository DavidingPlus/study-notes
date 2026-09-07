# TTS:

TTS 服务在 GopherAI 第二版中提供了将文本转换为语音的能力，是模型生成语音输出的核心组件。它的设计目标是将用户输入的文本，通过标准化的 API 调用生成高质量的音频

通过前端轮询的方式去获取语音资源，任务完成后，语音输出

# 工作流程图:

![1766837354282-74b9467e-9ea7-4f02-b7f3-f32b40ab52ae.png](./img/S8V64HwlD8fEjv1R/1766837354282-74b9467e-9ea7-4f02-b7f3-f32b40ab52ae-673313.png)

# 代码解析:

## 创建 TTS 对象

创建 TTS 对象非常简单，通过 `NewTTSService` 初始化即可获得服务实例，便于在系统中复用。文本转语音的核心操作由 `CreateTTS` 方法完成，它接收用户文本，构建请求负载，并调用百度语音合成接口生成语音任务。在请求中，我们设置了音频格式、语音类型、语言、语速、音高、音量以及是否开启字幕等参数，以确保生成的语音符合预期效果

```go
func (s *TTSService) CreateTTS(ctx context.Context, text string) (string, error) {
	accessToken := s.GetAccessToken()
	if accessToken == "" {
		return "", fmt.Errorf("failed to get access token")
	}

	payload := TTSRequest{
		Text:           text,
		Format:         "mp3-16k",
		Voice:          4194,
		Lang:           "zh",
		Speed:          5,
		Pitch:          5,
		Volume:         5,
		EnableSubtitle: 0,
	}

	bodyBytes, err := json.Marshal(payload)
	if err != nil {
		return "", err
	}

	url := "https://aip.baidubce.com/rpc/2.0/tts/v1/create?access_token=" + accessToken
	req, err := http.NewRequestWithContext(ctx, http.MethodPost, url, bytes.NewReader(bodyBytes))
	if err != nil {
		return "", err
	}

	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Accept", "application/json")

	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		return "", err
	}
	defer resp.Body.Close()

	respBody, err := io.ReadAll(resp.Body)
	if err != nil {
		return "", err
	}

	log.Println("[TTS Create] raw:", string(respBody))

	var result TTSCreateResponse
	if err := json.Unmarshal(respBody, &result); err != nil {
		return "", err
	}

	if result.TaskID == "" {
		return "", fmt.Errorf("create tts failed: empty task_id")
	}

	return result.TaskID, nil
}
```

## GetAccessToken 函数解析:

在调用语音合成 API 前，服务会先获取访问令牌（Access Token）

`GetAccessToken` 方法使用配置中的 API Key 和 Secret 发送请求，并解析返回的 JSON，提取有效的访问令牌。整个流程中，服务对请求错误、响应解析错误以及空令牌等情况都进行了妥善处理，保证服务调用的稳定性和可靠性。

```go
func (s *TTSService) GetAccessToken() string {
	conf := config.GetConfig()

	url := "https://aip.baidubce.com/oauth/2.0/token"
	postData := fmt.Sprintf(
		"grant_type=client_credentials&client_id=%s&client_secret=%s",
		conf.VoiceServiceConfig.VoiceServiceApiKey,
		conf.VoiceServiceConfig.VoiceServiceSecretKey,
	)

	resp, err := http.Post(url, "application/x-www-form-urlencoded", bytes.NewReader([]byte(postData)))
	if err != nil {
		log.Println("get token error:", err)
		return ""
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		log.Println("read token error:", err)
		return ""
	}

	var tokenResp struct {
		AccessToken string `json:"access_token"`
	}

	if err := json.Unmarshal(body, &tokenResp); err != nil {
		log.Println("unmarshal token error:", err)
		return ""
	}

	return tokenResp.AccessToken
}
```

当任务成功创建后，服务会返回一个 `task_id`，标识这次语音生成请求。随后，系统可以通过 `QueryTTSFull` 方法查询任务状态。

## QueryTTSFull代码解析

查询方法同样需要访问令牌，并向官方接口发送包含 `task_id` 的请求。返回的 JSON 会被解析为统一结构 `TTSQueryResponse`，其中包括任务的状态和结果信息。如果任务完成，会将生成的语音 URL 封装到 `TTSTaskResult` 中返回给调用方。这样，系统即可根据任务状态动态获取音频文件，实现实时语音播放或下载。

```go
// QueryTTSFull 查询官方 TTS 状态，解析完整 JSON
func (s *TTSService) QueryTTSFull(ctx context.Context, taskID string) (*TTSQueryResponse, error) {
	accessToken := s.GetAccessToken()
	if accessToken == "" {
		return nil, fmt.Errorf("failed to get access token")
	}

	reqBody := map[string][]string{
		"task_ids": {taskID},
	}
	bodyBytes, _ := json.Marshal(reqBody)

	url := "https://aip.baidubce.com/rpc/2.0/tts/v1/query?access_token=" + accessToken
	req, err := http.NewRequestWithContext(ctx, http.MethodPost, url, bytes.NewReader(bodyBytes))
	if err != nil {
		return nil, err
	}

	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Accept", "application/json")

	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()

	respBody, _ := io.ReadAll(resp.Body)
	log.Println("[TTS Query] raw:", string(respBody))

	// 官方返回原始 JSON
	var rawResp struct {
		LogID     json.Number `json:"log_id"`
		TasksInfo []struct {
			TaskID     string          `json:"task_id"`
			TaskStatus string          `json:"task_status"`
			TaskResult json.RawMessage `json:"task_result,omitempty"`
		} `json:"tasks_info"`
	}

	if err := json.Unmarshal(respBody, &rawResp); err != nil {
		return nil, err
	}

	result := &TTSQueryResponse{
		LogID:     rawResp.LogID.String(),
		TasksInfo: make([]TTSTask, 0, len(rawResp.TasksInfo)),
	}

	for _, t := range rawResp.TasksInfo {
		task := TTSTask{
			TaskID:     t.TaskID,
			TaskStatus: t.TaskStatus,
			TaskResult: nil, // 默认 nil
		}

		if t.TaskStatus == "Success" && len(t.TaskResult) > 0 {
			var r TTSTaskResult
			if err := json.Unmarshal(t.TaskResult, &r); err != nil {
				log.Println("parse task_result error:", err)
				return nil, fmt.Errorf("failed to parse task result: %v", err)
			}
			task.TaskResult = &r
		}

		result.TasksInfo = append(result.TasksInfo, task)
	}

	return result, nil
}
```

需要注意的是.在项目逻辑中,前端调用tts接口会不断定时轮询后端这个接口,当任务完成时候,后端会返回相应的语音url,前端这边会进行播放

```vue
try {
  // 创建TTS任务
  const createResponse = await api.post('/AI/chat/tts', { text })
  if (createResponse.data && createResponse.data.status_code === 1000 && createResponse.data.task_id) {
    const taskId = createResponse.data.task_id

    // 先等待5秒钟再开始轮询
    await new Promise(resolve => setTimeout(resolve, 5000))

    // 轮询查询任务结果
    const maxAttempts = 30
    const pollInterval = 2000
    let attempts = 0

    const pollResult = async () => {
      const queryResponse = await api.get('/AI/chat/tts/query', { params: { task_id: taskId } })

      if (queryResponse.data && queryResponse.data.status_code === 1000) {
        const taskStatus = queryResponse.data.task_status

        if (taskStatus === 'Success' && queryResponse.data.task_result) {
          // 任务完成，播放音频
          // 后端返回的 task_result 是直接的 URL 字符串
          const audio = new Audio(queryResponse.data.task_result)
          audio.play()
          return true
        } else if (taskStatus === 'Running' ||taskStatus === 'Created' ) {
          // 任务进行中，继续轮询
          attempts++
          if (attempts < maxAttempts) {
            await new Promise(resolve => setTimeout(resolve, pollInterval))
            return await pollResult()
          } else {
            ElMessage.error('语音合成超时')
            return true
          }
        } else {
          // 其他状态（如失败）
          ElMessage.error('语音合成失败')
          return true
        }
      }

      attempts++
      if (attempts < maxAttempts) {
        await new Promise(resolve => setTimeout(resolve, pollInterval))
        return await pollResult()
      } else {
        ElMessage.error('语音合成超时')
        return true
      }
    }

    await pollResult()
  } else {
    ElMessage.error('无法创建语音合成任务')
  }
} catch (error) {
  console.error('TTS error:', error)
  ElMessage.error('请求语音接口失败')
  }
  }
```


> 更新: 2025-12-28 06:53:20  
> 原文: <https://www.yuque.com/chengxuyuancarl/agcwre/wvbhw8duwwlgv8sn>