# SpringBoot项目配置多module

多模块结构（multi-module）是企业级项目中常见的组织方式。它有利于模块解耦、职责划分清晰、提高构建效率，同时便于多人协作和功能复用。

## 一、适用场景
+ 项目包含多个职责分明的功能模块
+ 需要提取公共依赖、工具类、通用配置
+ 希望分层解耦，提升代码结构清晰度

## 二、项目结构示意
具体见

[✅目录结构](https://www.yuque.com/chengxuyuancarl/edybxb/silots4bt89k9fch)

```xml
NetworkDisk_Server/               —— 根目录（聚合工程）
│
├── pom.xml                       —— 父工程 pom（统一依赖 & 插件管理）
│
├── networkdisk_auth/         
│   └── pom.xml
│
├── networkdisk_business/             
│   ├── pom.xml 
│		|
|   ├── networkdisk_user
|   ...
|
|
├── networkdisk_common/         
│   ├── pom.xml 
│		|
|   ├── networkdisk_api
|   ...
│
└── networkdisk_gateway/             
    └── pom.xml

```

### 父工程配置
> 通过modules配置对应子模块
>

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 https://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <parent>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-parent</artifactId>
        <version>3.2.2</version>
        <relativePath/>
    </parent>

    <groupId>com.disk</groupId>
    <artifactId>NetworkDisk</artifactId>
    <version>1.0.0-SNAPSHOT</version>
    <name>NetworkDisk</name>
    <description>NetworkDisk Server</description>
    <packaging>pom</packaging>

    <properties>
        <java.version>21</java.version>
        <project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
        <project.reporting.outputEncoding>UTF-8</project.reporting.outputEncoding>
        <java.version>21</java.version>
        <spring-boot.version>3.2.2</spring-boot.version>
        <spring-cloud.version>2023.0.0</spring-cloud.version>
        <spring-cloud-alibaba.version>2022.0.0.0</spring-cloud-alibaba.version>
        <dubbo.version>3.2.10</dubbo.version>
        <commons-lang3.version>3.5</commons-lang3.version>
        <commons-collections.version>3.2.1</commons-collections.version>
        <commons-io.version>2.4</commons-io.version>
    </properties>

    <modules>
        <module>networkdisk-auth</module>
        <module>networkdisk-business</module>
        <module>networkdisk-common</module>
        <module>networkdisk-gateway</module>
    </modules>

    <dependencyManagement>
        <dependencies>
          ...
        </dependencies>
    </dependencyManagement>


    <build>
        ...
    </build>

    <repositories>
       ...
    </repositories>
    <pluginRepositories>
        ...
    </pluginRepositories>

</project>

```



### 子模块配置
> 使用<parent> 指定父工程
>

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 https://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <parent>
        <groupId>com.disk</groupId>
        <artifactId>NetworkDisk</artifactId>
        <version>1.0.0-SNAPSHOT</version>
    </parent>

    <groupId>com.disk</groupId>
    <artifactId>networkdisk-auth</artifactId>
    <version>1.0.0-SNAPSHOT</version>
    <name>networkdisk-auth</name>
    <description>networkdisk-auth</description>

    <properties>
        <application.name>networkdisk-auth</application.name>
        <maven.compiler.source>21</maven.compiler.source>
        <maven.compiler.target>21</maven.compiler.target>
        <project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
    </properties>

    <dependencies>
        ...
    </dependencies>

</project>
```



> 更新: 2025-08-04 10:44:52  
> 原文: <https://www.yuque.com/chengxuyuancarl/edybxb/qn2b14q11qm2cw8c>