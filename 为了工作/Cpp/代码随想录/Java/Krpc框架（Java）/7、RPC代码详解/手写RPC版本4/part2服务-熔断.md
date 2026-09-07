# part2 服务-熔断

<font style="color:rgb(55, 65, 81);">服务端进行自我保护，最简单有效的方式就是限流。那么调用端呢？调用端是否需要自我保护呢？举个例子，假如要发布一个服务 B，而服务 B 又依赖服务 C，当一个服务 A 来调用服务 B 时，服务 B 的业务逻辑调用服务 C，而这时服务 C 响应超时了，由于服务 B 依赖服务 C，C 超时直接导致 B 的业务逻辑一直等待，而这个时候服务 A 在频繁地调用服务 B，服务 B 就可能会因为堆积大量的请求而导致服务宕机</font>

![1731565276196-b21b68e2-05a4-440f-92fe-5f4afed29e1a.png](./img/Hyp6DzRDlOOj8AfZ/1731565276196-b21b68e2-05a4-440f-92fe-5f4afed29e1a-300242.webp)

<font style="color:rgb(55, 65, 81);">由此可见，服务 B 调用服务 C，服务 C 执行业务逻辑出现异常时，会影响到服务 B，甚至可能会引起服务 B 宕机。这还只是 A->B->C 的情况，试想一下 A->B->C->D->……呢？在整个调用链中，只要中间有一个服务出现问题，都可能会引起上游的所有服务出现一系列的问题，甚至会引起整个调用链的服务都宕机，这是非常恐怖的。</font>

<font style="color:rgb(55, 65, 81);">所以说，在一个服务作为调用端调用另外一个服务时，为了防止被调用的服务出现问题而影响到作为调用端的这个服务，这个服务也需要进行自我保护。而最有效的自我保护方式就是熔断。</font>

# <font style="color:rgb(55, 65, 81);">熔断逻辑</font>

<font style="color:rgb(55, 65, 81);">熔断设计来源于日常生活中的电路系统，在电路系统中存在一种熔断器（Circuit Breaker），它的作用就是在电流过大时自动切断电路。熔断器一般要实现三个状态：闭合、断开和半开，分别对应于正常、故障和故障后检测故障是否已被修复的场景。</font>

* **闭合**<font style="color:rgb(55, 65, 81);">：正常情况，后台会对调用失败次数进行积累，到达一定阈值或比例时则自动启动熔断机制。</font>
* **断开**<font style="color:rgb(55, 65, 81);">：一旦对服务的调用失败次数达到一定阈值时，熔断器就会打开，这时候对服务的调用将直接返回一个预定的错误，而不执行真正的网络调用。同时，熔断器需要设置一个固定的时间间隔，当处理请求达到这个时间间隔时会进入半熔断状态。</font>
* **半开**<font style="color:rgb(55, 65, 81);">：在半开状态下，熔断器会对通过它的部分请求进行处理，如果对这些请求的成功处理数量达到一定比例则认为服务已恢复正常，就会关闭熔断器，反之就会打开熔断器。</font>

<font style="color:rgb(55, 65, 81);">熔断设计的一般思路是，在请求失败 N 次后在 X 时间内不再请求，进行熔断；然后再在 X 时间后恢复 M% 的请求，如果 M% 的请求都成功则恢复正常，关闭熔断，否则再熔断 Y 时间，依此循环。</font>

<font style="color:rgb(55, 65, 81);">在熔断的设计中，根据 Netflix 的开源组件 hystrix 的设计，我们可以仿照以下二个模块：熔断请求判断算法、熔断恢复机制</font>

* <font style="color:rgb(55, 65, 81);">熔断请求判断机制算法：根据事先设置的在固定时间内失败的比例来计算。</font>
* <font style="color:rgb(55, 65, 81);">熔断恢复：对于被熔断的请求，每隔 X 时间允许部分请求通过，若请求都成功则恢复正常。</font>

# <font style="color:rgb(55, 65, 81);">整体顺序</font>

创建 circuitBreaker 包->CircuitBreaker 类->CircuitBreakProvider 类->修改ClientProxy->修改 TestClient 类

# CircuitBreaker

```java
public class CircuitBreaker {

    private CircuitBreakerState state = CircuitBreakerState.CLOSED; // 熔断器初始状态：关闭
    private AtomicInteger failureCount = new AtomicInteger(0); // 失败请求计数
    private AtomicInteger successCount = new AtomicInteger(0); // 成功请求计数
    private AtomicInteger requestCount = new AtomicInteger(0); // 请求总数
    private final int failureThreshold; // 失败阈值
    private final double halfOpenSuccessRate; // 半开状态下的成功率阈值
    private final long resetTimePeriod; // 重置时间周期
    private long lastFailureTime = 0; // 最后一次失败时间

    // 构造函数初始化熔断器参数
    public CircuitBreaker(int failureThreshold, double halfOpenSuccessRate, long resetTimePeriod) {
        this.failureThreshold = failureThreshold;
        this.halfOpenSuccessRate = halfOpenSuccessRate;
        this.resetTimePeriod = resetTimePeriod;
    }

    // 根据当前熔断器状态判断是否允许请求
    public synchronized boolean allowRequest() {
        long currentTime = System.currentTimeMillis();
        switch (state) {
            case OPEN:
                if (currentTime - lastFailureTime > resetTimePeriod) {
                    state = CircuitBreakerState.HALF_OPEN;
                    resetCounts(); // 重置计数
                    return true; // 允许请求
                }
                return false; // 继续熔断
            case HALF_OPEN:
                requestCount.incrementAndGet(); // 在半开状态下记录请求
                return true; // 允许请求
            case CLOSED:
            default:
                return false; // 服务正常，拒绝请求
        }
    }

    // 记录一次成功的请求
    public synchronized void recordSuccess() {
        if (state == CircuitBreakerState.HALF_OPEN) {
            successCount.incrementAndGet();
            if (successCount.get() >= halfOpenSuccessRate * requestCount.get()) {
                state = CircuitBreakerState.CLOSED; // 恢复正常状态
                resetCounts(); // 重置计数
            }
        } else {
            resetCounts(); // 不是半开状态，重置计数
        }
    }

    // 记录一次失败的请求
    public synchronized void recordFailure() {
        failureCount.incrementAndGet(); // 增加失败次数
        lastFailureTime = System.currentTimeMillis(); // 记录失败时间

        if (state == CircuitBreakerState.HALF_OPEN) {
            state = CircuitBreakerState.OPEN; // 半开失败，切换到打开状态
        } else if (failureCount.get() >= failureThreshold) {
            state = CircuitBreakerState.OPEN; // 失败超过阈值，切换到打开状态
        }
    }

    // 重置计数器
    private void resetCounts() {
        failureCount.set(0);
        successCount.set(0);
        requestCount.set(0);
    }
}
```

## 1.本类的作用？

本类实现了一个 **熔断器（Circuit Breaker）** 模式，熔断器模式主要用于保护系统的高可用性，它用于监控一段时间内的服务调用失败情况，当失败次数超过一定阈值时，熔断器会切换到“打开”状态，从而避免继续调用可能导致错误的服务。当失败恢复或有一定条件满足时，熔断器会进入“半开”状态进行一次小范围的测试，最终恢复到“关闭”状态。

## 2.字段有哪些?

* **失败计数（**<code>**failureCount**</code>**）**: 记录失败请求的数量。
* **成功计数（**<code>**successCount**</code>**）**: 记录成功请求的数量。
* **请求计数（**<code>**requestCount**</code>**）**: 记录当前“半开”状态下的请求次数。
* <code>**failureThreshold**</code>: 失败次数的阈值，超过这个值后熔断器会打开。
* <code>**halfOpenSuccessRate**</code>: 半开状态下需要的成功率阈值，只有当成功请求占比达到这个阈值时，才会切换回“关闭”状态。
* <code>**resetTimePeriod**</code>: 当熔断器状态为“打开”时，等待多长时间后进入“半开”状态进行重试。
* <code>**lastFailureTime**</code>: 上次切换为打开状态的时间，帮助判断是否需要进入“半开”状态。

## 3.这个枚举类是干什么的？

**熔断器状态（**<code>**state**</code>**）**: 记录熔断器当前的状态，有三种状态：

* **CLOSED（关闭）**：正常状态，所有请求都被允许。
* **OPEN（打开）**：服务出现大量失败，熔断器处于打开状态，不允许任何请求。
* **HALF\_OPEN（半开）**：熔断器恢复状态，允许一些请求进入以验证服务是否恢复。

## 4.讲讲这几个方法

### `allowRequest` 方法

<code>**allowRequest()**</code>: 这个方法用于检查当前的熔断器状态，并决定是否允许新的请求。

* 如果熔断器处于 `OPEN`（打开）状态，它会检查自上次失败以来是否已经过了指定的重置时间。如果过了，熔断器会进入 `HALF_OPEN`（半开）状态并允许请求；否则，它会继续阻止所有请求。
* 如果熔断器处于 `HALF_OPEN` 状态，它允许请求并记录该请求，判断是否应该恢复到 `CLOSED` 状态。
* 如果熔断器处于 `CLOSED` 状态，默认拒绝请求。

### `recordSuccess` 方法

<code>**recordSuccess()**</code>: 该方法用于记录一个成功的请求。

* 如果熔断器处于 `HALF_OPEN` 状态，它会增加成功计数。如果成功请求数超过设定的成功率阈值，熔断器会切换回 `CLOSED`（关闭）状态。
* 如果熔断器不在 `HALF_OPEN` 状态，说明不需要检查成功率，直接重置计数器。

### `recordFailure` 方法

<code>**recordFailure()**</code>: 该方法用于记录一个失败的请求。

* 如果熔断器处于 `HALF_OPEN` 状态，失败后会切换到 `OPEN`（打开）状态。
* 如果失败次数超过了设定的阈值，熔断器会直接切换到 `OPEN` 状态，阻止后续请求。

# CircuitBreakProvider

```java
public class CircuitBreakerProvider {

    // 用一个 Map 存储每个服务的熔断器实例，key 为服务名，value 为该服务的熔断器
    private Map<String, CircuitBreaker> circuitBreakerMap = new HashMap<>();

    // 根据服务名获取对应的熔断器
    public synchronized CircuitBreaker getCircuitBreaker(String serviceName) {
        CircuitBreaker circuitBreaker;
        
        // 检查是否已经存在该服务的熔断器
        if (circuitBreakerMap.containsKey(serviceName)) {
            // 如果存在，则直接返回对应的熔断器
            circuitBreaker = circuitBreakerMap.get(serviceName);
        } else {
            // 如果不存在，则创建一个新的熔断器
            System.out.println("serviceName=" + serviceName + " 创建一个新的熔断器");
            circuitBreaker = new CircuitBreaker(1, 0.5, 10000); // 这里传入熔断器的参数：失败阈值1，成功率50%，重置时间周期10000毫秒
            circuitBreakerMap.put(serviceName, circuitBreaker); // 将新创建的熔断器存入 Map
        }

        return circuitBreaker; 
    }
}

```

## 1.本类的目的？

`CircuitBreakerProvider` 类的作用是根据服务名称（`serviceName`）提供一个对应的熔断器（`CircuitBreaker`）。它使用一个 `Map` 来存储每个服务的熔断器实例，并在请求时返回对应的熔断器。如果该服务的熔断器不存在，则会创建一个新的熔断器并将其加入到 `Map` 中。

# 修改ClientProxy

增加熔断逻辑

```java
private RpcClient rpcClient;
private ServiceCenter serviceCenter;
private CircuitBreakerProvider circuitBreakerProvider;
public ClientProxy() throws InterruptedException {
    serviceCenter = new ZKServiceCenter();
    rpcClient=new NettyRpcClient();
    circuitBreakerProvider=new CircuitBreakerProvider();
}
```

```java
@Override
public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
    //构建request
    RpcRequest request= RpcRequest.builder()
    .interfaceName(method.getDeclaringClass().getName())
    .methodName(method.getName())
    .params(args).paramsType(method.getParameterTypes()).build();
    //熔断器
    CircuitBreaker circuitBreaker = circuitBreakerProvider.getCircuitBreaker(method.getName());
    //判断熔断器是否允许通过
    if (!circuitBreaker.allowRequest()) {
        //这里可以针对熔断进行特殊处理，返回特殊值
        return null;
    }
    //数据传输
    RpcResponse response= rpcClient.sendRequest(request);
    //添加逻辑：保持幂等性，只对白名单上的服务进行重试
    if (serviceCenter.checkRetry(request.getInterfaceName())) {
        //调用retry框架进行重试操作
        response = new guavaRetry().sendServiceWithRetry(request, rpcClient);
    } else {
        //只调用一次
        response = rpcClient.sendRequest(request);
    }
    return response.getData();
}
```

# 修改 TestClient 类

## 1. 本类的作用

`TestClient` 类的目的是使用 `ClientProxy` 创建一个 `UserService` 的代理对象，并模拟并发调用该服务中的方法。每个线程会调用 `getUserByUserId` 和 `insertUserId` 方法，并在控制台输出结果。你通过在每 30 次调用后暂停 10 秒来模拟实际的负载

## 2.做出的修改

```java
public class TestClient {
    public static void main(String[] args) throws InterruptedException {
        ClientProxy clientProxy=new ClientProxy();
        UserService proxy=clientProxy.getProxy(UserService.class);
        for(int i = 0; i < 120; i++) {
            Integer i1 = i;
            if (i%30==0) {
                Thread.sleep(10000);
            }
            new Thread(()->{
                try{
                    User user = proxy.getUserByUserId(i1);

                    System.out.println("从服务端得到的user="+user.toString());

                    Integer id = proxy.insertUserId(User.builder().id(i1).userName("User" + i1.toString()).sex(true).build());
                    System.out.println("向服务端插入user的id"+id);
                } catch (NullPointerException e){
                    System.out.println("user为空");
                    e.printStackTrace();
                }
            }).start();
        }
    }
    //User user = proxy.getUserByUserId(1);
    //System.out.println("从服务端得到的user="+user.toString());
    //
    //User u=User.builder().id(100).userName("wxx").sex(true).build();
    //Integer id = proxy.insertUserId(u);
    //System.out.println("向服务端插入user的id"+id);
}
```


> 更新: 2024-11-29 19:49:39  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/pl5yl2wig5m99rzz>