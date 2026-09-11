# Mini-Spring 打包与引用

本部分教程用于介绍怎么通过以 maven 引用的方式，在我们自己的项目中引入 mini-spring。

```xml
<dependency>
  <groupId>com.kama</groupId>
  <artifactId>mini-spring</artifactId>
  <version>1.0-SNAPSHOT</version>
</dependency>
```

## 克隆项目代码 & 创建测试项目

首先需要初始化当前教程需要的目录环境，一共分为两步：

> tips：推荐在电脑的任意目录下创建一个 `workspace` 目录，当作本教程的工作目录。

1. **在工作目录下执行以下命令**， 克隆 [mini-spring](https://github.com/youngyangyang04/mini-spring) 的源代码：

> 也可以 fork 项目后，将项目地址修改为自己 fork 后的仓库地址。

```bash
git clone git@github.com:youngyangyang04/mini-spring.git
```

2. **在工作目录下新建一个 maven 项目**，项目名推荐为 `mini-spring-test`

至此，当前工作目录的结构如下：

```bash
workspace
├── mini-spring
│   ├── version1
│   └── version2
├── mini-spring-test
│   ├── src
│   └── target
│   └── pom.xml
```

## 使用 maven 打包 mini-spring

mini-spring 目前有两个版本，分别为 version1 和 version2。两个版本是互相独立的，**这里我们选择 version2**。

切换到 version2 的路径下后，查看 version2 当前的目录结构，核心的文件是 `pom.xml` 文件：

```xml
./version2
├── README.md
├── pom.xml
├── src
│   ├── main
│   │   ├── java
│   │   └── resources
│   └── test
│       ├── java
│       └── resources
```

检查一下 `pom.xml` 文件的开头，如果缺少 `<packaging>jar</packaging>`，**需要自己添加上（以下配置中标记为红色的部分）。**

Java 的版本我选择的是 17 的。

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <groupId>com.kama</groupId>
    <artifactId>mini-spring</artifactId>
    <version>1.0-SNAPSHOT</version>
    <packaging>jar</packaging>

    <properties>
        <maven.compiler.source>17</maven.compiler.source>
        <maven.compiler.target>17</maven.compiler.target>
        <project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
        <junit.version>5.9.2</junit.version>
    </properties>

    <dependencies>
        <!-- JUnit 5 -->
        <dependency>
            <groupId>org.junit.jupiter</groupId>
            <artifactId>junit-jupiter-api</artifactId>
            <version>${junit.version}</version>
            <scope>test</scope>
        </dependency>
        <dependency>
            <groupId>org.junit.jupiter</groupId>
            <artifactId>junit-jupiter-engine</artifactId>
            <version>${junit.version}</version>
            <scope>test</scope>
        </dependency>
        
        <!-- DOM4J 用于XML解析 -->
        <dependency>
            <groupId>org.dom4j</groupId>
            <artifactId>dom4j</artifactId>
            <version>2.1.3</version>
        </dependency>
        
        <!-- CGLIB 用于AOP代理 -->
        <dependency>
            <groupId>cglib</groupId>
            <artifactId>cglib</artifactId>
            <version>3.3.0</version>
        </dependency>
        
        <!-- AspectJ用于切点表达式 -->
        <dependency>
            <groupId>org.aspectj</groupId>
            <artifactId>aspectjweaver</artifactId>
            <version>1.9.9.1</version>
        </dependency>
        
        <!-- 日志框架 -->
        <dependency>
            <groupId>ch.qos.logback</groupId>
            <artifactId>logback-classic</artifactId>
            <version>1.2.11</version>
        </dependency>
    </dependencies>

    <build>
        <plugins>
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-compiler-plugin</artifactId>
                <version>3.8.1</version>
                <configuration>
                    <source>17</source>
                    <target>17</target>
                </configuration>
            </plugin>
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-surefire-plugin</artifactId>
                <version>3.0.0</version>
                <configuration>
                    <argLine>
                        --add-opens java.base/java.lang=ALL-UNNAMED
                        --add-opens java.base/java.util=ALL-UNNAMED
                        --add-opens java.base/java.io=ALL-UNNAMED
                        --add-opens java.base/java.net=ALL-UNNAMED
                        --add-opens java.base/java.lang.reflect=ALL-UNNAMED
                    </argLine>
                </configuration>
            </plugin>
        </plugins>
    </build>
</project>
```

`pom.xml` 文件没问题了以后，在 ./version2 路径下执行以下命令：

```bash
mvn clean install
```

执行成功了一下，会有类似的提示：

```bash
[INFO] --- jar:3.4.1:jar (default-jar) @ mini-spring ---
[INFO] Building jar: /workspace/mini-spring/version2/target/mini-spring-1.0-SNAPSHOT.jar
[INFO] 
[INFO] --- install:3.1.2:install (default-install) @ mini-spring ---
[INFO] Installing /workspace/mini-spring/version2/pom.xml to /Users/user/.m2/repository/com/kama/mini-spring/1.0-SNAPSHOT/mini-spring-1.0-SNAPSHOT.pom
[INFO] Installing /workspace/mini-spring/version2/target/mini-spring-1.0-SNAPSHOT.jar to /Users/user/.m2/repository/com/kama/mini-spring/1.0-SNAPSHOT/mini-spring-1.0-SNAPSHOT.jar
[INFO] ------------------------------------------------------------------------
[INFO] BUILD SUCCESS
[INFO] ------------------------------------------------------------------------
```

这代表我们已经打包成功了。

与此同时，你可以在 target 目录下看到 mini-spring 的 jar 包——`mini-spring-1.0-SNAPSHOT.jar`，

并且这个 jar 包已经被下载到了**本地 maven 仓库**内。现在我们通过 maven 依赖的方式，在我们的项目里引用 mini-spring 了。

```xml
<dependency>
  <groupId>com.kama</groupId>
  <artifactId>mini-spring</artifactId>
  <version>1.0-SNAPSHOT</version>
</dependency>
```

## 在测试项目中引用 mini-spring

回到在工作目录下新建的 `mini-spring-test` 项目下，查看目录结构：

```bash
./mini-spring-test
├── pom.xml
├── src
│   ├── main
│   │   ├── java
│   │   └── resources
```

现在我们在 `pom.xml` 文件中引用 mini-spring 依赖，`pom.xml` 的详细配置如下：

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <groupId>com.kama</groupId>
    <artifactId>mini-spring-test</artifactId>
    <version>1.0-SNAPSHOT</version>

    <properties>
        <maven.compiler.source>17</maven.compiler.source>
        <maven.compiler.target>17</maven.compiler.target>
        <project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
    </properties>

    <dependencies>
        <!-- mini-spring -->
        <dependency>
            <groupId>com.kama</groupId>
            <artifactId>mini-spring</artifactId>
            <version>1.0-SNAPSHOT</version>
        </dependency>
    </dependencies>
</project>
```

在引用完成后，可以开始使用 mini-spring 了，这里会编写三个代码文件，一个 xml 配置文件。

目录结构如下：

```xml
./mini-spring-test
├── pom.xml
├── src
│   ├── main
│   │   ├── java
│   │   │   └── com
│   │   │       └── kama
│   │   │           ├── Main.java
│   │   │           ├── repository
│   │   │           │   └── UserRepository.java
│   │   │           └── service
│   │   │               └── UserService.java
│   │   └── resources
│   │       └── beans.xml
```

具体的文件内容如下：

**1、UserService.java**

```java
package com.kama.service;

import com.kama.repository.UserRepository;

public class UserService {
    private UserRepository userRepository;

    // 提供 setter 供 XML 注入
    public void setUserRepository(UserRepository userRepository) {
        this.userRepository = userRepository;
    }

    public void doSomething() {
        System.out.println("UserService 开始调用");
        userRepository.printInfo();
    }
}
```

**2、UserRepository.java**

```java
package com.kama.repository;

public class UserRepository {
    public void printInfo() {
        System.out.println("UserRepository 正在工作");
    }
}

```

**3、Main.java**

```java
package com.kama;

import com.kama.service.UserService;
import com.minispring.context.ApplicationContext;
import com.minispring.context.support.ClassPathXmlApplicationContext;

public class Main {
    public static void main(String[] args) {
        ApplicationContext context = new ClassPathXmlApplicationContext("classpath:beans.xml");
        UserService userService = (UserService) context.getBean("userService");
        userService.doSomething();
    }
}
```

**4、beans.xml**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans>
    <bean id="userService" class="com.kama.service.UserService">
        <property name="userRepository" ref="userRepository" />
    </bean>

    <bean id="userRepository" class="com.kama.repository.UserRepository" />
</beans>
```

最终在运行 main 函数之前，还有一个问题需要解决。

mini-spring 中的 aop 模块使用了 CGLIB 库做代理，CGLIB 使用了字节码技术 + 反射，动态的生成一个类的子类，并在底层用 `ClassLoader#defineClass` 去定义它，这个方法在 JDK 16 的时候已经不对外暴露了，我们前面选择了 Java17，所以需要解决一下这个问题。

解决方案是在 JVM 启动参数上添加以下参数，告诉 JVM 打开这个模块的访问权限。

```java
--add-opens java.base/java.lang=ALL-UNNAMED
```

下面将在 IDEA 上运行测试项目，使用其他 IDE 的自行配置。

点击 Main 下的 Edit Configurations

![1753436597247-cfb65d8a-cb95-4268-bc6e-c28a27cb196c.png](https://cdn.davidingplus.cn/images/2026/09/11/1753436597247-cfb65d8a-cb95-4268-bc6e-c28a27cb196c-534584.png)

在 Build and run > VM options 部分加上以上的参数。

![1753436913185-79ffe1fe-ba3c-4f9b-a130-c9e549fbc707.png](https://cdn.davidingplus.cn/images/2026/09/11/1753436913185-79ffe1fe-ba3c-4f9b-a130-c9e549fbc707-627080.png)

VM options 输入框可能是隐藏的，可以通过点击的 `Modify options` 下拉框，勾选 `Add VM options` 选项 打开。

![1753437013865-2cd1bb0d-8112-49a2-a40a-00fec1587c0b.png](https://cdn.davidingplus.cn/images/2026/09/11/1753437013865-2cd1bb0d-8112-49a2-a40a-00fec1587c0b-723719.png)

最终的配置如下：

![1753437038840-2cae718a-4bb4-4232-922c-9d1a8fe83f9e.png](https://cdn.davidingplus.cn/images/2026/09/11/1753437038840-2cae718a-4bb4-4232-922c-9d1a8fe83f9e-350450.png)

最终运行结果如下：

![1753437695818-841a2f89-8e63-4071-a9bc-3bfe6d487695.png](https://cdn.davidingplus.cn/images/2026/09/11/1753437695818-841a2f89-8e63-4071-a9bc-3bfe6d487695-719055.png)


> 更新: 2025-07-25 18:01:39  
> 原文: <https://www.yuque.com/chengxuyuancarl/fryvxd/trezgf0mmql7o7ug>