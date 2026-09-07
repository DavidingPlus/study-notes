# Bean 生命周期解析（上）：注册BeanDefinition

> 在分析完毕 Mini-Spring 的接口设计之后，本部分的重点就是解析一个 XML 配置文件中的 <bean> 标签是怎么变为 beanFactory 中的 beanDefinitionMap 中的元素的。

## 使用示例

在讲解 `BeanDefinition` 的解析和注册之前，先来看一个简单的例子：

```java
public class ClassPathXmlApplicationContext {
    public static void main(String[] args) {
        ApplicationContext context = new ClassPathXmlApplicationContext("classpath:beans.xml");
        UserService userService = (UserService) context.getBean("userService");
    }
}
```

每个刚学 Spring 的同学都会写一段这样的代码，虽然简短，但它已经包含了两个关键步骤：

1. **配置文件的加载**：通过 `ApplicationContext`，容器加载 `beans.xml` 配置文件
2. **Bean 的获取**：通过 `context.getBean()` 获取 `userService` 的实例

而本章节的内容，正是发生在第一行代码执行的时机。

## 执行流程

当我们使用 XML 配置文件来定义 Bean 的时候，解析并加载配置文件的步骤总体上可分为以下步骤：

![1754852474148-80269523-fb54-40a2-bf5c-2cd3b6d8b90d.png](./img/5N7YbRyl7Ge8ysAL/1754852474148-80269523-fb54-40a2-bf5c-2cd3b6d8b90d-870286.png)

**1、定位配置文件**

根据传入的不同形式的 URI（如 `classpath:`、`file:`、`http:` 等），将所有配置文件路径解析并收集到 `configLocations` 数组中。

**2、资源加载**

遍历 `configLocations`，将每个配置文件路径解析为 `Resource` 对象，并获取其对应的输入流（`InputStream`）。

**3、XML 解析**

使用第三方 XML 解析库将输入流解析为 DOM 文档对象。

**4, 5、BeanDefinition 注册**

遍历 DOM 文档中的 `<bean>` 元素，将其转换为 `BeanDefinition` 对象，

以 `beanName -> BeanDefinition` 的形式注册到 `beanDefinitionMap`（IoC 容器的注册表）中。

以上步骤只是对整体流程的**高度抽象与简化**。实际实现中还包含更多细节，例如对同一继承链上各类功能的逐层抽象与泛化，以及不同继承链之间类与类的协同配合。

## 新建 beanFactory

在 Mini-Spring 中，ApplicationContext 中有两个最底层的实现：

1. ClassPathXmlApplicationContext
2. FileSystemXmlApplicationContext

这两个类都继承于 AbstractXmlApplicationContext，都继承了加载 XML 配置的逻辑，它们的区别在于**配置文件的读取方式。**

* ClassPathXmlApplicationContext 从 `classpath:` 读取资源
* FileSystemXmlApplicationContext 从 `file:` 读取资源

下面是 ClassPathXmlApplicationContext 的案例：

```java
public class Main {
    public static void main(String[] args) {
        ApplicationContext applicationContext
                = new ClassPathXmlApplicationContext("classpath:beans.xml");
        UserService userService = (UserService) applicationContext.getBean("userService");
    }
}
```

这两个类都会将构造函数的参数的 XML 配置的路径存储在自身的 `configLocations` 属性中：

```java
// FileSystemXmlApplicationContext
public class FileSystemXmlApplicationContext extends AbstractXmlApplicationContext {
    private String[] configLocations;

    public FileSystemXmlApplicationContext(String configLocation) throws BeansException {
        this(new String[]{configLocation});
    }

    public FileSystemXmlApplicationContext(String[] configLocations) throws BeansException {
        this.configLocations = configLocations;
        this.refresh();
    }
    // ....
}

// ClassPathXmlApplicationContext
public class ClassPathXmlApplicationContext extends AbstractXmlApplicationContext {
    private String[] configLocations;

    public ClassPathXmlApplicationContext(String configLocation) throws BeansException {
        this(new String[]{configLocation});
    }

    public ClassPathXmlApplicationContext(String[] configLocations) throws BeansException {
        this.configLocations = configLocations;
        this.refresh();
    }
}
```

将 XML 配置资源的路径保存到 `configLocations` 属性后，两个类都同时执行了 `refresh()` 的方法。

`refresh()` 是 IoC 容器启动的核心入口方法，在 `AbstractApplicationContext` 抽象类中实现。它定义了一套完整的流程，用于初始化或完全重置整个 IoC 容器。继承链中的子类必须实现其定义好的各个具体功能步骤。

```java
@Override
public void refresh() throws BeansException {
    // 1. 准备刷新上下文环境
    prepareRefresh();

    // 2. 创建BeanFactory，并加载BeanDefinition
    ConfigurableListableBeanFactory beanFactory = obtainFreshBeanFactory();
    
    // 3. 准备BeanFactory，设置类加载器等
    prepareBeanFactory(beanFactory);
    
    try {
        // 4. 允许子类在标准初始化后修改应用上下文的内部BeanFactory
        postProcessBeanFactory(beanFactory);
        
        // 5. 调用BeanFactoryPostProcessor，修改BeanDefinition
        invokeBeanFactoryPostProcessors(beanFactory);
        
        // 6. 注册BeanPostProcessor，这些处理器在Bean初始化时使用
        registerBeanPostProcessors(beanFactory);
        
        // 7. 初始化事件多播器
        initApplicationEventMulticaster();
        
        // 8. 初始化特定子类的其他bean
        onRefresh();
        
        // 9. 注册监听器
        registerListeners();
        
        // 10. 完成所有单例Bean的实例化
        finishBeanFactoryInitialization(beanFactory);
        
        // 11. 完成刷新过程，发布事件
        finishRefresh();
    } catch (BeansException ex) {
        // 12. 销毁已创建的单例Bean
        destroyBeans();
        
        // 13. 重置上下文活动标志
        cancelRefresh(ex);
        
        // 重新抛出异常
        throw ex;
    }
}
```

容器的启动步骤很多，我们这里仅介绍最核心的一步：**创建 beanFactory**

创建 \*\*beanFactory \*\*从 `obtainFreshBeanFactory()` 方法的调用开始的，在它的内部又调用了另外一个重要的方法——refreshBeanFactory()

```java
ConfigurableListableBeanFactory beanFactory = obtainFreshBeanFactory();

// obtainFreshBeanFactory() 的实现
protected final ConfigurableListableBeanFactory obtainFreshBeanFactory() {
    this.refreshBeanFactory();
    return this.getBeanFactory();
}
```

接下来是 refreshBeanFactory 的具体实现：

```java
protected void refreshBeanFactory() throws BeansException {
    // 如果已存在BeanFactory，则销毁所有单例Bean并关闭工厂
    if (this.beanFactory != null) {
        this.beanFactory.destroySingletons();
        this.beanFactory = null;
    }
    
    // 创建新的BeanFactory
    DefaultListableBeanFactory beanFactory = createBeanFactory();
    
    // 加载Bean定义
    loadBeanDefinitions(beanFactory);
    
    this.beanFactory = beanFactory;
}

protected DefaultListableBeanFactory createBeanFactory() {
    return new DefaultListableBeanFactory();
}
```

`refreshBeanFactory()` 整体逻辑可以分为几个环节：

**1、 检查是否存在旧的 BeanFactory**

第一步就是判断当前应用上下文中是否已经存在一个 BeanFactory 实例。

如果存在：

1. 调用 `destroyBeanFactory()` 方法销毁该实例，并清理它所管理的所有 Bean。
2. 将 `this.beanFactory` 置为 `null`，明确表示上下文中不再持有任何 BeanFactory。

这样做的意义在于，防止新旧 Bean 定义或实例相互干扰，保证后续初始化的容器环境是干净的。

**2、创建新的 BeanFactory**

无论第一步是否销毁了旧的 BeanFactory，接下来都会创建一个新的 `DefaultListableBeanFactory` 对象。

创建完成后，将该实例赋值给 `this.beanFactory`，使其成为当前应用上下文的核心容器。

**3、加载 Bean 定义**

这个时候的 beanFactory，还是一个全新的 IoC 容器，内部没有任何数据。

所以在这里调用了 `loadBeanDefinitions(beanFactory)` 方法，将配置文件中的 Bean 定义解析并注册到新创建的 BeanFactory 中。

> 这里 loadBeanDefinitions(beanFactory) 方法的参数是 beanFactory，前面我们提到过，beanFactory 实现了 BeanDefinitionRegistry 接口，因此 beanFactory 有将 BeanDefinition 注册到 beanDefinitionMap 中的能力，所以在这里作为参数被传入。

## 加载 XML 文件

`loadBeanDefinitions` 是一个很特别的方法。沿着 `ApplicationContext` 这一条继承链，你会发现它在很多层级里都被重写或重载过。

之所以会出现在这么多层级，是因为**不同层级承担的职责不同**：有些层级负责定义通用行为，有些层级负责处理特定场景，但核心都是围绕“加载 Bean 定义”这个共性来抽象和泛化不同的实现。

前文 `refreshBeanFactory()` 的调用中的 `loadBeanDefinitions` 是方法是在

<code>**AbstractXmlApplicationContext**</code>\*\* 中实现的。\*\*

这个类是

* `ClassPathXmlApplicationContext`
* `FileSystemXmlApplicationContext`

的直接父类，它并不关心 `beans.xml` 的物理位置，在它看来，`beans.xml` 只是一个 `Resource` 对象，处理的重点就是如何处理这个 `Resource`。

至于 `Resource` 如何定位和创建——这是两个子类和 XmlBeanDefinitionReader 责任：

* `ClassPathXmlApplicationContext` 返回自己存储的 beans.xml 路径
* `FileSystemXmlApplicationContext` 返回自己存储的 beans.xml 路径
* XmlBeanDefinitionReader 根据路径，将 beans.xml 转成具体的 Resource 对象。

我们来看 AbstractXmlApplicationContext 的实现：

```java
protected void loadBeanDefinitions(DefaultListableBeanFactory beanFactory) throws BeansException {
    // 创建XmlBeanDefinitionReader
    XmlBeanDefinitionReader beanDefinitionReader = new XmlBeanDefinitionReader(beanFactory);
    
    // 获取配置文件资源
    Resource[] configResources = getConfigResources();
    if (configResources != null) {
        // 从资源加载Bean定义 
        beanDefinitionReader.loadBeanDefinitions(configResources);
    }
    
    // 获取配置文件位置
    String[] configLocations = getConfigLocations();
    if (configLocations != null) {
        // 从位置加载Bean定义
        beanDefinitionReader.loadBeanDefinitions(configLocations);
    }
}
```

`ClassPathXmlApplicationContext` 和 `FileSystemXmlApplicationContext` 会先把配置文件的路径保存到自身的 `configLocations` 属性中，并重写父类 `AbstractXmlApplicationContext` 提供的 `getConfigLocations()` 方法。

这样，当 `AbstractXmlApplicationContext` 在执行逻辑时调用 `getConfigLocations()`，就能拿到具体的配置文件路径列表。接下来，这些路径会交给 `beanDefinitionReader` 去处理和加载。

那接下来我们看 `beanDefinitionReader` 实现的 `loadBeanDefinitions` 方法。

```java
public abstract class AbstractBeanDefinitionReader implements BeanDefinitionReader {
    @Override
    public void loadBeanDefinitions(Resource... resources) throws BeansException {
        for (Resource resource : resources) {
            loadBeanDefinitions(resource);
        }
    }

    @Override
    public void loadBeanDefinitions(String location) throws BeansException {
        Resource resource = getResourceLoader().getResource(location);
        loadBeanDefinitions(resource);
    }

    @Override
    public void loadBeanDefinitions(String... locations) throws BeansException {
        for (String location : locations) {
            loadBeanDefinitions(location);
        }
    }
}
```

`BeanDefinitionReader` 内部也实现了多个 `loadBeanDefinitions()` 方法，形式上有些区别，但它们之间基本是互相调用的关系，最终都会汇聚到同一个核心流程。

在这个流程中，真正负责把“文件路径”转换成 `Resource` 对象的，其实是 `resourceLoader`。\
也就是说，`BeanDefinitionReader` 本身并不直接解析路径，而是把路径交给 `resourceLoader`，由它来完成路径解析和 `Resource` 创建的工作。

接下来，我们就来看看 `resourceLoader` 是如何处理这些路径的，以及它是怎样将一个简单的路径字符串，变成容器可以直接使用的 `Resource` 对象的。

```java
public interface ResourceLoader {
    
    String CLASSPATH_URL_PREFIX = "classpath:";
    String FILE_URL_PREFIX = "file:";
    String HTTP_URL_PREFIX = "http:";
    String HTTPS_URL_PREFIX = "https:";
    
    /**
     * 获取资源
     * @param location 资源位置
     * @return 资源对象
     */
    Resource getResource(String location);
}

public class DefaultResourceLoader implements ResourceLoader {
    @Override
    public Resource getResource(String location) {
        if (location == null || location.isEmpty()) {
            throw new IllegalArgumentException("资源路径不能为空");
        }
        
        if (location.startsWith(CLASSPATH_URL_PREFIX)) {
            // 加载classpath资源
            return new ClassPathResource(location.substring(CLASSPATH_URL_PREFIX.length()), getClassLoader());
        }
        
        try {
            // 尝试作为URL资源加载
            URL url = new URL(location);
            return new UrlResource(url);
        } catch (MalformedURLException ex) {
            // 作为文件系统资源加载
            return new FileSystemResource(location);
        }
    }
} 
```

代码运行到这里，逻辑已经很清晰了：\
`resourceLoader` 会根据路径的前缀来决定采用哪种方式处理

* 如果路径以 `classpath:` 开头，就会创建一个 `ClassPathResource` 对象。
* 如果不是，则会尝试将其解析为 URL 资源，或者作为文件系统路径进行加载，并最终创建对应的 `File` 对象。

`ClassPathResource` 的创建过程，其实是借助 `classLoader` 完成的，整体实现并不复杂，感兴趣的话可以直接查看源码，这里已经跳转得太深了，包括 `URLResource` 和 `FileSystemResource` 的实现，也都可以自行查看代码。

经过这一系列调用与跳转，我们终于把一个简单的文件路径，转化成了一个真正可供容器处理的 `Resource` 对象。

## 创建 DOM 文档对象

当 `BeanDefinitionReader` 将文件路径转换成 `Resource` 对象之后，下一步就是解析这个资源，将其内容转化为容器可用的 Bean 定义数据。\
具体过程是：先把 `Resource` 解析成一个 DOM 文档对象，再遍历这个 DOM，把 XML 中 `<bean>` 标签的信息转化为 <code>**beanName -> BeanDefinition**</code> 的映射关系，存入容器内部的 `map` 结构。

这个步骤的实现位于 <code>**XmlBeanDefinitionReader**</code> 中，我们来看核心代码：

```java
public class XmlBeanDefinitionReader extends AbstractBeanDefinitionReader {
    
    @Override
    public void loadBeanDefinitions(Resource resource) throws BeansException {
        try (InputStream inputStream = resource.getInputStream()) {
            doLoadBeanDefinitions(inputStream, resource);
        } catch (IOException | DocumentException e) {
            throw new XmlBeanDefinitionStoreException("解析XML文件失败 [" + resource + "]", e);
        }
    }

    protected void doLoadBeanDefinitions(InputStream inputStream, Resource resource) throws DocumentException {
        // 使用DocumentLoader加载XML文档
        Document document = documentLoader.loadDocument(inputStream);
        
        // 使用BeanDefinitionDocumentReader注册Bean定义
        beanDefinitionDocumentReader.registerBeanDefinitions(document, getRegistry());
        
        // 记录日志
        System.out.println("从资源 [" + resource + "] 加载了Bean定义");
    }
}
```

这里第一个重写的 `loadBeanDefinitions()` 方法参数是 `Resource` 类型，也就是前文通过 `ResourceLoader` 加载得到的对象。\
方法内部先从 `Resource` 获取输入流（`InputStream`），再调用 `doLoadBeanDefinitions()` 进行下一步处理。

在 `doLoadBeanDefinitions()` 中有这样一行代码：

```java
Document document = documentLoader.loadDocument(inputStream);
```

就是在这里，`documentLoader` 将输入流转为了 DOM 对象，而 `documentLoader` 的具体实现是通过第三方 XML 解析库完成的，我们来看默认的 `documentLoader` 的实现：

```java
public class DefaultDocumentLoader implements DocumentLoader {

    /**
     * 从输入流加载XML文档
     * 
     * @param inputStream XML输入流
     * @return 解析后的Document对象
     * @throws DocumentException 如果解析过程中发生错误
     */
    @Override
    public Document loadDocument(InputStream inputStream) throws DocumentException {
        SAXReader reader = new SAXReader();
        return reader.read(inputStream);
    }
} 
```

`SAXReader` 会直接读取输入流并解析为 `Document` 对象。

接下来，这个 `Document` 就会交给 `BeanDefinitionDocumentReader` 去遍历、解析，并最终把

`<bean>` 标签转换为 `BeanDefinition` 注册到容器中。

## 注册 BeanDefinition

在上文的 `doLoadBeanDefinitions()` 方法中，最终生成的 DOM 对象会交给 `beanDefinitionDocumentReader` 处理。这个对象通过自身的 `registerBeanDefinitions()` 方法完成 Bean 的注册流程，我们直接来看核心逻辑。

```java
public class DefaultBeanDefinitionDocumentReader implements BeanDefinitionDocumentReader {

    public static final String BEAN_ELEMENT = "bean";
    public static final String PROPERTY_ELEMENT = "property";
    public static final String ID_ATTRIBUTE = "id";
    public static final String NAME_ATTRIBUTE = "name";
    // ...
    public static final String SCOPE_ATTRIBUTE = "scope";

    @Override
    public void registerBeanDefinitions(Document document, BeanDefinitionRegistry registry) throws BeansException {
        // 获取根元素
        Element root = document.getRootElement();
        
        // 解析文档中的Bean定义
        doRegisterBeanDefinitions(root, registry);
    }
    
    protected void doRegisterBeanDefinitions(Element root, BeanDefinitionRegistry registry) throws BeansException {
        // 处理根元素下的所有子元素
        List<Element> elements = root.elements();
        for (Element element : elements) {
            // 获取元素的命名空间
            String namespaceUri = element.getNamespaceURI();
            
            if (namespaceUri != null && !namespaceUri.isEmpty()) {
                // 处理自定义命名空间
                parseCustomElement(element, registry);
            } else if (element.getName().equals(BEAN_ELEMENT)) {
                // 处理默认的bean元素
                processBeanDefinition(element, registry);
            }
        }
    }
    
    protected void processBeanDefinition(Element beanElement, BeanDefinitionRegistry registry) throws BeansException {
        // 解析bean元素的属性
        String id = beanElement.attributeValue(ID_ATTRIBUTE);
        String name = beanElement.attributeValue(NAME_ATTRIBUTE);
        String className = beanElement.attributeValue(CLASS_ATTRIBUTE);
        String initMethodName = beanElement.attributeValue(INIT_METHOD_ATTRIBUTE);
        String destroyMethodName = beanElement.attributeValue(DESTROY_METHOD_ATTRIBUTE);
        String scope = beanElement.attributeValue(SCOPE_ATTRIBUTE);

        // 获取Class对象
        Class<?> clazz;
        try {
            clazz = Class.forName(className);
        } catch (ClassNotFoundException e) {
            throw new XmlBeanDefinitionStoreException("找不到类 [" + className + "]", e);
        }

        // 确定bean的名称
        String beanName = id != null && !id.isEmpty() ? id : name;
        if (beanName == null || beanName.isEmpty()) {
            // 如果没有指定id和name，使用类名的首字母小写作为bean名称
            beanName = Character.toLowerCase(clazz.getSimpleName().charAt(0)) + clazz.getSimpleName().substring(1);
        }

        // 创建BeanDefinition
        BeanDefinition beanDefinition = new BeanDefinition(clazz);

        // 设置初始化和销毁方法
        if (initMethodName != null && !initMethodName.isEmpty()) {
            beanDefinition.setInitMethodName(initMethodName);
        }
        if (destroyMethodName != null && !destroyMethodName.isEmpty()) {
            beanDefinition.setDestroyMethodName(destroyMethodName);
        }

        // 设置作用域
        if (scope != null && !scope.isEmpty()) {
            beanDefinition.setScope(scope);
        }

        // 解析property元素
        parsePropertyElements(beanElement, beanDefinition);
        
        // 处理自定义属性
        parseCustomAttributes(beanElement, beanDefinition, registry);

        // 注册BeanDefinition
        registry.registerBeanDefinition(beanName, beanDefinition);
    }
}
```

`BeanDefinitionDocumentReader` 的默认实现类是 <code>**DefaultBeanDefinitionDocumentReader**</code>。\
在这个类中可以看到大量静态常量，例如 `BEAN_ELEMENT`、`PROPERTY_ELEMENT` 等，它们对应的值正是 XML 配置文件中常见的标签名称。

`registerBeanDefinitions()` 在获取到 DOM 的根节点后调用 `doRegisterBeanDefinitions()` 方法，开始处理两类内容：

* 自定义命名空间（custom namespace）
* 默认命名空间（default namespace）

在这里，我们只关心默认命名空间的解析过程，也就是 `processBeanDefinition()` 方法，该方法注释比较全面，我们主要看第 83 行代码：

```java
registry.registerBeanDefinition(beanName, beanDefinition);
```

这一步标志着：**XML 配置文件中的 **<code>**<bean>**</code>** 标签，已经被正式解析并注册到 **<code>**beanFactory**</code>** 的 **<code>**beanDefinitionMap**</code>** 中，形成 **<code>**beanName -> BeanDefinition**</code>** 的映射关系。**

**来看 registerBeanDefinition 的实现，直接调用了 map 对象的 put 方法。**

```java
// DefaultListableBeanFactory.java
@Override
public void registerBeanDefinition(String beanName, BeanDefinition beanDefinition) {
    beanDefinitionMap.put(beanName, beanDefinition);
}
```

**那么到这里，BeanDefinition 的加载与注册过程就完成了。**

下一章的内容是当我们调用 `getBean()` 方法时，这些已经注册好的 `BeanDefinition` 又是如何被转换为真正的 Bean 实例的。


> 更新: 2025-08-12 15:04:22  
> 原文: <https://www.yuque.com/chengxuyuancarl/fryvxd/hr5n8zd4dgnwbodo>