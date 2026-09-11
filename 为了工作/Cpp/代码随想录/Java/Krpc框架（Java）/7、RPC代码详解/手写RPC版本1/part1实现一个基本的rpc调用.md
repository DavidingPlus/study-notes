# part1实现一个基本的rpc调用

<font style="color:rgb(55, 65, 81);">假设A，B位于不同的服务器，A 想远程调用 B的xxx方法，该通过什么方式来完成这一操作呢</font>

![1732621886511-b9ebd787-3bf3-48ad-9ebd-55b1c398a69a.png](https://cdn.davidingplus.cn/images/2026/09/11/1732621886511-b9ebd787-3bf3-48ad-9ebd-55b1c398a69a-218551.png)

<font style="color:rgb(55, 65, 81);">例如：</font>

<font style="color:rgb(55, 65, 81);">有以下这样一个场景：</font>

**服务端B：**

<font style="color:rgb(55, 65, 81);">有一个用户表</font>

* <font style="color:rgb(55, 65, 81);">1.UserService 里有一个功能： getUserByUserId(Integer id)</font>
* <font style="color:rgb(55, 65, 81);">2.UserServiceImpl 实现了UserService接口和方法</font>

**客户端A：**

<font style="color:rgb(55, 65, 81);">调用getUserByUserId方法， 内部传一个Id给服务端，服务端查询到User对象返回给客户端</font>

<font style="color:rgb(55, 65, 81);">如何实现以上调用过程呢？</font>

<font style="color:rgb(55, 65, 81);">我们可以从以下几个方面来思考：</font>

* **客户端A怎么实现**<font style="color:rgb(55, 65, 81);">：</font>
  * <font style="color:rgb(55, 65, 81);">调用getUserByUserId方法时，内部将调用信息处理后发送给服务端B，告诉B我要获取User</font>
  * <font style="color:rgb(55, 65, 81);">外部调用方法，内部进行其它的处理——这种场景我们可以使用动态代理的方式，改写原本方法的处理逻辑</font>

<font style="color:rgb(55, 65, 81);">（比如，当我们正常调用getUserByUserId方法，代码逻辑是去数据库中找user，但是在当前远程调用的场景下肯定不能走这样的逻辑；我们通过动态代理的方式，绕过【去数据库查询】这原始的逻辑，改成封装信息发送到B中请求调用服务）</font>

* **服务端B怎么实现：**
  * <font style="color:rgb(55, 65, 81);">监听到A的请求后，接收A的调用信息，并根据信息得到A想调用的服务与方法</font>
  * <font style="color:rgb(55, 65, 81);">根据信息找到对应的服务，进行调用后将结果发送回给A</font>
* **A与B之间怎么通信：**
  * <font style="color:rgb(55, 65, 81);">使用Java的socket网络编程进行通信</font>
  * <font style="color:rgb(55, 65, 81);">为了方便A ，B之间 对接收的消息进行处理，我们需要将请求信息和返回信息封装成统一的消息格式</font>

# 公共部分

创建 common 包->pojo 包->User 类->service 包->UserService 接口->impl 包->

UserServiceImpl 类->Message 包->RpcRequest 类->RpcResponse 类

## User

```java
@Builder
@Data
@NoArgsConstructor
@AllArgsConstructor
public class User implements Serializable {//实现了 Serializable 接口，表示该类可以序列化。
    // 客户端和服务端共有的
    private Integer id;
    private String userName;
    private Boolean sex;
}
```

### 1.上面的四个注解有什么作用？

* <code>**@Builder**</code>：
  * `@Builder` 是 Lombok 提供的一个注解，自动生成一个 `Builder` 模式的实现。它允许通过链式调用来构建 `User` 对象。例如，可以这样创建 `User` 对象：

```java

User user = User.builder().id(1).userName("John").sex(true).build();
```

* 使用 `Builder` 模式可以避免构造函数参数过多时的麻烦，提升代码的可读性和可维护性。
* <code>**@Data**</code>：
  * `@Data` 是 Lombok 提供的一个注解，它相当于组合了多个常用的注解，包括 `@Getter`、`@Setter`、`@ToString`、`@EqualsAndHashCode` 和 `@RequiredArgsConstructor`。
  * `@Getter` 和 `@Setter` 会为所有字段自动生成 getter 和 setter 方法。
  * `@ToString` 会自动生成 `toString` 方法，便于输出对象的字符串表示。
  * `@EqualsAndHashCode` 会自动生成 `equals()` 和 `hashCode()` 方法，确保对象比较的一致性。
  * `@RequiredArgsConstructor` 会自动生成一个构造函数，只包括类中 `final` 字段和带有 `@NonNull` 注解的字段（这里没有 `final` 字段，所以只会生成一个无参构造函数）。
* <code>**@NoArgsConstructor**</code>：
  * `@NoArgsConstructor` 会自动生成一个无参构造函数。通常是为了方便使用反射、序列化框架（如 Jackson）或者在创建对象时不传入参数的情况。
* <code>**@AllArgsConstructor**</code>：
  * `@AllArgsConstructor` 会生成一个全参构造函数，该构造函数会包含所有字段。

### 2.为什么要实现 Serializable 接口？

* `User` 类是一个普通的 POJO（Plain Old Java Object）类。
* 该类实现了 `Serializable` 接口，这意味着 `User` 对象可以被转换为字节流（序列化）以便保存或传输，反之，也可以从字节流中恢复（反序列化）。在分布式应用中，常常需要将对象在网络中传输，或者存储到文件、数据库等，因此序列化是一种必不可少的功能。

### 3.有哪些字段？

* `id`：表示用户的唯一标识符，类型为 `Integer`。
* `userName`：表示用户的用户名，类型为 `String`。
* `sex`：表示用户的性别，类型为 `Boolean`，`true` 表示男，`false` 表示女（假设性别只有这两种情况）。

## <font style="color:rgb(38, 38, 38);">UserService</font>

定义调用所需要的服务接口

```java
// 客户端通过这个接口调用服务端的实现类
public interface UserService {
    //根据ID查询用户
    User getUserByUserId(Integer id);
    //新增一个功能
    Integer insertUserId(User user);
}
```

## <font style="color:rgb(38, 38, 38);">UserServiceImpl</font>

```java
public class UserServiceImpl implements UserService {
    @Override
    public User getUserByUserId(Integer id) {
        System.out.println("客户端查询了"+id+"的用户");
        // 模拟从数据库中取用户的行为
        Random random = new Random();
        //random.nextBoolean()：随机生成 true 或 false，表示用户的性别。
        User user = User.builder().userName(UUID.randomUUID().toString())
        .id(id)
        .sex(random.nextBoolean()).build();
        return user;
    }

    @Override
    public Integer insertUserId(User user) {
        System.out.println("插入数据成功"+user.getUserName());
        return user.getId();
    }
}
```

### 1.UUID.randomUUID() 是做什么用的?

UUID.randomUUID()：生成一个全局唯一的字符串，作为随机用户名。

### 2.User.builder 是做什么的？

User.builder()：使用 Lombok 的 Builder 模式构造一个新的用户对象。

## <font style="color:rgb(38, 38, 38);">RpcRequest</font>

```java
@Data
@Builder
public class RpcRequest implements Serializable {
    //服务类名，客户端只知道接口，在服务端接口指向实现类
    private String interfaceName;
    //调用的方法名
    private String methodName;
    //参数列表
    private Object[] params;
    //参数类型
    private Class<?>[] paramsType;
}
```

### 1. 请求消息中有哪些字段？

接口名、调用的方法名、参数列表、参数类型

### 2.<font style="color:rgb(55, 65, 81);">为什么请求信息中的服务类名定为接口名？</font>

<font style="color:rgb(55, 65, 81);">因为我们使用动态代理，外部给定的信息是接口信息</font>

## <font style="color:rgb(38, 38, 38);">RpcResponse</font>

```java
@Data
@Builder
public class RpcResponse implements Serializable {
    //状态信息
    private int code;
    private String message;
    //具体数据
    private Object data;
    //构造成功信息
    public static RpcResponse sussess(Object data){
        return RpcResponse.builder().code(200).data(data).build();
    }
    //构造失败信息userService.getUserByUserId(id);
    public static RpcResponse fail(){
        return RpcResponse.builder().code(500).message("服务器发生错误").build();
    }
}
```

### 1. 响应消息中有哪些信息？

状态码、状态信息、具体数据、构建成功信息、构建失败信息

# 客户端

创建 Client 包-> IOClient->proxy 包->ClientProxy 类->TestClient 类

## IOClient

```java
public class IOClient {
    //这里负责底层与服务端的通信，发送request，返回response
    public static RpcResponse sendRequest(String host, int port, RpcRequest request){//服务端的主机IP地址、端口号、请求对象
        try {
            Socket socket=new Socket(host, port);//通过socket与服务端建立 TCP 连接。
            //将对象序列化发送到服务端。
            ObjectOutputStream oos=new ObjectOutputStream(socket.getOutputStream());
            //接收并反序列化对象
            ObjectInputStream ois=new ObjectInputStream(socket.getInputStream());
            //将 RpcRequest 对象序列化，并通过输出流发送到服务端。
            oos.writeObject(request);
            //刷新输出流以确保数据完全发送
            oos.flush();

            //从输入流中读取服务端返回的序列化对象，并反序列化为 RpcResponse。
            RpcResponse response=(RpcResponse) ois.readObject();
            return response;
            //与网络通信相关的异常、反序列化对象时找不到对应类的异常
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
            return null;
        }
    }
}
```

#### ObjectOutputStream oos = newObjectOutputStream(socket.getOutputStream());

* <code>**ObjectOutputStream**</code>\*\* 是一个用于将对象序列化成字节流的类。在这行代码中，**<code>**socket.getOutputStream()**</code>** 获取到连接的输出流，接着通过 **<code>**ObjectOutputStream**</code>** 对请求对象进行序列化。\*\*
* <code>**RpcRequest**</code>\*\* 是一个普通的 Java 对象，通过 **<code>**ObjectOutputStream**</code>** 将其转换为字节流并通过网络发送给服务端。\*\*

#### oos.writeObject(request);

`writeObject(request)` 方法将 `request` 对象序列化并写入到输出流中，准备发送到服务端。`RpcRequest` 对象会被转换成字节流，并通过网络传输。

#### oos.flush();

`flush()` 方法用于刷新输出流，确保所有数据都被发送到服务端。虽然 `ObjectOutputStream` 会在 `writeObject()` 后自动进行刷新，但手动调用 `flush()` 可以确保数据立即发送，特别是在有多个写操作时，确保数据的即时性。

#### ObjectInputStream ois = newObjectInputStream(socket.getInputStream());

`ObjectInputStream` 用于从输入流中读取对象并进行反序列化。在这里，`socket.getInputStream()` 获取到从服务端返回的输入流，通过 `ObjectInputStream` 将数据反序列化为 Java 对象。

#### RpcResponse response= (RpcResponse) ois.readObject();

* `readObject()` 方法从输入流中读取数据，并将其反序列化为一个 `RpcResponse` 对象。这里假设服务端返回的就是一个 `RpcResponse` 类型的对象。
* `RpcResponse` 对象通常包含服务端处理后的结果，如方法执行结果、状态码等。

### 1. 底层通信做了哪些事情？

建立连接、发送请求、接收响应、异常处理

## <font style="color:rgb(38, 38, 38);">ClientProxy</font>

```java
@AllArgsConstructor
public class ClientProxy implements InvocationHandler {
    //传入参数service接口的class对象，反射封装成一个request
    //初始化代理类时传入 host 和 port
    private String host;
    private int port;

    //jdk动态代理，每一次代理对象调用方法，都会经过此方法增强（反射获取request对象，socket发送到服务端）
    //核心逻辑，用于封装请求并处理服务端响应。
    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        //构建request
        RpcRequest request=RpcRequest.builder()
        .interfaceName(method.getDeclaringClass().getName())
        .methodName(method.getName())
        .params(args).paramsType(method.getParameterTypes()).build();
        //与服务端进行通信，将请求发送出去，并接收 RpcResponse 响应。
        RpcResponse response= IOClient.sendRequest(host,port,request);
        //获取服务端返回的结果，并返回给调用者。
        return response.getData();
    }
    //动态生成一个实现指定接口的代理对象。
    public <T>T getProxy(Class<T> clazz){
        //使用 Proxy.newProxyInstance 方法动态创建一个代理对象，传入类加载器、需要代理的接口和调用处理程序。
        Object o = Proxy.newProxyInstance(clazz.getClassLoader(), new Class[]{clazz}, this);
        return (T)o;
    }
}
```

### 1.为什么要实现 InvocationHandler 接口

JDK 动态代理的核心功能是通过反射生成一个代理对象，该对象可以代理一个或多个接口的方法调用。当代理对象的方法被调用时，实际的调用逻辑由 `InvocationHandler` 接口的 `invoke` 方法来处理。

* `InvocationHandler` 接口有一个 `invoke` 方法，该方法会在代理对象的方法被调用时执行。
* 为了使 `ClientProxy` 成为一个有效的代理类，需要实现 `InvocationHandler` 接口，并提供 `invoke` 方法的实现。通过这种方式，所有对代理对象方法的调用都可以通过 `invoke` 方法进入，从而执行我们定义的增强逻辑（如远程调用、参数封装、响应处理等）。

实现 `InvocationHandler` 的主要目的就是能够拦截被代理对象的方法调用，并在 `invoke` 方法中执行定制的处理逻辑。比如，在你提供的 `ClientProxy` 类中，`invoke` 方法的核心功能是：

* **封装请求**：将方法调用的信息封装成一个 `RpcRequest` 对象。
* **网络通信**：通过网络向远程服务发送请求，并接收服务端的响应。
* **返回结果**：将远程服务返回的数据返回给调用者。

通过 `InvocationHandler`，你可以控制代理对象方法的行为，实现远程调用的透明化，隐藏远程调用的复杂性。

### 2. 动态代理中有哪些字段？什么作用？

* `host`：服务端的地址，可能是 IP 地址或者主机名。
* `port`：服务端的端口号，用于连接远程服务。

用于建立与远程服务的通信

### 3.invoke 方法的作用？里面的参数表示什么？

`invoke` 方法是 `InvocationHandler` 接口的核心方法，它被调用时会处理所有通过代理对象调用的方法。每当代理对象的方法被调用时，`invoke` 方法都会被触发，从而实现增强逻辑（如远程调用）。

## 参数

* `proxy`：代理对象本身。
* `method`：被调用的方法（`Method` 类型）。通过反射可以获取方法的详细信息。
* `args`：方法的参数。

### 4.如何获得一个代理对象？

通过 getProxy 方法

`getProxy` 方法用于动态生成一个指定接口的代理对象，利用 JDK 的动态代理机制。

**方法逻辑：**

* `Proxy.newProxyInstance(clazz.getClassLoader(), new Class[]{clazz}, this)`：
  * `clazz.getClassLoader()`：传入接口的类加载器。
  * `new Class[]{clazz}`：指定代理接口，`clazz` 就是需要被代理的接口。
  * `this`：`InvocationHandler` 的实现，即当前的 `ClientProxy` 对象，它会处理所有方法调用。

这个方法会返回一个代理对象，调用该对象的方法会被转发到 `ClientProxy` 的 `invoke` 方法中，进而实现远程调用。

## TestClient

```java
public class TestClient {
    public static void main(String[] args) {
        //创建 ClientProxy 对象：初始化 ClientProxy 对象，连接到指定的服务器地址和端口。
        ClientProxy clientProxy=new ClientProxy("127.0.0.1",9999);
        //获得UserService的代理对象
        UserService proxy=clientProxy.getProxy(UserService.class);

        User user = proxy.getUserByUserId(1);
        System.out.println("从服务端得到的user="+user.toString());

        User u=User.builder().id(100).userName("wxx").sex(true).build();
        Integer id = proxy.insertUserId(u);
        System.out.println("向服务端插入user的id"+id);
    }
}
```

### 1.客户端如何测试？

* `创建 ClientProxy` 作为一个代理类，处理远程方法调用的封装、发送请求和接收响应的工作。
* 客户端通过 `clientProxy.getProxy(UserService.class)` 动态生成 `UserService` 接口的代理对象。
* 使用代理对象调用 `getUserByUserId` 和 `insertUserId` 方法时，实际上是在触发 `ClientProxy` 中 `invoke` 方法的执行，后者会封装成远程调用请求并通过网络传递给服务端。
* 服务端处理完请求后返回数据，客户端获取并输出这些数据。

# 服务端

创建 Server 包->provider 包->server 包->RpcServer 接口->ServiceProvider 类->work 包->WorkThread 类->impl 包->SimpleRPCServer 类->ThreadPoolRPCServer 类->TestServer 类

## RpcServer

```java
public interface RpcServer {
    //开启监听
    void start(int port);
    void stop();
}
```

### 1.本接口的方法有哪些？作用是什么？

start：开启服务端监听

stop：停止服务端服务

## ServiceProvider

```java
//本地服务存放器
public class ServiceProvider {
    //集合中存放服务的实例,接口的全限定名（String 类型）,接口对应的实现类实例（Object 类型）。
    private Map<String,Object> interfaceProvider;
    //为 interfaceProvider 字段分配一个新的 HashMap 实例
    public ServiceProvider(){
        this.interfaceProvider=new HashMap<>();
    }
    //本地注册服务
    public void provideServiceInterface(Object service){//接收一个服务实例（service）
        String serviceName=service.getClass().getName();//获取服务对象的完整类名
        Class<?>[] interfaceName=service.getClass().getInterfaces();//获取服务对象实现的所有接口

        for (Class<?> clazz:interfaceName){//遍历 service 实现的所有接口。
            interfaceProvider.put(clazz.getName(),service);//将接口的全限定名和对应的服务的实例添加到 map 中。
        }

    }
    //获取服务实例
    public Object getService(String interfaceName){
        return interfaceProvider.get(interfaceName);
    }
}
```

### 1.本类的作用是什么？

`ServiceProvider` 类提供了一种简单的方式来注册和获取服务，它将服务对象和接口名称进行了映射，从而实现了服务的本地注册和动态获取。

主要有两个方法

#### `provideServiceInterface(Object service)` 方法

**作用**：此方法用于将一个服务实例注册到 `interfaceProvider` 中，将服务对象与其实现的接口关联起来。

* **获取服务的接口**：
  * `service.getClass().getInterfaces()`：通过反射获取服务对象实现的所有接口（`Class<?>[]` 类型）。每个服务对象可能实现多个接口，所以返回的是一个接口数组。
* **遍历接口并注册服务**：
  * 遍历 `service` 实现的每个接口，将接口的全限定类名作为 `Map` 的键，服务实例作为 `Map` 的值，注册到 `interfaceProvider` 中。也就是说，`Map` 中的每个键值对表示某个接口与对应服务实例的映射关系。
* 例如，如果 `service` 实现了接口 `UserService` 和 `OrderService`，那么方法会将 `UserService` 和 `OrderService` 的全限定类名作为键，`service` 实例作为值，存储在 `interfaceProvider` 中。

#### `getService(String interfaceName)` 方法

**作用**：根据接口的全限定类名（`interfaceName`），从 `interfaceProvider` 中获取对应的服务实例。

* `interfaceProvider.get(interfaceName)`：从 `interfaceProvider` 中查找与指定接口名称对应的服务实例。如果找到了对应的服务实例，就返回它；如果没有找到，返回 `null`。
* 这个方法提供了根据接口名称获取服务实例的能力，可以动态地获取服务对象，具体调用哪个服务接口完全由接口名称决定。

### 2.Map 中的 key 与 value 代表什么？

Map 是用来存放服务实例的

key 是服务接口全限定类名，即接口的完整路径名

value 是服务实例，实现了该接口的对象

## WorkThread

```java
@AllArgsConstructor
public class WorkThread implements Runnable {
    private Socket socket;//建立网络连接
    private ServiceProvider serviceProvide;//本地服务注册中心

    @Override
    public void run() {
        try {
            //将响应数据（即服务端返回的 RpcResponse）通过网络连接发送给客户端。
            ObjectOutputStream oos = new ObjectOutputStream(socket.getOutputStream());
            //从客户端的网络连接中接收数据，读取序列化的对象
            ObjectInputStream ois = new ObjectInputStream(socket.getInputStream());
            //读取客户端传过来的request
            RpcRequest rpcRequest = (RpcRequest) ois.readObject();
            //反射调用服务方法获取返回值
            RpcResponse rpcResponse = getResponse(rpcRequest);
            //向客户端写入response
            oos.writeObject(rpcResponse);
            oos.flush();
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }
    //处理客户端请求，根据请求内容调用对应服务的方法，并返回执行结果
    private RpcResponse getResponse(RpcRequest rpcRequest) {
        //得到服务名
        String interfaceName = rpcRequest.getInterfaceName();
        //得到服务端相应服务实现类
        Object service = serviceProvide.getService(interfaceName);
        //反射调用方法
        Method method = null;
        try {
            //获得方法对象
            method = service.getClass().getMethod(rpcRequest.getMethodName(), rpcRequest.getParamsType());
            //通过反射调用方法
            Object invoke = method.invoke(service, rpcRequest.getParams());
            //封装响应对象并返回
            return RpcResponse.sussess(invoke);
            //找不到请求的方法、方法无法访问、方法执行过程中抛出异常
        } catch (NoSuchMethodException | IllegalAccessException | InvocationTargetException e) {
            e.printStackTrace();
            System.out.println("方法执行错误");
            return RpcResponse.fail();
        }
    }
}
```

### 1.本类的作用是？

实现了 `Runnable` 接口，用于处理客户端请求并返回响应。其核心功能是在多线程环境中接收来自客户端的请求，调用本地服务，并将服务的结果返回给客户端。

### 2.为什么要实现 Runable 接口？

实现 `Runnable` 接口的主要原因是为了使 `WorkThread` 类能够在多线程环境中运行。`Runnable` 接口提供了一个标准的方式来定义线程的任务，它的 `run()` 方法包含了线程执行的具体代码

**线程池的使用**：如果没有实现 `Runnable` 接口，无法将 `WorkThread` 交给线程池来管理。通过实现 `Runnable` 接口，可以将 `WorkThread` 类作为任务提交给线程池，线程池会为每个请求分配一个线程来执行。这种方式是高效且易于扩展的。

### 3.Socket 字段与 serviceProvide 字段的作用？

#### `socket` 字段

* **作用**：用于与客户端进行网络通信。`Socket` 是 Java 中用于网络通信的类，它表示了一个 TCP 连接端点，客户端和服务器通过 `Socket` 进行数据传输。在这段代码中，`socket` 字段用于接收来自客户端的请求，并通过该 `socket` 返回处理结果。

#### 具体功能：

* **接收客户端请求**：当客户端通过网络发送请求时，`socket.getInputStream()` 用于读取客户端发送过来的数据（例如 `RpcRequest` 对象）。
* **发送响应数据**：`socket.getOutputStream()` 用于将处理结果通过网络发送回客户端。在 `run()` 方法中，通过 `ObjectOutputStream` 将 `RpcResponse` 响应对象发送回客户端。

在 `WorkThread` 中，`socket` 代表了服务器与某个特定客户端之间的连接，`WorkThread` 使用这个 `socket` 来与客户端通信。

#### `serviceProvide` 字段

* **作用**：`serviceProvide` 是一个本地的服务注册中心，类型是 `ServiceProvider`。它负责管理本地服务的注册和查找。`ServiceProvider` 根据接口名称返回对应的服务实现对象。在 `WorkThread` 中，它的作用是通过接口名称从本地获取相应的服务实例，然后调用该服务的方法处理请求。

#### 具体功能：

* **查找服务**：当客户端请求某个服务时（通过 `RpcRequest` 中的接口名称），`serviceProvide.getService(interfaceName)` 方法会根据请求的接口名称，从注册中心获取对应的服务实例。
* **服务调用**：一旦获取到服务实例，`WorkThread` 就可以使用反射调用该服务实例的方法，处理客户端的请求。

`serviceProvide` 是 `WorkThread` 类与本地服务之间的桥梁，它使得 `WorkThread` 能够根据请求动态地查找并调用相应的服务。

### 4.讲讲 getResponse

**1. 获取服务实现对象**：

* `String interfaceName = rpcRequest.getInterfaceName()`：从 `RpcRequest` 中获取客户端请求的接口名称。
* `Object service = serviceProvide.getService(interfaceName)`：通过 `ServiceProvider` 获取对应的服务实现对象，`serviceProvide.getService(interfaceName)` 方法根据接口名称查找本地已注册的服务实例。

**2. 反射调用方法**：

* `method = service.getClass().getMethod(rpcRequest.getMethodName(), rpcRequest.getParamsType())`：使用反射根据请求中的方法名（`rpcRequest.getMethodName()`）和参数类型（`rpcRequest.getParamsType()`）获取对应的 `Method` 对象。
* `Object invoke = method.invoke(service, rpcRequest.getParams())`：通过 `Method` 对象调用服务的方法，并传入请求中的参数（`rpcRequest.getParams()`）。这是一个动态方法调用，能够在运行时确定调用哪个服务方法。

**3. 封装响应并返回**：

* `return RpcResponse.sussess(invoke)`：如果方法调用成功，则封装方法返回值（`invoke`）为成功的响应对象 (`RpcResponse.sussess`) 并返回给客户端。

**4. 异常处理**：

* `catch (NoSuchMethodException | IllegalAccessException | InvocationTargetException e)`：捕获反射过程中可能出现的异常，如：
  * `NoSuchMethodException`：找不到方法。
  * `IllegalAccessException`：方法不可访问。
  * `InvocationTargetException`：方法执行过程中抛出的异常。
* 如果捕获到异常，打印异常堆栈信息并返回一个失败的响应 (`RpcResponse.fail()`)，表示请求处理失败。

## SimpleRPCServer

```java
@AllArgsConstructor
public class SimpleRPCRPCServer implements RpcServer {
    private ServiceProvider serviceProvide;//本地注册中心
    @Override
    public void start(int port) {
        try {
            //创建一个 ServerSocket 实例，用于在指定的 port 端口上监听客户端的连接请求
            ServerSocket serverSocket=new ServerSocket(port);
            System.out.println("服务器启动了");
            //服务器持续接受客户端的连接请求
            while (true) {
                //如果没有连接，会堵塞在这里
                Socket socket = serverSocket.accept();
                //有连接，创建一个新的线程执行处理
                new Thread(new WorkThread(socket,serviceProvide)).start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void stop() {
        //停止服务端
        //可以在未来版本中优化服务端关闭的流程。
    }
}
```

### 1.本类的作用是什么？

`SimpleRPCRPCServer` 类实现了 `RpcServer` 接口，用于启动一个简单的 RPC 服务器，并监听客户端的连接请求，处理客户端请求，并通过线程并发处理每个连接。

### 2.ServerSocket 是干什么的？

<code>**ServerSocket serverSocket = new ServerSocket(port);**</code>

创建一个 `ServerSocket` 实例，该对象用于在指定的端口（`port`）上监听客户端的连接请求。`ServerSocket` 是一个 TCP 服务器端的类，它负责接收客户端的连接并为每个连接创建一个新的 `Socket` 对象。

## <font style="color:rgb(38, 38, 38);">ThreadPoolRPCServer</font>

```java
public class ThreadPoolRPCRPCServer implements RpcServer {
    private final ThreadPoolExecutor threadPool;//定义一个线程池对象 threadPool，用于管理和执行线程任务
    private ServiceProvider serviceProvider;
    //默认构造方法：创建一个线程池，核心线程数等于 CPU 核心数，最大线程数 1000，非核心线程空闲存活时间60秒，队列大小为 100。
    public ThreadPoolRPCRPCServer(ServiceProvider serviceProvider){
        threadPool=new ThreadPoolExecutor(Runtime.getRuntime().availableProcessors(),
                                          1000,60, TimeUnit.SECONDS,new ArrayBlockingQueue<>(100));
        this.serviceProvider= serviceProvider;
    }
    //自定义构造方法：允许用户传入线程池参数，自定义线程池配置。
    public ThreadPoolRPCRPCServer(ServiceProvider serviceProvider, int corePoolSize,
                                  int maximumPoolSize,
                                  long keepAliveTime,
                                  TimeUnit unit,
                                  BlockingQueue<Runnable> workQueue){

        threadPool = new ThreadPoolExecutor(corePoolSize, maximumPoolSize, keepAliveTime, unit, workQueue);
        this.serviceProvider = serviceProvider;
    }

    @Override
    public void start(int port) {
        System.out.println("服务端启动了");
        try {
            //初始化 ServerSocket，监听端口。
            ServerSocket serverSocket=new ServerSocket();
            while (true){
                //接受连接：通过 accept() 阻塞等待客户端连接，返回 Socket 对象。
                Socket socket= serverSocket.accept();
                //使用线程池分发任务，每个客户端请求交给线程池管理。
                threadPool.execute(new WorkThread(socket,serviceProvider));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void stop() {

    }
}
```

### 1.这个类的作用是什么

`ThreadPoolRPCRPCServer` 类是一个实现了 `RpcServer` 接口的 RPC 服务器，它通过线程池来管理和执行请求处理任务，以提高并发处理能力。

### 2.对比简单实现，线程池实现有什么好处？

相比于 `SimpleRPCRPCServer`，这个类通过使用线程池来管理工作线程，可以更有效地处理大量并发请求，并避免每个请求都创建一个新的线程导致性能问题。

### 3.threadPool 对象是用来做什么的？

这是一个 `ThreadPoolExecutor` 类型的字段，表示服务器使用的线程池。通过线程池来管理和执行客户端请求的处理任务，以避免频繁创建销毁线程的问题。

### 4.构造方法中那几个参数是什么？

#### 默认构造方法

* **核心线程数**：设为 CPU 核心数 (`Runtime.getRuntime().availableProcessors()`)。
* **最大线程数**：设置为 1000，表示线程池最多可以有 1000 个工作线程。
* **非核心线程空闲存活时间**：设置为 60 秒。超过此时间后，非核心线程会被回收。
* **队列大小**：使用一个大小为 100 的 `ArrayBlockingQueue`，该队列用于存放待处理的任务（客户端请求）。

#### 自定义构造方法

允许用户通过参数自定义线程池的配置，包括：

* `corePoolSize`：核心线程数
* `maximumPoolSize`：最大线程数
* `keepAliveTime` ：非核心线程的最大空闲存活时间
* `unit`：设置时间单位，默认构造方法的是秒
* `workQueue`：线程池使用的工作队列（存放待执行任务的队列）。

## TestServer

```java
public class TestServer {
    public static void main(String[] args) {
        //创建服务实现类
        UserService userService=new UserServiceImpl();
        //实例化服务注册中心，用于管理所有可供客户端调用的服务。
        ServiceProvider serviceProvider=new ServiceProvider();
        //注册服务
        serviceProvider.provideServiceInterface(userService);
        //实例化服务端
        RpcServer rpcServer=new SimpleRPCRPCServer(serviceProvider);
        //启动服务端
        rpcServer.start(9999);
    }
}
```

### 1. 服务端是如何测试的？

1. 根据 UserService 创建一个服务实现类
2. 实例化服务注册中心， 服务端通过 `ServiceProvider` 将服务注册到中心，供客户端查找并调用。这个注册中心管理着服务接口与对应实现类之间的映射关系。
3. 注册服务到服务注册中心 ，使得客户端能够根据接口名称或标识查找到对应的服务。
4. 实例化并启动 RPC 服务端 ，可以在这里选择使用简单服务端或线程池服务端
5. 启动服务端


> 更新: 2024-11-29 17:46:36  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/sagz6yby53czxmhc>