# 4、RPC技术栈

+ **注册中心**：zookeeper 可扩展
+ **序列化方式** ：**Hessian**（默认）、<font style="color:rgb(31, 35, 40);">Protostuff、kryo、JSON、JDK 原生序列化</font>
+ **网络通信**：Netty
+ **动态代理**：<font style="color:rgb(31, 35, 40);">JDK 动态代理</font>
+ **负载均衡**：**一致性哈希算法**（默认）、轮询法、随机法
+ **异常重试**：白名单
+ **熔断限流**
+ **SPI 机制**



> 更新: 2024-12-06 19:09:01  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/rxms1ofmyewtce1a>