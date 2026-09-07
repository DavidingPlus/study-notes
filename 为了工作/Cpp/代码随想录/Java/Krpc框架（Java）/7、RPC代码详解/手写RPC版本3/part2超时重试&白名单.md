# part2 超时重试 &白名单

# 为什么需要超时重试？

<font style="color:rgb(55, 65, 81);">一次 RPC 调用，去调用远程的一个服务，比如用户的登录操作，会先对用户的用户名以及密码进行验证，验证成功之后会获取用户的基本信息。当通过远程的用户服务来获取用户基本信息的时候，恰好网络出现了问题，比如网络突然抖了一下，导致请求失败了，而这个请求希望它能够尽可能地执行成功，那这时要怎么做呢？</font>

<font style="color:rgb(55, 65, 81);">当调用端发起的请求失败时，RPC 框架自身可以进行重试，再重新发送请求，通过这种方式保证系统的容错率</font>

# 如何实现超时重试？

<font style="color:rgb(55, 65, 81);">java中有多种实现重试的方式，如手动重试，Spring Retry ，Guava Retry ...</font>

<font style="color:rgb(55, 65, 81);">这里我们选择灵活性和功能性更强的Guava Retry</font>

# 重试机制存在什么问题？

<font style="color:rgb(55, 65, 81);">如果这个服务业务逻辑不是幂等的，比如插入数据操作，那触发重试的话会不会引发问题呢？</font>

**<font style="color:rgb(55, 65, 81);">会的。</font>**

<font style="color:rgb(55, 65, 81);">在使用 RPC 框架的时候，要确保被调用的服务的业务逻辑是幂等的，这样才能考虑根据事件情况开启 RPC 框架的异常重试功能</font>

<font style="color:rgb(55, 65, 81);">所以，我们可以</font>**设置一个白名单**<font style="color:rgb(55, 65, 81);">，服务端在注册节点时，将幂等性的服务注册在白名单中，客户端在请求服务前，先去白名单中查看该服务是否为幂等服务，如果是的话使用重试框架进行调用</font>

<font style="color:rgb(55, 65, 81);">白名单可以存放在zookeeper中（充当配置中心的角色）</font>

# <font style="color:rgb(55, 65, 81);">整体顺序</font>

导包->在 Client 下创建包 retry->创建 guavaRetry 类->修改serviceCenter接口->

在ZKServiceCenter中实现方法->修改ZKServiceRegister->修改ClientProxy

# guavaRetry

```java
public class guavaRetry {
    //用来发送RPC请求
    private RpcClient rpcClient;

    public RpcResponse sendServiceWithRetry(RpcRequest request, RpcClient rpcClient) {
        this.rpcClient = rpcClient;
        Retryer<RpcResponse> retryer = RetryerBuilder.<RpcResponse>newBuilder()
        .retryIfException()
        //重试会在请求发生异常或返回状态码为500时进行
        .retryIfResult(response -> Objects.equals(response.getCode(), 500))
        //每次重试之间固定等待2秒
        .withWaitStrategy(WaitStrategies.fixedWait(2, TimeUnit.SECONDS))
        //最多重试3次
        .withStopStrategy(StopStrategies.stopAfterAttempt(3))
        //用于为 Retryer 配置一个重试监听器
        .withRetryListener(new RetryListener() {
            @Override
            public <V> void onRetry(Attempt<V> attempt) {
                System.out.println("RetryListener: 第" + attempt.getAttemptNumber() + "次调用");
            }
        })
        .build();
        try {
            // retryer.call执行RPC请求，进行重试。
            //传入的 Lambda 表达式是要执行的操作，即 rpcClient.sendRequest(request)，这会发送请求并返回一个 RpcResponse 对象。
            return retryer.call(() -> rpcClient.sendRequest(request));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return RpcResponse.fail();
    }
}
```

<font style="color:rgb(55, 65, 81);">通过很多方法来设置重试机制：</font>

<font style="color:rgb(55, 65, 81);">retryIfException()：对所有异常进行重试 retryIfRuntimeException()：设置对指定异常进行重试 retryIfExceptionOfType()：对所有 RuntimeException 进行重试 retryIfResult()：对不符合预期的返回结果进行重试</font>

<font style="color:rgb(55, 65, 81);">还有五个以 withXxx 开头的方法，用来对重试策略/等待策略/阻塞策略/单次任务执行时间限制/自定义监听器进行设置，以实现更加强大的异常处理：</font>

<font style="color:rgb(55, 65, 81);">withRetryListener()：设置重试监听器，用来执行额外的处理工作 withWaitStrategy()：重试等待策略 withStopStrategy()：停止重试策略 withAttemptTimeLimiter：设置任务单次执行的时间限制，如果超时则抛出异常 withBlockStrategy()：设置任务阻塞策略，即可以设置当前重试完成，下次重试开始前的这段时间做什么事情</font>

## 1.如何实现的异常重试？

使用了 **Guava Retry** 库来实现对 RPC 请求的重试机制。通过设置重试策略，可以在发生异常或服务器返回特定错误时，尝试重新发起请求。代码中的 `Retryer` 通过 Guava 提供的 `RetryerBuilder` 来配置重试的策略。

```java
try {
    return retryer.call(() -> rpcClient.sendRequest(request));
} catch (Exception e) {
    e.printStackTrace();
}
return RpcResponse.fail();
```

* <code>**retryer.call(() -> rpcClient.sendRequest(request))**</code>：
  * 通过 `retryer.call()` 方法执行实际的 RPC 请求，并进行重试。传入的 Lambda 表达式是要执行的操作，即 `rpcClient.sendRequest(request)`，这会发送请求并返回一个 `RpcResponse` 对象。
  * 如果请求成功且返回结果没有满足重试条件（例如没有抛出异常或返回 500 错误），`retryer.call()` 会返回该结果。
* <code>**catch (Exception e)**</code>：
  * 如果 `retryer.call()` 在执行过程中抛出任何异常（例如所有重试都失败），则进入 `catch` 块，打印堆栈信息。
  * 这确保了即使在重试过程中发生错误，也能捕获并处理异常。
* <code>**return RpcResponse.fail();**</code>：
  * 如果发生异常并且重试未能成功，最终返回一个失败的 `RpcResponse`。假设 `RpcResponse.fail()` 是一个静态方法，用于返回一个表示失败的响应。

## 2.sendServiceWithRetry(RpcRequest request, RpcClient rpcClient)方法的作用是什么？

本方法是用来发送一个 RPC 请求，并在发生某些特定错误时自动进行重试的。它封装了发送请求的过程，并通过 **Guava Retry** 库实现了重试机制

方法接受两个参数：

* `RpcRequest request`：表示要发送的 RPC 请求。
* `RpcClient rpcClient`：表示用来发送请求的客户端对象。

## 3. 构建 `Retryer` 对象  是为了做什么？

* <code>**RetryerBuilder.newBuilder()**</code>：
  * 创建一个新的 `Retryer` 构建器，用于配置重试逻辑。
* <code>**retryIfException()**</code>：
  * 设置当 RPC 请求抛出任何异常时进行重试。默认情况下，所有异常都会触发重试。如果希望限制特定类型的异常，可以使用 `retryIfExceptionOfType()` 来指定具体异常类型。
* <code>**retryIfResult(response -> Objects.equals(response.getCode(), 500))**</code>：
  * 设置重试条件，若响应的 `code` 为 500（服务器错误），则认为该请求需要重试。这里使用了一个 Lambda 表达式来判断响应的状态码。
  * 只有当响应对象的 `getCode()` 返回 500 时才会触发重试。
* <code>**withWaitStrategy(WaitStrategies.fixedWait(2, TimeUnit.SECONDS))**</code>：
  * 设置重试时的等待策略。在这种情况下，重试时每次会等待 2 秒。
  * `WaitStrategies.fixedWait()` 表示等待固定的时间，在每次重试之间等待 2 秒。
* <code>**withStopStrategy(StopStrategies.stopAfterAttempt(3))**</code>：
  * 设置停止策略。该策略指定重试的最大次数为 3 次。当重试达到 3 次时，会停止进一步的重试操作。
* <code>**withRetryListener(new RetryListener() {...})**</code>：
  * 设置一个监听器，用于监听重试过程中的各个事件。这里使用了一个匿名类来实现 `RetryListener` 接口。
  * 在每次重试时，`onRetry` 方法会被调用。这个方法通过 `attempt.getAttemptNumber()` 获取当前的重试次数，并打印出重试信息。

## 4.这段代码的作用？

```java
.withRetryListener(new RetryListener() {
    @Override
    public <V> void onRetry(Attempt<V> attempt) {
        System.out.println("RetryListener: 第" + attempt.getAttemptNumber() + "次调用");
    }
})
```

这段代码是为 **Guava Retry** 库的 `Retryer` 配置了一个 **重试监听器（**<code>**RetryListener**</code>**）**。`RetryListener` 允许你在每次重试时执行某些自定义的操作，比如记录日志、统计重试次数、或者做其他的业务逻辑处理。

### `withRetryListener` 的作用

* `withRetryListener` 是 **Guava Retry** 库中的一个方法，用于为 `Retryer` 配置一个重试监听器。
* 重试监听器（`RetryListener`）会在每次执行重试时被调用，你可以在监听器中定义你希望在重试过程中执行的逻辑。

### `RetryListener` 接口

`RetryListener` 是一个接口，它有一个方法 `onRetry`，该方法会在每次重试时被触发。`onRetry` 方法接收一个 `Attempt` 对象作为参数，`Attempt` 对象包含了当前重试的相关信息，例如当前是第几次重试、是否成功、上一次请求的结果等。

* <code>**onRetry(Attempt<V> attempt)**</code>：
  * `onRetry` 是 `RetryListener` 接口中的方法，它在每次重试时被调用。
  * `Attempt<V>` 是 Guava Retry 库中表示某次尝试的对象，`V` 代表重试操作返回的值类型。在这个场景中，`V` 代表 `RpcResponse` 类型，因为重试操作的结果是一个 `RpcResponse` 对象。
* <code>**attempt.getAttemptNumber()**</code>：
  * `getAttemptNumber()` 是 `Attempt` 类提供的方法，返回当前重试的次数（从 1 开始）。例如，如果这是第一次重试，它返回 1；如果是第二次重试，它返回 2，依此类推。
  * 这个方法是用来输出当前的重试次数，以便跟踪重试的进度。
* **打印重试信息**：
  * `System.out.println("RetryListener: 第" + attempt.getAttemptNumber() + "次调用");` 这一行代码用于在控制台打印当前重试的次数。当 `onRetry` 被调用时，每次重试的次数都会被打印出来，帮助开发者或运维人员了解系统的重试情况。

# 修改serviceCenter

```java
//服务中心接口
public interface ServiceCenter {
    //  查询：根据服务名查找地址
    InetSocketAddress serviceDiscovery(String serviceName);
    //判断是否可重试
    boolean checkRetry(String serviceName);
}
```

# 在ZKServiceCenter中实现方法

## List<String> serviceList = client.getChildren().forPath("/" + RETRY);做什么的？

* 这行代码使用了 **Zookeeper** 客户端（`client`）来获取 `/RETRY` 路径下的所有子节点。
* `client.getChildren()` 是一个 Zookeeper API 调用，用来获取指定路径下的所有子节点。这里的路径是 `"/" + RETRY`，假设 `RETRY` 表示 Zookeeper 中某个节点的路径。
* `forPath("/" + RETRY)` 是执行此操作的方法，返回一个 `List<String>`，即 Zookeeper 上 `/RETRY` 节点下所有子节点的名称，这些子节点的名称代表了可以重试的服务。

## 流程

先在变量里添加这句代码 private static final String RETRY = "CanRetry";

```java
// curator 提供的zookeeper客户端
private CuratorFramework client;
//zookeeper根路径节点
private static final String ROOT_PATH = "MyRPC";
private static final String RETRY = "CanRetry";
//serviceCache
private serviceCache cache;
```

## 实现白名单

**1 .初始化标志变量**：

* `canRetry` 用于标记该服务是否可以重试，初始值为 `false`。

**2 .获取服务白名单**：

* 通过 Zookeeper 客户端 (`client.getChildren()`) 获取 `/RETRY` 路径下的所有子节点，即所有被允许重试的服务名称。

**3 .遍历白名单**：

* 遍历获取到的服务列表，如果当前服务名称 (`s`) 与输入的 `serviceName` 相等，说明该服务在白名单中，设置 `canRetry = true`。

**4 .异常处理**：

* 如果 Zookeeper 操作发生异常，捕获并打印异常信息，确保不会因异常导致程序崩溃。

**5. 返回结果**：

* 返回 `canRetry`，如果服务在白名单中，返回 `true`，否则返回 `false`。

```java
@Override
public boolean checkRetry(String serviceName) {
boolean canRetry = false;
try {
    List<String> serviceList = client.getChildren().forPath("/" + RETRY);
    for (String s : serviceList) {
        if (s.equals(serviceName)) {
            System.out.println("服务" + serviceName + "在白名单上,可进行重试");
            canRetry = true;
        }
    }
} catch (Exception e) {
    e.printStackTrace();
}
return canRetry;
}
```

# 修改ZKServiceRegister

## 1. 先加这行 private static final String RETRY = "CanRetry";

```java
private CuratorFramework client;
//zookeeper根路径节点
private static final String ROOT_PATH = "MyRPC";
private static final String RETRY = "CanRetry";
```

## 2.在ServiceRegister 接口中加参数

```java
public interface ServiceRegister {
    //  注册：保存服务与地址。
    void register(String serviceName, InetSocketAddress serviceAddress, boolean canRetry);

}
```

## 3.ServiceProvider 中改参数

```java
public void provideServiceInterface(Object service,boolean canRetry){
    String serviceName=service.getClass().getName();
    Class<?>[] interfaceName=service.getClass().getInterfaces();

    for (Class<?> clazz:interfaceName){
        //本机的映射表
        interfaceProvider.put(clazz.getName(),service);
        //在注册中心注册服务
        serviceRegister.register(clazz.getName(),new InetSocketAddress(host,port),canRetry);
    }
}
```

## 4.改 TestServer

```java
public class TestServer {
    public static void main(String[] args) throws InterruptedException {
        UserService userService=new UserServiceImpl();

        ServiceProvider serviceProvider=new ServiceProvider("127.0.0.1",9999);

        serviceProvider.provideServiceInterface(userService,true);

        RpcServer rpcServer=new NettyRPCRPCServer(serviceProvider);
        rpcServer.start(9999);
    }
}
```

## 5. 如果是可重试的服务，添加到zookeeper 的分支中

```java
//注册服务到注册中心
@Override
public void register(String serviceName, InetSocketAddress serviceAddress, boolean canRetry) {
    try {
        // serviceName创建成永久节点，服务提供者下线时，不删服务名，只删地址
        if(client.checkExists().forPath("/" + serviceName) == null){
            client.create().creatingParentsIfNeeded().withMode(CreateMode.PERSISTENT).forPath("/" + serviceName);
        }
        // 路径地址，一个/代表一个节点
        String path = "/" + serviceName +"/"+ getServiceAddress(serviceAddress);
        // 临时节点，服务器下线就删除节点
        client.create().creatingParentsIfNeeded().withMode(CreateMode.EPHEMERAL).forPath(path);
        //如果这个服务是幂等性，就添加到节点中
        if (canRetry) {
            path += "/" + RETRY + "/" + serviceName;
            client.create().creatingParentsIfNeeded().withMode(CreateMode.EPHEMERAL).forPath(path);
        }
    } catch (Exception e) {
        System.out.println("此服务已存在");
    }
}
```

### path += "/" + RETRY + "/" + serviceName;

* `path` 变量是一个字符串，表示在 Zookeeper 中创建节点的路径。通过将 `"/" + RETRY + "/" + serviceName` 拼接到 `path` 中，构建出一个完整的 Zookeeper 节点路径。
* `RETRY` 是一个常量，而 `serviceName` 是服务的名称，那么最终的路径格式可能是类似 `/retry/serviceA`。

### client.create().creatingParentsIfNeeded().withMode(CreateMode.EPHEMERAL).forPath(path);

* 这个路径表示该服务（`serviceName`）正在重试，或者它被标记为可以重试。
* <code>**client.create()**</code>：
  * 这是 Zookeeper 客户端 (`client`) 创建一个节点的操作。
  * `create()` 是 Zookeeper 中的一个方法，用于创建新的节点。
* <code>**creatingParentsIfNeeded()**</code>：
  * 这是一个保证父节点存在的方法。如果指定的路径中的某些父节点不存在，Zookeeper 会自动创建这些父节点。
  * 例如，如果 `path` 是 `/retry/serviceA`，而 `/retry` 路径不存在，`creatingParentsIfNeeded()` 会确保先创建 `/retry` 节点，然后再创建 `/retry/serviceA` 节点。
  * 这样做可以避免因父节点缺失而导致的创建失败。
* <code>**withMode(CreateMode.EPHEMERAL)**</code>：
  * `withMode(CreateMode.EPHEMERAL)` 设置节点的类型为 **临时节点（EPHEMERAL）**。临时节点是指，当客户端与 Zookeeper 的连接断开时，Zookeeper 会自动删除这个节点。
  * 这种节点通常用于表示临时的、会随时间变化的状态。例如，可以用来表示服务的状态（例如，某个服务正在重试），一旦该服务完成重试或失效，节点就会被自动删除。
  * 在这种情况下，创建临时节点的目的是标记服务正在进行重试，并且一旦服务不再需要重试（例如，重试成功或失败），该节点会自动消失。
* <code>**forPath(path)**</code>：
  * `forPath(path)` 指定了创建节点的完整路径，也就是 `path` 变量中存储的路径。这个路径指向的是你希望创建的 Zookeeper 节点。
  * 最终会在 Zookeeper 中创建一个临时节点，路径为 `"/retry/serviceA"`（假设 `serviceName` 是 `serviceA`）。

# 修改ClientProxy

## 1. 先增加字段private ServiceCenter serviceCenter;修改构造器

```java
private RpcClient rpcClient;
private ServiceCenter serviceCenter;
public ClientProxy() throws InterruptedException {
    serviceCenter=new ZKServiceCenter();
    rpcClient=new NettyRpcClient(serviceCenter);
}
```

## 2.在客户端最上层的ClientProxy 调用服务时，添加重试机制和白名单的验证

判断是否在白名单中

如果 `checkRetry` 返回 `true`，即该服务被允许重试，接着代码调用了 `guavaRetry().sendServiceWithRetry(request, rpcClient)` 来执行重试操作。

如果 `checkRetry` 返回 `false`，即服务不允许重试，代码执行 `rpcClient.sendRequest(request)` 发送一次请求。

这种情况下，服务只会被调用一次。如果请求失败，系统不会再进行重试，直接返回失败的响应。

```java
@Override
public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
    //构建request
    RpcRequest request=RpcRequest.builder()
    .interfaceName(method.getDeclaringClass().getName())
    .methodName(method.getName())
    .params(args).paramsType(method.getParameterTypes()).build();
    //数据传输
    RpcResponse response;
    //后续添加逻辑：为保持幂等性，只对白名单上的服务进行重试
    if (serviceCenter.checkRetry(request.getInterfaceName())){
        //调用retry框架进行重试操作
        response=new guavaRetry().sendServiceWithRetry(request,rpcClient);
    }else {
        //只调用一次
        response= rpcClient.sendRequest(request);
    }
    return response.getData();
}
```


> 更新: 2024-11-29 19:41:25  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/plq5eidyvhk627ss>