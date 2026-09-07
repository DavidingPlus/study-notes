# 5、RPC架构和流程图

总体架构图

![655c04a02b08474e985ff4bf8a561d12.png](./img/F6qb1kvkOxZT-ZeV/655c04a02b08474e985ff4bf8a561d12-275181.png)

下面 我们先看一个最简单的 RPC 框架图

![1731553275055-ad96240b-e07d-4883-bb67-111f6980455a.png](./img/F6qb1kvkOxZT-ZeV/1731553275055-ad96240b-e07d-4883-bb67-111f6980455a-989518.png)

序列化是一个什么过程呢？

序列化是将对象转化为便于网络传输的二进制数据的过程。由于网络传输只能传递二进制数据，因此我们需要通过序列化将对象转换为二进制格式，方便进行数据传输。如图

![1731553289138-6f8edb7d-54c6-4954-83e5-c9fbfbf933f6.png](./img/F6qb1kvkOxZT-ZeV/1731553289138-6f8edb7d-54c6-4954-83e5-c9fbfbf933f6-597585.png)

version1 的 part1 流程图

![1731553365071-ad4d235d-db3c-42db-8855-f06fb9ecabf9.png](./img/F6qb1kvkOxZT-ZeV/1731553365071-ad4d235d-db3c-42db-8855-f06fb9ecabf9-408305.png)

version1 的 part3 流程图

![1732278971190-d0c11e6b-089a-4552-a44c-b921ac2ac151.png](./img/F6qb1kvkOxZT-ZeV/1732278971190-d0c11e6b-089a-4552-a44c-b921ac2ac151-616930.png)

version2 的流程图

![1732279034160-64565983-111f-4593-a7a8-af643b0bc28c.png](./img/F6qb1kvkOxZT-ZeV/1732279034160-64565983-111f-4593-a7a8-af643b0bc28c-874796.png)

version3 的流程图

![1731553470414-4ac72119-e335-4d4e-8774-5403c55adb82.png](./img/F6qb1kvkOxZT-ZeV/1731553470414-4ac72119-e335-4d4e-8774-5403c55adb82-746641.png)

version4 的流程图

![1731553518476-dcba70e4-3c8d-4dd4-9ba0-f916f8cb8e3b.png](./img/F6qb1kvkOxZT-ZeV/1731553518476-dcba70e4-3c8d-4dd4-9ba0-f916f8cb8e3b-293701.png)

version5 的流程图![1733712142889-ed2670d1-e81c-46a1-9a2d-628deee0ecfa.png](./img/F6qb1kvkOxZT-ZeV/1733712142889-ed2670d1-e81c-46a1-9a2d-628deee0ecfa-891745.png)



> 更新: 2024-12-09 10:42:46  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/bxzmgu4s414mw730>