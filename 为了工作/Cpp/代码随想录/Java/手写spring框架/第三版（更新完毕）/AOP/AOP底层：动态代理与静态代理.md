# AOP 底层：动态代理与静态代理

在了解了 AOP 的基本概念之后，我们需要看一下实现 AOP 的底层技术，主要包括：

* 反射（自行学习）
* 动态代理
* CGLIB（第三方库）

在了解这些底层技术之前，我们先看看**静态代理**。静态代理其实并不是 AOP 底层使用的技术，但是它结构清晰，逻辑直白，非常适合用来**了解代理的工作机制**。

## 静态代理

下面是一个简单的示例：

首先我们先定义一个接口 `UserService`，并且创建一个实现了该接口的类 `UserServiceImpl`。

```java
// UserService.java
public interface UserService {
    void registerUser(String username);
}

// UserServiceImpl.java
public class UserServiceImpl implements UserService {
    @Override
    public void registerUser(String username) {
        System.out.println("用户 " + username + " 注册成功");
    }
}
```

然后再创建一个代理类，这个代理类同样也会实现 `UserService` 接口，**并且会持有 UserServiceImpl 的引用。**

```java
// UserServiceProxy.java
public class UserServiceProxy implements UserService {
    private UserService target;

    public UserServiceProxy(UserService target) {
        this.target = target;
    }

    @Override
    public void registerUser(String username) {
        // 前置通知
        System.out.println("[前置通知]: 用户 " + username + " 开始注册");

        // 调用目标方法
        target.registerUser(username);

        // 后置通知
        System.out.println("[后置通知]: 用户 " + username + " 完成注册");
    }
}
```

在后续代码中，我们通过调用代理类实现的方法，代替直接调用目标对象的方法，就可以模拟一个简单的 AOP 实现了。

```java
// Main.java
public class Main {
    public static void main(String[] args) {
        UserService userService = new UserServiceImpl(); // 创建目标对象
        UserService proxy = new UserServiceProxy(userService); // 创建代理对象
        
        // 使用代理对象进行方法调用
        proxy.registerUser("张三");
    }
}

/* 输出结果 */
// [前置通知]: 用户 张三 开始注册
// 用户 张三 注册成功
// [后置通知]: 用户 张三 完成注册
```

通过上面的例子，我们用静态代理实现了 AOP 的前置和后置通知。代理类只需要**实现同样的接口**，并**持有目标对象**，就可以在执行前后添加额外逻辑，实现基本的 AOP 效果。

在这里总结并扩展一下，简单来说，静态代理的核心可以拆成这几步：

1. 手动写一个代理类（是的，亲手写，纯手工）
2. 代理类实现和目标对象一样的接口
3. 持有目标对象的引用
4. 通过代理类的方法来**间接调用目标对象的方法**

这些步骤其实跟动态代理没有太大的区别，都是靠着实现同样的接口，持有目标对象，然后加逻辑。

唯一的区别就在于第一步：**静态代理的代理类是我们写死的，而动态代理的代理类，是 JVM 在运行时，通过反射 + 字节码生成技术帮我们写的。**

## 动态代理

关于动态代理，其**代理部分的核心思想我们已经在静态代理**介绍过，接下的重点是了解如何**动态生成代理对象。**

当我们编写一个 .java 文件时，它会被编译成 .class 文件，并通过类加载器加载到 JVM 中执行。而动态代理则是通过字节码生成技术，直接生成 .class 文件并加载到 JVM，而无需手动编写 .java 文件。

在 Java 的 `java.lang.reflect` 包下，提供了一个 `Proxy` 类。通过该类的静态方法，我们可以动态地生成代理类以及代理对象。

```java
// 获取与动态代理类对应的 Class 对象
static Class<?> getProxyClass(ClassLoader loader, Class<?>... interfaces)

// 创建动态代理对象
static Object newProxyInstance(
    ClassLoader loader,        // 类加载器
    Class<?>[] interfaces,     // 代理接口
    InvocationHandler handler  // 调用处理器
)
```

以上的两个静态方法的作用分别为：

* `getProxyClass`：生成一个动态代理类
* `newProxyInstance`：生成动态代理类并直接基于该代理类生成动态代理对象

我们的目标是生成一个代理对象，`newProxyInstance` 方法是一个一步到位的做法，而 `getProxyClass` 是一个中间步骤，最终还需要根据其生成的动态代理类来创建。

接下来我们再分析一下其参数：

首先，不管是 .java 文件编译后的 .class 文件还是字节码技术直接生成的 .class 文件，都需要通过类加载器加载到 JVM 中执行，所以这两个静态方法的参数里面有一个 ClassLoader（类加载器）。

其次，我们上文有提到，实现代理的 3 个核心步骤：

1. 代理类需要实现和目标对象一样的接口
2. 持有目标对象的引用
3. 通过代理类的方法来**间接调用目标对象的方法**

这两个静态方法的参数分别提供了

* Class\<?>... interfaces
* Class\<?>\[] interfaces

用于传入目标对象实现的接口，来帮助我们完成代理的第一个步骤，那剩下两个步骤怎么办呢？

`InvocationHandler` 接口的作用就在于完成这两步。`newProxyInstance` 方法为我们提供了这个承载这个接口的参数，而 `getProxyClass` 方法虽然没有直接提供，但它在创建代理对象时仍需要这个接口。

我们来看一下 `InvocationHandler` 接口的定义：

```java
public interface InvocationHandler {
    public Object invoke(Object proxy, Method method, Object[] args)
        throws Throwable;
}
```

目前，我们知道 `InvocationHandler` 主要负责实现代理的后两个步骤，但还没有完全弄清楚，它是如何完成这些步骤的。

接下来，考虑以下场景：根据第一个步骤，代理类需要实现与目标对象相同的接口。也就是说，**动态代理类必须实现接口中的所有方法**。那么问题来了，动态代理类是由 JVM 自动生成的，\*\*JVM 怎么知道这些接口的方法的实现细节呢？\*\*在这些接口方法内部，是需要打日志？还是需要发送网络请求？还是要读取数据库中的数据？\*\*很明显，JVM 并不知道，\*\*这些实现细节只有开发者才了解。

> 在静态代理的例子中，`UserServiceProxy` 代理类实现了 `UserService` 接口的 `registerUser()` 方法，我们在 `registerUser()` 中打印了一条前置通知和一条后置通知。

因此，JVM 需要提供一种机制，将接口方法的具体实现交给开发者处理。当 JVM 执行动态代理类的接口方法时，它会统一调用开发者提供的实现。

这个机制就是 `InvocationHandler` 提供的 `invoke()` 方法。开发者需要重写该方法，每当代理类的某个方法被调用时，`invoke()` 方法就会被执行，不管方法是什么，都会统一进入 `invoke()` 方法进行处理。

与此同时，`invoke()` 方法的参数也为实现代理类后两个步骤提供了基础。

我们现在来看 `invoke()` 方法的参数。

```java
public Object invoke(Object proxy, Method method, Object[] args)
        throws Throwable;
```

* `Object proxy`：当前的代理对象实例，用得很少
* `Method method`：\*\*被调用的接口方法对象，\*\*也就是我们通过代理调用的那个方法
* `Object[] args`：调用方法时传入的实参列表，顺序对应方法的参数

现在我们再来看一下，使用 Proxy 提供的这两个接口，是怎么模拟 AOP 的功能的。

### 使用 getProxyClass

需要代理的接口和实现类我们还是使用上文提到的 `UserService` 接口和 `UserServiceImpl`。

这里我们主要的创建一个日志代理：

日志代理内部有一个\*\* **<code>**Object target**</code>** 字段，\*\*通过这个字段持有目标对象。

```java
public class LogInvocationHandler implements InvocationHandler {
    private final Object target;

    public LogInvocationHandler(Object target) {
        this.target = target;
    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        System.out.println("[调用前]" + method.getName());
        Object result = method.invoke(target, args);
        System.out.println("[调用后]" + method.getName());
        return result;
    }
}
```

使用 `getProxyClass` 手动创建代理对象：

```java
public class Main {
    public static void main(String[] args) throws Exception {
        // 1. 目标对象
        UserService target = new UserServiceImpl();

        // 2. 获取代理类的 Class 对象
        Class<?> proxyClass = Proxy.getProxyClass(
            UserService.class.getClassLoader(),
            UserService.class
        );

        // 3. 获取代理类的构造函数
        Constructor<?> constructor = proxyClass.getConstructor(InvocationHandler.class);

        // 4. 创建 InvocationHandler 实例
        InvocationHandler handler = new LogInvocationHandler(target);

        // 5. 通过构造函数手动 new 出代理对象
        UserService proxyInstance = (UserService) constructor.newInstance(handler);

        // 6. 调用方法
        proxyInstance.registerUser("张三");
    }
}

// 执行结果
// [调用前]registerUser
// 注册用户：张三
// [调用后]registerUser
```

`getProxyClass` 相对于 `newProxyInstance` 来说更加底层，但是也能够一步步的让我们了解创建一个代理对象这后面的流程。

### 使用 newProxyInstance

使用 newProxyInstance 创建代理对象比 getProxyClass 更简洁，写法为：

```java
public class Main {
    public static void main(String[] args) throws Exception {
        // 1. 目标对象
        UserService target = new UserServiceImpl();

        // 2. 创建 InvocationHandler（可用你现有的 LogInvocationHandler）
        InvocationHandler handler = new LogInvocationHandler(target);

        // 3. 直接用 Proxy.newProxyInstance 生成代理对象
        UserService proxyInstance = (UserService) Proxy.newProxyInstance(
                UserService.class.getClassLoader(),   // 类加载器
                new Class<?>[]{UserService.class},    // 需要实现的接口
                handler                               // 调用处理器
        );

        // 4. 调用方法
        proxyInstance.registerUser("张三");
    }
}
```

## CGLIB

上文已经介绍完毕动态代理的概念了，本部分我们主要来介绍一下 cglib 创建动态代理对象的用法。

> 这部分其实是固定用法，也不好额外讲解什么。

通过 JDK 生成动态代理对象，只能通过接口完成，如果目标类没有接口的话，那只能使用 cglib 生成子类（通过继承目标类）来实现代理功能了。

> 既然是通过继承，那被 final 修饰的类就不能被 cglib 代理了。

cglib 是一个第三方的依赖，使用的话需要先引入依赖：

```xml
<dependencies>
  <dependency>
    <groupId>cglib</groupId>
    <artifactId>cglib</artifactId>
    <version>3.3.0</version>
  </dependency>
</dependencies>
```

以下是需要被代理的目标类：

```java
public class UserService {
    public String register(String name) {
        System.out.println("UserService.register 执行中...");
        return "OK: " + name;
    }

    public void ping() {
        System.out.println("pong");
    }
}
```

编辑拦截器：

```java
import net.sf.cglib.proxy.MethodInterceptor;
import net.sf.cglib.proxy.MethodProxy;

import java.lang.reflect.Method;

public class LogInterceptor implements MethodInterceptor {
    @Override
    public Object intercept(Object proxyObj, Method method, Object[] args, MethodProxy methodProxy) throws Throwable {
        long t0 = System.nanoTime();
        System.out.println("[LOG] before: " + method.getName());
        // 关键点：调用父类原始方法要用 invokeSuper，避免死递归
        Object ret = methodProxy.invokeSuper(proxyObj, args);
        System.out.println("[LOG] after: " + method.getName() + " cost=" + (System.nanoTime() - t0) + "ns");
        return ret;
    }
}
```

创建代理对象：

```java
import net.sf.cglib.proxy.Enhancer;

public class Main {
    public static void main(String[] args) {
        Enhancer enhancer = new Enhancer();
        enhancer.setSuperclass(UserService.class);      // 代理谁：指定父类
        enhancer.setCallback(new LogInterceptor());     // 怎么拦：指定回调

        // 如果目标类有无参构造，直接 create()
        UserService proxy = (UserService) enhancer.create();

        // 调用代理对象的方法
        String res = proxy.register("张三");
        System.out.println("返回值 = " + res);

        proxy.ping();
    }
}
```


> 更新: 2025-08-13 20:42:01  
> 原文: <https://www.yuque.com/chengxuyuancarl/fryvxd/ri4snnlh8h6rg98b>