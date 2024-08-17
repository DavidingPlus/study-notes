# 搭建自己的 teamspeak 服务器

# 前言

teamspeak 是一款开源、轻量的语音软件，这是它的[官网](https://teamspeak.com/en/)。在官网可以下载客户端 client 和服务端 server，国内客户端下载推荐地址 [https://ts3.com.cn/downloads/teamspeak](https://ts3.com.cn/downloads/teamspeak)，后续的下载安装都是针对服务端。为了方便，后文简称 teamspeak 为 ts。

为什么要用 ts 呢？有人会说，用 yy 啊，用 qq、微信啊。qq 和微信语音的质量没法说， yy 很吃电脑配置和带宽，我的电脑比较垃圾，所以想要一个比较轻量的语音软件。这时候 ts 就是一个非常好的选择了。

> 其实 [kook](https://www.kookapp.cn/) 也不错，但是 ts 更轻量，我个人感觉用起来也更舒服。
>
> 啊？你问为什么不用 [discord](https://discord.com/) 。你说为什么，谁打个游戏聊个天还需要翻墙才行？

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
docker run -p 9987:9987/udp -p 10011:10011 -p 10022:10022 -p 10080:10080 -p 10443:10443 -p 30033:30033 -p 41144:41144 -e TS3SERVER_LICENSE=accept -e TS3SERVER_SERVERADMIN_PASSWORD=<your_password> --name="teamspeak-server
" -d teamspeak
```

关于这些 `- xxx` 参数的含义和规则，可以参考文章[https://blog.csdn.net/weixin_39998006/article/details/99680522](https://blog.csdn.net/weixin_39998006/article/details/99680522)，这里只进行简单的阐述：

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

<img src="https://img-blog.csdnimg.cn/direct/799343eab78047e385bffa6a63fb3aad.png" alt="image-20240816192530057" style="zoom:67%;" />

连接成功以后，会弹出提示，输入 token 激活获得管理员权限即可。

# 给服务器绑定域名

众所周知，IP 非常难记，并且随意暴露自己的公网 IP 还有被攻击的风险（虽然我的服务器绑定了域名但是没套 cdn 来隐藏 IP）。因此，可以考虑给 ts 服务器绑定一个域名。

首先需要一个属于自己的域名，国内和国外的服务商都可以考虑。例如你的域名是 abc.com，你想把子级域名 ts.abc.com 绑定在 ts 服务器上。

我们需要添加一条 A 记录和一条 SRV 记录，域名 DNS 解析记录的具体含义请参考文档 [https://www.freebuf.com/articles/349439.html](https://www.freebuf.com/articles/349439.html)。

## A 记录

1. 主机名（Hostname）处填写子域名，如我使用 ts.abc.com 作为服务器的域名，则填写 ts。
2. 地址/值（Address/Value）处填写 ts 服务器的 IP 地址。
3. TTL 设置为 600 秒即可，这是 ts 服务器刷新的时长。我的默认就是 600 秒。

## SRV 记录

1. 以 `_ts3` 作为服务，以 `_udp` 作为协议，以上一条填写的子域名为名称，在主机名（Hostname）处填写 `_ts3._udp.ts`。
2. 在目标主机名（Target Hostname）处填写完整的域名，如 ts.abc.com。
3. 端口（Port）处填写 ts 语音服务端口（默认为 9987）。
4. 权重（Weight）处填写 5。
5. 优先级（Distance/Priority）处填写 0（或 1）。
6. TTL 同上设置为 600 秒即可。

## 结果

设置完毕以后，两条 DNS 解析记录的情况应该类似于这样：

| 类型 | 名称           | 内容/值             | TTL    |
| ---- | -------------- | ------------------- | ------ |
| A    | ts             | 服务器的 IP 地址    | 600 秒 |
| SRV  | `_ts3._udp.ts` | 0 5 9987 ts.abc.com | 600 秒 |

这样等待 DNS 刷新以后就能使用域名进入服务器了。

# 维护服务器

ts 客户端使用 Qt 开发的，功能齐全也非常好用。关于客户端的具体教程可参考 [https://blog.irain.in/archives/teamspeak3_usage_n_power.html](https://blog.irain.in/archives/teamspeak3_usage_n_power.html)，包含了频道管理、用户管理、个性化服务器、调整自己的设备等。

以上都是关于客户端的一些使用方法，下面主要想讨论关于服务端的一些维护技巧。

## 找回管理员权限

假设某一天 ts 服务器所有人的 admin 管理员权限莫名其妙的丢了（不排除某位 admin 因为某些原因给你下了，但是联系不上他了），现在需要重新找回管理员权限，该怎么办呢？

首先，如果你本身是具有管理员权限的话，TODO

# 参考资料

1. [https://www.bilibili.com/read/cv22920320/](https://www.bilibili.com/read/cv22920320/)
2. [https://www.bilibili.com/read/cv36119499/](https://www.bilibili.com/read/cv36119499/)
3. [https://www.bilibili.com/read/cv4893931/](https://www.bilibili.com/read/cv4893931/)

