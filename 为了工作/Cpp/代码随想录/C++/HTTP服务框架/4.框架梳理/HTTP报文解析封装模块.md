# HTTP报文解析封装模块

## 模块分析

### HttpRequest（HTTP请求）

`HttpRequest`对象就是对HTTP请求报文的封装，将请求报文的重要信息封装到对象中。看一下他的成员变量就懂了：

```cpp
Method                                       method_; // 请求方法
std::string                                  version_; // http版本
std::string                                  path_; // 请求路径
std::unordered_map<std::string, std::string> pathParameters_; // 路径参数
std::unordered_map<std::string, std::string> queryParameters_; // 查询参数
muduo::Timestamp                             receiveTime_; // 接收时间
std::map<std::string, std::string>           headers_; // 请求头
std::string                                  content_; // 请求体
uint64_t                                     contentLength_ { 0 }; // 请求体长度
```

再简单分析一下他的几个成员函数就更能清晰的知道这个类的作用了。

关键成员方法：

* setMethod()：设置 HTTP 方法（GET、POST 等）。
* addHeader()：向请求中添加头部。
* setBody()：设置请求的主体内容。

通过上述描述我们知道了`HttpRequest`类的是用于封装HTTP请求报文信息的，那么我们应该如何验证客户端发送过来HTTP报文的格式和基本内容是否符合规范呢？这时就轮到`HttpContext`发挥作用了。

### HttpContext

我们通过分析`HttpContext`中的成员变量和成员函数就不难发现，`HttpContext`是用于解析`HttpContext`报文并把信息封装到`HttpRequest`中的。

HttpContext重要成员变量：

```cpp
enum HttpRequestParseState
{
    kExpectRequestLine, // 解析请求行
    kExpectHeaders, // 解析请求头
    kExpectBody, // 解析请求体
    kGotAll, // 解析完成
};

HttpRequestParseState state_;
HttpRequest           request_;
```

HttpContext的重要成员函数：

**解析请求报文：**

```cpp
// 将报文解析出来将关键信息封装到HttpRequest对象里面去
bool HttpContext::parseRequest(Buffer *buf, Timestamp receiveTime)
{
    bool ok = true; // 解析每行请求格式是否正确
    bool hasMore = true;
    while (hasMore)
    {
        if (state_ == kExpectRequestLine)
        {
            const char *crlf = buf->findCRLF(); // 注意这个返回值边界可能有错
            if (crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok)
                {
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2);
                    state_ = kExpectHeaders;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (state_ == kExpectHeaders)
        {
            const char *crlf = buf->findCRLF();
            if (crlf)
            {
                const char *colon = std::find(buf->peek(), crlf, ':');
                if (colon < crlf)
                {
                    request_.addHeader(buf->peek(), colon, crlf);
                }
                else if (buf->peek() == crlf)
                { // 空行，结束Header
                    if (!request_.getHeader("Content-Length").empty())
                    {
                        request_.setContentLength(std::stoi(request_.getHeader("Content-Length")));
                    }

                    if (request_.contentLength() == 0)
                    {
                        state_ = kGotAll;
                        hasMore = false;
                    }
                    else // 有请求体
                    {
                        state_ = kExpectBody;
                    }
                }
                else
                {
                    ok = false; // Header行格式错误
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2); // 开始读指针指向下一行数据
            }
            else
            {
                hasMore = false;
            }
        }
        else if (state_ == kExpectBody)
        {
            // 检查缓冲区中是否有足够的数据，
            if (buf->readableBytes() < request_.contentLength())
            {
                hasMore = false; // 数据不完整，等待更多数据
                return true;
            }

            // 只读取 Content-Length 指定的长度
            std::string body(buf->peek(), buf->peek() + request_.contentLength());
            request_.setBody(body);

            // 准确移动读指针
            buf->retrieve(request_.contentLength());

            state_ = kGotAll;
            hasMore = false;
        }
    }
    return ok;
}
```

**解析请求行：**

```cpp
// 解析请求行
bool HttpContext::processRequestLine(const char *begin, const char *end)
{
    bool succeed = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' ');
    if (space != end && request_.setMethod(start, space))
    {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char *argumentStart = std::find(start, space, '?');
            if (argumentStart != space) // 请求带参数
            {
                request_.setPath(start, argumentStart); // 注意这些返回值边界
                request_.setQueryParameters(argumentStart + 1, space);
            }
            else // 请求不带参数
            {
                request_.setPath(start, space);
            }

            start = space + 1;
            succeed = ((end - start == 8) && std::equal(start, end - 1, "HTTP/1."));
            if (succeed)
            {
                if (*(end - 1) == '1')
                {
                    request_.setVersion("HTTP/1.1");
                }
                else if (*(end - 1) == '0')
                {
                    request_.setVersion("HTTP/1.0");
                }
                else
                {
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}
```

### HttpResponse（HTTP响应）

`HttpResponse`的封装跟`HttpRequest`的封装同理，是对HTTP响应报文信息的封装，这里不详细介绍了，直接看代码没问题。

### HttpServer

当看完`HttpResponse`和`HttpRequest`之后，肯定会疑惑他们之间是怎么建立起联系的？他们又是什么时候调用的？那这时候就需要用到`HttpServer`了。`HttpServer`是框架的核心，也是接入Muduo网络库的地方。`HttpServer`中主要实现的重要的块分别是新连接建立时回调、接收连接数据的消息回调、对业务层提供业务回调接口等功能。下面我逐一为大家介绍。

#### 成员变量

```cpp
muduo::net::InetAddress                      listenAddr_; // 监听地址
muduo::net::TcpServer                        server_; 
muduo::net::EventLoop                        mainLoop_; // 主循环
HttpCallback                                 httpCallback_; // 回调函数
Router                                       router_; // 路由
```

#### 新连接建立时回调(onConnection)

每次客户端连接服务端之后都会调用这个函数，该函数是通过Muduo网络库中提供的接口注册上的。

```cpp
void HttpServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    // 给连接设置一个HttpContext对象用于解析请求报文，提取封装请求信息
    if (conn->connected())
    {
        conn->setContext(HttpContext());
    }
}
```

#### 接收连接数据的消息回调（onMessage）

服务端程序在接收到跟服务端建立连接的客户端的数据时，会调用该函数，同样是通过Muduo网络库中提供的接口注册上的。

```cpp
void HttpServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buf,
                           muduo::Timestamp receiveTime)
{
    try
    {
        // HttpContext对象用于解析出buf中的请求报文，并把报文的关键信息封装到HttpRequest对象中
        HttpContext *context = boost::any_cast<HttpContext>(conn->getMutableContext());
        if (!context->parseRequest(buf, receiveTime)) // 解析一个http请求
        {
            // 如果解析http报文过程中出错
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
        }

        // 如果buf缓冲区中解析出一个完整的数据包才封装响应报文
        if (context->gotAll())
        {
            onRequest(conn, context->request());
            context->reset();
        }
    }
    catch (const std::exception &e)
    {
        // 捕获异常，返回错误信息
        LOG_ERROR << "Exception in onMessage: " << e.what();
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
}
```

#### 当接收到消息回调后调用的处理

```cpp
void HttpServer::onRequest(const muduo::net::TcpConnectionPtr &conn, const HttpRequest &req)
{
    const std::string &connection = req.getHeader("Connection");
    bool close = ((connection == "close") ||
                  (req.getVersion() == "HTTP/1.0" && connection != "Keep-Alive"));
    HttpResponse response(close);
    
    // 处理OPTIONS请求
    if (req.method() == HttpRequest::kOptions)
    {
        response.setStatusLine(req.getVersion(), HttpResponse::k200Ok, "OK");
        response.addHeader("Access-Control-Allow-Origin", "*");
        response.addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        response.addHeader("Access-Control-Allow-Headers", "Content-Type");
        response.addHeader("Access-Control-Max-Age", "86400");

        muduo::net::Buffer buf;
        response.appendToBuffer(&buf);
        conn->send(&buf);
        return;
    }

    // 根据请求报文信息来封装响应报文对象
    httpCallback_(req, &response); // 执行onHttpCallback函数

    // 可以给response设置一个成员，判断是否请求的是文件，如果是文件设置为true，并且存在文件位置在这里send出去。
    muduo::net::Buffer buf;
    response.appendToBuffer(&buf);

    conn->send(&buf);

    // 如果是短连接的话，返回响应报文后就断开连接
    if (response.closeConnection())
    {
        conn->shutdown();
    }
}

// 执行请求对应的路由处理函数
void HttpServer::onHttpCallback(const HttpRequest &req, HttpResponse *resp)
{
    if (!router_.route(req, resp))
    {
        std::cout << "请求的啥，url：" << req.method() << " " << req.path() << std::endl;
        std::cout << "未找到路由，返回404" << std::endl;
        resp->setStatusCode(HttpResponse::k404NotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }
}
```

#### 对业务层提供业务回调接口

业务层可以通过这些提供的接口，注册 URI -> 回调函数。大家可能会疑惑为什么有的第二个参数是`const HttpCallback& cb`，有的第二个参数是`Router::HandlerPtr handler`，这是因为该HTTP框架提供了两种注册回调的方式：一种是直接注册可执行的函数对象，另一种是通过继承`RouterHandler`基类，重写相应的执行函数（这里看不懂没事，后面会介绍`RouterHandler`）。前者适用于回调函数执行简单的场景，后者适用于回调函数执行更复杂的场景，不过总的来说他们都是注册请求URI对应执行的回调函数。

```cpp
    // 注册静态路由处理器
    void Get(const std::string& path, const HttpCallback& cb)
    {
        router_.registerCallback(HttpRequest::kGet, path, cb);
    }
    
    // 注册静态路由处理器
    void Get(const std::string& path, Router::HandlerPtr handler)
    {
        router_.registerHandler(HttpRequest::kGet, path, handler);
    }

    void Post(const std::string& path, const HttpCallback& cb)
    {
        router_.registerCallback(HttpRequest::kPost, path, cb);
    }

    void Post(const std::string& path, Router::HandlerPtr handler)
    {
        router_.registerHandler(HttpRequest::kPost, path, handler);
    }

    // 注册动态路由处理器
    void addRoute(HttpRequest::Method method, const std::string& path, Router::HandlerPtr handler)
    {
        router_.addRegexHandler(method, path, handler);
    }

    // 注册动态路由处理函数
    void addRoute(HttpRequest::Method method, const std::string& path, const Router::HandlerCallback& callback)
    {
        router_.addRegexCallback(method, path, callback);
    }
```

###


> 更新: 2025-09-26 16:34:30  
> 原文: <https://www.yuque.com/chengxuyuancarl/imh9xc/cvyq9iyyeuqs7n6w>