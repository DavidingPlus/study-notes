# part2 在客户端建立本地服务缓存并实现动态更新

<font style="color:rgb(55, 65, 81);">以前的版本中，调用方每次调用服务，都要去注册中心zookeeper中查找地址，性能是不是很差呢？</font>

<font style="color:rgb(55, 65, 81);">我们可以在客户端建立一个本地缓存，缓存服务地址信息，作为优化的方案</font>

![1732535303511-a5b06d4d-9473-45b3-8a0d-d41e406603c3.png](./img/TSY-bqatCm3W8pCc/1732535303511-a5b06d4d-9473-45b3-8a0d-d41e406603c3-870045.png)

# <font style="color:rgb(55, 65, 81);">创建缓存</font>

<font style="color:rgb(55, 65, 81);">在Client层建立cache包</font>

**本地缓存serviceCache**

<font style="color:rgb(55, 65, 81);">既然设立了服务缓存，那么在ZKServiceCenter中，服务发现时应该先去寻找本地缓存</font>

**修改ZKServiceCenter的serviceDiscovery方法**

# 动态更新缓存的实现

<font style="color:rgb(55, 65, 81);">那么如何更新缓存呢？</font>

<font style="color:rgb(55, 65, 81);">首先在你脑海的肯定是最经典的cache aside旁路缓存策略</font>

* <font style="color:rgb(55, 65, 81);">首先去本地缓存中读，读不到，再去注册中心中读，返回数据时刷新缓存....等等老生常谈的八股</font>

<font style="color:rgb(55, 65, 81);">但是这种策略，不适用于当前场景，或者说，在当前场景下，存在很多问题</font>

<font style="color:rgb(55, 65, 81);">（这是笔者在刚拿rpc项目时面试被问到的问题，属实被问懵逼了）</font>

<font style="color:rgb(55, 65, 81);">比如如下场景</font>

![1732535414051-9920a2e4-a21f-4877-a88e-c906451c8247.png](./img/TSY-bqatCm3W8pCc/1732535414051-9920a2e4-a21f-4877-a88e-c906451c8247-583386.png)

<font style="color:rgb(55, 65, 81);">如果一个服务在注册中心中新增了一个地址，但是调用方始终能在本地缓存中读到这个服务</font>

<font style="color:rgb(55, 65, 81);">那么 新增的变化就永远无法感知到....</font>

<font style="color:rgb(55, 65, 81);">那么问题来了，Server端新上一个服务地址，Client端的本地缓存该怎么才能感知到呢</font>

<font style="color:rgb(55, 65, 81);">笔者当时的回答是：Server新上一个地址时，去更新Client端的本地缓存....</font>

<font style="color:rgb(55, 65, 81);">简直就是倒反天罡了。当时还停留在单体架构的项目思维中，所以回答的莫名其妙</font>

<font style="color:rgb(55, 65, 81);">面罢研究了一番，才幡然醒悟</font>

**<font style="color:rgb(55, 65, 81);">那么正解是：通过在注册中心注册Watcher，监听注册中心的变化，实现本地缓存的动态更新</font>**

<font style="color:rgb(55, 65, 81);">简单介绍一下zookeeper的事务监听机制</font>

# 事件监听机制

## watcher概念

* `zookeeper`<font style="color:rgb(55, 65, 81);">提供了数据的</font>`发布/订阅`<font style="color:rgb(55, 65, 81);">功能，多个订阅者可同时监听某一特定主题对象，当该主题对象的自身状态发生变化时例如节点内容改变、节点下的子节点列表改变等，会实时、主动通知所有订阅者</font>
* `zookeeper`<font style="color:rgb(55, 65, 81);">采用了 </font>`Watcher`<font style="color:rgb(55, 65, 81);">机制实现数据的发布订阅功能。该机制在被订阅对象发生变化时会异步通知客户端，因此客户端不必在 </font>`Watcher`<font style="color:rgb(55, 65, 81);">注册后轮询阻塞，从而减轻了客户端压力</font>
* `watcher`<font style="color:rgb(55, 65, 81);">机制事件上与观察者模式类似，也可看作是一种观察者模式在分布式场景下的实现方式</font>

## watcher架构

`watcher`<font style="color:rgb(55, 65, 81);">实现由三个部分组成</font>

* `zookeeper`<font style="color:rgb(55, 65, 81);">服务端</font>
* `zookeeper`<font style="color:rgb(55, 65, 81);">客户端</font>
* <font style="color:rgb(55, 65, 81);">客户端的</font>`ZKWatchManager对象`

<font style="color:rgb(55, 65, 81);">客户端</font>\*\*首先将 **<code>**Watcher**</code>**注册到服务端**<font style="color:rgb(55, 65, 81);">，同时将 </font>`Watcher`<font style="color:rgb(55, 65, 81);">对象</font>**保存到客户端的**<code>**watch**</code>**管理器中**<font style="color:rgb(55, 65, 81);">。当</font>`Zookeeper`<font style="color:rgb(55, 65, 81);">服务端监听的数据状态发生变化时，服务端会</font>**主动通知客户端**<font style="color:rgb(55, 65, 81);">，接着客户端的 </font>`Watch`<font style="color:rgb(55, 65, 81);">管理器会**触发相关 </font>`Watcher`<font style="color:rgb(55, 65, 81);">\*\*来回调相应处理逻辑，从而完成整体的数据 </font>`发布/订阅`<font style="color:rgb(55, 65, 81);">流程</font>

![1732535521507-a2a5a2b2-10d3-47f7-b4a1-384485e81e9a.png](./img/TSY-bqatCm3W8pCc/1732535521507-a2a5a2b2-10d3-47f7-b4a1-384485e81e9a-453127.png)

<font style="color:rgb(55, 65, 81);">使用curator，可以方便的进行watcher的使用</font>

# <font style="color:rgb(55, 65, 81);">整体顺序</font>

<font style="color:rgb(55, 65, 81);">在Client层建立cache包->本地缓存serviceCache->修改ZKServiceCenter的serviceDiscovery方法->创建一个 ZKWatcher 的包-> watchZK ->在ZKServiceCenter 加入缓存和监听器->改 NettyRpcClient->改 ClientProxy</font>

<font style="color:rgb(55, 65, 81);">->改 TestClient</font>

# <font style="color:rgb(55, 65, 81);">serviceCache</font>

```java
public class serviceCache {
    //成员变量，存储服务名和地址列表
    private static Map<String, List<String>> cache =new HashMap<>();
    //添加服务
    public void addServiceToCache(String serviceName, String address) {
        if (cache.containsKey(serviceName)) {
            List<String> addressList = cache.get(serviceName);
            addressList.add(address);
            System.out.println("将name为" + serviceName + "和地址为" + address + "的服务添加到本地缓存中");
        } else {
            List<String> addressList = new ArrayList<>();
            addressList.add(address);
            cache.put(serviceName, addressList);
        }
    }
    //修改服务
    public void replaceServiceAddress (String serviceName, String oldAddress, String newAddress) {
        if (cache.containsKey(serviceName)) {
            List<String> addressList = cache.get(serviceName);
            addressList.remove(oldAddress);
            addressList.add(newAddress);
        } else {
            System.out.println("修改失败，服务不存在");
        }
    }
    //从缓存中取服务地址
    public List<String> getServiceFromCache(String serviceName) {
        if (!cache.containsKey(serviceName)) {
            return null;
        }
        List<String> addressList = cache.get(serviceName);
        return addressList;
    }
    //从缓存中删除服务地址
    public void delete(String serviceName, String address) {
        List<String> addressList = cache.get(serviceName);
        addressList.remove(address);
        System.out.println("将name为" + serviceName + "和地址为" + address + "的服务从本地缓存中删除");
    }
}
```

## 1.cache 的 Map 中 key 和 value 代表什么？

key:serviceName 服务名

value：addressList 服务提供者列表

# <font style="color:rgb(55, 65, 81);">修改ZKServiceCenter</font>

先加 cache 字段

```java
//serviceCache
private serviceCache cache;
```

<font style="color:rgb(55, 65, 81);">既然设立了服务缓存，那么在ZKServiceCenter中，服务发现时应该先去寻找本地缓存</font>

```java
@Override
public InetSocketAddress serviceDiscovery(String serviceName) {
    try {
        //先从本地缓存中找
        List<String> serviceList = cache.getServiceFromCache(serviceName);
        //如果找不到，就从zookeeper中找
        if (serviceList == null) {
            serviceList = client.getChildren().forPath("/" + serviceName);
        }
        //这里默认使用第一个服务，后面加负载均衡
        String string = serviceList.get(0);
        return parseAddress(string);
    } catch (Exception e) {
        e.printStackTrace();
    }
    return null;
}
```

## 1. serviceList = client.getChildren().forPath("/" + serviceName);这句代码讲讲？

1. 使用 Zookeeper 客户端 API 获取指定路径下的所有节点，并把结果存储到serviceList 中
2. `getChildren()` 方法是 `CuratorFramework` 提供的 API，作用是获取指定节点（路径）的所有子节点。
3. 该方法返回一个 `GetChildrenBuilder` 对象，允许我们设置其他选项（例如，是否需要递归等），然后通过 `forPath()` 来获取具体数据。
4. `forPath(String path)` 是 `GetChildrenBuilder` 接口的一个方法，接受一个 Zookeeper 路径作为参数，表示要获取这个路径下的所有子节点。
5. 这里的路径是 `"/" + serviceName`，即将服务名称（`serviceName`）拼接到 `/` 之后，形成一个完整的路径。例如，如果 `serviceName` 是 `"user-service"`，那么完整路径就是 `"/user-service"`。
6. `forPath` 方法会返回该路径下的所有子节点名称列表（即该路径下所有注册的服务实例名称）。返回的结果是一个 `List<String>`，每个元素都是一个子节点的名称。

# <font style="color:rgb(55, 65, 81);">watchZK </font>

```java
public class watchZK {
    //zk客户端
    private CuratorFramework client;
    //服务缓存
    serviceCache cache;

    public watchZK(CuratorFramework client, serviceCache cache) {
        this.client = client;
        this.cache = cache;
    }
    //监听当前节点和子节点的更新，创建，删除
    public void watchToUpdate(String path) throws InterruptedException {
        //用于监视指定路径下的节点变化，并在节点变化时更新本地缓存
        //CuratorCache是Curator提供的一个用于监听节点变化的API
        //他会监听指定路径节点变化，这里监听的是根路径/
        CuratorCache curatorCache = CuratorCache.build(client, "/");
        //注册一个监听器，用于处理节点变化事件
        curatorCache.listenable().addListener(new CuratorCacheListener() {
            // 第一个参数：事件类型（枚举）
            // 第二个参数：节点更新前的状态、数据
            // 第三个参数：节点更新后的状态、数据
            // 创建节点时：节点刚被创建，不存在 更新前节点 ，所以第二个参数为 null
            // 删除节点时：节点被删除，不存在 更新后节点 ，所以第三个参数为 null
            // 节点创建时没有赋予值 create /curator/app1 只创建节点，在这种情况下，更新前节点的 data 为 null，获取不到更新前节点的数据
            @Override
            public void event(Type type, ChildData childData, ChildData childData1) {
                switch (type.name()) {
                        // 节点创建
                    case "NODE_CREATED"://监听器第一次执行节点存在也会触发此事件
                        String[] pathList = pasrePath(childData1);
                        if (pathList.length <= 2) break;
                        else {
                            String serviceName = pathList[1];
                            String address = pathList[2];
                            cache.addServiceToCache(serviceName, address);
                        }
                        break;
                        //节点更新
                    case "NODE_CHANGE":
                        if (childData.getData() != null) {
                            System.out.println("修改前的数据:" + new String(childData.getData()));
                        } else {
                            System.out.println("节点第一次赋值");
                        }
                        String[] oldPathList = pasrePath(childData);
                        String[] newPathList = pasrePath(childData1);
                        cache.replaceServiceAddress(oldPathList[1], oldPathList[2], newPathList[2]);
                        System.out.println("修改后的数据:" + new String(childData1.getData()));
                        break;
                        //节点删除
                    case "NODE_DELETED"://节点删除
                        String[] pathList_d = pasrePath(childData);
                        if (pathList_d.length <= 2) break;
                        else {
                            String serviceName = pathList_d[1];
                            String address = pathList_d[2];
                            cache.delete(serviceName, address);
                        }
                        break;
                    default:
                        break;
                }
            }
        });
        //开始监听
        curatorCache.start();
    }
    //解析节点对应地址
    private String[] pasrePath(ChildData childData) {
        //获取更新的节点的路径
        String path = new String(childData.getData());
        //按照格式，读取
        return path.split("/");
    }
}
```

## 1.curatorCache.listenable().addListener(new CuratorCacheListener() )这个方法是用来做什么的

这行代码的作用是为 `CuratorCache` 注册一个事件监听器，当 Zookeeper 中监视的节点发生变化时，指定的事件处理逻辑就会被触发。

`CuratorCache` 是 Curator 提供的一个类，用于监视 Zookeeper 中路径的变化。`listenable()` 方法返回一个可以添加监听器的对象，即 `Listenable` 接口，它允许我们注册事件监听器。

`addListener` 是 `Listenable` 接口提供的方法，用来为缓存（`CuratorCache`）添加一个 `CuratorCacheListener` 监听器。

`CuratorCacheListener` 是 Curator 框架提供的一个接口，它定义了节点变化时的回调方法。

## 2.event 的三个参数有什么含义？

事件类型

节点更新前的状态、数据

节点更新后的状态、数据

## 3.if(pathList.length<=2) break;为什么小于 2 退出？

路径应该至少包含两个部分：服务名和地址。如果路径列表的长度小于或等于 2，就说明路径不符合预期结构，因此应该跳过该事件的处理。

## 4.为什么是 pathList\[1]、pathList\[2]?

```java
String serviceName = pathList[1];
String address = pathList[2];
```

对于路径 `/serviceName/address`，分割后的数组是：`["", "serviceName", "address"]`。

所以，`pathList[1]` 是服务名，`pathList[2]` 是服务地址。因为路径 `/` 本身没有携带有用的信息，它只是分隔符。

# <font style="color:rgb(55, 65, 81);">在ZKServiceCenter 加入缓存和监听器</font>

```java
//负责zookeeper客户端的初始化，并与zookeeper服务端进行连接
public ZKServiceCenter() throws InterruptedException{
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
    // 初始化缓存
    this.cache = new serviceCache();
    //加入zookeeper事件监听器
    watchZK watcher = new watchZK(client,cache);
    //监听启动
    watcher.watchToUpdate(ROOT_PATH);
}
```

后面改的这三个都是只加上throws InterruptedException

# <font style="color:rgb(55, 65, 81);">改 NettyRpcClient</font>

```java
public NettyRpcClient() throws InterruptedException{
    this.serviceCenter=new ZKServiceCenter();
}
```

# <font style="color:rgb(55, 65, 81);">改 ClientProxy</font>

```java
public ClientProxy() throws InterruptedException{
    rpcClient=new NettyRpcClient();
}
```

# <font style="color:rgb(55, 65, 81);">改 TestClient</font>

```java
public static void main(String[] args) throws InterruptedException{
    ClientProxy clientProxy=new ClientProxy();
    //ClientProxy clientProxy=new part2.Client.proxy.ClientProxy("127.0.0.1",9999,0);
    //内部改为通过动态服务发现机制（如 Zookeeper）获取服务端地址
    UserService proxy=clientProxy.getProxy(UserService.class);

    User user = proxy.getUserByUserId(1);
    System.out.println("从服务端得到的user="+user.toString());

    User u= User.builder().id(100).userName("wxx").sex(true).build();
    Integer id = proxy.insertUserId(u);
    System.out.println("向服务端插入user的id"+id);
}
```


> 更新: 2024-11-29 19:29:10  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/nwxk8bbpnff42tai>