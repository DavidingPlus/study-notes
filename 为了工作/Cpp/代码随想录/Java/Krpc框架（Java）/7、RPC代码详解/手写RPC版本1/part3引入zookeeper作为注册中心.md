# part3 引入zookeeper作为注册中心

<font style="color:rgb(55, 65, 81);">在part1 和part2 中，我们在调用服务时，对目标的ip地址和端口port都是写死的，默认本机地址和9999端口号</font>

<font style="color:rgb(55, 65, 81);">在实际场景下，服务的地址和端口会被记录到【注册中心】中。服务端上线时，在注册中心注册自己的服务与对应的地址，而客户端调用服务时，去注册中心根据服务名找到对应的服务端地址。</font>

**<font style="color:rgb(55, 65, 81);">这里我们使用zookeeper作为注册中心</font>**<font style="color:rgb(55, 65, 81);">。</font>

`zookeepepr`<font style="color:rgb(55, 65, 81);">是一个经典的</font>**分布式**<font style="color:rgb(55, 65, 81);">数据一致性解决方案，致力于为分布式应用提供一个高性能、高可用,且具有严格顺序访问控制能力的分布式协调存储服务。</font>

<font style="color:rgb(55, 65, 81);">1.高性能</font>

* `zookeeper`<font style="color:rgb(55, 65, 81);">将全量数据存储在</font>**内存**<font style="color:rgb(55, 65, 81);">中，并直接服务于客户端的所有非事务请求，尤其用于以读为主的应用场景</font>

<font style="color:rgb(55, 65, 81);">2.高可用</font>

* `zookeeper`<font style="color:rgb(55, 65, 81);">一般以集群的方式对外提供服务，一般</font>`3~5`<font style="color:rgb(55, 65, 81);">台机器就可以组成一个可用的 </font>`Zookeeper`<font style="color:rgb(55, 65, 81);">集群了，每台机器都会在内存中维护当前的服务器状态，井且每台机器之间都相互保持着通信。只要集群中超过一半的机器都能够正常工作，那么整个集群就能够正常对外服务</font>

<font style="color:rgb(55, 65, 81);">3.严格顺序访问</font>

* <font style="color:rgb(55, 65, 81);">对于来自客户端的每个更新请求，</font>`Zookeeper`<font style="color:rgb(55, 65, 81);">都会分配一个全局唯一的递增编号，这个编号反应了所有事务操作的先后顺序</font>

# 数据模型<font style="color:rgb(55, 65, 81);">：</font>

![1732680932689-ecb47121-b31b-41e8-b202-c8c9379eab05.png](https://cdn.davidingplus.cn/images/2026/09/11/1732680932689-ecb47121-b31b-41e8-b202-c8c9379eab05-074281.png)

`zookeeper`<font style="color:rgb(55, 65, 81);">的数据结点可以视为树状结构(或目录)，树中的各个结点被称为</font>`znode`<font style="color:rgb(55, 65, 81);">(即</font>`zookeeper node`<font style="color:rgb(55, 65, 81);">)，一个</font>`znode`<font style="color:rgb(55, 65, 81);">可以由多个子结点。</font>`zookeeper`<font style="color:rgb(55, 65, 81);">结点在结构上表现为树状；</font>

<font style="color:rgb(55, 65, 81);">使用路径</font>`path`<font style="color:rgb(55, 65, 81);">来定位某个</font>`znode`<font style="color:rgb(55, 65, 81);">，比如</font>`/ns-1/itcast/mysqml/schemal1/table1`<font style="color:rgb(55, 65, 81);">，此处</font>`ns-1，itcast、mysql、schemal1、table1`<font style="color:rgb(55, 65, 81);">分别是</font>`根结点、2级结点、3级结点以及4级结点`<font style="color:rgb(55, 65, 81);">；其中</font>`ns-1`<font style="color:rgb(55, 65, 81);">是</font>`itcast`<font style="color:rgb(55, 65, 81);">的父结点，</font>`itcast`<font style="color:rgb(55, 65, 81);">是</font>`ns-1`<font style="color:rgb(55, 65, 81);">的子结点，</font>`itcast`<font style="color:rgb(55, 65, 81);">是</font>`mysql`<font style="color:rgb(55, 65, 81);">的父结点....以此类推</font>

`znode`<font style="color:rgb(55, 65, 81);">，间距文件和目录两种特点，即像文件一样维护着数据、元信息、ACL、时间戳等数据结构，又像目录一样可以作为路径标识的一部分</font>

<font style="color:rgb(55, 65, 81);">那么如何描述一个</font>`znode`<font style="color:rgb(55, 65, 81);">呢？一个</font>`znode`<font style="color:rgb(55, 65, 81);">大体上分为</font>`3`<font style="color:rgb(55, 65, 81);">个部分：</font>

* <font style="color:rgb(55, 65, 81);">结点的数据：即</font>`znode data`<font style="color:rgb(55, 65, 81);">(结点</font>`path`<font style="color:rgb(55, 65, 81);">，结点</font>`data`<font style="color:rgb(55, 65, 81);">)的关系就像是</font>`Java map`<font style="color:rgb(55, 65, 81);">中的 </font>`key value`<font style="color:rgb(55, 65, 81);">关系</font>
* <font style="color:rgb(55, 65, 81);">结点的子结点</font>`children`
* <font style="color:rgb(55, 65, 81);">结点的状态</font>`stat`<font style="color:rgb(55, 65, 81);">：用来描述当前结点的创建、修改记录，包括</font>`cZxid`<font style="color:rgb(55, 65, 81);">、</font>`ctime`<font style="color:rgb(55, 65, 81);">等</font>

**应用场景**<font style="color:rgb(55, 65, 81);">：通常作为注册中心和配置中心</font>

# 环境配置

<font style="color:rgb(55, 65, 81);">\[windows 环境下zookeeper的安装与配置] </font>

[【Zookeeper】Windows下安装Zookeeper（图文记录详细步骤，手把手包安装成功）-CSDN博客](https://blog.csdn.net/tttzzzqqq2018/article/details/132093374?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522172149339116800211548359%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D\&request_id=172149339116800211548359\&biz_id=0\&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-2-132093374-null-null.142^v100^pc_search_result_base9\&utm_term=zookeeper%E5%AE%89%E8%A3%85%E4%B8%8E%E9%85%8D%E7%BD%AE\&spm=1018.2226.3001.4187)

# <font style="color:rgb(55, 65, 81);">pom.xml中 引入包Curator客户端</font>

```java
<!--这个jar包应该依赖log4j,不引入log4j会有控制台会有warn，但不影响正常使用-->
<dependency>
    <groupId>org.apache.curator</groupId>
    <artifactId>curator-recipes</artifactId>
    <version>5.1.0</version>
</dependency>
```

<font style="color:rgb(55, 65, 81);">Curator：对zookeeper进行连接操作的工具</font>

<font style="color:rgb(55, 65, 81);">导入 log4j 包</font>

```java
<dependency>
    <groupId>org.apache.logging.log4j</groupId>
    <artifactId>log4j-core</artifactId>
    <version>2.20.0</version>
</dependency>
<dependency>
    <groupId>org.apache.logging.log4j</groupId>
    <artifactId>log4j-slf4j-impl</artifactId>
    <version>2.20.0</version>
</dependency>
```

# 启动步骤

<font style="color:rgb(55, 65, 81);">bin目录下 1.双击zkServer.cmd 2.双击zkCli.cmd</font>

<font style="color:rgb(55, 65, 81);">zkCli界面显示</font>

![1732681141615-96302c8e-4335-491b-ad3e-66d3a02fc2ac.png](https://cdn.davidingplus.cn/images/2026/09/11/1732681141615-96302c8e-4335-491b-ad3e-66d3a02fc2ac-404608.png)

<font style="color:rgb(55, 65, 81);">证明运行成功</font>

# 客户端

创建serviceCenter包->ServiceCenter 接口->ZKServiceCenter->修改 NettyRpcClient->修改 ClientProxy->修改 TestClient

## ServiceCenter

```java
//服务中心接口
public interface ServiceCenter {
    //  查询：根据服务名查找地址
    InetSocketAddress serviceDiscovery(String serviceName);
}
```

### 1.InetSocketAddress 是个什么类？

`InetSocketAddress` 是 Java 中的一个类，属于 <code>**java.net**</code> 包。它表示了一个网络地址（包含 IP 地址和端口号），通常用于在网络中标识一个计算机的端口。

## ZKServiceCenter

```java
public class ZKServiceCenter implements ServiceCenter{
    // curator 提供的zookeeper客户端
    private CuratorFramework client;
    //zookeeper根路径节点
    private static final String ROOT_PATH = "MyRPC";

    //负责zookeeper客户端的初始化，并与zookeeper服务端进行连接
    public ZKServiceCenter(){
        // 指数时间重试
        RetryPolicy policy = new ExponentialBackoffRetry(1000, 3);
        // zookeeper的地址固定，不管是服务提供者还是，消费者都要与之建立连接
        // sessionTimeoutMs 与 zoo.cfg中的tickTime 有关系，
        // zk还会根据minSessionTimeout与maxSessionTimeout两个参数重新调整最后的超时值。默认分别为tickTime 的2倍和20倍
        // 使用心跳监听状态
        this.client = CuratorFrameworkFactory.builder().connectString("127.0.0.1:2181")
        .sessionTimeoutMs(40000).retryPolicy(policy).namespace(ROOT_PATH).build();
        this.client.start();
        System.out.println("zookeeper 连接成功");
    }
    //根据服务名（接口名）返回地址
    @Override
    public InetSocketAddress serviceDiscovery(String serviceName) {
        try {//获取服务名对应路径下的所有子节点，子节点通常保存服务实例的地址（ip:port 格式）。
            List<String> strings = client.getChildren().forPath("/" + serviceName);
            // 这里默认用的第一个，后面加负载均衡
            String string = strings.get(0);
            //将子节点字符串（ip:port 格式）解析为 InetSocketAddress，便于客户端进行通信。
            return parseAddress(string);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }
    // 地址 -> XXX.XXX.XXX.XXX:port 字符串
    private String getServiceAddress(InetSocketAddress serverAddress) {
        return serverAddress.getHostName() +
        ":" +
        serverAddress.getPort();
    }
    // 字符串解析为地址
    private InetSocketAddress parseAddress(String address) {
        String[] result = address.split(":");
        return new InetSocketAddress(result[0], Integer.parseInt(result[1]));
    }
}
```

### 1.讲讲 zk 客户端的初始化

* <code>**RetryPolicy policy = new ExponentialBackoffRetry(1000, 3);**</code>：指定了一个 **指数回退重试策略**，用于在连接失败时，进行自动重试。这里的意思是：
  * 初始重试间隔为 1000 毫秒（1 秒）。
  * 最大重试次数为 3 次。
  * 这种策略会使得每次重试的时间间隔呈指数增长，即：第一次重试等待 1 秒，第二次重试等待 2 秒，第三次重试等待 4 秒。
* <code>**CuratorFrameworkFactory.builder().connectString("127.0.0.1:2181")**</code>：指定连接到 Zookeeper 的地址（在本地的 2181 端口）。无论是服务提供者还是消费者，都需要与 Zookeeper 建立连接。
* <code>**sessionTimeoutMs(40000)**</code>：设置客户端的 **会话超时时间**，单位是毫秒。这里设置为 40 秒，表示如果客户端 40 秒没有任何活动，Zookeeper 会认为客户端失联。
* <code>**retryPolicy(policy)**</code>：指定重试策略，即当连接失败时，应该按照怎样的方式进行重试。
* <code>**namespace(ROOT_PATH)**</code>：设置 **命名空间**，表示所有操作都将作用在 `MyRPC` 目录下，这样可以把不同服务的节点隔离开来，避免混淆。

然后调用 `client.start()` 来启动连接。

### 2.getChildren 方法与 forPath 方法是做什么的

* <code>**client.getChildren().forPath("/" + serviceName)**</code>：这行代码用于获取指定 `serviceName`（服务名称）路径下的所有子节点。每个子节点通常表示一个服务实例的地址，存储的格式一般是 <code>**ip:port**</code>。
* 例如，如果服务名是 `UserService`，Zookeeper 上的路径可能是 `/MyRPC/UserService`，其中包含多个子节点，每个子节点代表一个服务实例，存储的值是该实例的 IP 地址和端口号。
* <code>**strings.get(0)**</code>：默认选取第一个服务实例。如果有多个实例，可以通过负载均衡等策略来选择。这里直接选取第一个实例，后续可以根据需要添加更多的负载均衡机制。
* <code>**parseAddress(string)**</code>：将 `ip:port` 字符串解析成 `InetSocketAddress` 对象，便于客户端进行通信。

## 修改 NettyRpcClient

从固定端口号和地址改为 zk 传入

```java
private static final Bootstrap bootstrap;
private static final EventLoopGroup eventLoopGroup;

private ServiceCenter serviceCenter;
public NettyRpcClient(){
    this.serviceCenter=new ZKServiceCenter();
}
```

## 修改 ClientProxy

选择 Netty 客户端，并且不用传参

```java
private RpcClient rpcClient;
public ClientProxy(){
    rpcClient=new NettyRpcClient();
}
```

## 修改 TestClient

创建代理对象时，不用从客户端这传入端口、地址等信息了

```java
public class TestClient {
    public static void main(String[] args) {
        ClientProxy clientProxy=new ClientProxy();
        //ClientProxy clientProxy=new part2.Client.proxy.ClientProxy("127.0.0.1",9999,0);
        //内部改为通过动态服务发现机制（如 Zookeeper）获取服务端地址
        UserService proxy=clientProxy.getProxy(UserService.class);

        User user = proxy.getUserByUserId(1);
        System.out.println("从服务端得到的user="+user.toString());

        User u=User.builder().id(100).userName("wxx").sex(true).build();
        Integer id = proxy.insertUserId(u);
        System.out.println("向服务端插入user的id"+id);
    }
}
```

# 服务端

创建 serviceRegister 包->ServiceRegister 接口->Impl 包->ZKServiceRegister 类->修改 ServiceProvider 类->修改 TestServer

## ServiceRegister

```java
// 服务注册接口
public interface ServiceRegister {
    //  注册：保存服务与地址。
    void register(String serviceName, InetSocketAddress serviceAddress);
}
```

## ZKServiceRegister

```java
public class ZKServiceRegister implements ServiceRegister {
    // curator 提供的zookeeper客户端
    private CuratorFramework client;
    //zookeeper根路径节点
    private static final String ROOT_PATH = "MyRPC";

    //负责zookeeper客户端的初始化，并与zookeeper服务端进行连接
    public ZKServiceRegister(){
        // 指数时间重试
        RetryPolicy policy = new ExponentialBackoffRetry(1000, 3);
        // zookeeper的地址固定，不管是服务提供者还是，消费者都要与之建立连接
        // sessionTimeoutMs 与 zoo.cfg中的tickTime 有关系，
        // zk还会根据minSessionTimeout与maxSessionTimeout两个参数重新调整最后的超时值。默认分别为tickTime 的2倍和20倍
        // 使用心跳监听状态
        this.client = CuratorFrameworkFactory.builder().connectString("127.0.0.1:2181")
        .sessionTimeoutMs(40000).retryPolicy(policy).namespace(ROOT_PATH).build();
        this.client.start();
        System.out.println("zookeeper 连接成功");
    }
    //注册服务到注册中心
    @Override
    public void register(String serviceName, InetSocketAddress serviceAddress) {
        try {
            // serviceName创建成永久节点，服务提供者下线时，不删服务名，只删地址
            if(client.checkExists().forPath("/" + serviceName) == null){
                client.create().creatingParentsIfNeeded().withMode(CreateMode.PERSISTENT).forPath("/" + serviceName);
            }
            // 路径地址，一个/代表一个节点
            String path = "/" + serviceName +"/"+ getServiceAddress(serviceAddress);
            // 临时节点，服务器下线就删除节点
            client.create().creatingParentsIfNeeded().withMode(CreateMode.EPHEMERAL).forPath(path);
        } catch (Exception e) {
            System.out.println("此服务已存在");
        }
    }
    // 地址 -> XXX.XXX.XXX.XXX:port 字符串
    private String getServiceAddress(InetSocketAddress serverAddress) {
        return serverAddress.getHostName() +
        ":" +
        serverAddress.getPort();
    }
    // 字符串解析为地址
    private InetSocketAddress parseAddress(String address) {
        String[] result = address.split(":");
        return new InetSocketAddress(result[0], Integer.parseInt(result[1]));
    }
}
```

### <font style="color:rgb(38, 38, 38);">1.服务是如何注册到注册中心的？</font>

#### 首先检查是否已有服务名路径

* <code>**client.checkExists().forPath("/" + serviceName)**</code>：首先检查 Zookeeper 上是否已经有该服务的根节点。`/serviceName` 代表一个服务类型的根节点，例如，如果服务名是 `UserService`，则路径为 `/UserService`。
  * 如果该路径已经存在，说明已经有这个服务的注册记录；如果路径不存在，则表示该服务名还没有注册过。
  * 这个检查的作用是避免重复创建相同的服务名节点。
* <code>**client.create().creatingParentsIfNeeded().withMode(CreateMode.PERSISTENT).forPath("/" + serviceName)**</code>：
  * <code>**creatingParentsIfNeeded()**</code>：确保父路径存在，如果父路径不存在会一并创建。例如，如果你要创建 `/UserService/127.0.0.1:8080`，而 `/UserService` 路径还没有创建，则会先创建 `/UserService`。
  * <code>**withMode(CreateMode.PERSISTENT)**</code>：使用持久化模式创建服务名节点。持久化节点意味着该节点在 Zookeeper 中是长期存在的，直到被显式删除。服务名节点通常是持久的，因为即使服务提供者离线，服务名节点仍然需要存在。
  * `forPath("/" + serviceName)`：指定要创建的路径，即服务名的根节点路径（如 `/UserService`）。

#### 创建服务实例路径

Stringpath="/" + serviceName + "/" + getServiceAddress(serviceAddress);

* 这里构造了服务实例的路径，路径格式为：`/serviceName/ip:port`。
* `serviceName` 是服务的名称，`serviceAddress` 是服务实例的地址（例如，`127.0.0.1:8080`）。
* `getServiceAddress(serviceAddress)` 将 `InetSocketAddress` 对象（包含 IP 和端口）转化为字符串（`ip:port`）。
* 这个路径表示一个服务实例的地址节点。例如，`/UserService/127.0.0.1:8080` 代表 `UserService` 服务在 `127.0.0.1:8080` 这个地址上的实例。

#### 创建服务实例的临时节点

client.create().creatingParentsIfNeeded().withMode(CreateMode.EPHEMERAL).forPath(path);

* <code>**creatingParentsIfNeeded()**</code>：如果 `serviceName` 路径下没有父节点，创建它们。这里主要是确保服务实例的路径 `/serviceName/ip:port` 所需要的父路径（即 `/serviceName`）已存在。
* <code>**withMode(CreateMode.EPHEMERAL)**</code>：使用 **临时节点** 创建服务实例路径。临时节点有一个特殊的特性：当客户端断开与 Zookeeper 的连接时，Zookeeper 会自动删除该临时节点。因此，服务实例节点会随着服务提供者的下线而自动删除。
* 临时节点适合用来注册服务实例，因为服务提供者如果下线了，临时节点会被自动删除，不会在 Zookeeper 中留下无效的服务实例。
* <code>**forPath(path)**</code>：指定节点的路径，也就是 `/serviceName/ip:port`，对应着服务实例的地址信息。

## 修改 ServiceProvider 类

在本地注册这里加入端口和地址

```java
private Map<String,Object> interfaceProvider;

private int port;
private String host;
//注册服务类
private ServiceRegister serviceRegister;

public ServiceProvider(String host,int port){
    //需要传入服务端自身的网络地址
    this.host=host;
    this.port=port;
    this.interfaceProvider=new HashMap<>();
    this.serviceRegister=new ZKServiceRegister();
}
```

遍历接口名时，将服务注册到注册中心

```java
public void provideServiceInterface(Object service){
String serviceName=service.getClass().getName();
Class<?>[] interfaceName=service.getClass().getInterfaces();

for (Class<?> clazz:interfaceName){
    //本机的映射表
    interfaceProvider.put(clazz.getName(),service);
    //在注册中心注册服务
    serviceRegister.register(clazz.getName(),new InetSocketAddress(host,port));
}
}
```

## 修改 TestServer

需要在初始化注册中心时传入接口和地址。

```java
public class TestServer {
    public static void main(String[] args) {
        UserService userService=new UserServiceImpl();

        ServiceProvider serviceProvider=new ServiceProvider("127.0.0.1",9999);
        serviceProvider.provideServiceInterface(userService);

        RpcServer rpcServer=new NettyRPCRPCServer(serviceProvider);
        rpcServer.start(9999);
    }
}
```


> 更新: 2024-11-29 19:14:05  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/pfsg2zs1uxnc1v7v>