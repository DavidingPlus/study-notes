# IoC 容器进阶：接口设计与继承结构

> 本部分才算是真正的 Mini-Spring 的教程部分，从本篇以后，所涉及到的接口、代码实现，如果没有特殊说明，
>
> 那么都是出自于 Mini-Spring，和 Spring 在具体的实现上会有所差异。

## IoC 容器的设计目标与实现路径

在学习 Mini-Spring 的接口设计体系（例如 `BeanFactory` 和 `ApplicationContext`）前，我们不妨站在设计者的视角考虑以下问题：

> 我该怎样设计一个容器，让它既能解耦对象之间的依赖关系，又便于使用和扩展

这些问题，正是 IoC 容器接口设计的出发点。它的核心思想是将**对象的创建**与**依赖的管理**从业务代码中剥离出来，交由容器统一控制——也就是我们熟知的**控制反转（IoC）与依赖注入（DI）**。

理解理念容易，真正实现却异常复杂。

即使是简化版的 Spring（Mini-Spring），它在接口和类的继承体系上依然十分复杂，最深的继承层级也有十层往上。如果没有一条清晰的主线，很容易被那些满屏的 `xxxFactory`、`xxxContext`、`xxxReader` 绕晕。

所以我在这里引入一条主线，**在分析各种接口的设计之前，一定要先记住这条主线。**

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753845568459-5f871d23-241f-4cbe-96a9-dc1dcff37fef-625987.jpeg)

这条主线描述了 Mini-Spring **是如何将一个 XML 配置文件，最终转化为我们可以直接使用的 Java 对象的。**

**简单描述一下这个过程：**

1. Mini-Spring 首先加载我们编写的 XML 配置文件，随后将 XML 内容转化为 DOM 树，方便程序访问和处理
2. 通过遍历 DOM，读取 `<bean>` 等标签，构建对应的 `BeanDefinition`，也就是对象的“蓝图”
3. 容器根据这些定义，通过反射等方式创建实际的对象
4. 容器向外提供这些对象，供业务代码使用

***

> 后续几章在分析这些过程实际代码，或者你自己看 Mini-Spring 的实现代码时，很可能会陷入无数实现细节的“泥沼”，会有一些第三方库的看起来十分麻烦的固定写法、反射创建的细节、一些工厂类的层层委托。这时候千万别急着钻进去抠每一行代码。
>
> **最好的学习方式，是先把整体的流程跑通。**
>
> 当你理解了“从 XML 到对象”的宏观路径之后，那些复杂的类和方法会自己慢慢对齐。
>
> 至于暂时看不懂、逻辑过于繁琐的部分，完全可以**先当作黑盒**，记住它“做了什么”，不必强求立即理解“怎么做的”。
>
> 学习 Mini-Spring 的核心**不是一次性看懂所有细节**，而是抓住**主线逻辑**，并在反复不断的重复主线逻辑时，让每个黑盒慢慢“变透明”。

## IoC 容器接口设计分析

借鉴于 Spring 所采用的“职责分离、接口解耦、按需组合”的优秀设计方式，Mini-Spring 对 IoC 容器需要支持的功能进行细化，设计出**多个职责分离的接口体系**。每个接口体系**按照功能逐步扩展**，**最终再组合**在一起，形成完整的容器结构。

以下是具体功能划分，每一个功能最终都会有相应的接口体系与之对应。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753847466750-b60b321c-05b9-4f1d-95ca-202d2ac02314-425141.jpeg)

在 IoC 容器功能这里，比较值得一说的是第一点：**容器的构造逻辑**

相比于其他 IoC 容器的功能，**容器的构造逻辑又出现了两个分支**，这两个分支都是 `BeanFactory`出发，逐渐的发展出来的，它们分别为：

* 容器的运行机制（Factory 体系）
* 容器的应用场景（Context 体系）

我们在使用 Spring 创建容器的时候，最常见的有两种方式：

第一种：通过 Factory 创建容器

```java
public class Main {
    public static void main(String[] args) {
        // 创建 factory
        DefaultListableBeanFactory factory = new DefaultListableBeanFactory();

        // 创建 reader
        XmlBeanDefinitionReader xmlBeanDefinitionReader 
                = new XmlBeanDefinitionReader(factory);

        // 读取配置
        xmlBeanDefinitionReader.loadBeanDefinitions("classpath:beans.xml");

        // 获取和使用 bean
        UserService userService = (UserService) factory.getBean("userService");
        userService.doSomething();
    }
}
```

第二种：通过 Context 创建容器

```java
public class Main {
    public static void main(String[] args) {
        // 创建 context 容器
        ApplicationContext applicationContext
                = new ClassPathXmlApplicationContext("classpath:beans.xml");
        // 获取 bean
        UserService userService = (UserService) applicationContext.getBean("userService");
    }
}
```

分成两个体系的原因不外乎是向着**职责分离、可扩展、可组合**的目标去的，重要的是理解它们之间的区别。

两个体系的主要区别在于它们各自所面向的使用场景：

**1、Factory 系（以 **<code>**BeanFactory**</code>** 为核心）**

* 提供容器底层的核心能力，比如对象创建、依赖注入、作用域控制等
* 这些功能更偏底层，主要是为**框架开发者**服务，是 IoC 容器的基础架构

**2、Context 系（以 **<code>**ApplicationContext**</code>** 为核心）**

* 在 Factory 系的基础上向上扩展，提供更多的应用层功能，如事件发布、国际化、资源加载等
* 更贴近**业务开发者**的使用场景

从我们前面创建容器的流程也可以看出：**Context 系的容器创建更简单、更直接**。

### 容器构造逻辑（Factory 体系）

#### IoC 容器的起点 —— BeanFactory 接口

在 Mini-Spring 中，IoC 容器的起点是一个叫做 `BeanFactory` 的接口。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753846377962-712741e2-d845-4289-a915-11b0f7f97c04-822312.jpeg)

它的核心作用就一个——**根据名字，返回对象**

```java
Object getBean(String name);
```

**这就是最本质的 IoC 的思想**。

前几章说来说去一堆概念，什么控制反转，依赖注入，这些难以理解的概念，其实都是为了实现一个最终的目标：**以后都不要自己创建对象，而是直接问容器要。**

所以现在你去看 `BeanFactory` 的定义，是不是就清晰多了，**都是 getBean 及其变体**。

#### IoC 容器的分层设计 —— HierarchicalBeanFactory 接口

上面我们说到了 `BeanFactory` 接口，现在来看它的一个的子接口——`HierarchicalBeanFactory`。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753775595674-3948c4e7-0a4d-4fc6-9f0c-c2d1e2c74102-057767.jpeg)

这个接口的核心方法是：

```java
BeanFactory getParentBeanFactory();
```

这个接口设计其实是为了解决一个问题:如果我在这个 IoC 容器中没有查找我想要的对象，那这个容器能不能拥有上一级的容器，然后向上查找对象。

这个接口给 IoC 容器提供了可以有父子容器的能力。

#### IoC 容器的查询能力扩展 —— ListableBeanFactory 接口

同上，在 `BeanFactory` 基础上，`ListableBeanFactory` 接口给 IoC 容器扩展了更加强力的查询 Bean 的能力，**这个接口是个很重要的接口，是很多基础框架运行的基础**。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753777469018-e53fe05f-25c8-47cb-9867-67a3a88f6ade-779810.jpeg)

`ListableBeanFactory` 接口的核心方法如下：

```java
<T> Map<String, T> getBeansOfType(Class<T> type);
```

我们现在来看这个方法有什么应用场景。

以一个简单的 Web 开发的需求为例，我们需要注册多个拦截器。

每个拦截器都需要实现 Spring 提供的 `HandlerInterceptor` 接口：

```java
public class AuthInterceptor implements HandlerInterceptor { ... }
public class LogInterceptor implements HandlerInterceptor { ... }
public class MetricsInterceptor implements HandlerInterceptor { ... }
```

如果我们希望在系统初始化时**统一获取所有拦截器进行注册**，该怎么办？

`BeanFactory` 提供的接口：

```java
Object getBean(String name);
```

这个时候就有些不够用了，所以有了 `ListableBeanFactory` 接口，这个接口提供了按照**类型过滤 Bean 并批量获取的能力。**

#### IoC 容器的注入执行器 —— AutowireCapableBeanFactory 接口

在上文中，我们介绍了三个接口：

1. `BeanFactory`：提供通过 name 获取 Bean 的能力
2. `HierarchicalBeanFactory`：为 IoC 容器提供了分层的能力
3. `ListableBeanFactory`：为 IoC 容器提供了批量查询 Bean 能力

但是，IoC 容器不止提供配置和查询功能，它需要真正的将对象创建出来，并且为创建出来的对象完成注入和初始化操作。

`AutowireCapableBeanFactory` 接口将会正式为 IoC 赋予创建对象的能力。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753780136926-4a39ce2b-d07a-4b37-9139-76a6ef8e2fe1-429949.jpeg)

`AutowireCapableBeanFactory` 接口提供的核心接口如下：

```java
// 根据类型直接创建一个 Bean（并完成依赖注入）
Object createBean(Class<?> beanClass);
// 对已有对象执行依赖注入（不负责创建）
void autowireBean(Object existingBean);
```

#### IoC 容器的配置中心 —— ConfigurableBeanFactory 接口

IoC 容器在支持最基本容器功能——获取和创建 Bean 后。就需要支持扩展高级的功能了，例如支持作用域，未来需要实现的 aop 模块。

`ConfigurableBeanFactory` 就是 IoC 容器支持配置这些高级功能的接口。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753786545316-7b898a6e-050b-4eda-bb79-1c9d0750652e-871492.jpeg)

以下是核心的方法：

```java
// 注册作用域
void registerScope(String scopeName, Scope scope);
// 添加 BeanPostProcessor
void addBeanPostProcessor(BeanPostProcessor beanPostProcessor);
```

`BeanPostProcess` 会在 AOP 模块处理再解释。

#### IoC 容器的总控接口 —— ConfigurableListableBeanFactory 接口

在 BeanFactory 这一继承体系中，目前设计完毕的接口如下：

1. BeanFactory：最基础的 Bean 获取功能
2. AutowireCapableBeanFactory：支持创建对象以及依赖注入
3. HierarchicalBeanFactory： 支持父子容器结构
4. ListableBeanFactory：支持按类型批量查找 Bean
5. ConfigurableBeanFactory： 支持 Bean 生命周期管理、作用域等配置能力

这些接口各自独立，关注点明确。只需要通过一个总控接口将其整合后，就能得到一个功能完整的 IoC 容器接口。

`ConfigurableListableBeanFactory` 接口就是这个总控接口。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753787214102-64ea4205-8570-4e30-a5b0-192758109c37-895790.jpeg)

`ConfigurableListableBeanFactory` 除了整合上述的接口之外，其自身对 IoC 容器的功能进行了拓展。

```java
// 获取某个 Bean 的注册信息
BeanDefinition getBeanDefinition(String beanName);
// 主动创建所有单例 Bean（触发依赖注入）
void preInstantiateSingletons() 
```

### 单例对象的管理

**什么是单例对象？为什么 IoC 容器需要专门拆分一个功能模块来管理它呢？**

在 IoC 容器中，所有被管理的 Bean 都有自己的生命周期 ，默认的两种的作用域：

1. **原型（Prototype）**：每次从容器中获取 Bean 时，都会创建一个新的实例
2. **单例（Singleton）**：容器在启动时就会创建该 Bean 的唯一实例，并将其缓存到一个内部的 Map 中。之后每次获取，都是从这个 Map 中直接返回同一个对象

#### 单例对象的注册和缓存 —— SingletonBeanRegistry 接口

前文我们说到，Mini-Spring 在设计上采用了 Spring 优秀的设计思路，细化**多个职责分离的接口体系，每个接口体系按照功能逐步扩展，最终再组合在一起，形成完整的容器结构。**

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753856413331-b394fb32-5e83-40c8-9d65-06080972b08b-165694.jpeg)

`SingletonBeanRegistry` 接口则是将**单例对象的缓存与管理**从容器本身中解耦出来，它提供以下两个核心方法：

```java
// 获取单例 Bean
Object getSingleton(String beanName);
// 注册单例 Bean
void registerSingleton(String beanName, Object singletonObject);
```

#### 单例对象的注册与缓存的核心实现 —— DefaultSingletonBeanRegistry 实现类

前面我们已经看过了 `SingletonBeanRegistry` 接口，它定义了 IoC 容器中最核心的能力之一：**单例对象的注册与获取**。

接下来，我们终于要进入第一个真正落地的实现类 —— `DefaultSingletonBeanRegistry`。

这个类不仅实现了 `SingletonBeanRegistry` 接口，还内置了 IoC 容器中十分关键的 **三级缓存机制（用于解决循环依赖问题的机制）。**

后续我们还会讲解的，IoC 容器真正的集大成者（`DefaultListableBeanFactory` 实现类）的拥有的**单例对象管理能力，也是从这里继承去的。**

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753858539173-496d3a52-0d5e-43cc-bd81-f9b40847a3d2-866562.jpeg)

我们先来简单看一下 `DefaultSingletonBeanRegistry`实现类的**核心属性**：

```java
public class DefaultSingletonBeanRegistry implements SingletonBeanRegistry {
    /** 一级缓存：完全初始化好的单例对象缓存 */
    private final Map<String, Object> singletonObjects = new ConcurrentHashMap<>(256);
    
    /** 二级缓存：提前曝光的单例对象（未完全初始化）缓存 */
    private final Map<String, Object> earlySingletonObjects = new HashMap<>(16);
    
    /** 三级缓存：单例工厂缓存，用于保存bean创建工厂，以便后面利用工厂为bean创建代理对象 */
    private final Map<String, ObjectFactory<?>> singletonFactories = new HashMap<>(16);
    
    /** 正在创建中的单例Bean名称集合 */
    private final Set<String> singletonsCurrentlyInCreation = ConcurrentHashMap.newKeySet();
}
```

这里看 `singletonObjects` 属性，是一个 String -> Object 的哈希表，这个 `singletonObjects` 就是 IoC 容器真正的用来存储已经创建了 Bean 的容器。

我们前面通过以下代码获取创建好的 Bean（默认都是单例）：

```java
UserService userService = (UserService) applicationContext.getBean("userService");
```

`getBean`操作最终就是从 `singletonObjects` 这里获取的 Bean。

#### 连接 BeanFactory 和 SingletonBeanRegistry 的调度中心 —— AbstractBeanFactory 实现类

上文我们提到过，在 Mini-Spring 的设计中，`SingletonBeanRegistry` 是一个独立的接口，它将**单例对象的缓存和管理**从容器内抽离出来，并通过 `DefaultSingletonBeanRegistry` 这一实现类独立的完成了这项功能。

需要注意的是，SingletonBeanRegistry 接口并不隶属于 BeanFactory，也就是说：

`BeanFactory` 提供的是“容器的依赖查找能力”，而 `SingletonBeanRegistry` 提供的是“对象实例的缓存能力”，这两个能力在设计上是**两条平行的分支。**

因此，我们需要一个媒介，将这两种能力组合起来：

`AbstractBeanFactory` 就充当了这个媒介。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753858667891-142f32da-fbc6-4dba-aa47-da4eb6c72348-176566.jpeg)

一方面，`AbstractBeanFactory` 继承了`DefaultSingletonBeanRegistry` 的单例管理能力，另一方面，它还实现了 BeanFactory 接口，拥有了 IoC 容器初级的查找对象的能力。

#### 唯一会造对象的类 —— AbstractAutowireCapableBeanFactory 实现类

`AbstractAutowireCapableBeanFactory` 实现类是 Mini-Spring 中真正用来创建对象的类。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753861740504-90336f0a-8b74-4fab-b639-7bb76ec32102-901078.jpeg)

> 你可能会觉得奇怪：这个类都叫 `AutowireCapableBeanFactory` 了，为什么它没有实现 `AutowireCapableBeanFactory` 接口？
>
> 答：**接口与实现解耦是 Spring 一贯的设计哲学。**
>
> * `AbstractAutowireCapableBeanFactory` 负责“实现自动注入的逻辑”；
> * `AutowireCapableBeanFactory` 是对外暴露“我支持自动注入能力”的声明；
> * 真正实现这个接口、并将该能力暴露给容器使用者的是 `DefaultListableBeanFactory`。
>
> 这种设计保证了内部实现逻辑的灵活性，接口职责的明确性。

### 注册与维护 BeanDefinition

和 `SingletonBeanRegistry` 将单例对象的缓存和管理能力从 IoC 容器内解耦出来类似，

`BeanDefinitionRegistry`也将 BeanDefinition 的注册和获取能力从 IoC 容器内解耦出来了。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753862294867-8838bc59-c4a6-4fed-9612-c21986759794-707332.jpeg)

以下是 `BeanDefinitionRegistry`接口的核心方法：

```java
// 注册BeanDefinition
void registerBeanDefinition(String beanName, BeanDefinition beanDefinition);
// 获取BeanDefinition
BeanDefinition getBeanDefinition(String beanName);
```

### DefaultListableBeanFactory

在正式介绍 `DefaultListableBeanFactory` 之前，我们先来以下的继承结构图：

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753864528749-3c06ff6c-d933-485b-9a20-0ae78c6b8ed7-316918.jpeg)

光是看 `DefaultListableBeanFactory` 在继承结构图上的位置，就大概知道这个类就是 IoC 容器的最终形态了，我们前面提到的所有接口，所有的实现类拥有的所有能力，它全都继承了。

`DefaultListableBeanFactory`是 Mini-Spring 最完整，最通用的 IoC 容器实现。

我们之前用来创建 IoC 容器的 Factory 系的代码：

```java
public class Main {
    public static void main(String[] args) {
        // 创建 factory
        DefaultListableBeanFactory factory = new DefaultListableBeanFactory();

        // 创建 reader
        XmlBeanDefinitionReader xmlBeanDefinitionReader 
                = new XmlBeanDefinitionReader(factory);

        // 读取配置
        xmlBeanDefinitionReader.loadBeanDefinitions("classpath:beans.xml");

        // 获取和使用 bean
        UserService userService = (UserService) factory.getBean("userService");
        userService.doSomething();
    }
}
```

我们 new 的 IoC 容器就是 `DefaultListableBeanFactory`

### 其他的功能接口

在本部分介绍的接口，都是独立的接口，与上面的主线接口没有隶属关系，都是通过 new 一个对象的方式，在具体的实现类中发挥作用。

#### BeanDefinitionReader 接口

`BeanDefinitionReader`接口的作用，其实就是将配置文件中的写好的 Bean 的定义，转化为容器能够识别的 BeanDefinition。

`AbstractBeanDefinitionReader` 定义好了加载配置的流程的框架，这个流程不关心配置文件是什么格式的，具体的格式由子类实现。

`XmlBeanDefinitionReader` 是我们最熟悉的 beans.xml 文件的配置加载器，通过它能够将 beans.xml 文件中的 Bean 的定义转换成 BeanDefinition。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753865243328-9f5cd777-36c8-4953-b550-e75f8e021a10-862971.jpeg)

> 如果有一天你想要通过 JSON 配置 bean，只需要继承 `AbstractBeanDefinitionReader`，重新实现里面所有的方法就可以了，类的命名大概为 `JsonBeanDefinitionReader`

#### DocumentLoader 接口

DocumentLoader 接口和其默认实现类 DefaultDocumentLoader 的作用其实就是将 beans.xml 文件解析成可以在程序中访问的 W3C 标准的 DOM 对象，有一些前端基础的同学应该会很熟悉 DOM 对象。

后续通过访问这个 DOM 对象，生成的 BeanDefinition。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753866036585-293c99bf-9598-4734-b405-fdace0b4442e-050124.jpeg)

#### Resource 接口

Mini-Spring 中的所有配置文件、XML、Properties......只要是文件，统一都通过 Resource 来描述。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753867162129-cb952f47-7290-4923-832d-d796d2175e9a-715277.jpeg)

Resource 接口的核心方法是：

```java
// 获取输入流
InputStream getInputStream();
```

目前实现的三种 Resource 读取方式分别为：

* ClassPathResource：类路径资源，如 `classpath:beans.xml`
* FileSystemResource：本地文件系统资源，如：`file:/data/beans.xml`
* UrlResource: URL 类型资源，如：`https://xxx.com/beans.xml`

Mini-Spring 使用 Resource 描述所有的字段，屏蔽底层差异。

### 容器构造逻辑（Context 体系）

<code>**BeanFactory**</code>\*\* 是 IoC 容器的“内核”\*\*，但是对于业务开发者来说，其提供的功能还是太少了。

Mini-Spring 的 `ApplicationContext` 在继承了 `BeanFactory` 的核心功能后，实现了以下高级功能：

1. 事件机制
2. 运行环境

#### 高级 IoC 容器接口 —— ApplicationContext

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753871420109-e1942829-1da5-469a-abd1-6d0d4aa6abf6-737859.jpeg)

`ApplicationContext` 接口的核心方法：

```java
Environment getEnvironment();
```

#### 容器的生命周期管理 —— ConfigurableApplicationContext 接口

`ConfigurableApplicationContext` 接口提供了管理 IoC 容器生命周期能力，具体如下：

* 控制容器生命周期
* 设置运行环境

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753943175287-93ce9320-7d6f-4a79-a7c0-6da52ace2e12-972360.jpeg)

这个接口提供了**一个十分重要的方法：**

```java
// 刷新容器
void refresh();
```

Mini-Spring 的 IoC 容器就是通过这个方法启动的。

#### AbstractApplicationContext

`AbstractApplicationContext` 实现类是整个 `ApplicationContext` 初始化工作的实现框架，它实现了 `refresh` 方法，还继承了 `DefaultResourceLoader`，让它有了获取读取资源的能力。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753943664949-421aaac2-f901-4cb7-9d0c-a2e6aacbc895-390357.jpeg)

这个类实现了一个最重要的 refresh 方法，这个 refresh 方法是容器启动的一个模板：

```java
public void refresh() throws BeansException {
    this.prepareRefresh();
    ConfigurableListableBeanFactory beanFactory = this.obtainFreshBeanFactory();
    this.prepareBeanFactory(beanFactory);

    try {
        this.postProcessBeanFactory(beanFactory);
        this.invokeBeanFactoryPostProcessors(beanFactory);
        this.registerBeanPostProcessors(beanFactory);
        this.initApplicationEventMulticaster();
        this.onRefresh();
        this.registerListeners();
        this.finishBeanFactoryInitialization(beanFactory);
        this.finishRefresh();
    } catch (BeansException var3) {
        BeansException ex = var3;
        this.destroyBeans();
        this.cancelRefresh(ex);
        throw ex;
    }
}
```

很明显，这个 refersh 是一个模板方法，在这个方法里面调用了其他的方法实现，其他方法有自己实现的，有等待子类实现的。

#### AbstractRefreshableApplicationContext

在 refresh 的方法里，有一步：

```java
ConfigurableListableBeanFactory beanFactory = obtainFreshBeanFactory();
```

这一步就是 `AbstractRefreshableApplicationContext` 接管的。

它的职责有三个：

* 创建一个新的 BeanFactory 实例
* 让子类加载 BeanDefinitions

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753943895502-3ff1defe-20e7-4a21-b387-34a7d34be9b3-382740.jpeg)

#### AbstractXmlApplicationContext

`AbstractXmlApplicationContext` 是**专门用于解析 XML 配置文件**的抽象类，它实现了

`loadBeanDefinitions()` 方法，负责把 XML 中的 `<bean>` 标签加载进 BeanFactory。

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753944179562-e7f26cb6-de0b-4221-a392-536f2773d2aa-179284.jpeg)

#### ClassPathXmlApplicationContext 和 FileSystemXmlApplicationContext

![画板](https://cdn.davidingplus.cn/images/2026/09/11/1753944489317-7b2a4003-f880-432e-baae-8e1b75e2351f-678853.jpeg)


> 更新: 2025-10-11 19:22:37  
> 原文: <https://www.yuque.com/chengxuyuancarl/fryvxd/et7xmltvvtmc2i7k>