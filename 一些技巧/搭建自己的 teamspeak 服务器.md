---
title: 搭建自己的 teamspeak 服务器
categories:
  - 一些技巧
abbrlink: eb8579d8
date: 2024-08-16 19:46:14
updated: 2024-08-20 14:10:00
---

<meta name="referrer" content="no-referrer"/>

# 前言

teamspeak 是一款开源、轻量的语音软件，这是它的[官网](https://teamspeak.com/en/)。在官网可以下载客户端 client 和服务端 server，国内客户端下载推荐地址 [https://ts3.com.cn/downloads/teamspeak](https://ts3.com.cn/downloads/teamspeak)，后续的下载安装都是针对服务端。为了方便，后文简称 teamspeak 为 ts。

为什么要用 ts 呢？有人会说，用 yy 啊，用 qq、微信啊。qq 和微信语音的质量没法说， yy 很吃电脑配置和带宽，我的电脑比较垃圾，所以想要一个比较轻量的语音软件。这时候 ts 就是一个非常好的选择了。

> 其实 [kook](https://www.kookapp.cn/) 也不错，但是 ts 更轻量，我个人感觉用起来也更舒服。
>
> 啊？你问为什么不用 [discord](https://discord.com/) 。你说为什么，谁打个游戏聊个天还需要翻墙才行？

<!-- more -->

ts 官方提供了服务器租赁的服务，根据租赁时间和服务器最大人数设有不同的价位，国内的话具体[点击这里](https://ts3.com.cn/price)进行查看。但是作为一个码农，既然 ts 是开源的，并且我可以自己下载 server 部署，那我为什么不自己搭建一个 ts 服务器呢？将其搭建在自己的服务器上，不就可以和朋友们一起愉快的玩耍了吗？

> 注意：
>
> 本文需要读者有一台带有公网的机器，最好是 Linux。这里我以我的云服务器 Ubuntu 20.04 为例。我的云服务器通过 [1Panel](https://1panel.cn) 进行管理，所以后续的部分操作会通过 1Panel 面板直接进行，关于命令行或者其他的操作请自行查阅资料。

# 搭建部署

ts 服务端也是一个程序，所以显而易见，部署 ts 服务端的本质就是让服务端程序跑起来。这一点不管是对于本地还是公网都是一样的，唯一区别就在于一个只能本地访问，一个能进入公网访问。

关于部署形式，有很多种。可以直接下载 ts 服务端，配置好运行环境，调好运行参数，然后在云服务器后台挂着跑；也可以通过 docker 进行隔离化部署。当然，这里我们选择通过方便的 docker 进行部署。

## 通过 docker 部署

安装 docker 不是本文重点，可以参考文章 [https://www.cnblogs.com/Liyuting/p/17022739.html](https://www.cnblogs.com/Liyuting/p/17022739.html) 或者自行查阅资料。当然有些可视化面板例如宝塔或者 1Panel 是可以通过面板操作的，这里也不详细阐述。

docker 安装成功以后，通过以下命令部署 ts 服务端：

```bash
docker run -p 9987:9987/udp -p 10011:10011 -p 10022:10022 -p 10080:10080 -p 10443:10443 -p 30033:30033 -p 41144:41144 -e TS3SERVER_LICENSE=accept -e TS3SERVER_SERVERADMIN_PASSWORD=<your_password> --name="teamspeak-server" -d teamspeak
```

关于这些 `-xxx` 参数的含义和规则，可以参考文章[https://blog.csdn.net/weixin_39998006/article/details/99680522](https://blog.csdn.net/weixin_39998006/article/details/99680522)，这里只进行简单的阐述：

- -p：**主机端口:容器内部端口**，代表程序需要用到的端口，当然云服务器需要放开公网端口的防火墙。关于主机端口和容器内部端口，后续会做解释。
- -e：运行程序配置的环境变量，这里非常推荐加上 `TS3SERVER_SERVERADMIN_PASSWORD=<your_password>`，`<your_password>` 改为读者自己想要设置的密码，这个在后续找回 ts 服务器权限 token 的时候非常有用，后文会解释这一点。
- --name：容器名称，方便我们自己管理。
- -d：在后台运行，退出终端也不会结束进程，相当于 nohup 命令。

## docker 被墙了

有人问，现在 docker hub 以及镜像全被墙了，怎么办？本地其实很简单，科学上网就行了。但是云服务器呢？哥们，我咋给云服务器开代理？

网上有很多解决方案，有搭建自己的镜像源的，也有一些可用的镜像源，例如我正在用的 [https://docker.1panel.live](https://docker.1panel.live)，好用爱用。

具体请参考文章 [https://mixuying.com/archives/1719753069678](https://mixuying.com/archives/1719753069678)，详细介绍了如何修改 docker 镜像源。

## 开放防火墙端口

ts 服务器是需要几个 tcp 协议的端口和一个 udp 协议的端口的，具体如下：

这里有一点需要强调的是，docker 容器应用具有容器内端口，通过容器内端口转发到云服务器的主机端口，再对外通信。因为 docker 的本质是一个具有运行环境的轻量隔离化容器，显然具有自己的容器内端口。为了避免可能发生的错误，最好保持这两个端口一致。

|       服务        | 协议 | 主机端口 | 是否必要 |
| :---------------: | :--: | :------: | :------: |
|     语音服务      | UDP  |   9987   |    是    |
|     文件传输      | TCP  |  30033   |    是    |
| 服务器查询（raw） | TCP  |  10011   |   可选   |
| 服务器查询（SSH） | TCP  |  10022   |   可选   |
| 网络查询（http）  | TCP  |  10080   |   可选   |
| 网络查询（https） | TCP  |  10443   |   可选   |
|       TSDNS       | TCP  |  41144   |   可选   |

通过云服务器的安全组策略放开对应的防火墙端口即可。具体如何操作不进行赘述。

值得注意的是，如果你的服务器安装了可视化面板，除了云服务器厂商需要放行一次端口以外，面板可能也需要放行一次。

这样操作以后，整个 ts 服务器就算是搭建成功了，后续进行初始化配置就可以正常使用了。

# 初始化服务器

## 获取管理员 token

当 ts 服务端第一次初始化的时候，会自动生成一个 token 用作管理员权限的激活。查看日志即可获得：

```markdown
------------------------------------------------------------------
                      I M P O R T A N T                           
------------------------------------------------------------------
               Server Query Admin Account created                 
         loginname= "serveradmin", password= "your_password"
         apikey= "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
------------------------------------------------------------------

...

------------------------------------------------------------------
                      I M P O R T A N T                           
------------------------------------------------------------------
      ServerAdmin privilege key created, please use it to gain 
      serveradmin rights for your virtualserver. please
      also check the doc/privilegekey_guide.txt for details.

       token=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
------------------------------------------------------------------
```

`token=` 后面的东西就是激活码了。

值得注意的是，上面一条重要的信息中，给出了一个 loginname 和 password ，这是用作服务器查询的管理员账号，这里的 password 正好是我们安装时候指定的 password ，而不是默认随机生成的，而 loginname 是统一的 serveradmin 。这样对后续的服务器维护操作，特别是需要迁移的时候提供了极大的方便。apikey 我目前没有需要会用到，因此忽略即可。

## 第一次进入服务器

获得 token 以后，就可以通过客户端连接进入 ts 服务器了。连接地址输入自己的服务器公网 IP 即可，如图所示：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20240816192530057.png" alt="image-20240816192530057" style="zoom:67%;" />

连接成功以后，会弹出提示，输入 token 激活获得管理员权限即可。

# 维护服务器

ts 客户端使用 Qt 开发的，功能齐全也非常好用。关于客户端的具体教程可参考[官方文档](https://www.teamspeak3.com/teamspeak-server-support.php)，包含了频道管理、用户管理、个性化服务器、调整自己的设备等。

以上都是关于客户端的一些使用方法，下面主要想讨论关于服务端的一些维护技巧。

## 给服务器绑定域名

众所周知，IP 非常难记，并且随意暴露自己的公网 IP 还有被攻击的风险（虽然我的服务器绑定了域名但是没套 cdn 来隐藏 IP）。因此，可以考虑给 ts 服务器绑定一个域名。

首先需要一个属于自己的域名，国内和国外的服务商都可以考虑。例如你的域名是 abc.com，你想把子级域名 ts.abc.com 绑定在 ts 服务器上。

我们需要添加一条 A 记录和一条 SRV 记录，域名 DNS 解析记录的具体含义请参考文档 [https://zhuoyun233.com/2022/05/10/TeamSpeakGuide/](https://zhuoyun233.com/2022/05/10/TeamSpeakGuide/)。

### A 记录

1. 主机名（Hostname）处填写子域名，如我使用 ts.abc.com 作为服务器的域名，则填写 ts。
2. 地址/值（Address/Value）处填写 ts 服务器的 IP 地址。
3. TTL 设置为 600 秒即可，这是 ts 服务器刷新的时长。我的默认就是 600 秒。

### SRV 记录

1. 以 `_ts3` 作为服务，以 `_udp` 作为协议，以上一条填写的子域名为名称，在主机名（Hostname）处填写 `_ts3._udp.ts`。
2. 在目标主机名（Target Hostname）处填写完整的域名，如 ts.abc.com。
3. 端口（Port）处填写 ts 语音服务端口（默认为 9987）。
4. 权重（Weight）处填写 5。
5. 优先级（Distance/Priority）处填写 0（或 1）。
6. TTL 同上设置为 600 秒即可。

### 结果

设置完毕以后，两条 DNS 解析记录的情况应该类似于这样：

| 类型 | 名称           | 内容/值             | TTL    |
| ---- | -------------- | ------------------- | ------ |
| A    | ts             | 服务器的 IP 地址    | 600 秒 |
| SRV  | `_ts3._udp.ts` | 0 5 9987 ts.abc.com | 600 秒 |

这样等待 DNS 刷新以后就能使用域名进入服务器了。

## server query 查询服务

如果我想要生成一个新的管理员 token 权限码，应该怎么办呢？

首先，如果你本身是具有管理员权限的话，你可以在 ts 客户端直接生成备用 token 权限码，并且可以针对不同的权限生成对应权限的 token。具体教程可以查看官方文档：[TEAMSPEAK SUPPORT :: HOW TO USE A PRIVILEGE KEY IN TEAMSPEAK 3](https://www.teamspeak3.com/support/teamspeak-3-privilege-keys.php)

但是问题来了，假设某一天你的 ts 服务器的 admin 管理员权限莫名其妙的丢了（不排除某位 admin 因为某些原因给你下了，但是联系不上他了），现在你需要重新找回管理员权限，该怎么办呢？

好，现在如果你作为服主，你的管理员权限丢了，不用担心。我们可以通过 ts 服务器 10011 端口的查询服务 server query 以管理员身份登录，生成新的管理员 token 或者重置所有人的权限。当然里面还有更多其他的操作，官方也给出了 [issue](https://community.teamspeak.com/t/how-to-use-the-server-query/25386) 进行讨论，有兴趣请自行查阅。

### 登录

第一步，通过命令行登入 ts 服务器 server query 后台，此处需要使用 telnet 命令。Linux 下可以通过 apt/yum 包管理器安装，Windows 下可以推荐使用 Scoop 包管理器安装。

```bash
telnet 服务器的 IP 地址 10011
```

出现这样的输出信息，代表连接成功：

![image-20240820100020844](https://image.davidingplus.cn/images/2025/01/31/image-20240820100020844.png)

输入 `help` 命令可以看到所有的命令列表，肉眼可见，可以干非常多的事情：

```bash
help

# TeamSpeak 3 Server :: ServerQuery
# (c) TeamSpeak Systems GmbH

# ServerQuery is a command-line interface built into the TeamSpeak 3 Server which
# allows powerful scripting and automation tools to be built based on the exact
# same instruction set and functionality provided by the TeamSpeak 3 Client. For
# example, you can use scripts to automate the management of virtual servers or
# nightly backups. In short, you can perform operations more efficiently by using
# ServerQuery scripts than you can by using a user interface.

# Command Overview:
#    apikeyadd                   | create a apikey
#    apikeydel                   | delete a apikey
#    apikeylist                  | list apikeys
#    banadd                      | create a ban rule
#    banclient                   | ban a client
#    bandel                      | delete a ban rule
#    bandelall                   | delete all ban rules
#    banlist                     | list ban rules on a virtual server
#    bindinglist                 | list IP addresses used by the server instance
#    channeladdperm              | assign permission to channel
#    channelclientaddperm        | assign permission to channel-client combi
#    channelclientdelperm        | remove permission from channel-client combi
#    channelclientpermlist       | list channel-client specific permissions
#    channelcreate               | create a channel
#    channeldelete               | delete a channel
#    channeldelperm              | remove permission from channel
#    channeledit                 | change channel properties
#    channelfind                 | find channel by name
#    channelgroupadd             | create a channel group
#    channelgroupaddperm         | assign permission to channel group
#    channelgroupclientlist      | find channel groups by client ID
#    channelgroupcopy            | copy a channel group
#    channelgroupdel             | delete a channel group
#    channelgroupdelperm         | remove permission from channel group
#    channelgrouplist            | list channel groups
#    channelgrouppermlist        | list channel group permissions
#    channelgrouprename          | rename a channel group
#    channelinfo                 | display channel properties
#    channellist                 | list channels on a virtual server
#    channelmove                 | move channel to new parent
#    channelpermlist             | list channel specific permissions
#    clientaddperm               | assign permission to client
#    clientdbdelete              | delete client database properties
#    clientdbedit                | change client database properties
#    clientdbfind                | find client database ID by nickname or UID
#    clientdbinfo                | display client database properties
#    clientdblist                | list known client UIDs
#    clientdelperm               | remove permission from client
#    clientedit                  | change client properties
#    clientfind                  | find client by nickname
#    clientgetdbidfromuid        | find client database ID by UID
#    clientgetids                | find client IDs by UID
#    clientgetnamefromdbid       | find client nickname by database ID
#    clientgetnamefromuid        | find client nickname by UID
#    clientgetuidfromclid        | find client UID by client ID
#    clientinfo                  | display client properties
#    clientkick                  | kick a client
#    clientlist                  | list clients online on a virtual server
#    clientmove                  | move a client
#    clientpermlist              | list client specific permissions
#    clientpoke                  | poke a client
#    clientsetserverquerylogin   | set own login credentials
#    clientupdate                | set own properties
#    complainadd                 | create a client complaint
#    complaindel                 | delete a client complaint
#    complaindelall              | delete all client complaints
#    complainlist                | list client complaints on a virtual server
#    custominfo                  | display custom client properties
#    customsearch                | search for custom client properties
#    customset                   | add or update a custom client property.
#    customdelete                | remove a custom client property.
#    ftcreatedir                 | create a directory
#    ftdeletefile                | delete a file
#    ftgetfileinfo               | display details about a file
#    ftgetfilelist               | list files stored in a channel filebase
#    ftinitdownload              | init a file download
#    ftinitupload                | init a file upload
#    ftlist                      | list active file transfers
#    ftrenamefile                | rename a file
#    ftstop                      | stop a file transfer
#    gm                          | send global text message
#    help                        | read help files
#    hostinfo                    | display server instance connection info
#    instanceedit                | change server instance properties
#    instanceinfo                | display server instance properties
#    logadd                      | add custom entry to log
#    login                       | authenticate with the server
#    logout                      | deselect virtual server and log out
#    logview                     | list recent log entries
#    messageadd                  | send an offline message
#    messagedel                  | delete an offline message from your inbox
#    messageget                  | display an offline message from your inbox
#    messagelist                 | list offline messages from your inbox
#    messageupdateflag           | mark an offline message as read
#    permfind                    | find permission assignments by ID
#    permget                     | display client permission value for yourself
#    permidgetbyname             | find permission ID by name
#    permissionlist              | list permissions available
#    permoverview                | display client permission overview
#    permreset                   | delete all server and channel groups and
#                                | restore default permissions
#    privilegekeyadd             | creates a new privilege key
#    privilegekeydelete          | delete an existing privilege key
#    privilegekeylist            | list all existing privilege keys on this server
#    privilegekeyuse             | use a privilege key
#    queryloginadd               | add a query client login
#    querylogindel               | remove a query client login
#    queryloginlist              | list all query client logins
#    quit                        | close connection
#    sendtextmessage             | send text message
#    servercreate                | create a virtual server
#    serverdelete                | delete a virtual server
#    serveredit                  | change virtual server properties
#    servergroupadd              | create a server group
#    servergroupaddclient        | add client to server group
#    servergroupaddperm          | assign permissions to server group
#    servergroupautoaddperm      | globally assign permissions to server groups
#    servergroupautodelperm      | globally remove permissions from server group
#    servergroupclientlist       | list clients in a server group
#    servergroupcopy             | create a copy of an existing server group
#    servergroupdel              | delete a server group
#    servergroupdelclient        | remove client from server group
#    servergroupdelperm          | remove permissions from server group
#    servergrouplist             | list server groups
#    servergrouppermlist         | list server group permissions
#    servergrouprename           | rename a server group
#    servergroupsbyclientid      | get all server groups of specified client
#    serveridgetbyport           | find database ID by virtual server port
#    serverinfo                  | display virtual server properties
#    serverlist                  | list virtual servers
#    servernotifyregister        | register for event notifications
#    servernotifyunregister      | unregister from event notifications
#    serverprocessstop           | shutdown server process
#    serverrequestconnectioninfo | display virtual server connection info
#    serversnapshotcreate        | create snapshot of a virtual server
#    serversnapshotdeploy        | deploy snapshot of a virtual server
#    serverstart                 | start a virtual server
#    serverstop                  | stop a virtual server
#    servertemppasswordadd       | create a new temporary server password
#    servertemppassworddel       | delete an existing temporary server password
#    servertemppasswordlist      | list all existing temporary server passwords
#    setclientchannelgroup       | set a clients channel group
#    tokenadd                    | alias for privilegekeyadd
#    tokendelete                 | alias for privilegekeydelete
#    tokenlist                   | alias for privilegekeylist
#    tokenuse                    | alias for privilegekeyuse
#    use                         | select virtual server
#    version                     | display version information
#    whoami                      | display current session info


# error id=0 msg=ok
```

留意最下面的一行，代表了你输入命令的执行状态，`error id=0 msg=ok` 代表没有任何问题，即执行成功。

现在让我们为自己生成一个新的管理员 token。首先需要通过账号密码登录，管理员账号是 `serveradmin`，密码是部署服务器一开始我千叮咛万嘱咐让你配置的密码环境变量。看，在这里就发挥作用了。如果使用默认的密码，又长又难记，并且只会在日志最开头展示一次。如果忘记备份并且日志丢失或损坏，那就完蛋了。

输入以下命令进行登录，如果不出意外，会得到 ok 的结果：

```bash
login serveradmin <your_password>
# error id=0 msg=ok
```

### 找回管理员 token

登录成功以后，需要选择 teamspeak instance 实例。按照前面的流程走下来，我们的 ts 服务端的实例应该就是默认值 1。

```bash
use 1
# error id=0 msg=ok
```

之后就可以生成新的 token 了，输入如下命令：

```bash
tokenadd tokentype=0 tokenid1=6 tokenid2=0
# token=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

如果不输入 `use 1`，可能会出现如下的错误：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20240820101616202.png" alt="image-20240820101616202" style="zoom:70%;" />

### 重置服务器的权限

当然，前面的命令是可能出现问题的，并且还不好解决。那么有没有什么办法可以重置权限呢？

是可以的，通过命令 `permreset`，这样会重置服务器的权限组为默认并重新为你生成一个管理员 token。注意，是权限组的重置，所以服务器的文件或者频道并不会收到任何影响。同时这个命令一般不会出现什么问题，这也是我的下下策。

```bash
permreset
# token=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
# error id=0 msg=ok
```

## 迁移服务器

### 官网的迁移教程

对于我们这些抠门的码农而言，云服务器基本上是一年一换的。哪家有羊毛就薅哪家，所以 ts 服务器的迁移是非常必要的需求。如果每次都重新配置频道和权限，那未免也太麻烦了。当然，ts 服务器当然是支持迁移的。

官网的教程请参考：[I WANT TO MOVE MY SERVER TO ANOTHER MACHINE, WHICH FILES SHOULD I COPY?](https://support.teamspeak.com/hc/en-us/articles/360002713038-I-want-to-move-my-server-to-another-machine-which-files-should-I-copy)

翻译总结一下如下：

1. licensekey.dat：此文件包含你的许可证。你可能没有此文件，在这种情况下，你无需复制它。
2. query_ip_whitelist.txt：查询接口的白名单 IP。
3. query_ip_blacklist.txt：查询接口的黑名单 IP。
4. files/：上传到服务器的任何图标、头像和文件。请务必复制整个文件夹，包括任何子文件夹和其中的文件。
5. ts3server.sqlitedb：数据库此文件最为重要，包含有关虚拟服务器、用户、权限、频道和组等的所有信息。服务器实例及其虚拟服务器的所有设置都包含在此文件中。
6. *.ini：可能存在的包含启动参数和数据库配置的文件。默认情况下不存在这些文件。
7. tsdns/tsdns_settings.ini：TSDNS 服务的配置。仅在你实际使用 TSDNS 时需要此配置。

### 对于我自己和大部分人的情况

对于我个人而言，服务端没有配置什么 TSDNS 服务配置，也没有写入什么 ini 文件。同时我是通过 docker 部署，通过 1Panel 进行管理的。所以上面官网的教程可能不一定适用，因此我做了一定的尝试，摸索出了适合我自己的迁移方法。

让我们查看 docker 中 ts 服务器对应的存储卷的内容，经过验证，除了 logs/ 日志目录的文件不是必要的，其他都需要拷贝到新机器对应的位置。注意这里数据库的文件似乎与官网提到的有冲突。我个人尝试过只拷贝第一个，但是备份失败，因此全部拷过去覆盖即可。

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20240820103802807.png" alt="image-20240820103802807" style="zoom:55%;" />

> 最后，再次回到前面设置密码环境变量的伏笔。这里我们的日志是没有同步的，当然如果你也可以选择直接覆盖日志进行同步。但是问题是，如果不手动设置密码，将服务器做了迁移以后，日志中的密码就不一定正确了，这一点我也不知道为什么，试验出来的结果是这样的。因此将密码牢牢掌握在自己手中，才是最终的解决方案。

做好迁移以后，再次进入服务器，如果不出意外的话，所有的频道和权限都会保留，又可以和朋友继续开心的玩耍了。

# 参考文档

1. [TEAMSPEAK SERVER SUPPORT](https://www.teamspeak3.com/teamspeak-server-support.php)

2. [使用 Docker 搭建属于自己的 TeamSpeak 服务器](https://www.bilibili.com/read/cv22920320/)

3. [Teamspeak 服务器搭建、绑定域名、迁移](https://www.bilibili.com/read/cv36119499/)

4. [TeamSpeak 3 服务器管理员权限丢失了怎么办？7 步帮你找回！](https://www.bilibili.com/read/cv4893931/)

