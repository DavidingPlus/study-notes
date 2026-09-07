# 4.框架梳理

## 框架组成

该HTTP框架的实现主要分为以下几个部分：

* HttpRequest（HTTP请求）：表示一个HTTP请求。负责解析HTTP方法、头部和主体内容
* HttpResponse（HTTP响应）：表示一个HTTP响应。构建带有适当状态码、头部和主体内容的响应
* HttpContext：该类管理 HTTP 请求在处理过程中的状态。它跟踪解析状态并存储 `HttpRequest` 对象。

1. HttpServer：框架的核心，`HttpServer` 负责接受连接、读取请求和发送响应。它使用 Muduo 库高效地处理网络通信。

* 静态路由、动态路由和处理器（Router）：管理HTTP请求的路由，根据请求路径和方法将其路由到适当的处理器
* 会话管理模块
* 中间件模块
* 集成数据库连接池模块
* HTTPS拓展模块

####


> 更新: 2025-01-16 15:20:34  
> 原文: <https://www.yuque.com/chengxuyuancarl/imh9xc/wv4w7ko8ifnltdnb>