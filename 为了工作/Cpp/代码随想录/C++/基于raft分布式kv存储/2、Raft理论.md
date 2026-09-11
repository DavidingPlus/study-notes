# 2、Raft理论

# 大纲图：
<font style="color:#DF2A3F;">温馨提醒篇幅较长，请自己按自己能力安排时间，慢慢食用。</font>

沿用上一个模块引入的大纲图

![1732538542475-e7028faf-cadb-462b-af66-dd10d367a605.png](https://cdn.davidingplus.cn/images/2026/09/11/1732538542475-e7028faf-cadb-462b-af66-dd10d367a605-834448.png)

其中项目大概可以分为以下几个部分：

+ raft节点：raft算法的实现核心层，负责与其他机器的raft节点沟通，达到分布式共识的目的。
+ raftServer：负责raft节点与K-V数据库中间的协调服务；负责持久化K-V数据库的数据（可选）。
+ 上层状态机(K-V数据库)：复制数据存储。
+ 持久层：负责相关数据的落盘，对于raft节点，根据共识算法要求，必须对一些关键数据进行落盘处理，以保证节点宕机后重启可以恢复关键数据；对于raftServer，可能会有一些K-V数据库的东西需要落盘持久化。
+ RPC通信：在领导者选举、日志复制、数据查询、心跳等多个Raft重要过程提供多节点快速简单的通信能力。

# Raft算法的基本原理：
<font style="color:rgb(51, 51, 51);">Raft算法是一种分布式算法，在解决分布式系统中的一致性问题，相对于Paxos算法而言容易理解。</font>

<font style="color:rgb(51, 51, 51);">Raft算法将系统中的所有节点分成三个角色：追随者，候选者，领导者。其选举机制确保系统中的一个节点被选为领导者，领导者负责处理客户端请求，向追随者更新日志。</font>

这里推荐[ https://raft.github.io/]( https://raft.github.io/) ，一个可视化网站，可以对Raft算法有初步的整体了解。

## Raft的一些重要概念：
1. 状态机：raft的上层应用，可以是K-V数据库、也可也是redis等。
2. 日志log、任期term、entry条目：
    - 日志log(raft保存的是客户端请求外部命令是以日志保存的)。
    - entry条目：日志有很多，可以看成一个连续的数组，而其中的一个成为enrty条目
3. 提交日志commit：raft保存日志后，经过复制同步，才能真正应用到上层状态机，这个“应用”的过程称为提交。
4. raft的节点身份：Follower追随者、Candidate候选者、Leader领导者。
5. 选举：Follower变成候选者Candidate，成功选举后将身份变化成Leader的一个过程。
6. 日志的term(领导任期)：在日志提交的时候，会记录这个日志在什么“时候”(哪一个领导的term)记录的，用于后续日志的新旧对比。
7. 心跳、日志同步：Leader向Follower发送心跳(AppendEntryRPC)用于告诉Follower自己的存在以及通过心跳携带日志，来同步Follower的日志。

---

## 接下来是对上面部分概念的详解：
### 1、日志
首先掌握日志的概念，Raft算法可以让多个节点的上层状态机保存一致的关键是让**各个节点的日志保持一致，**日志中保存客户端发送来的操作命令，上层状态机根据传递上来的日志执行命令，那么日志一致那么raft集群的个个状态机的状态肯定也一致。

<font style="color:#DF2A3F;">所以raft共识算法的目的就是保证各个节点的日志是相同的。</font>

节点身份：Follower、Candidate、Leader；

每个Raft节点也有自己的状态机，又下面三种状态构成：

ps：【这里的状态机与前面的状态机语义上有些“重载”，但不是一个东西，如果你无法区分那么其他地方的状态机或者上层状态机都可以换成kv数据库】

![1732603589706-cd024495-7cdc-466f-b0ee-6547679c8211.png](https://cdn.davidingplus.cn/images/2026/09/11/1732603589706-cd024495-7cdc-466f-b0ee-6547679c8211-325603.png)

简单解释一下：

+ 当raft集群启动首先都是一个Follower的状态
+ Follower的过程：当raft集群启动首先都是一个Follower的状态，当通过心跳机制会去判断这个集群中是否存在领导节点，如果心跳机制超时代表此时并没有领导节点，就会变成Candidate候选者。
+ Candidate的过程：向其他Follower发起选票，当然不用怕多个候选者同时选票的情况，采取随机时间可以很好避免这种情况，当候选者Candidate得到超过大部分的票数的时候，就会从候选者变成Leader。
+ Leader的过程：开始日志同步，提交日志到kv数据库完成操作。

**注：**<font style="color:#DF2A3F;">但是如果你运气很不好自己当选领导者或者还是候选者的时候发现，有的领导的任期已经比你要当选的领导的任期更大的时候，会采取主动退让的机制，从领导者或者候选者变成Follower追随者。</font>



### 2、任期Term
Raft将Term作为内部逻辑时钟，使用Term的对比来比较日志、身份、心跳的新旧而不是绝对时间。Term与Leader的身份绑定，即某个节点是Leader更严谨一点的说法是集群某个Term的Leader。Term用连续的数字进行标识。Term会在follower发起选举(称为候选者试图成为领导者)的时候加1，对于一次选举可能存在两种结果：

1、胜利的man：<font style="color:rgb(47, 48, 52);">胜利的条件是超过半数的节点认为当前Candidate有资格成为Leader，即超过半数的节点给当前Candidate投了选票。</font>

2、失败的man：<font style="color:rgb(47, 48, 52);">如果没有任何Candidate（一个Term的Leader只有一位，但是如果多个节点同时发起选举，那么某个Term 的Candidate可能有多位）获得超半数的选票，那么选举超时之后又会开始另一个Term（Term递增）的选举。</font>

**问题：**

Raft是如何保证一个Term只有一个Leader的？

因为Candidate变成Leader的条件是获得超过半数选票，一个节点在一个Term内只有一个投票(投给了别人就不能再继续投了)，因此不可能有两个节点同时获得超过半数的选票。



发生故障时，一个节点无法知道当前最新的Term是多少，在故障恢复后，节点可以通过其他节点发送过来的心跳中的Term信息查明一些过期信息。

当发现自己的Term小于其他Term时，这意味着“自己已经过期”，不同身份的节点处理方式有所不同：

    - Leader、Candidate：退回follower并更新Term到较大的那个Term。
    - follower：更新Term信息到较大的那个Term。

**<font style="color:rgb(47, 48, 52);">注：</font>**这里解释一下为什么 自己的Term小于其他节点的Term时leader、Candidate会退回follower 而不是延续身份，因为通过Term信息知道自己过期，意味着自己可能发生了网络隔离等故障，那么在此期间整个Raft集群可能已经有了新的leader、**提交了新的日志**，此时自己的日志是有缺失的，如果不退回follower，那么可能会导致整个集群的日志缺失，不符合安全性。

--------------------------缓冲线---------------------------------------------------

### 3、领导选举的过程
Raft是一个强Leader模型，可以粗暴理解成Leader负责统领follower，如果Leader出现故障，那么整个集群都会对外停止服务，直到选举出下一个Leader。

**问题：**

一、节点之间通过网络通信，其他节点(follower)如何知道leader出现故障？

leader会定时向集群中follower节点发送AppendEntry(作为心跳，heartbeat)以通知自己仍然存活。

那么，如果一段时间内follower没有收到leader发来的AppendEntry，那么follower就会认为当前的leader出现故障，从而发起选举。

**<font style="color:#DF2A3F;">重：</font>**这里**“follower在一段时间内没有接收到Leader发送的AppendEntry“**，在实现上可以利用定时器和一个标志位实现，每到定时时间就检查这期间内有无AppendEntry即可。

**<font style="color:#DF2A3F;">补：</font>**AppeandEntry具体来说有两种主要作用和一个附带作用：

主要作用：

心跳；

携带日志entry及其辅助信息，以控制日志的同步和日志向状态机提交

附带作用：

通告leader的index和term等关键信息以便follower对比确认follower自己或者leader是否过期。



二、follower知道leader出现故障后如何选举出leader?

follower认为leader故障后只能通过：term增加，变成candidate，向其他节点发起RequestVoteRPC申请其他follower的选票，过一段时间之后会发生如下情况：

1.赢得选举(获得大部分的票)，马上成为leader(此时term已经增加了)

<font style="color:rgb(47, 48, 52);">发现有符合要求的leader，自己马上变成follower 了，这个符合要求包括：leader的term≥自己的term。</font>

<font style="color:rgb(47, 48, 52);">2.一轮选举结束，无人成为leader，那么循环这个过程，即：term增加，变成candidate。</font>

 引申出的问题：

        * 如果在选举过程中没有"一半以上"选票的限制，会发生什么？会有什么问题？
        * raft节点的数量要求是奇数，为什么有这个要求？
        * 如果发现一个leader，但是term小于自己会发生什么？

三个问题一次性解决，为了防止同一时间有太多的follower转变为candidate导致一直无法选出leader，Raft采用了随机选举超时的机制，每个candidate在发起选举后，都会随机化一个新的选举超时时间。



3、符合什么条件的节点可以成为Leader？

这一点也成为“选举限制”，有限制的目的是为了保证选举出的leader一定包含了整个集群中目前已经committed(已经提交的所有日志)。

当candidate发送RequestVoteRPC时，会带最后一个entry(条目)的信息。所有节点收到该请求后，都会对比自己的日志，如果发现自己的日志更新一些，则会拒绝投票给该candidate，即自己的日志必须要“不旧于”该发起投票的candidate。

判断日志老旧的方法：

需要比较两个东西：最新日志entry的term和对应的index。index即日志entry在整个日志的索引。

```cpp
if 两个节点最新的日志entry的term不同
    term大的日志新；
else 
  最新日志entry的index大的新；
end
```

这样限制可以保证：成为leader的节点，其日志已经是多数节点中最完备的，即包含了整个集群中所有的committed entries(提交的条目)。



### 4、日志同步、心跳
在RPC中日志同步和心跳是放在一个**RPC(AppendEntryRPC)**中实现的，原因为：

    - 心跳RPC可以看成是没有携带日志的特殊日志同步的RPC。

对于一个follower，如果leader认为其日志已经和自己匹配了，那么在AppendEntryRPC中不用携带日志(再携带日志属于无效信息了，但其他信息依然要携带)，反之如果follower的日志只有部分匹配，那么就需要在AppendEntryRPC中携带对应的日志。

**问题：  
****     **1.为什么不直接让follower拷贝leader的日志？

   leader发送的日志目的是让follower同步自己的日志，当然可以让leader发送自己的全部日志给follower，然后follower接收后旧覆盖自己原有的日志，但是这样就会携带大量无效的日志(因为这些日志follower本身就有)。

因此raft的方式是：先找到日志不匹配的那个点，然后只同步那个点之后的日志。

  	

    2.leader如何知道follower的日志是否与自己完全匹配？

在AppendEntryRPC中携带上entry的index和对应的term(日志的term)，可以通过比较最后一个日志的index和term来得出来某个follower日志是否匹配。



   3.如果发现不完全匹配，那么如何找到哪部分日志是匹配的，那部分日志是不匹配呢？

    l<font style="color:rgb(47, 48, 52);">eader每次发送AppendEntryRPC后，follower都会根据其entry的index和对应的term来判断某一个  日志是否匹配。</font>

<font style="color:rgb(47, 48, 52);">在leader刚当选，会从最后一个日志开始判断是否匹配，如果匹配，那么后续发送AppendEntryRPC就不需要携带日志entry了。</font>

<font style="color:rgb(47, 48, 52);">如果不匹配，那么下一次就发送 倒数第2个 日志entry的index和其对应的term来判断匹配，如果还不匹配，那么依旧重复这个过程，即发送 倒数第3个 日志entry的相关信息</font>

<font style="color:rgb(47, 48, 52);">重复这个过程，知道遇到一个匹配的日志。</font>

---

<font style="color:rgb(47, 48, 52);">话很啰嗦，字也多但是最后一点点内容这个模块就完结了加油。</font>

**<font style="color:rgb(47, 48, 52);">raft日志的两个特点：</font>**

raft对于日志可以保证其具有两个特点：

1. <font style="color:rgb(47, 48, 52);">两个节点的日志中，有两个 entry 拥有相同的 index 和 term，那么它们一定记录了相同的内容/操作，即两个日志匹配</font>
2. <font style="color:rgb(47, 48, 52);">两个节点的日志中，有两个 entry 拥有相同的 index 和 term，那么它们前面的日志entry也相同</font>

<font style="color:rgb(47, 48, 52);">如何保证这两点：</font>

1. <font style="color:rgb(47, 48, 52);">保证第一点：仅有 leader 可以生成 entry</font>
2. <font style="color:rgb(47, 48, 52);">保证第二点：leader 在通过 AppendEntriesRPC 和 follower 通讯时，除了带上自己的term等信息外，还会带上entry的index和对应的term等信息，follower在接收到后通过对比就可以知道自己与leader的日志是否匹配，不匹配则拒绝请求。leader发现follower拒绝后就知道entry不匹配，那么下一次就会尝试匹配前一个entry，直到遇到一个entry匹配，并将不匹配的entry给删除（覆盖）。</font>

---

## 其他理论部分：
### 共识的概念：
共识算法是什么？  
共识是容错分布式系统中的一个基本问题。共识涉及多个服务器对状态机状态**(对本项目而言就是上层的K-V数据库)达成一致。**一旦他们对状态机做出决定**(就是大多数raft机器都有了一样的决定)**，那么这个决定就是最终决定(已经被集群共识的值可以保证后面不会被覆盖，Raft的安全性)。

典型的一致性算法在其大部分服务器可用时保持运行；例如，即使有2台服务器出现故障，如果是5台服务器的集群则可以继续运行。如果更多的服务器出现故障，它们将停止对外提供服务(但永远不会返回不正确的结果)。<font style="color:#DF2A3F;">即小于一半的节点出现故障不会对整个集群的运行造成影响，一半或一半以上的节点出现故障则整一个集群停止对外提供服务。</font>



### 共识算法要满足的性质：
**实际使用系统中的共识算法一般满足以下特性：**

+ 在非拜占庭条件下保证共识的一致性。非拜占庭条件就是可信的网络条件，即与你通信的节点的信息都说真实的，不存在欺骗。
+ 在多数节点存活时，保持可用性。**“多数”永远指的是配置文件中所有节点的多数(简单来说就是总数比如多少个raft节点的机器)**，而不是存活节点的多数。多数等同于超过半数的节点，多数这个概念很重要，贯穿Raft算法的多个步骤。
+ **不依赖于绝对时间。**理解这点要明白共识算法是要应对出现故障的情况，在这样的环境中网络报文也很可能会受到干扰而延迟，如果完全依靠绝对时间，会带来问题，Raft用自定义的Term(任期)作为逻辑时钟来代替绝对时间。
+ 在多数节点一致后就返回结果，而不会受到个别慢节点的影响。这点与第二点联合理解，“只要大多数节点同意操作”就代表整个集群同意该操作。对于Raft来说，“操作”是存储到日志log中，一个操作就是一个log中的一个entry（条目）。



> 更新: 2025-05-21 17:28:13  
> 原文: <https://www.yuque.com/chengxuyuancarl/kkiykz/ygvgeq0ksnc9aau9>