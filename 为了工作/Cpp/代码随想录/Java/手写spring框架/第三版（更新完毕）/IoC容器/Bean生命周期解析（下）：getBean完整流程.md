# Bean 生命周期解析（下）：getBean 完整流程

## getBean 的调用时机

上一章，我们已经了解了容器是如何将 XML 配置文件中的 `<bean>` 标签解析并注册到 `DefaultListableBeanFactory` 的 `beanDefinitionMap` 中，最终形成 <code>**beanName -> BeanDefinition**</code> 这样的映射关系。\
这些操作，其实都发生在下面示例中 `main()` 方法的**第一行代码**执行时：

```java
public class ClassPathXmlApplicationContext {
    public static void main(String[] args) {
        ApplicationContext context = new ClassPathXmlApplicationContext("classpath:beans.xml");
        UserService userService = (UserService) context.getBean("userService");
    }
}
```

本章的内容将顺着这个流程继续讲解：**这些已经注册到容器中的 BeanDefinition，是如何一步步变成真正的 Bean 实例的？**

换句话说，当调用 `getBean()` 方法时，容器内部到底经历了什么之后返回了一个对象。

第二行代码是我们开发者手动调用的 `getBean()` 方法，但是在**第一行代码的后半部分**，容器**可能就自己调用了许多次 **<code>**getBean()**</code>** 方法**。

为什么这么说呢？因为**非懒加载的单例 bean，在容器启动后就需要进行实例化**。在上一章提到的容器启动的入口方法——`refresh()`最后的阶段有一个步骤：

```java
finishBeanFactoryInitialization(beanFactory);
```

方法里会调用：

```java
beanFactory.preInstantiateSingletons();
```

我们直接来看 `preInstantiateSingletons()` 的实现：

```java
public void preInstantiateSingletons() throws BeansException {
    // 预初始化所有非懒加载的单例Bean
    for (String beanName : getBeanDefinitionNames()) {
        BeanDefinition beanDefinition = getBeanDefinition(beanName);
        if (beanDefinition.isSingleton()) {
            getBean(beanName);
        }
    }
}
```

这个方法遍历了 `beanDefinitionMap` 中所有的 `BeanDefinition`。

找出了符合以下要求的 `BeanDefinition`：

* 作用域为 `singleton`
* 不是懒加载（`lazyInit = false`）

然后对这些 Bean 调用了 `getBean()`，提前进行实例化。

## getBean 的内部实现

我们直接来看一下 `AbstractBeanFactory` 的中，关于 getBean 的实现。

```java
public abstract class AbstractBeanFactory 
                extends DefaultSingletonBeanRegistry implements BeanFactory {
    @Override
    public Object getBean(String name) throws BeansException {
        return doGetBean(name, null, null);
    }

    @Override
    public <T> T getBean(String name, Class<T> requiredType) throws BeansException {
        return (T) doGetBean(name, requiredType, null);
    }

    @Override
    public <T> T getBean(Class<T> requiredType) throws BeansException {
        return getBean(requiredType.getName(), requiredType);
    }
    
    @Override
    public Object getBean(String name, Object... args) throws BeansException {
        return doGetBean(name, null, args);
    }
}
```

`getBean()` 方法在有很多个实现，但是最终都汇聚了 `doGetBean()` 方法，现在我们来看其内部实现：

```java
protected <T> T doGetBean(String name, Class<T> requiredType, Object[] args) throws BeansException {
    // 先从单例 Bean 缓存中获取
    Object bean = getSingleton(name);
    if (bean != null) {
        System.out.println("从缓存中获取到Bean: " + name);
        return (T) bean;
    }

    // 如果没有，则创建 Bean 实例
    BeanDefinition beanDefinition = getBeanDefinition(name);
    
    if (beanDefinition.isSingleton()) {
        // 对于单例Bean，使用getSingleton方法创建并缓存
        bean = getSingleton(name, new ObjectFactory<Object>() {
            @Override
            public Object getObject() throws BeansException {
                try {
                    return createBean(name, beanDefinition, args);
                } catch (Exception e) {
                    throw new BeansException("创建Bean[" + name + "]失败", e);
                }
            }
        });
        System.out.println("创建并缓存单例Bean: " + name);
    } else {
        // 对于原型Bean，直接创建新实例
        bean = createBean(name, beanDefinition, args);
        System.out.println("创建原型Bean: " + name);
    }

    return (T) bean;
}
```

在 `doGetBean()` 方法中，首先通过 `getSingleton()` 向缓存中查询创建完毕的 Bean

* 如果缓存中存在，则直接返回这个已完成初始化的 Bean
* 如果不存在，则进入 **实例化 Bean** 的流程

在正式实例化之前，需要先判断该 Bean 的作用域是 **单例（singleton）** 还是 **原型（prototype）**

* **单例（singleton）**：实例化后会将 Bean 缓存起来，后续对该 Bean 的所有请求都会返回同一个对象
* **原型（prototype）**：每次请求都会创建一个全新的实例，不会缓存

但是不管单例还是原型 Bean，最终都会进入同一个入口方法：<code>**createBean()**</code>，由它负责后续的实例化和初始化过程。

## createBean 的实现

我们接着来看 `createBean()` 方法，该方法在 `AbstractAutowireCapableBeanFactory` 类中实现。

```java
@Override
protected Object createBean(String beanName, BeanDefinition beanDefinition, Object[] args) throws BeansException {
    Object bean = null;
    try {
        // 通过反射或者 cglib 创建 bean 实例
        bean = createBeanInstance(beanDefinition, beanName, args);
        
        // 处理循环依赖，将实例化后的Bean对象提前放入三级缓存
        // 只有单例才会进行提前暴露
        if (beanDefinition.isSingleton()) {
            final Object finalBean = bean;
            addSingletonFactory(beanName, () -> getEarlyBeanReference(beanName, beanDefinition, finalBean));
            System.out.println("将Bean[" + beanName + "]提前曝光到三级缓存");
        }
        
        // 创建Bean包装器
        BeanWrapper beanWrapper = new BeanWrapper(bean);
        
        // 填充Bean属性
        applyPropertyValues(beanName, bean, beanDefinition, beanWrapper);
        
        // 执行Bean的初始化方法和BeanPostProcessor的前置和后置处理
        bean = initializeBean(beanName, bean, beanDefinition);
    } catch (Exception e) {
        throw new BeansException("创建Bean失败: " + beanName, e);
    }
    
    // 注册销毁方法回调
    registerDisposableBeanIfNecessary(beanName, bean, beanDefinition);
    
    // 注册单例Bean
    if (beanDefinition.isSingleton()) {
        // 处理FactoryBean和循环依赖后，最终加入到单例缓存
        // 如果这个Bean被提前暴露过（即解决了循环依赖），这一步会清除三级缓存中的工厂对象
        registerSingleton(beanName, bean);
    }
    
    return bean;
}
```

整体来看，`createBean()` 内部有三个重要的方法

1. `createBeanInstance()`：创建 Bean 实例
2. `applyPropertyValues()`：为 Bean 实例提供属性
3. `initializeBean()`：执行初始化方法和处理 BeanPostProcessor

此外，`createBean()` 内部还包含与 **循环依赖** 及 **三级缓存** 相关的逻辑，但这些内容本章暂时不展开。

### createBeanInstance 方法

接下来我们看 `createBeanInstance()` 的实现

```java
protected Object createBeanInstance(BeanDefinition beanDefinition, String beanName, Object[] args) {
    System.out.println("创建Bean实例: " + beanName + ", 构造参数: " + (args != null ? args.length : 0));
    
    // 获取所有构造函数
    Constructor<?>[] declaredConstructors = beanDefinition.getBeanClass().getDeclaredConstructors();
    
    // 创建构造函数解析器
    ConstructorResolver constructorResolver = new ConstructorResolver(this);
    
    // 解析构造函数和参数
    BeanInstantiationContext instantiationContext = constructorResolver.autowireConstructor(
            beanName, beanDefinition, declaredConstructors, args);
    
    // 使用实例化策略创建Bean实例
    return getInstantiationStrategy().instantiate(
            beanDefinition, beanName, instantiationContext.getConstructor(), instantiationContext.getArgs());
}
```

创建实例的第一步，是从 `beanDefinition` 中获取 beanName 对应的 `Class` 对象，并确定可用的构造函数，然后根据需要选择合适的构造函数进行实例化。

实例化的方式由 `getInstantiationStrategy()` 决定，这里提供了两种实例策略：

* JDK 反射
* CGLIB 子类代理

下面我们来看 JDK 反射的实现方案：

```java
@Override
public Object instantiate(BeanDefinition beanDefinition, String beanName, Constructor<?> ctor, Object[] args) throws BeansException {
    Class<?> clazz = beanDefinition.getBeanClass();
    try {
        if (ctor != null) {
            // 参数校验
            if (args == null) {
                args = new Object[0];
            }
            
            // 检查参数个数是否匹配
            if (args.length != ctor.getParameterCount()) {
                throw new BeansException("构造函数参数个数不匹配: " + beanName + 
                        "，期望 " + ctor.getParameterCount() + " 个参数，但提供了 " + args.length + " 个参数");
            }
            
            // 使用指定构造函数实例化
            return ctor.newInstance(args);
        } else {
            // 使用默认构造函数实例化
            return clazz.getDeclaredConstructor().newInstance();
        }
    } catch (InstantiationException | IllegalAccessException | InvocationTargetException | NoSuchMethodException e) {
        throw new BeansException("实例化Bean失败 [" + beanName + "]", e);
    }
}
```

JDK 反射在实现上很简单，基本就是反射的简单应用，就不过多讲解了。

> 关于 cglib 的实现，感兴趣的同学可以自行查看代码，具体实现在 `CglibSubclassingInstantiationStrategy.java` 内

### applyPropertyValues 方法

在创建 Bean 实例后，下一步就是填充 Bean 实例的属性了，不过在具体的分析 `applyPropertyValues()` 之前，我们需要先看看 `BeanWrapper` 是什么？

```java
// 创建Bean包装器
BeanWrapper beanWrapper = new BeanWrapper(bean);

// 填充Bean属性
applyPropertyValues(beanName, bean, beanDefinition, beanWrapper);
```

在填充 Bean 实例的属性前，为什么要先创建一个 BeanWrapper 对象呢？还是以 Bean 实例作为参数。

其实只需要看看它的构造函数和，就知道是用来干嘛的了。

```java
public class BeanWrapper {

    private final Object wrappedInstance;
    private Class<?> wrappedClass;
    private TypeConverter typeConverter;

    public BeanWrapper(Object wrappedInstance) {
        this.wrappedInstance = wrappedInstance;
        this.wrappedClass = wrappedInstance.getClass();
        this.typeConverter = new SimpleTypeConverter();
    }

    private void setPropertyValueInternal(Object object, String propertyName, Object value) throws BeansException {
        try {
            // 获取属性类型
            Class<?> propertyType = getPropertyType(object.getClass(), propertyName);
            if (propertyType == null) {
                throw new BeansException("无法确定属性类型: " + propertyName);
            }
            
            // 类型转换
            Object convertedValue = value;
            if (value != null && !propertyType.isInstance(value)) {
                convertedValue = typeConverter.convert(value, propertyType);
            }
            
            // 首先尝试通过setter方法设置
            String setterMethodName = "set" + propertyName.substring(0, 1).toUpperCase() + propertyName.substring(1);
            try {
                Method setterMethod = object.getClass().getMethod(setterMethodName, propertyType);
                setterMethod.invoke(object, convertedValue);
                return;
            } catch (NoSuchMethodException e) {
                // 如果没有setter方法，尝试直接设置字段
                Field field = object.getClass().getDeclaredField(propertyName);
                field.setAccessible(true);
                field.set(object, convertedValue);
            }
        } catch (TypeMismatchException e) {
            throw e;
        } catch (Exception e) {
            throw new BeansException("设置属性值失败: " + propertyName, e);
        }
    }
}
```

在上面的 `setPropertyValueInternal()` 方法中，我们可以看到 `BeanWrapper` 正在通过反射为 Bean 实例设置属性值。\
因此，可以将 `BeanWrapper` 简单看成 Bean 实例的一个 **“属性编辑器”。**

现在我们来看 `applyPropertyValues()` 的具体实现：

> 这里和 Spring 有一些区别，Spring 填充 Bean 实例属性的方法是 populateBean()

```java
// applyPropertyValues(beanName, bean, beanDefinition, beanWrapper);

protected void applyPropertyValues(String beanName, Object bean, BeanDefinition beanDefinition, BeanWrapper beanWrapper) {
    try {
        // 获取 beanDefinition 的 properties
        PropertyValues propertyValues = beanDefinition.getPropertyValues();
        if (propertyValues.isEmpty()) {
            return;
        }
        
        // 创建类型转换器
        TypeConverter typeConverter = new SimpleTypeConverter();
        
        for (PropertyValue propertyValue : propertyValues.getPropertyValues()) {
            String name = propertyValue.getName();
            Object value = propertyValue.getValue();
            
            // 处理Bean引用
            if (value instanceof BeanReference) {
                BeanReference beanReference = (BeanReference) value;
                value = getBean(beanReference.getBeanName());
            }
            
            // 使用BeanWrapper设置属性值
            beanWrapper.setPropertyValue(name, value);
        }
    } catch (Exception e) {
        throw new BeansException("填充Bean属性失败: " + beanName, e);
    }
}
```

在属性填充部分，通过读取 `beanDefinition` 中的 `property` 配置，然后让 `BeanWrapper` 来为 Bean 实例批量设置属性值。

这里有一处逻辑比较重要：

```java
// 处理 Bean 引用
if (value instanceof BeanReference) {
    BeanReference beanReference = (BeanReference) value;
    value = getBean(beanReference.getBeanName());
}
```

如果某个属性的值是对其他 Bean 的引用，那么这处逻辑会再次调用 `getBean()` 来获取这个被依赖的 Bean 实例，并将其注入到当前 Bean 中。这样，Bean 与 Bean 之间的依赖关系就被自动解析并注入完成了。

### initializeBean 方法

填充完毕属性之后，bean 实例的创建工作基本就完成了，接下来需要进行一些收尾工作，

```java
// 执行Bean的初始化方法和BeanPostProcessor的前置和后置处理
bean = initializeBean(beanName, bean, beanDefinition);
```

`initializeBean()` 还是挺重要的一个方法，我们在写 xml 配置的时候，会有一个 init-method 属性，设置了这个属性之后，当 bean 实例创建完毕后，会执行这个属性名的函数。

```xml
<bean id="userService" class="com.example.UserService" init-method="init">
  <property name="name" value="Tom"/>
</bean>
```

我们来看 initializeBean 的具体实现：

```java
private Object initializeBean(String beanName, Object bean, BeanDefinition beanDefinition) {
    // 0. 处理Aware接口
    if (bean instanceof BeanNameAware) {
        ((BeanNameAware) bean).setBeanName(beanName);
    }

    if (bean instanceof BeanFactoryAware) {
        ((BeanFactoryAware) bean).setBeanFactory(this);
    }

    // 1. 执行 BeanPostProcessor 的前置处理
    Object wrappedBean = applyBeanPostProcessorsBeforeInitialization(bean, beanName);

    // 2. 执行初始化方法
    try {
        invokeInitMethods(beanName, wrappedBean, beanDefinition);
    } catch (Exception e) {
        throw new BeansException("执行Bean初始化方法失败: " + beanName, e);
    }

    // 3. 执行 BeanPostProcessor 的后置处理
    wrappedBean = applyBeanPostProcessorsAfterInitialization(wrappedBean, beanName);

    return wrappedBean;
}
```

在 Bean 实例初始化结尾，如果该 Bean 实现了某些 <code>**Aware**</code>\*\* 接口\*\*（例如 `BeanNameAware`、`BeanFactoryAware` 等），`initializeBean()` 方法内会给它注入一些额外的信息，例如：

* **创建它的 BeanFactory**（通过 `BeanFactoryAware`）
* **当前 Bean 在容器中的名称**（通过 `BeanNameAware`）

这些信息能够让 Bean 感知到自己在容器中的环境，不过在这里我们就不继续深入了。

接下来看 `initializeBean()` 方法内的三个重要步骤：

**第一步、执行 BeanPostProcessor 前置方法**

在调用 Bean 自身的初始化方法之前，容器会遍历所有已注册的 `BeanPostProcessor`，依次执行它们的 `postProcessBeforeInitialization()` 方法。

**第二步、执行 init-method 指定的方法**

**第三步、执行 BeanPostProcessor 后置方法**

在 Bean 初始化方法执行完毕后，容器会再次遍历 `BeanPostProcessor`，执行它们的 `postProcessAfterInitialization()` 方法。

第三步是 AOP 实现的基础。


> 更新: 2025-08-29 10:46:49  
> 原文: <https://www.yuque.com/chengxuyuancarl/fryvxd/wfbnxoflm3uzvbse>