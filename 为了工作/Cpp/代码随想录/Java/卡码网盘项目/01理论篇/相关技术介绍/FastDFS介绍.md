# FastDFS介绍

## <font style="color:rgb(31, 35, 40);">概述</font>
+ <font style="color:rgb(31, 35, 40);">FastDFS是一个轻量级的开源分布式文件系统</font>
+ <font style="color:rgb(31, 35, 40);">FastDFS主要解决了大容量的文件存储和高并发访问的问题，文件存取时实现了负载均衡</font>
+ <font style="color:rgb(31, 35, 40);">FastDFS实现了软件方式的RAID，可以使用廉价的IDE硬盘进行存储</font>
+ <font style="color:rgb(31, 35, 40);">支持存储服务器在线扩容</font>
+ <font style="color:rgb(31, 35, 40);">支持相同内容的文件只保存一份，节约磁盘空间</font>
+ <font style="color:rgb(31, 35, 40);">FastDFS只能通过Client API访问，不支持POSIX访问方式</font>
+ <font style="color:rgb(31, 35, 40);">FastDFS特别适合大中型网站使用，用来存储资源文件（如：图片、文档、音频、视频等等）</font>

## <font style="color:rgb(31, 35, 40);">FastDFS与集中存储方案的对比</font>
![fdfsVsNFS.png](https://cdn.davidingplus.cn/images/2026/09/11/fdfsVsNFS-820742.png)

## 架构&原理
<font style="color:rgb(31, 35, 40);">如图所示，FastDFS总体架构由三个部分构成</font>

+ <font style="color:rgb(31, 35, 40);">客户端（Client）</font>
+ <font style="color:rgb(31, 35, 40);">访问服务器（TrackerServer）</font>
+ <font style="color:rgb(31, 35, 40);">存储服务器（StorageServer）</font>

![architecture.png](https://cdn.davidingplus.cn/images/2026/09/11/architecture-560578.png)

### <font style="color:rgb(31, 35, 40);">1. 客户端（Client）</font>
<font style="color:rgb(31, 35, 40);">客户端指的是访问FastDFS分布式存储的客户端设备，通常是应用服务器。</font>

### <font style="color:rgb(31, 35, 40);">2. 访问服务器（TrackerServer）</font>
<font style="color:rgb(31, 35, 40);">TrackerServer是访问（或者翻译为跟踪）服务器，是Client访问StorageServer的入口。</font>

<font style="color:rgb(31, 35, 40);">TrackerServer起到几个作用:</font>

+ <font style="color:rgb(31, 35, 40);">【服务注册】管理StorageServer存储集群，StorageServer启动时，会把自己注册到TrackerServer上，并且定期报告自身状态信息，包括磁盘剩余空间、文件同步状况、文件上传下载次数等统计信息</font>
+ <font style="color:rgb(31, 35, 40);">【服务发现】Client访问StorageServer之前，必须先访问TrackerServer，动态获取到StorageServer的连接信息</font>

#### <font style="color:rgb(31, 35, 40);">2.1 TrackerServer高可用</font>
<font style="color:rgb(31, 35, 40);">为了保证高可用，一个FastDFS集群当中可以有多个TrackerServer节点，由集群自动选举一个leader节点。</font>

### <font style="color:rgb(31, 35, 40);">3. 存储服务器（StorageServer）</font>
<font style="color:rgb(31, 35, 40);">StorageServer是数据存储服务器，文件和meta data都保存到存储服务器上。</font>

+ <font style="color:rgb(31, 35, 40);">可采用高可用的方式进行数据存储</font>
+ <font style="color:rgb(31, 35, 40);">FastDFS集群当中StorageServer按组(Group/volume)提供服务，不同组的StorageServer之间不会相互通信，同组内的StorageServer之间会相互连接进行文件同步</font>
+ <font style="color:rgb(31, 35, 40);">Storage server采用binlog文件记录文件上传、删除等更新操作。binlog中只记录文件名，不记录文件内容</font>
+ <font style="color:rgb(31, 35, 40);">文件同步只在同组内的Storage server之间进行，采用push方式，即源头服务器同步给目标服务器</font>

### <font style="color:rgb(31, 35, 40);">4. 文件存储</font>
<font style="color:rgb(31, 35, 40);">与大多数分布式文件系统类似，FastDFS可以将文件以及相关的描述信息(MetaData)保存到StorageServer当中。</font>

<font style="color:rgb(31, 35, 40);">文件存储以后将返回唯一的“文件标识”，“文件标识”由"组名"和"文件名"两部分构成</font>

<font style="color:rgb(31, 35, 40);">MetaData是文件的描述信息，如 width=1024,heigth=768</font>



> 更新: 2026-02-10 16:03:29  
> 原文: <https://www.yuque.com/chengxuyuancarl/edybxb/upzgyuu7n0gcwex6>