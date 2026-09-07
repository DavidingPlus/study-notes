# IoC 容器基础：控制反转与依赖注入

## IoC 容器与控制反转

IoC 容器（Inversion of Control Container）是一个自动创建对象、管理依赖、控制对象生命周期的对象工厂。

IoC 容器这次名词可以拆分来看：

* IoC——控制反转
* 容器

**我们先来看容器。**

在面向对象里，我们写功能的时候，通常是先写一个类，然后通过它创建对象来执行具体的操作。一个系统的功能，就是这些对象一起完成的。这些对象都是我们手动 new 出来的，而 new 出一个新对象的操作往往分散在系统的各个部分。

所谓容器，就是把原来那些分散在各处、我们手动 new 出来的对象，集中放到一起统一管理。

**我们可以把容器简单理解成一个哈希表**，key 是对象的名字，value 就是对象本身（实际上也是这样做的）。

```java
Map<String, Object> container;
```

既然这些对象都集中在一起管理了，那创建对象的操作自然也交给容器来了，至此我们就可以十分自然的引出依赖反转的概念了。

在实际的工程中，只有非常简单的对象才能独立的完成工作。大多数时候，对象之间会有依赖关系，比如 Service 层对象，就需要依赖于 Dao 层对象，才能完成数据的持久化。

我们以打印机为例子：

```java
public class InkService {
    public boolean hasInk() {
        return true; // 假设墨水充足
    }
}

public class PaperTray {
    public boolean hasPaper() {
        return true; // 假设纸张充足
    }
}

public class PrinterService {
    private InkService inkService;
    private PaperTray paperTray;

    public PrinterService(InkService inkService, PaperTray paperTray) {
        this.inkService = inkService;
        this.paperTray = paperTray;
    }

    public void print(String content) {
        if (!inkService.hasInk()) {
            System.out.println("Cannot print: No ink.");
            return;
        }
        if (!paperTray.hasPaper()) {
            System.out.println("Cannot print: No paper.");
            return;
        }
        System.out.println("Printing: " + content);
    }
}

public class Main {
    public static void main(String[] args) {
        InkService inkService = new InkService();
        PaperTray paperTray = new PaperTray();
        PrinterService printerService = new PrinterService(inkService, paperTray);

        printerService.print("Hello, world!");
    }
}
```

`PrinterService` 本身是没法单独工作的，它还需要 `InkService` 和 `PaperTray` 这两个对象来完成打印操作。这种对象与对象之间的关系就是我们说的“依赖”。

在上面的代码里，我们是在 `main` 方法里手动 `new` 出 `InkService` 和 `PaperTray`，再通过构造函数传给 `PrinterService`。

但是如果我们希望让容器来帮我们创建 `PrinterService`，那它就得把这两个依赖也一起创建出来，并注入进去。这样，依赖的创建不再由我们自己控制，而是交给了容器处理。**这就是依赖反转的含义：依赖对象的获得被反转了。**

## 依赖注入和依赖查找

IoC 是一种设计思想，只要对象依赖的获取过程不再由对象本身控制，而是由外部容器处理，只要符合这种思想的实现，都可以说它是 IoC。

目前的主要实现方式有两种：

1. 依赖查找
2. 依赖注入（DI）

### 依赖查找

容器只提供一套对外的 API，让对象在其需要时，自行从容器中查找获取其所依赖的对象。

这意味着，**依赖对象获取的控制权**，仍然在对象手上，容器仅提供了一个仓库供其查询。

以上面的打印机为例子：

**这是 beans.xml 配置文件：**

```xml
<bean id="inkService" class="com.example.InkService"/>
<bean id="paperTray" class="com.example.PaperTray"/>
<bean id="printerService" class="com.example.PrinterService">
    <property name="inkService" ref="inkService"/>
    <property name="paperTray" ref="paperTray"/>
</bean>
```

根据 beans.xml 创建了一个 IoC 容器。

```java
ApplicationContext context = new ClassPathXmlApplicationContext("beans.xml")

PrinterService printer = (PrinterService) context.getBean("printerService");
InkService ink = (InkService) context.getBean("inkService");
InkService paper = (PaperTray) context.getBean("paperTray");

printer.setInkService(ink);
printer.setPaperTray(paper);
```

可以看到，printer 依赖的两个对象是从容器中获取，但是获取对象的控制权，仍然在 printer 手中（自行 setter)

> 仅做思想上的示例，ClassPathXmlApplicationContext 是 Spring 的具体实现，因为  beans.xml 中配置了依赖关系，Spring 会自动将 inkService 和 paperTray 注入到 printer 中。

### 依赖注入

依赖注入最核心的思想是，依赖对象的获取，不再依靠对象自行获取，而是由容器主动的将依赖“注入”到对象中。

对象只需要提供 Java 方法，供容器注入依赖即可。

根据对象提供的 Java 方法不同，注入方式也有以下两种区分：

**1、setter 注入：**

```java
public class PrinterService {
    private InkService inkService;
    private PaperTray paperTray;

    public void setInkService(InkService ink) {
        this.inkService = ink;
    }

    public void setPaperTray(PaperTray paper) {
        this.paperTray = paper;
    }
}
```

容器在分析 `PrinterService` 依赖于 `InkService` 和 `PaperTray` 对象后，会自行在容器内查找其依赖的两个对象，并通过 `PrinterService` 提供的 `setter` 方法，将查找到的依赖对象注入到  `PrinterService` 内。

**2、构造器注入：**

```java
public class PrinterService {
    private InkService inkService;
    private PaperTray paperTray;

    public PrinterService(InkService ink, PaperTray paper) {
        this.inkService = ink;
        this.paperTray = paper;
    }
}
```

同理，容器将 `PrinterService` 依赖的两个对象，通过构造器的方式，注入到 `PrinterService` 内。

### 依赖查找与依赖注入

虽然说现在的许多教程，包括维基百科等平台，都将依赖查找当作 IoC 的一种实现方式，但是依赖查找更像一个实现 IoC 的**半成品**。

在依赖查找内，对象仍然需要**主动的向容器**获取依赖对象。也就是说：

* 容器仅仅只是一个对象仓库
* 获取依赖的**主动权仍然在对象手中**

这其实只是**部分让渡了依赖管理的职责**，控制权仍然在对象手里，并不能说“控制反转”。

这里再复习一下 IoC（依赖反转）的概念：依赖对象的**获取**被反转了\*\*。\*\*

因此，依赖查找仅仅只是借用容器存储依赖对象，**而依赖对象的获取仍没有被反转**。

相比之下，依赖注入才是真正的 IoC 实现。对象只需要提供普通的 Java 方法，其所依赖的对象就会被容器自行查找，并注入到对象内部，这整个过程完全由容器进行控制，对象完全失去了获取依赖对象的控制权，这种模式才是真正的反转了依赖对象的获取。


> 更新: 2025-07-29 11:20:55  
> 原文: <https://www.yuque.com/chengxuyuancarl/fryvxd/tepwy6rfddte7iwz>