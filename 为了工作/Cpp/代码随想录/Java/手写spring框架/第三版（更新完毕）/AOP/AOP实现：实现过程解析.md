# AOP 实现：实现过程解析

在理解了 AOP 的底层原理后，我们来看一下 Mini-Spring 的具体实现。相比 IoC 部分，AOP 这块的继承结构简单得多，层级也很浅。只要掌握了 AOP 的基础，再过一遍它的继承结构，就能发现它其实就是用代码把这些基础概念实现了一遍。

## 继承结构

### 通知（Advice）

> 标记接口就是告诉处理该类的框架或者系统，该类拥有特定的行为和特性

Advice 是标记接口，没有任何实现。

```java
public interface Advice {
}
```

Mini-Spring 当前实现通知体系：

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1755056461016-2b2f5b71-adc6-4e4d-84cf-0ab02985f669-103405.jpeg)

#### 后置通知 AfterAdvice

AfterAdvice 同样也是一个标记接口，没有任何实现。

```java
public interface AfterAdvice extends Advice {
}
```

#### 方法返回通知 AfterReturningAdvice

方法返回通知接口内拥有一个方法，这个方法在目标对象执行成功执行完毕后，会被调用。

```java
public interface AfterReturningAdvice extends AfterAdvice {
    /**
     * 在目标方法成功执行后被调用
     *
     * @param returnValue The value returned by the method, if any
     * @param method 被执行的方法
     * @param args 方法的参数
     * @param target 目标对象
     * @throws Throwable 可能抛出的异常
     */
    void afterReturning(Object returnValue, Method method, Object[] args, Object target) throws Throwable;
}
```

#### 前置通知 BeforeAdvice

同样是标记接口。

```java
public interface BeforeAdvice extends Advice {
}
```

#### 方法前置通知接口 MethodBeforeAdvice

拥有一个方法，这个方法会在目标对象执行前被调用。

```java
public interface MethodBeforeAdvice extends BeforeAdvice {
    /**
     * 在目标方法执行前被调用
     *
     * @param method 正在被调用的方法
     * @param args 方法的参数
     * @param target 目标对象
     * @throws Throwable 可能抛出的异常
     */
    void before(Method method, Object[] args, Object target) throws Throwable;
} 
```

### 切点（PointCut）

前面我们说过，切点是用于定位的，AOP 可以代理对象，可以代理方法，切点接口就用于集成代理对象的定位和代理方法的定位。

```java
public interface Pointcut {
    /**
     * 返回切点的类匹配器，用于确定哪些类应该被拦截
     *
     * @return 类匹配器
     */
    ClassFilter getClassFilter();

    /**
     * 返回切点的方法匹配器，用于确定类中哪些方法应该被拦截
     *
     * @return 方法匹配器
     */
    MethodMatcher getMethodMatcher();

    /**
     * 代表匹配所有的切点常量
     */
    Pointcut TRUE = TruePointcut.INSTANCE;
}
```

ClassFilter 是来匹配类的接口：

```java
/**
 * 类过滤器接口，用于确定哪些类应该被代理
 */
public interface ClassFilter {
    
    /**
     * 判断给定的类是否匹配
     * @param clazz 要检查的类
     * @return 如果类匹配则返回true
     */
    boolean matches(Class<?> clazz);
    
    /**
     * 默认的类过滤器，匹配所有类
     */
    ClassFilter TRUE = clazz -> true;
} 
```

MethodMatcher 是用来匹配方法的接口：

```java
public interface MethodMatcher {
    /**
     * 判断给定的方法是否匹配
     * @param method 要检查的方法
     * @param targetClass 目标类
     * @return 如果方法匹配则返回true
     */
    boolean matches(Method method, Class<?> targetClass);
    
    boolean isRuntime();
    
    boolean matches(Method method, Class<?> targetClass, Object... args);
    
    /**
     * 默认的方法匹配器，匹配所有方法
     */
    MethodMatcher TRUE = new MethodMatcher() {
        @Override
        public boolean matches(Method method, Class<?> targetClass) {
            return true;
        }
        
        @Override
        public boolean isRuntime() {
            return false;
        }
        
        @Override
        public boolean matches(Method method, Class<?> targetClass, Object... args) {
            return true;
        }
    };
} 
```

在 MethodMatcher 和 ClassFilter 内都有一个 TRUE 的已经实现的方法，这个方法用来表示，所有的类和方法都能够匹配得上，属于默认的实现。

### 通知器（Advisor）

Advisor 是通知（Advice）+切点（PointCut）的组合，不过在这里分为了两个接口 Advisor 和 PointcutAdvisor。

```java
public interface Advisor {
    
    /**
     * 返回此切面使用的通知
     * @return 通知对象
     */
    Advice getAdvice();
}

public interface PointcutAdvisor extends Advisor {
    
    /**
     * 返回此切面的切点
     * @return 切点对象
     */
    Pointcut getPointcut();
}
```

为什么要这样分？

因为**有些通知根本不需要做匹配**，直接对所有方法生效就行（比如全局异常处理），这种就只用 `Advisor` 就够了；\
而需要指定“在哪些方法生效”的，就得用 `PointcutAdvisor`，再配个 PointCut 去筛选目标方法。

### 代理（AopProxy）

`AopProxy` 接口是 AOP 实现的核心接口，主要作用就是**创建代理对象**。

```java
public interface AopProxy {
    /**
     * 创建一个新的代理对象
     * @return 代理对象
     */
    Object getProxy();
    
    /**
     * 使用给定的类加载器创建一个新的代理对象
     * @param classLoader 用于创建代理的类加载器
     * @return 代理对象
     */
    Object getProxy(ClassLoader classLoader);
}
```

两个方法都是用来生成代理对象的，区别只是第二个方法可以手动指定类加载器，而第一个使用默认的类加载器。\
在具体实现上，JDK 动态代理和 CGLIB 代理都会基于这个接口来创建代理对象。

## 创建代理对象流程分析

### 创建目标对象

以下步骤和我们创建动态代理的步骤类似，需要先准备好“要被代理的目标对象”，这里我们定义了一个接口 `ITestService` 和它的实现类 `TestService`。

```java
/**
 * 用于测试的服务接口
 */
public interface ITestService {
    String sayHello();
}

/**
 * 用于测试的服务实现
 */
static class TestService implements ITestService {
    @Override
    public String sayHello() {
        return "TestService.sayHello()";
    }
}
```

接下来就是创建代理的过程：

```java
public void testAopProxy() throws Exception {
    // 1. 创建目标对象
    TestService target = new TestService();
    
    // 2. 创建切点
    AspectJExpressionPointcut pointcut = new AspectJExpressionPointcut();
    pointcut.setExpression("execution(* com.minispring.test.AopTest$ITestService.*(..))");
    
    // 3. 创建通知
    TestBeforeAdvice beforeAdvice = new TestBeforeAdvice();
    
    // 4. 创建Advisor（切点和通知的组合）
    DefaultPointcutAdvisor advisor = new DefaultPointcutAdvisor(pointcut, beforeAdvice);
    
    // 5. 使用代理工厂创建代理
    ProxyFactory proxyFactory = new ProxyFactory(target);
    proxyFactory.addAdvisor(advisor);
    ITestService proxy = (ITestService) proxyFactory.getProxy();
    
    // 6. 调用代理方法
    proxy.sayHello();
}
```

总共有六步。

1. 创建目标对象
2. 创建切点
3. 创建通知
4. 创建通知器
5. 使用代理工厂创建代理
6. 调用代理方法

### 创建 PointCut

第一步不用过多的描述了，接着来看第二步：`AspectJExpressionPointcut` 类到底是干啥的：

```java
public class AspectJExpressionPointcut 
             implements Pointcut, ClassFilter, MethodMatcher {
    // 设置切点表达式，并解析它
    public void setExpression(String expression) {
        this.expression = expression;
        buildPointcutExpression();
    }

    // 使用第三方库解析表达式
    private void buildPointcutExpression() {
        if (this.expression == null) {
            throw new IllegalStateException("Expression must not be null");
        }
        
        PointcutParser parser = PointcutParser.getPointcutParserSupportingSpecifiedPrimitivesAndUsingSpecifiedClassLoaderForResolution(
                SUPPORTED_PRIMITIVES, this.pointcutClassLoader);
        this.pointcutExpression = parser.parsePointcutExpression(this.expression);
    }

    // 用解析后的表达式检查类是否匹配
    @Override
    public boolean matches(Class<?> clazz) {
        checkReadyToMatch();
        return this.pointcutExpression.couldMatchJoinPointsInType(clazz);
    }
}
```

`AspectJExpressionPointcut` 里面有很多方法，这里挑三个最核心的来看。

**第一个**：`setExpression()` 方法

它用来设置切点表达式 `execution(...)`，并且在内部调用 `buildPointcutExpression()` 解析表达式。

**第二个**：`buildPointcutExpression()` 方法

这里通过第三方库对切点表达式进行解析，生成 `pointcutExpression` 对象。之后，就可以用它来实现 `MethodMatcher` 和 `ClassFilter` 中定义的匹配方法。

使用的第三方库是这个：

```xml
<dependency>
    <groupId>org.aspectj</groupId>
    <artifactId>aspectjweaver</artifactId>
    <version>1.9.9.1</version>
</dependency>
```

**第三个**：`matches()` 方法

关键在于展示 `pointcutExpression` 是如何工作的。这个方法会用 `pointcutExpression` 来判断传入的类是否符合切点表达式的规则。

### 创建 Advice

第三步是创建通知部分，这个通知是我们手动写的，用来在匹配到切点时执行的逻辑。

```java
// 3. 创建通知
// TestBeforeAdvice beforeAdvice = new TestBeforeAdvice();
class TestBeforeAdvice implements MethodBeforeAdvice {
    private int counter = 0;

    @Override
    public void before(Method method, Object[] args, Object target) throws Throwable {
        counter++;
        System.out.println("TestBeforeAdvice.before called, counter = " + counter);
        System.out.println("Before method [" + method.getName() + "] on target [" + target.getClass().getName() + "]");
    }
    
    public int getCounter() {
        return counter;
    }
}
```

这里的 `TestBeforeAdvice` 是一个简单的前置通知，继承了 `MethodBeforeAdvice` 接口。每次方法执行之前，`before()` 方法会被调用，它会打印当前方法的名字、目标对象的类名，还会统计调用的次数。

### 创建 Advisor

第四步是创建通知器，通知器的作用是将 `advice` 和 `pointcut` 封装在一起，提供 `getter` 和 `setter` 方法。

```java
public class DefaultPointcutAdvisor implements PointcutAdvisor {
    
    private Advice advice;
    private Pointcut pointcut = Pointcut.TRUE;
    
    public DefaultPointcutAdvisor(Pointcut pointcut, Advice advice) {
        this.pointcut = pointcut;
        this.advice = advice;
    }
    
    public void setAdvice(Advice advice) {
        this.advice = advice;
    }
    
    public void setPointcut(Pointcut pointcut) {
        this.pointcut = (pointcut != null ? pointcut : Pointcut.TRUE);
    }
    
    @Override
    public Advice getAdvice() {
        return this.advice;
    }
    
    @Override
    public Pointcut getPointcut() {
        return this.pointcut;
    }
}
```

### 创建 ProxyFactory

第五步是核心步骤——创建 `ProxyFactory`。首先，我们创建代理工厂并传入目标对象 `target`：

```java
ProxyFactory proxyFactory = new ProxyFactory(target);
proxyFactory.addAdvisor(advisor);
ITestService proxy = (ITestService) proxyFactory.getProxy();
```

我们前面说过，代理对象需要持有目标对象的引用，所以我们将 `TestService` 对象作为参数传给 `ProxyFactory`。接着，调用 `addAdvisor` 添加通知器（可以添加多个）。最后，通过 `getProxy()` 方法获取代理对象。

接下来看 ProxyFactory 的内部实现：

```java
public class ProxyFactory extends AdvisedSupport {
    
    public ProxyFactory(Object target) {
        setTargetSource(new SingletonTargetSource(target));
    }
    
    public Object getProxy() {
        return createAopProxy().getProxy();
    }
    
    protected AopProxy createAopProxy() {
        if (getTargetClass().getInterfaces().length > 0) {
            return new JdkDynamicAopProxy(this);  // 使用 JDK 动态代理
        }
        return new CglibAopProxy(this);  // 使用 CGLIB 代理
    }
    
    public void addAdvisor(Advisor advisor) {
        super.addAdvisor(advisor);  // 调用父类方法，添加通知器
    }
}
```

构造函数将目标对象通过 `SingletonTargetSource` 保存。

然后，`getProxy()` 方法调用 `createAopProxy()` 创建代理对象：

* 如果目标类实现了接口，使用 JDK 动态代理
* 如果目标类没有实现任何接口，使用 CGLIB 代理

`addAdvisor` 方法会把通知器添加到父类 `AdvisedSupport` 的通知器列表中。

接下来，我们看 `AdvisedSupport`，它的作用是存储目标对象和通知器：

```java
public class AdvisedSupport {
    private TargetSource targetSource;  // 目标对象源
    private List<Advisor> advisors = new ArrayList<>();  // 通知器列表
}
```

再来看 `JdkDynamicAopProxy` 的实现，它实现了 `AopProxy` 和 `InvocationHandler` 接口，负责创建代理对象并处理方法调用，就像我们的动态代理章节说过那样。

```java
public class JdkDynamicAopProxy implements AopProxy, InvocationHandler {
    
    private final AdvisedSupport advised;
    
    public JdkDynamicAopProxy(AdvisedSupport advised) {
        this.advised = advised;
    }

    @Override
    public Object getProxy() {
        return getProxy(Thread.currentThread().getContextClassLoader());
    }

    @Override
    public Object getProxy(ClassLoader classLoader) {
        if (this.advised.getTargetSource() == null) {
            throw new IllegalStateException("TargetSource cannot be null when creating a proxy");
        }
        Class<?> targetClass = this.advised.getTargetSource().getTargetClass();
        
        Class<?>[] interfaces = targetClass.getInterfaces();
        if (interfaces.length == 0) {
            throw new IllegalStateException("Target class '" + targetClass.getName() +
                    "' does not implement any interfaces, cannot create JDK proxy");
        }
        
        return Proxy.newProxyInstance(classLoader, interfaces, this);
    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        // ...
    }
    
    private Object processInterceptors(List<Object> chain, ReflectiveMethodInvocation invocation) throws Throwable {
        // ...
    }

    private static class AopMethodInvocation extends ReflectiveMethodInvocation {
        //...
    }
}

```

`JdkDynamicAopProxy` 的构造函数用 `AdvisedSupport` 作为参数，也就是说 `JdkDynamicAopProxy`持有目标对象以及我们添加的所有通知器。

`getProxy()` 方法的实现与上一章类似，通过 Proxy 的 `newProxyInstance()` 方法创建。

我们知道，代理对象执行任何方法，其实都是在执行重写的 InvocationHandler 接口的 invoke 方法，接下来我们看 invoke 方法。

```java
public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
    Object target = null; 
    try {
        // 1. 拿到目标对象
        target = this.advised.getTargetSource().getTarget();
        if (target == null) {
            throw new IllegalStateException("Target is null");
        }
        
        // 2. 通过目标对象的反射获取目标类声明的方法，如果没有找到，就找默认使用接口提供的方法
        Method targetMethod = null;
        try {
            targetMethod = target.getClass().getMethod(method.getName(), method.getParameterTypes());
        } catch (NoSuchMethodException e) {
            targetMethod = method;
        }

        // 3. 获取方法对应的拦截器链
        List<Object> chain = this.advised.getInterceptorsAndDynamicInterceptionAdvice(method, this.advised.getTargetClass());

        // 4. 如果没有拦截器，直接调用目标方法
        if (chain.isEmpty()) {
            return method.invoke(target, args);
        }

        // 5. 创建方法调用
        ReflectiveMethodInvocation invocation = new ReflectiveMethodInvocation(target, targetMethod, args);

        // 6. 处理拦截器链
        return processInterceptors(chain, invocation);
    } finally {
        if (target != null) {
            this.advised.getTargetSource().releaseTarget(target);
        }
    }
}
```

这里我们关注代码中的第 3 点：获取方法对应的拦截器链，也就是在执行当前代理对象时需要执行的 AOP 方法。`getInterceptorsAndDynamicInterceptionAdvice` 方法是通过 `advised` 调用的，实际上它是在 `AdvisedSupport` 中实现的。

接下来，我们来看它的具体实现：

```java
public List<Object> getInterceptorsAndDynamicInterceptionAdvice(Method method, Class<?> targetClass) {
    // 检查缓存中是否已有计算过的结果
    List<Object> cached = this.methodCache.get(method);
    if (cached != null) {
        return cached;
    }
    // 计算适用于此方法的通知
    List<Object> interceptors = new ArrayList<>();

    // 首先遍历所有的 advisor
    for (Advisor advisor : this.advisors) {
        // 如果 advisor 是拥有 pointcut 的，需要判断 advisor 是否匹配当前传入的 method
        if (advisor instanceof PointcutAdvisor) {
            PointcutAdvisor pointcutAdvisor = (PointcutAdvisor) advisor;
            // 检查切点是否匹配类
            boolean classMatches = pointcutAdvisor.getPointcut().getClassFilter().matches(targetClass);                
            // 对于实现了接口的类，也检查是否匹配接口
            if (!classMatches && targetClass.getInterfaces().length > 0) {
                for (Class<?> iface : targetClass.getInterfaces()) {
                    if (pointcutAdvisor.getPointcut().getClassFilter().matches(iface)) {
                        classMatches = true;
                        break;
                    }
                }
            }
            // 检查切点是否匹配方法
            boolean methodMatches = false;
            if (classMatches) {
                methodMatches = pointcutAdvisor.getPointcut().getMethodMatcher().matches(method, targetClass);
                // 如果方法不匹配，尝试在接口上查找对应的方法并匹配
                if (!methodMatches && targetClass.getInterfaces().length > 0) {
                    for (Class<?> iface : targetClass.getInterfaces()) {
                        try {
                            Method ifaceMethod = iface.getMethod(method.getName(), method.getParameterTypes());
                            methodMatches = pointcutAdvisor.getPointcut().getMethodMatcher().matches(ifaceMethod, iface);
                            if (methodMatches) {
                                break;
                            }
                        } catch (NoSuchMethodException ex) {
                            // 接口上没有这个方法，继续下一个接口
                            System.out.println("AdvisedSupport: 接口 " + iface.getName() + " 上没有方法 " + method.getName());
                        }
                    }
                }
            }
            
            // 如果匹配，添加通知
            if ((classMatches && methodMatches)) {
                interceptors.add(advisor.getAdvice());
            }
        } else {
            // 如果不是 PointcutAdvisor，表示该 advisor 属于匹配所有的情况，直接加入到拦截器中
            interceptors.add(advisor.getAdvice());
        }
    }
    // 缓存结果
    this.methodCache.put(method, interceptors);
    return interceptors;
}
```

这个方法主要的目的就是，当前的 advisors 里面存放的所有 advisor，有几个适合当前代理对象的。如果适合，就将它加入到 interceptors 中，然后返回。

接下来我们看第五步的 ReflectiveMethodInvocation 是什么？

查看其定义：

```java
public class ReflectiveMethodInvocation implements MethodInvocation {
    // 目标对象
    protected final Object target;
    // 目标方法
    protected final Method method;
    // 方法参数
    protected final Object[] arguments;
    
    public ReflectiveMethodInvocation(Object target, Method method, Object[] arguments) {
        this.target = target;
        this.method = method;
        this.arguments = arguments;
    }
    
    @Override
    public Object proceed() throws Throwable {
        return invokeJoinPoint();
    }
    
    protected Object invokeJoinPoint() throws Throwable {
        if (!method.isAccessible()) {
            method.setAccessible(true);
        }
        return method.invoke(target, arguments);
    }
}
```

`ReflectiveMethodInvocation` 就是封装了 `method.invoke(target, arguments)` 这行代码，用来通过反射调用目标对象的方法。\
现在我们已经拿到了代理对象需要执行的所有方法，接下来就是一个一个处理这些方法。其实，这些方法就是不同的 `advice`，我们会根据 `advice` 的类型来分别执行它们。

来看最终的 processInterceptors 方法：

```java
private Object processInterceptors(List<Object> chain, ReflectiveMethodInvocation invocation) throws Throwable {
    // 创建一个包含拦截器链的方法调用对象
    AopMethodInvocation methodInvocation = new AopMethodInvocation(invocation, chain);
    // 执行方法调用，这将依次执行所有拦截器和目标方法
    return methodInvocation.proceed();
}

private static class AopMethodInvocation extends ReflectiveMethodInvocation {
    private final List<Object> interceptorsAndAdvices;
    private int currentInterceptorIndex = -1;

    public AopMethodInvocation(ReflectiveMethodInvocation invocation, List<Object> interceptorsAndAdvices) {
        super(invocation.getThis(), invocation.getMethod(), invocation.getArguments());
        this.interceptorsAndAdvices = interceptorsAndAdvices;
    }

    @Override
    public Object proceed() throws Throwable {
        // 所有拦截器已执行完毕，调用目标方法
        if (this.currentInterceptorIndex == this.interceptorsAndAdvices.size() - 1) {
            System.out.println("Invoking target method directly");
            return super.proceed();
        }

        // 获取下一个拦截器
        Object interceptorOrAdvice = this.interceptorsAndAdvices.get(++this.currentInterceptorIndex);
        System.out.println("Processing interceptor: " + interceptorOrAdvice.getClass().getName());

        // 处理不同类型的通知，目前仅实现前置通知和后置通知
        if (interceptorOrAdvice instanceof MethodBeforeAdvice) {
            MethodBeforeAdvice beforeAdvice = (MethodBeforeAdvice) interceptorOrAdvice;
            beforeAdvice.before(getMethod(), getArguments(), getThis());
            return proceed();
        } else if (interceptorOrAdvice instanceof AfterReturningAdvice) {
            Object returnValue = proceed();
            AfterReturningAdvice afterAdvice = (AfterReturningAdvice) interceptorOrAdvice;
            afterAdvice.afterReturning(returnValue, getMethod(), getArguments(), getThis());
            return returnValue;
        } else {
            throw new IllegalStateException("Unknown advice type: " + interceptorOrAdvice.getClass());
        }
    }
}
```

`processInterceptors` 会创建一个 `AopMethodInvocation` 对象，然后调用它的 `proceed()` 方法，实际上 AOP 的逻辑就是在 `proceed()` 方法里执行的。

在这个方法里，首先会判断 `interceptors` 里存放的 `advice` 是哪种类型。

* 如果是 `MethodBeforeAdvice`，会先执行目标方法之前的逻辑，然后再继续调用 `proceed()`
* 如果是 `AfterReturningAdvice`，会等目标方法执行完后，再执行 `afterReturning` 逻辑

代码执行的顺序完全是按照`Advice` 定义的顺序来执行的。

到这里，AOP 原理部分就已经完结了。


> 更新: 2025-08-29 10:47:05  
> 原文: <https://www.yuque.com/chengxuyuancarl/fryvxd/vytpz6i6m2o58al2>