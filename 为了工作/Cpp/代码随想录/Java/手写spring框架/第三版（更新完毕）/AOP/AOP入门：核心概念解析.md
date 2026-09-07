# AOP 入门：核心概念解析

## AOP 相关概念

AOP 是 Aspect-Oriented Programming（面向切面编程）的简称。

OOP（面向对象编程） 通过继承共享不同对象的共同行为，通过多态以相同的方式处理不同的对象。通常情况下，这种方式可以很大的程度上避免代码重复。但凡事总有例外，在一个复杂的系统中，总有 OOP 照顾不到的地方。

就以常见的权限校验问题来说，我们希望在执行一段代码之前，判断当前的用户有没有权限执行某段代码：

```java
public class MyBusinessObject implements BusinessObject {
    public void businessMethod1() throws UnauthorizedException {
        doPermissionCheck();
    }

    public void businessMethod2() throws UnauthorizedException {
        doPermissionCheck();
    }

    public void requiresNoPermissionCheck() {}

    protected void doPermissionCheck() throws UnauthorizedException {}
}
```

在示例中，我们已经将权限检查的具体代码进行了封装，之后在具体用到的地方进行调用，让重复的代码数量只有一行。

但是就是这一行代码：`doPermissionCheck();`

会分散在系统的各处，而且 OOP 也没有很好的方式，能够消除这些重复的代码。

AOP 以横切面的视角来审视这些重复的代码，最终提出一种新的编程方式来模块化这些本来分散于系统中的代码。

这里我们先来了解一下 AOP 中常见的各个概念（其实 AOP 本身的思想很简单，基本上就是拦截 + 增强，但是 AOP 又有一堆很麻烦的名词，能把这些名词区分清楚，AOP 基本没有什么难点的......）

**1、Advice：针对这个名词的中文翻译有“通知”、“增强”、“建议” 等**

> 对于其百花齐放的中文翻译不想做任何评价。。。。。。

这里我们采用“通知”这个翻译，并在后续所有的文档中统一采用这个翻译。

这个 Advice 其实就是上面提到的会**分散在系统各处的重复代码**，例如权限校验，安全性检查，日志打印等等实现代码。

不过 AOP 对其进行了扩展，按照执行时机不同，分为了以下几类：

* **前置通知（Before Advice）**：在目标方法执行前执行
* **后置通知（After Advice）**：无论目标方法是否抛出异常，执行方法后执行
* **返回通知（AfterReturning Advice）**：在目标方法正常返回结果后执行
* **异常通知（AfterThrowing Advice）**：在目标方法抛出异常时执行
* **环绕通知（Around Advice）**：可以控制目标方法在任意时机执行，并能访问方法的返回值、参数

***

**2、PointCut：切点**

在上文的 AOP 针对 Advice 扩展部分提出了一个新的概念——目标方法。\*\*目标方法就是需要应用那些重复代码的方法。\*\*比如，目标方法就是需要调用权限校验，进行安全性检查的方法。

而 PointCut 就是用来**定位**目标方法的。

这里举一个例子：

```java
@Aspect
@Component
public class LoggingAspect {
    // 前置通知：在目标方法执行前记录日志
    @Before("execution(* com.example.service.UserService.registerUser(..))")
    public void logBefore(JoinPoint joinPoint) {
        System.out.println("Before calling registerUser method.");
    }
}
```

这个是 Spring 中很常见的 AOP 的写法，其作用就是在 `UserService.registerUser()`方法执行之前，先执行 `logBefore()` 方法。

为了方便理解，我们可以简单的认为：

* `"execution(* com.example.service.UserService.registerUser(..))"` 就是 PointCut
* `@Before + logBefore()` 就是 Advice（Before Advice）

***

**3、JoinPoint：连接点**

我们现在来看这个被我们当作 PointCut 的表达式：

`execution(* com.example.service.UserService.registerUser(..))`

它的含义是：匹配 `com.example.service` 包下 `UserService` 类中，名为 `registerUser` 的方法，不论参数是什么、返回值是什么，只要方法名是 `registerUser`，都能被这个表达式匹配到。

再来看几个常见的变体：

* `execution(* com.example.service.*.*(..))`匹配 `com.example.service` 包下所有类的所有方法，参数不限
* `execution(public void com.example..*.save*(..))`匹配 `com.example` 包及其子包中，所有以 `save` 开头的方法，并且返回类型是 `void`、访问修饰符是 `public`
* `execution(* *..*Controller.*(..))`匹配项目中所有以 `Controller` 结尾的类中的所有方法

可以看出来了吗？PointCut 指的是**一批**符合条件的目标方法。这一批中的**任意一个就是 JoinPoint**。

总结一下：

* PointCut：一批目标方法
* JoinPoint：具体的一个目标方法

***

**4、Advisor：通知器**

Advisor 是 Advice 和 PointCut 的组合体（**Advisor = Advice + PointCut）**，如果按照 OOP 的方式来的话，就是下面的例子：

```java
public class Advisor {
    Advice advice;
    PointCut pointcut;
    public Advisor(Advice ad, PointCut cut) {
        this.advice = ad;
        this.pointcut = cut;
    }
}
```

***

**5、Aspect：切面**

这个切面就是 AOP 中的 A 字母，将 Aspect 和 Advisor 对照起来更好理解。

我们已经知道：Advisor = Advice + PointCut，这表示对什么方法，在什么地方，做什么通知，粒度比较细。

而 Aspect 更加关注与对一个切面完整的处理，比如打印日志切面，Aspect 会这样衡量：

1. 在目标方法执行前需要打印什么
2. 目标方法执行后需要打印什么
3. 目标方法发生异常后需要打印什么

可以这样说，Aspect 是由一组 Advisor 一起组成的

```java
Aspect（切面）
│
├── Advisor（前置通知）—— BeforeAdvice + Pointcut
├── Advisor（后置通知）—— AfterAdvice + Pointcut
├── Advisor（异常通知）—— AfterThrowingAdvice + Pointcut
```

***

**6、Weaving：织入**

至此，我们可以总结出 AOP 的核心概念为：**都通知谁（PointCut）**+ **怎么通知（Advice），最终通过 Aspect 封装这个一整个过程。这个过程被称之为织入（Weaving）。**


> 更新: 2025-08-06 18:55:57  
> 原文: <https://www.yuque.com/chengxuyuancarl/fryvxd/ohu8zyst2p8khbco>