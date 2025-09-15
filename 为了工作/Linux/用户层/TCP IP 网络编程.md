---
title: TCP IP 网络编程
categories:
  - Linux 学习
  - 用户层
abbrlink: c5690fb7
date: 2024-06-20 23:00:00
updated: 2024-06-20 23:00:00
---

<meta name="referrer" content="no-referrer"/>

# 前言

里面需要很多计算机网络的知识，我也懒得写了，在计算机网络复习当中有写到，这是文章的链接：[计算机网络期末复习](https://blog.davidingplus.cn/posts/b9598422.html)

# 第一章 理解网络编程和套接字

## 理解网络编程和套接字

网络编程的目的就是使两台联网的计算机相互交换数据。首先需要物理连接，这一点已经达到了，基本上所有的电脑都是连通庞大的互联网的，所以这一点不需要担心；在此基础上我们只需要考虑如何使用程序来将两个主机建立连接即可，操作系统会给我们提供名为套接字(`socket`)的部件。

<!-- more -->

### TCP套接字

我们先考虑比较重要的TCP套接字，他是建立连接的，可靠的运输层协议

这是TCP协议的通信过程，我们来看一下其中哪些地方比较重要：

![image-20230818143501064](https://cdn.davidingplus.cn/images/2025/02/01/image-20230818143501064.png)

我们将通信过程总结如下：

~~~cpp
// TCP 通信的流程
// 服务器端 （被动接受连接的角色）
    1. 创建一个用于监听的套接字
        - 监听：监听有客户端的连接
        - 套接字：这个套接字其实就是一个文件描述符
    2. 将这个监听文件描述符和本地的IP和端口绑定（IP和端口就是服务器的地址信息）
    	- 客户端连接服务器的时候使用的就是这个IP和端口
    3. 设置监听，监听的fd开始工作
    4. 阻塞等待，当有客户端发起连接，解除阻塞，接受客户端的连接，会得到一个和客户端通信的套接字(fd)
    5. 通信
        - 接收数据
        - 发送数据
    6. 通信结束，断开连接
~~~

~~~cpp
// 客户端
    1. 创建一个用于通信的套接字（fd）
    2. 连接服务器，需要指定连接的服务器的 IP 和 端口
    3. 连接成功了，客户端可以直接和服务器通信
        - 接收数据
        - 发送数据
    4. 通信结束，断开连接
~~~

**客户端有两个套接字比较重要，一个是用于监听服务端的套接字`listen_fd`，这个套接字用于时刻监听客户端是否连接；另一个是和客户端进行通信的套接字`connect_fd`，这个套接字就是用来进行通信的。套接字就是伪文件描述符，使用方法和文件描述符一样**

### <span id='example'>例子</span>

我们写一个程序来进行TCP本地双方的简单通信：

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_IPV4_STRING_SIZE 16

int main(int argc, char* const argv[]) {
    // 判断命令行参数
    if (argc < 3) {
        printf("usage: %s  <ip>  <port>.\n", argv[0]);
        return -1;
    }

    const char* server_ip = argv[1];
    const unsigned short server_port = atoi(argv[2]);

    // 1.创建socket套接字
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listen_fd) {
        perror("socket");
        return -1;
    }

    // 2.由于客户端需要主动连接，服务端需要绑定一个固定端口
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // ip
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);
    // 端口
    server_addr.sin_port = htons(server_port);

    int ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    printf("server has initalized.\n");

    // 3.开始监听
    ret = listen(listen_fd, 8);
    if (-1 == ret) {
        perror("listen");
        return -1;
    }

    // 4.接受客户端连接
    // 我们就简单的让客户端发送数据，服务端不断接受，并且是p2p
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int connect_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (-1 == connect_fd) {
        perror("accept");
        return -1;
    }

    // 获取客户端信息
    in_port_t client_port = ntohs(client_addr.sin_port);
    char client_ip[MAX_IPV4_STRING_SIZE] = {0};
    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));

    char buf[MAX_BUFFER_SIZE] = {0};
    // 5.开始通信
    while (1) {
        bzero(buf, sizeof(buf));

        int len = recv(connect_fd, buf, sizeof(buf) - 1, 0);
        if (-1 == len) {
            perror("recv");
            return -1;
        }

        if (len > 0)
            printf("recv client(ip: %s,port:%d) message: %s", client_ip, client_port, buf);
        else {  // 写端关闭
            printf("client(ip: %s,port:%d) has closed...\n", client_ip, client_port);
            break;
        }
    }

    // 6.关闭socket
    close(connect_fd);
    close(listen_fd);

    return 0;
}
~~~

~~~cpp
// client.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char* const argv[]) {
    // 判断命令行参数
    if (argc < 3) {
        printf("usage: %s  <ip>  <port>.\n", argv[0]);
        return -1;
    }

    const char* server_ip = argv[1];
    const unsigned short server_port = atoi(argv[2]);

    // 1.创建socket套接字
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == connect_fd) {
        perror("socket");
        return -1;
    }

    // 2.开始连接
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // ip
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);
    // port
    server_addr.sin_port = htons(server_port);

    int ret = connect(connect_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("connect");
        return -1;
    }

    // 3.开始通信
    char buf[MAX_BUFFER_SIZE] = {0};
    // 每隔一秒向服务端发送一次数据
    while (1) {
        static int count = 0;
        bzero(buf, sizeof(buf));
        sprintf(buf, "hello,this is client, count:%d\n", count++);
        printf("send: %s", buf);

        send(connect_fd, buf, strlen(buf), 0);
        sleep(1);
    }

    // 4.关闭连接
    close(connect_fd);

    return 0;
}
~~~

执行结果：

客户端每隔一秒发送一次，服务端正确接收并且打印了出来

服务端

![image-20230903173752565](https://cdn.davidingplus.cn/images/2025/02/01/image-20230903173752565.png)

客户端

![image-20230903173820969](https://cdn.davidingplus.cn/images/2025/02/01/image-20230903173820969.png)

## 基于Linux的文件操作

对Linux而言，socket套接字和文件操作没有区别，或者说socket套接字是一个伪文件，我们完全可以用操作文件的方式去操作它

但是在windows当中，是要区分socket和套接字文件的，在windows当中需要调用特殊的数据传输的相关函数

### 文件描述符(文件句柄)

#### open函数

两种形式

```c++
// 打开一个已经存在的文件
int open(const char *pathname, int flags);

// 创建一个新的文件
int open(const char *pathname, int flags, mode_t mode);
```

##### open函数打开文件

```c++
int open(const char* pathname, int flags);
```

参数解释

- pathname：文件路径
- flags：对文件的操作权限设置，还有其他的设置，例如：O_RDONLY，O_WRONLY，O_RDWR 这三个设置是互斥的
- 返回值：返回一个新的文件描述符(int类型，类似于编号)，如果失败，返回-1

##### open函数创建新文件

```c++
int open(const char *pathname, int flags, mode_t mode);
```

参数解释：

- pathname：创建的文件路径

- flags：对文件的操作权限和其他的设置 
  **必选项：O_RDONLY, O_WRONLY, or O_RDWR 这三个之间是互斥的**
  **可选项：O_CREAT 文件不存在创建新文件；O_APPEND 可以进行文件的追加**
  **flags参数是一个int类型的数据，占4个字节，32位，每一位就是一个标志位，1表示有，0表示没有，所以用按位或**

- mode：八进制的数，表示用户对创建出的新的文件的操作权限，比如：0777
  **3个7分别表示对不同的用户(所有者，组成员，其他用户的权限)的权限，每一个都是3位 ，第一位表示读R，第二位表示写W，第三位 表示可执行X，7对应就是111全有!!!**

  **最终的权限是：mode & ~umask**
  umask可以通过shell命令 umask 查看
  umask的作用是为了抹去某些权限，让我们创建的权限更加合理一些
  例子：0777 & ~0022

关闭函数

~~~cpp
int close(int fd);//  fd：文件描述符 fd
~~~

#### read,write函数

头文件：

```c++
#include <unistd.h>
```

两个函数：

```c++
ssize_t read(int fd, void *buf, size_t count);
```

- 参数：

  - fd：文件描述符，通过open得到的，通过文件描述符操作某个文件
  - buf：缓冲区，需要读取数据存放的地方，数组的地方(传出参数)
  - count：指定的数组的大小

- 返回值：

  - 成功 >0 返回实际读取到的字节数

    ​		**==0 文件已经读取完了(注意是在调用read函数之前文件指针就在末尾了才会返回0，一次性从头读完是会返回读取的字节数的)**

  - 失败 -1 并且修改errno

```c++
ssize_t write(int fd, const void *buf, size_t count);
```

- 参数：
  - fd：文件描述符，通过open得到，通过文件描述符操作某个文件
  - buf：要往磁盘写入的数据
  - count：要写入的实际的大小

- 返回值：
  - 成功 >0 返回实际写入的字节数
     ==0 文件已经写入完了
  - 失败 -1 并且修改errno

程序我就不写了，之前学牛客 Linux的时候写了无数遍了，需要自查

# 第二章 套接字类型与协议设置

## 套接字协议以及数据传输特性

### 关于协议

两个人需要进行远距离通话，必须要先决定通话方式，如果一方使用电话，那么另一方也必须只能使用电话，否则就无法进行正常的通信。对于他们而言，电话就是两个人通话的协议。

**而对于`socket`而言，协议就是计算机之前对话需要具备的通信规则，它由语法，语义，同步三个要素组成；简单理解，协议就是为了完成数据交换而定好的约定**

### 创建套接字socket

`Linux`给我们提供了系统调用`socket`函数来是我们可以创建套接字

~~~cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // 包含了这个头文件，上面两个就可以省略

int socket(int domain, int type, int protocol);
    - 功能：创建一个套接字
    - 参数：
    - domain: 协议族
        AF_INET : ipv4
        AF_INET6 : ipv6
        AF_UNIX, AF_LOCAL : 本地套接字通信（进程间通信）
	// 第二个参数type和第三个参数protocol一个是协议类型，一个是具体的某个协议，划分的还比较细和周到
    - type: 通信过程中使用的协议类型
        SOCK_STREAM : 流式协议(例如：字节流的TCP，当然不一定只有这一种，可以被第三个参数具体指定)
        SOCK_DGRAM : 报式协议(例如：用户数据报的UDP，当然不一定只有这一种，可以被第三个参数具体指定)
    - protocol : 具体的一个协议。一般写0，可以表示默认的或者当协议类型中只有一个具体类型的时候就是用这个具体的类型
        - SOCK_STREAM : 流式协议默认使用 TCP
        - SOCK_DGRAM : 报式协议默认使用 UDP
    - 返回值：
        - 成功：返回文件描述符，操作的区域是内核缓冲区。
        - 失败：-1，并且设置errno
~~~

### 协议族

`socket`函数的协议族可分为如下几类：

我们用的最多的就是`PF_INET`和`PF_LOCAL`，也就是网络上通过`IPV4`，本地通过本地套接字，当然本地也可以用`IPV4`

![image-20230904193622248](https://cdn.davidingplus.cn/images/2025/02/01/image-20230904193622248.png)

### 套接字类型(重要)

套接字类型就是表示套接字的数据传输方式，比如是类似于`TCP`的建立连接可靠的面向字节流的，还是类似`UDP`针对于用户数据报的无连接不可靠的，通过socket函数的第二个参数进行传输，下面我们将针对这两种进行阐述：

#### 面向连接的套接字(SOCK_STREAM)：TCP

面向连接，顾名思义，就是在进行正式数据传输之前需要建立连接，可以理解为在双方之间建立了一条逻辑通道，这种方式书上给出了一个类比的例子，同时图中也给出了他的特点：

![image-20230904194133084](https://cdn.davidingplus.cn/images/2025/02/01/image-20230904194133084.png)

在运输层当中这就对应者TCP协议，TCP协议是面向字节流的建立连接的可靠的传输服务，下面是一些理解：

- **收发双方之间存在缓冲，虽然`write`和`read`函数是不带缓冲的`I/O`函数，但是内核会提供缓冲区，带不带缓冲是针对在用户区有没有缓冲而言的。这个缓冲简而言之就是字节数组，套接字传输的数据并没有直接给到对方，而是在对方的内核的读缓冲区等待读取**
- **关于不存在数据边界的理解：就是我每次传输的数据不一定就是一个完整的数据，可能我指定的写端的缓冲区比较小，我一次写的数据很少，但是我读端一次可以读取很多，写端写了三次，但是读端一次就读完了；这是合理的，读端也能够把数据正确接受，并不会因为数据被分开传输收到就会怎么样，因此我们称为不存在数据边界**
- **数据不会丢失和按序传输数据：`TCP`套接字有累计确认`ACK`的机制，如果我前面的数据没有收到，那么到了一定时间会让发送方进行重传，然后进行累计确认，这就是协议内部具体的实现了，我们目前不需要过于了解，只需知道TCP可靠的机制保证了按序到达即可**

#### 面向消息的套接字(SOCK_DGREAM)：UDP

面向消息，顾名思义，就是强调传输的速度，强调消息传输的及时性，在运输层中对应UDP协议；TCP的传输虽然稳定，但是由于需要建立连接，还有其他的乱七八糟的，效率自然就没有UCP好，虽然面向消息的传输是无连接的面向数据报的不可靠的服务，但是在针对实时性的问题上我们还是选择这个会好一点，比如游戏和视频会议的通信；TCP就可以用在文件的传输上

书上给出了类比图和特点：

![image-20230904195220706](https://cdn.davidingplus.cn/images/2025/02/01/image-20230904195220706.png)

理解：

- **由于没有建立连接，所以发送方发给谁，其实是不知道的，接收方通过套接字收取数据，可能这个时候发送方已经发送了部分数据，接收方就丢失了一部分数据；另外，由于没有连接保证，传输的顺序也是没有保证的，有可能按序到达，也有可能不按序到达，我们本地测试的时候看不出来，但是一旦网络拓扑复杂起来就会出问题了因为数据并不一定是沿一条路径走的；**
- **其次，由于没有连接保证，发送方可不会管接收方接受多少数据，或者是否接收到，发送方就自己发，一旦数据大了，接收方没办法及时接受完全，这部分数据就丢了，这就是坏处，虽然效率提高了，因此需要限制每次传输的数据大小；**
- **关于`UDP`的缓冲区，`UDP`只有一个接收缓冲区，是没有发送缓冲区的，就是说发送方有数据就发出去，不会说接收方缓冲区满了怎么样，因此我们使用UDP通信的时候都不用`read`/`write`函数，都用另一组系统调用`recvfrom`和`sendto`；所以UDP的发送方分批发送的数据，接收方没有办法缓存起来一起接受，因为我们不知道发送方什么时候会继续发送数据，`TCP`如果满了就会等待读完在发送，这也得益于`TCP`的写缓冲区，因此`UDP`具有传输的数据边界**

### 协议的最终选择

第三个参数使用来决定最终使用什么协议，因为确认了协议族和套接字类型之后，仍有可能存在多个数据传输方式相同的协议，但是TCP和UDP这里是唯一的，他们表示如下：

~~~cpp
// TCP
socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // 第三个参数也可以传入0，表示默认用这个

// UDP
socket(AF_INET, SOCK_DGREAM, IPPROTO_UDP);  // 也可以传0
~~~

### 例子

为了证明TCP的套接字是没有数据边界的，我们可以修改上面程序的缓冲区大小，让recv函数和send函数调用次数不同，我们再来看读取结果，[程序点击这里](#example)，下面只写改动的地方：

~~~cpp
// server.cpp
int len = recv(connect_fd, buf, 5, 0);
if (-1 == len) {
    perror("recv");
    return -1;
}

if (len > 0)
    // printf("recv client(ip: %s,port:%d) message: %s", client_ip, client_port, buf);
    printf("%s", buf);
// 这里我让每次读取的最大的长度为5个字节，显然一次没办法接受完数据
// 然后为了避免每次接受数据都有前缀打印，会有干扰，我把前缀删了
~~~

结果：

服务端接收到的数据仍然没有任何问题，说明TCP的数据传输是没有边界的

![image-20230904203131451](https://cdn.davidingplus.cn/images/2025/02/01/image-20230904203131451.png)

# 第三章 地址族与数据序列

## 分配给套接字的IP地址与端口号

**`IP`是`Internet Protocol`(网络协议)的简写，是为了收发网络数据而分配给计算机的值，可以用于唯一标识一台在互联网中的计算机；**

**端口并非赋予计算机的值，而是为了区分不同主机之间的进程，这个和进程号有区别，端口号是为了让不同主机的进程都知道，这样才能正确的进行通信**

### 网络地址

`IP`地址分为`IPv4`和`IPv6`地址，分别由4个字节和16个字节组成，一个字节8个`bit`，所以`IPv4`有32位，`IPv6`有128位；

`IPv6`是为了应对`IPv4`地址耗尽的问题提出的标准，但是目前尚未得到普及

网络地址是为了区分不同网络而设置的`IP`地址，是把`IP`地址的子网号部分设置为0得到的`IP`地址，全部设置为1就是广播地址，这些东西在计网已经学过了，详情请参考计网复习资料的部分

### 网络地址分类与主机地址边界

书上写的很清楚，这里截图：

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20230905105657336.png" alt="image-20230905105657336" style="zoom:80%;" />

当然更详细的分类如下：

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20230624203408369.png" alt="image-20230624203408369" style="zoom: 50%;" />

当然还有一些特殊的IP使用：

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20230624211609006.png" alt="image-20230624211609006" style="zoom: 80%;" />

### 用于区分套接字的端口号

我们已经有了IP用来区分不同的主机，这样可以使我们的数据从一个主机通过庞大的互联网找到另一个主机，但是虽然数据到了主机，但是仅凭这些没有办法传输给应用程序。

**这个时候我们就需要区分套接字的端口号了，端口号就是为了区分同一操作系统内不同套接字而设置的，所以没有办法将一个端口号分配给不同的套接字；**

**同时也是为了区分不同主机之间的应用进程，端口号在设置的时候是双方都知道的，因此可以定位到某一个用于通信的进程；**

**值得注意的是，虽然端口号不能重复，但是`TCP`和`UDP`的套接字不会公用端口号，所以允许重复**

用一个图可以如下形象的表示：

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20230905112425335.png" alt="image-20230905112425335" style="zoom:80%;" />

## 地址信息的表示

### 表示IPv4的结构体

`Unix`对网络编程接口的`socket`地址提供了通用`socket`地址和专用`socket`地址

`socket` 网络编程接口中表示 `socket` 地址的是结构体 `sockaddr`，这个在一般是通用的，其定义如下：

~~~cpp
#include <bits/socket.h>
struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14]; //存储数据，包括IP和端口号的信息
};
typedef unsigned short int sa_family_t;
~~~

我们向内核提供的是这个通用的`socket`地址，但是`socket`地址实际上根据不同的用途可以分为很多种，比如`IPv4`，`IPv6`还有本地套接字，他们都有适用于自己的专用`socket`地址，下图就非常清晰的列出来了，我们在这里主要研究`IPv4`的`socket`地址

![image-20230905154922422](https://cdn.davidingplus.cn/images/2025/02/01/image-20230905154922422.png)

#### sockaddr_in结构体

~~~cpp
#include <netinet/in.h>
struct sockaddr_in {
    sa_family_t sin_family; /* __SOCKADDR_COMMON(sin_) */ // 地址族
    in_port_t sin_port; /* Port number. */ // 16位端口号
    struct in_addr sin_addr; /* Internet address. */ // 存储IPv4地址，是一个结构体，定义在下面
    /* Pad to size of `struct sockaddr'. */
    unsigned char sin_zero[sizeof (struct sockaddr) - __SOCKADDR_COMMON_SIZE -
    sizeof (in_port_t) - sizeof (struct in_addr)];
};

struct in_addr {
	in_addr_t s_addr; // 32位IPv4地址
};
~~~

我们来逐个分析一下这个结构体的成员：

- `sin_family`：**每种协议族对应了一个地址族，其实在实现的时候协议族和地址族宏对应的值是相同的**，所以用哪个都无所谓，以下是对应的协议族和地址族：

![image-20230904193622248](https://cdn.davidingplus.cn/images/2025/02/01/image-20230904193622248.png)

![image-20230905155342817](https://cdn.davidingplus.cn/images/2025/02/01/image-20230905155342817.png)

- `sin_port`：**保存16位端口号，注意传入的时候记得转化为大端网络字节序**
- `sin_addr`：**是一个结构体，保存32位IPv4地址信息，也是按照大端网络字节序保存的**，但是我们不用自己动手，有系统调用(`inet_pton`)帮我们做了
- sin_zero：**没有特殊含义，因为系统接受的是通用的`sockaddr`结构体，我们的`sockaddr_in`结构体的成员大小不一定匹配，需要有一定的额外大小来让两个结构体的大小一致而适配，没有特殊含义**

#### 思考

既然`sockaddr_in`是专门给`IPv4`使用的结构体，那么为什么里面还要有`sin_family`的地址族成员呢？

**这个问题非常好，我们前面说到，需要做到专用`socket`地址和通用`socket`地址的匹配，我通过指针强制类型转换的时候不会发生错误，而通过`socket`地址里面有地址族的成员，因为他不是为某一个协议而服务的，所以为了和它保持一致，专用`socket`地址这里也需要一个地址族成员**

## 网络字节序与地址变换

对于单个字节的数据，在计算机中的保存倒无所谓，计算机是以字节存储数据的；但是如果是多个字节，比如双字节，就分为高字节和低字节，高字节就是高位嘛；那么我们的高字节是存储在内存地址的高处还是低处呢？这就是一个问题了，因此字节序分为了大端和小端字节序

我先给出定义：**小端字节序是低位字节存储在内存的低地址处，高位字节存储在内存的高地址处；大端字节序则相反!!!**

### 例子

我们可以写一个程序来看自己的电脑是大端还是小端字节序

~~~cpp
#include <iostream>
using namespace std;

int main() {
    // 给一个双字节的数据
    short num = 0x1223;

    char* ch = (char*)&num;

    cout << ((*ch == 0x23) ? "小端字节序" : "大端字节序") << endl;

    return 0;
}
~~~

结果：

在我的电脑上是以小端字节序存储的，也就是高位存在高地址的地方

![image-20230905162539547](https://cdn.davidingplus.cn/images/2025/02/01/image-20230905162539547.png)

### 思考

所以问题来了，不同的电脑存储的方式不同，难以得到统一，如果我们不加规范，那么在网络传输信息当中就很容易出现数据错误的问题，这将是致命的；

因此，**在网络中，我们规定都用大端字节序传递数据，称为网络字节序**

### 字节序转换

**`Linux`系统给我们提供了系统`API`可以把主机字节序转化为网络字节序，可以转化`short`类型和`long`类型的值，也就是2个字节和8个字节的转化，为什么没有4个字节的`int`呢？因为2个字节对应16位的端口号，这是因为最初的时候`long`是4个字节，可以用来进行32位的转化，但是不同的系统的long不同，现在的long在大多数的机器上都是8个字节了，但是我们一般用不到，因为转化`IPv4`有其他的接口**

以下是转化字节序的`API`：

~~~cpp
h - host 主机，主机字节序
to - 转换成什么
n - network 网络字节序
s - short unsigned short
l - long unsigned int
~~~

~~~cpp
#include <arpa/inet.h>
// 转换端口 端口号 0-65535，就是16位，因此是 uint16_t
uint16_t htons(uint16_t hostshort); // 主机字节序 - 网络字节序
uint16_t ntohs(uint16_t netshort); // 主机字节序 - 网络字节序
// 转IP IP地址，IPV4是32位，因此是uint32_t
uint32_t htonl(uint32_t hostlong); // 主机字节序 - 网络字节序
uint32_t ntohl(uint32_t netlong); // 主机字节序 - 网络字节序
~~~

所以以后在给`sockaddr_in`结构体初始化的时候，传入的端口号和`IPv4`地址记得转换字节序

#### 例子

我们写一个转化网络字节序的例子

~~~cpp
#include <iostream>
using namespace std;
#include <arpa/inet.h>

int main() {
    unsigned short host_port = 0x1234;
    unsigned long host_addr = 0x12345678;

    unsigned short net_port = htons(host_port);
    unsigned long net_addr = htonl(host_addr);

    // hex流可以让一个数以十六进制输出!!!
    cout << "Host ordered port: 0x" << hex << host_port << endl;
    cout << "Network ordered port: 0x" << hex << net_port << endl;
    cout << "Host ordered address: 0x" << hex << host_addr << endl;
    cout << "Network ordered address: 0x" << hex << net_addr << endl;

    return 0;
}
~~~

结果：

我们看到了主机字节序和网络字节序的区别

![image-20230905164245873](https://cdn.davidingplus.cn/images/2025/02/01/image-20230905164245873.png)

#### 补充

让数据以二进制，八进制，十六进制输出(`c++`)

- **二进制：借助`bitset`库**
- **八进制：借助`oct`流**
- **十六进制：借助`hex`流**

用法如下：

~~~cpp
#include <iostream>
using namespace std;
#include <bitset>

int main() {
    int num = 100;

    cout << bitset<10>(num) << endl; // 0001100100，模板参数10是输出多少位
    cout << oct << num << endl; // 144，oct流
    cout << hex << num << endl; // 64，hex流

    return 0;
}
~~~

## 网络地址的初始化与分配

通常，人们习惯用可读性好的字符串来表示 IP 地址，比如用点分十进制字符串表示 IPv4 地址，以及用 十六进制字符串表示 IPv6 地址。但编程中我们需要先把它们转化为整数（二进制数）方能使用。而记录 日志时则相反，我们要把整数表示的 IP 地址转化为可读的字符串。下面 3 个函数可用于用点分十进制字 符串表示的 IPv4 地址和用网络字节序整数表示的 IPv4 地址之间的转换：

**这个是旧的函数，只能适用于IPv4地址，可以使用但是不建议**

~~~cpp
#include <arpa/inet.h>
// 这个数字转化过来之后是网络字节序，就是大端
in_addr_t inet_addr(const char *cp); 
// 第二个参数是传出参数，保存转换后的结果，返回值 1 成功，0 失败，字符串非法，不设置错误号
int inet_aton(const char *cp, struct in_addr *inp); 
char *inet_ntoa(struct in_addr in);
~~~

**下面这对更新的函数也能完成前面 3 个函数同样的功能，并且它们同时适用 IPv4 地址和 IPv6 地址：(推荐使用这里的新的api)**

我一般写代码只用新的API，就是p开头的

~~~cpp
#include <arpa/inet.h>
// p:点分十进制的IP字符串，n:表示network，网络字节序的整数
int inet_pton(int af, const char *src, void *dst);
    // af:地址族： AF_INET AF_INET6
    // src:需要转换的点分十进制的IP字符串
    // dst:转换后的结果保存在这个里面，是一个传出参数
    // 将网络字节序的整数，转换成点分十进制的IP地址字符串
    // 返回值：1 成功 ；失败 0 或者 -1，0表示传入的点分制字符串不合理invalid，不设置errno；-1表示错误(比如地址族内容不合理)，并且设置errno
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
    // af:地址族： AF_INET AF_INET6
    // src: 要转换的ip的整数的地址
    // dst: 转换成IP地址字符串保存的地方
    // size：第三个参数的大小（数组的大小）
    // 返回值：返回转换后的数据的地址（字符串），和 dst 是一样的
~~~

### INADDR_ANY

**我们在创建服务端的时候可能有多个IP都可以分配给服务端，但是我们不知道用哪一个，这个时候我们可以传入`INADDR_ANY`参数，表示地址只要是我有的都能接受，可以省去一些麻烦**

以下是书上的例子：

![image-20230905165356272](https://cdn.davidingplus.cn/images/2025/02/01/image-20230905165356272.png)

# 第四章 基于TCP的服务端/客户端(1)

## 深入理解TCP，UDP

基于网络协议的套接字可以分为`TCP`套接字和`UDP`套接字，`TCP`是面向连接的，又称为基于字节流的套接字

### TCP/IP协议栈

如图所示，我们的数据收发过程实际上经过了这四个层次的过程：

我们的计算机网络问题并不是仅凭软件就能够解决的，编写软件之前需要构建硬件系统，在此基础上通过软件实现各种算法；因此我们是把一个大问题划分为若干个小问题再逐个击破，这样可以大幅度的提高效率

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20230909150644608.png" alt="image-20230909150644608" style="zoom:80%;" />

###  链路层

链路层是将`IP`数据报封装成为帧并且传输的过程，又叫数据链路层(实际的`bit`传输在物理层)

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20230909151100190.png" alt="image-20230909151100190" style="zoom:80%;" />

### IP层(网络层)

`IP`层(网络层)帮我们解决了在巨大的互联网当中我们怎么如何选择路径还有如何进行传递

`IP`本身是面向消息的，不可靠的协议，但是我们的传输层可以通过`TCP`协议来帮我们实现可靠数据的传输

### TCP/UDP层(运输层)

TCP/UDP是在网络层之上的运输层，TCP协议是面向字节流的可靠的传输服务，它可以帮助不可靠的IP协议变得可靠，他有一系列机制帮助实现他的面向连接和可靠性；UDP协议是面向数据报的不可靠的协议

TCP具体来说是这么为IP的不可靠服务提供可靠性的：

![image-20230909151937552](https://cdn.davidingplus.cn/images/2025/02/01/image-20230909151937552.png)

### 应用层

上面的功能都是不会暴露给用户的，因此我们用户在使用的过程中根本看不见下层的操作；我们需要做的就是使用下层封装出来给我们的`API`，在网络中就是`socket`套接字，我们根据规定的规则编写程序就可以进行通信，这就是一个简单的例子

## 实现基于TCP的服务端/客户端

### 服务端的操作顺序

如下图：

具体的操作在我的另一门课牛客`Linux`当中已经讲的很清楚了，这里给出对应文章的地址：

[第四章：TCP套接字通信（这一篇就够了！）](https://blog.csdn.net/m0_61588837/article/details/132432615)

![image-20230909152233914](https://cdn.davidingplus.cn/images/2025/02/01/image-20230909152233914.png)

我们注意一点的`listen`函数，注意他的第二个参数`backlog`

~~~cpp
int listen(int sockfd, int backlog); // /proc/sys/net/core/somaxconn
    - 功能：监听这个socket上的连接
    - 参数：
        - sockfd : 通过socket()函数得到的文件描述符
        - backlog : 连接请求等待队列的长度，表示最多有多少个连接请求排队，并不是服务端最多可以连接通信的个数，因为出队列之后就可以进行通信，这个时候请求队列减一恢复了
~~~

代码我就不放了，第一章写得非常清楚，[点击这里跳转过去](#example)

### 客户端的操作顺序

客户端一般不需要绑定`bind`，因为服务端绑定是因为客户端需要`IP`和端口号来进行请求连接，所以需要绑定而不是让系统自动给我分配，我们也可以看到在没有绑定的客户端的IP和端口都是系统分配的

![image-20230909153956078](https://cdn.davidingplus.cn/images/2025/02/01/image-20230909153956078.png)

### TCP服务端和客户端的关系

总体流程如下：

![image-20230909154125497](https://cdn.davidingplus.cn/images/2025/02/01/image-20230909154125497.png)

## 实现迭代服务端和客户端

TODO

