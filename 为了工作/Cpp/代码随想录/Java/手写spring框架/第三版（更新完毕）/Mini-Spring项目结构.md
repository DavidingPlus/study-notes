# Mini-Spring 项目结构

## Mini-Spring 项目结构

项目结构怎么划分，写的代码文件应该存放在那个文件夹下，文件夹之间的关系应该怎么组织？这是每个人在做体量稍微大一些的项目时都会遇到的问题。在我看来这个问题的难度不亚于怎么给变量取名。。。

**Mini-Spring 在项目结构上的划分完全遵循 Spring 的设计**，具体划分如下：

```bash
mini-spring
├── aop
│   ├── aspectj
│   ├── framework
│   └── support
├── beans
│   └── factory
│   │   ├── config
│   │   ├── support
│   │   └── xml
|   └── BeanFactory.java  # 很重要
├── context
│   ├── event
│   └── support
├── core  # 核心
│   ├── convert
│   │   ├── converter
│   │   └── support
│   ├── env
│   └── io
├── util
└── web
    └── context
        └── request
```

**1、util 模块**

这个模块提供公共的工具类，用于将工具代码与业务代码进行分离。

***

**2、core 模块**

core 模块是 Spring 最底层的模块，不依赖于其他的任何模块，为框架运行提供最基础的能力。

core 模块下有三个子模块，分别为：

1. convert
2. env
3. io

\*\*convert 是内部实现的类型转换系统。\*\*提供统一的 Converter 接口，并内置了多种实现，例如：

* String -> Integer
* String -> Boolean

**env 是对程序运行环境的抽象。**程序运行所需的配置可能需要从**多种来源**加载。例如从 `.yaml`、`.properties`、系统的环境变量等。env 需要提供**统一的接口**来访问这些属性，屏蔽配置来源的差异。

**io 是对 Resource 的抽象。**Spring 通过读取配置文件来定义和管理 Bean 之间的依赖关系，这些**配置文件被抽象成资源（Resource）**。资源可能会处于类路径、本地文件系统、网络等，资源的格式也可能是多样的，例如 xml 格式、现代 spring 最常用的 Java 注解的格式，或者是其它的自定义格式。Spring 提供一个**统一访问资源**的接口，**不需要关心资源的来源和格式**。

在 env 和 io 都涉及到了配置，这里做一个区分：

* env 提到的配置侧重于管理程序在运行时的环境属性，比如数据库的配置
* io 提到的配置则是被抽象成资源的 Bean 的配置文件

***

**3、beans 模块**

beans 模块是 IoC 容器的核心模块，其中的 BeanFactory 接口是整个 IoC 容器的最底层抽象。你也可以直接将 BeanFacory 当作 IoC 容器的最简实现，factory 模块和 BeanFactory 接口同级。

facotory 下有三个子模块：

1. config
2. support
3. xml

\*\*config 用于定义和描述 Bean 的元信息。\*\*包括 BeanDefinition 的定义，属性的管理，作用域等配置。计算机的功能，往往通过用数据结构对现实世界进行抽象来实现。**BeanDefinition** 也是一样，它把配置文件里的 `<bean>` 标签映射成了程序里可以操作的对象。

\*\*support 主要包含了 BeanFactory 的很多实现类。\*\*封装了许多容器的核心行为，例如 Bean 的注册、获取等，为上层容器提供公共支持。

\*\*xml 负责解析解析 XML 文件。\*\*其负责将 `<bean>` 标签转换为 BeanDefinition 对象，核心功能包括读取文件，解析 DOM 结构等。

mini-spring 目前仅支持从 XML 文件加载 Bean 定义，如果希望通过注解（@Component、@Configuration）加载 Bean 信息，可以在 factory 下再添加一个 annotation 子模块。

如果你想支持 JSON、YAML、亦或是从其它自定义格式中定义、读取 Bean 的功能，也都可以通过添加子模块的方式来实现。

***

**4、context 模块**

\*\*ApplicationContext 接口是对 BeanFactory 接口的增强，提供了很多高级的功能。\*\*例如事件发布、监听模型等。context 模块下有两个子模块：

* event
* support

**event 内用于实现事件发布，监听模型。**

**support 中包含了 Application 的具体实现。**

***

**5、aop 模块**


> 更新: 2025-08-13 20:31:02  
> 原文: <https://www.yuque.com/chengxuyuancarl/fryvxd/ibxlnvu9pbfs2khe>