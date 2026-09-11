# 8、经典的RPC框架

# gRPC
![1731327924332-3644e086-8be7-456e-af51-a7d8f0cdf2c1.png](https://cdn.davidingplus.cn/images/2026/09/11/1731327924332-3644e086-8be7-456e-af51-a7d8f0cdf2c1-916387.png)

## 简介
由 Google 开发，基于 HTTP/2 协议，支持多种编程语言，并采用 protocol Buffers(protocbuf) 作为序列化方式。

## 优点
**高效传输：**基于 HTTP/2 和protocbuf，传输效率高，适合高并发场景。

**多语言支持：**支持多种语言，便于构建多语言微服务架构。

**流式传输：**支持双向流通信，可以实现实时通信。

**强类型：**通过protocbuf 定义接口，严格的类型约束提高了通信的可靠性。

## 缺点
**学习成本高：** 需要学习 protobuf 语法和 HTTP/2。  

**RESTful 支持不佳  ：**gRPC 的服务定义方式与 RESTful API 不完全兼容，在传统 REST API 迁移可能会有不便。

**浏览器兼容性： **浏览器不原生支持 HTTP/2，因此需要额外的代理。**  **

## 官网：[gRPC](https://grpc.io/)
## 源码：[gRPC](https://github.com/grpc/grpc-java)
# Dubbo
![1731328063928-776b5e51-4e6e-47b9-a0f4-8cde45fedeea.png](https://cdn.davidingplus.cn/images/2026/09/11/1731328063928-776b5e51-4e6e-47b9-a0f4-8cde45fedeea-354166.png)

## 简介
由阿里巴巴开源的 RPC 框架，主要面向 Java 社区，后来扩展支持其他语言。

## 优点
**高度集成：**支持服务注册、发现、负载均衡、容错等功能，适合构建完整的分布式系统。

**高性能：**提供二进制传输协议 Dubbo protocol,效率高于 HTTP 协议。

**可扩展性：**可插拔设计，用户可以根据需求替换协议，序列化等组件。

## 缺点
**跨语言支持较差：**虽然近些年，Dubbo 支持了多种语言，但主流应用还是在 Java 环境中

**依赖 Zookeeper:**服务注册一般使用 Zookeeper,部署和维护的成本高。

**学习曲线：**Dubbo 的概念和配置较多，初学者需要花比较多的时间学习。

## 官网：[Apache Dubbo 中文](https://cn.dubbo.apache.org/zh-cn/)
## 源码：[Dubbo](https://github.com/apache/dubbo)
# Thrift
## 简介
由 FaceBook 开发的 RPC 框架，支持多种语言。

## 优点
**多语言支持：**支持多种语言互操作，适合跨语言的分布式系统。

**轻量且高效：**提供多种传输协议和序列化，适合不同情况的网络条件。

**服务端实现灵活：**可以自定义传输协议和序列化，来满足不同的性能需求。

## 缺点
**开发体验不如 gRPC:**缺乏标准的流式传输支持，需要开发者自己处理复杂的双向通信。

**生态不够完善：**与 gRPC 和 Dubbo 相比，工具和插件相对较少。

## 官网：[Apache Thrift - Home](https://thrift.apache.org/)
## 源码：[GitHub - apache/thrift: Apache Thrift](https://github.com/apache/thrift)
# Spring Cloud OpenFeign
## 简介
是 Spring Cloud 生态的一部分，基于 HTTP 协议，通常与 Eureka、Ribbon 等服务注册和负载均衡组件配合使用。

## 优点
**与 Spring 无缝集成：**适合 Spring 系统，配置简单，使用注解即可实现调用。

**RESTful 风格：**基于 REST 接口，便于与已有的 REST API 集成。

**学习成本低：**面向 Java 开发者友好，不需要额外学习新的协议或序列化方式。

## 缺点
**性能相对较低：**基于 HTTP 协议的 JSON 传输，序列化和反序列化性能不如二进制协议。

**不支持双向流：**不适合实时通信和流式传输的场景。

## 官网：[Spring Cloud OpenFeign](https://spring.io/projects/spring-cloud-openfeign)
## 源码：[GitHub - spring-cloud/spring-cloud-openfeign: Support for using OpenFeign in Spring Cloud apps](https://github.com/spring-cloud/spring-cloud-openfeign)
# TARS
## 简介
由腾讯开发的 RPC 框架，支持多语言和高性能的服务治理，适合构建大型分布式系统。

## 优点
**多语言支持：**支持 C++、Java、PHP、Node.js 等语言。

**高性能：**采用二进制协议，性能较好。

**服务治理：**集成服务发现、负载均衡、配置管理等功能，适合分布式架构。

## 缺点
**学习曲线陡峭：**配置和功能较多，适合有一定分布式系统经验的开发者。

**生态限制：**虽然支持多语言，但主要在腾讯内部使用，在开源社区的支持相对较弱。

## 官网：[TARS](https://doc.tarsyun.com/#/default-index)
如果你是 Java 选手，并且在纠结选择哪一个 RPC 框架，我建议选 Dubbo

# 总结
|  | 优点 | 缺点 | 适合场景 |
| --- | --- | --- | --- |
| gRPC | 高效、跨语言、支持流式传输 | 学习成本高、不支持 REST | 高性能跨语言系统 |
| Dubbo |  高度集成、高性能、可扩展   | 跨语言差、依赖 ZooKeeper | Java 微服务系统 |
| Thrift |  多语言、高效   |  开发体验一般、生态较弱   | 跨语言分布式系统 |
|  OpenFeign   |  与 Spring Cloud 集成、简单   |  性能低、无流式传输   |  Spring Cloud RESTful 系统   |
|  TARS   | 多语言、高性能、服务治理 |  学习曲线陡、生态限制   |  大型分布式系统   |




> 更新: 2024-12-03 19:25:51  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/eyfmu1lq2xw8gfa3>