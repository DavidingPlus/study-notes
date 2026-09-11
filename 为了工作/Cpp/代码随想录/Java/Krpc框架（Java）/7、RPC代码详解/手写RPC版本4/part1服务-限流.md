# part1 服务-限流

# <font style="color:rgb(55, 65, 81);">假如要发布一个 RPC 服务，作为服务端接收调用端发送过来的请求，这时服务端的某个节点负载压力过高了，该如何保护这个节点？</font>

![1731564679795-75c61e05-a715-4b80-9d76-7c0498a714b6.png](https://cdn.davidingplus.cn/images/2026/09/11/1731564679795-75c61e05-a715-4b80-9d76-7c0498a714b6-853892.webp)

<font style="color:rgb(55, 65, 81);">这个问题还是很好解决的，既然负载压力高，那就不让它再接收太多的请求就好了，等接收和处理的请求数量下来后，这个节点的负载压力自然就下来了。</font>

<font style="color:rgb(55, 65, 81);">限流是一个比较通用的功能，这里我们可以当调用端发送请求过来时，服务端在执行业务逻辑之前先执行限流逻辑，如果发现访问量过大并且超出了限流的阈值，就让服务端直接抛回给调用端一个限流异常，否则就执行正常的业务逻辑。</font>

# <font style="color:rgb(55, 65, 81);">限流算法</font>

<font style="color:rgb(55, 65, 81);">常见的限流算法有4种：</font>**<font style="color:rgb(55, 65, 81);">计数器法，滑动窗口算法，漏桶算法</font>**<font style="color:rgb(55, 65, 81);">和</font>**<font style="color:rgb(55, 65, 81);">令牌桶算法</font>**

<font style="color:rgb(55, 65, 81);">对于上面4种算法的详细介绍和优缺点比较可以参考</font>

[服务限流详解](https://javaguide.cn/high-availability/limit-request.html)

<font style="color:rgb(55, 65, 81);">这里我们使用最高频的令牌桶算法作为项目使用的限流算法</font>

# 令牌桶算法简介

<font style="color:rgb(55, 65, 81);">令牌桶是指一个限流容器，容器有最大容量，每秒或每100ms产生一个令牌（具体取决于机器每秒处理的请求数），当容量中令牌数量达到最大容量时，令牌数量也不会改变了，只有当有请求过来时，使得令牌数量减少（只有获取到令牌的请求才会执行业务逻辑），才会不断生成令牌，所以令牌桶算法是一种弹性的限流算法</font>

# 令牌桶算法限流范围：

<font style="color:rgb(55, 65, 81);">假设令牌桶最大容量为n，每秒产生r个令牌</font>

<font style="color:rgb(55, 65, 81);">平均速率：则随着时间推延，处理请求的平均速率越来越趋近于每秒处理r个请求，说明令牌桶算法可以控制平均速率</font>

<font style="color:rgb(55, 65, 81);">瞬时速率：如果在一瞬间有很多请求进来，此时来不及产生令牌，则在一瞬间最多只有n个请求能获取到令牌执行业务逻辑，所以令牌桶算法也可以控制瞬时速率</font>

<font style="color:rgb(55, 65, 81);">在这里提下漏桶，漏桶由于出水量固定，所以无法应对突然的流量爆发访问，也就是没有保证瞬时速率的功能，但是可以保证平均速率</font>

# <font style="color:rgb(55, 65, 81);">整体顺序</font>

创建 ratelimit 包->RateLimit 接口->TokenBucketRateLimitImpl 类->provider 包->RateLimitProvider 类->修改 ServiceProvider->修改 NettyRPCServerHandler

# RateLimit

`RateLimit` 接口用于定义限流机制，`getToken()` 方法表示是否允许获取访问许可。

```java
public interface RateLimit {
    //获取访问许可
    boolean getToken();
}
```

# TokenBucketRateLimitImpl

```java
public class TokenBucketRateLimitImpl implements RateLimit {
    private static int RATE;         // 令牌产生速率（单位为ms）
    private static int CAPACITY;     // 桶容量
    private volatile int curCapcity; // 当前桶容量
    private volatile long timeStamp = System.currentTimeMillis(); // 上次请求的时间戳

    public TokenBucketRateLimitImpl(int rate, int capacity) {
        RATE = rate;  // 令牌产生的速率，表示每隔 `RATE` 毫秒会生成一个令牌
        CAPACITY = capacity;  // 令牌桶的最大容量
        curCapcity = capacity;  // 初始化当前桶容量为最大容量
    }

    @Override
    public synchronized boolean getToken() {
        // 如果桶内还有令牌，直接消费一个令牌并返回 true
        if (curCapcity > 0) {
            curCapcity--;
            return true;
        }

        // 如果桶内没有令牌，开始计算生成令牌的情况
        long current = System.currentTimeMillis();

        // 判断自上次获取令牌以来是否已过去足够的时间
        if (current - timeStamp >= RATE) {
            // 计算经过的时间内应该生成多少个令牌
            // (current - timeStamp) / RATE 表示这段时间内能够生成多少个令牌
            if ((current - timeStamp) / RATE >= 2) {
                // 如果时间间隔大于 RATE 的两倍以上，则加入 (当前时间 - 上次时间) / RATE - 1 个令牌
                curCapcity += (int) ((current - timeStamp) / RATE) - 1;
            }

            // 保证令牌桶的容量不会超过最大值 CAPACITY
            if (curCapcity > CAPACITY) {
                curCapcity = CAPACITY;
            }

            // 更新时间戳为当前时间
            timeStamp = current;
            return true;  // 返回 true，表示可以消费一个令牌
        }

        // 如果无法获取令牌，则返回 false
        return false;
    }
}

```

## 1. 令牌桶中有哪些字段

* `RATE`（令牌产生速率）：表示每隔 `RATE` 毫秒生成一个令牌。这个值越小，令牌生成的速度越快，流量限制就越宽松。默认情况下，令牌的生成速度是固定的，通常是一个常数。
* `CAPACITY`（桶容量）：表示令牌桶的最大容量。令牌桶最多可以存储多少个令牌。当生成的令牌数超过这个容量时，多余的令牌会被丢弃。
* `curCapcity`（当前桶容量）：表示令牌桶中当前剩余的令牌数量。初始时，桶容量为 `CAPACITY`。
* `timeStamp`（时间戳）：记录上一次消费令牌的时间，用来计算自上次消费令牌以来经过的时间。每当消费一个令牌或者桶容量被填充时，时间戳会更新。

## 2.如何进行限流

`getToken()` 方法（令牌请求和生成）：

这个方法用于请求获取令牌，控制限流的核心逻辑就是在这里：

* **如果桶中有令牌（**<code>**curCapacity > 0**</code>**）**：
  * 直接消耗一个令牌并返回 `true`，表示允许请求处理。
* **如果桶中没有令牌**（`curCapacity == 0`）：
  * 通过计算当前时间和上次时间的差值（`current - timeStamp`），来判断是否已经过了足够的时间（大于等于 `RATE`）生成新令牌。
  * 如果 `current - timeStamp >= RATE`（即足够的时间已过去），表示该时间间隔内可以生成新的令牌。然后，系统通过以下方式计算可以生成的令牌数：
    * `current - timeStamp / RATE >= 2` 计算的结果是判断在这段时间内产生了多少个令牌，`RATE` 为每个令牌的生成间隔（单位为毫秒），每经过 `RATE` 毫秒生成一个令牌。
    * 根据时间间隔生成的令牌数（`(current - timeStamp) / RATE - 1`），将这些令牌加到 `curCapacity` 中，表示令牌桶中的容量增加了相应数量的令牌。
  * **限制容量**：如果生成的令牌数量超过了桶的最大容量 `CAPACITY`，会将 `curCapacity` 限制为最大容量 `CAPACITY`，保证令牌桶中的令牌不会超过容量。
  * 然后，更新 `timeStamp` 为当前时间，表示上次操作的时间被刷新。
* **如果请求的时间间隔不足**（`current - timeStamp < RATE`），也就是桶中的令牌数已经用尽，但还没有生成新的令牌时，方法会返回 `false`，表示当前请求被限流，无法处理。

# RateLimitProvider

## 1.本类的作用是什么？

主要功能是提供速率限制（Rate Limiting）相关的服务。

## 2.Map 存储的是？

* `rateLimitMap` 是一个 `HashMap` 类型的成员变量，它用于存储每个接口的速率限制器实例。`Map<String, RateLimit>` 的键是接口的名称（`interfaceName`），值是对应的速率限制器实例。
* 这样设计可以让 `RateLimitProvider` 在调用时根据不同的接口名称返回相应的速率限制器，确保不同的接口有不同的速率限制策略。

## 3.讲讲 getRateLimit 方法

* `getRateLimit` 方法用于根据接口名称 `interfaceName` 返回相应的速率限制器实例（`RateLimit` 类型）。
* 如果 `rateLimitMap` 中已经包含该接口名称的速率限制器实例，直接返回该实例；如果没有，则创建一个新的速率限制器实例并存入 `rateLimitMap` 中。

```java
public class RateLimitProvider {

    // 用于存储每个接口名称与对应的速率限制器实例之间的映射关系
    private Map<String, RateLimit> rateLimitMap = new HashMap<>();

    /**
     * 根据接口名称获取对应的速率限制器实例。
     * 如果该接口的速率限制器实例不存在，则会创建一个新的实例并返回。
     * 
     * @param interfaceName 接口名称
     * @return 对应接口的速率限制器实例
     */
    public RateLimit getRateLimit(String interfaceName) {
        
        // 检查rateLimitMap中是否已经有该接口的速率限制器实例
        if (!rateLimitMap.containsKey(interfaceName)) {
            // 如果没有，则创建一个新的速率限制器实例
            // 这里我们使用TokenBucketRateLimitImpl实现类，假设它使用令牌桶算法进行速率限制
            RateLimit rateLimit = new TokenBucketRateLimitImpl(100, 10);
            
            // 将新创建的速率限制器存入map中，以接口名称为键
            rateLimitMap.put(interfaceName, rateLimit);
            
            // 返回新创建的速率限制器
            return rateLimit;
        }
        
        // 如果map中已经包含该接口的速率限制器实例，则直接返回现有实例
        return rateLimitMap.get(interfaceName);
    }
}
```

# 修改 ServiceProvider

添加限流逻辑

```java
private Map<String,Object> interfaceProvider;

private int port;
private String host;
//注册服务类
private ServiceRegister serviceRegister;
//限流器
private RateLimitProvider rateLimitProvider;
public ServiceProvider(String host,int port){
    //需要传入服务端自身的网络地址
    this.host=host;
    this.port=port;
    this.interfaceProvider=new HashMap<>();
    this.serviceRegister=new ZKServiceRegister();
    this.rateLimitProvider=new RateLimitProvider();
}
```

```java
public Object getService(String interfaceName){
return interfaceProvider.get(interfaceName);
}
public RateLimitProvider getRateLimitProvider(){
    return rateLimitProvider;
}
```

# 修改 NettyRPCServerHandler

增加限流逻辑

```java
private RpcResponse getResponse(RpcRequest rpcRequest){
    //得到服务名
    String interfaceName=rpcRequest.getInterfaceName();
    //接口限流降级
    RateLimit rateLimit=serviceProvider.getRateLimitProvider().getRateLimit(interfaceName);
    if(!rateLimit.getToken()){
        //如果获取令牌失败，进行限流降级，快速返回结果
        System.out.println("服务限流！！");
        return RpcResponse.fail();
    }

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
```


> 更新: 2024-11-29 19:45:16  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/iegrtk7b6zdhm9xa>