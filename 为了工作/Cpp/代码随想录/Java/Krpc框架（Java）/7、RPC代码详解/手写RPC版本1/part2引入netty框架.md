# part2 引入netty框架

<font style="color:rgb(55, 65, 81);">在客户端与服务端进行网络传输，采用Java原生的socket编程方式，效率低</font>

<font style="color:rgb(55, 65, 81);">引入</font>`netty`<font style="color:rgb(55, 65, 81);">高性能网络框架，进行优化</font>

# <font style="color:rgb(55, 65, 81);">netty和传统socket编程相比有哪些优势?</font>

* <font style="color:rgb(55, 65, 81);">io传输由BIO ->NIO模式；底层 池化技术复用资源</font>
* <font style="color:rgb(55, 65, 81);">可以自主编写 编码/解码器，序列化器等等，可拓展性和灵活性高</font>
* <font style="color:rgb(55, 65, 81);">支持TCP,UDP多种传输协议；支持堵塞返回和异步返回</font>
* <font style="color:rgb(55, 65, 81);">...</font>

<font style="color:rgb(55, 65, 81);">\[netty知识点]  </font>[【硬核】肝了一月的Netty知识点-CSDN博客](https://blog.csdn.net/qq_35190492/article/details/113174359?spm=1001.2014.3001.5506%C2%A0)

<font style="color:rgb(55, 65, 81);">引入netty，就是在part1中的【信息传输部分】进行代码的重构</font>

# <font style="color:rgb(55, 65, 81);">在pom.xml中引入netty包</font>

```java
<dependency>
    <groupId>io.netty</groupId>
    <artifactId>netty-all</artifactId>
    <version>4.1.51.Final</version>
    <scope>compile</scope>
</dependency>
```

# Netty 客户端部分

创建 rpcClient 包->RpcClient 接口->Impl 包-> SimpleSocketRpcClient 类->netty 包->handler 包->nettyInitializer 包->NettyClientHandler 类->NettyClientInitializer 类->NettyRpcClient 类->修改 ClientProxy->修改 TestClient

## RpcClient

把共性抽取出来，通过 request 获取 response 的方法

```java
public interface   RpcClient {

    //共性抽取出来，定义底层通信的方法
    RpcResponse sendRequest(RpcRequest request);
}
```

## SimpleSocketRpcClient

这是一个简单实现的客户端，sendRequest 方法也与 IOClient 中相同。

```java
public class SimpleSocketRpcClient implements RpcClient {
    private String host;//主机地址
    private int port;//端口号
    public SimpleSocketRpcCilent(String host,int port){
        this.host=host;
        this.port=port;
    }
    @Override
    public RpcResponse sendRequest(RpcRequest request) {
        try {
            Socket socket=new Socket(host, port);
            ObjectOutputStream oos=new ObjectOutputStream(socket.getOutputStream());
            ObjectInputStream ois=new ObjectInputStream(socket.getInputStream());

            oos.writeObject(request);
            oos.flush();

            RpcResponse response=(RpcResponse) ois.readObject();
            return response;
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
            return null;
        }
    }
}
```

## NettyClientHandler

```java
public class NettyClientHandler extends SimpleChannelInboundHandler<RpcResponse> {
    //这是 SimpleChannelInboundHandler 的核心方法，用于读取服务端返回的数据。
    @Override
    protected void channelRead0(ChannelHandlerContext ctx, RpcResponse response) throws Exception {
        // 接收到response, 给channel设计别名，让sendRequest里读取response
        // 将服务端返回的 RpcResponse 绑定到当前 Channel 的属性中，以便后续逻辑能够通过 Channel 获取该响应数据
        AttributeKey<RpcResponse> key = AttributeKey.valueOf("RPCResponse");
        ctx.channel().attr(key).set(response);
        // 关闭当前 Channel（短连接模式）
        ctx.channel().close();
    }
    //用于捕获运行过程中出现的异常，进行处理并释放资源。
    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        //异常处理
        cause.printStackTrace();
        ctx.close();
    }
}
```

### 1.本类的作用是什么？

实现了一个 `NettyClientHandler` 类，继承自 `SimpleChannelInboundHandler<RpcResponse>`，是 Netty 中用于处理服务器端响应的处理器。主要功能是接收来自服务器的 `RpcResponse` 对象，并在处理过程中管理连接的生命周期。

### 2.为什么要继承SimpleChannelInboundHandler<RpcResponse>类？

* 这个类继承自 `SimpleChannelInboundHandler<RpcResponse>`，`SimpleChannelInboundHandler` 是一个通用的 Netty 处理器基类，它为每个事件提供了默认的实现，避免我们每次都需要去实现 `channelRead()` 等方法。
* 这里 `RpcResponse` 表示服务端返回的响应数据的类型，`NettyClientHandler` 主要是处理这种类型的消息。

### 3.channelRead0 方法的作用？参数代表什么意思？

`channelRead0` 是 `SimpleChannelInboundHandler` 中的核心方法，用于处理接收到的消息。在这个方法中，客户端从服务器接收到 `RpcResponse` 对象，通常是 RPC 调用的结果。

* `ctx.channel()` 获取当前的 Channel（即与服务端的网络连接）。
* `AttributeKey.valueOf("RPCResponse")` 创建一个 `AttributeKey`，这是用来存储和检索 Channel 上的属性。
* `ctx.channel().attr(key).set(response)` 将服务器返回的 `RpcResponse` 存储到当前的 `Channel` 上，确保在之后的处理流程中能够获取到这个响应。这样做的原因是 Netty 中的 Channel 是与 I/O 操作绑定的，允许每个连接有自己的状态或上下文，这里是将响应存储在 Channel 上，方便后续操作。
* ctx.channel().close();   在接收到响应后，主动关闭当前的连接，采用短连接模式。

#### 参数

`ctx` 是 `ChannelHandlerContext` 类型的参数，它是 Netty 中每个处理器（`ChannelHandler`）的上下文对象，代表了当前 I/O 操作的环境。

response 是返回的响应信息。

## NettyClientInitializer

```java
public class NettyClientInitializer extends ChannelInitializer<SocketChannel> {
    @Override
    protected void initChannel(SocketChannel ch) throws Exception {
        //初始化，每个 SocketChannel 都有一个独立的管道（Pipeline），用于定义数据的处理流程。
        ChannelPipeline pipeline = ch.pipeline();
        //消息格式 【长度】【消息体】，解决沾包问题
        /*
        参数含义：
        Integer.MAX_VALUE：允许的最大帧长度。
        0, 4：表示长度字段的起始位置和长度。
        0, 4：去掉长度字段后，计算实际数据的偏移量。
         */
        pipeline.addLast(
            new LengthFieldBasedFrameDecoder(Integer.MAX_VALUE,0,4,0,4));
        //计算当前待发送消息的长度，写入到前4个字节中
        pipeline.addLast(new LengthFieldPrepender(4));
        //编码器
        //使用Java序列化方式，netty的自带的解码编码支持传输这种结构
        pipeline.addLast(new ObjectEncoder());
        //解码器
        //使用了Netty中的ObjectDecoder，它用于将字节流解码为 Java 对象。
        //在ObjectDecoder的构造函数中传入了一个ClassResolver 对象，用于解析类名并加载相应的类。
        pipeline.addLast(new ObjectDecoder(new ClassResolver() {
            @Override
            public Class<?> resolve(String className) throws ClassNotFoundException {
                return Class.forName(className);
            }
        }));

        pipeline.addLast(new NettyClientHandler());
    }
}
```

### 1.本类的作用是什么？

实现了一个 `NettyClientInitializer` 类，继承自 `ChannelInitializer<SocketChannel>`，用于初始化客户端的 `Channel` 和 `ChannelPipeline`。在 Netty 中，`Channel` 是网络通信的基本单元，而 `ChannelPipeline` 是一个用于处理消息的责任链，它包含了一系列的 `ChannelHandler`，每个 `ChannelHandler` 都负责处理不同的操作，如编码、解码、异常处理等。

### 2. 为什么要继承ChannelInitializer<SocketChannel>？

* `ChannelInitializer` 是一个抽象类，用于初始化 `Channel`。在这个类中，我们会配置处理网络数据的 `ChannelPipeline`。它是 Netty 中为每个连接提供的初始化类。
* `SocketChannel` 是 Netty 中的一个实现，代表客户端的网络连接，通常用于 TCP 连接。

### 3.讲讲initChannel 方法

* `initChannel(SocketChannel ch)` 方法用于初始化每个新的 `SocketChannel`（即新的连接）。每个 `SocketChannel` 会有一个独立的 `ChannelPipeline`，用于定义该连接上所有数据的处理流程。
* `ChannelPipeline` 是 Netty 中的数据处理链，它由多个 `ChannelHandler` 组成，每个 `ChannelHandler` 可以进行数据的处理、编解码、错误处理等任务。

### pipeline.addLast( newLengthFieldBasedFrameDecoder(Integer.MAX\_VALUE, 0, 4, 0, 4));

* `LengthFieldBasedFrameDecoder` 用于处理基于长度字段的帧解码。这是为了解决 **粘包** 和 **拆包** 问题，常见于 TCP 协议中，数据包的边界不明确时。
* 具体地，它根据消息头中包含的长度字段来解析消息体，保证从网络中读取的数据是一个完整的消息。

参数说明：

* `Integer.MAX_VALUE`：允许的最大帧长度，表示消息体的最大长度。
* `0, 4`：表示消息中长度字段的位置，`0` 代表从消息的起始位置读取，`4` 代表长度字段的长度为 4 字节。
* `0, 4`：指明在解码后，从第 4 个字节开始计算实际的数据部分。

### pipeline.addLast(newLengthFieldPrepender(4));

* `LengthFieldPrepender` 用于在发送数据前，自动计算消息体的长度并将其添加到消息的前面。
* 它将消息的长度作为一个 4 字节的字段（即前四个字节）写入消息头，以便接收方能知道消息的长度。

在这里，`4` 表示长度字段的长度是 4 字节。

### pipeline.addLast(newObjectEncoder());

* `ObjectEncoder` 是一个编码器，它将 Java 对象转换为字节流进行传输。
* 这里使用了 Java 的默认序列化机制，将待发送的对象（例如 `RpcRequest` 或其他对象）编码成字节流，准备通过网络发送。

### 解码器

```java
pipeline.addLast(new ObjectDecoder(new ClassResolver() {
    @Override
    public Class<?> resolve(String className) throws ClassNotFoundException {
        return Class.forName(className);
    }
}));
```

* `ObjectDecoder` 是一个解码器，它将接收到的字节流解码回 Java 对象。
* `ClassResolver` 是一个接口，用于提供如何根据类名加载类的策略。在这里，它的 `resolve` 方法通过 `Class.forName(className)` 动态加载类。
* 这使得解码器能够根据收到的类名解析出相应的 Java 类，并将字节流转换为该类的对象。

最后把 NettyClientHandler  处理器加到容器中。

## NettyRpcClient

```java
public class NettyRpcClient implements RpcClient {
    private String host;
    private int port;
    private static final Bootstrap bootstrap;
    private static final EventLoopGroup eventLoopGroup;
    public NettyRpcClient(String host,int port){
        this.host=host;
        this.port=port;
    }
    //netty客户端初始化
    static {
        eventLoopGroup = new NioEventLoopGroup();
        bootstrap = new Bootstrap();
        bootstrap.group(eventLoopGroup).channel(NioSocketChannel.class)
        //NettyClientInitializer这里 配置netty对消息的处理机制
        .handler(new NettyClientInitializer());
    }
    @Override
    public RpcResponse sendRequest(RpcRequest request) {
        try {
            //创建一个channelFuture对象，代表这一个操作事件，sync方法表示堵塞直到connect完成
            ChannelFuture channelFuture  = bootstrap.connect(host, port).sync();
            //channel表示一个连接的单位，类似socket
            Channel channel = channelFuture.channel();
            // 发送数据
            channel.writeAndFlush(request);
            //sync()堵塞获取结果
            channel.closeFuture().sync();
            // 阻塞的获得结果，通过给channel设计别名，获取特定名字下的channel中的内容（这个在hanlder中设置）
            // AttributeKey是，线程隔离的，不会由线程安全问题。
            // 当前场景下选择堵塞获取结果
            // 其它场景也可以选择添加监听器的方式来异步获取结果 channelFuture.addListener...
            AttributeKey<RpcResponse> key = AttributeKey.valueOf("RPCResponse");
            RpcResponse response = channel.attr(key).get();

            System.out.println(response);
            return response;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return null;
    }
}
```

### 1.bootstrap 和 eventLoopGroup 字段的作用是什么？

* <code>**bootstrap**</code>：是 Netty 用于启动客户端的对象，负责设置与服务器的连接配置。
* <code>**eventLoopGroup**</code>：是 Netty 的线程池，用于处理 I/O 操作。`NioEventLoopGroup` 是基于 NIO（非阻塞 I/O）实现的，适用于客户端和服务器的网络通信。

### 2.netty 客户端为什么需要初始化？

客户端初始化的步骤主要是为了配置和准备 `Netty` 网络通信所需的各种资源，使得客户端能够正确地与服务器建立连接、发送请求、处理响应等。

* <code>**NioEventLoopGroup**</code>：是 Netty 的事件循环组，负责管理所有的 I/O 线程。每个线程负责处理一个或多个 `Channel` 的 I/O 操作。
* <code>**Bootstrap**</code>：是 Netty 客户端的启动类，用来设置客户端连接的相关配置，包括 I/O 线程池、连接类型、消息处理器等。
* `.group(eventLoopGroup)`：将 `eventLoopGroup` 设置为客户端的 I/O 线程池。
* `.channel(NioSocketChannel.class)`：指定使用 NIO 传输通道（`NioSocketChannel`）来建立 TCP 连接。
* `.handler(new NettyClientInitializer())`：设置 `NettyClientInitializer`，这是自定义的处理器初始化器，负责配置客户端的消息处理逻辑（例如编码、解码、消息处理等）。

### 3.讲讲重写的 sendRequest 方法

#### ChannelFuturechannelFuture= bootstrap.connect(host, port).sync();

<code>**ChannelFuture**</code>：表示一个异步操作的结果。`bootstrap.connect(host, port).sync()` 表示客户端发起连接操作，并调用 `sync()` 方法进行阻塞，直到连接成功或失败。

* `connect(host, port)`：发起连接到指定的服务器。
* `sync()`：是一个阻塞方法，直到连接建立完成。

#### Channelchannel= channelFuture.channel();

获取与远程服务器建立的 `Channel`。`Channel` 表示一个网络连接，客户端和服务器之间的所有数据都通过这个 `Channel` 进行传输。

#### channel.writeAndFlush(request);

通过 `channel.writeAndFlush(request)` 将 `RpcRequest` 请求发送到远程服务器。`writeAndFlush()` 会将消息写入并立刻发送出去。

#### channel.closeFuture().sync();

`channel.closeFuture().sync()`：这是一个阻塞操作，直到连接被关闭。在这里，它等待服务端返回结果后，客户端的连接才会关闭。

#### AttributeKey<RpcResponse> key = AttributeKey.valueOf("RPCResponse");

通过 `AttributeKey` 获取 `RpcResponse` 对象。`AttributeKey` 用于在 `Channel` 中存储和检索特定的数据，这里我们把 `RpcResponse` 绑定到 `Channel` 上，并通过 `key` 获取。

#### RpcResponse response= channel.attr(key).get();

* `channel.attr(key).get()`：从 `Channel` 中获取与 `key` 对应的 `RpcResponse` 对象。这个 `RpcResponse` 是在处理响应时存储在 `Channel` 上的。

## 修改 ClientProxy

可选择 Netty 客户端或简单实现客户端

```java
//传入参数service接口的class对象，反射封装成一个request
//RPCClientProxy类中需要加入一个RPCClient类变量即可， 传入不同的client(simple,netty), 即可调用公共的接口sendRequest发送请求
private RpcClient rpcClient;
public ClientProxy(String host,int port,int choose){
    switch (choose){
        case 0:
            rpcClient=new NettyRpcClient(host,port);
            break;
        case 1:
            rpcClient=new SimpleSocketRpcCilent(host,port);
    }
}
public ClientProxy(String host,int port){
    rpcClient=new NettyRpcClient(host,port);
}
```

## 修改 TestClient

创建代理对象参数改一下，看选择哪个客户端

```java
public class TestClient {
    public static void main(String[] args) {
        ClientProxy clientProxy=new ClientProxy("127.0.0.1",9999,0);
        UserService proxy=clientProxy.getProxy(UserService.class);

        User user = proxy.getUserByUserId(1);
        System.out.println("从服务端得到的user="+user.toString());

        User u=User.builder().id(100).userName("wxx").sex(true).build();
        Integer id = proxy.insertUserId(u);
        System.out.println("向服务端插入user的id"+id);
    }
}
```

# Netty 服务端部分

创建 netty 包->handler 包->nettyInitializer 包->NettyServerHandler 类->NettyServerInitializer 类->NettyRpcServer 类->修改 TestServer 类

## NettyServerHandler

getResponse 与 WorkThread 类中的一样直接 copy 过来。

```java
@AllArgsConstructor
public class NettyRPCServerHandler extends SimpleChannelInboundHandler<RpcRequest> {
    private ServiceProvider serviceProvider;
    @Override
    protected void channelRead0(ChannelHandlerContext ctx, RpcRequest request) throws Exception {
        //接收request，读取并调用服务
        RpcResponse response = getResponse(request);
        ctx.writeAndFlush(response);
        ctx.close();
    }
    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();
        ctx.close();
    }
    private RpcResponse getResponse(RpcRequest rpcRequest){
        //得到服务名
        String interfaceName=rpcRequest.getInterfaceName();
        //得到服务端相应服务实现类
        Object service = serviceProvider.getService(interfaceName);
        //反射调用方法
        Method method=null;
        try {
            method= service.getClass().getMethod(rpcRequest.getMethodName(), rpcRequest.getParamsType());
            Object invoke=method.invoke(service,rpcRequest.getParams());
            return RpcResponse.sussess(invoke);
        } catch (NoSuchMethodException | IllegalAccessException | InvocationTargetException e) {
            e.printStackTrace();
            System.out.println("方法执行错误");
            return RpcResponse.fail();
        }
    }
}
```

### 1. 讲讲重写的channelRead0

\*\*1. 调用 \*\*<code>**getResponse(request)**</code>：

* 在接收到客户端的 `RpcRequest` 后，首先调用 `getResponse(request)` 方法来处理这个请求并生成响应（`RpcResponse`）。
* `getResponse` 方法通过反射机制，根据请求中的方法名和参数调用对应的服务实现类中的方法。调用完成后会返回 `RpcResponse`。

**2. 发送响应**：

* 调用 `ctx.writeAndFlush(response)` 将返回的 `RpcResponse` 发送回客户端。`writeAndFlush` 会将数据写入到 `Channel` 并立即发送出去。
* 需要注意的是，这里使用了 `flush`，意味着会把消息立即推送到网络层，不会被缓存在本地。

**3. 关闭连接**：

* `ctx.close()` 关闭当前的 `Channel` 连接。`channelRead0` 方法是服务器端处理请求的地方，在处理完请求并发送响应后，通常会关闭连接。在这个实现中，连接关闭操作是在响应发送后进行的，意味着服务端处理完请求并回复之后，连接就被关闭了。

## NettyServerInitializer

添加个本地注册中心字段，其余与\*\*<font style="color:rgb(38, 38, 38);">NettyClientInitializer 基本一致，</font>\*\*<font style="color:rgb(38, 38, 38);">最后把</font>new NettyRPCServerHandler(serviceProvider)添加到容器中。

```java
@AllArgsConstructor
public class NettyServerInitializer extends ChannelInitializer<SocketChannel> {
    private ServiceProvider serviceProvider;
    @Override
    protected void initChannel(SocketChannel ch) throws Exception {
        ChannelPipeline pipeline = ch.pipeline();
        //消息格式 【长度】【消息体】，解决沾包问题
        pipeline.addLast(
            new LengthFieldBasedFrameDecoder(Integer.MAX_VALUE,0,4,0,4));
        //计算当前待发送消息的长度，写入到前4个字节中
        pipeline.addLast(new LengthFieldPrepender(4));

        //使用Java序列化方式，netty的自带的解码编码支持传输这种结构
        pipeline.addLast(new ObjectEncoder());
        //使用了Netty中的ObjectDecoder，它用于将字节流解码为 Java 对象。
        //在ObjectDecoder的构造函数中传入了一个ClassResolver 对象，用于解析类名并加载相应的类。
        pipeline.addLast(new ObjectDecoder(new ClassResolver() {
            @Override
            public Class<?> resolve(String className) throws ClassNotFoundException {
                return Class.forName(className);
            }
        }));
        //将 NettyRPCServerHandler 添加到 ChannelPipeline 中，使其成为数据处理链中的一个环节，负责处理客户端发送的 RpcRequest
        pipeline.addLast(new NettyRPCServerHandler(serviceProvider));
    }
}
```

## NettyRpcServer

### 1. 在 netty 服务端中是如何监听请求的？

在 **Netty** 服务端中，监听请求是通过 <code>**ServerBootstrap**</code> 和 <code>**NioServerSocketChannel**</code> 来实现的。

```java
ServerBootstrap serverBootstrap = new ServerBootstrap();
serverBootstrap.group(bossGroup, workGroup)
               .channel(NioServerSocketChannel.class)
               .childHandler(new NettyServerInitializer(serviceProvider));
```

* <code>**ServerBootstrap**</code> 是 Netty 中用于启动服务端的核心类。它用于配置和启动 Netty 服务端。
* <code>**group(bossGroup, workGroup)**</code>：将两个线程组 (`bossGroup` 和 `workGroup`) 配置到 `ServerBootstrap` 中，分别用于处理连接请求和处理 I/O 操作。
  * <code>**bossGroup**</code>：用于处理客户端的连接请求（建立连接的过程）。
  * <code>**workGroup**</code>：用于处理客户端连接后的数据读写（数据传输、消息处理）。
* <code>**channel(NioServerSocketChannel.class)**</code>：指定服务端使用的 **NIO 服务器套接字通道**。`NioServerSocketChannel` 是 Netty 中专门用于接收 **TCP 连接** 的通道类，它用于监听端口、接收连接请求等操作。
  * <code>**NioServerSocketChannel**</code> 是基于 NIO 的服务端通道，它会自动使用非阻塞 I/O 方式，处理并发连接时具有很高的效率。

<code>**channel**</code> 代表了一个服务器套接字，它在后台依赖 NIO（Java NIO API）来监听客户端的连接。

#### ChannelFuture channelFuture =  serverBootstrap.bind(port).sync();

* <code>**bind(port)**</code>：将服务端绑定到指定端口（这里的 `port` 是客户端连接的目标端口）。调用 `bind` 方法后，Netty 会启动一个 **监听套接字**，并开始监听客户端的连接请求。
* <code>**sync()**</code>：这是一个同步方法，它会阻塞当前线程，直到绑定操作完成，确保服务器已经开始监听端口。

调用 `bind()` 会创建一个 <code>**ServerSocketChannel**</code>，然后它会开始监听指定的端口，等待客户端发起连接。此时，服务器已经进入 **监听状态**，随时准备接受来自客户端的连接。

### 2.为什么要分为两个线程组？

在 **Netty** 中，`NioEventLoopGroup` 被用来管理所有的 I/O 操作，而分为 **bossGroup** 和 **workGroup** 两个线程组是为了更好地分离 **连接接收** 和 **数据处理** 的职责，提升性能和可扩展性。

* <code>**bossGroup**</code>：负责处理 **客户端连接** 的建立。当客户端连接到服务器时，`bossGroup` 会接管与客户端的连接建立过程，并将连接交给 `workGroup` 来处理后续的通信。
* 具体来说，`bossGroup` 负责监听指定的端口，接受客户端的连接请求，并为每个连接创建一个新的 **Channel**。这时连接的创建是一个非常轻量级的操作，通常只涉及到网络 I/O 和连接的注册，后续的 I/O 操作交由 `workGroup` 处理。
* <code>**workGroup**</code>：负责处理 **客户端数据的读写** 操作。连接建立后，`workGroup` 会负责处理网络数据的读写和事件的处理。这包括处理网络读写、消息的编解码、业务逻辑处理等。

**并行处理**：通过将 `bossGroup` 和 `workGroup` 分开，Netty 可以实现更好的 **负载均衡** 和 **并行处理**。`bossGroup` 专注于连接的接收，`workGroup` 专注于连接的处理，它们可以各自独立工作，而不会相互阻塞。

* 比如，`bossGroup` 负责接收客户端的连接，`workGroup` 负责处理连接的数据传输。这样即使有多个客户端连接到服务器，`bossGroup` 和 `workGroup` 可以同时工作，从而提升服务器的并发性能。

**线程分配**：每个 `NioEventLoopGroup` 实际上是由多个线程组成的，`bossGroup` 负责接收连接的线程通常比较少，因为接收连接的频率较低，而 `workGroup` 的线程可以更多，因为每个连接需要处理大量的数据传输操作。通过这样合理的线程分配，可以避免某一组线程成为性能瓶颈。

## 修改 TestServer

把启动服务端改为 Netty 服务端

```java
public class TestServer {
    public static void main(String[] args) {
        UserService userService=new UserServiceImpl();

        ServiceProvider serviceProvider=new ServiceProvider();
        serviceProvider.provideServiceInterface(userService);

        RpcServer rpcServer=new NettyRPCRPCServer(serviceProvider);
        rpcServer.start(9999);
    }
}
```


> 更新: 2024-11-29 18:04:54  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/ukma3iggny7xs9f5>