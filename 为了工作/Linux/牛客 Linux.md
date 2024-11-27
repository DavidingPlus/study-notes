---
title: 牛客 Linux
categories:
  - Linux 学习
abbrlink: ff333d31
date: 2023-09-21 01:00:00
updated: 2023-09-21 01:00:00
---

<meta name="referrer" content="no-referrer"/>

# 学习路线

![image-20230712153227763](https://img-blog.csdnimg.cn/f9569f3266e1419798595a3d39830138.png)

<!-- more -->

# 第一章 Linux系统编程入门

## GCC

### 什么是GCC

![image-20230712154631108](https://img-blog.csdnimg.cn/eef3f6d24396407783456347cb91d08b.png)

### GCC工作流程

![image-20230712162031153](https://img-blog.csdnimg.cn/635fdd6a0725454d89589b6192faeb9c.png)

预处理命令 **-E**

![image-20230712162645602](https://img-blog.csdnimg.cn/fb62d39c50e5483b8bf451c05c4bd73a.png)

得到的结果：

![image-20230712162728058](https://img-blog.csdnimg.cn/d711885ee18f4bc596f65e29811dc86d.png)

得到汇编代码命令 **-S**

![image-20230712163010950](https://img-blog.csdnimg.cn/6841d02827954bfca698dd056c1be2c5.png)

得到的结果：

![image-20230712163038321](https://img-blog.csdnimg.cn/0ba4d185722049478228d3ecc7e00a97.png)

### 常用参数选项

![image-20230712163407827](https://img-blog.csdnimg.cn/f82a923c7d674387ac7c2e5e60425161.png)

![image-20230712163412996](https://img-blog.csdnimg.cn/9eb6958cbaf34915a8e0b11ea4eb1d64.png)

关于-D：**用于在编译的时候指定一个宏**

```c++
#include <iostream>
using namespace std;

int main() {
    int a = 10;

    // 用 -D 来表示在编译的时候指定一个宏，从而可以输出这句话!!! -DDEBUG
    // 一个用途是：可以在用于调试的时候输出一些信息来检测程序，对程序整体的运行没有太大作用
#ifdef DEBUG
    cout << "FUCK" << endl;
#endif

    cout << "you" << endl;

    return 0;
}

```

关于-On：进行优化

```c++
#include <iostream>
using namespace std;

int main() {
    int b, c, d, e;
    b = 10;
    c = b;
    d = c;
    e = d;

    // 使用 -On 进行优化,比如可以优化成如下
    // int b, c, d, e;
    // b = 10;
    // c = 10;
    // d = 10;
    // e = 10;

    return 0;
}
```

### gcc和g++区别

![image-20230712170016696](https://img-blog.csdnimg.cn/d53dcb933f6344ccbe340321547c0362.png)

![image-20230712170241165](https://img-blog.csdnimg.cn/9276ba915e9942fc9039958c829fc2dc.png)

## 静态库

关于库：

![image-20230712192929996](https://img-blog.csdnimg.cn/290948d435b54861992708b667546872.png)

### 命名规则

<img src="https://img-blog.csdnimg.cn/a38a339b2a374f3899e2eb2e4ad7260b.png" alt="image-20230712193256400" style="zoom:80%;" />

### 制作

![image-20230712193344515](https://img-blog.csdnimg.cn/2d5879b349d74c5c8037ab71b9f80331.png)

制作示例：

现在我想把四则运算的代码打成一个静态库供别人使用

![image-20230712194227453](https://img-blog.csdnimg.cn/3aebc655b00a41b781ea89928e7681d1.png)

**按照上面的操作，先通过gcc生成 .o 文件，也就是 -c 到编译汇编，但是不链接的状态，这就得到了各个文件的.o**

![image-20230712194715642](https://img-blog.csdnimg.cn/0ff0b7ee50c248bb9e4147fe57a63efd.png)

### 使用

在实际的开发环境中，代码的结构往往是这样的

![image-20230712200023339](https://img-blog.csdnimg.cn/fe0d970bd18441a6866acf5c3c678ebb.png)

**include文件夹包含相应库当中实现的头文件，lib就存放对应的库，src存放源码，这个对程序的运行没有影响，可以理解为就是工作目录**

**在实际编译运行程序的过程中，不仅需要提供include下的头文件，也需要提供lib下相对应实现的库**

这就需要在g++或者gcc编译的过程中加入参数了

![image-20230712200254526](https://img-blog.csdnimg.cn/8b9608825f674a11a3c8fe9d1136f829.png)

直接编译会导致找不到头文件，因为这个时候head.h和main.c不在同一目录，所以需要用 -I 来包含头文件搜索的目录

![image-20230712200359114](https://img-blog.csdnimg.cn/82e075647b814e69896ff02e05481b8d.png)

在包含了头文件之后，发现里面的函数进行了声明但是没有实现，这个时候就需要引用库文件了

![image-20230712200507667](https://img-blog.csdnimg.cn/1cbf441a2a73417eb870e5f0a7ade039.png)

**calc是库的名称，libcalc.a是我们认为要求的库文件的名称**

![image-20230712200537940](https://img-blog.csdnimg.cn/7dda5ca7d675471487fd3f4316ae5fa2.png)

## 动态库

### 命名规则

![image-20230712202308744](https://img-blog.csdnimg.cn/b8823ef300404e27985d012cd6c93286.png)

### 制作

**得到与位置无关的代码 -fpic/-fPIC (这个 / 是或者的意思，就是两个任选一个写都可以)**

![image-20230712202505969](https://img-blog.csdnimg.cn/a15d2c9d9d77439593e2f84d092f7de3.png)

![image-20230712204636658](https://img-blog.csdnimg.cn/ce5b7e3631644c5994bc89bf1d7ba362.png)

### 使用

和静态库的使用方法一致，但是这里会出现动态库加载失败的问题

![image-20230712204703427](https://img-blog.csdnimg.cn/609390f6cfec4a98ba31e7d7c15fd56a.png)

![image-20230712204335700](https://img-blog.csdnimg.cn/24f43f022049421392ceba95dbb8d6ae.png)

### 加载失败及其解决

![image-20230712205553942](https://img-blog.csdnimg.cn/00a02b33a76647ec9a6f30926559f328.png)

**ldd命令(找到动态库的依赖关系)**

可以看出libcalc.so，我们自己写的动态库找不到依赖，显然无法执行

![image-20230712210027653](https://img-blog.csdnimg.cn/ef39db6d45584c3ea251968eec16b97c.png)

**/lib64/ld-linux-x86-64.so.2 这个是系统提供的动态载入器，用来获取依赖库的绝对路径并且装入到内存当中，这样程序就不会报错了**

#### **如何解决**

##### DT_RPATH段无法修改

##### 通过在环境变量 LD_LIBRARY_PATH 中进行添加

```bash
export LD_LIBRARY_PATH = $LD_LIBRARY_PATH:/mnt/d/Code/Code-C++/深入学习/Linux方向/牛客网课程/第一章-Linux开发环境搭建/04/library/lib
```

export就是修改环境变量的意思，$获取原先的环境变量，:表示在后面添加新的绝对路径，这里把我们的路径添加进去就好了

添加完之后就可以正常运行了

![image-20230712212247869](https://img-blog.csdnimg.cn/dc3dc137792c464c82a8ca7102b288e3.png)

**但是注意：这个环境变量的配置只是暂时的，当终端关闭环境变量也就消失了，每次需要重新配置**

所以需要永久级别的配置

- 用户级别

**通过 .bashrc 来进行配置**

![image-20230712212541900](https://img-blog.csdnimg.cn/7bb4330ad3c1484799cce31b49a074a7.png)

vim .bashrc 进入并修改

![image-20230712212710023](https://img-blog.csdnimg.cn/ba1eb0c53a9a481a8ff72cbf7f63aa4a.png)

在里面添加一行表示配置好了

**完事之后进行更新，以使其应用生效**

![image-20230712212817883](https://img-blog.csdnimg.cn/7270199f5fe44c32af92e84f4b3c7a56.png)

之后也能正常运行

- 系统级别

 在这个文件( /etc/profile )当中进行添加，需要sudo权限

![image-20230712213659601](https://img-blog.csdnimg.cn/93a3212cddb546f794542fd6b2849517.png)

同样加上这句话

![image-20230712213823973](https://img-blog.csdnimg.cn/c6fe37872a4e4be99b326770601a982a.png)

然后保存更新

![image-20230712213917346](https://img-blog.csdnimg.cn/50eef56246584bb19ed950a86fe39a4f.png)

然后同样能正常执行

##### 修改 /etc/ld.so.cathe 文件列表

通过这个文件进行配置 /etc/ld.so.conf

![image-20230712214441428](https://img-blog.csdnimg.cn/cd48e6a7374a4420afc5e6f9f9aa2f81.png)

然后把路径放到这个文件里面就好了

![image-20230712214549726](https://img-blog.csdnimg.cn/2fb8e2cfb2ac4b6398caa39e584338c4.png)

然后进行更新

![image-20230712215334405](https://img-blog.csdnimg.cn/b0e55ec196484620b5865ba16b2960cd.png)

也能正常运行

##### 将动态库文件放到 /lib 或者 /usr/lib 目录下

不建议使用，因为这两个目录中已经放了很多文件，再放入我们自己的文件不利于管理，并且由于可能我们自己的文件和系统文件重名，有可能造成替换然后造成出错

## 静态库和动态库的对比

### 程序编译成可执行程序的过程

**静态库和动态库都是在链接阶段起作用**

![image-20230713095340490](https://img-blog.csdnimg.cn/5834f9a862b44b7fa40c807ec6a5adff.png)

### 静态库制作过程

![image-20230713095639181](https://img-blog.csdnimg.cn/8638c1bbd52f4b7b958eec3250232b03.png)

### 动态库制作过程

![image-20230713100533853](https://img-blog.csdnimg.cn/888348041b7140fb8700decae7c8e0f4.png)

### 静态库的优缺点

![image-20230713100855560](https://img-blog.csdnimg.cn/6737d4307dd54af5894f6a4d7afb3638.png)

### 动态库的优缺点

![image-20230713101528981](https://img-blog.csdnimg.cn/da47622c4c5f4ac0aae40403cfb6cdb5.png)

## makefile

### 什么是makefile

**主要是为了方便进行自动化编译，因为实际的开发过程中代码的存放位置是有规定的，不一定都在同一个目录，这样就导致可能文件依赖出问题，找不到这种，还有就是某个文件依赖于另一个文件，这就需要要求哪些文件需要先编译，哪些文件后编译，这样就需要一个makefile文件来全自动化编译**

![image-20230713102059928](https://img-blog.csdnimg.cn/591b005d2f3542cca00362ecad560748.png)

### 命名规则

提供一个或者多个规则

![image-20230713103610886](https://img-blog.csdnimg.cn/b6d84aa8c85a48348da0e0697a716d1b.png)

### 简单案例

创建Makefile文件

![image-20230713103811609](https://img-blog.csdnimg.cn/7c1dd0ac8a7144a9ad0106a79fe85ead.png)

编写

![image-20230713104023510](https://img-blog.csdnimg.cn/570dd7bfdc69479fb55200ff20638ba4.png)

**第一行app：生成的目标，: 后面是生成目标所需要的依赖文件**

**第二行(需要Tab缩进)：通过执行该命令生成目标**

![image-20230713104408213](https://img-blog.csdnimg.cn/26776604c6264e6881a30534dc7c10a0.png)

### 工作原理

![image-20230713105602304](https://img-blog.csdnimg.cn/5de61aa07fe74283a949cf17b67b1204.png)

**makefile可以检测更新，就是我执行命令的时候，会看上次目标的生成时间和现在的依赖文件的时候是否匹配，不匹配则更新了，于是重新执行相对应的代码然后更新目标文件**

举个例子来说明：

![image-20230713105556164](https://img-blog.csdnimg.cn/1746b7b0fc704a10ab4adb12a3bc2411.png)

**现在我的目标是app文件，然后我写的依赖是这些 .o 文件，也就是编译了但未链接的可执行文件**

**然后这些文件按理来说是找不到的，所以需要在下面作为目标文件被已有的依赖来进行命令生成**

**最下面的clean是makefile文件最好都有的，清理编译过程中生成的 *.o 和 app 文件**

**这样做的好处是：当某个源文件进行修改之后，在执行make命令的时候，其他的文件不用重新编译，提高了效率**

### 变量

**自动变量只能在规则的命令当中使用!!!**

![image-20230713113345866](https://img-blog.csdnimg.cn/f693d7999b0449d2bcf7964350727cc8.png)

### 模式匹配

**把一些格式差不多的规则用通式写出来**!

![image-20230713113808672](https://img-blog.csdnimg.cn/f7ed660e900e4fa08b5a0422ee4531dd.png)

### 函数

**wildcard这个函数只能用来获取 .c 文件**

![image-20230713132533052](https://img-blog.csdnimg.cn/26ad23b0aef84a97acc0252c04682099.png)

**用patsubst 来将 .c 文件替换为 .o文件**

![image-20230713132756923](https://img-blog.csdnimg.cn/1297dc9c4bf04160ba88e56c543d26e1.png)

举例：这样就可以把上面的例子优化成这样

<img src="https://img-blog.csdnimg.cn/74b6e8d237dd402db2d346a8b87045e1.png" alt="image-20230713133926048" style="zoom:80%;" />

遇到c++可以这么写：

<img src="https://img-blog.csdnimg.cn/6fe4fb6e71db4ea9a3e62c4c9da4057c.png" alt="image-20230713140259470" style="zoom:80%;" />

**注意：patsubst里面几个逗号之间不能用空格!!!!(否则出错)**

## GDB调试

### 什么是GDB

![image-20230713140857081](https://img-blog.csdnimg.cn/787966f3de1b46afa416b45df36dffe6.png)

### 准备工作

**gdb是调试可执行程序的，所以我们需要先编译文件成为一个可执行程序**

![image-20230713141622040](https://img-blog.csdnimg.cn/6a86d47c75964c6f8dd3d823ee332772.png)

示例

**-g 保证了gdb能找到源文件**

![image-20230713143121240](https://img-blog.csdnimg.cn/22d5a35849f846538b949cf2070280a1.png)

### GDB命令

**gdb 启动的是可执行程序!!!**

![image-20230713142329930](https://img-blog.csdnimg.cn/158817cf28304218b5be55b871187cb6.png)

示例：

![image-20230713143240623](https://img-blog.csdnimg.cn/a545facc80ac470a862f7ca28a5e1d50.png)

#### 查看 list

**list命令**

![image-20230713143813271](https://img-blog.csdnimg.cn/7d7758c91b214dac974c55a41f8a46fd.png)

**查看别的文件**

先把这三个cpp文件编译连接成为一个可执行文件，加上-g -Wall

![image-20230713144716128](https://img-blog.csdnimg.cn/7fe35b0ed2704502adcdf87a5297035d.png)

进入gdb，默认查看的是main.cpp，现在我看bubble.cpp

![image-20230713144828842](https://img-blog.csdnimg.cn/603e156247d4459a87ecda722acc3b91.png)

#### 断点操作 break

![image-20230713150206430](https://img-blog.csdnimg.cn/d8d6acf8da2f49bf9244aaf445317a16.png)

**示例：**

![image-20230713150556682](https://img-blog.csdnimg.cn/f4ff475d192342708e4d01cc45766e67.png)

**在其他文件打断点**

![image-20230713150932690](https://img-blog.csdnimg.cn/e89264f750014dffa0674976bbc18f90.png)

**设置断点无效或者有效**

![image-20230713151406288](https://img-blog.csdnimg.cn/b7dbf0a4388f45b198a4d26d1e2df0e2.png)

**设置条件断点(一般用在循环的位置)**

![image-20230713151623439](https://img-blog.csdnimg.cn/8add970e212d430aa52dc2b6542b6f17.png)

#### 调试命令

![image-20230713152020106](https://img-blog.csdnimg.cn/d82d4fbea15744fa8aaddbbdd01c9771.png)

示例：

start

![image-20230713152439462](https://img-blog.csdnimg.cn/867f92ab39594fcfbe197348bcc5e0b1.png)

run，next，step

![image-20230713153024581](https://img-blog.csdnimg.cn/3aa7d5acf6a745bf8805e27a45ff40fe.png)

![image-20230713153323313](https://img-blog.csdnimg.cn/758e800265c54fb78d7bfd67653ba59b.png)

**next不会进入函数体，step会进入函数体**

![image-20230713153540431](https://img-blog.csdnimg.cn/d2aea7d459a74f0bb70d46a9cebef5e9.png)

**自动变量操作**

**这样每次执行代码都可以打印变量的值**

![image-20230713154539740](https://img-blog.csdnimg.cn/02e27106283d4b8ca6d510478abb2b7a.png)

## 文件IO

### 标准C库IO函数

**使用标准C库的IO函数开发的程序在任意平台上都可以运行**

**标准C库的IO函数的效率更高，因为当中带有缓冲区(设置在内存当中)**

![image-20230714100055222](https://img-blog.csdnimg.cn/cbed27855863488ba716f7a57d9f23c0.png)

标准C库的IO和Linux系统IO的关系

**调用这两个标准C库IO函数的时候，会让数据进入设置在内存当中的缓冲区IO buffer，然后通过系统提供的API例如write和read来进行内存到磁盘的读写操作；如果直接使用Linux系统IO函数，则不会经过内存中的缓冲区**

![image-20230714101445015](https://img-blog.csdnimg.cn/07ba3e1cc3084b848bf30cdf1fb53905.png)

### 虚拟地址空间

**堆空间是从下往上存，也就是低地址到高地址；**

**栈空间是从上往下村，也就是高地址到低地址**

![image-20230714102646385](https://img-blog.csdnimg.cn/842df24d5a0348f497cd10d7aec446b2.png)

### 文件描述符

![image-20230714105536343](https://img-blog.csdnimg.cn/c46fc363a96e454ea5eb9ca93b562a17.png)

### Linux系统IO函数

![image-20230714110840838](https://img-blog.csdnimg.cn/137e5ab52aab4940971c197c9356b5db.png)

### open函数

两种形式

```c++
// 打开一个已经存在的文件
int open(const char *pathname, int flags);

// 创建一个新的文件
int open(const char *pathname, int flags, mode_t mode);
```

#### open函数打开文件

```c++
int open(const char* pathname, int flags);
```

参数解释

- pathname：文件路径
- flags：对文件的操作权限设置，还有其他的设置，例如：O_RDONLY，O_WRONLY，O_RDWR 这三个设置是互斥的
- 返回值：返回一个新的文件描述符(int类型，类似于编号)，如果失败，返回-1

关于错误errno

**当打开文件错误的时候，系统会自动将错误号赋值给errno**

errno属于linux系统函数库，库里面一个全局变量，记录错误号，记录的是最近的错误号

比如这里open函数失败了，系统会自动把错误号赋值给errno

如何打印错误信息？perror()

```c++
 void perror(const char *s);
```

打印error对应的错误描述

s参数：用户描述，比如hello，最终输出的内容是 hello:xxx(实际的错误描述)

![image-20230714113744182](https://img-blog.csdnimg.cn/40f38a8bc7bc41d9a3d492ef8ec2533d.png)

代码：

```c++
#include <iostream>
using namespace std;

// 使用linux系统IO open()函数的头文件
#include <fcntl.h>  //函数的声明文件
#include <sys/stat.h>
#include <sys/types.h>

// 使用 close()函数头文件
#include <unistd.h>

int main() {
    // 打开
    int fd = open("a.txt", O_RDONLY);

    if (fd == -1)  // 调用错误
        perror("open");

    // 关闭
    close(fd);

    return 0;
}
```

关闭函数

```c++
int close(int fd);//  fd：文件描述符 fd
```

#### open函数创建新文件

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

```c++
#include <iostream>
using namespace std;

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    // 创建一个新的文件
    int fd = open("1.txt", O_RDWR | O_CREAT, 0777);  // 标签之间要用按位或

    if (fd == -1)
        perror("create");

    close(fd);

    return 0;
}
```

**用shell命令 ll 来查看文件的权限**

![image-20230714134141808](https://img-blog.csdnimg.cn/98c112fa731b4bafa1c10f26ed2f75ab.png)

![image-20230714134158964](https://img-blog.csdnimg.cn/2c733f9e6e164485805c8ab72505d0d8.png)

### read,write函数

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

    		**==0 文件已经读取完了(注意是在调用read函数之前文件指针就在末尾了才会返回0，一次性从头读完是会返回读取的字节数的)**

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

代码：

```c++
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SIZE 1024

int main() {
    // 通过open打开english.txt文件
    int fd_src = open("english.txt", O_RDONLY);
    if (fd_src == -1) {
        perror("open");
        return -1;
    }

    // 创建新的文件
    int fd_dest = open("cpy.txt", O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (fd_dest == -1) {
        perror("create");
        return -1;
    }

    // 频繁的读写操作
    char buf[SIZE] = {0};
    int len = 0;
    while ((len = read(fd_src, buf, sizeof(buf))) > 0)
        // 在循环的条件中进行读操作，在循环体中进行写入
        write(fd_dest, buf, len);

    // 关闭文件
    close(fd_src);
    close(fd_dest);

    return 0;
}
```

### lseek函数

在标准C库里面也有一个非常相似的函数fseek()，我甚至怀疑在linux平台下他的底层是不是调用的是fseek()

```c++
//标准C库的函数
#include <stdio.h>

int fseek(FILE *stream, long offset, int whence);

//Linux系统函数
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

- 参数：

  - fd：文件描述符，通过open得到，通过这个fd操作某个文件
  - offset：off_t(long别名) 偏移量
  - whence：

  		 \- SEEK_SET 设置文件指针的偏移量，从头开始

      	- SEEK_CUR 设置偏移量：当前位置 + 第二参数offset的值
	    	
		 	- SEEK_END 设置偏移量：文件大小 + 第二参数offset的值

- 返回值：返回文件指针设置之后的位置

- 作用：

  	1.移动文件指针到头部 lseek(fd,0,SEEK_SET);
  	
  	2.获取当前文件指针的位置 lseek(fd,0,SEEK_CUR);
  	
  	3.获取文件长度 lseek(fd,0,SEEK_END);
  	
  	4.拓展文件的长度，当前文件10B，增加100B，增加了100个字节 lseek(fd,100,SEEK_END);
  	
  	**为什么能扩展？**
  	
  	**因为我们把文件指针移动到了本来不属于文件的位置上去，系统就进行了扩展，**
  	
  	**不过一定要一次写的操作。迅雷等下载工具在下载文件时候先扩展一个空间，然后再下载的。**

代码：

```c++
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int fd = open("hello.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    int ret = lseek(fd, 100, SEEK_END);
    if (ret == -1) {
        perror("lseek");
        return -1;
    }

    // 写入一个空数据
    write(fd, " ", 1);

    close(fd);

    return 0;
}
```

### stat,lstat函数

```c++
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>

    int stat(const char *pathname, struct stat *statbuf);
//作用：获取一个文件的相关的信息
//参数：
    //pathname：操作的文件路径
    //statbuf：结构体变量，传出参数，用于保存获取到的文件信息
//返回值：
    //成功 0
    //失败 -1，并且修改errno

    int lstat(const char *pathname, struct stat *statbuf);
//参数，返回值同上
```

里面有一个stat结构体变量，他的结构如下：

#### stat结构体(!!!)

![image-20230714152755154](https://img-blog.csdnimg.cn/c153a5adb208413183b065b43bdb0580.png)

比较重要的是mode_t类型的 st_mode变量：**就是下面的16位数(01)**

**之前创建文件open函数的第三个参数也是这个类型，只不过里面只用了User,Group,Others这三个**

![image-20230714153204023](https://img-blog.csdnimg.cn/029d299ee6be4dd7ac220ccb1e41f3b5.png)

**判断后面某一位是否为1，也就是比如User是否具有r权限，就用st_mode变量(他就是这16位数)与其相与**

**判断文件类型：由于文件类型在前四位当中可能不止一位为1，那么联系计网，与掩码相与看是不是和这个标识相同就知道了!!!**

#### 区别

软链接文件

![image-20230714154550861](https://img-blog.csdnimg.cn/82764aa7f37f43939311be3eb5ba41f9.png)

**就是说 2.txt 是指向 1.txt 文件的!!!但是它本身的大小是5个字节**

**lstat()就是用来获取该软链接文件本身的信息的，而不是获取他指向的文件的信息，如果用stat()函数就会获得指向的文件，这里就是1.txt的信息**

代码：

```c++
#include <iostream>
using namespace std;
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    struct stat statbuf;

    int ret = stat("1.txt", &statbuf);
    if (ret == -1) {
        perror("stat");
        return -1;
    }

    cout << "size: " << statbuf.st_size << endl;

    ret = stat("2.txt", &statbuf);
    if (ret == -1) {
        perror("stat");
        return -1;
    }

    cout << "size: " << statbuf.st_size << endl;

    ret = lstat("2.txt", &statbuf);
    if (ret == -1) {
        perror("stat");
        return -1;
    }

    cout << "size: " << statbuf.st_size << endl;

    return 0;
}
```

**在shell终端中可以用 stat 命令来获取文件的信息，这里的文件信息就是本文件了，就不是软链接文件的指向文件**

<img src="https://img-blog.csdnimg.cn/bc205610c7b8443abcb1c4193344b918.png" alt="image-20230714154900243"  />

### 模拟实现ls -l 命令

**里面有很多库函数的调用，这些大概知道就行，开发的时候去查文档就好了**

![image-20230715104004715](https://img-blog.csdnimg.cn/d86092f52a414673901be09c3962b250.png)

第一个字符( ’-‘ )是文件类型，后面一堆rwx是读写权限

后面 1 是硬连接数量

在后面两个是文件所有者和文件所在组

然后是文件大小

然后是上一次的修改时间

最后就跟着文件的名称

代码：

~~~cpp
#include <ctime>
#include <iostream>
#include <string>
using namespace std;
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXSIZE 1024

// 模拟实现 ls -l 这个指令
// -rwxrwxrwx 1 lzx0626 lzx0626 107 Jul 15 09:40 ls-l.cpp

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        cout << "usage: " << argv[0] << " <filename>" << endl;
        return -1;
    }

    struct stat statbuf;

    // 通过stat()函数获取文件的信息
    int ret = stat(argv[1], &statbuf);
    if (ret == -1) {
        perror("stat");
        return -1;
    }

    // 获取文件类型和文件权限 st_mode变量
    string perms;  // 保存文件类型和权限的字符串
    mode_t _mode = statbuf.st_mode;
    // 获得文件类型和掩码 -S_IFMT 相与
    switch (_mode & S_IFMT) {
    case S_IFSOCK:
        perms.append("s");
        break;
    case S_IFLNK:
        perms.append("1");
        break;
    case S_IFREG:
        perms.append("-");
        break;
    case S_IFBLK:
        perms.append("b");
        break;
    case S_IFDIR:
        perms.append("d");
        break;
    case S_IFCHR:
        perms.append("c");
        break;
    case S_IFIFO:
        perms.append("p");
        break;
    default:
        perms.append("?");
        break;
    }

    // 判断文件访问权限 Users Group Others
    // Users
    perms.append((_mode & S_IRUSR) ? "r" : "-");
    perms.append((_mode & S_IWUSR) ? "w" : "-");
    perms.append((_mode & S_IXUSR) ? "x" : "-");
    // Group
    perms.append((_mode & S_IRGRP) ? "r" : "-");
    perms.append((_mode & S_IWGRP) ? "w" : "-");
    perms.append((_mode & S_IXGRP) ? "x" : "-");
    // Others
    perms.append((_mode & S_IROTH) ? "r" : "-");
    perms.append((_mode & S_IWOTH) ? "w" : "-");
    perms.append((_mode & S_IXOTH) ? "x" : "-");

    // 获取硬连接数
    nlink_t link_num = statbuf.st_nlink;

    // 文件所有者
    // 这个函数可以通过用户uid获得用户名称
    string _User = getpwuid(statbuf.st_uid)->pw_name;

    // 文件所在组
    // 这个函数通过组gid获得名称
    string _Group = getgrgid(statbuf.st_gid)->gr_name;

    // 文件大小
    off_t _size = statbuf.st_size;

    // 获取修改时间
    // ctime()函数可以将时间差值转化为本地时间
    string _mtime = string(ctime(&statbuf.st_mtime));
    // 这个时间格式化之后回车换行了，将其去掉
    _mtime.pop_back();

    // 输出
    char ret_buf[MAXSIZE];
    // 这个函数可以往字符串中填充
    sprintf(ret_buf, "%s %ld %s %s %ld %s %s", perms.c_str(), link_num, _User.c_str(), _Group.c_str(),
            _size, _mtime.c_str(), argv[1]);

    cout << ret_buf << endl;

    return 0;
}
~~~

里面有一个系统函数对应的功能已经列出了，下面罗列他们的头文件

~~~cpp
#include <pwd.h>
struct passwd *getpwuid(uid_t uid);
~~~

~~~cpp
#include <grp.h>
struct group *getgrgid(gid_t gid);
~~~

~~~cpp
#include <time.h>
char *ctime(const time_t *timep);
~~~

### 文件属性操作函数

四个函数

![image-20230715110918357](https://img-blog.csdnimg.cn/f2fccb0195ad4c5c986e78e727b197ec.png)

#### access函数

```cpp
#include <unistd.h>
int access(const char *pathname, int mode);
// 作用：用来判断某个文件是否有某个权限，或者判断文件是否存在
// 参数：
// pathname：文件路径
// mode：
    // R_OK 是否有读权限
    // W_OK 是否有写权限
    // X_OK 是否有执行权限
    // F_OK 文件是否存在
// 返回值：
// 成功 返回0
// 失败(没有这个权限) 返回-1，并且修改errno
```

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    int ret = access("1.txt", F_OK);
    if (ret == -1) {
        perror("access");
        return -1;
    }

    cout << "file exists." << endl;

    return 0;
}
~~~

#### chmod函数

~~~cpp
#include <sys/stat.h>
int chmod(const char *pathname, mode_t mode);
// 作用：修改文件权限
// 参数：
//     pathname：文件路径
//     mode：需要修改的权限值，八进制的数
// 返回值：
//     成功返回0
//     失败返回-1
~~~

```cpp
#include <iostream>
using namespace std;
#include <sys/stat.h>

int main() {
    int ret = chmod("1.txt", 0777);
    if (ret == -1) {
        perror("chmod");
        return -1;
    }

    return 0;
}
```

#### chown函数(了解)

```cpp
#include <unistd.h>

int chown(const char *pathname, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int lchown(const char *pathname, uid_t owner, gid_t group);
```

#### truncate函数

~~~cpp
#include <sys/types.h>
#include <unistd.h>

int truncate(const char *path, off_t length);
// 作用：缩减或者扩展文件尺寸到达指定的大小
// 参数:
    // path：文件路径
    // length：需要最终文件变成的大小
// 返回值：
    // 成功 0
    // 失败 -1
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <sys/types.h>
#include <unistd.h>

int main() {
    int ret = truncate("b.txt", 5);
    if (ret == -1) {
        perror("truncate");
        return -1;
    }

    return 0;
}
~~~

### 文件目录操作函数

![image-20230715113517243](https://img-blog.csdnimg.cn/ab7b2f9ec0244f8394378f614c113a24.png)

#### mkdir函数

~~~cpp
    #include <sys/stat.h>
    #include <sys/types.h>

    int mkdir(const char *pathname, mode_t mode);
// 作用：去创建一个目录
// 参数：
//     pathname：目录名称
//     mode：权限，八进制数
// 返回值：
//     成功 0
//     失败 -1
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <sys/stat.h>
#include <sys/types.h>

int main() {
    int ret = mkdir("fuck", 0777);
    if (ret == -1) {
        perror("mkdir");
        return -1;
    }

    return 0;
}
~~~

#### rmdir函数

~~~cpp
#include <unistd.h>
int rmdir(const char *pathname);
~~~

删除目录，略

**这个函数只能删除空目录，如果里面有文件需要先把文件清空，才能进行删除!!!**

#### rename函数

~~~cpp
#include <stdio.h>
int rename(const char *oldpath, const char *newpath);
//一看就懂了
~~~

~~~cpp
#include <iostream>
using namespace std;

int main() {
    int ret = rename("fuck", "fuckyou");
    if (ret == -1) {
        perror("rename");
        return -1;
    }

    return 0;
}
~~~

#### chdir函数和getcwd函数

~~~cpp
#include <unistd.h>
int chdir(const char *path);
//作用：修改进程的工作目录
    //比如在 /home/newcoder 启动了一个可执行程序 a.out，进程的工作目录 /home/newcoder
//参数：
    //path：需要修改到的工作目录


#include <unistd.h>
char *getcwd(char *buf, size_t size);
//作用：获取当前工作目录
//参数：
    //buf：往数组里存，传出参数
    //size：数组的大小
//返回值：返回的是指向的一块内存，这个数据就是第一个参数buf
~~~

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>

int main() {
    // 获取当前工作目录
    char buf[1024] = {0};

    getcwd(buf, sizeof(buf));
    cout << buf << endl;

    // 修改工作目录
    int ret = chdir("./fuckyou");
    if (ret == -1) {
        perror("chdir");
        return -1;
    }

    // 代码执行到这里把工作目录修改之后在该目录之下进行下一步操作
    // 只在程序运行的时候生效，程序结束后终端当中的工作目录不会改变!
    
    // 创建新的文件
    int fd = open("1.txt", O_RDWR | O_CREAT, 0664);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    close(fd);

    char Null_buf[1024] = {0};
    strncpy(buf, Null_buf, sizeof(buf));

    getcwd(buf, sizeof(buf));
    cout << buf << endl;

    return 0;
}
~~~

### 文件目录遍历函数

![image-20230715125019218](https://img-blog.csdnimg.cn/68baea5562d14e739d8168448a3d14ab.png)

#### opendir函数

~~~cpp
#include <dirent.h>
#include <sys/types.h>
DIR *opendir(const char *name);
//参数：
    //name：需要打开的目录的名称
//返回值：
    //DIR * 类型，理解为目录流
    //错误 返回nullptr
~~~

#### readdir函数

#### dirent结构体和d_type(!!!)

![image-20230715130311932](https://img-blog.csdnimg.cn/97556f847fac458d822040eaf02202ef.png)

**这个dirent存储的也是文件的信息，前面也有个stat结构体也有存储，但是这两个存储的东西不太一样**

~~~cpp
//打开目录
#include <dirent.h>
#include <sys/types.h>
DIR *opendir(const char *name);
//参数：
    //name：需要打开的目录的名称
//返回值：
    //DIR * 类型，理解为目录流
    //错误 返回nullptr

//读取目录中的数据
#include <dirent.h>
struct dirent *readdir(DIR *dirp);
//参数：
    //dirp是通过opendir返回的结果
//返回值：
    //struct dirent 代表读取到的文件的信息
    //读取到了文件末尾或者失败了，返回Null，区别是读到文件末尾不会修改errno，失败会修改

//关闭目录
#include <dirent.h>
int closedir(DIR *dirp);
~~~

代码：

~~~cpp
#include <iostream>
#include <string>
using namespace std;
#include <dirent.h>
#include <sys/types.h>

// 递归函数，用于获取目录下所有普通文件的个数
void getFileNum(string path, int& num) {
    // 打开目录
    DIR* _dir = opendir(path.c_str());
    if (_dir == nullptr) {
        perror("opendir");
        exit(0);
    }

    // 读取目录数据
    // 循环读取，因为// 注意这个16的由来dir是一个一个读取的，读到末尾返回Null
    struct dirent* _ptr;

    while ((_ptr = readdir(_dir)) != nullptr) {
        // 获取名称
        // Shell终端中有当前目录 ./ 和 上级目录 ../，这两个不能拿来递归，需要忽略
        string _dname = _ptr->d_name;
        if (_dname == "." || _dname == "..")
            continue;

        // 判断是普通文件还是目录
        if (_ptr->d_type == DT_DIR)
            // 目录需要拼接一下
            getFileNum(path + "/" + _dname, num);
        else if (_ptr->d_type == DT_REG)
            ++num;
    }

    // 关闭目录
    closedir(_dir);
}

int main(int argc, char* const argv[]) {
    // 读取某个目录下所有普通文件的个数
    if (argc < 2) {
        cout << "usage: " << argv[0] << " <path>" << endl;
        return -1;
    }

    int num = 0;
    getFileNum(argv[1], num);
    cout << num << endl;

    return 0;
}
~~~

### dup,dup2函数(与文件描述符相关)

![image-20230715141246853](https://img-blog.csdnimg.cn/1304264d2ffd444a9b7a9489951c5c62.png)

#### dup()

~~~cpp
#include <unistd.h>
int dup(int oldfd);
// 作用：复制一个新的文件描述符，他们是指向同一个文件的，只是用了两个文件描述符
// 新的文件描述符会使用空闲的文件描述符表中最小的那个!!!
~~~

~~~cpp
#include <iostream>
#include <string>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int fd = open("1.txt", O_RDWR | O_CREAT, 0664);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    int fd1 = dup(fd);
    if (fd1 == -1) {
        perror("dup");
        return -1;
    }

    printf("fd : %d , fd1 : %d\n", fd, fd1);

    // 关闭fd，现在只有fd1指向文件
    close(fd);

    // 通过fd1来写该文件
    string fuck = "hello world";
    int ret = write(fd1, fuck.c_str(), fuck.size());
    if (ret == -1) {
        perror("write");
        return -1;
    }

    return 0;
}
~~~

#### dup2()

~~~cpp
#include <unistd.h>

int dup2(int fd1, int fd2);
// 作用：重定向文件描述符
    // fd1指向a.txt，fd2指向b.txt
    // 调用函数成功后，fd2和b.txt的连接做close(fd1仍指向a.txt)，fd2指向a.txt
    // fd1必须是一个有效的文件描述符
    // 如果相同则相当于什么都没做
// 返回值：
    // fd2，他们都指向的是fd1之前指向的文件
~~~

~~~cpp
#include <iostream>
#include <string>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int fd = open("2.txt", O_RDWR | O_CREAT, 0664);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    int fd1 = open("3.txt", O_RDWR | O_CREAT, 0664);
    if (fd1 == -1) {
        perror("open");
        return -1;
    }

    printf("fd : %d , fd1 : %d\n", fd, fd1);

    int fd2 = dup2(fd, fd1);
    // 现在fd本来指向 2.txt ，现在fd仍指向2.txt
    // fd1本来 3.txt，现在指向到了 2.txt
    if (fd2 == -1) {
        perror("dup2");
        return -1;
    }

    // 通过fd1去写数据，实际操作的是2.txt
    // 通过fd一样指向2.txt
    string fuck = "hello world";
    int ret = write(fd1, fuck.c_str(), fuck.size());
    if (ret == -1) {
        perror("write");
        return -1;
    }

    printf("fd : %d , fd1 : %d , fd2: %d\n", fd, fd1, fd2);

    return 0;
}
~~~

### fcntl()

两个作用：

**复制文件描述符**

**设置或者获取文件的状态标志**

![image-20230715145100478](https://img-blog.csdnimg.cn/530146ebbb03403cbd72ef421761015b.png)

~~~cpp
#include <fcntl.h>
#include <unistd.h>

int fcntl(int fd, int cmd, ...); ...当中是可变参数
// 参数：
//     fd：需要操作的文件描述符
//     cmd：表示对文件描述符进行如何操作
//         F_DUPFD 复制文件描述符，复制的是第一个参数，得到一个新的文件描述符(返回值)
//             int ret = fcntl(fd,F_DUPFD);
//         F_GETFL 获取指定文件描述符的文件状态flag
//             获取的flag和我们通过open函数传递的flag是一个东西
//         F_SETFL 设置文件描述符的文件状态flag
//             必选项：O_RDONLY O_WRONLY O_RDWR 不可以被修改
//             可选项：O_APPEND O_NONBLOCK
//                 O_APPEND 表示追加数据
//                 O_NONBLOCK 设置成非阻塞
//                     阻塞和非阻塞：描述的是函数调用的行为
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    // 1.复制文件描述符，得到两个文件描述符(int值不同)指向同一个文件，可以进行的操作相同
    // int fd = open("1.txt", O_RDONLY);
    // if (fd == -1) {
    //     perror("open");
    //     return -1;
    // }

    // int ret = fcntl(fd, F_DUPFD);
    // if (ret == -1) {
    //     perror("fcntl");
    //     return -1;
    // }

    // 2.修改或者获取文件描述符的文件状态flag
    // 这里必须读写权限都要有才行
    int fd = open("1.txt", O_RDWR, 0664);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // 修改文件描述符的flag，加入O_APPEND这个标记
    // 首先获得
    int _flag = fcntl(fd, F_GETFL);
    if (_flag == -1) {
        perror("fcntl");
        return -1;
    }

    // 然后修改
    int ret = fcntl(fd, F_SETFL, _flag | O_APPEND);
    if (ret == -1) {
        perror("fcntl");
        return -1;
    }

    // 然后进行追加
    string fuck = "你好";
    ret = write(fd, fuck.c_str(), fuck.size());
    if (ret == -1) {
        perror("write");
        return -1;
    }

    close(fd);

    return 0;
}
~~~

### 文件删除函数

#### unlink()和remove()

~~~cpp
#include <unistd.h>
int unlink(const char *pathname); // linux系统的函数
~~~

~~~cpp
#include <stdio.h>
int remove(const char *pathname); // 标准C库的函数
~~~

# 第二章 Linux多进程开发

## 一些概念

### 程序和进程

![image-20230716113520531](https://img-blog.csdnimg.cn/8c066e253e664b8dbbae425c6cbc8495.png)

![image-20230716113908652](https://img-blog.csdnimg.cn/87dae5a63f714639825c0af3f655a13a.png)

### 单道，多道程序设计

![image-20230716114055627](https://img-blog.csdnimg.cn/f54a9eb14ba34142b26ae51249f8a52b.png)

### 时间片

![image-20230716114135444](https://img-blog.csdnimg.cn/0d62d3a6ba2a4ad7897a3c072f416f2e.png)



### 并行和并发

![image-20230716114205304](https://img-blog.csdnimg.cn/07a4ca3a5b2a4cffa1660c292e8e8dee.png)

### 进程控制块PCB

**在Linux内核的进程控制块是 task_struct 结构体**

![image-20230716120419774](https://img-blog.csdnimg.cn/9f8ec96ec55c4f63b7d88793a161d004.png)

<img src="https://img-blog.csdnimg.cn/f6d27217a7f947c8928170e64f1f9e4d.png" alt="image-20230716121448756" style="zoom:80%;" />

## 进程状态

### 状态转化图

![image-20230718093121456](https://img-blog.csdnimg.cn/d3bc8a04198348c189aa395edc27ad62.png)

**加入新建和终止的状态**

![image-20230718093806701](https://img-blog.csdnimg.cn/81675cad6df6469b935a8b9d42d5cfef.png)

### 进程相关命令

~~~shell
ps aux / ajx
#注意aux和ajx的区别
~~~

![image-20230718094028703](https://img-blog.csdnimg.cn/7c6e6637ee1347f4bd0703747a917a25.png)

#### ps aux

![image-20230718094423758](https://img-blog.csdnimg.cn/0a14c4080bcb46928559584830f9337a.png)

stat参数的意义：

![image-20230718095104233](https://img-blog.csdnimg.cn/698614e12584488592fe8230c5e58f6d.png)

#### ps ajx

![image-20230718095230919](https://img-blog.csdnimg.cn/ee8464effa5c45fab889c27bd4057ea1.png)

#### top

**实时显示进程动态**

![image-20230718095445176](https://img-blog.csdnimg.cn/6c3abdd3ae9543b581491bead1006e8c.png)

#### kill

**杀死进程**

<img src="https://img-blog.csdnimg.cn/cf9073b5d0234181a1b4d5fdafffb1f5.png" alt="image-20230718095843259" style="zoom: 80%;" />

里面有一个 9 号信号(SIGKILL)，比如：

~~~bash
kill -9 <pid>
~~~

**这个命令可以强制杀掉某个进程**

![image-20230718100643298](https://img-blog.csdnimg.cn/09d94842dff847a89af558ad60c8096a.png)

#### ./a.out & 后台运行程序

**这个时候，这个进程会被挂在后台进行运行，然后他的打印信息仍然会打印在屏幕上，这里使用ps aux查看命令可以看出是可以正常执行命令的，不冲突，然后可以用 kill 998 将其杀掉**

![image-20230718101259447](https://img-blog.csdnimg.cn/a562b991c8e14231b161e51bf344e347.png)

## 创建进程

### 进程号和相关函数

**进程组是一个或者多个进程的集合，他们相关联，并且可以接受同一终端的各种信号，具有一个进程组号(pgid)**

![image-20230718101452289](https://img-blog.csdnimg.cn/b4d9c9cf59e24a59b0bd7b06ce20ea10.png)

### 进程创建

![image-20230718101951634](https://img-blog.csdnimg.cn/bb0d676a8e4e4ff49673312b9bb9bbb3.png)

#### fork()

~~~cpp
#include <unistd.h>

pid_t fork(void);
//作用：用于创建子进程
//返回值：
    //fork()的返回值会返回两次，一次是在父进程中，一次是在子进程中
    //为什么会走两次呢？因为这个cpp文件编译出来是一个可执行文件 a.out，这个文件执行后是进程 ./a.out，自然创建出来的也是 ./a.out进程，也就是走的是这个cpp程序
    //但是注意一点：子进程是不会执行fork()函数的，因为fork()函数是父进程执行的，但是他会持有返回值，在子进程当中就是0
    //成功 父进程中返回子进程的pid，子进程中返回0
    //失败 父进程中返回-1，没有子进程被创建，然后修改errno
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    int pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    // 判断是父进程还是子进程
    if (pid > 0) {  // 父进程走，返回的是创建的子进程编号
        cout << pid << endl;
        printf("I am parent process, pid : %d , ppid : %d\n", getpid(), getppid());
    } else if (pid == 0)  // 子进程走
        printf("I am child process, pid : %d , ppid : %d\n", getpid(), getppid());

    // for
    for (int i = 0; i < 3; ++i) {
        sleep(1);
        cout << i << endl;
    }

    return 0;
}
~~~

**父进程和子进程是并发执行的，在如上的程序当中先打印那个是不确定的，取决于谁先拿到处理机!!!**

**当然如果程序的运行时间太短了，导致在一个时间片当中可以完成，那么看起来就像是两个东西是按照顺序进行的，但是实际上是并发的!!!**

例如这是两次的执行结果：

![image-20230718104207327](https://img-blog.csdnimg.cn/a1283af4784c410baf2221a8906d39cf.png)

当给多点的时候，到后面并发执行的时候就可能是交替执行

<img src="https://img-blog.csdnimg.cn/85cf918904d54db281d097b77777cb0b.png" alt="image-20230718105046657" style="zoom:67%;" />

### 父子进程虚拟地址空间

**执行到fork()函数的时候，克隆一份空间(不完全正确，后面会讲)，子进程这边不执行fork()函数，fork()函数只给父进程执行，但是子进程会得到返回值为0用来和父进程进行区别，然后两个进程根据逻辑并发执行!!**

**但其实这两个空间大部分的东西都是一样的!!!**

![image-20230718142613257](https://img-blog.csdnimg.cn/84822f6a580a4e9696f020862dfadc97.png)

#### 读时拷贝，写时共享

**子进程在被创建出来的时候，如果没有对变量进行修改，进行写的操作，那么父子进程共享一块空间；当父子进程需要进行写，就是修改变量的值的时候就拷贝一块空间，这样就节省了物理内粗拿到空间!!!**

![image-20230718143403658](https://img-blog.csdnimg.cn/90d61fd388c94bd3ae1f46ffd5cf6588.png)

### 总结：父子进程关系

区别：

- fork()函数返回值不同：父进程 > 0 ，返回子进程的pid；子进程 = 0
- pcb中的一些数据：当前进程的id pid，当前进程的父进程的id ppid，**信号集**

共同点：

- 在某些状态下，子进程刚被创建出来，还没执行任何写的操作

  用户区的数据，文件描述符表是共享的!!!

- 父子进程对变量是不是共享的？

  刚开始的时候是一样的，共享的；但是修改了数据就不共享了

  读时共享(两个进程没有做任何写的操作)，写时拷贝

## GDB多进程调试

**默认情况下gdb调试的都是父进程，加了断点的话默认只对父进程生效，可以设置**

![image-20230718145227613](https://img-blog.csdnimg.cn/1b30c958fd9e4a71b65ad4ded2c5e8c1.png)

### 设置调试子进程

![image-20230718145632605](https://img-blog.csdnimg.cn/7e023458740f4d5696327288f7dcf2c3.png)

### 设置调试模式

**这里我把 detac-on-fork 模式关闭了，父进程在被调试的时候，子进程会被挂起，所以屏幕上没显示j的信息**

![image-20230718151209448](https://img-blog.csdnimg.cn/93e2f24a56fd4a629d4f50c4b3c42967.png)

![image-20230718151224879](https://img-blog.csdnimg.cn/904d8708e6ec4868a3bc9c959e1c8142.png)

## exec函数族

### 函数族介绍

**exec函数族的作用是根据指定的文件名找到可执行文件，然后用它替换当前我们调用的进程的内容然后去执行**

**为了保护当前的进程的内容，一般创建一个子进程然后让exec函数族去替换子进程的内容去执行，这样就好了**

**执行成功了不会返回，因为新的已经取代了旧的，旧的再接受返回值并做操作没有任何意义；只有调用失败了才会返回-1，这个时候新的不会进来，还是旧的**

![image-20230718152959795](https://img-blog.csdnimg.cn/765dbf5bbec34206b5d2b0ba84b7f8f6.png)

图解

![image-20230718154755731](https://img-blog.csdnimg.cn/72938bd9ed2b4abfa8965691a9dc9eb8.png)

### 函数族(附图)

**前面6个函数是C标准库当中的函数，最后一个(execve)是Linux下的系统函数**

![image-20230718154816547](https://img-blog.csdnimg.cn/41b24a104c994e10970781c4cbbff38f.png)

#### execl()

~~~cpp
#include <unistd.h>
int execl(const char *pathname, const char *arg, ... , (char *) NULL );
//参数：
    //path：指定的执行的可执行文件的路径，例如：a.out 推荐写绝对路径
    //arg：字符串，是可执行文件所需要的参数列表
        //第一个参数(arg当中的第一个参数!!)一般没有作用，为了方便，一般写的是可执行的程序的名称
        //从第二参数开始往后，就是程序执行所需要的参数列表
        //参数最后需要以NULL结束(哨兵)

//返回值：
    //只有调用错误，才会有返回值，为-1，且设置errno
    //调用成功不会有返回值，
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    // 创建一个子进程，在子进程中执行exec函数族当中的函数
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        return -1;
    }

    if (child_pid > 0) {
        // 父进程
        printf("I am parent process , pid : %d\n", getpid());
        sleep(1);
    } else if (child_pid == 0) {
        // 子进程
        execl("hello", "hello", nullptr);
        printf("I am child process , pid : %d\n", getpid());
    }

    for (int i = 0; i < 3; ++i)
        printf("i = %d , pid = %d\n", i, getpid());

    return 0;
}
~~~

执行结果：

可以看出 child process 这句话没有打印出来，他被可执行文件hello替换掉了，最后的打印i是父进程打印出来的

![image-20230718170703719](https://img-blog.csdnimg.cn/direct/508d01d1858f45b6a9f9575cc8ce1593.png)

**还可以让其执行linux的shell命令**

比如 ps aux 命令，ps就是一个命令，对应可执行文件 /usr/bin/ps

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    // 创建一个子进程，在子进程中执行exec函数族当中的函数
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        return -1;
    }

    if (child_pid > 0) {
        // 父进程
        printf("I am parent process , pid : %d\n", getpid());
        sleep(1);
    } else if (child_pid == 0) {
        // 子进程
        // execl("hello", "hello", nullptr);

        // 可以执行linux系统自己的可执行程序，比如ps命令(/usr/bin/ps)
        execl("/usr/bin/ps", "ps", "aux", nullptr);

        printf("I am child process , pid : %d\n", getpid());
    }

    for (int i = 0; i < 3; ++i)
        printf("i = %d , pid = %d\n", i, getpid());

    return 0;
}
~~~

执行结果：

![image-20230718171319019](https://img-blog.csdnimg.cn/4cbd92ebe080486394ce600977223393.png)

#### execlp()

**与execl()函数的区别是execlp()函数里面传入的是可执行文件名，不是路径；会自动到环境变量中去查找这些可执行文件名，适合用作系统的命令，而execl()传路径，适合自己的可执行文件(不用配置环境变量)**

~~~cpp
#include <unistd.h>
int execlp(const char *file, const char *arg, ... , (char *) NULL );
//参数：
    //会到环境变量中查找可执行文件，找到了就执行，找不到就不成功!!!
    //file：需要执行的可执行文件的文件名，不包含路径!!!


    //arg：字符串，是可执行文件所需要的参数列表
        //第一个参数(arg当中的第一个参数!!)一般没有作用，为了方便，一般写的是可执行的程序的名称
        //从第二参数开始往后，就是程序执行所需要的参数列表
        //参数最后需要以NULL结束(哨兵)

//返回值：
    //只有调用错误，才会有返回值，为-1，且设置errno
    //调用成功不会有返回值
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    // 创建一个子进程，在子进程中执行exec函数族当中的函数
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        return -1;
    }

    if (child_pid > 0) {
        // 父进程
        printf("I am parent process , pid : %d\n", getpid());
        sleep(1);
    } else if (child_pid == 0) {
        // 子进程
        // execl("hello", "hello", nullptr);

        // 可以执行linux系统自己的可执行程序，比如ps命令(/usr/bin/ps)
        execlp("ps", "ps", "aux", nullptr);

        printf("I am child process , pid : %d\n", getpid());
    }

    for (int i = 0; i < 3; ++i)
        printf("i = %d , pid = %d\n", i, getpid());

    return 0;
}
~~~

依然能执行ps aux 命令

## 进程控制

### 进程退出

**exit()是标准C库的函数，在linux系统下底层会调用linux系统的函数_exit()**

![image-20230718194621050](https://img-blog.csdnimg.cn/41f0c0ada69b4a96950f995a416ebfd7.png)

两个的区别：**exit()在调用之后需要比_exit()做更多的事情，其中有一个就是刷新I/O缓冲关闭文件描述符**

对于下面的程序

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    cout << "hello" << endl;
    cout << "world";

    // exit(0);
    _exit(0);

    return 0;
}
~~~

使用exit(0)和_exit(0)会有如下两个不同结果

exit:

![image-20230718195742100](https://img-blog.csdnimg.cn/467fdaea69074146a9fea7f8563a8af4.png)

_exit:

![image-20230718195756973](https://img-blog.csdnimg.cn/direct/218040bf61b0410a99a1fb049b3082cc.png)

那么为什么会这样呢？

#### 刷新缓冲区问题(重要)

**c++和c语言中的cout或者printf()也好，本质都是printf()，printf()作为C语言标准库的函数，并不是直接输出到终端屏幕上，而是先写在缓冲区当中，然后当缓冲区刷新的时候在清空缓冲区输出到屏幕当中!!!**

![image-20230718200326121](https://img-blog.csdnimg.cn/6d76b07a6fdb48388c3739682ddbe2d8.png)

于是乎刚才的代码：

~~~cpp
cout << "hello" << endl;
cout << "world";

// exit(0);
_exit(0);
~~~

hello遇到endl，相当于 '\n'，刷新缓冲区，显示；第二个world在缓冲区中，本来正常程序结束就显示在屏幕上了，现在遇到了_exit()不刷新缓冲区，那么就丢掉了，不显示

##### 变式

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    cout << "hello";

    sleep(2);

    return 0;
}
~~~

这个程序在输出的时候不会立刻输出hello，因为这个时候缓冲区没有刷新，会谁2秒，然后return 0主程序结束的时候再刷新缓冲区进行显示!!!

### 孤儿进程

**父进程有义务回收子进程的资源，但是当父进程结束而子进程未结束的时候这一点无法做到，所以这个时候内核会把这个子进程的父进程设置为init进程(linux上的第一层进程，linux下的进程都是不断的创建子进程而创建出来的)，init进程会循环的wait()等待这个子进程，当这个子进程结束了之后，init进程会将其回收，处理善后工作!!!**

![image-20230718202030384](https://img-blog.csdnimg.cn/693e90b4cccd448a8f61495f79ca24c8.png)

演示代码：

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    int pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    // 判断是父进程还是子进程
    if (pid > 0) {  // 父进程走，返回的是创建的子进程编号
        printf("I am parent process, pid : %d , ppid : %d\n", getpid(), getppid());
    } else if (pid == 0) {  // 子进程走
        sleep(1);           // 强制让子进程睡1秒，让父进程跑完，子进程称为孤儿进程
        printf("I am child process, pid : %d , ppid : %d\n", getpid(), getppid());
    }

    // for
    for (int i = 0; i < 3; ++i) {
        printf("i : %d , pid : %d\n", i, getpid());
    }

    return 0;
}
~~~

输出效果：

![image-20230718203522772](https://img-blog.csdnimg.cn/fca4df6fe7674773bea506ad19163f9e.png)

**父进程创建出来子进程之后，子进程休眠1秒，父进程早已跑完，结束，子进程交给_init进程作为他的父进程进行管理，所以会显示终端是因为父进程结束了，但是这个时候子进程尚未结束，父进程和子进程在内核区域有一些数据是一样的，比如文件描述符012的标准输入，输出和错误，所以仍然可以在当前终端输出信息。并且这里 _init还让他结束之后阻塞了**

### 僵尸进程

**每个进程结束之后，内核区的PCB没有办法自己释放，需要父进程释放；用户区的数据可以自己释放**

**进程终止的时候，父进程尚未回收，子进程的残留资源(PCB)存放在内核中，变成了僵尸进程**

**僵尸进程不能被 kill -9 杀死**

**僵尸进程多了会占据进程号，进程号范围 0 ~ 32767，占据完了就会有危险**

![image-20230718203825193](https://img-blog.csdnimg.cn/780461dabe0c41759a699f17aabc9963.png)

演示代码：

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    int pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    // 判断是父进程还是子进程
    if (pid > 0) {   // 父进程走，返回的是创建的子进程编号
        while (1) {  // 强制让父进程一直循环，不退出，让子进程结束，父进程没办法回收他的资源
            printf("I am parent process, pid : %d , ppid : %d\n", getpid(), getppid());
            sleep(1);
        }
    } else if (pid == 0) {  // 子进程走
        printf("I am child process, pid : %d , ppid : %d\n", getpid(), getppid());
    }

    return 0;
}
~~~

输出结果：

![image-20230718205238264](https://img-blog.csdnimg.cn/98a06fdf668d4357ae0cdce5736628e4.png)

可以看出，子进程结束了但是父进程没结束，没有办法释放子进程内核区域的数据，导致了僵尸进程

![image-20230718205324350](https://img-blog.csdnimg.cn/direct/2c0a8e6b043a4874986e8665b0927356.png)

现在的状态，子进程Z+代表是僵尸进程，< defunct >代表不存在的；父进程S+代表睡眠

kill -9 杀不掉僵尸进程

![image-20230718205637544](https://img-blog.csdnimg.cn/8f2c17fb11464d5ba06a77c87db61666.png)

![image-20230718205647171](https://img-blog.csdnimg.cn/direct/945fed4011c84c73855845eb77eaf067.png)

**这个时候想要解决这个问题只能杀掉父进程，把子进程托管给 /init ，这样才能将其释放，但是实际开发当中杀掉父进程往往不现实，所以需要父进程调用wait()和waitpid()来保证父进程会把子进程的内核PCB的数据给释放掉，这样才能避免僵尸进程**

![image-20230718210109506](https://img-blog.csdnimg.cn/aa22c1101edc4e3081f4c2b79fc60c48.png)

### 进程回收

![image-20230718211154805](https://img-blog.csdnimg.cn/893cfb19d0c546439998ecdba3ee41ad.png)

#### wait()

**为了避免僵尸进程，父进程需要回收子进程的资源**

**wait()函数会阻塞，父进程调用之后会阻塞在那里等待子进程结束然后释放子进程的资源**

~~~cpp
#include <sys/wait.h>

pid_t wait(int *wstatus);
//功能：等待任意一个子进程结束，如果任意一个子进程结束了，此函数会回收这个子进程的资源
//参数：int *wstatus
    //进程退出时候的状态信息，传入的是一个int类型的地址，传出参数
//返回值：
    //成功 返回被回收的子进程的id
    //失败 -1(所有的子进程都结束，调用函数失败) 并且修改errno

//调用wait()函数，进程会阻塞，知道他的一个子进程退出或者收到一个不能被忽略的信号，这个时候才被唤醒
//如果没有子进程，这个函数立刻返回-1；如果子进程都已经结束了，也会返回-1
~~~

示例代码：

~~~cpp
#include <iostream>
using namespace std;
#include <sys/wait.h>
#include <unistd.h>

int main() {
    // 有一个父进程，创建5个子进程
    pid_t pid;

    for (int i = 0; i < 5; ++i) {
        pid = fork();

        if (pid == -1) {
            perror("fork");
            return -1;
        }

        if (pid == 0)  // 说明是子进程，如果不加这行代码，子进程也会走for循环，他也会fork()产生更多的孙子进程，重孙进程等等
            break;
    }

    if (pid > 0) {
        // 父进程
        while (1) {
            printf("parent , pid = %d\n", getpid());

            // int ret = wait(NULL);

            int status;
            int ret = wait(&status);

            printf("child die,pid = %d\n", ret);

            sleep(1);
        }
    } else if (pid == 0) {
        // 子进程
        printf("child , pid = %d\n", getpid());
        sleep(1);

        exit(0);
    }

    return 0;
}
~~~

这个代码用父进程创建了5个子进程(注意看怎么创建的)，父进程中用wait()函数阻塞等待子进程结束，子进程结束一个，父进程输出关于ret的一段信息表示子进程已经被回收，然后重复，直到所有子进程全部结束被回收完毕；这个时候父进程依然在循环，只不过wait()返回-1

执行结果：

![image-20230719094953685](https://img-blog.csdnimg.cn/a26add6da0f943cbbc11983e2967bc3f.png)

稍加修改，现在我需要查看进程退出的状态：

我让进程正常退出

~~~cpp
#include <iostream>
using namespace std;
#include <sys/wait.h>
#include <unistd.h>

int main() {
    // 有一个父进程，创建5个子进程
    pid_t pid;

    for (int i = 0; i < 5; ++i) {
        pid = fork();

        if (pid == -1) {
            perror("fork");
            return -1;
        }

        if (pid == 0)  // 说明是子进程，如果不加这行代码，子进程也会走for循环，他也会fork()产生更多的孙子进程，重孙进程等等
            break;
    }

    if (pid > 0) {
        // 父进程
        while (1) {
            printf("parent , pid = %d\n", getpid());

            // int ret = wait(NULL);

            int status;
            int ret = wait(&status);

            if (ret == -1)  // 没有子进程
                break;

            // 有子进程，回收了子进程的资源
            if (WIFEXITED(status)) {  // 是不是正常退出
                printf("退出的状态码: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {  // 是不是异常退出
                printf("被哪个信号干掉了: %d\n", WTERMSIG(status));
            }

            printf("child die,pid = %d\n", ret);

            sleep(1);
        }
    } else if (pid == 0) {
        // 子进程
        // while(1) {
        printf("child , pid = %d\n", getpid());
        sleep(1);
	    // }
        
        exit(0);
    }

    return 0;
}
~~~

这个时候子进程退出时候调用C标准库函数exit(0)，状态码就是0，因此正常退出的状态码用图中形式接受并打印

执行结果：

![image-20230719095319683](https://img-blog.csdnimg.cn/e4dcf8ca023f4fb78de0f20aa639b659.png)

现在我把子进程改成while(1)循环，然后用 kill -9 杀掉他，看看他是被哪个信号干掉的

##### 退出信息宏函数(status相关)

![image-20230718220547310](https://img-blog.csdnimg.cn/38968df7a29c4c549536d86b0d5ad54d.png)

执行结果：

![image-20230719095606504](https://img-blog.csdnimg.cn/7d2eaef2e73b4190a506f92fad91e63b.png)

![image-20230719095625400](https://img-blog.csdnimg.cn/9a594fc86bca4f44ad97b1c9f8eb2fcf.png)

#### waitpid()

~~~cpp
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *_Nullable wstatus, int options);
// 作用：回收指定进程号的子进程，可以设置是否阻塞
// 参数：
//     pid：> 0 表示某个子进程的id
//         == 0 回收当前进程组的所有子进程(我自己的子进程不一定和我属于一个组，有可能被我给出去了)
//         == -1 回收所有的子进程，相当于wait()，最常用
//         < -1 回收某个进程组当中的所有子进程，组号是这个参数的绝对值
//     options：设置阻塞或者非阻塞
//         0 阻塞
//         WNOHANG：非阻塞
// 返回值：
//     > 0 返回子进程的id
//     == 0 options = WNOHANG，表示还有子进程活着
//     ==-1 错误，并且设置错误号，在非阻塞的情况下返回-1可以代表没有子进程了

// 这么来看,waitpid(-1,&status,0) 相当于是 wait(&status)
~~~

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <sys/wait.h>
#include <unistd.h>

int main() {
    // 有一个父进程，创建5个子进程
    pid_t pid;

    for (int i = 0; i < 5; ++i) {
        pid = fork();

        if (pid == -1) {
            perror("fork");
            return -1;
        }

        if (pid == 0)  // 说明是子进程，如果不加这行代码，子进程也会走for循环，他也会fork()产生更多的孙子进程，重孙进程等等
            break;
    }

    if (pid > 0) {
        // 父进程
        while (1) {
            printf("parent , pid = %d\n", getpid());
            sleep(1);

            // int ret = wait(NULL);

            int status;
            // int ret = waitpid(-1, &status, 0);        // 阻塞
            int ret = waitpid(-1, &status, WNOHANG);  // 非阻塞

            if (ret == -1)  // 没有子进程
                break;
            else if (ret == 0)
                // 非阻塞就是执行到这个位置判断一下，然后遇到了就回收了，没有就走了
                // ret==0表明还有子进程活着，重开循环判断
                // 非阻塞的好处：父进程不用一直阻塞这等待子进程结束，可以做自己的逻辑，然后每隔一段时间就回来看子进程是否运行完毕然后回收。提高效率
                continue;
            else if (ret > 0) {
                // 有子进程，回收了子进程的资源
                if (WIFEXITED(status)) {  // 是不是正常退出
                    printf("退出的状态码: %d\n", WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {  // 是不是异常退出
                    printf("被哪个信号干掉了: %d\n", WTERMSIG(status));
                }
                printf("child die,pid = %d\n", ret);
            }
        }
    } else if (pid == 0) {
        // 子进程
        while (1) {
            printf("child , pid = %d\n", getpid());
            sleep(1);
        }

        exit(0);
    }

    return 0;
}
~~~

非阻塞的含义：非阻塞就是执行到这个位置判断一下，然后遇到了就回收了，没有就走了

非阻塞的好处：父进程不用一直阻塞这等待子进程结束，可以做自己的逻辑，然后每隔一段时间就回来看子进程是否运行完毕然后回收。提高效率

图中的代码含义就是：子进程一直循环输出，父进程每次非阻塞的waitpid()，有子进程则重新循环再次判断，如果子进程结束(正常或异常)，则进入下方循环输出相关信息然后再回去循环

执行结果：

我不管他是这样，可以父进程是非阻塞的一直在工作的

![image-20230719103805234](https://img-blog.csdnimg.cn/707ec56a360049e6b471aca8520ed2b0.png)

现在我把子进程kill掉

![image-20230719103852259](https://img-blog.csdnimg.cn/11459e6588564d8ca2c30d6c207046e8.png)

kill一次输出一个信息，然后kill完毕之后结束进程

![image-20230719103900180](https://img-blog.csdnimg.cn/d25345b307064c60aedc155c9c8f32c1.png)

## 进程间通信 IPC

### 概念

![image-20230719104207710](https://img-blog.csdnimg.cn/7809ee6cadf749d28350dfc7a22f8177.png)

进程间通信方式(记忆!!!)

![image-20230719110333954](https://img-blog.csdnimg.cn/efa21266bccf4f7dbe0d6cf4fa50b288.png)

### 管道

管道的数据结构是环形队列。为什么是环形？大概率是为了处理边界问题吧

**管道拥有文件的特质，读操作和写操作；**

**匿名管道没有文件实体；有名管道有文件实体，但是不存储数据**

**可以按照操作文件的方式对管道进行操作，也具有文件描述符，有两个，分别指向读端和写端**

**一个管道就是一个字节流，管道没有消息或者消息边界的概念，从管道读数据可以任意读，不用考虑写入的数据是多少；并且管道传递的数据是有顺序的**

![image-20230719112527654](https://img-blog.csdnimg.cn/abd180c84cfe48e4b33ad44c16276639.png)

**管道单向，一边写，一边读；半双工**

**匿名管道只能在拥有共同祖先的进程当中使用，例如父进程和子进程；两个兄弟进程**

![image-20230719143725384](https://img-blog.csdnimg.cn/e5c949d220c74af88c74952b90a10217.png)

**为什么可以用管道通信**

父进程 fork() 之后，子进程和父进程共享了一份这个文件描述符表。管道也具有文件的性质，他的两端分别对应读和写的文件描述符，因此需要两个进程指向这同一个位置，所以必须是具有一定关系的进程才能使用，就像如图所示，父子进程的5号都对应写数据，6号都对应读数据，然后这样就可以联系起来了

#### 那么在fork之前还是fork之后建立管道呢？

**答案是fork()之前。**

**因为fork()之前建立管道，比如如图，管道占据两个文件描述符5和6，现在fork()之后，由于管道具有文件描述符，子进程内核区域文件描述符指向的东西是和父进程一块东西，所以他们两个指向的是同一块管道，这样才能进行通信!!!**

![image-20230719144556698](https://img-blog.csdnimg.cn/43e68821b3a24c98b1c34dd184e8115d.png)

管道的数据结构：环形队列

![image-20230719145008340](https://img-blog.csdnimg.cn/a01f32e05a7c4c189dc85d4b4a1f2299.png)

#### 匿名管道(pipe)

![image-20230719111308071](https://img-blog.csdnimg.cn/724481d797fa4c1691a093c945ecb4fe.png)

看如图的命令：

~~~bash
ls | wc -l
~~~

**这是两个命令 ls 和 wc -l，整个的作用就是用ls查看目录下的文件然后传递给 wc -l 命令实现统计并且在屏幕上进行输出**

**那么怎么进行传递呢？或者说怎么把ls获取到的数据交给wc进程呢？**

**这就需要管道了，命令当中的 | 就是指在两个进程之间建立一个匿名管道，然后前面的进程向后面的进程进行通信，提供数据信息**

匿名管道的使用

![image-20230719145051553](https://img-blog.csdnimg.cn/direct/daca29153e704891bbb9f5d04e75a820.png)

##### pipe()

~~~cpp
#include <unistd.h>

int pipe(int pipefd[2]);
// 功能：创建一个匿名管道，用于进程间通信
// 参数：int pipefd[2] 这个数组是一个传出参数
//     pipefd[0] 对应的是管道的读端
//     pipefd[1] 对应的是管道的写端
// 返回值：
//     成功 0
//     失败 -1，并且设置errno

// 注意：匿名管道只能用于具有关系的进程通信之间，比如：父子进程，兄弟进程等等

//     管道默认是阻塞的，如果管道中没有数据，read阻塞；管道满了，write阻塞
~~~

~~~cpp
#include <cstring>
#include <iostream>
#include <string>
using namespace std;
#include <unistd.h>
#define _size 1024

int main() {
    // 子进程发送数据给父进程，父进程读取到数据输出

    // 在fork之前创建管道，因为要指向一个管道
    int pipefd[2];

    int ret = pipe(pipefd);
    if (ret == -1) {
        perror("pipe");
        return -1;
    }

    // 管道创建成功了，现在创建子进程
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {  // 父进程
        printf("i am parent process , pid : %d\n", getpid());

        char buf[_size] = {0};
        const char *str = "hello,i am parent";
        while (1) {
            // 读数据
            // read默认是阻塞的
            read(pipefd[0], buf, sizeof(buf));
            printf("parent recv : \"%s\" , pid : %d\n", buf, getpid());
            // 读完清空buf
            bzero(buf, _size);

            // 写数据
            write(pipefd[1], str, strlen(str));
            // read会在缓冲区空的时候阻塞，父进程写一次子进程读一次，然后再次阻塞，所以sleep是在父进程这边
            // 如果在子进程这边sleep，父进程在这段时间疯狂写
            sleep(1);
        }

    } else if (pid == 0) {  // 子进程
        printf("i am child process , pid : %d\n", getpid());

        char buf[_size] = {0};
        const char *str = "hello,i am child";
        while (1) {
            // 写数据
            write(pipefd[1], str, strlen(str));
            sleep(1);

            // 读数据
            // read默认是阻塞的
            read(pipefd[0], buf, sizeof(buf));
            printf("child recv : \"%s\" , pid : %d\n", buf, getpid());
            // 读完清空buf
            bzero(buf, _size);
        }
    }

    return 0;
}
~~~

这个程序的本意是让父子进程之间建立管道，然后子进程向管道写数据，父进程接受数据并输出

代码当中用了两个循环，子进程循环写数据给管道，父进程调用read()函数接受，read()函数默认是阻塞的，调用while(1)一直读，因此最后的结果是这样

![image-20230719153654422](https://img-blog.csdnimg.cn/e3471fa35c6d4c59a289eaf725824580.png)

变式：

父进程和子进程相互通信，一个道理

~~~cpp
#include <cstring>
#include <iostream>
#include <string>
using namespace std;
#include <unistd.h>

int main() {
    // 子进程发送数据给父进程，父进程读取到数据输出

    // 在fork之前创建管道，因为要指向一个管道
    int pipefd[2];

    int ret = pipe(pipefd);
    if (ret == -1) {
        perror("pipe");
        return -1;
    }

    // 管道创建成功了，现在创建子进程
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {  // 父进程
        printf("i am parent process , pid : %d\n", getpid());
        while (1) {
            // 读数据
            char buf[1024] = {0};
            // read默认是阻塞的
            read(pipefd[0], buf, sizeof(buf));
            printf("parent recv : \"%s\" , pid : %d\n", buf, getpid());

            // 写数据
            const char *str = "hello,i am parent";
            write(pipefd[1], str, strlen(str));
            sleep(1);
        }

    } else if (pid == 0) {  // 子进程
        printf("i am child process , pid : %d\n", getpid());

        while (1) {
            // 写数据
            const char *str = "hello,i am child";
            write(pipefd[1], str, strlen(str));
            sleep(1);

            // 读数据
            char buf[1024] = {0};
            // read默认是阻塞的
            read(pipefd[0], buf, sizeof(buf));
            printf("child recv : \"%s\" , pid : %d\n", buf, getpid());
        }
    }

    return 0;
}
~~~

父子进程都设置读操作和写操作，只是要注意一点，这里顺序要相反，因为父进程和子进程如果都先读都阻塞程序没办法推进了

执行结果：

![image-20230719153922897](https://img-blog.csdnimg.cn/0e43f5ffa7e44d22b4122952924b49b8.png)

##### fpathconf()

用来获取管道缓冲区的大小(4096 bytes)

~~~cpp
#include <unistd.h>

long fpathconf(int fd, int name);
// 作用：获取管道的大小
// 参数：fd 管道的文件描述符(两个填一个即可)
//     name：宏值
//         获取大小使用 _PC_PIPE_BUF
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    int pipefd[2];

    int ret = pipe(pipefd);
    if (ret == -1) {
        perror("pipe");
        return -1;
    }

    // 获取管道大小
    long size = fpathconf(pipefd[0], _PC_PIPE_BUF);
    printf("pipe size : %ld\n", size);  // 4096

    return 0;
}
~~~

#### 匿名管道通信案例

##### 注意

刚才的案例有一个问题就是说匿名管道在实现相互通信的时候，可能会出现进程自己写的数据被自己读取

一般的交互情况如下所示：

![image-20230721103551177](https://img-blog.csdnimg.cn/4418ff50019642c883a8e6721b66943d.png)

![image-20230721103558631](https://img-blog.csdnimg.cn/e130b90ef9e94061aa02457de30a6866.png)

**但是有可能在cpu分配时间片的时候处理不得当，或者说我写了数据之后忘了加sleep让自己的进程和对面抢夺read，就可能导致自己写的数据被自己读取，并且在实际开发的过程中我们肯定不可能让写的一方sleep来让出cpu让对方来读取，所以就很可能出现这种情况，这个是没有办法避免的**

**所以我们匿名管道在实际运用的时候一般规定数据流只从一端流向另一端，不会去实现双向的数据流动，因为这样就可能发生自己数据被自己接受的情况**

![image-20230721103607149](https://img-blog.csdnimg.cn/35c9d8ea7dab44eea1a5604b35ec7143.png)

所以在实际的开发过程中我们往往直接关闭一方的写端和另一方的读端，如下所示：

我们想要的就是第三种情况

<img src="https://img-blog.csdnimg.cn/dda85503648f4b1094df0eb38595d41b.png" alt="image-20230721104349892" style="zoom:80%;" />

~~~cpp
#include <cstring>
#include <iostream>
#include <string>
using namespace std;
#include <unistd.h>
#define _size 1024

int main() {
    // 子进程发送数据给父进程，父进程读取到数据输出

    // 在fork之前创建管道，因为要指向一个管道
    int pipefd[2];

    int ret = pipe(pipefd);
    if (ret == -1) {
        perror("pipe");
        return -1;
    }

    // 管道创建成功了，现在创建子进程
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {  // 父进程
        printf("i am parent process , pid : %d\n", getpid());

        char buf[_size] = {0};
        // const char *str = "hello,i am parent";
        while (1) {
            // 读数据
            // read默认是阻塞的
            read(pipefd[0], buf, sizeof(buf));
            printf("parent recv : \"%s\" , pid : %d\n", buf, getpid());
            // 读完清空buf
            bzero(buf, _size);

            // 关闭写端
            close(pipefd[1]);
        }

    } else if (pid == 0) {  // 子进程
        printf("i am child process , pid : %d\n", getpid());

        char buf[_size] = {0};
        const char *str = "hello,i am child";
        while (1) {
            // 写数据
            write(pipefd[1], str, strlen(str));
            sleep(1);

            // 关闭读端
            close(pipefd[0]);
        }
    }

    return 0;
}
~~~

##### 案例(!!!)

实现 ps aux | grep root，父子进程之间通信

![image-20230721115524021](https://img-blog.csdnimg.cn/0e5fa04d7d654d76b5b2384959871fce.png)

代码：(看代码理解!!!)

注意：

父进程需要调用wait()函数来释放子进程，防止出现僵尸进程;

dup2()函数的作用：

~~~cpp
#include <unistd.h>
int dup2(int oldfd , int newfd);

//作用：重定向文件描述符
    //oldfd指向a.txt，newfd指向b.txt
    //调用函数成功后，newfd和b.txt的连接做close(oldfd仍指向a.txt)，newfd指向a.txt
    //oldfd必须是一个有效的文件描述符
    //如果相同则相当于什么都没做
//返回值：
    //newfd，他们都指向的是oldfd之前指向的文件
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define _size 1024

/*
    实现 ps aux 父子进程之间通信

    子进程：ps aux，子进程结束之后将数据发送给父进程
    父进程：获取到数据，打印

    思路：
    子进程需要执行 ps aux 命令，调用exec族函数，但是这些函数的默认输出端是在stdout_fileno
    所以需要使用dup2()函数将其重定向到管道的写端
    将读取的内容存到文本中，然后去执行grep命令即可
*/

int main() {
    // 创建管道
    int pipefd[2];

    int ret = pipe(pipefd);
    if (ret == -1) {
        perror("pipe");
        return -1;
    }

    // 创建子进程
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {
        // 父进程

        // 关闭写端
        close(pipefd[1]);

        // 读
        char buf[_size] = {0};
        int len = -1;

        // 先打开，如果不存在则创建，存在则删除再创建
        // 建议不要重复打开，这里就打开一次然后写，最后关闭即可
        int fd = open("file.txt", O_RDONLY);
        if (fd != -1)            // 存在，将其删除
            remove("file.txt");  // 不能用exec()函数族，因为这就把这个主进程替换了，不会回来
        close(fd);

        // 然后创建一个
        fd = open("file.txt", O_RDWR | O_CREAT, 0664);
        if (fd == -1) {
            perror("open");
            return -1;
        }

        // -1 留一个结束符
        // 循环读
        while ((len = read(pipefd[0], buf, sizeof(buf) - 1)) != 0) {
            if (len == -1) {
                perror("read");
                return -1;
            }

            // printf("%s", buf);
            write(fd, buf, strlen(buf));

            bzero(buf, _size);
        }

        close(fd);

        // grep筛选root
        execlp("grep", "grep", "root", "file.txt", nullptr);

        // 父进程回收子进程资源防止出现僵尸进程
        wait(nullptr);
    } else if (pid == 0) {
        // 子进程

        // 关闭读端
        close(pipefd[0]);

        // 将标准输入stdout_fileno重定向到管道的写端
        // dup2() newfd指向oldfd指向的位置，oldfd被释放
        int ret = dup2(pipefd[1], STDOUT_FILENO);
        if (ret == -1) {
            perror("dup2");
            return -1;
        }

        // 使用exec函数族执行shell命令，他输出靠的是的是stdout_fileno
        // 为防止管道大小不够，循环的去执行保证指令被写完
        while (ret = execlp("ps", "ps", "aux", nullptr))
            if (ret == -1) {
                perror("execlp");
                return -1;
            }
    }

    return 0;
}
~~~

执行结果：

![image-20230721115546729](https://img-blog.csdnimg.cn/e1b4ec013efc409886cbca0e4d35e78d.png)

#### 管道的读写特点

使用管道的时候，需要注意一下几种特殊的情况(假设都是阻塞I/O操作)：

- **所有指向管道写端的文件描述符都关闭了(管道写端引用计数为0)，有进程从管道的读端读数据，那么管道中剩余的数据被读取以后，再次去read()会返回0，就像读到文件末尾一样**

  如图所示，这里的读端计数为2，写端计数为0

  ![image-20230721134808630](https://img-blog.csdnimg.cn/135d998d14ce43f3869a5008c438e534.png)

- **如果有指向管道写端的文件描述符没有关闭(管道写端的引用计数大于0)，而持有管道写端的进程也没有往管道中写数据，这个时候有进程往管道中读取数据，那么管道中剩余的数据被读取完毕之后，再次read阻塞，直到管道中有数据可以读取了才会读取数据并且返回**
  ![image-20230721135603117](https://img-blog.csdnimg.cn/ab41b9e1dd9244bd9101479c5df15cfd.png)

- **如果所有指向管道读端的文件描述符没有关闭(管道读端的引用计数为0)，这个时候有进程向管道中写数据，那么该进程会收到一个信号SIGPIPE，通常会导致进程异常终止**

  ![image-20230721135452517](https://img-blog.csdnimg.cn/85ad9bcd46d944e8a8b681fd41b0d345.png)

- **如果有指向管道读端的文件描述符没有关闭(管道读端的引用计数大于0)，而持有管道读端的进程也没有从管道中读取数据，这时候有进程向管道中写数据，那么在管道被写满的时候再次调用write()会阻塞，直到管道中有空位置才能再次写入数据并返回**
  ![image-20230721140511484](https://img-blog.csdnimg.cn/048e32341e44417bb4580271a0825cf6.png)

总结：

- **读管道：**
  - **管道中有数据，读取会返回实际读到的字节数**
  - **管道中无数据：**
    - **写端全部关闭，read返回0(相当于读到文件的末尾)**
    - **写端没有完全关闭，read阻塞等待**
- **写管道：**
  - **管道读端全部关闭，产生信号SIGPIPE，进程异常终止**
  - **管道读端没有全部关闭：**
    - **管道已满，write阻塞**
    - **管道没有满，write将数据写入，并返回实际写入的字节数**

#### 设置管道非阻塞(fcntl)

~~~cpp
#include <fcntl.h>
#include <unistd.h>

int fcntl(int fd, int cmd, ...); ...当中是可变参数
// 参数：
//     fd：需要操作的文件描述符
//     cmd：表示对文件描述符进行如何操作
//         F_DUPFD 复制文件描述符，复制的是第一个参数，得到一个新的文件描述符(返回值)
//             int ret = fcntl(fd,F_DUPFD);
//         F_GETFL 获取指定文件描述符的文件状态flag
//             获取的flag和我们通过open函数传递的flag是一个东西
//         F_SETFL 设置文件描述符的文件状态flag
//             必选项：O_RDONLY O_WRONLY O_RDWR 不可以被修改
//             可选项：O_APPEND O_NONBLOCK
//                 O_APPEND 表示追加数据
//                 O_NONBLOCK 设置成非阻塞
//                     阻塞和非阻塞：描述的是函数调用的行为
~~~

如何设置？

~~~cpp
int flags = fcntl(pipefd[0], F_GETFL);
flags |= O_NONBLOCK;
fcntl(pipefd[0], F_SETFL, flags);
~~~

代码：

~~~cpp
#include <cstring>
#include <iostream>
#include <string>
using namespace std;
#include <fcntl.h>
#include <unistd.h>
#define _size 1024

/*
    设置管道非阻塞
    int flags =  fcntl(fd[0],F_GETFL); //获取原来的flag
    flags | = O_NONBLOCK; //修改flag的值
    fcntl(fd[0],F_SETFL,flags); //设置新的flag
*/

int main() {
    // 子进程发送数据给父进程，父进程读取到数据输出

    // 在fork之前创建管道，因为要指向一个管道
    int pipefd[2];

    int ret = pipe(pipefd);
    if (ret == -1) {
        perror("pipe");
        return -1;
    }

    // 管道创建成功了，现在创建子进程
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {  // 父进程
        printf("i am parent process , pid : %d\n", getpid());

        char buf[_size] = {0};

        int flags = fcntl(pipefd[0], F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(pipefd[0], F_SETFL, flags);

        // 关闭写端
        close(pipefd[1]);

        while (1) {
            // 读数据
            // read默认是阻塞的
            // 设置成为非阻塞
            int len = read(pipefd[0], buf, sizeof(buf));
            printf("len = %d\n", len);
            printf("parent recv : \"%s\" , pid : %d\n", buf, getpid());
            // 读完清空buf
            bzero(buf, _size);

            sleep(1);  // 子进程和父进程睡眠的时间不同，这样可以方便的观察是否阻塞
        }

    } else if (pid == 0) {  // 子进程
        printf("i am child process , pid : %d\n", getpid());
        const char *str = "hello,i am child";

        // 关闭读端
        close(pipefd[0]);

        while (1) {
            // 写数据
            write(pipefd[1], str, strlen(str));
            sleep(5);
        }
    }

    return 0;
}
~~~

执行结果：

可见子进程在睡眠的时候父进程执行到read()并没有阻塞，而是执行走了!!!

![image-20230721145953793](https://img-blog.csdnimg.cn/1f61e6d4587449f185f754abbb0dec91.png)

#### 有名管道(FIFO)

**有名管道和匿名管道的区别在于：匿名管道本身没有一个文件描述符或者说路径可以让两个进程找到他，这就导致我们只能通过某种方式让两个进程指向同一块管道，比如主进程先建立管道，然后创建子进程，这样保证了两个进程的读端和写端的文件描述符指向的是同样的匿名管道的两端，这样就只能用于亲缘关系的进程之间通信，而有名管道则恰好克服了这个问题；设置了一个路径名方便两个进程关联，并且这个路径名可以像文件一样被访问(FIFO)，这样就可以被任意关系的两个进程找到并且建立通信**

![image-20230721150130194](https://img-blog.csdnimg.cn/82969707e2a24ab986a2f06648a36e55.png)

区别：

![image-20230721151257066](https://img-blog.csdnimg.cn/d70b7067050f40189192979343118254.png)

##### mkinfo()

![image-20230721151329604](https://img-blog.csdnimg.cn/e2371fae53a749e2b07ffd4458b02f6e.png)

**通过命令 mkfifo < name >**

![image-20230721152717230](https://img-blog.csdnimg.cn/cd1075eed2dc4539982d9b0ab6857d0d.png)

这里创建失败，为什么呢？因为windows系统的文件系统不支持管道文件(匿名管道没有管道文件)

![image-20230721151935217](https://img-blog.csdnimg.cn/2cfcc74c790d456496232ac72e6c339b.png)

在linux系统自己的本地文件夹当中创建

![image-20230721152127780](https://img-blog.csdnimg.cn/635913278bce418c83d9024839f6ded7.png)

**观察发现fifo文件的大小为0，这是因为fifo管道文件的信息是存储在内核的缓冲区里面的，当程序结束之后便会清空，留给下一次使用**

**通过函数 mkfifo()**

注意这里的路径也是linux本地文件夹的路径，否则就会被拒绝

~~~cpp
// - 通过函数
 	#include <sys/stat.h>
	#include <sys/types.h>

    int mkfifo(const char *pathname, mode_t mode);
// 参数：
//     pathname：管道名称的路径
//     mode：文件的权限 和 open 的 mode 一样，八进制数
// 返回值：
//     成功 返回0
//     失败 返回-1，并设施errno
~~~

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    // 判断文件是否存在
    // access()函数可以获取文件的权限和查看是否存在
    int ret = access("/home/lzx0626/fuck/fifo", F_OK);
    if (ret == -1) {
        printf("管道不存在，创建管道\n");

        ret = mkfifo("/home/lzx0626/fuck/fifo", 0664);
        if (ret == -1) {
            perror("mkfifo");
            return -1;
        }
    }

    return 0;
}
~~~

现在我需要写两个进程并且通过有名管道来实现通信，实现write.cpp和read.cpp

~~~cpp
// write.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define _size 1024

// 向管道中写数据
int main() {
    // 判断管道是否存在，不存在则创建
    int ret = access("/home/lzx0626/fuck/fifo", F_OK);
    if (ret == -1) {
        printf("管道不存在，创建管道\n");

        ret = mkfifo("/home/lzx0626/fuck/fifo", 0664);
        if (ret == -1) {
            perror("mkfifo");
            return -1;
        }
    }

    // 打开管道，以只写的方式
    int fd = open("/home/lzx0626/fuck/fifo", O_WRONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // 写数据
    for (int i = 0; i < 100; ++i) {
        char buf[_size];
        sprintf(buf, "hello, %d", i);
        printf("write data : %s\n", buf);
        write(fd, buf, strlen(buf));
        sleep(1);
    }

    close(fd);

    return 0;
}
~~~

~~~cpp
// read.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>
#define _size 1024

// 向管道中读数据
int main() {
    // 打开管道文件，以只读的方式
    int fd = open("/home/lzx0626/fuck/fifo", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // 读取数据
    char buf[_size] = {0};
    while (1) {
        int len = read(fd, buf, sizeof(buf));
        if (len == 0) {
            printf("写端断开连接了...\n");
            break;
        }
        printf("recv buf : %s\n", buf);
        bzero(buf, sizeof(buf));
    }

    close(fd);

    return 0;
}
~~~

执行结果：(本来想写注意事项的，都在下面了)

![image-20230721164727872](https://img-blog.csdnimg.cn/dbb61f0813ee45ac8de2654c0635d50e.png)

![image-20230721164735118](https://img-blog.csdnimg.cn/856494b4dd994b9abcaaf41b806ad20a.png)

**有名管道的注意事项：**

- **一个为读而打开一个管道的进程会阻塞，直到另一个进程为写打开管道**

- **一个为写而打开一个管道的进程会阻塞，直到另一个进程为读打开管道**
  **(可见有名管道的实现还是非常严谨的，双方没有就位不开放)**

  (所以测试程序当中先后打开两个进程先打开的进程会等待后打开的进程，这里阻塞就是因为这个，并且是阻塞在open()函数的位置)

**读管道：**

- **管道中有数据，read会返回实际读取到的数据**
- **管道中无数据：**
  - **管道写端被全部关闭，read返回0(相当于读到文件末尾)**
  - **管道写端没有被完全关闭，read阻塞等待**

**写管道：**

- **管道读端被全部关闭，进程异常终止(收到 SIGPIPE信号)**
- **管道读端没有全部关闭：**
  - **管道已经满了，write会阻塞**
  - **管道没有满，write会将数据写入，并且返回实际写入的字节数**

#### 有名管道通信案例

实现一个简易聊天的功能，循环读写，我写你读，你写我读

##### 思路

![image-20230721170443404](https://img-blog.csdnimg.cn/44990241d935479c8f9556cbecca7770.png)

需要注意一点的就是两个进程是你来我往的，所以需要一方先写，另一方先读，否则就会导致阻塞

~~~cpp
//一方的代码，另一方稍加修改即可
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define _size 1024

int main() {
    // 判断有名管道文件1 2是否存在
    int ret = access("/home/lzx0626/FIFO/fifo1", F_OK);
    if (ret == -1) {
        printf("管道1不存在,创建相关的管道文件\n");
        ret = mkfifo("/home/lzx0626/FIFO/fifo1", 0664);
        if (ret == -1) {
            perror("mkfifo");
            return -1;
        }
    }

    ret = access("/home/lzx0626/FIFO/fifo2", F_OK);
    if (ret == -1) {
        printf("管道2不存在,创建相关的管道文件\n");
        ret = mkfifo("/home/lzx0626/FIFO/fifo2", 0664);
        if (ret == -1) {
            perror("mkfifo");
            return -1;
        }
    }

    // 以只写的方式打开管道1
    int fdw = open("/home/lzx0626/FIFO/fifo1", O_WRONLY);
    if (fdw == -1) {
        perror("open");
        return -1;
    }
    printf("打开管道fifo1成功,等待写入...\n");

    // 以只读的方式打开管道2
    int fdr = open("/home/lzx0626/FIFO/fifo2", O_RDONLY);
    if (fdr == -1) {
        perror("open");
        return -1;
    }
    printf("打开管道fifo2成功,等待读取...\n");

    char buf[_size] = {0};
    // 循环的写读数据
    while (1) {
        // 写数据
        bzero(buf, sizeof(buf));
        // 获取标准输入的数据
        fgets(buf, sizeof(buf) - 1, stdin);  // 包含了回车符号
        // 写数据
        int ret = write(fdw, buf, sizeof(buf) - 1);
        if (ret == -1) {
            perror("write");
            return -1;
        }

        // 读数据
        bzero(buf, sizeof(buf));
        int len = read(fdr, buf, sizeof(buf));
        if (len == -1) {
            perror("read");
            return -1;
        }
        if (len == 0)
            break;
        printf("buf : %s", buf);
    }

    // 关闭
    close(fdw);
    close(fdr);

    return 0;
}
~~~

问题来了，这个程序只能我写了你读，然后你写了我读，不能实现随意的交流，因为程序的逻辑就是写了之后读，读了之后写，如果写了之后再写，由于读是阻塞在那里的，所以写的东西会存在终端stdin的缓冲区当中，直到read()之后再刷新

现在我想让随时通信，随意的读写，这样不妨可以联想到可以让读和写的操作独立起来，可以用子进程的方式，父进程绑定读，子进程绑定写，这样就可以实现了

~~~cpp
//一方的代码，另一方稍微修改一些就好了
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define _size 1024

int main() {
    // 父进程写，子进程读
    int pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {
        // 负责写，绑定管道fifo1
        int ret = access("/home/lzx0626/FIFO/fifo1", F_OK);
        if (ret == -1) {
            printf("管道文件fifo1不存在,正在创建...\n");
            // 不存在则创建
            ret = mkfifo("/home/lzx0626/FIFO/fifo1", 0664);
            if (ret == -1) {
                perror("mkfifo");
                return -1;
            }
        }

        // 打开管道
        int fdw = open("/home/lzx0626/FIFO/fifo1", O_WRONLY);
        if (fdw == -1) {
            perror("open");
            return -1;
        }
        printf("父进程管道已打开,等待写入...\n");

        // 写数据
        char buf[_size] = {0};
        while (1) {
            bzero(buf, sizeof(buf));
            fgets(buf, sizeof(buf) - 1, stdin);
            printf("send : %s", buf);
            int len = write(fdw, buf, sizeof(buf) - 1);
            if (len == -1) {
                perror("write");
                return -1;
            }
        }
        // 关闭
        close(fdw);
        // 父进程回收子进程
        wait(NULL);
    } else if (pid == 0) {
        // 负责读，绑定管道fifo2
        int ret = access("/home/lzx0626/FIFO/fifo2", F_OK);
        if (ret == -1) {
            // 不存在则创建
            printf("管道文件fifo2不存在,正在创建...\n");
            ret = mkfifo("/home/lzx0626/FIFO/fifo2", 0664);
            if (ret == -1) {
                perror("mkfifo");
                return -1;
            }
        }

        // 打开管道
        int fdr = open("/home/lzx0626/FIFO/fifo2", O_RDONLY);
        if (fdr == -1) {
            perror("open");
            return -1;
        }
        printf("子进程管道已打开,等待读取...\n");

        // 读数据
        char buf[_size] = {0};
        while (1) {
            bzero(buf, sizeof(buf));
            int len = read(fdr, buf, sizeof(buf));
            if (len == -1) {
                perror("read");
                return -1;
            }
            if (len == 0)  // 读端全部关闭，相当于读到文件末尾
                break;
            printf("recv : %s", buf);
        }
        // 关闭
        close(fdr);
    }

    return 0;
}
~~~

### 内存映射

将磁盘文件的数据映射到内存，用户修改内存就能修改磁盘文件

![image-20230722143701427](https://img-blog.csdnimg.cn/ab4f9d1a8b994affb2af944d88540148.png)

#### 相关函数(!!!)

mmap()用来建立映射，munmap()用来取消映射

![image-20230722143806582](https://img-blog.csdnimg.cn/90cfd09ed9ea4d0ea1f045749436fc93.png)

~~~cpp
    #include <sys/mman.h>

    void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
// 作用：将一个文件或者设备的数据映射到内存中
// 参数：
//     void *addr：绝大多数情况传nullptr，表示由内核决定映射的内存地址在哪里
//     length：要映射的数据的长度，这个值不能为0，建议使用文件的长度
//         获取文件的长度：stat lseek
//         这个虚拟地址的应映射是以分页为管理的，如果映射的长度不满页的大小，会自动调整为一个分页的大小
//     prot：对我们申请的内存映射区的操作权限
//         - PROT_EXEC  可执行的权限
//         - PROT_READ  读的权限
//         - PROT_WRITE 写的权限
//         - PROT_NONE  没有权限
//         要操作映射区，必须要有读权限，一般给 读权限 或者 读权限和写权限都有 (按位或)
//     flags：
//         - MAP_SHARED 映射区的数据会自动和磁盘文件进行同步，进程间通信必须设置这个选项
//         - MAP_PROVATE 不同步，内存映射区的数据改变了，对原来的文件不会修改，会重新创建一个新的文件(copy on write)
//     fd：需要映射的文件的文件描述符
//         通过open()得到，open的是一个磁盘文件
//         注意，文件的大小不能为0；open指定的权限不能和prot参数的权限有冲突
//         比如，   prot：PROT_READ                    open：只读/读写
//                 prot：PROT_READ | PROT_WRITE       open：读写
//     offset：映射时候的偏移量，必须指定的是4K的整数倍，0表示不偏移
// 返回值：
//     成功 返回创建好的映射区的内存首地址
//     失败 返回 MAP_FAILED (void *)-1，并且设置errno

	int munmap(void *addr, size_t length);
// 功能：释放内存映射
// 参数：
//     addr：要释放的内存映射的首地址
//     length：要释放的内存大小，要和mmap()的length参数值一样
// 

// 
// 使用内存映射实现进程之间通信
// 1.有关系的进程，父子进程
//     在没有子进程的时候，通过唯一的父进程先通过一个大小不是0的磁盘文件创建内存映射区，有了之后再创建子进程，然后父子共享这个内存映射区
// 2.没有关系的进程间通信
//     准备一个大小不是0的磁盘文件
//     进程1通过磁盘文件得到一个内存映射区，得到一个操作这个内存的指针
//     进程2同理，得到一个指针
//     使用内存映射区进行通信

// 注意：内存映射区通信，不会阻塞
~~~

父子进程通过内存映射区通信的例子：

**思路：通信？内存映射的本质是将文件映射到内存当中形成一块区域，和父子进程联系起来，不妨联想到了匿名管道，父进程在创建子进程之前就创建好内存映射区，然后fork()出子进程，这样父子进程就指向了同一块内存映射区了，就可以互相通信了**

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define _size 1024

int main() {
    // 打开1个文件
    int fd = open("test.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    // 获取大小
    off_t size = lseek(fd, 0, SEEK_END);

    // 创建内存映射区
    void* ptr = mmap(nullptr, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    // 创建子进程
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {   // 父进程
        wait(NULL);  // 等待子进程写入数据然后回收完毕再读取

        // 读数据
        char buf[_size] = {0};
        strcpy(buf, (char*)ptr);
        printf("read data : %s\n", buf);
    } else if (pid == 0) {  // 子进程
        // 写数据，注意是直接操作这个指针，和管道不一样，管道是通过文件描述符操作
        // 我写的字符串后面带有 '\0' 结束符，不用担心会和原来文件的数据冲突，因为我是从头开始覆盖，然后走到尾部自动补上 '\0'，读操作也是一样的
        strcpy((char*)ptr, "nihao");
    }

    // 关闭内存映射区
    int ret = munmap(ptr, size);
    if (ret == -1) {
        perror("munmap");
        return -1;
    }

    // 关闭文件
    close(fd);

    return 0;
}
~~~

**注意：'\0'就是 char(0)，所以我才用 char buf[_size] = {0} 对字符串初始化!!!**

不相关的进程之间通过内存映射通信

思路：写进程先打开文件，创建映射区，然后修改数据，然后读进程打开文件，创建映射区然后读取修改后的数据；我这里的设计两个进程不能并发执行，因为读进程不是阻塞的，这样读取的是文件中原本的数据，需要等待写进程写完数据之后再执行

~~~cpp
// write.cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#define _size 1024

int main() {
    // 打开文件
    int fd = open("test.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // 获取文件大小
    struct stat statbuf;
    int ret = stat("test.txt", &statbuf);
    if (ret == -1) {
        perror("stat");
        return -1;
    }
    off_t size = statbuf.st_size;

    // 创建内存映射区
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    // 操作这块内存
    char buf[_size] = {0};
    fgets(buf, sizeof(buf) - 1, stdin);  // 保证后面留有一个'\0'符号
    // 写数据
    strcpy((char*)ptr, buf);

    // 关闭内存映射区
    munmap(ptr, size);
    // 关闭文件
    close(fd);

    return 0;
}
~~~

~~~cpp
// read.cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#define _size 1024

int main() {
    // 打开文件
    int fd = open("test.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // 获取文件大小
    struct stat statbuf;
    int ret = stat("test.txt", &statbuf);
    if (ret == -1) {
        perror("stat");
        return -1;
    }
    off_t size = statbuf.st_size;

    // 创建内存映射区
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    // 操作这块内存
    char buf[_size] = {0};
    // 读数据
    strcpy(buf, (char*)ptr);
    printf("read data : %s", buf);

    // 关闭内存映射区
    munmap(ptr, size);
    // 关闭文件
    close(fd);

    return 0;
}
~~~

执行结果：

![image-20230722155039444](https://img-blog.csdnimg.cn/b0dfec023cba42fc8274bbc7997fadfe.png)

**但是要注意一点，写的数据不能比文件本身的大小大，不然就会超出内存的大小范围了，就会写不进去!!!**

#### 思考问题

1.如果对mmap的返回值(ptr)做++操作(ptr++), munmap是否能够成功?
void * ptr = mmap(...);
ptr++;  可以对其进行++操作
munmap(ptr, len);   // 错误,要保存地址

2.如果open时O_RDONLY, mmap时prot参数指定PROT_READ | PROT_WRITE会怎样?
错误，返回MAP_FAILED
open()函数中的权限建议和prot参数的权限保持一致。

3.如果文件偏移量为1000会怎样?
**偏移量必须是4K的整数倍，返回MAP_FAILED**

4.mmap什么情况下会调用失败?
    - 第二个参数：length = 0
    - 第三个参数：prot
        - 只指定了写权限
        - prot PROT_READ | PROT_WRITE
          第5个参数fd 通过open函数时指定的 O_RDONLY / O_WRONLY

5.可以open的时候O_CREAT一个新文件来创建映射区吗?
    - 可以的，但是创建的文件的大小如果为0的话，肯定不行
        - 可以对新的文件进行扩展
        - lseek()
        - truncate()

6.mmap后关闭文件描述符，对mmap映射有没有影响？
    int fd = open("XXX");
    mmap(,,,,fd,0);
    close(fd); 
    映射区还存在，创建映射区的fd被关闭，没有任何影响。

7.对ptr越界操作会怎样？
void * ptr = mmap(NULL, 100,,,,,);
映射出来会调整为**4K**
越界操作操作的是非法的内存 -> 段错误

#### 示例

**将english.txt文件拷贝一份为cpy.txt，保存在当前目录**

思路：两个文件分别映射到内存当中，然后操纵内存进行复制即可

**注意：新文件需要预分配大小，不能出现空文件**

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// 将english.txt文件拷贝一份为cpy.txt，保存在当前目录
int main() {
    // 打开english.txt
    int fd_src = open("english.txt", O_RDONLY);
    if (fd_src == -1) {
        perror("open");
        return -1;
    }

    // 获取english.txt的大小
    off_t size = lseek(fd_src, 0, SEEK_END);

    // 创建内存映射区
    void* ptr_src = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd_src, 0);
    if (ptr_src == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    // 创建cpy.txt
    int ret = access("copy.txt", F_OK);
    if (ret == 0)  // 存在把他删除
        unlink("cpy.txt");
    // 创建
    int fd_dest = open("cpy.txt", O_RDWR | O_CREAT, 0664);
    if (fd_dest == -1) {
        perror("open");
        return -1;
    }
    // 将空文件的大小修改为源文件的大小，防止出现空文件
    ret = truncate("cpy.txt", size);
    // ret = lseek(fd_dest, size, SEEK_END);
    // write(fd_dest, " ", 1);  // lseek扩展文件需要进行一次写的操作，truncate不需要!!!
    if (ret == -1) {
        perror("truncate");
        return -1;
    }

    // 创建内存映射区
    void* ptr_dest = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dest, 0);
    if (ptr_dest == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    // 拷贝
    strncpy((char*)ptr_dest, (char*)ptr_src, size);

    // 关闭内存映射区
    munmap(ptr_src, size);
    munmap(ptr_dest, size);
    // 关闭文件
    close(fd_src);
    close(fd_dest);

    return 0;
}
~~~

这里我们得到了两种扩展文件内存的方式，分别使用truncate()和lseek()

- **truncate()中，size是想要扩展到的文件大小**
- **lseek()中，length是在SEEK_END基础上的文件指针偏移量，最后文件指针会走到length+SEEK_END的位置，这是原来文件走不到的位置，因此文件就相应扩展了，length就是扩展了的文件大小**
- **lseek扩展文件需要进行一次写的操作，truncate不需要!!!**

~~~cpp
truncate("cpy.txt", size);

lseek(fd_dest, length, SEEK_END);
write(fd_dest, " ", 1);  // lseek扩展文件需要进行一次写的操作，truncate不需要!!!
~~~

#### 匿名映射

**顾名思义，匿名映射，都匿名了，说明没有文件实体做支撑了吧，因此两个不相关的进程不适用于这个，因为找不到连接的接口，而父子进程恰好可以用这个来通信**

**flags参数当中，MAP_SHARED和MAP_PRIVATE参数是必选一个，然后其他的是可选项，MAP_ANONYMOUS就是一个可选项**

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define _size 1024

/*
匿名映射：不需要文件实体进行一个内存映射，只能在父子和有关系的进程之间通信，因为没有办法通过文件进行关联
- 修改flags参数，做匿名映射需要传入 MAP_ANONYMOUS，这样会忽略掉fd参数，一般我们传入-1
- flags参数当中，MAP_SHARED和MAP_PRIVATE参数是必选一个，然后其他的是可选项，MAP_ANONYMOUS就是一个可选项
*/

int main() {
    // 创建匿名内存映射区
    int length = 4096;
    void* ptr = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    // 父子进程间通信
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {  // 父进程
        strcpy((char*)ptr, "helloworld");
        wait(nullptr);
    } else if (pid == 0) {  // 子进程
        sleep(1);           // 非阻塞，先睡一秒让父进程执行完写入操作
        printf("%s\n", (char*)ptr);
    }

    // 关闭内存映射区
    int ret = munmap(ptr, length);
    if (ret == -1) {
        perror("munmap");
        return -1;
    }

    return 0;
}
~~~

### 信号

#### 概念

![image-20230722171544342](https://img-blog.csdnimg.cn/aea70e45c0ce4195bc9884c76669d976.png)

软件中断

处理紧急事务，完毕后回到保存的位置继续执行

<img src="https://img-blog.csdnimg.cn/cb3a19e815134daaa4211162b5390f8d.png" alt="image-20230722171944530" style="zoom:67%;" />

#### 目的

**让进程知道了已经发生了某一个特定的事情；强迫进程执行他自己代码当中的信号处理程序**

<img src="https://img-blog.csdnimg.cn/cbfda02d77c24be9b56c08e0283620cc.png" alt="image-20230722172651186" style="zoom: 67%;" />

#### Linux信号列表

**一共62个(32 33没有)，1-31是常规信号，32-64是预定义好的信号，目前还没有使用，将来可能会使用，并且是实时信号**

![image-20230722172945238](https://img-blog.csdnimg.cn/b21fe318e2d3415284459418204f1048.png)

红色信号比较重要：

![image-20230722173136070](https://img-blog.csdnimg.cn/477a33d271084c76899e5be86eae5a04.png)

![image-20230722173332099](https://img-blog.csdnimg.cn/596796ebe4504fe2acb32e2c0b1a59c6.png)

![image-20230722173427247](https://img-blog.csdnimg.cn/a2d120fe441341439a2a7170852a2629.png)

![image-20230722173511150](https://img-blog.csdnimg.cn/2d085be8839a4789aabe53ccc135c5ba.png)

#### 信号的5种默认处理动作

其中，Core是指终止进程并且生成一个Core文件，这个文件当中记载了程序异常终止时候保存的一些错误信息等等

![image-20230722173544492](https://img-blog.csdnimg.cn/c4b2644e692144068d65a43b7cbd6113.png)

##### 查看并且使用Core文件

Core文件里面记录了程序异常退出的状态信息，可以让程序在异常退出的时候生成Core文件，方便我们查看和调试错误的信息

系统默认在进程异常退出的时候是不会产生Core文件的，通过 ulimit -a 命令查看：

可以看出 core file size 这一项默认是0

![image-20230722190246653](https://img-blog.csdnimg.cn/19cb6adc8f0b4cd1aabe545de5fe4ea1.png)

我们将其设置一下，可以设置为一个具体的大小或者不限制

![image-20230722190333227](https://img-blog.csdnimg.cn/0c020dc61caa45d28ed6242c9267f8e0.png)

现在就应该有了

还要注意一点：这个Core文件要想被记录下来，必须在Linux的本地目录当中操作，不能在和windows的共享文件夹或者windows文件夹当中操作，这样出来的Core文件是会生成，但是大小为0，没有用

源代码：

~~~cpp
#include <iostream>
using namespace std;
#include <cstring>

int main() {
    char* buf;

    strcpy(buf, "hello");
    //这里显然会报错，因为buf指针没有被初始化，根本不知道操作的是哪一块内存，指向的字符串区域有多大，是个野指针

    return 0;
}
~~~

<img src="https://img-blog.csdnimg.cn/5087a798952a49849252f9c4b3c669c2.png" alt="image-20230722190630746" style="zoom:67%;" />

执行 a.out 之后，报了段错误，我们来查看下core文件的大小，不为0，可见记录了相关错误信息

<img src="https://img-blog.csdnimg.cn/ee83f2681d3844179ef5efa6ce51ad17.png" alt="image-20230722190751504" style="zoom:80%;" />

怎么查看呢？可以通过gdb调试来查看，这也是为什么我们编译的时候加上 -g 调试参数的原因

通过gdb调试可执行文件a.out，里面有一个命令

~~~shell
core-file core #用来查看该可执行文件对应的core文件信息
~~~

<img src="https://img-blog.csdnimg.cn/c02b8801212e464caca55b8a53a589ce.png" alt="image-20230722190919579" style="zoom:80%;" />

可以看出程序的异常终止是因为这一行的问题，与我们的预期一致；

而发出的终止信号SIGSEGV的含义就是程序进行了无效的内存访问，也就是段错误

#### 相关函数

##### kill(),raise(),abort()

![image-20230722174710585](https://img-blog.csdnimg.cn/0254f8325770424498c423cfcb61d87a.png)

~~~cpp
    #include <signal.h>

    int kill(pid_t pid, int sig);
// 作用：给任何的进程或者进程组pid，发送任何的信号sig
// 参数：
//     pid：
//         >  0  将信号发送给指定的进程
//         == 0  将信号发送给当前的进程组中所有的进程
//         == -1 将信号发送给每一个有权限接受这个信号的进程
//         <  -1 这个pid = 某个进程组的ID的相反数，给这个进程组中所有的进程发送信号

//     sig：需要发送的信号编号或者宏值，如果是0则表示不发送任何信号
// 返回值：成功 0 ; 失败 -1，并设置errno


    int raise(int sig);
// 作用：给当前进程发送信号
// 参数：sig：需要发送的信号编号或者宏值，如果是0则表示不发送任何信号
// 返回值： 成功 0 ; 失败 非 0
//     kill(getpid(),sig);


    void abort(void);
// 功能：发送一个SIGABRT信号给当前的进程，默认是杀死当前的进程
//     kill(getpid(),SIGABRT);
~~~

测试程序：

~~~cpp
#include <iostream>
using namespace std;
#include <signal.h>

int main() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {
        printf("parent process\n");
        sleep(2);
        printf("kill child process now\n");
        kill(pid, SIGINT);
    } else if (pid == 0) {
        for (int i = 0; i < 5; ++i) {
            printf("child process\n");
            sleep(1);
        }
    }

    return 0;
}
~~~

执行结果：

<img src="https://img-blog.csdnimg.cn/85ac2901f0124ec48126d4cff3bffa6e.png" alt="image-20230722193118885" style="zoom:67%;" />

<img src="https://img-blog.csdnimg.cn/03b746110e2c4b958a69f09240244806.png" alt="image-20230722193131543" style="zoom:67%;" />

**有可能为2次也有可能为3次，因为父子进程是并发执行的，谁先运行要看抢占cpu，这里如果是父进程先执行就是2次，子进程先执行就会先偷偷打印一次，就是3次，这也体现了多进程程序执行结果的不确定性，这完全取决于操作系统对于进程的调度**

##### alarm()

**这里面请注意alarm()函数的返回值和设置参数为0时候的情况**

定时器到时间之后程序会接受 SIGALARM 信号，然后终止

~~~cpp
    #include <unistd.h>

    unsigned int alarm(unsigned int seconds);
// 作用：去设置定时器；函数调用，开始倒计时；
//     当倒计时为0的时候，函数会给当前的进程发送一个信号 SIGALARM
// 参数：
//     seconds：倒计时时长，单位秒，当参数为0的时候，计时器无效(不进行倒计时，也不发送信号)
//         取消一个定时器 alarm(0)
// 返回值：
//     - 之前没有定时器，返回0
//     - 之前有定时器，返回之前定时器剩余的时间

// - SIGALARM 信号：默认终止当前的进程，每一个进程都有且只有唯一的一个定时器
//     alarm(10); ->返回0
//     //过了一秒
//     alarm(5);  ->返回9

// 该函数不阻塞，设置之后会继续往下执行
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    int seconds = alarm(5);             // 不阻塞
    printf("seconds : %d\n", seconds);  // 0

    sleep(2);
    seconds = alarm(2);
    printf("seconds : %d\n", seconds);  // 3

    while (1)
        ;

    return 0;
}
~~~

执行结果：

![image-20230722200410797](https://img-blog.csdnimg.cn/4b8a26e7aae4421997723676fe0862eb.png)

案例：查看计算机一秒能输出多少个数

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

// 1秒钟电脑能数多少个数
int main() {
    alarm(1);

    int i = 0;
    while (1)
        printf("%d\n", i++);

    return 0;
}
~~~

执行结果：

![image-20230722200801779](https://img-blog.csdnimg.cn/64b892140d2b4a3fb1e4430338182c3b.png)

但是我们观察到输出完毕花的时间好像不止1秒，这是为什么呢？

**注意到这里输出的数好多好多，而终端的输出是依靠内核当中的012文件描述符，0标准输入STDIN_FILENO，1标准输出STDOUT_FILENO，2标准错误STDERR_FILENO，然后要想输出到终端必须要经历特定的事件，比如程序终止或者遇到回车'\n'等等，这是因为终端有缓冲区的存在；之所以花了超过1秒是因为从缓冲区输出到屏幕上，还是输出了这么多的数据，花费了大量时间**

**但是这里只数了20万不到，感觉少太多了，这是因为往终端上输出的时候需要调用文件描述符，输出一次调用一次磁盘I/O，这样浪费时间，CPU没有百分百去执行数的这个操作，我们可以将其输出重定向到一个文本当中，这样只用调用一次文件I/O就可以把文件写在里面**

![image-20230722201704262](https://img-blog.csdnimg.cn/f9a3c1bc73f142dc82eb7133171c47fe.png)

<img src="https://img-blog.csdnimg.cn/8dcfd915f6d04faba461816cee62c517.png" alt="image-20230722201732964" style="zoom:67%;" />

可见这样就多了很多

**实际的时间 = 内核时间 + 用户时间 + 消耗的时间(比如I/O操作)**

**进行文件I/O操作的时候比较浪费时间**

**定时器，和进程的状态无关(自然定时法)；无论进程处于什么状态，这个alarm()都会计时**

##### settimer()

注意注释的介绍

~~~cpp
    #include <sys/time.h>

    int setitimer(int which, const struct itimerval *restrict new_value,
                struct itimerval *_Nullable restrict old_value);
// 作用：设置定时器；可以替代alarm函数。精度可以达到微秒，并且还可以实现周期性的定时
// 参数：
//     which：指定的是定时器以什么时间计时
//         - ITIMER_REAL：真实时间(包含内核+用户+消耗的时间(例如I/O))，时间到达发送 SIGALRM 常用
//         - ITIMER_VIRTUAL：用户时间，时间到达发送 SIGVTALRM
//         - ITIMER_PROF：以该进程在用户态和内核态所消耗的时间来计算，时间到达发送 SIGPROF

//     new_value：设置定时器属性

//         struct itimerval { //定时器的结构体
//             struct timeval it_interval;  // 每个阶段的时间，间隔时间
//             struct timeval it_value;     // 延迟多长时间执行定时器
//         };

//         struct timeval { //时间的结构体
//             time_t tv_sec;        // 秒数
//             suseconds_t tv_usec;  // 微秒
//         };

//         eg：过10秒(it_value)后，每隔2秒(it_interval)定时一次

//     old_value：记录上一次定时的时间参数，是一个传出参数，函数将上一次的状态心如进去，一般不使用，就指定nullptr就可以了
// 返回值：
//     成功 0
//     失败 -1，设置errno
~~~

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <sys/time.h>

// 过3秒以后，每隔2秒定时一次
int main() {
    // 过三秒，会发送信号
    struct timeval _value;
    _value.tv_sec = 3;
    _value.tv_usec = 0;

    // 每隔两秒，会发送信号
    struct timeval _interavl;
    _interavl.tv_sec = 2;
    _interavl.tv_usec = 0;

    // itimerval结构体
    struct itimerval new_value;
    new_value.it_value = _value;
    new_value.it_interval = _interavl;

    // 设置定时器
    int ret = setitimer(ITIMER_REAL, &new_value, nullptr);  // 非阻塞
    printf("定时器开始了\n");                               // 立刻执行，表明是非阻塞的
    if (ret == -1) {
        perror("setitimer");
        return -1;
    }

    while (1)
        ;

    return 0;
}
~~~

**3秒的延迟开始和2秒的定时间隔到了后都会发送信号，因此程序执行下来是在3秒之后就退出了，这是3秒的延迟开始的信号**

**而且"定时器开始了"这句话是立刻开始的，这就表明这个函数是非阻塞的，这个函数执行后，虽然定时器还没有开始，但是程序继续在执行**

**但是这样看起来没有办法实现每2秒一次的效果，这就需要捕捉信号，这样才能做我们自己的事情**

#### 信号捕捉函数

![image-20230723150743944](https://img-blog.csdnimg.cn/ade91a3110bd4794b278a63120c3c87d.png)

##### signal()

注意回调函数的函数格式定义是有要求的，就是 typedef 那一行，那是个函数指针的写法，**要求我们传入的回调函数返回值必须为void，然后参数是int，这个记录的是捕捉到的信号的编号**

~~~cpp
    #include <signal.h>

    typedef void (*sighandler_t)(int);
    sighandler_t signal(int signum, sighandler_t handler);
// 作用：设置某个信号的捕捉行为，
// 参数：
//     signum：要捕捉的信号
//         注意：SIGKILL 和 SIGSTOP 不能被捕捉，不能被忽略；
//             因为这两个信号都是带有强制性的杀死或者暂停进程，这个是需要保证权威的，否则强制性都解决不了就可以被不法分子利用了，比如制作病毒让进程一直运行消耗资源，这两个信号没有办法解决
//     hander：捕捉到信号要如何处理
//         SIG_IGN：忽略信号
//         SIG_DFL：用信号默认的行为
//         回调函数：这个函数是内核调用，程序员只负责写，捕捉到信号后如何去处理信号
//             回调函数需要程序员实现，提前准备好，函数的类型根据实际需求，看函数指针的定义
//             不是程序员调用的，而是当信号产生由内核调用
//             函数指针是实现回调的手段，函数实现后，将函数名放到函数指针的位置就可以了

// 返回值：
//     成功，返回上一次注册的信号处理函数的地址；第一次返回nullptr
//     失败，返回SIG_ERR，设置errno
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <signal.h>
#include <sys/time.h>

void myalarm(int num) {
    printf("捕捉到了信号的编号是: %d\n", num);
}

int main() {
    // 注册信号捕捉，需要提前注册，避免定时器开始执行后可能信号捕捉还没生效导致错过信号捕捉的情况
    // signal(SIGALRM, SIG_IGN);  // 信号产生后忽略信号，程序会一直执行
    // signal(SIGALRM, SIG_DFL);  // 按照默认的方式处理信号，程序延迟3秒的时候开始计时，发送信号然后终止

    // typedef void (*sighandler_t)(int); 函数指针的类型，int类型的参数表示捕捉到的信号的值
    sighandler_t ret = signal(SIGALRM, myalarm);
    if (ret == SIG_ERR) {
        perror("signal");
        return -1;
    }

    // 过三秒，会发送信号
    struct timeval _value;
    _value.tv_sec = 3;
    _value.tv_usec = 0;

    // 每隔两秒，会发送信号
    struct timeval _interavl;
    _interavl.tv_sec = 2;
    _interavl.tv_usec = 0;

    // itimerval结构体
    struct itimerval new_value;
    new_value.it_value = _value;
    new_value.it_interval = _interavl;

    // 设置定时器
    int rets = setitimer(ITIMER_REAL, &new_value, nullptr);  // 非阻塞
    printf("定时器开始了\n");                                // 立刻执行，表明是非阻塞的
    if (rets == -1) {
        perror("setitimer");
        return -1;
    }

    while (1)
        ;

    return 0;
}
~~~

#### 信号集

**位图机制：信号集是一堆信号的集合，那么怎么去表示这个信号集呢？我们知道信号是用一个整数的序号表示的(1-31 34-64)，所以我们用类似于文件st_mode那个的形式，用每一位来表示一个信号，0 1 表示信号有无，这样信号集就相当于是一个整数，而想要添加一个信号进去就用这个信号(用信号集的格式表示)按位或就好了**

**信号三种状态：**

**产生：信号产生**

**未决：信号产生到信号被处理之前的这段时间**

**抵达：信号抵达**

**然后阻塞信号是指阻止信号被处理，而不是阻止信号产生；阻塞就是让系统保持信号，留着以后发送**

**我们可以设置阻塞信号集，表示要阻塞哪些信号；而不能修改或者设置未决信号集，我们不能阻止信号的产生；**

**系统PCB当中自带阻塞信号集和未决信号集，我们也不能直接操作，需要借助系统提供的API才能操作**

![image-20230723154213795](https://img-blog.csdnimg.cn/08c60fbba30d4d5aa6a930b773e3bd9d.png)

阻塞信号集和未决信号集(在PCB当中)

- **用户通过键盘 Ctrl + C，产生SIGINT信号，信号被创建**
- **信号产生，但是没有被处理，未决状态，以下是工作过程(也解释了为什么信号产生了不会被立即处理)**
  - **在内核当中，将所有的没有被处理的信号存储在一个集合当中(未决信号集)**
  - **SIGINT信号，状态存储在第二个标志位，这个标志位的值为0说明信号不是未决状态，为1说明信号是未决状态**
- **这个未决状态的信号，需要被处理，处理之前需要和另一个信号集(阻塞信号集)对应的标志位进行比较**
  - **阻塞信号集默认不阻塞所有信号**
  - **如果想要阻塞某些信号，需要用户调用系统的API**
- **在处理的时候和阻塞信号集中的标志位查询，看是不是对该信号设置了阻塞**
  - **没有阻塞，这个信号就会被处理**
  - **如果阻塞了，这个信号就继续处于未决状态，直到阻塞解除，这个信号被处理**

![image-20230723165142990](https://img-blog.csdnimg.cn/0fa70955c5e645f797254c9ae650283e.png)

##### 相关函数

![image-20230723170027725](https://img-blog.csdnimg.cn/3e0ad9eea12445ca8166e5c753485688.png)

**前面五个，都是对自己定义的信号集进行操作，信号集的类型是 sigset_t，本质就是一个数组，下标对应信号signum，值代表是否信号状态**

~~~cpp
	#include <signal.h>

// 以下的信号集相关的函数都是对自定义的信号集进行操作，我们不能直接修改系统当中的未决信号集和阻塞信号集!!!

	int sigemptyset(sigset_t *set);
// 功能：清空信号集中的数据，将信号集中的所有标志位置为0
// 参数：set，传出参数，需要操作的信号集
// 返回值：成功 0；失败 -1，修改errno

	int sigfillset(sigset_t *set);
// 功能：将信号集中的所有标志位置为1
// 参数：set，传出参数，需要操作的信号集
// 返回值：成功 0；失败 -1，修改errno

	int sigaddset(sigset_t *set, int signum);
// 功能：设置信号集中的某一个信号对应的标志位为1，表示阻塞这个信号
// 参数：set，传出参数，需要操作的信号集；signum：需要设置为阻塞的信号
// 返回值：成功 0；失败 -1，修改errno

	int sigdelset(sigset_t *set, int signum);
// 功能：设置信号集中的某一个信号对应的标志位为0，表示不阻塞这个信号
// 参数：set，传出参数，需要操作的信号集；signum：需要设置不为阻塞的信号
// 返回值：成功 0；失败 -1，修改errno

	int sigismember(const sigset_t *set, int signum);
// 功能：判断某个信号是否阻塞
// 参数：set，需要操作的信号集；signum：需要查看是否阻塞的信号
// 返回值：(与前面不一样!!!)
//     1 是成员，signum被阻塞；0 不是成员，不阻塞
//     -1 表示失败，修改errno
~~~

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <signal.h>

void Judge(const sigset_t& set, const int& signum) {
    int ret = sigismember(&set, signum);
    if (ret == -1) {
        perror("sigismember");
        exit(-1);
    }

    if (ret == 1)
        printf("信号%d在set当中\n", signum);
    else if (ret == 0)
        printf("信号%d不在set当中\n", signum);
}

int main() {
    // 创建一个信号集
    sigset_t set;

    // 这么创建的数据一般是随机的，我们一般用系统的api清空
    int ret = sigemptyset(&set);
    if (ret == -1) {
        perror("sigemptyset");
        return -1;
    }

    // 判断SIGINT是否在信号集set中
    Judge(set, SIGINT);  // 2号信号不在

    // 添加几个信号
    ret = sigaddset(&set, SIGINT);
    if (ret == -1) {
        perror("sigaddset");
        return -1;
    }

    ret = sigaddset(&set, SIGQUIT);
    if (ret == -1) {
        perror("sigaddset");
        return -1;
    }

    // 判断是否在信号集set中
    Judge(set, SIGINT);   // 2号信号在
    Judge(set, SIGQUIT);  // 3号信号在

    // 删除一个信号
    ret = sigdelset(&set, SIGQUIT);
    if (ret == -1) {
        perror("sigdelset");
        return -1;
    }

    // 判断SIGQUIT是否在信号集set中
    Judge(set, SIGQUIT);  // 3号信号不在

    return 0;
}
~~~

**sigprocmask()和sigpending()**

**调用之后就可以把我们自己设置的信号集设置到系统提供的阻塞信号集当中，这也是我们唯一能设置系统内核PCB中的信号集，未决信号集不能被设置或者处理，只能被读取**

<img src="https://img-blog.csdnimg.cn/efc1889c57184bf295ac911ac51c922e.png" alt="image-20230724150434261" style="zoom:50%;" />

~~~cpp
    #include <signal.h>

	int sigprocmask(int how, const sigset_t *_Nullable restrict set,
                                   sigset_t *_Nullable restrict oldset);
// 功能：将自定义信号集中的数据设置到内核当中(设置阻塞，接触阻塞，替换)
// 参数：
//     how：如何对内核阻塞信号集进行处理
//         SIG_BLOCK：将用户设置的阻塞信号集添加到内核中，原来的数据不变
//             假设中内核中默认的阻塞信号集是mask，则 mask | set (添加的公式)
//         SIG_UNBLOCK：根据用户设置的数据，对内核中的数据进行接触阻塞
//             mask & = ~ set (去除的公式)
//             比如 mask 1 0 1 1 1 ，set 0 0 1 0 1，解除这两位的阻塞
//             那么就是 ~set 1 1 0 1 0 ，然后想与就得到 1 0 0 1 0
//         SIG_SETMASK：覆盖内核中原来的值

//     set：已经初始化好的用户自定义的信号集
//     oldset：保存的之前内核中的阻塞信号集的状态，传出参数，一般不使用，设置为nullptr即可
// 返回值：
//     成功 0
//     失败 -1，并且设置errno，有两个值：EFAULT，EINVAL

	int sigpending(sigset_t *set);
// 功能：获取内核中的未决信号集
// 参数：set，传出参数，保存的是内核中的未决信号集
// 返回值：
//     成功 0，失败 -1，设置errno
~~~

注意一点，就是在二进制数当中，添加位数为1和解除位数为1(变为0)的操作

~~~cpp
mask | set //添加
mask & = ~set //解除
~~~

现在我们需要写一个程序，用来查看内核当中的未决信号集，并且设置某些信号阻塞，然后再次查看

~~~cpp
#include <iostream>
using namespace std;
#include <signal.h>
#include <unistd.h>

// 编写一个程序，把所有的常规信号(1-31)的未决状态打印到屏幕
// 设置某些信号是阻塞的，通过键盘产生这些信号
int main() {
    // 设置 2号信号 SIGINT(ctrl+C) 和 3号信号SIGQUIT(ctrl+\) 阻塞
    sigset_t set;
    // 清空
    int ret = sigemptyset(&set);
    if (-1 == ret) {
        perror("sigemptyset");
        return -1;
    }

    // 将2号和3号信号添加进去
    ret = sigaddset(&set, SIGINT);
    if (-1 == ret) {
        perror("sigaddset");
        return -1;
    }
    ret = sigaddset(&set, SIGQUIT);
    if (-1 == ret) {
        perror("sigaddset");
        return -1;
    }

    // 修改内核中的信号集
    ret = sigprocmask(SIG_BLOCK, &set, nullptr);
    if (-1 == ret) {
        perror("sigprocmask");
        return -1;
    }

    int count = 0;

    // 在循环当中获取未决信号集的数据
    while (1) {
        sigset_t pendingset;
        ret = sigemptyset(&pendingset);
        if (-1 == ret) {
            perror("sigemptyset");
            return -1;
        }

        sigpending(&pendingset);

        // 遍历前32位 即1-31号(0号没用)
        for (int i = 1; i < 32; ++i) {
            ret = sigismember(&pendingset, i);
            if (-1 == ret) {
                perror("sigismember");
                return -1;
            }

            if (1 == ret)
                printf("1");
            else if (0 == ret)
                printf("0");
        }
        puts("");

        // 为了防止只能通过kill -9 命令杀死该进程，现在我们计数，到10就接触阻塞
        if (count++ == 10) {
            printf("2号信号SIGINT和3号信号SIGQUIT已经解除阻塞\n");
            ret = sigprocmask(SIG_UNBLOCK, &set, nullptr);
            if (-1 == ret) {
                perror("sigprocmask");
                return -1;
            }
        }
        sleep(1);
    }

    return 0;
}
~~~

这里我们设置了10秒后就会解除阻塞，因为我们需要防止这个进程只能通过kill -9命令强制杀死，给自己留一条后路

执行结果：

**可见，当我们输出 ctrl+c 和ctrl+\ 的时候，未决信号集里面添加了这两个信号，但是由于我们设置了阻塞，不会去立即处理，这种情况会持续到我解除他的阻塞才行，所以他很急，但是他没得选择。当我解除了这两个信号的阻塞后，马上就处理了，程序异常终止**

这里输出一个空行是因为第一，字符串我输出了换行，第二，SIGINT信号和SIGQUIT信号执行后都会输出空行，这里是执行了SIGINT信号

![image-20230724155507574](https://img-blog.csdnimg.cn/9099f82e92fa4d54834ad7299bbfc77a.png)

补充：将程序挂到后台执行，加上 & 符号

~~~shell
./a.out &
~~~

程序到后台运行，所以我 ctrl+c 没有用，并且我可以执行我自己的命令，图中就执行了ls和kill -9

并且由于这个进程的输出是默认定向到终端的，所以终端会输出

![image-20230724160250340](https://img-blog.csdnimg.cn/82900528317f4dd9ade23e38734bdc51.png)

切换到前台

~~~shell
fg
~~~

![image-20230724160542261](https://img-blog.csdnimg.cn/1f6fdf918f4d4a3c9f2e391b0205ca62.png)

#### (续信号捕捉函数)sigaction()

~~~cpp
    #include <signal.h>

    int sigaction(int signum,
                        const struct sigaction *_Nullable restrict act,
                        struct sigaction *_Nullable restrict oldact);
// 作用：用来检查或者改变信号的处理，信号捕捉
// 参数：
//     signum：需要捕捉的信号的编号或者宏值
//     act：捕捉到信号之后相应的处理动作
//     oldact：上一次对信号捕捉的相关的设置，一般不使用，传递nullptr
// 返回值：
//     成功 0
//     失败 -1，设置errno

    struct sigaction {
            //函数指针，指向的函数就是信号捕捉到之后的处理函数
            void     (*sa_handler)(int);
            //函数指针，一般不使用
            void     (*sa_sigaction)(int, siginfo_t *, void *);
            //临时阻塞信号集，在信号捕捉函数执行过程中会临时阻塞某些信号，执行完之后恢复
            sigset_t   sa_mask;
            //指定是用第一个回调处理sa_handler还是第二个sa_sigaction，0表示第一个，SA_SIGINFO表示第二个，还有其他的值，但是用的少
            int        sa_flags;
            //被废弃掉了，不需要用，传入nullptr
            void     (*sa_restorer)(void);
	};
~~~

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <signal.h>
#include <sys/time.h>

void myalarm(int num) {
    printf("捕捉到了信号的编号是: %d\n", num);
}

int main() {
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = myalarm;
    sigemptyset(&act.sa_mask);  // 清空吧，表示不要临时阻塞任何信号

    int ret = sigaction(SIGALRM, &act, nullptr);
    if (-1 == ret) {
        perror("signal");
        return -1;
    }

    // 过三秒，会发送信号
    struct timeval _value;
    _value.tv_sec = 3;
    _value.tv_usec = 0;

    // 每隔两秒，会发送信号
    struct timeval _interavl;
    _interavl.tv_sec = 2;
    _interavl.tv_usec = 0;

    // itimerval结构体
    struct itimerval new_value;
    new_value.it_value = _value;
    new_value.it_interval = _interavl;

    // 设置定时器
    int rets = setitimer(ITIMER_REAL, &new_value, nullptr);  // 非阻塞
    printf("定时器开始了\n");                                // 立刻执行，表明是非阻塞的
    if (rets == -1) {
        perror("setitimer");
        return -1;
    }

    while (1)
        ;

    return 0;
}
~~~

执行结果和signal.cpp是一样的，延迟三秒后开始定时器发送信号，然后每隔两秒发送信号

##### 比较二者

**建议使用 sigaction()**

- **signal()是ANSI C signal handling，是美国那边的标准，对其他的标准例如POSIX可能不匹配，所以有一定局限性**
- **sigaction()是标准的，也可以说是改进过的函数，基本都能适配标准，并且功能更多**

##### 更好理解信号捕捉

![image-20230724170811186](https://img-blog.csdnimg.cn/fd5cf34fe65943aa846d764da84a44aa.png)

要注意几点：

- **在sigaction()中，处理信号的时候使用的是我们传递进去的临时阻塞信号集，当处理结束之后会回到PCB当中的阻塞信号集**
- **信号发出之后不会立即处理，先进入未决信号集，变为1，然后去找对应的阻塞信号集，不阻塞则处理，并且修改未决信号集相应为0，当信号在处理过程当中如果未处理完毕这时候收到一个对应的新的信号，不会处理，而是先填入未决信号集，然后等待处理结束然后处理**
- **如果查找阻塞信号集发现阻塞，则阻塞等待，这个时候如果收到新的信号，由于未决信号集相应位置都还是1，那么表示信号尚未被处理，新来的信号会被忽略，当然也不可能记录来了几个，到时候一起处理这种，因为只能存0 1，这也是忽略的原因**

![image-20230724172117665](https://img-blog.csdnimg.cn/46282d2d655c474facb8c9095d9ba8b3.png)

#### SIGCHLD信号

**顾名思义，这是子进程给父进程发送的信号**

**产生的三种条件：**

- **子进程终止**
- **子进程收到SIGSTOP信号停止**
- **子进程处在停止态，收到SIGCONT唤醒**

**父进程接收到这个信号之后，默认处理是忽略这个信号**

**如果我们能接受这个信号，然后去回收子进程的资源，因为wait()函数是阻塞的，父进程不可能一直等待子进程等待结束然后回收，那么可以捕捉子进程结束时候(当然还有其他两种情况)发出的SIGCHLD信号，然后父进程中断去处理这个事情，回收子进程，这样就很好的避免了僵尸进程的问题**

![image-20230724173035421](https://img-blog.csdnimg.cn/952c8a078e9d4ab6ab5f04ec7f3b220d.png)

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

/*
    SIGCHLD信号产生的三个条件
    - 子进程结束
    - 子进程暂停
    - 子进程从暂停状态继续运行
    都会给父进程发送该信号，父进程默认忽略该信号

    可以使用SIGCHLD信号解决僵尸进程的问题

*/

void myFunc(int num) {
    printf("捕捉到的信号 : %d\n", num);
    // 回收子进程PCB的资源
    // wait(nullptr);

    while (1) {
        int ret = waitpid(-1, nullptr, WNOHANG);
        if (ret > 0) {
            printf("chile die , pid = %d\n", getpid());
        } else if (0 == ret)
            // 说明还有子进程，这一次的循环捕捉回收没回收完毕
            break;
        else if (-1 == ret)
            // 说明没有子进程了
            break;
    }
}

int main() {
    // 创建子进程
    pid_t pid;
    for (int i = 0; i < 20; ++i) {
        pid = fork();
        if (0 == pid)
            break;
    }

    if (pid > 0) {
        // 父进程

        // 提前设置好阻塞信号集，阻塞SIGCHLD，因为子进程可能很快结束，父进程还没注册好
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGCHLD);
        sigprocmask(SIG_BLOCK, &set, nullptr);

        // 捕捉子进程死亡时发送的SIGCHLD信号
        struct sigaction act;
        act.sa_flags = 0;
        act.sa_handler = myFunc;
        sigemptyset(&act.sa_mask);

        sigaction(SIGCHLD, &act, nullptr);

        // 注册完信号捕捉之后解除阻塞
        sigprocmask(SIG_UNBLOCK, &set, nullptr);

        while (1) {
            printf("parent process pid : %d\n", getpid());
            sleep(2);
        }
    } else if (pid == 0) {
        // 子进程
        printf("child process pid : %d\n", getpid());
        // sleep(1);
    }

    return 0;
}
~~~

**由于我们对子进程的设置，在运行中20个子进程结束的时间非常接近，waitpid(-1,...)是能识别所有的子进程，但是一次只能清理一个，这些子进程的SIGCHLD信号发送到未决信号集这里，当然未决信号集只能接受一个并且填入，然后交给阻塞信号集，其他的丢弃，所以我们需要while()循环来释放这些几乎同时结束的子进程；之所以设置非阻塞是因为可能个别子进程因为自己的原因，没有和上面的匹配，所以我们设置非阻塞，那个时候这个进程完了发送信号然后父进程去处理，这个时候的未决信号集肯定是写入(0)的，因为如果不可以写入，那必然这个进程就是和前面是一样的了**

### 共享内存(效率最高)

#### 概念

并不是完全没有内核介入，而是相比于其他通信的操作要少得多，因为没有经过内核和用户之间的切换操作或者说非常少，省去了这一大部分的时间，就是将数据从用户空间当中拷贝到内核当中的这一段时间，所以他的效率是最高的

![image-20230724204226816](https://img-blog.csdnimg.cn/370f8d0ac21c4b6fb7f7796ac728bb25.png)

#### 使用步骤

创建共享内存，连接共享内存；分离，删除

![image-20230725090718804](https://img-blog.csdnimg.cn/d2ad0f2eb9164e80a6379221c774fd2f.png)

#### 相关函数

![image-20230725091618011](https://img-blog.csdnimg.cn/3360f8b6264048fd8b79423ba15450b9.png)

记得查man文档，太多太杂了!!!

~~~cpp
	#include <sys/shm.h>

	int shmget(key_t key, size_t size, int shmflg);
// 作用：创建一个新的共享内存段或者获取一个既有的共享内存段的标识
//     新创建的内存段中的数据都会被初始化为0
// 参数：
//     key：key_t类型，是一个整形，通过这个找到或者创建一个共享内存
//         一般用16进制表示，并且是非0值，创建的时候可以随便给，给一个16进制的数或者10进制(会转化)，找到的时候按照创建时候匹配就行
//     size：size_t类型，共享内存的大小，会自动调整为分页边界的整数倍(和内存映射是一样的)
//     shmflg：
//         共享内存的属性：用按位或连接
//             - 访问权限
//             - 附加属性(创建共享内存，判断共享内存是否存在，获取共享内存)

//                 创建：IPC_CREAT 加上 访问权限(比如0664)
//                 获取：IPC_CREAT(不加访问权限)
//                 判断：IPC_EXCL，需要和IPC_CREAT一起使用，用按位或连接
// 返回值：
//     成功 >0 返回共享内存引用的ID，后面操作共享内存使用这个标识
//     失败 -1.修改errno

	void *shmat(int shmid, const void *_Nullable shmaddr, int shmflg);
// 作用：和当前的进程进行关联
// 参数：
//     shmid：共享内存的标识，ID，由shmget()返回值获取
//     shmaddr：申请的共享内存的起始地址，指定为nullptr，让系统帮我们去分配
//     shmflg：
//         对共享内存的操作
//             - 读：SHM_RDONLY，而且必须要有读权限
//             - 读写：0，我们指定什么都不给，但是由于必须有读权限，系统会给我们加上读写的权限
// 返回值：
//     成功 返回共享内存的起始地址
//     失败 (void*)-1


	int shmdt(const void *shmaddr);
// 作用：解除当前进程和共享内存的关联
// 参数：
//     shmaddr：共享内存的首地址
// 返回值：
//     成功 0
//     失败 -1，修改errno

	int shmctl(int shmid, int cmd, struct shmid_ds *buf);
// 作用：对共享内存进行操作，比如可以删除，共享内存要删除才会消失；创建共享内存的进程被销毁了对这块共享内存没有任何影响，必须要手动删除才行
// 参数：
//     shmid：共享内存的id
//     cmd：要做的操作
//         IPC_STAT：获取共享内存当前的状态
//         IPC_SET：设置共享内存的状态
//         IPC_RMID：标记共享内存被销毁，之所以是标记是因为有很多个进程都连接了这个共享内存，我这一个进程并不能想删除就删除，而只是标记下来，当检测到连接数为0时，系统自会将这块共享内存删除
//     buf：需要设置或者获取的共享内存的属性信息
//         IPC_STAT：buf存储数据
//         IPC_SET：buf中需要初始化数据，设置到内核中
//         IPC_RMID：没有用，传递nullptr即可

    #include <sys/ipc.h>

    key_t ftok(const char *pathname, int proj_id);
//作用：根据指定的路径名和int值，生成一个共享内存的key，我们可以不用自己指定
//参数：
    //pathname：指定一个存在的路径
    //proj_id：int类型的值，但是这系统调用只会使用其中的一个字节(8位)
        //返回：0-255，一般指定一个字符 'a'
~~~

#### 示例

写两个程序进行通信

~~~cpp
//write.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <sys/ipc.h>
#include <sys/shm.h>
#define _size 1024

int main() {
    // 创建共享内存
    int shmid = shmget(100, 4096, 0664 | IPC_CREAT);
    if (-1 == shmid) {
        perror("shmget");
        return -1;
    }
    printf("shmid : %d\n", shmid);

    // 和当前进程进行关联
    void *ptr = shmat(shmid, nullptr, 0);
    if ((void *)-1 == ptr) {
        perror("shmat");
        return -1;
    }

    char str[_size] = {0};

    printf("请输入写入的字符串: ");
    fgets(str, sizeof(str), stdin);

    // 写数据
    memcpy(ptr, str, strlen(str) + 1);  // 为了保险，拷上字符串结束符

    printf("按任意键继续\n");
    getchar();

    // 解除关联
    int ret = shmdt(ptr);
    if (-1 == ret) {
        perror("shmdt");
        return -1;
    }

    // 删除共享内存
    shmctl(shmid, IPC_RMID, nullptr);

    return 0;
}
~~~

~~~cpp
//read.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    // 获得共享内存的标识，我们是用key标识的
    int shmid = shmget(100, 4096, IPC_CREAT);
    if (-1 == shmid) {
        perror("shmget");
        return -1;
    }
    printf("shmid : %d\n", shmid);

    // 绑定连接
    void* ptr = shmat(shmid, nullptr, 0);
    if ((void*)-1 == ptr) {
        perror("shmat");
        return -1;
    }

    // 读数据
    printf("data : %s", (char*)ptr);

    printf("按任意键继续\n");
    getchar();

    // 关闭关联
    int ret = shmdt(ptr);
    if (-1 == ret) {
        perror("shmdt");
        return -1;
    }

    // 标记删除
    shmctl(shmid, IPC_RMID, nullptr);

    return 0;
}
~~~

执行结果：

![image-20230725102001298](https://img-blog.csdnimg.cn/8448bb6a5d954f00bec4142c75a3d81c.png)

![image-20230725102008328](https://img-blog.csdnimg.cn/41f01f67706743b8a2360ffa32711830.png)

#### 共享内存操作命令

**注意，shmctl()执行的删除只是标记删除操作，执行到这一步后，这个共享内存的key修改为0，然后不再接受连接，其他程序再次执行shmctl()标记删除相当于什么也没做，但是可以执行不会报错，系统就监听其他进程对这个共享内存的解除连接操作，然后维护shm_nattach，记录关联数，当程序执行shmdt()手动解除或者程序结束的时候系统自动解除连接，当连接数为0之后系统就删除这块共享内存。之所以标记删除，是为了防止删除后还有其他进程在使用这块内存造成不必要的危险**

![image-20230725104726840](https://img-blog.csdnimg.cn/8e9acf799ab049579a303c3561ad120e.png)

#### 注意

**问题1：操作系统如何知道一块共享内存被多少个进程关联？**

- 共享内存维护了一个结构体 struct shmid_ds 这个结构体中有一个成员 shm_nattach
- shm_nattach记录了关联的进程个数

**问题2：可以不可以对共享内存多次删除 stmctl()**

- 可以，因为shmctl()只是标记删除共享内存，不是直接删除
- 什么时候真正删除，当和共享内存关联的进程数为0的时候，就真正被删除
- 当共享内存的key为0的时候，表示共享内存被标记删除，如果进程取消关联就不能继续操作这个共享内存，这种情况下也不能再次关联

**问题3：共享内存和内存映射的区别**

- 共享内存可以直接创建，内存映射需要磁盘文件(匿名映射除外)
- 共享内存效率更高
- 内存：
  - 共享内存：所有的进程操作的是同一块共享内存
    - 内存映射：(父子进程除外)每个进程在自己的虚拟地址空间中有一个独立的内存
- 数据安全
  - 进程突然退出，共享内存还存在，内存映射消失了
  - 运行进程的电脑死机了，数据存储在共享内存中就没有了，内存映射区的数据也没有了，但是他的数据已经同步给磁盘了
- 生命周期
  - 内存映射区：进程退出，内存映射区销毁
  - 共享内存：进程退出，共享内存还在，标记删除(所有关联的进程数为0)，或者关机
    如果进程退出，系统会自动和共享内存取消关联

## 守护进程

### 终端

![image-20230725111929961](https://img-blog.csdnimg.cn/e8d254c6a1704bb998412f551acb777d.png)

### 进程组

![image-20230725112626809](https://img-blog.csdnimg.cn/18f44d231aff44f6b96ef80bdca21634.png)

### 会话

![image-20230725113205286](https://img-blog.csdnimg.cn/727b488f98624c419c9e5fc2d76d8b00.png)

### 理解关系举例

![image-20230725113517970](https://img-blog.csdnimg.cn/c10a580a4de54209a4bb4ca95501f67c.png)

### 操作函数

gid：进程组id；sid：会话的id

<img src="https://img-blog.csdnimg.cn/af6d9f3d02004c23b16b90d2aebb6801.png" alt="image-20230725113838738" style="zoom: 67%;" />

### 守护进程(Daemon进程，精灵进程)

**后台服务进程，是一个生存期较长的进程，一般采用以d结尾的名字**

![image-20230725114051864](https://img-blog.csdnimg.cn/fd8c10c993144c968ea1bb829b7a447e.png)

#### 创建步骤(!!!)

**必须有的是的是前两步和最后一步**

**首先为什么要用子进程来创建会话，因为如果是父进程创建会话的话，一旦父进程是这个进程组的首进程，进程组号就是父进程的id，然后创建会话之后新会话中创建出来的进程组号也用的是这个，两个不同会话中存在同一个进程组号，这个显然是不可以的，所以我们用子进程创建，就避免了这个问题；然后父进程退出一是为了保证不出现僵尸进程(这是孤儿进程没有什么危险)，而是避免子进程运行着时候父进程完了然后输出终端提示符，就是如下(突然冒出来很诡异)**

![image-20230725150200682](https://img-blog.csdnimg.cn/9a54e160a4774c53af9626c08239a11e.png)

**第二，为什么要创建一个新会话？因为如果不是新创建而是挪入其他的会话或者就用自身的会话，那么可能这个会话绑定了控制终端，能够接受信号处理信号这些，这显然与守护进程的初衷不符，所以我们要创建一个新会话，新会话默认是不绑定控制终端的，但是不代表没有终端，至少文件描述符012，标准输入输出错误是有的，言下之意就是可以向屏幕上输出数据，所以这就有了下面关闭文件描述符，然后重定向到 dev/null 的操作，当然这一步也不是必须的**

![image-20230725114544913](https://img-blog.csdnimg.cn/0f6632e4c3dd4908acdfd29f417b1f4e.png)

##### 示例

写一个守护进程，用来每两秒记录一次当前的时间并写到文本当中

**就严格按照这几步来，创建子进程，子进程创建会话，设置umask(不必要)，设置工作目录(不必要)，关闭从父进程继承而来的文件描述符(不必要，这里没有)，重定向文件描述符(不必要，这里有)，核心业务逻辑(设置定时器，捕捉信号)**

~~~cpp
#include <cstring>
#include <ctime>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

// 写一个守护进程，每隔两秒获取系统时间，将这个时间写到磁盘文件中

void _deal(int num) {
    // 获取系统时间写入磁盘文件
    time_t _time = time(nullptr);
    // 将time()获得的距离计算机元年(1970-1-1 00:00:00)的秒数转化为当前的时间
    struct tm* _localtime = localtime(&_time);

    const char* str = asctime(_localtime);
    // 如果不存在则创建，存在则追加
    int ret = access("time.txt", F_OK);
    int fd = -1;
    if (-1 == ret)
        // 不存在
        fd = open("time.txt", O_RDWR | O_CREAT, 0664);
    else if (0 == ret)
        // 存在
        fd = open("time.txt", O_RDWR | O_APPEND);
    if (-1 == fd) {
        perror("open");
        exit(-1);
    }

    ret = write(fd, str, strlen(str));
    if (-1 == ret) {
        perror("write");
        exit(-1);
    }
}

int main() {
    // 创建子进程，退出父进程
    pid_t pid = fork();
    if (-1 == pid) {
        perror("fork");
        return -1;
    }

    if (pid > 0)
        // 父进程
        return 0;
    else if (0 == pid) {
        // 子进程

        // 如果存在time.txt，将其删除，准备工作
        int ret = access("time.txt", F_OK);
        if (0 == ret)
            unlink("time.txt");

        // 在子进程中重新创建一个会话，脱离原来的控制终端
        pid_t sid = setsid();
        if (-1 == pid) {
            perror("setsid");
            return -1;
        }

        // 设置umask
        umask(022);

        // 更改工作目录
        chdir("/mnt/d/Code/Cpp/深入学习/Linux方向/牛客网Linux网络课程/第2章-多进程开发/13");

        // 关闭，以及重定向文件描述符
        int fd = open("/dev/null", O_RDWR);
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

        // 业务逻辑

        // 注册信号捕捉器
        struct sigaction _act;
        _act.sa_flags = 0;
        _act.sa_handler = _deal;
        sigemptyset(&_act.sa_mask);
        sigaction(SIGALRM, &_act, nullptr);

        // 创建定时器
        itimerval _new;
        // 延迟时间
        _new.it_interval.tv_sec = 2;
        _new.it_interval.tv_usec = 0;
        // 周期时间
        _new.it_value.tv_sec = 2;
        _new.it_value.tv_usec = 0;

        ret = setitimer(ITIMER_REAL, &_new, nullptr);
        if (-1 == ret) {
            perror("setitimer");
            return -1;
        }

        // 不让进程结束，不然无法记录
        while (1)
            sleep(10);
    }

    return 0;
}
~~~

执行结果：

<img src="https://img-blog.csdnimg.cn/856759b42fe04763afd2008285bf9a60.png" alt="image-20230725162229689" style="zoom:67%;" />

并且从文件大小不断变化可以看出是实时更新的

![image-20230725162327872](https://img-blog.csdnimg.cn/de856cc3e2754b6fb7585c6895e4712a.png)

守护进程没有控制终端，所以没有办法接受控制终端发出的信号(例如SIGINT( ctrl+c )和SIGQUIT( ctrl+\ ) )，我们只能通过kill -9 强制杀死

![image-20230725162413359](https://img-blog.csdnimg.cn/8f261286005b42e69d6b9260ee0fbe5f.png)

# 第三章 Linux多线程开发

## 概述

### 概念

**同一个程序的所有线程均会执行相同程序，并且会共享同一份全局内存区域**

**进程是资源持有的最小单位，线程是操作系统分配和调度的最小单位**

**线程是轻量级的进程(LWP)，在Linux下线程的本质仍是进程**

![image-20230725162732120](https://img-blog.csdnimg.cn/29ac3f7497f64160a45d76e7007a641e.png)

查看指定进程的LWP号

例如这里打开firefox进程，它的内部是多线程实现的

![image-20230725163508676](https://img-blog.csdnimg.cn/3f11ad12b32e408bb1925eb6800b0e69.png)

我们用命令查看，图中进程号是105266

~~~shell
ps -LF 105266
~~~

结果：

可以看出，firefox进程的这么多线程，虽然进程号都是一样的，但是线程号是不一样的

![image-20230725163632242](https://img-blog.csdnimg.cn/092292276b82459a894148f5311091b7.png)

### 进程和线程区别

**进程之间的信息难以共享，想要共享需要采取进程间通信的方式；并且fork()代价比较高**

**线程之间能够方便，快速的共享信息，只需要将数据复制到共享(全局或堆)变量中即可**

**创建线程的速率比进程通常快很多，10倍甚至更多；线程之间共享虚拟地址空间，无序采取写时复制的方式复制内存，也无须复制页表**

![image-20230725164029090](https://img-blog.csdnimg.cn/b97f07addee3480b8739faeb48a56ba3.png)

我们画个图来理解一下

**虽然线程是共享虚拟地址空间的，但不代表空间中所有的区域都是共享的，比如下面的栈空间和.text代码段就不共享**

**.text是代码段，这个线程是不共享的，而是划分出自己的一块区域**

**栈空间也是不共享的，各个线程将这一块栈空间划分出自己的一块区域**

![image-20230725203244279](https://img-blog.csdnimg.cn/659477be7be04625b7c6e726b5fab581.png)

### 线程的共享和非共享资源

- **用户区中，虚拟地址空间除了栈和代码段不共享，其他共享；剩余的都是内核区的数据，这些是共享的，没有复制操作**
- **非共享资源：线程ID，信号掩码(阻塞信号集)，线程特有的数据，errno变量，实时调度策略和优先级；栈，本地变量和函数调用链接信息**

![image-20230725201723897](https://img-blog.csdnimg.cn/c1fbc9f02b8d4cbfbabb2e9a36531a26.png)

## 线程操作

![image-20230726174153972](https://img-blog.csdnimg.cn/8339e9b5cef4451383c2279266d6586c.png)

### 创建线程 pthead_create()

注意返回值与进程那一套有区别，还有错误号也有区别了

~~~cpp
//	   一般情况下，main函数所在的线程称为主线程(main线程)，其余创建的线程称为子线程
//     程序中默认只有一个进程，fork()函数调用，变为2个进程
//     程序中默认只有一个线程，pthread_create()调用，变为2个线程

    #include <pthread.h>

    int pthread_create(pthread_t *restrict thread,
                     const pthread_attr_t *restrict attr,
                     void *(*start_routine)(void *),
                     void *restrict arg);
// 作用：创建一个子线程
// 参数：
//     thread：类型是pthread_t指针，传出参数，线程创建成功后，子线程的线程ID被写到在变量中
//     attr：设置线程的属性，一般使用默认值，传递nullptr
//     start_routine：函数指针，这个函数是子线程需要处理的逻辑代码
//     arg：给第三个参数使用，传参
// 返回值：
//     成功 0
//     失败 返回一个错误号，这个错误号和之前的errno不太一样(实现方式一样，但是含义不同)
//         不能通过perror()去获取错误号信息
//         如何获取？ char* strerror(int errnum);
~~~

代码

**这个代码只需要注意一点，就是主线程和子线程执行的区域是不一样的，主线程执行main函数里面的内容，然后创建出了子线程，子线程的代码段当中执行call_back()回调函数里面的逻辑，他的范围就局限于这个回调函数，参数可以通过主线程传递给他，这就是也是为什么线程之间代码段和栈空间的内容不是共享的，因为这样设计下来就没法共享了**

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

// 主线程和子线程执行的代码段是不一样的，这个回调函数里面是子线程执行的代码逻辑
void* call_back(void* args) {
    printf("child thread...\n");
    printf("arg value : %d\n", *(int*)args);

    return nullptr;
}

// main函数里面是主线程执行的逻辑
int main() {
    // 创建一个子线程
    pthread_t tid;

    int num = 10;

    int ret = pthread_create(&tid, nullptr, call_back, (void*)&num);
    if (0 != ret) {
        const char* _error = strerror(ret);
        printf("error : %s\n", _error);
        return -1;
    }

    for (int i = 0; i < 5; ++i)
        printf("%d\n", i);

    sleep(1);  // 保证子线程万一没有创建好主线程就执行完了

    return 0;
}
~~~

执行结果：

**注意编译要链接上pthread动态库，文件名是libpthread.so，库名字是pthread，用 -l 参数链接**

![image-20230725212001708](https://img-blog.csdnimg.cn/9aeb9eee0cd44f048594bddb9cbcc569.png)

当然，由于主线程和子线程是并发的关系，很有可能执行结果不一样

![image-20230725212124179](https://img-blog.csdnimg.cn/75ee1524643f4089bd83a5598e90226d.png)

### 终止线程 pthread_exit()

~~~cpp
	#include <pthread.h>

	void pthread_exit(void *retval);
// 作用：终止一个线程，在哪个线程中调用，就表示终止哪个线程
// 参数：
//     retval：需要传递一个指针，作为一个返回值，可以在pthread_join()中获取到

	pthread_t pthread_self(void);
// 作用：获取当前线程的线程id(unsigned long int 无符号长整形)

	int pthread_equal(pthread_t t1.pthread_t t1);
// 作用：比较两个线程id是否相等
//     不同的操作系统对于 pthread_t 的实现不一样，有的是无符号的长整型，有的是用结构体去实现的，不能简单的用 == 号判断
~~~

代码：

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

void* call_back(void* arg) {
    // 为了测试pthread_exit()会不会影响其他线程的正常运行，这里睡三秒，让主线程跑完
    sleep(3);
    printf("child thread id : %ld\n", pthread_self());

    // 这里两个对等是因为子线程结束并不决定整个进程的结束，主线程 return 0 就代表进程结束退出，子线程则不一样
    return nullptr;  // pthread_exit(nullptr)
}

int main() {
    // 创建一个子线程
    pthread_t tid;

    int ret = pthread_create(&tid, nullptr, call_back, nullptr);
    if (0 != ret) {
        const char* str = strerror(ret);
        printf("error : %s\n", str);
        return -1;
    }

    // 主线程
    for (int i = 0; i < 5; ++i)
        printf("%d\n", i);

    printf("tid : %ld , parent thread id : %ld\n", tid, pthread_self());

    // 让主线程退出，当主线程退出的时候不会影响其他正常运行的线程
    pthread_exit(nullptr);

    // 这一行代码没有执行，说明主线程退出后执行return 0结束掉整个进程；而是当所有线程跑完进程才结束，因此不会对其他的线程产生影响
    printf("main thread exit.\n");

    return 0;
}
~~~

**注意对pthread_exit()的理解：**

- **线程退出和线程结束的含义是不同的，线程退出不会影响其他的线程，特别是主线程，线程结束对于主线程而言就会导致整个进程结束了，程序结束，而主线程退出则不会**
- **主线程调用这个函数，意思是主线程退出，但是不走后面的代码，比如图中后面一句的打印就不走，也不会 return 0，因为在主线程中 return 0 就会导致整个进程的结束，所以这时进程不会结束，子线程可以尽情的运行，知道均运行完毕，然后整个进程结束**
- **子线程的执行函数逻辑当中，最后返回，例如 return nullptr，也相当于pthread_exit()，因为子线程退出不会对整个进程造成退出的影响，所以没有什么区别；所以子线程退出和子线程结束基本没有区别**

### 连接已终止的线程 pthread_join()

~~~cpp
	#include <pthread.h>

	int pthread_join(pthread_t thread, void **retval);
// 作用：和一个已经终止的线程进行连接
//     说白了就是回收子线程的资源，防止产生僵尸线程
//     这个函数是阻塞函数，调用一次只能回收一个子线程
//     一般在主线程中去使用(父线程回收子线程的资源)
// 参数：
//     thread：需要回收的子线程id
//     retval：接受子线程退出时的返回值，是个二级指针；如果不需要则传递nullptr
// 返回值：
//     成功 0
//     失败 返回错误号(用strerror())
~~~

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

int value = 10;

void* call_back(void* arg) {
    // 为了测试pthread_exit()会不会影响其他线程的正常运行，这里睡三秒，让主线程跑完
    printf("child thread id : %ld\n", pthread_self());
    sleep(3);

    // 这里两个对等是因为子线程结束并不决定整个进程的结束，主线程 return 0 就代表进程结束退出，子线程则不一样
    // 这里给一个返回值
    // int value = 10;               // 局部变量，这是存在于自己的栈空间当中，子线程结束之后就被释放，所以主线程无法接收到；一般用全局变量或者堆空间的数据
    pthread_exit((void*)&value);  // return (void*)&value;
}

int main() {
    // 创建一个子线程
    pthread_t tid;

    int ret = pthread_create(&tid, nullptr, call_back, nullptr);
    if (0 != ret) {
        const char* str = strerror(ret);
        printf("error : %s\n", str);
        return -1;
    }

    // 主线程
    for (int i = 0; i < 5; ++i)
        printf("%d\n", i);

    printf("tid : %ld , parent thread id : %ld\n", tid, pthread_self());

    // 主线程调用pthread_join()去回收子线程资源
    // 这里可以选择接受子线程执行的返回值也可以选择不要，不要就传递nullptr
    // 为什么是二级指针？返回值是一级指针，我要传递他的指针做传出参数才能接受到!!!
    int* thread_retval;
    // ret = pthread_join(tid, nullptr);
    ret = pthread_join(tid, (void**)&thread_retval);
    if (0 != ret) {
        const char* str = strerror(ret);
        printf("error : %s\n", str);
        return -1;
    }

    printf("exit data : %d\n", *thread_retval);

    printf("回收子线程资源成功\n");  // 这一行代码会在回收子线程之后结束

    // 由于子线程已经结束，主线程已经回收完了所有的资源，所以不用担心主线程结束会导致进程结束子线程没跑完的问题了
    // 所以这一行有无没有区别，有下面一行执行不了，没有下面一行会执行
    pthread_exit(nullptr);

    printf("main thread exit.\n");

    return 0;
}
~~~

注意几点：

- pthread_join(pthread_t thread, void **retval)，第一个参数是需要回收的线程id，第二个参数可以选择接受该子线程的执行的回调函数的返回值，注意类型是二级指针，不需要接受则传递nullptr

  那为什么要传递二级指针呢？就是传出参数的含义了，因为回调函数返回的是void*一级指针类型，我们要想通过传入的参数让系统帮我们修改不能return by value，只能return by pointer或者return by reference，这样才能正确修改，所以需要传入的是二级指针

- ![image-20230727111343535](https://img-blog.csdnimg.cn/ed6a80c46893431dbced4e7364dc5cc1.png)

call_back()函数里面的返回的变量不能是局部变量，也就是放在栈上面的，因为线程之间非常重要的两个不共享的东西就是栈空间和.text代码段，栈空间里面的变量在子线程结束后就会释放掉，所以如果要传递的话最好选择全局变量。堆空间虽然也是共享的，但是可能有问题(不管是全局堆还是局部堆)，虽然空间是公用的，但是好像其他线程一是没有办法free()这个数据，二是我测试过好像不行，我也不知道为什么。所以建议就用全局变量

### 线程的分离 pthread_detach()

~~~cpp
	#include <pthread.h>

	int pthread_detach(pthread_t thread);
// 作用：分离一个线程，被分离的线程在终止的时候会自动释放资源返回给系统
//     - 不能多次分离，会产生不可预期的结果
//     - 不能去连接(join)一个已经分离的线程，如果操作了会报错(会自动释放资源)
// 参数：需要分离的线程id
// 返回值：
//     成功 0
//     失败 错误号
~~~

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <pthread.h>

void* call_back(void* arg) {
    printf("child thread id : %ld\n", pthread_self());

    return nullptr;
}

int main() {
    // 创建一个线程
    pthread_t tid;
    int ret = pthread_create(&tid, nullptr, call_back, nullptr);
    if (0 != ret) {
        const char* errstr = strerror(ret);
        printf("error pthread_create : %s\n", errstr);
        return -1;
    }

    // 输出主线程和子线程的id
    printf("tid : %ld , main thread id : %ld\n", tid, pthread_self());

    // 设置子线程分离，子线程分离后，结束时候对应的资源就不需要主线程手动回收了
    ret = pthread_detach(tid);
    if (0 != ret) {
        const char* errstr = strerror(ret);
        printf("error pthread_detach : %s\n", errstr);
        return -1;
    }

    // 设置分离后，对分离的子线程进行连接，我偏要手动释放，程序执行结果就会报错
    // ret = pthread_join(tid, nullptr);
    // if (0 != ret) {
    //     const char* errstr = strerror(ret);
    //     printf("error pthread_join : %s\n", errstr);
    //     return -1;
    // }

    // 退出主线程防止主线程结束导致进程结束导致程序结束
    pthread_exit(nullptr);

    return 0;
}
~~~

设置子线程分离后就不能再去连接子线程手动释放他的资源了，因为系统会自动将他的资源给释放掉，不用我们操心

如果强行加上的话pthread_join()的返回值就会是个错误号了，但是Linux本身并未对他进行处理，没有发出信号说错误什么什么的，这就需要我们自己进行严谨的判断了，加上的话ret是个错误号，然后就会获取到错误信息，如下：

![image-20230727114257371](https://img-blog.csdnimg.cn/6b62b775c31e47f69f7529d52fe2d924.png)

因此不能手动释放(连接 join)已经分离的线程

### 线程取消 pthread_cancel()

**执行线程取消后，子进程不是立马退出的，而是执行到了某个取消点，线程才会终止**

**取消点就是系统设置好的一些系统调用(比如printf()就是之一)，可以粗略的理解为从用户区到内核区的切换**

~~~cpp
	#include <pthread.h>

    int pthread_cancel(pthread_t thread);
// 作用：取消线程，让线程终止；底层的执行是发送一个取消的请求给线程(有可能是信号)，取消(终止)线程是要执行到某个条件点才能终止
// 取消某个线程可以终止某个线程的运行，但是并不是立马终止，而是执行到了一个取消点，线程才会终止
// 取消点：系统规定好的一些系统调用(比如就有printf())，我们可以粗略的理解为从用户区到内核区的切换，这个位置称之为取消点
~~~

~~~cpp
#include <pthread.h>
#include <unistd.h>
using namespace std;
#include <cstring>
#include <iostream>

void* call_back(void* arg) {
    printf("child thread id : %ld\n", pthread_self());

    for (int i = 0; i < 5; ++i)
        printf("child : %d\n", i);

    return nullptr;
}

int main() {
    // 创建一个线程
    pthread_t tid;
    int ret = pthread_create(&tid, nullptr, call_back, nullptr);
    if (0 != ret) {
        const char* errstr = strerror(ret);
        printf("error pthread_create : %s\n", errstr);
        return -1;
    }

    // 取消线程
    pthread_cancel(tid);

    for (int i = 0; i < 5; ++i)
        printf("%d\n", i);

    // 输出主线程和子线程的id
    printf("tid : %ld , main thread id : %ld\n", tid, pthread_self());

    pthread_exit(nullptr);

    return 0;
}
~~~

输出的结果每次可能都是不一样的，因为主线程和子线程运行的顺序可能不同，所以执行到的取消点位置也可能不同，所以很可能输出的结果是不一样的，比如下面：

![image-20230731101134205](https://img-blog.csdnimg.cn/ebb01b7c09bb4144ad85e17dbc0291b8.png)

## 线程属性

![image-20230731101232100](https://img-blog.csdnimg.cn/a889c39dc1d242109cce649c9f445ce3.png)

注意：线程的属性不只这一个，还有很多其他的，如下，都可以由我们自己去设置：

![image-20230731103140397](https://img-blog.csdnimg.cn/aa9325b50a1e4f0aa57dd519aea92f76.png)

~~~cpp
    #include <pthread.h>

    int pthread_attr_init(pthread_attr_t *attr);
// 作用：初始化线程属性变量

    int pthread_attr_destroy(pthread_attr_t *attr);
// 作用：释放线程属性资源

    int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
// 作用：获取线程分离的状态属性

    int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
// 作用：设置线程分离的状态属性
~~~

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

void* call_back(void* arg) {
    printf("child thread id : %ld\n", pthread_self());

    return nullptr;
}

int main() {
    // 创建一个线程属性变量
    pthread_attr_t attr;
    // 初始化属性变量
    pthread_attr_init(&attr);
    // 设置属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // 表示设置了线程分离

    // 获取线程栈的大小
    size_t _size;

    pthread_attr_getstacksize(&attr, &_size);
    printf("thread stack size : %ld\n", _size);

    // 创建一个线程
    pthread_t tid;
    int ret = pthread_create(&tid, &attr, call_back, nullptr);  // 这里第二个参数，表示线程属性就需要传递进来了
    if (0 != ret) {
        const char* errstr = strerror(ret);
        printf("error pthread_create : %s\n", errstr);
        return -1;
    }

    // 输出主线程和子线程的id
    printf("tid : %ld , main thread id : %ld\n", tid, pthread_self());

    // 释放线程属性资源，初始化了必要释放!!!
    pthread_attr_destroy(&attr);

    // 退出主线程防止主线程结束导致进程结束导致程序结束
    pthread_exit(nullptr);

    return 0;
}
~~~

在代码中注意两点：

- **线程属性结构体初始化(init)之后就必须要释放(destroy)**
- **一般来说主线程和子线程的释放可以有两种方法来写：一是主线程调用pthread_join()手动阻塞回收子线程资源，这个时候就不用考虑子线程结束了主线程还没结束没办法回收资源的问题，当然我们不能让pthread_join()前面的逻辑执行太久，这样僵尸线程的存在时间可能会太长，和没有处理几乎是一样的；二是主线程将子线程分离pthread_detach()，这样主线程就不用去管子线程的释放问题了，但是这样最好在末尾加上pthread_exit()让主线程退出，否则很可能主线程执行完了导致进程结束然后子线程跑不完，主线程退出恰好就解决了这个问题**

## 线程同步

### 示例引入

写一个主线程，创建三个子线程，三个子线程的任务是共同售卖100张门票

~~~cpp
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

// 使用多线程去实现卖票的案例
// 有3个窗口，一共100张票

// 全局变量，但是还是有问题
int tickets = 100;

void* CALLBACK_sell(void* arg) {
    while (tickets > 0) {
        usleep(8000);
        printf("pthread tid %ld is selling ticket %d\n", pthread_self(), tickets);
        --tickets;
    }

    return nullptr;
}

int main() {
    // 创建子线程
    pthread_t tid1, tid2, tid3;

    pthread_create(&tid1, nullptr, CALLBACK_sell, nullptr);
    pthread_create(&tid2, nullptr, CALLBACK_sell, nullptr);
    pthread_create(&tid3, nullptr, CALLBACK_sell, nullptr);

    // 分离线程
    pthread_detach(tid1);
    pthread_detach(tid2);
    pthread_detach(tid3);

    // 主线程退出
    pthread_exit(nullptr);

    return 0;
}
~~~

在代码当中我使用了全局变量tickets，来让三个线程进行共享对他进行处理，但是想法是很美好的，现实却不美好

![image-20230731143537103](https://img-blog.csdnimg.cn/91ef2197e48c4624ba1b422c60368dc4.png)

图中有两个问题，一是7号票三个线程卖了三次；二是线程卖了0号和-1号票，这如果对于实际的问题将会是毁灭性的打击

- **我们先来看为什么会卖三次？**

**谁想如下情形：三个线程ABC，线程A这时候抢占到了CPU，然后睡眠；在睡眠的时候线程B和C进来抢占了CPU，然后睡眠，如果这个时候B先拿到CPU打印这一句，但是还没来得及 --ticket ，就被C抢占了，C也打印同样的ticket，然后来不及--又被A抢占了，所以这个时候就会出现三个线程卖同一张票的情况**

- **那么用类似的思路我们去看为什么会卖出0和-1**

**图样的事情，三个线程同时在 ticket == 1 的时候进来然后睡眠，然后A线程进来执行打印和 --ticket 两句，这个时候ticket变成0，B线程同样执行这两句，但是这个时候打印的ticket是0，C线程同理，只不过这个时候打印的是-1**

**总结一下，这就是没有加访问互斥锁的原因，对共享数据的处理没有加锁导致几个线程同时对数据进行处理，这样数据的更新时机和读取时间一旦不恰当，就很有可能出现数据不同步的问题，这对于要求精确的项目是毁灭性的打击**

### 概述

**临界区是访问一个共享资源的代码片段，并且对该代码进行原子操作，原子操作在执行的过程中不能被中断，必须要执行完毕才能被其他线程占用访问临界区资源**

线程同步就是让一个线程在内存进行操作的时候，其他线程都不允许对这个内存资源进行访问，只有该线程完成操作，其他线程才能对该内存地址进行操作，在执行的过程中其他线程位于阻塞等待状态

![image-20230731154000837](https://img-blog.csdnimg.cn/b88a86483a7a48108d38ee672adeec15.png)

### 互斥锁

所以访问共享资源的时候，为了避免线程更新共享变量的时候出现问题，需要使用互斥锁mutex来对访问进行限制，访问的时候线程给这个共享资源加上互斥锁，其他线程不能试图在加锁的时候对该资源进行访问或者尝试解锁，只有所有者才能给互斥量解锁

![image-20230731161340736](https://img-blog.csdnimg.cn/480ddf475f90441abec0ab58fa0f15db.png)

加了互斥锁之后，现在的访问过程就是这样了：

![image-20230801145448245](https://img-blog.csdnimg.cn/9a871473090c4d4d9889f04a1011b29a.png)

#### 相关函数

![image-20230801145628830](https://img-blog.csdnimg.cn/1559a69b8d204f799ff3b0edb8c71729.png)

#### 示例修改(!!!)

回到之前哪个卖票的例子，现在我加上互斥锁，如下：

互斥量必须是全局的，如果是局部的，线程之间没有办法共享这个互斥量，则会导致有问题

~~~cpp
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

// 全局变量，但是还是有问题
int tickets = 100;
// 全局互斥量
pthread_mutex_t mutex;

void* CALLBACK_sell(void* arg) {
    // 加锁
    pthread_mutex_lock(&mutex);

    // 临界区
    while (tickets > 0) {
        usleep(8000);
        printf("pthread tid %ld is selling ticket %d\n", pthread_self(), tickets);
        --tickets;
    }

    // 解锁
    pthread_mutex_unlock(&mutex);

    return nullptr;
}

int main() {
    // 初始化互斥量，必须是全局的
    pthread_mutex_init(&mutex, nullptr);

    // 创建子线程
    pthread_t tid1, tid2, tid3;

    pthread_create(&tid1, nullptr, CALLBACK_sell, nullptr);
    pthread_create(&tid2, nullptr, CALLBACK_sell, nullptr);
    pthread_create(&tid3, nullptr, CALLBACK_sell, nullptr);

    // 阻塞等待回收线程
    pthread_join(tid1, nullptr);
    pthread_join(tid2, nullptr);
    pthread_join(tid3, nullptr);

    // 释放互斥量资源
    pthread_mutex_destroy(&mutex);

    return 0;
}
~~~

这段代码实际上还是有问题的，我们来看输出结果：

我们发现所有的票都是由一个线程卖出的，我们从代码中查看是为什么

![image-20230802144136067](https://img-blog.csdnimg.cn/e5a2f834bd3243afbb5b746b10b22557.png)

来看我们进行加锁的这段逻辑：

我们发现如果A线程上了锁，那么他就进入了while()循环，在这个while()循环结束之前是没有办法被其他线程加锁访问的，所以就导致了所有的票都是由一个线程去卖的，这显然不符合我们的预期

~~~cpp
void* CALLBACK_sell(void* arg) {
    // 加锁
    pthread_mutex_lock(&mutex);

    // 临界区
    while (tickets > 0) {
        usleep(8000);
        printf("pthread tid %ld is selling ticket %d\n", pthread_self(), tickets);
        --tickets;
    }

    // 解锁
    pthread_mutex_unlock(&mutex);

    return nullptr;
}
~~~

所以我们这里得到一个启发，就是加锁要加在while()循环里面

所以我们的代码这么修改：

在while(1)循环里面要开始访问临界区的时候，然后跳出循环的条件是卖完了，其他线程可以在上一张票卖完准备跳到下一张票中间没有互斥锁保护的循环过渡期进行抢占 或者 时间片用完进行抢占

~~~cpp
void* CALLBACK_sell(void* arg) {
    // 临界区
    while (1) {
        // 加锁
        pthread_mutex_lock(&mutex);

        if (tickets > 0) {
            usleep(5000);
            printf("pthread tid %ld is selling ticket %d\n", pthread_self(), tickets);
            --tickets;
        } else {
            // 卖完了
            pthread_mutex_unlock(&mutex);
            break;
        }
        // 解锁
        pthread_mutex_unlock(&mutex);
    }

    return nullptr;
}
~~~

但是我们的输出结果是：

还是只有一个人在卖，为什么呢？

我们观察发现，我们卖完票睡眠了一段时间，这段时间肯定是比CPU的时间片要大的，并且线程A在休眠的过程中还是被加锁保护了的，除非这段时间小于时间片，系统才会把CPU给他，但是没有，所以往返而来就是一个线程在卖

![image-20230802145837448](https://img-blog.csdnimg.cn/6db0dad81bfb4a05888989b218aab76b.png)

所以我们可以把睡眠去掉再来看

~~~cpp
void* CALLBACK_sell(void* arg) {
    // 临界区
    while (1) {
        // 加锁
        pthread_mutex_lock(&mutex);

        if (tickets > 0) {
            // usleep(5000);
            printf("pthread tid %ld is selling ticket %d\n", pthread_self(), tickets);
            --tickets;
        } else {
            // 卖完了
            pthread_mutex_unlock(&mutex);
            break;
        }
        // 解锁
        pthread_mutex_unlock(&mutex);
    }

    return nullptr;
}
~~~

执行结果为：

可以看出的确线程是交替来卖票的，只不过时间片完了被其他线程进行抢占，然后交替卖票

并且由于我们的设计是while(1)死循环，跳出的点是票卖完了，所以我们的线程可以进行第二轮的卖票，而不是卖了一轮就结束

<img src="https://img-blog.csdnimg.cn/ceca7c0940404544b460baa8062739f5.png" alt="image-20230802150151733" style="zoom:67%;" />

<img src="https://img-blog.csdnimg.cn/a26fa20269c7439e9d0de9c69bac961e.png" alt="image-20230802150219200" style="zoom:67%;" />

### 死锁

死锁可能产生的几种场景：

- **忘记释放锁**
- **重复加锁**
- **多线程多锁，抢占锁资源**

死锁产生的四个必要条件(缺一不可):

- **互斥**
- **非剥夺**
- **请求和保持**
- **环路等待**

![image-20230803135330646](https://img-blog.csdnimg.cn/c112dbde10af41be87944709348ba3af.png)

我们来看死锁产生的几种情景：

- 忘记释放锁：很显然，一个线程访问临界区的时候加上锁，访问完毕走的时候忘了解锁，这样其他的线程没办法加锁，更没办法访问了，自己第二次过来想继续加锁访问也是不可以的，因为上一把锁还没解开

  ~~~cpp
  void* CALLBACK_sell(void* arg) {
      // 临界区
      while (1) {
          // 加锁
          pthread_mutex_lock(&mutex);
  
          if (tickets > 0) {
              // usleep(5000);
              printf("pthread tid %ld is selling ticket %d\n", pthread_self(), tickets);
              --tickets;
          } else {
              // 卖完了
              pthread_mutex_unlock(&mutex);
              break;
          }
          // 解锁
          // pthread_mutex_unlock(&mutex);
      }
  
      return nullptr;
  }
  ~~~

  ![image-20230803143045431](https://img-blog.csdnimg.cn/39fc734ba53142c4967e51b9ccada133.png)

- 重复加相同的锁：设想一个我们写代码的时候应该不会犯的错误，就是在我要加锁的时候，我加了两次，第一把锁能够加上，但是第二把是加不上的，因为第一把锁还没加开，所以自己没办法访问，显然其他的线程更没办法访问了；但是我们一般不会犯这么傻的错误，我们可能加了锁之后去调用其他的函数，然后其他的函数当中存在加锁，这样显然就出现了上面的情况，然而这样的话我们不易察觉

  ~~~cpp
  void* CALLBACK_sell(void* arg) {
      // 临界区
      while (1) {
          // 加锁
          // 这里对同一把锁加了两次
          pthread_mutex_lock(&mutex);
          pthread_mutex_lock(&mutex);
  
          if (tickets > 0) {
              // usleep(5000);
              printf("pthread tid %ld is selling ticket %d\n", pthread_self(), tickets);
              --tickets;
          } else {
              // 卖完了
              pthread_mutex_unlock(&mutex);
              break;
          }
          // 解锁
          pthread_mutex_unlock(&mutex);
          pthread_mutex_unlock(&mutex);
      }
  
      return nullptr;
  }
  ~~~

  ![image-20230803143334595](https://img-blog.csdnimg.cn/8ed91cd3df0248eca19f2a510ee83fbb.png)

- 多线程多锁，抢占锁资源：看图中，线程A和线程B分别给资源1和资源2加锁，但是线程的执行依赖于这两个资源的共同访问，所以谁都没办法进行，这就导致了环路等待，产生了死锁

  下面是一个示例：

  假设我们这里的代码不睡1秒，那么线程会先后执行，因为线程执行的时间太短了，导致在时间片内就完成了，这样其他线程没有抢占然后去加锁的过程，因此这种情况是不会产生死锁的

  但是如果我们死循环的去执行，就有可能产生死锁，死循环执行，时间片完了我们也不知道线程执行到哪一步，然后被抢占了万一刚好给第一个加锁，另外一个也给第一个加锁，这不就死锁了嘛

  这里我在加锁了之后睡1秒，我让第二个线程去抢占加锁，让他成为死锁，从执行结果来看必然是死锁

  ~~~cpp
  #include <iostream>
  using namespace std;
  #include <pthread.h>
  #include <unistd.h>
  
  pthread_mutex_t mutex1, mutex2;
  
  void* CALLBACK_A(void* arg) {
      pthread_mutex_lock(&mutex1);
      sleep(1);  // 这里睡一秒，让线程B得到抢占权
      pthread_mutex_lock(&mutex2);
  
      printf("thread A , tid : %ld is working.\n", pthread_self());
  
      // 释放锁的时候最好反着来，因为加锁是有顺序的
      pthread_mutex_unlock(&mutex2);
      pthread_mutex_unlock(&mutex1);
  
      return nullptr;
  }
  
  void* CALLBACK_B(void* arg) {
      pthread_mutex_lock(&mutex2);
      sleep(1);  // 这里睡一秒，让线程A得到抢占权
      pthread_mutex_lock(&mutex1);
  
      printf("thread B , tid : %ld is working.\n", pthread_self());
  
      pthread_mutex_unlock(&mutex1);
      pthread_mutex_unlock(&mutex2);
  
      return nullptr;
  }
  
  int main() {
      // 初始化互斥信号量
      pthread_mutex_init(&mutex1, nullptr);
      pthread_mutex_init(&mutex2, nullptr);
  
      pthread_t tid1, tid2;
  
      // 创建子线程
      pthread_create(&tid1, nullptr, &CALLBACK_A, nullptr);
      pthread_create(&tid2, nullptr, &CALLBACK_B, nullptr);
  
      // 回收子线程
      pthread_join(tid1, nullptr);
      pthread_join(tid2, nullptr);
  
      // 释放信号量
      pthread_mutex_destroy(&mutex1);
      pthread_mutex_destroy(&mutex2);
  
      return 0;
  }
  ~~~

  ![image-20230803151736722](https://img-blog.csdnimg.cn/f4dc3cddfb9e4a9782b84848375db8e5.png)

### 读写锁

在实际的开发过程中，存在读和写的两种情况，我们发现如果读写都是独占加锁的话，读是可以多个线程同时进行的呀，因为没有修改数据的大小，所以加锁就造成了资源和效率上的浪费；所以我们可以让多个线程可以同时读数据，然后写数据需要加互斥锁，只能一个线程写数据，并且在写的时候其他线程不能读数据

特点：

- **如果有线程在读数据，其他线程只允许读数据而不允许写数据**
- **如果有线程在写数据，其他线程都不允许进行读写操作**
- **写数据是独占的，他的优先级更高**

![image-20230804134719753](https://img-blog.csdnimg.cn/e8c4c611d0654525b4c367b80c52d5f4.png)

#### 相关函数

![image-20230816104436489](https://img-blog.csdnimg.cn/9aa13fe74c3a40059c1711daa16c7a87.png)

示例代码：(8个线程，3个写线程，5个读线程)

~~~cpp
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

/*
    读写锁的类型 pthread_rwlock_t

    int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict attr);

    int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

    int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);

    int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);

    int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

    int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);

    int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
*/

// 定义全局变量
int num = 1;

// 定义读写互斥量
pthread_rwlock_t _rwlock;

void* WRITE_CALLBACK(void* arg) {
    while (1) {
        pthread_rwlock_wrlock(&_rwlock);

        printf("++write, tid : %ld , num : %d\n", pthread_self(), ++num);

        pthread_rwlock_unlock(&_rwlock); //这行代码要在usleep上面，因为需要睡眠让其他线程进行抢占，如果在下面就不好说了

        usleep(1000);
    }

    return nullptr;
}

void* READ_CALLBACK(void* arg) {
    while (1) {
        pthread_rwlock_rdlock(&_rwlock);

        printf("===read, tid : %ld , num : %d\n", pthread_self(), num);

        pthread_rwlock_unlock(&_rwlock);

        usleep(1000);
    }

    return nullptr;
}

// 案例：创建8个线程，操作同一个全局变量
// 3个线程不定时的写一个全局变量，5个线程不定时的读这个全局变量
int main() {
    // 初始化读写互斥量
    pthread_rwlock_init(&_rwlock, nullptr);

    // 创建3个写线程，5个读线程
    pthread_t wr_tids[3], rd_tids[5];

    for (int i = 0; i < 3; ++i)
        pthread_create(&wr_tids[i], nullptr, WRITE_CALLBACK, nullptr);

    for (int i = 0; i < 5; ++i)
        pthread_create(&rd_tids[i], nullptr, READ_CALLBACK, nullptr);

    // 分离线程
    for (int i = 0; i < 3; ++i)
        pthread_detach(wr_tids[i]);

    for (int i = 0; i < 5; ++i)
        pthread_detach(rd_tids[i]);

    // 退出主线程
    pthread_exit(nullptr);

    // 释放读写互斥量
    pthread_rwlock_destroy(&_rwlock);

    return 0;
}
~~~

执行结果：

![image-20230816112647486](https://img-blog.csdnimg.cn/b7ab0ba3e91a4355b6f6c6d93e75e357.png)

### 生产者消费者模型

![image-20230816150325060](https://img-blog.csdnimg.cn/c1250dbbbe0742b380471ae2507f5e67.png)

我们现在用一个简陋的实现来模拟这个过程

~~~cpp
#include <ctime>
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

/*
    生产者消费者模型(粗略的版本)
*/

// 定义一个链表
struct Node {
    int val;
    struct Node* next;
};

// 定义头结点
struct Node* head = nullptr;

void* PRO_CALLBACK(void*) {
    // 不断生成新节点，插入到链表当中(头插)
    while (1) {
        struct Node* newNode = new struct Node;
        newNode->next = head;
        head = newNode;

        newNode->val = rand() % 1000;

        printf("add node , val : %d , tid : %ld\n", newNode->val, pthread_self());

        usleep(1000);
    }

    return nullptr;
}

void* CUS_CALLBACK(void*) {
    // 不断从头部释放头结点
    while (1) {
        struct Node* tmp = head;
        head = head->next;
        printf("delete node , val : %d , tid : %ld\n", tmp->val, pthread_self());

        delete tmp;
        tmp = nullptr;

        usleep(1000);
    }

    return nullptr;
}

int main() {
    // 创建5个生产者线程，和5个消费者线程
    pthread_t ptids[5], ctids[5];

    for (int i = 0; i < 5; ++i) {
        pthread_create(&ptids[i], nullptr, PRO_CALLBACK, nullptr);
        pthread_create(&ctids[i], nullptr, CUS_CALLBACK, nullptr);
    }

    // 线程分离
    for (int i = 0; i < 5; ++i) {
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    while (1) {
        sleep(10);
    }

    // 线程退出
    pthread_exit(nullptr);

    return 0;
}
~~~

这个程序没有对多线程进行数据处理的同步操作，会导致一系列问题，比如链表没有数据就进行释放，这样就会导致内存的访问错误，也就是会报段错误，多次执行，每次执行的结果可能都是不一样的

![image-20230817102529078](https://img-blog.csdnimg.cn/7af776fc6c2d4629880af4903581ec70.png)

#### 自己的思路

我们自己先尝试着解决这两个问题，一个是数据不同步的问题，一个是非法访问内存导致段错误的问题，我们的代码如下：

- **数据同步：由于我们在生产和消费的时候都是处理的是头结点，这一块区域就是临界区，我们可以给这个区域加上互斥锁，也就是定义pthread_mutex_t类型互斥锁来处理**
- **段错误，这个问题的出现在于我们在链表为空的时候进行了消费者行为，移出数据，这个时候会导致内存的非法访问，因此我们可以加上一个条件判断，当没有数据的时候就循环直到有数据**
  **但是这么做的坏处就是如果消费者线程一直拿到CPU执行一直没有数据，那就一直空转等待，会消耗性能和降低效率，我们希望消费者在没有数据的时候能够题型生产者去生产数据，而生产者相反的可以在满了的时候提醒消费者消费数据，当然我们这个设计链表可以无限延申，这里不需要考虑，这才是我们想要的，所以下面就有条件变量和信号量两种操作**

~~~cpp
#include <ctime>
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

/*
    生产者消费者模型(粗略的版本)
*/

// 创建互斥量来解决数据同步的问题
pthread_mutex_t mutex;

// 定义一个链表
struct Node {
    int val;
    struct Node* next;
};

// 定义头结点
struct Node* head = nullptr;

void* PRO_CALLBACK(void*) {
    // 不断生成新节点，插入到链表当中(头插)
    while (1) {
        pthread_mutex_lock(&mutex);

        struct Node* newNode = new struct Node;
        newNode->next = head;
        head = newNode;

        newNode->val = rand() % 1000;

        printf("add node , val : %d , tid : %ld\n", newNode->val, pthread_self());

        pthread_mutex_unlock(&mutex);

        usleep(1000);
    }

    return nullptr;
}

void* CUS_CALLBACK(void*) {
    // 不断从头部释放头结点
    while (1) {
        pthread_mutex_lock(&mutex);

        struct Node* tmp = head;

        // 这里如果没有数据head就为nullptr就会报错这一行，非法访问内存
        // 需要进行判断
        if (head == nullptr) {
            delete tmp;
            tmp = nullptr;

            pthread_mutex_unlock(&mutex);

            usleep(1000);
            continue;
        }

        head = head->next;
        printf("delete node , val : %d , tid : %ld\n", tmp->val, pthread_self());

        delete tmp;
        tmp = nullptr;

        pthread_mutex_unlock(&mutex);

        usleep(1000);
    }

    return nullptr;
}

int main() {
    // 初始化互斥锁
    pthread_mutex_init(&mutex, nullptr);

    // 创建5个生产者线程，和5个消费者线程
    pthread_t ptids[5], ctids[5];

    for (int i = 0; i < 5; ++i) {
        pthread_create(&ptids[i], nullptr, PRO_CALLBACK, nullptr);
        pthread_create(&ctids[i], nullptr, CUS_CALLBACK, nullptr);
    }

    // 回收线程
    for (int i = 0; i < 5; ++i) {
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    // 用死循环来保证主线程不会结束，如果用 pthread_exit() 会导致互斥锁释放的位置问题
    while (1)
        ;

    // 释放互斥锁
    pthread_mutex_destroy(&mutex);

    // 主线程退出(这里其实没什么用了)
    pthread_exit(nullptr);

    return 0;
}
~~~

#### 条件变量

**条件变量可以有两个行为，满足某个条件线程阻塞，或者满足条件线程解除阻塞**

**他不能保证数据混乱的问题，数据混乱需要加互斥锁，需要使用 pthread_mutex_t 类型的互斥锁解决**

**我们发现阻塞和解除阻塞，还要满足某个条件，这不就是我想要的嘛？所以head为空就阻塞，head不为空就解除阻塞，这就是基本的思路**

![image-20230817104711611](https://img-blog.csdnimg.cn/fc9fd315ecf64e9c94e2e5025b400d28.png)

将我们的代码进行改进：

**其实就改了两个地方，就是生产者生产了之后的动作和消费者没有数据时候的处理**

- **生产者：生产者在生产了数据之后会调用条件变量的信号函数 pthread_cond_signal() 函数来提醒消费者有数据了，至于是怎么提醒的呢？这就依赖于消费者里面的函数 pthread_cond_wait()**
- **消费者：调用阻塞函数 pthread_cond_wait() 进行阻塞等待(没有数据的时候)，然后当有数据的时候就解除阻塞**
  **这里会出现一个问题，为什么 pthread_cond_wait() 函数中会需要传入mutex互斥锁的信息呢？**
  **肯定是操作了互斥锁，我们来看，假设不操作，那么我阻塞，然后我还拿着临界区的访问互斥锁，那么就出问题了，其他的线程不管是消费者还是生产者都没有办法拿到这把锁，那么就肯定会导致死锁，所以肯定对这个锁进行了处理**
  **其实，当调用阻塞的时候，会释放掉这把锁，让其他线程进行争抢，当生产者拿到锁，生产了数据，调用pthread_cond_signal()函数告诉消费者可以解除阻塞了，那么这个时候就会解除阻塞并且重新给这个线程上锁，因此解除阻塞的时候锁还在我身上，刚才阻塞的过程中不在了，所以后面需要跟上一句释放这个锁，因为根据我们的逻辑会重新循环拿锁，我们需要避免死锁**

~~~cpp
#include <ctime>
#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

/*
    生产者消费者模型(粗略的版本)
*/

// 创建互斥量来解决数据同步的问题
pthread_mutex_t mutex;
// 定义条件变量
pthread_cond_t cond;

// 定义一个链表
struct Node {
    int val;
    struct Node* next;
}* head = nullptr;  // 定义头结点

void* PRO_CALLBACK(void* args) {
    // 不断生成新节点，插入到链表当中(头插)
    while (1) {
        pthread_mutex_lock(&mutex);

        struct Node* newNode = new struct Node;
        newNode->next = head;
        head = newNode;

        newNode->val = rand() % 1000;

        printf("add node , val : %d , tid : %ld\n", newNode->val, pthread_self());

        pthread_mutex_unlock(&mutex);
        
        pthread_cond_signal(&cond);

        usleep(1000);
    }

    return nullptr;
}

void* CUS_CALLBACK(void* args) {
    // 不断从头部释放头结点
    while (1) {
        pthread_mutex_lock(&mutex);

        struct Node* tmp = head;

        // 这里如果没有数据head就为nullptr就会报错这一行，非法访问内存
        // 需要进行判断
        if (head != nullptr) {
            head = head->next;
            printf("delete node , val : %d , tid : %ld\n", tmp->val, pthread_self());

            delete tmp;
            tmp = nullptr;

            pthread_mutex_unlock(&mutex);

            usleep(1000);
        } else {
            // 没有数据，需要阻塞等待
            // 当这个函数调用阻塞的时候，会解锁，当不阻塞的时候继续向下执行，会重新加锁
            pthread_cond_wait(&cond, &mutex);
            // 当有数据唤醒之后，我们的代码逻辑是重新进入循环加锁，因此必须提前释放锁
            pthread_mutex_unlock(&mutex);
        }
    }

    return nullptr;
}

int main() {
    // 初始化互斥锁
    pthread_mutex_init(&mutex, nullptr);
    // 初始化条件变量
    pthread_cond_init(&cond, nullptr);

    // 创建5个生产者线程，和5个消费者线程
    pthread_t ptids[5], ctids[5];

    for (int i = 0; i < 5; ++i) {
        pthread_create(&ptids[i], nullptr, PRO_CALLBACK, nullptr);
        pthread_create(&ctids[i], nullptr, CUS_CALLBACK, nullptr);
    }

    // 回收线程
    for (int i = 0; i < 5; ++i) {
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    // 用死循环来保证主线程不会结束，如果用 pthread_exit() 会导致互斥锁释放的位置问题
    while (1)
        ;

    // 释放条件变量
    pthread_cond_destroy(&cond);
    // 释放互斥锁
    pthread_mutex_destroy(&mutex);

    // 主线程退出(这里其实没什么用了)
    pthread_exit(nullptr);

    return 0;
}
~~~

#### 信号量

看到 sem_wait() 和 sem_post() 函数就想到操作系统中学到的PV问题了

![image-20230817114724983](https://img-blog.csdnimg.cn/0ea55f60e3674e2ea1e3c05330d6c412.png)

~~~cpp
    #include <semaphore.h>

    int sem_init(sem_t *sem, int pshared, unsigned int value);
//作用：初始化信号量
//参数：
    //sem：信号量变量的地址
    //pshared：0 用在线程； 非0 用在进程
    //value：信号量的值

    int sem_destroy(sem_t *sem);
//作用：释放资源

    int sem_wait(sem_t *sem);
//作用：对信号量加锁，调用一次，对信号量的值减1，如果值为0，就阻塞

    int sem_trywait(sem_t *sem);
//作用：尝试wait

    int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict abs_timeout);
//作用：等待一段时间

    int sem_post(sem_t *sem);
//作用：解锁一个信号量，调用一次，对信号量的值加1

    int sem_getvalue(sem_t *restrict sem, int *restrict sval);
//作用：获取信号量的值
~~~

实际代码：

~~~cpp
#include <ctime>
#include <iostream>
using namespace std;
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// 创建互斥量来解决数据同步的问题
pthread_mutex_t mutex;
// 创建两个信号量
sem_t psem, csem;

// 定义一个链表
struct Node {
    int val;
    struct Node* next;
}* head = nullptr;  // 定义头结点

void* PRO_CALLBACK(void* args) {
    // 不断生成新节点，插入到链表当中(头插)
    while (1) {
        sem_wait(&psem);

        pthread_mutex_lock(&mutex);

        struct Node* newNode = new struct Node;
        newNode->next = head;
        head = newNode;

        newNode->val = rand() % 1000;

        printf("add node , val : %d , tid : %ld\n", newNode->val, pthread_self());

        pthread_mutex_unlock(&mutex);

        sem_post(&csem);  // 将消费者的信号量加1表示可以进行消费

        usleep(1000);
    }

    return nullptr;
}

void* CUS_CALLBACK(void* args) {
    // 不断从头部释放头结点
    // 这就是操作系统当中学的经典的PV问题了，巩固一下
    while (1) {
        sem_wait(&csem);

        pthread_mutex_lock(&mutex);

        struct Node* tmp = head;

        head = head->next;
        printf("delete node , val : %d , tid : %ld\n", tmp->val, pthread_self());

        delete tmp;
        tmp = nullptr;

        pthread_mutex_unlock(&mutex);

        sem_post(&psem);  // 将生产者的信号量加1表示可以生产

        usleep(1000);
    }

    return nullptr;
}

int main() {
    // 初始化互斥锁
    pthread_mutex_init(&mutex, nullptr);
    // 初始化信号量
    sem_init(&psem, 0, 8);
    sem_init(&csem, 0, 0);

    // 创建5个生产者线程，和5个消费者线程
    pthread_t ptids[5],
        ctids[5];

    for (int i = 0; i < 5; ++i) {
        pthread_create(&ptids[i], nullptr, PRO_CALLBACK, nullptr);
        pthread_create(&ctids[i], nullptr, CUS_CALLBACK, nullptr);
    }

    // 回收线程
    for (int i = 0; i < 5; ++i) {
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    // 用死循环来保证主线程不会结束，如果用 pthread_exit() 会导致互斥锁释放的位置问题
    while (1)
        ;

    // 释放互斥锁
    pthread_mutex_destroy(&mutex);

    // 主线程退出(这里其实没什么用了)
    pthread_exit(nullptr);

    return 0;
}
~~~

# 第四章 Linux网络编程

## 网络结构模式

### C/S结构

#### 简介

- 服务器 - 客户机，即 Client - Server（C/S）结构。C/S 结构通常采取两层结构。服务器负责数据的 管理，客户机负责完成与用户的交互任务。客户机是因特网上访问别人信息的机器，服务器则是提 供信息供人访问的计算机。
- 客户机通过局域网与服务器相连，接受用户的请求，并通过网络向服务器提出请求，对数据库进行 操作。服务器接受客户机的请求，将数据提交给客户机，客户机将数据进行计算并将结果呈现给用 户。服务器还要提供完善安全保护及对数据完整性的处理等操作，并允许多个客户机同时访问服务 器，这就对服务器的硬件处理数据能力提出了很高的要求。 
- 在C/S结构中，应用程序分为两部分：服务器部分和客户机部分。服务器部分是多个用户共享的信 息与功能，执行后台服务，如控制共享数据库的操作等；客户机部分为用户所专有，负责执行前台 功能，在出错提示、在线帮助等方面都有强大的功能，并且可以在子程序间自由切换。

#### 优点

1. 能充分发挥客户端 PC 的处理能力，很多工作可以在客户端处理后再提交给服务器，所以 C/S 结构客户端响应速度快；
2. 操作界面漂亮、形式多样，可以充分满足客户自身的个性化要求； 
3. C/S 结构的管理信息系统具有较强的事务处理能力，能实现复杂的业务流程； 
4. 安全性较高，C/S 一般面向相对固定的用户群，程序更加注重流程，它可以对权限进行多层次校验，提供了更安全的存取模式，对信息安全的控制能力很强，一般高度机密的信息系统采用 C/S 结构适宜。

#### 缺点

1. 客户端需要安装专用的客户端软件。首先涉及到安装的工作量，其次任何一台电脑出问题，如病 毒、硬件损坏，都需要进行安装或维护。系统软件升级时，每一台客户机需要重新安装，其维护和 升级成本非常高； 
2. 对客户端的操作系统一般也会有限制，不能够跨平台。

### B/S结构

#### 简介

B/S 结构（Browser/Server，浏览器/服务器模式），是 WEB 兴起后的一种网络结构模式，WEB 浏览器是客户端最主要的应用软件。这种模式统一了客户端，将系统功能实现的核心部分集中到服 务器上，简化了系统的开发、维护和使用。客户机上只要安装一个浏览器，如 Firefox 或 Internet Explorer，服务器安装 SQL Server、Oracle、MySQL 等数据库。浏览器通过 Web Server 同数据 库进行数据交互。

#### 优点

B/S 架构最大的优点是总体拥有成本低、维护方便、 分布性强、开发简单，可以不用安装任何专门的软 件就能实现在任何地方进行操作，客户端零维护，系统的扩展非常容易，只要有一台能上网的电脑就能 使用。

#### 缺点

1. 通信开销大、系统和数据的安全性较难保障; 
2. 个性特点明显降低，无法实现具有个性化的功能要求；
3. 协议一般是固定的：http/https
4. 客户端服务器端的交互是请求-响应模式，通常动态刷新页面，响应速度明显降低。

## MAC地址

网卡是一块被设计用来允许计算机在计算机网络上进行通讯的计算机硬件，又称为网络适配器或网络接口卡NIC。其拥有 MAC 地址，属于 OSI 模型的第 2 层，它使得用户可以通过电缆或无线相互 连接。每一个网卡都有一个被称为 MAC 地址的独一无二的 48 位串行号。网卡的主要功能：1.数据的封装与解封装、2.链路管理、3.数据编码与译码。

![image-20230817154336052](https://img-blog.csdnimg.cn/aee449063c1c4bec8a46657cf7f5185e.png)

MAC 地址（Media Access Control Address），直译为媒体存取控制位址，也称为局域网地址、 以太网地址、物理地址或硬件地址，它是一个用来确认网络设备位置的位址，由网络设备制造商生产时烧录在网卡中。在 OSI 模型中，第三层网络层负责 IP 地址，第二层数据链路层则负责 MAC 位址 。MAC 地址用于在网络中唯一标识一个网卡，一台设备若有一或多个网卡，则每个网卡都需 要并会有一个唯一的 MAC 地址。

MAC 地址的长度为 48 位（6个字节），通常表示为 12 个 16 进制数，如：00-16-EA-AE-3C-40 就 是一个MAC 地址，**其中前 3 个字节，16 进制数 00-16-EA 代表网络硬件制造商的编号，它由 IEEE（电气与电子工程师协会）分配，而后 3 个字节，16进制数 AE-3C-40 代表该制造商所制造的 某个网络产品（如网卡）的系列号。**只要不更改自己的 MAC 地址，MAC 地址在世界是唯一的。 形象地说，MAC 地址就如同身份证上的身份证号码，具有唯一性。

## IP地址

### 简介

IP 协议是为计算机网络相互连接进行通信而设计的协议。在因特网中，它是能使连接到网上的所有计算机网络实现相互通信的一套规则，规定了计算机在因特网上进行通信时应当遵守的规则。任 何厂家生产的计算机系统，只要遵守 IP 协议就可以与因特网互连互通。各个厂家生产的网络系统 和设备，如以太网、分组交换网等，它们相互之间不能互通，不能互通的主要原因是因为它们所传 送数据的基本单元（技术上称之为“帧”）的格式不同。

IP 协议实际上是一套由软件程序组成的协议 软件，它把各种不同“帧”统一转换成“IP 数据报”格式，这种转换是因特网的一个最重要的特点，使 所有各种计算机都能在因特网上实现互通，即具有“开放性”的特点。正是因为有了 IP 协议，因特 网才得以迅速发展成为世界上最大的、开放的计算机通信网络。因此，IP 协议也可以叫做“因特网 协议”。

IP 地址（Internet Protocol Address）是指互联网协议地址，又译为网际协议地址。IP 地址是 IP 协议提供的一种统一的地址格式，它为互联网上的每一个网络和每一台主机分配一个逻辑地址，以 此来屏蔽物理地址的差异。 IP 地址是一个 32 位的二进制数，通常被分割为 4 个“ 8 位二进制数”（也就是 4 个字节）。IP 地址 通常用“点分十进制”表示成（a.b.c.d）的形式，其中，a,b,c,d都是 0~255 之间的十进制整数。 例：点分十进IP地址（100.4.5.6），实际上是 32 位二进制数 （01100100.00000100.00000101.00000110）。

### IP地址编址方式(ABCDE和CIDR)

最初设计互联网络时，为了便于寻址以及层次化构造网络，每个 IP 地址包括两个标识码（ID），即网络 ID 和主机 ID。同一个物理网络上的所有主机都使用同一个网络 ID，网络上的一个主机（包括网络上工 作站，服务器和路由器等）有一个主机 ID 与其对应。Internet 委员会定义了 5 种 IP 地址类型以适合不 同容量的网络，即 A 类~ E 类。

其中 A、B、C 3类（如下表格）由 InternetNIC 在全球范围内统一分配，D、E 类为特殊地址。

![image-20230817162026337](https://img-blog.csdnimg.cn/1fd122da47d642b5bb2cac571b053112.png)

#### A类IP地址

一个 A 类 IP 地址是指， 在 IP 地址的四段号码中，第一段号码为网络号码，剩下的三段号码为本地计算 机的号码。如果用二进制表示 IP 地址的话，A 类 IP 地址就由 1 字节的网络地址和 3 字节主机地址组 成，网络地址的最高位必须是“0”。A 类 IP 地址中网络的标识长度为 8 位，主机标识的长度为 24 位，A 类网络地址数量较少，有 126 个网络，每个网络可以容纳主机数达 1600 多万台。 

A 类 IP 地址 地址范围 1.0.0.1 - 126.255.255.254（二进制表示为：00000001 00000000 00000000 00000001 - 01111111 11111111 11111111 11111110）。最后一个是广播地址。 

A 类 IP 地址的子网掩码为 255.0.0.0，每个网络支持的最大主机数为 256 的 3 次方 - 2 = 16777214 台

#### B类IP地址

一个 B 类 IP 地址是指，在 IP 地址的四段号码中，前两段号码为网络号码。如果用二进制表示 IP 地址的 话，B 类 IP 地址就由 2 字节的网络地址和 2 字节主机地址组成，网络地址的最高位必须是“10”。B 类 IP 地址中网络的标识长度为 16 位，主机标识的长度为 16 位，B 类网络地址适用于中等规模的网络，有 16384 个网络，每个网络所能容纳的计算机数为 6 万多台。 

B 类 IP 地址地址范围 128.0.0.1 - 191.255.255.254 （二进制表示为：10000000 00000000 00000000 00000001 - 10111111 11111111 11111111 11111110）。 最后一个是广播地址。 

B 类 IP 地址的子网掩码为 255.255.0.0，每个网络支持的最大主机数为 256 的 2 次方 - 2 = 65534 台。

#### C类IP地址

一个 C 类 IP 地址是指，在 IP 地址的四段号码中，前三段号码为网络号码，剩下的一段号码为本地计算 机的号码。如果用二进制表示 IP 地址的话，C 类 IP 地址就由 3 字节的网络地址和 1 字节主机地址组 成，网络地址的最高位必须是“110”。C 类 IP 地址中网络的标识长度为 24 位，主机标识的长度为 8 位， C 类网络地址数量较多，有 209 万余个网络。适用于小规模的局域网络，每个网络最多只能包含254台 计算机。 

C 类 IP 地址范围 192.0.0.1-223.255.255.254 （二进制表示为: 11000000 00000000 00000000 00000001 - 11011111 11111111 11111111 11111110）。 

C类IP地址的子网掩码为 255.255.255.0，每个网络支持的最大主机数为 256 - 2 = 254 台。

#### D类IP地址

D 类 IP 地址在历史上被叫做多播地址（multicast address），即组播地址。在以太网中，多播地址命 名了一组应该在这个网络中应用接收到一个分组的站点。多播地址的最高位必须是 “1110”，范围从 224.0.0.0 - 239.255.255.255。

#### 特殊IP地址

每一个字节都为 0 的地址（ “0.0.0.0” ）对应于当前主机； 

IP 地址中的每一个字节都为 1 的 IP 地址（ “255.255.255.255” ）是当前子网的广播地址； 

IP 地址中凡是以 “11110” 开头的 E 类 IP 地址都保留用于将来和实验使用。 

IP地址中不能以十进制 “127” 作为开头，该类地址中数字 127.0.0.1 到 127.255.255.255 用于回路测 试，如：127.0.0.1可以代表本机IP地址。

#### 子网掩码

子网掩码（subnet mask）又叫网络掩码、地址掩码、子网络遮罩，它是一种用来指明一个 IP 地 址的哪些位标识的是主机所在的子网，以及哪些位标识的是主机的位掩码。子网掩码不能单独存 在，它必须结合 IP 地址一起使用。子网掩码只有一个作用，就是将某个 IP 地址划分成网络地址和 主机地址两部分。 

子网掩码是一个 32 位地址，用于屏蔽 IP 地址的一部分以区别网络标识和主机标识，并说明该 IP 地址是在局域网上，还是在广域网上。

子网掩码是在 IPv4 地址资源紧缺的背景下为了解决 lP 地址分配而产生的虚拟 lP 技术，通过子网掩码将 A、B、C 三类地址划分为若干子网，从而显著提高了 IP 地址的分配效率，有效解决了 IP 地址资源紧张 的局面。另一方面，在企业内网中为了更好地管理网络，网管人员也利用子网掩码的作用，人为地将一 个较大的企业内部网络划分为更多个小规模的子网，再利用三层交换机的路由功能实现子网互联，从而 有效解决了网络广播风暴和网络病毒等诸多网络管理方面的问题。 

在大多数的网络教科书中，一般都将子网掩码的作用描述为通过逻辑运算，将 IP 地址划分为网络标识 (Net.ID) 和主机标识(Host.ID)，只有网络标识相同的两台主机在无路由的情况下才能相互通信。 

根据 RFC950 定义，子网掩码是一个 32 位的 2 进制数， 其对应网络地址的所有位都置为 1，对应于主 机地址的所有位置都为 0。子网掩码告知路由器，地址的哪一部分是网络地址，哪一部分是主机地址， 使路由器正确判断任意 IP 地址是否是本网段的，从而正确地进行路由。网络上，数据从一个地方传到另 外一个地方，是依靠 IP 寻址。从逻辑上来讲，是两步的。第一步，从 IP 中找到所属的网络，好比是去 找这个人是哪个小区的；第二步，再从 IP 中找到主机在这个网络中的位置，好比是在小区里面找到这个 人。 

子网掩码的设定必须遵循一定的规则。与二进制 IP 地址相同，子网掩码由 1 和 0 组成，且 1 和 0 分别 连续。子网掩码的长度也是 32 位，左边是网络位，用二进制数字 “1” 表示，1 的数目等于网络位的长 度；右边是主机位，用二进制数字 “0” 表示，0 的数目等于主机位的长度。这样做的目的是为了让掩码 与 IP 地址做按位与运算时用 0 遮住原主机数，而不改变原网络段数字，而且很容易通过 0 的位数确定子 网的主机数（ 2 的主机位数次方 - 2，因为主机号全为 1 时表示该网络广播地址，全为 0 时表示该网络 的网络号，这是两个特殊地址）。通过子网掩码，才能表明一台主机所在的子网与其他子网的关系，使 网络正常工作。

## 端口

### 简介

“端口” 是英文 port 的意译，可以认为是设备与外界通讯交流的出口。端口可分为虚拟端口和物理 端口，其中虚拟端口指计算机内部或交换机路由器内的端口，不可见，是特指TCP/IP协议中的端 口，是逻辑意义上的端口。例如计算机中的 80 端口、21 端口、23 端口等。物理端口又称为接 口，是可见端口，计算机背板的 RJ45 网口，交换机路由器集线器等 RJ45 端口。电话使用 RJ11 插 口也属于物理端口的范畴。 

如果把 IP 地址比作一间房子，端口就是出入这间房子的门。真正的房子只有几个门，但是一个 IP 地址的端口可以有 65536（即：2^16）个之多！端口是通过端口号来标记的，端口号只有整数， 范围是从 0 到65535（2^16-1）。

### 端口类型

1. 周知端口（Well Known Ports） 周知端口是众所周知的端口号，也叫知名端口、公认端口或者常用端口，范围从 0 到 1023，它们紧密 绑定于一些特定的服务。例如 80 端口分配给 WWW 服务，21 端口分配给 FTP 服务，23 端口分配给 Telnet服务等等。我们在 IE 的地址栏里输入一个网址的时候是不必指定端口号的，因为在默认情况下 WWW 服务的端口是 “80”。网络服务是可以使用其他端口号的，如果不是默认的端口号则应该在地址栏 上指定端口号，方法是在地址后面加上冒号“:”（半角），再加上端口号。比如使用 “8080” 作为 WWW 服务的端口，则需要在地址栏里输入“网址:8080”。但是有些系统协议使用固定的端口号，它是不能被改 变的，比如 139 端口专门用于 NetBIOS 与 TCP/IP 之间的通信，不能手动改变。 
2. 注册端口（Registered Ports） 端口号从 1024 到 49151，它们松散地绑定于一些服务，分配给用户进程或应用程序，这些进程主要是 用户选择安装的一些应用程序，而不是已经分配好了公认端口的常用程序。这些端口在没有被服务器资 源占用的时候，可以用用户端动态选用为源端口。 
3. 动态端口 / 私有端口（Dynamic Ports / Private Ports） 动态端口的范围是从 49152 到 65535。之所以称为动态端口，是因为它一般不固定分配某种服务，而是动态分配

## 网络模型

### OSI 七层参考模型

七层模型，亦称 OSI（Open System Interconnection）参考模型，即开放式系统互联。参考模型 是国际标准化组织（ISO）制定的一个用于计算机或通信系统间互联的标准体系，一般称为 OSI 参 考模型或七层模型。 

它是一个七层的、抽象的模型体，不仅包括一系列抽象的术语或概念，也包括具体的协议。

![image-20230817164927457](https://img-blog.csdnimg.cn/1141a430a33449419acb7bd873c01f02.png)

1. 物理层：主要定义物理设备标准，如网线的接口类型、光纤的接口类型、各种传输介质的传输速率 等。它的主要作用是传输比特流（就是由1、0转化为电流强弱来进行传输，到达目的地后再转化为 1、0，也就是我们常说的数模转换与模数转换）。这一层的数据叫做比特。 
2. 数据链路层：建立逻辑连接、进行硬件地址寻址、差错校验等功能。定义了如何让格式化数据以帧 为单位进行传输，以及如何让控制对物理介质的访问。将比特组合成字节进而组合成帧，用MAC地 址访问介质。 
3. 网络层：进行逻辑地址寻址，在位于不同地理位置的网络中的两个主机系统之间提供连接和路径选 择。Internet的发展使得从世界各站点访问信息的用户数大大增加，而网络层正是管理这种连接的 层。 
4. 传输层：定义了一些传输数据的协议和端口号（ WWW 端口 80 等），如：TCP（传输控制协议， 传输效率低，可靠性强，用于传输可靠性要求高，数据量大的数据），UDP（用户数据报协议，与 TCP 特性恰恰相反，用于传输可靠性要求不高，数据量小的数据，如 QQ 聊天数据就是通过这种方 式传输的）。 主要是将从下层接收的数据进行分段和传输，到达目的地址后再进行重组。常常把这 一层数据叫做段。 
5. 会话层：通过传输层（端口号：传输端口与接收端口）建立数据传输的通路。主要在你的系统之间发起会话或者接受会话请求。 
6. 表示层：数据的表示、安全、压缩。主要是进行对接收的数据进行解释、加密与解密、压缩与解压缩等（也就是把计算机能够识别的东西转换成人能够能识别的东西（如图片、声音等）。 
7. 应用层：网络服务与最终用户的一个接口。这一层为用户的应用程序（例如电子邮件、文件传输和 终端仿真）提供网络服务。

### TCP/IP 四层模型

#### 简介

现在 Internet（因特网）使用的主流协议族是 TCP/IP 协议族，它是一个分层、多协议的通信体 系。TCP/IP协议族是一个四层协议系统，自底而上分别是数据链路层、网络层、传输层和应用 层。每一层完成不同的功能，且通过若干协议来实现，上层协议使用下层协议提供的服务。

![image-20230817165030403](https://img-blog.csdnimg.cn/195019fe9bad4d01aaf2ff12ae8ae02e.png)

TCP/IP 协议在一定程度上参考了 OSI 的体系结构。OSI 模型共有七层，从下到上分别是物理层、数据链路层、网络层、传输层、会话层、表示层和应用层。但是这显然是有些复杂的，所以在 TCP/IP 协议中， 它们被简化为了四个层次。

（1）应用层、表示层、会话层三个层次提供的服务相差不是很大，所以在 TCP/IP 协议中，它们被合并为应用层一个层次。 

（2）由于传输层和网络层在网络协议中的地位十分重要，所以在 TCP/IP 协议中它们被作为独立的两个层次。 

（3）因为数据链路层和物理层的内容相差不多，所以在 TCP/IP 协议中它们被归并在网络接口层一个层 次里。只有四层体系结构的 TCP/IP 协议，与有七层体系结构的 OSI 相比要简单了不少，也正是这样， TCP/IP 协议在实际的应用中效率更高，成本更低。

![image-20230817165057099](https://img-blog.csdnimg.cn/8ca8225db3014de8b981261e74b6d60f.png)

#### 四层介绍

1. 应用层：应用层是 TCP/IP 协议的第一层，是直接为应用进程提供服务的。 
   （1）对不同种类的应用程序它们会根据自己的需要来使用应用层的不同协议，邮件传输应用使用 了 SMTP 协议、万维网应用使用了 HTTP 协议、远程登录服务应用使用了有 TELNET 协议。 
   （2）应用层还能加密、解密、格式化数据。 
   （3）应用层可以建立或解除与其他节点的联系，这样可以充分节省网络资源。 
2. 传输层：作为 TCP/IP 协议的第二层，运输层在整个 TCP/IP 协议中起到了中流砥柱的作用。且在运输层中， TCP 和 UDP 也同样起到了中流砥柱的作用。 也可以叫运输层(我一般叫运输层)
3. 网络层：网络层在 TCP/IP 协议中的位于第三层。在 TCP/IP 协议中网络层可以进行网络连接的建立 和终止以及 IP 地址的寻找等功能。 
4. 网络接口层：在 TCP/IP 协议中，网络接口层位于第四层。由于网络接口层兼并了物理层和数据链路层所以，网络接口层既是传输数据的物理媒介，也可以为网络层提供一条准确无误的线路。

## 协议

### 简介

协议，网络协议的简称，网络协议是通信计算机双方必须共同遵从的一组约定。如怎么样建立连 接、怎么样互相识别等。只有遵守这个约定，计算机之间才能相互通信交流。它的三要素是：**语法、语义、时序**。 

为了使数据在网络上从源到达目的，网络通信的参与方必须遵循相同的规则，这套规则称为协议 （protocol），它最终体现为在网络上传输的数据包的格式。 协议往往分成几个层次进行定义，分层定义是为了使某一层协议的改变不影响其他层次的协议。

### 常见协议

应用层常见的协议有：FTP协议（File Transfer Protocol 文件传输协议）、HTTP协议（Hyper Text Transfer Protocol 超文本传输协议）、NFS（Network File System 网络文件系统）。 

传输层常见协议有：TCP协议（Transmission Control Protocol 传输控制协议）、UDP协议（User Datagram Protocol 用户数据报协议）。 

网络层常见协议有：IP 协议（Internet Protocol 因特网互联协议）、ICMP 协议（Internet Control Message Protocol 因特网控制报文协议）、IGMP 协议（Internet Group Management Protocol 因特 网组管理协议）。 

网络接口层常见协议有：ARP协议（Address Resolution Protocol 地址解析协议）、RARP协议 （Reverse Address Resolution Protocol 反向地址解析协议）。

### UDP协议

![image-20230817171145866](https://img-blog.csdnimg.cn/ff34ea95877f497595bb8e7cd423cd21.png)

### TCP协议

![image-20230817171159785](https://img-blog.csdnimg.cn/b0c8a693f69b4bb89c8970623ab1dc5a.png)

<img src="https://img-blog.csdnimg.cn/6bdd7982b10a4249b7aa6abe879ddd9b.png"/>

### IP协议

![image-20230817171259144](https://img-blog.csdnimg.cn/c1e48d7fece547b494f5f3e022f1f405.png)

![image-20230817171313342](https://img-blog.csdnimg.cn/703adb8ed9cb40af9bdf2394942f7c5e.png)

### 以太网帧协议

![image-20230817171325127](https://img-blog.csdnimg.cn/2ffb02cc3f3c4190a7431ac44c7af469.png)

### ARP协议

![image-20230817171331187](https://img-blog.csdnimg.cn/7d9a5ea0d1ad4eb7a0fff4bd0bde6a8b.png)

例如，这里就是一个arp请求包

![image-20230817203353404](https://img-blog.csdnimg.cn/dfba8ded30f54339a1a6c4d1b91e0e6c.png)

当然，需要发送出去还要加上帧头帧尾能封装成为一个完整的帧

### 封装

上层协议是如何使用下层协议提供的服务的呢？其实这是通过封装（encapsulation）实现的。应用程序 数据在发送到物理网络上之前，将沿着协议栈从上往下依次传递。每层协议都将在上层数据的基础上加上自己的头部信息（有时还包括尾部信息），以实现该层的功能，这个过程就称为封装。

![image-20230817171342456](https://img-blog.csdnimg.cn/2386628aff654123a970d98aab468049.png)

### 分用

当帧到达目的主机时，将沿着协议栈自底向上依次传递。各层协议依次处理帧中本层负责的头部数据， 以获取所需的信息，并最终将处理后的帧交给目标应用程序。这个过程称为分用（demultiplexing）。 分用是依靠头部信息中的类型字段实现的。

![image-20230817171356559](https://img-blog.csdnimg.cn/0fc5759fb84947b088150a984f7ebf49.png)

![image-20230817171401748](https://img-blog.csdnimg.cn/86b1165686484e6d8161fbdbb698b9e5.png)

## 网络通信的过程

示意图

![image-20230817201604241](https://img-blog.csdnimg.cn/eff1cd272a9f4d769908ec4fe4c5ae68.png)

## socket介绍

所谓 socket（套接字），就是对网络中不同主机上的应用进程之间进行双向通信的端点的抽象。 一个套接字就是网络上进程通信的一端，提供了应用层进程利用网络协议交换数据的机制。从所处的地位来讲，套接字上联应用进程，下联网络协议栈，是应用程序通过网络协议进行通信的接口， 是应用程序与网络协议根进行交互的接口。 

socket 可以看成是两个网络应用程序进行通信时，各自通信连接中的端点，这是一个逻辑上的概 念。它是网络环境中进程间通信的 API，也是可以被命名和寻址的通信端点，使用中的每一个套接 字都有其类型和一个与之相连进程。通信时其中一个网络应用程序将要传输的一段信息写入它所在主机的 socket 中，该 socket 通过与网络接口卡（NIC）相连的传输介质将这段信息送到另外一台主机的 socket 中，使对方能够接收到这段信息。socket 是由 IP 地址和端口结合的，提供向应用 层进程传送数据包的机制。 

**socket 本身有“插座”的意思，在 Linux 环境下，用于表示进程间网络通信的特殊文件类型。本质为内核借助缓冲区形成的伪文件。既然是文件，那么理所当然的，我们可以使用文件描述符引用套接字。与管道类似的，也存在读写缓冲区，写方向写的缓冲区写入数据，读方接收到数据在读缓冲区中，这就形成了数据的传递，Linux 系统将其封装成文件的目的是为了统一接口，使得读写套接字和读写文件的操作一致。区别是管道主要应用于本地进程间通信，而套接字多应用于网络进程间数据的传递。**

![image-20230817210404237](https://img-blog.csdnimg.cn/a136ec722c564d5bb7c9ffcccab70124.png)

~~~cpp
// 套接字通信分两部分：
- 服务器端：被动接受连接，一般不会主动发起连接
- 客户端：主动向服务器发起连接
socket是一套通信的接口，Linux 和 Windows 都有，但是有一些细微的差别
~~~

##  字节序

### 简介

现代 CPU 的累加器一次都能装载（至少）4 字节（这里考虑 32 位机），即一个整数。那么这 4 字节在内存中排列的顺序将影响它被累加器装载成的整数的值，这就是字节序问题。在各种计算机 体系结构中，对于字节、字等的存储机制有所不同，因而引发了计算机通信领域中一个很重要的问 题，即通信双方交流的信息单元（比特、字节、字、双字等等）应该以什么样的顺序进行传送。如 // 套接字通信分两部分： - 服务器端：被动接受连接，一般不会主动发起连接 - 客户端：主动向服务器发起连接 socket是一套通信的接口，Linux 和 Windows 都有，但是有一些细微的差别。 果不达成一致的规则，通信双方将无法进行正确的编码/译码从而导致通信失败。 

**字节序，顾名思义字节的顺序，就是大于一个字节类型的数据在内存中的存放顺序(一个字节的数据当然就无需谈顺序的问题了)。** 

**字节序分为大端字节序（Big-Endian） 和小端字节序（Little-Endian）。大端字节序是指一个整数的高位字节存储在内存的低地址处，低位字节存储在内存的高地址处；小端字节序则是指整数的高位字节存储在内存的高地址处，而低位字节则存储在内存的低地址处。**

### 字节序举例

**小端字节序是低位字节存储在内存的低地址处，高位字节存储在内存的高地址处；大端字节序则相反!!!**

**图中的0x11就是一个字节，因为他是用16进制表示的，也就是0x11对应的11是8个bit，然后就是一个字节，这个数就是这么存储下来的，因此就有了高位字节和低位字节在内存中的顺序问题**

![image-20230818092738767](https://img-blog.csdnimg.cn/716d54a0f66240328069b385965a5c9c.png)

![image-20230818092744993](https://img-blog.csdnimg.cn/eb2d6292d3a04c92b11f61df2653c358.png)

好，我们了解了这些就可以写一个测试样例来查看我们的主机是小端字节序还是大端字节序(记忆：小端字节序就是内存高位对应数里面的高位)

~~~cpp
#include <iostream>
using namespace std;

/*
字节序：字节在内存中存储的顺序
小端字节序：数据的高位字节存储在内存的高位地址，低位字节存储在内存的低位地址
大端字节序：数据的高位字节存储在内存的低位地址，低位字节存储在内存的高位地址(与前面的相反)
*/

// 通过代码检测当前主机的字节序

// 联合里面的数据地址的起始位置都是从头开始的，因此指向的内存很大概率是有重复的
union Fuck {
    short value;                // 两个字节
    char bytes[sizeof(short)];  // 两个字节的数组，可以查看两个字节分成一半是怎么存储的
} test;

int main() {
    test.value = 0x0102;

    if (test.bytes[0] == 1 && test.bytes[1] == 2)  // 数据的高位对应字节存储的低位，则为大端字节序
        printf("大端字节序\n");
    else
        printf("小端字节序\n");

    return 0;
}a
~~~

当然，这个代码简化一下一行就可以搞定

~~~cpp
cout << (char(0x0102) == 0x02 ? "小端字节序" : "大端字节序") << endl;  // 这么写更加简单
~~~

### 字节序转换函数

**当格式化的数据在两台使用不同字节序的主机之间直接传递时，接收端必然错误的解释之。解决问题的方法是：发送端总是把要发送的数据转换成大端字节序数据后再发送，而接收端知道对方传送过来的数据总是采用大端字节序，所以接收端可以根据自身采用的字节序决定是否对接收到的数据进行转换（小端机转换，大端机不转换）。** 

因此，**在网络中，我们规定都用大端字节序传递数据，称为网络字节序**

**网络字节顺序**是 TCP/IP 中规定好的一种数据表示格式，它与具体的 CPU 类型、操作系统等无关，从而 可以保证数据在不同主机之间传输时能够被正确解释，网络字节顺序采用大端排序方式。 

BSD Socket提供了封装好的转换接口，方便程序员使用。包括从主机字节序到网络字节序的转换函数： htons、htonl；从网络字节序到主机字节序的转换函数：ntohs、ntohl。

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

我们写一个测试案例来演示一下这几个函数

~~~cpp
#include <iostream>
using namespace std;
#include <arpa/inet.h>

/*
    网络通信时，需要将主机字节序转化为网络字节序(大端)
    另外一端获取到数据以后，根据情况将网络字节序转换为主机字节序

    #include <arpa/inet.h>
    // 转换端口 端口号 0-65535，就是16位，因此是 uint16_t
    uint16_t htons(uint16_t hostshort); // 主机字节序 - 网络字节序
    uint16_t ntohs(uint16_t netshort); // 主机字节序 - 网络字节序
    // 转IP IP地址，IPV4是32位，因此是uint32_t
    uint32_t htonl(uint32_t hostlong); // 主机字节序 - 网络字节序
    uint32_t ntohl(uint32_t netlong); // 主机字节序 - 网络字节序
*/

int main() {
    // htons() 转换端口
    unsigned short a = 0x0102;
    printf("a : %x\n", a);
    unsigned short b = htons(a);
    printf("b : %x\n", b);

    printf("----------------------------------------------\n");

    // htonl() 转换IP
    // 这里用char会报 narrowing conversion 缩窄转换
    // 这是c++11在使用初始化序列时候编译器会自动判断，如果发生缩窄转换就会报错
    unsigned char buf[4] = {192, 168, 1, 100};

    int num = *(int*)buf;
    int ans = htonl(num);

    unsigned char* p = (unsigned char*)&ans;
    printf("%u %u %u %u\n", *p, *(p + 1), *(p + 2), *(p + 3));

    printf("----------------------------------------------\n");

    // ntohl()
    unsigned char buf1[4] = {1, 1, 168, 192};
    int num1 = *(int*)buf1;
    int ans1 = ntohl(num1);
    unsigned char* p1 = (unsigned char*)&ans1;
    printf("%u %u %u %u\n", *p1, *(p1 + 1), *(p1 + 2), *(p1 + 3));

    return 0;
}
~~~

**我们来解释一下里面的一些东西，值得我们深思**

**在c++11当中新增了一个 narrowing convertions，就是缩窄转换；他的规定是c++11之后引入了一个新特性就是列表初始化initializer_list，在用初始化列表初始化值的时候不允许发生类型的缩窄转换**

**比如：**

~~~cpp
vector<int> tmp {1,2.2}; 
//这里不允许将double类型的2.2转化为int类型，在初始化列表中，因为double类型的范围比int类型更宽(这是新规定的)
~~~

**但是**

~~~cpp
int num = double(2.2);
//这行代码显然是合法的，没有用initializer_list，num的值显然为2
~~~

**我们再举一个例子：**

~~~cpp
int a(double(2.2));
int a{double(2.2)};
~~~

**类似的两行代码，第一个使用的是构造函数，第二个调用的是initializer_list，在c++11标准下，第一个是可以通过编译的，第二会报错，原因是narrowing convertions，缩窄变换**

**对应到我们这里，我们看这一行代码：**

~~~cpp
unsigned char buf[4] = {192, 168, 1, 100};
~~~

**为什么我们会选择unsigned char 而不是 char？**

**我们知道，c语言内置的类型都是signed，即有符号的，所以以char为例，是1个字节，表示的范围为-128到127，8位数，第一位拿来表示符号，1为负，0为正，然后-128是用来表示-0，和+0用于区分；然后我们图中传入的是192，168，这两个如果要想转化为char类型的话那就是负数，但是在initializer_list当中就不允许这样的缩窄转换，就是把int类型转化为char类型，因为这里我们可以看出192已经超过了-128到127的范围**

~~~cpp
char(192);
~~~

**虽然我们这么写是没有问题的，存入的数字应该是-64，但是initializer_list中会被认为是缩窄转换，所以不允许这么做**

**但是我们再来看unsigned char，由于网络字节当中的不管是IP还是port端口都是无符号，也就是非负的，所以我们可以选择unsigned，其次，这里的IP地址，一个字节的范围是0-255，而unsigned char的表示范围恰好就是0-255，他们把负数表示为对应的补码，所以范围恰好完美对应，当然如果数在大一点可能会出问题，但是我们对应的是实际的IP情况，所以就能应对了，因此这里的类型我们选择unsigned char**

**而C语言如果我们用char或者低版本的c++，例如c++98这种，就不会报错，因为这是语法定义的对数组初始化，只不过c++11之后引入了一个更加厉害的initializer_list而已**

## socket地址

**主要是用来封装IP和端口号port的信息**

~~~cpp
// socket地址其实是一个结构体，封装端口号和IP等信息。后面的socket相关的api中需要使用到这个socket地址。
// 客户端 -> 服务器（IP, Port）
~~~

### 通用socket地址

socket 网络编程接口中表示 socket 地址的是结构体 sockaddr，这个在一般是通用的，其定义如下：

~~~cpp
#include <bits/socket.h>
struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14]; //存储数据，包括IP和端口号的信息
};
typedef unsigned short int sa_family_t;
~~~

**这个结构体data部分的长度最大是14个字节，然后下面会看出其他两种都可能会超出14个字节，而IPv4就6个字节，可以存放下，因此绝大多数情况下是给IPv4使用的**

**sa_family 成员是地址族类型（sa_family_t）的变量。**地址族类型通常与协议族类型对应。常见的协议族（protocol family，也称 domain）和对应的地址族入下所示：

![image-20230818115634758](https://img-blog.csdnimg.cn/039a7f3aa42c4fd0bac99d10d85b40a2.png)

宏 PF_ * 和 AF_ * 都定义在 bits/socket.h 头文件中，且后者与前者有完全相同的值，所以二者通常混用。

sa_data 成员用于存放 socket 地址值。但是，不同的协议族的地址值具有不同的含义和长度，如下所示：

![image-20230818115654321](https://img-blog.csdnimg.cn/fdfdfc209e3e4390bda1d61957b23fac.png)

由上表可知，14 字节的 sa_data 根本无法容纳多数协议族的地址值。因此，Linux 定义了下面这个新的 通用的 socket 地址结构体，这个结构体不仅提供了足够大的空间用于存放地址值，而且是内存对齐的。

~~~cpp
#include <bits/socket.h>
struct sockaddr_storage
{
    sa_family_t sa_family;
    unsigned long int __ss_align;
    char __ss_padding[ 128 - sizeof(__ss_align) ]; //存储数据
};
typedef unsigned short int sa_family_t;
~~~

### 专用socket地址

**很多网络编程函数诞生早于 IPv4 协议，那时候都使用的是 struct sockaddr 结构体，为了向前兼容，现 在sockaddr 退化成了（void *）的作用，传递一个地址给函数，至于这个函数是 sockaddr_in 还是 sockaddr_in6，由地址族确定，然后函数内部再强制类型转化为所需的地址类型。**

<img src="https://img-blog.csdnimg.cn/29bc05b7ab6044fabf2dda2d33f93ba2.png" alt="image-20230818115728450" style="zoom:80%;" />

UNIX 本地域协议族使用如下专用的 socket 地址结构体：

~~~cpp
#include <sys/un.h>
struct sockaddr_un
{
	sa_family_t sin_family;
	char sun_path[108];
};
~~~

TCP/IP 协议族有 sockaddr_in 和 sockaddr_in6 两个专用的 socket 地址结构体，它们分别用于 IPv4 和 IPv6：

~~~cpp
#include <netinet/in.h>
struct sockaddr_in
{
    sa_family_t sin_family; /* __SOCKADDR_COMMON(sin_) */
    in_port_t sin_port; /* Port number. */
    struct in_addr sin_addr; /* Internet address. */
    /* Pad to size of `struct sockaddr'. */
    unsigned char sin_zero[sizeof (struct sockaddr) - __SOCKADDR_COMMON_SIZE -
    sizeof (in_port_t) - sizeof (struct in_addr)];
};

struct in_addr
{
	in_addr_t s_addr;
};

struct sockaddr_in6
{
    sa_family_t sin6_family;
    in_port_t sin6_port; /* Transport layer port # */
    uint32_t sin6_flowinfo; /* IPv6 flow information */
    struct in6_addr sin6_addr; /* IPv6 address */
    uint32_t sin6_scope_id; /* IPv6 scope-id */
};

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;
#define __SOCKADDR_COMMON_SIZE (sizeof (unsigned short int))
~~~

**所有专用 socket 地址（以及 sockaddr_storage）类型的变量在实际使用时都需要转化为通用 socket 地 址类型 sockaddr（强制转化即可），因为所有 socket 编程接口使用的地址参数类型都是 sockaddr。需要做到兼容**

## IP地址转换（字符串ip-整数 ，主机、网络字节序的转换）

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

我们写代码来巩固：

~~~cpp
#include <iostream>
using namespace std;
#include <arpa/inet.h>

int main() {
    // 创建一个IP字符串
    const char* buf = "192.168.1.4";

    in_addr_t num;
    // 其实这里给包装他的结构体 in_addr 也是可以的，因为函数要求传入的是指针，还是 void* ，脏活系统API帮我们干完了都

    // 将点分十进制的IP字符串转换为网络字节序的整数
    inet_pton(AF_INET, buf, &num);

    unsigned char* p = (unsigned char*)&num;
    // 转化之后的结果应该是数字高位192存放在地址低位，就是大端字节序，所以结果应该是192.168.1.4
    printf("%u %u %u %u\n", *p, *(p + 1), *(p + 2), *(p + 3));

    // 将网络字节序的IP整数转换为字符串形式
    // 字符串形式的IP地址最多多少个字节，一个字符一个字节，然后数字每个最多3位，3个点，加起来就是15，然后\0符，定义16就行
    char ip_str[16] = {0};
    const char* ans = inet_ntop(AF_INET, &num, ip_str, sizeof(ip_str) - 1);

    printf("ans : %s\n", ans);
    printf("ip_str : %s\n", ip_str);
    printf("%d\n", ip_str == ans);

    return 0;
}
~~~

**注意字符串形式的IP地址和数字形式的IP地址的字节数的区别和计算方法，注意不要弄混了!!!**

## TCP通信流程

~~~cpp
// TCP 和 UDP -> 传输层的协议
    UDP:用户数据报协议，面向无连接，可以单播，多播，广播， 面向数据报，不可靠
    TCP:传输控制协议，面向连接的，可靠的，基于字节流，仅支持单播传输(端对端)
    				      UDP 								TCP
    是否创建连接 			 无连接 							面向连接
    是否可靠 			   不可靠 				  			  可靠的
    连接的对象个数 		 一对一、一对多、多对一、多对多       支持一对一
    传输的方式 			  面向数据报 					   面向字节流
    首部开销 		       8个字节 					     最少20个字节
    适用场景 		       实时应用（视频会议，直播） 	  可靠性高的应用（文件传输）
~~~

![image-20230818143501064](https://img-blog.csdnimg.cn/ebc10231c8b5491681c9813c4dccf9f5.png)

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

## 套接字函数

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
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); // socket命名
    - 功能：绑定，将 fd 和 本地的IP + 端口 port 进行绑定
    - 参数：
        - sockfd : 通过socket函数得到的文件描述符
        - addr : 需要绑定的socket地址，这个地址封装了ip和端口号的信息
        - addrlen : 第二个参数结构体占的内存大小
int listen(int sockfd, int backlog); // /proc/sys/net/core/somaxconn
    - 功能：监听这个socket上的连接
    - 参数：
        - sockfd : 通过socket()函数得到的文件描述符
        - backlog : 连接请求等待队列的长度，表示最多有多少个连接请求排队，并不是服务端最多可以连接通信的个数，因为出队列之后就可以进行通信，这个时候请求队列减一恢复了
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    - 功能：接收客户端连接，默认是一个阻塞的函数，阻塞等待客户端连接
    - 参数：
        - sockfd : 用于监听的文件描述符
        - addr : 传出参数，记录了连接成功后客户端的地址信息（ip，port），注意是客户端，这个信息是系统给我的，我获得的
        - addrlen : 指定第二个参数的对应的内存大小
    - 返回值：
        - 成功 ：用于通信的文件描述符
        - -1 ： 失败，设置errno

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    - 功能： 客户端连接服务器
    - 参数：
        - sockfd : 用于通信的文件描述符
        - addr : 客户端要连接的服务器的地址信息，这个信息是我给的，表示我要和谁进行连接
        - addrlen : 第二个参数的内存大小
    - 返回值：成功 0， 失败 -1，设置errno
ssize_t write(int fd, const void *buf, size_t count); // 写数据
ssize_t read(int fd, void *buf, size_t count); // 读数据
~~~

### 示例

我们写一个例子，就是客户端可以从键盘读入字符串，发送给服务端，然后服务端原封不动的返回给客户端

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#define IPV4_STRING_MAX 16
#define MAX_SIZE 1024

// 回射服务器，客户端发送什么服务端就返回什么
int main(int argc, char const* argv[]) {
    // 判断命令行参数个数
    if (argc != 3) {
        printf("usage : %s  <ip_address>  <port>\n", argv[0]);
        return -1;
    }

    const char* server_ip = argv[1];
    const short server_port = atoi(argv[2]);  // atoi()函数可以把合理的字符串转化为整数

    // 1.创建socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listen_fd) {
        perror("socket");
        return -1;
    }

    // 2.绑定IP和端口号
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);  // 注意从主机字节序转换为网络字节序
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);

    int ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    // 3.开始监听
    ret = listen(listen_fd, 8);
    if (-1 == ret) {
        perror("listen");
        return -1;
    }

    // 4.接受连接请求
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int connect_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (-1 == connect_fd) {
        perror("accept");
        return -1;
    }

    // 打印连接的客户端的信息
    char client_ip[IPV4_STRING_MAX] = {0};
    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
    printf("client has connected , ip : %s , port : %d\n", client_ip, ntohs(client_addr.sin_port));

    // 5.开始通信
    // 我们的要求是客户端发送什么，服务端都返回相同的值
    char buf[MAX_SIZE] = {0};
    while (1) {
        bzero(buf, sizeof(buf));
        // 读数据
        int len = read(connect_fd, buf, sizeof(buf) - 1);
        if (-1 == len) {
            perror("read");
            return -1;
        }
        if (len > 0)
            printf("recv data : %s", buf);
        else if (0 == len) {  // 客户端断开连接
            printf("client closed...\n");
            break;
        }

        // 写数据
        write(connect_fd, buf, strlen(buf));
    }

    // 6.关闭连接
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

#define MAX_SIZE 1024

static int count = 0;

int main(int argc, char const* argv[]) {
    // 判断命令行参数个数
    if (argc != 3) {
        printf("usage : %s  <ip_address>  <port>\n", argv[0]);
        return -1;
    }

    const char* server_ip = argv[1];
    const short server_port = atoi(argv[2]);  // atoi()函数可以把合理的字符串转化为整数

    // 1.创建socket
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == connect_fd) {
        perror("socket");
        return -1;
    }

    // 2.建立连接
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // 端口号
    server_addr.sin_port = htons(server_port);
    // IP地址
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);

    int ret = connect(connect_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("connect");
        return -1;
    }
    // 连接成功，输出信息
    printf("connected successfully , waiting for communication.\n");

    // 3.开始通信
    // 我们要求客户端可以从键盘录入值进行通信
    char buf[MAX_SIZE] = {0};
    while (1) {
        bzero(buf, sizeof(buf));
        // 写数据
        fgets(buf, sizeof(buf), stdin);  
        // 这里有一个问题，服务器在阻塞读的时候服务端如果强制退出比如 ctrl + c，就会出问题，这样客户端不会停止，因为他还在写的部分阻塞，没有在读的部分，但是问题不大，因为实际开发当中我们不会对服务器做这样的操作

        // 增加退出功能
        if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "QUIT\n") == 0)
            return 0;

        printf("send : %s", buf);
        write(connect_fd, buf, strlen(buf));

        // 读数据
        bzero(buf, sizeof(buf));
        int len = read(connect_fd, buf, sizeof(buf) - 1);
        if (-1 == len) {
            perror("len");
            return -1;
        }
        if (len > 0)
            printf("recv : %s", buf);

        else if (0 == len) {
            // 服务端关闭了
            printf("server closed...\n");
            break;
        }
    }

    // 4.关闭连接
    close(connect_fd);

    return 0;
}
~~~

**其中为什么read()的结果为0的时候就表示对方关闭了呢？**

**首先对方是写端，我们是读端，如果对方关闭的话，read()会返回0表示读到文件末尾，也就是表示对方关闭了，和管道的操作非常类似**

**总结：**

- **读管道：**
  - **管道中有数据，读取会返回实际读到的字节数**
  - **管道中无数据：**
    - **写端全部关闭，read返回0(相当于读到文件的末尾)**
    - **写端没有完全关闭，read阻塞等待**
- **写管道：**
  - **管道读端全部关闭，产生信号SIGPIPE，进程异常终止**
  - **管道读端没有全部关闭：**
    - **管道已满，write阻塞**
    - **管道没有满，write将数据写入，并返回实际写入的字节数**

当然上面的代码还有点小问题，如图所示，后续修改

## TCP三次握手

TCP 是一种面向连接的单播协议，在发送数据前，通信双方必须在彼此间建立一条连接。所谓的“连接”，其实是客户端和服务器的内存里保存的一份关于对方的信息，如 IP 地址、端口号等。

TCP 可以看成是一种字节流，它会处理 IP 层或以下的层的丢包、重复以及错误问题。在连接的建立过程中，双方需要交换一些连接的参数。这些参数可以放在 TCP 头部。

TCP 提供了一种可靠、面向连接、字节流、传输层的服务，采用三次握手建立一个连接。采用四次挥手来关闭一个连接。

**三次握手的目的是保证了双方互相之间建立了连接。**

**三次握手发生在客户端连接的时候，当调用connect()的时候，底层会通过TCP协议进行三次握手。**

![image-20230818175354581](https://img-blog.csdnimg.cn/6311234b34424045afe8e302b8ab8222.png)

- 16 位端口号（port number）：告知主机报文段是来自哪里（源端口）以及传给哪个上层协议或 应用程序（目的端口）的。进行 TCP 通信时，客户端通常使用系统自动选择的临时端口号。
- 32 位序号（sequence number）：一次 TCP 通信（从 TCP 连接建立到断开）过程中某一个传输 方向上的字节流的每个字节的编号。假设主机 A 和主机 B 进行 TCP 通信，A 发送给 B 的第一个 TCP 报文段中，序号值被系统初始化为某个随机值 ISN（Initial Sequence Number，初始序号 值）。那么在该传输方向上（从 A 到 B），后续的 TCP 报文段中序号值将被系统设置成 ISN 加上 该报文段所携带数据的第一个字节在整个字节流中的偏移。例如，某个 TCP 报文段传送的数据是字 节流中的第 1025 ~ 2048 字节，那么该报文段的序号值就是 ISN + 1025。另外一个传输方向（从 B 到 A）的 TCP 报文段的序号值也具有相同的含义。 
- 32 位确认号（acknowledgement number）：用作对另一方发送来的 TCP 报文段的响应。其值是 收到的 TCP 报文段的序号值 + 标志位长度（SYN，FIN） + 数据长度 。假设主机 A 和主机 B 进行 TCP 通信，那么 A 发送出的 TCP 报文段不仅携带自己的序号，而且包含对 B 发送来的 TCP 报文段 的确认号。反之，B 发送出的 TCP 报文段也同样携带自己的序号和对 A 发送来的报文段的确认序 号。 
- 4 位头部长度（head length）：标识该 TCP 头部有多少个 32 bit(4 字节)。因为 4 位最大能表示 15，所以 TCP 头部最长是60 字节。
- 6 位标志位包含如下几项：
  - URG 标志，表示紧急指针（urgent pointer）是否有效。 
  - ACK 标志，表示确认号是否有效。我们称携带 ACK 标志的 TCP 报文段为确认报文段。 PSH 标志，提示接收端应用程序应该立即从 TCP 接收缓冲区中读走数据，为接收后续数据腾 出空间（如果应用程序不将接收到的数据读走，它们就会一直停留在 TCP 接收缓冲区中）。 
  - RST 标志，表示要求对方重新建立连接。我们称携带 RST 标志的 TCP 报文段为复位报文段。 
  - SYN 标志，表示请求建立一个连接。我们称携带 SYN 标志的 TCP 报文段为同步报文段。 
  - FIN 标志，表示通知对方本端要关闭连接了。我们称携带 FIN 标志的 TCP 报文段为结束报文 段。 
  - 16 位窗口大小（window size）：是 TCP 流量控制的一个手段。这里说的窗口，指的是接收 通告窗口（Receiver Window，RWND）。它告诉对方本端的 TCP 接收缓冲区还能容纳多少 字节的数据，这样对方就可以控制发送数据的速度。 
  - 16 位校验和（TCP checksum）：由发送端填充，接收端对 TCP 报文段执行 CRC 算法以校验 TCP 报文段在传输过程中是否损坏。注意，这个校验不仅包括 TCP 头部，也包括数据部分。 这也是 TCP 可靠传输的一个重要保障。 
  - 16 位紧急指针（urgent pointer）：是一个正的偏移量。它和序号字段的值相加表示最后一 个紧急数据的下一个字节的序号。因此，确切地说，这个字段是紧急指针相对当前序号的偏移，不妨称之为紧急偏移。TCP 的紧急指针是发送端向接收端发送紧急数据的方法。

![image-20230818175513668](https://img-blog.csdnimg.cn/e313e454b4ee445faaebad2c749c16f8.png)

![image-20230819113154633](https://img-blog.csdnimg.cn/8babe97b873a44b79f57b076d95cfcba.png)

### 类比

我们用男女朋友的例子来进行举例，画图如下：

![image-20230819103736969](https://img-blog.csdnimg.cn/21286cbe7afe4bda86c094e21cdda255.png)

男生向女生说能不能做我女朋友，女生说可以，但是这个时候女生不放心，需要问男生能不能做自己的男朋友，为了进行确认；男生回复可以，双方都表示愿意做男女朋友，也就是双方都要确认对方和自己能够建立连接并且能够发送消息和收到消息，这才能保证整个TCP连接的可靠性，这也是为什么TCP连接需要建立连接

### 为什么需要三次握手而不是两次握手？

**为了能够建立起可靠的连接，客户端和服务端双方都必须各自确认一些信息才能保证整个连接是可靠的，就是确认双方都能接受和发送消息，好，我们一次一次来看**

- **第一次客户端发送SYN=1的请求连接消息，这个时候客户端能够确认自己的发送数据没有问题；服务端收到请求连接消息之后能够确认自己的接收数据没有问题，并且还能够确认客户端的发送数据没有问题**
- **第二次服务端发送ACK=1 SYN=1的确认信息，服务端能够确认自己的发送数据没有问题，客户端收到服务端的消息之后能够确认服务端的发送数据没有问题，并且由于这条消息是因为客户端发送请求，服务端回复的，因此客户端还能确认服务端的收到数据没有问题，至此，客户端已经能够完全确认自己和服务端收发数据都没有问题了**
- **但是我们现在观察服务端，他还没有办法确认客户端接收数据有没有问题，因为服务端还没有收到客户端的回复报文，因此我们需要第三次握手，客户端针对上一条确认报文在发送一条确认报文，这个时候服务端才能完全确认所有都没问题，换句话说，这个连接才是可靠的!!!**

## TCP滑动窗口

滑动窗口（Sliding window）是一种流量控制技术。早期的网络通信中，通信双方不会考虑网络的拥挤情况直接发送数据。由于大家不知道网络拥塞状况，同时发送数据，导致中间节点阻塞掉包， 谁也发不了数据，所以就有了滑动窗口机制来解决此问题。滑动窗口协议是用来改善吞吐量的一种 技术，即容许发送方在接收任何应答之前传送附加的包。接收方告诉发送方在某一时刻能送多少包 （称窗口尺寸）。 

TCP 中采用滑动窗口来进行传输控制，滑动窗口的大小意味着接收方还有多大的缓冲区可以用于 接收数据。发送**方可以通过滑动窗口的大小来确定应该发送多少字节的数据。当滑动窗口为 0 时，发送方一般不能再发送数据报。 

滑动窗口是 TCP 中实现诸如 ACK 确认、流量控制、拥塞控制的承载结构。

窗口理解为缓冲区的大小

滑动窗口的大小会随着发送数据和接收数据而变化

通信的双方都有发送缓冲区和接受缓冲区

- 服务器：
  - 发送缓冲区  (发送缓冲区的窗口)
  - 接受缓冲区  (接受缓冲区的窗口)
- 客户端：
  - 发送缓冲区  (发送缓冲区的窗口)
  - 接受缓冲区  (接受缓冲区的窗口)

![image-20230818175617550](https://img-blog.csdnimg.cn/fbc9abfcc1e846519362531b33624995.png)

~~~cpp
发送方的缓冲区：
    白色格子：空闲的空间
    灰色格子：数据已经被发送出去了，但是还没有被接受
   	紫色格子：还没有发送出去的数据
    
接受方的缓冲区：
    白色格子：空闲的空间
    紫色格子：已经接收到的数据
~~~

![image-20230818175628648](https://img-blog.csdnimg.cn/4c02090435e749748d1d9a0636d517dd.png)

~~~cpp
# mss : Maximum Segment Size(一条数据最大的数据量)
# win : 滑动窗口

1.客户端向服务器发送连接，客户端的滑动窗口是4096，一次发送的最大数据量是1460
2.服务器接受连接请求，告诉客户端服务器的窗口大小是6144，一次发送的最大数据量是1024
3.第三次握手
4.4-9 客户端连续给服务器发送了6k的数据，每次发送1k
5.第10次，服务器告诉客户端：发送的6k数据已经接收到，存储在缓冲区中，缓冲区数据已经处理了2k，窗口大小是2k
6.第10次，服务器告诉客户端：发送的6k数据已经接收到，存储在缓冲区中，缓冲区数据已经处理了4k，窗口大小是4k
7.第12次，客户端给服务器发送了1k的数据
8.第13次，客户端主动请求和服务端断开连接，并且给服务器发送1k的数据
9.第14次，服务器回复ACK 8194，同意客户端断开连接的请求，并且告诉客户端已经接收到刚才的2k的数据，并且指出滑动窗口的大小
10.第15，16次，通知客户端滑动窗口的大小
11.第17次，第三次挥手，服务端给客户端发送FIN，请求断开连接
12.第18次，第四次挥手，客户端同意了服务端的断开连接请求
~~~

## TCP四次挥手

**四次挥手发生在断开连接的时候，在程序中当调用close()会使用TCP协议进行4次挥手。**

**客户端和服务端都可以主动发起断开连接，谁先调用close()谁就先发起**

**因为在TCP连接的时候，采用三次握手建立的连接是双向的，因此在断开的时候也需要双向断开，这就是为什么需要四次挥手**

![image-20230818175657616](https://img-blog.csdnimg.cn/7a807755fa5f4c2cb67f26c6be5f830c.png)

### 类比

还是用男女的例子来举例：

**客户端向服务端发出断开连接请求FIN，服务端接受请求，并返回确认，至此，客户端向服务端方向的数据传输就断开了；但是服务端仍可以向客户端发送数据，当发送数据完毕之后(或者不发)服务端向客户端发送断开连接请求FIN，客户端接受请求，然后返回针对该请求报文的确认，至此服务端向客户端方向的数据传输断开，该TCP连接就此关闭**

![image-20230819150618612](https://img-blog.csdnimg.cn/6c083bd2e2624bae90f70125b6b56c5d.png)

那我们想一下，第二步和第三步能不能合起来呢？也就是：

### 为什么要四次挥手而不是三次挥手？

**我个人的理解是，首先前两步和后两步的工作是不同的，前两步是针对客户端向服务端释放连接的(在这里是客户端先释放连接)，后两步是针对服务端向客户端释放连接，功能不同，针对不同；其次，在第三步服务端发出释放连接请求之前，服务端还可以但单方面的向客户端发送数据，这时客户端虽然不能发送数据，但是仍可以接受服务端发送的数据，服务端很可能还有没有发送完毕的数据想要发送，因为这个释放连接的请求是客户端提出的，这样两个步骤就必须分开了**

## TCP通信并发

### 多进程实现并发服务器

**要实现TCP通信服务器并发的任务，使用多线程或者多进程解决**

**思路：**

- **一个父进程，多个子进程**
- **父进程负责等待并且接受客户端的连接**
- **子进程负责完成通信，接受一个客户端连接，创建一个子进程用于通信**

以下是代码：

完成的功能是，服务端可以接受多个客户端的连接，然后客户端键入数据，服务端返回相同的数据

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_IPV4_STRING 16
#define MAXSIZE 1024

void Recycle_Callback(int signum) {
    while (1) {
        // 第二个参数可以用来获取子进程退出的状态
        int ret = waitpid(-1, nullptr, WNOHANG);
        if (-1 == ret)
            //-1表示回收错误，也就是没有子进程了，为了达到这个判断，我们使用非阻塞的，因为阻塞的他会阻塞
            return;
        else if (0 == ret)  // 0在非阻塞的情况下代表还有子进程活着，重开循环把结束的子进程尽可能都回收
            continue;
        else if (ret > 0)
            // 回收了某个子进程
            printf("child process (pid %d) has be recycled.\n", ret);
    }
}

// 接受多个客户端的连接，这个程序用多进程来处理
int main(int argc, char const* argv[]) {
    // 创建出来子进程父进程需要对其进行回收的操作，但是wait()或者waitpid()无论是阻塞还是非阻塞的情况都没办法实现我们想要的操作
    // 所以想到捕捉信号SIGCHID
    struct sigaction sig_child;
    sig_child.sa_flags = 0;
    sigemptyset(&sig_child.sa_mask);  // 不阻塞任何临时的信号
    sig_child.sa_handler = Recycle_Callback;

    sigaction(SIGCHLD, &sig_child, nullptr);

    // 命令行参数
    if (argc != 3) {
        printf("usage : %s  <ip_address>  <port>\n", argv[0]);
        return -1;
    }

    const char* server_ip = argv[1];
    const int server_port = atoi(argv[2]);

    // 1.创建socket套接字
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listen_fd) {
        perror("socket");
        return -1;
    }

    // 2.将套接字绑定IP和端口
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // 端口，注意转换字节序
    server_addr.sin_port = htons(server_port);
    // IP
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);

    int ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    // 3.开始监听
    ret = listen(listen_fd, 5);
    if (-1 == ret) {
        perror("listen");
        return -1;
    }

    // 不断循环等待客户端连接
    while (1) {
        // 4.接受请求
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int connect_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (-1 == connect_fd) {
            if (errno == EINTR)
                // 说明产生了信号发生了软中断，执行回来accept()就不阻塞了，这是预料之内的状态，我们对其进行细微处理
                continue;
            perror("accept");
            return -1;
        }

        // 5.开始通信，在子进程中进程通信
        pid_t pid = fork();
        if (-1 == pid) {
            perror("fork");
            return -1;
        }
        if (0 == pid) {  // 子进程
            //  输出连接的客户端的IP和端口
            char client_ip[MAX_IPV4_STRING] = {0};
            inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
            in_port_t client_port = ntohs(client_addr.sin_port);
            printf("one client has connected , ip : %s , port : %d\n", client_ip, client_port);

            // 处理数据，接受客户端的数据并且相同返回
            char buf[MAXSIZE] = {0};
            while (1) {
                // 读
                bzero(buf, sizeof(buf));
                int len = read(connect_fd, buf, sizeof(buf) - 1);  // 这里老师提到要注意\0，我这么做也是可以的，我在读的时候留出\0的空间，写的时候我全部写入，这样我的buf最后肯定是有\0的
                if (-1 == len) {
                    perror("read");
                    return -1;
                }
                if (len > 0)
                    printf("recv client (ip : %s , port : %d) data : %s", client_ip, client_port, buf);
                else if (0 == pid) {
                    // 写端断开连接，子进程任务结束，退出
                    printf("client (ip : %s , port : %d) has closed...\n", client_ip, client_port);
                    close(connect_fd);
                    goto FINAL;  // 跳转到程序结束的位置
                }

                // 写
                write(connect_fd, buf, strlen(buf));
            }
        } else if (pid > 0)  // 父进程
            continue;
    }

    // 6.断开连接
FINAL:
    close(listen_fd);

    return 0;
}
~~~

执行结果如下：

直截取了服务端的图片

![image-20230819170952283](https://img-blog.csdnimg.cn/4418a2bfc3a942a48eb1969382f2b1a9.png)

**服务器的代码涉及到两个问题：**

- **为了防止僵尸进程，服务端的父进程必须对所有的子进程进行回收，那么如何有效的对子进程进行回收？**

  **父进程是在不断的阻塞等待客户端的连接，调用accept()函数，在这里如果回收子进程是非常不好的，所以我们结合子进程结束会发出SIGCHID信号，这个信号默认被父进程忽略，但是我们捕捉这个信号，然后开启软中断，就可以对子进程进行回收了**

  **我们可以调用waitpid()函数对所有的子进程进行回收，但是一次只能回收一个所以需要while(1)，然后由于我们需要判断没有子进程可以回收的状态以此来跳出中断，因此我们使用非阻塞的waitpid()，当服务端结束的时候，对应的子进程也结束，我的非阻塞的waitpid()就尽可能的把已经结束的子进程给全部回收掉了，避免僵尸进程的出现**

- **那么这样的话我们执行就会出现第二个问题，如下：**

  **当我一个子进程结束的时候，父进程中断处理之后，应该回到原来accept()的状态，但是这时accept()报错了**

  **![image-20230819172037175](https://img-blog.csdnimg.cn/155eff53539a4f8a9193ed1dcbacf286.png)**

  **是什么原因呢？**

  **因为accpt()这里软中断结束之后，会从阻塞中断变成非阻塞的，然后没有客户端连接，就报错了，程序结束，errno被设置为EINTR**

  **![image-20230819172505207](https://img-blog.csdnimg.cn/448c70833e0343a48a7f199753ad07ff.png)**

  **所以这里我们判断一下errno的值特殊处理就好了**

客户端代码基本没变，所以不用深究

~~~cpp
// client.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_SIZE 1024

static int count = 0;

int main(int argc, char const* argv[]) {
    // 判断命令行参数个数
    if (argc != 3) {
        printf("usage : %s  <ip_address>  <port>\n", argv[0]);
        return -1;
    }

    const char* server_ip = argv[1];
    const short server_port = atoi(argv[2]);  // atoi()函数可以把合理的字符串转化为整数

    // 1.创建socket
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == connect_fd) {
        perror("socket");
        return -1;
    }

    // 2.建立连接
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // 端口号
    server_addr.sin_port = htons(server_port);
    // IP地址
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);

    int ret = connect(connect_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("connect");
        return -1;
    }
    // 连接成功，输出信息
    printf("connected successfully , waiting for communication.\n");

    // 3.开始通信
    // 我们要求客户端可以从键盘录入值进行通信
    char buf[MAX_SIZE] = {0};
    while (1) {
        bzero(buf, sizeof(buf));
        // 写数据
        fgets(buf, sizeof(buf), stdin);  // 这里有一个问题，服务器在阻塞读的时候服务端如果强制退出比如 ctrl + c，就会出问题，这样客户端不会停止，因为他还在写的部分阻塞，没有在读的部分，但是问题不大，因为实际开发当中我们不会对服务器做这样的操作

        // 增加退出功能
        if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "QUIT\n") == 0)
            return 0;

        printf("send : %s", buf);
        write(connect_fd, buf, strlen(buf));

        // 读数据
        bzero(buf, sizeof(buf));
        int len = read(connect_fd, buf, sizeof(buf) - 1);
        if (-1 == len) {
            perror("len");
            return -1;
        }
        if (len > 0)
            printf("recv : %s", buf);
        else if (0 == len) {
            // 服务端关闭了
            printf("server has closed...\n");
            break;
        }
    }

    // 4.关闭连接
    close(connect_fd);

    return 0;
}
~~~

### 多线程实现并发服务器

服务端的代码需要注意一些细节：

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define MAXSIZE 1024
#define MAX_IPV4_STRING 16
#define MAX_INFO_SIZE 128

// 封装一个结构体来保存需要传给子线程的信息
struct Pthread_Info {
    int _connect_fd;                  // 用于通信的文件描述符
    struct sockaddr_in _client_addr;  // 客户端的socket地址信息
} p_infos[MAX_INFO_SIZE];

// 定义一个这个数组的计数器
int count = 0;

// 线程处理的回调函数
void* Communicate_Callback(void* args) {
    // 接受参数得到通信用到的信息
    int connect_fd = ((Pthread_Info*)args)->_connect_fd;
    struct sockaddr_in client_addr = ((Pthread_Info*)args)->_client_addr;

    in_port_t client_port = ntohs(client_addr.sin_port);

    char client_ip[MAX_IPV4_STRING] = {0};
    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
    printf("one client has connected , ip : %s , port : %d\n", client_ip, client_port);

    // 开始通信
    char buf[MAXSIZE] = {0};
    while (1) {
        // 读
        bzero(buf, sizeof(buf));
        int len = read(connect_fd, buf, sizeof(buf) - 1);
        if (-1 == len) {
            perror("read");
            return (void*)-1;
        }
        if (len > 0)
            printf("recv client ( ip : %s , port : %d ) data : %s", client_ip, client_port, buf);
        else if (0 == len) {  // 写端关闭，也就是客户端关闭连接，才会返回0
            printf("client ( ip : %s , port : %d ) has closed...\n", client_ip, client_port);
            close(connect_fd);
            printf("child thread has closed , tid : %ld\n", pthread_self());

            // // 释放堆上的这块内存
            // delete args;

            return nullptr;
        }

        // 写
        write(connect_fd, buf, strlen(buf));
    }

    // // 释放堆上的这块内存
    // delete args;

    return nullptr;
}

// 用多线程实现服务器并发
int main(int argc, char const* argv[]) {
    // 初始化数据
    int size = sizeof(p_infos) / sizeof(p_infos[0]);
    for (int i = 0; i < size; ++i) {
        // 将所有的都初始化为0
        bzero(&p_infos[i], sizeof(p_infos[i]));
        // 文件描述符初始化为-1，不能让他占据正在使用的
        p_infos[i]._connect_fd = -1;
    }

    // 命令行参数
    if (argc != 3) {
        printf("usage : %s  <ip_address>  <port>\n", argv[0]);
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

    // 2.绑定IP和端口
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // 端口
    server_addr.sin_port = htons(server_port);
    // IP
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);

    int ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    // 3.监听
    ret = listen(listen_fd, 5);
    if (-1 == ret) {
        perror("listen");
        return -1;
    }

    printf("server has initialized...\n");

    // 4.接受客户端请求
    while (1) {
        // 思路：主线程不断接受客户端请求，然后创建子线程和客户端进行通信
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int connect_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (-1 == connect_fd) {
            perror("accept");
            return -1;
        }

        // 这里是局部变量，为了保证循环一次不会被释放，我们选择其他的方式
        // 我们可以选择用堆来存储，但是一是客户端数量多了没有办法进行限制，而是还要处理释放并且消耗资源大，所以我们可以开一个全局数组

        struct Pthread_Info& p_info = p_infos[count++];  // 创建数组成员的引用别名
        if (count >= MAX_INFO_SIZE) {
            // 超出最大客户端连接数量
            printf("client oversize , closing...\n");
            return 0;
        }
        p_info._connect_fd = connect_fd;
        p_info._client_addr = client_addr;  // 这个系统类实现了copy assignment，实现了深拷贝

        // 5.开始通信
        pthread_t tid;
        pthread_create(&tid, nullptr, Communicate_Callback, &p_info);
        // 将子线程分离，不用手动回收
        pthread_detach(tid);
    }

    // 6.关闭连接
    close(listen_fd);

    // 退出主线程，这里就这么写吧，主线程不可能比子线程早结束，因为主线程要等待
    pthread_exit(nullptr);

    return 0;
}
~~~

客户端的代码基本没变，可以不用深究

~~~cpp
// client.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#define MAXSIZE 1024

int main(int argc, char const* argv[]) {
    // 命令行
    if (argc != 3) {
        printf("usage : %s  <ip_address>  <port>\n", argv[0]);
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

    // 2.建立连接
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // 端口
    server_addr.sin_port = htons(server_port);
    // IP
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);

    int ret = connect(connect_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("connect");
        return -1;
    }

    printf("connected successfully , waiting for communicating.\n");

    char buf[MAXSIZE] = {0};
    // 3.开始通信
    while (1) {
        // 写
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);

        // 增加退出功能
        if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "QUIT\n") == 0)
            return 0;

        write(connect_fd, buf, strlen(buf));
        printf("send : %s", buf);

        // 读
        bzero(buf, sizeof(buf));
        int len = read(connect_fd, buf, sizeof(buf) - 1);
        if (-1 == len) {
            perror("read");
            return -1;
        }
        if (len > 0)
            printf("recv : %s", buf);
        else if (0 == len) {  // 说明写端关闭，也就是服务端关闭
            printf("server has closed...\n");
            break;
        }
    }

    // 4.关闭连接
    close(connect_fd);

    return 0;
}
~~~

## TCP状态转换

![image-20230819153502987](https://img-blog.csdnimg.cn/6e5a8d6fd144457c8b8dd990c9475776.png)

![image-20230819153510357](https://img-blog.csdnimg.cn/4b599e1fa6854c258f3839b2eaedd3bf.png)

### 2MSL（Maximum Segment Lifetime） 

**主动断开连接的一方, 最后进出入一个 TIME_WAIT 状态, 这个状态会持续: 2msl** 

- msl: 官方建议: 2分钟, 实际是30s
  当 TCP 连接主动关闭方接收到被动关闭方发送的 FIN 和最终的 ACK 后，连接的主动关闭方 必须处于TIME_WAIT 状态并持续 2MSL 时间。 
  **这样就能够让 TCP 连接的主动关闭方在它发送的 ACK 丢失的情况下重新发送最终的 ACK。 为了让被动关闭方第三次挥手的FIN应该获得的第四次挥手的ACK能够正确到达，如果最后一次ACK丢失了被动关闭方会及时再次发送第三次的FIN让主动关闭方发送ACK来确认关闭，否则直接关闭就没办法让被动关闭方确认了，也就是关闭的不完整**
  主动关闭方重新发送的最终 ACK 并不是因为被动关闭方重传了 ACK（它们并不消耗序列号， 被动关闭方也不会重传），而是因为被动关闭方重传了它的 FIN。事实上，被动关闭方总是 重传 FIN 直到它收到一个最终的 ACK。 

### 半关闭

**在四次挥手关闭的过程中主动断开连接方收到了两次被动连接方的报文，第一个是第二次挥手的ACK确认，第二个是第三次挥手的FIN请求，为什么叫FIN_WAIT_1和FIN_WAIT_2？就是因为第一次主动断开连接方发送FIN请求后等待ACK确认，然后第二次就主动等待被动连接方的FIN请求，最后就进入TIME_WAIT状态，这么说了理解得更透彻，参照下图理解**

<img src="https://img-blog.csdnimg.cn/7a39d72a3f634f828c6924c18eb27a02.png" alt="image-20230821093427390" style="zoom: 80%;" />

**当 TCP 链接中 A 向 B 发送 FIN 请求关闭，另一端 B 回应 ACK 之后（A 端进入 FIN_WAIT_2 状态），并没有立即发送 FIN 给 A，A 方处于半连接状态（半开关），此时 A 可以接收 B 发 送的数据，但是 A 已经不能再向 B 发送数据。**

#### shutdown()

从程序的角度，可以使用 API 来控制实现半连接状态：

~~~cpp
#include <sys/socket.h>
int shutdown(int sockfd, int how);
    sockfd: 需要关闭的socket的描述符
    how: 允许为shutdown操作选择以下几种方式:
        SHUT_RD(0): 关闭sockfd上的读功能，此选项将不允许sockfd进行读操作。
                    该套接字不再接收数据，任何当前在套接字接受缓冲区的数据将被无声的丢弃掉。
        SHUT_WR(1): 关闭sockfd的写功能，此选项将不允许sockfd进行写操作。进程不能在对此套接字发
                    出写操作。
        SHUT_RDWR(2):关闭sockfd的读写功能。相当于调用shutdown两次：首先是以SHUT_RD,然后以
                    SHUT_WR。
~~~

**使用 close 中止一个连接，但它只是减少描述符的引用计数，并不直接关闭连接，只有当描述符的引用计数为 0 时才关闭连接。**

**shutdown 不考虑描述符的引用计数，直接关闭描述符。也可选择中止一个方向的连接，只中止读或只中止写。** 

注意: 

1. **如果有多个进程共享一个套接字，close 每被调用一次，计数减 1 ，直到计数为 0 时，也就是所用进程都调用了 close，套接字将被释放。当然遇到父子进程的时候注意一下他们内核区是共享的还是独立的，下面会谈到** 

2. **在多进程中如果一个进程调用了 shutdown(sfd, SHUT_RDWR) 后，其它的进程将无法进行通信，因为直接关闭，不看引用计数。 但如果一个进程 close(sfd) 将不会影响到其它进程。**

3. **引申：为什么多进程中的文件描述符的引用计数不为1呢？我们来看我们代码的逻辑：**

   **我们用父进程接受连接，然后用子进程进行处理；文件描述符是内核区的，因此父进程和子进程就共享了这一份文件描述符数据了，父进程当然可以通过这个通信，但是逻辑没有这么干**
   ![image-20230822153655659](https://img-blog.csdnimg.cn/5ecddded39904d828ffee078685999c5.png)

#### 关于文件描述符的引用计数(file_description)

我们先从父子进程看起，就是父子进程对于文件描述符，下面有两段代码：

~~~cpp
// code1
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXSIZE 1024

int main() {
    int fd = open("a.txt", O_RDONLY);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    char buf[MAXSIZE] = {0};

    pid_t pid = fork();
    if (-1 == pid) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        read(fd, buf, 1);  // 我给buf中读进去1个字节内容
        printf("buf = %s\n", buf);
    } else if (pid > 0) {
        sleep(3);          // 保证子进程可以先读文件
        read(fd, buf, 2);  // 父进程中读取2个字节

        printf("buf = %s\n", buf);
        wait(nullptr);
    }

    close(fd);
    return 0;
}

// 执行结果：
// buf = h
// buf = el
~~~

~~~cpp
// code2
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXSIZE 1024

int main() {
    pid_t pid = fork();
    if (-1 == pid) {
        perror("fork");
        return -1;
    }

    int fd = open("a.txt", O_RDONLY);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    char buf[MAXSIZE] = {0};

    if (pid == 0) {
        read(fd, buf, 1);  // 我给buf中读进去1个字节内容
        printf("buf = %s\n", buf);
    } else if (pid > 0) {
        sleep(3);          // 保证子进程可以先读文件
        read(fd, buf, 2);  // 父进程中读取2个字节

        printf("buf = %s\n", buf);
        wait(nullptr);
    }

    close(fd);
    return 0;
}

// 执行结果：
// buff = h
// buff = he
~~~

**我们仔细观察这两个代码，发现第一个代码先open()文件在fork()子进程；第二个代码先fork()子进程在open()文件，第一个代码的执行结果发现父子进程的文件指针好像是公用的，第二个则是独立的，那么这是为什么呢？**

**在Linux系统中父子进程在实际操作的时候具有"读时共享，写时拷贝"的性质，这个是针对用户区的，我们通过open()函数打开的文件描述符fd是属于内核区的，并且内核区还专门设有一个文件描述符表用来存放文件描述符，比如  STDIN_FILENO 0 ， STDOUT_FILENO 1 ， STDERR_FILENO 2 ，由于先打开open()再创建子进程fork()，那么他们内核区的数据是共享的，因此文件描述符也是用的同一个，文件描述符的引用计数为1，虽然close()的机制是引用计数为0才真正关闭，但是这里为1，因此关闭任意一个都相当于关闭了这个文件描述符，因此他们读取的文件指针也是共享的，所以开始读的位置不相同；但是第二个先创建子进程fork()再打开open()，显然这两个的文件描述符是不同的，这就跟匿名管道pipe通信父子进程需要先创建匿名管道pipe再创建子进程fork()一个道理**

**总结：**

- **fork前进行open，子进程无条件继承父进程的文件描述信息，子进程和父进程指向一样文件描述信息**
- **fork后进行open，子进程可以有自己的选择啊，不用继承父进程的所有，比如文件描述信息**

## 端口复用(关于sockopt())

**端口复用最常用的用途是:** 

- **防止服务器重启时之前绑定的端口还未释放** 
- **程序突然退出而系统没有释放端口**

**这两个的本质都是因为某些原因主动连接方程序退出了但是TCP的信息还在TIME_WAIT状态，可能这个时候被动连接方没办法收到LAST ACK，这就会导致一直处在TIME_WAIT状态，然后TIME_WAIT的时间2mss一般是60s，很长，很烦，这就导致端口一直被占用而没办法及时进行后续操作**

~~~cpp
#include <sys/types.h>
#include <sys/socket.h>
// 设置套接字的属性(不仅仅能够设置端口复用)
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
参数：
    - sockfd：要操作的套接字的文件描述符
    - level：级别 SOL_SOCKET (端口复用的级别)
    - optname：选项的名称
    	- SO_REUSEADDR
    	- SO_REUSEPORT
    - optval：属性的值，可以是int类型，也可以是其他类型，所以用void*接受，这里是整型
    	- 1：可以复用
    	- 0：不可以复用
    - optlen：上一个属性的长度
返回值：
    成功 0
    失败 -1，设置errno
 
端口复用，设置的时机是服务器绑定端口之前，先设置再 bind() ，否则就失效了
~~~

关于level参数，有很多，我们这里选择 SOL_SOCKET，代表是端口复用的级别

<img src="https://img-blog.csdnimg.cn/10ac5b0ecb3a47e9ae9ab51e4c73caa6.png" alt="image-20230820172341423" style="zoom: 80%;" />

查看网络信息相关的命令

~~~bash
netstat
参数：
	-a 显示所有的socket
	-p 显示正在使用socket的程序的名称
	-n 直接使用IP地址，不通过域名服务器
~~~

我们可以使用命令 **netstat -anp** 来查看相关的信息

### 示例

我们现在来看一段代码，是关于TCP通信的：

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_IPV4_STRING 16
#define MAXSIZE 1024

int main() {
    // 创建socket
    int listen_fd = socket(PF_INET, SOCK_STREAM, 0);

    if (listen_fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9999);

    // 设置端口复用(在绑定之前)
    int optval = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    // 绑定
    int ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("bind");
        return -1;
    }

    // 监听
    ret = listen(listen_fd, 8);
    if (ret == -1) {
        perror("listen");
        return -1;
    }

    // 接收客户端连接
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int connect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
    if (connect_fd == -1) {
        perror("accpet");
        return -1;
    }

    // 获取客户端信息
    char client_ip[MAX_IPV4_STRING] = {0};
    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
    unsigned short client_port = ntohs(client_addr.sin_port);

    // 输出客户端的信息
    printf("client's ip is %s, and port is %d\n", client_ip, client_port);

    // 接收客户端发来的数据
    char buf[MAXSIZE] = {0};
    while (1) {
        int len = recv(connect_fd, buf, sizeof(buf), 0);
        if (len == -1) {
            perror("recv");
            return -1;
        } else if (0 == len) {
            printf("客户端已经断开连接...\n");
            break;
        } else if (len > 0)
            printf("read buf = %s", buf);

        // 小写转大写
        for (int i = 0; i < len; ++i)
            buf[i] = toupper(buf[i]);

        printf("after buf = %s", buf);

        // 大写字符串发给客户端
        ret = send(connect_fd, buf, strlen(buf) + 1, 0);
        if (ret == -1) {
            perror("send");
            return -1;
        }
    }

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

#define MAXSIZE 1024

int main() {
    // 创建socket
    int connect_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (connect_fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);

    // 连接服务器
    int ret = connect(connect_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (ret == -1) {
        perror("connect");
        return -1;
    }

    while (1) {
        char buf[MAXSIZE] = {0};
        fgets(buf, sizeof(buf), stdin);

        write(connect_fd, buf, strlen(buf) + 1);

        // 接收
        int len = read(connect_fd, buf, sizeof(buf));
        if (len == -1) {
            perror("read");
            return -1;
        } else if (len > 0)
            printf("read buf = %s", buf);
        else {
            printf("服务器已经断开连接...\n");
            break;
        }
    }

    close(connect_fd);

    return 0;
}
~~~

#### 理解

我们先把TCP通信的图拿过来

<img src="https://img-blog.csdnimg.cn/6e5a8d6fd144457c8b8dd990c9475776.png" alt="image-20230819153502987" style="zoom:67%;" />

这段代码的作用是服务端发送字符串，服务端接受并且返回大写后的版本，代码逻辑我们不谈，前面早就谈过了，我们来看看释放连接时候会发生些什么事情

server.cpp代码当中有一个注释的部分，这个部分是用来设置端口复用的，我们现在先不管

~~~cpp
int optval = 1;
setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
~~~

在代码中我们指定服务端的端口为9999，IP为任意IP(只要能连接)，然后用网络命令来查看一下状态

通过管道和grep命令通信来过滤剩下9999的信息

~~~bash
netstat -anp | grep 9999
~~~

我们来看下正常状态时候的输出：

服务端有两条信息，一条是用来监听的，因为我们没有关闭监听套接字，理论上它还可以继续accept()客户端，只是我们代码逻辑没有实现这个；另一条和客户端是已经建立连接；客户端就是和服务端建立连接

服务端使用的端口是9999

![image-20230820174841668](https://img-blog.csdnimg.cn/772dafc7511847ef91ab27e311467657.png)

现在我们使用 ctrl + c 发送SIGINT信号是服务端异常终止，输出：

此时，服务端到客户端的单向连接就关闭了，但是客户端还在，服务端处于FIN_WAIT2状态，等待客户端发送FIN的关闭连接报文；客户端处于CLOSE_WAIT状态

![image-20230820175148993](https://img-blog.csdnimg.cn/1276edb1e62642a2a6a28ad6347cd18e.png)

好，现在我们也类似强制关闭客户端，输出：

此时服务端进入TIME_WAIT状态，这个状态服务端接收到了FIN报文然后发送针对该报文的ACK报文，为了避免最后一次报文丢失，所以会有这个阶段，这个阶段的持续时间是2MSL，在Linux上大概是60秒

![image-20230820175507945](https://img-blog.csdnimg.cn/52490827b03e42bfbd2a9d862199c888.png)

如果我们在这段时间重开服务器，就会这样，表示端口被占用，这就是TIME_WAIT惹的祸

![image-20230820175849220](https://img-blog.csdnimg.cn/62498c08b93b41e9bb2f7ec28f9dac41.png)

因此，setsockapt()就是可以设置这个属性，使得端口能够复用，将那行代码恢复后，就可以正常打开服务器，并且网络状态是这样的

可以看出，之前的TIME_WAIT和现在的LISTEN状态共存了，这就是setsockapt()的作用，当然他不只可以设置端口复用，还有其他功能

![image-20230820180107223](https://img-blog.csdnimg.cn/97d1a68860a647ca9ec712fd5b09942b.png)

#### 思考

我现在不让客户端异常退出，当服务端异常退出后，我们代码的逻辑可以让客户端阻塞在输入的位置，所以这时我们输入代码然后就可以正常退出程序，但是正常退出程序之后我们发现TIME_WAIT状态不存在，也就是结束了，换句话说客户端正常收到了最后一次ACK，而我们刚才异常退出的时候客户端没办法收到最后一次ACK，所以端口被占用，需要端口复用，这点要注意

但是再想想，服务端也是异常退出的，他怎么收到了第二次ACK呢？这个问题我不知道准确的答案，但是我推测虽然服务端结束了，但是服务端的TCP信息尚未结束，因为服务端还要收到客户端的主动断开请求，这里是服务端先断开，但是被断开方客户端异常退出时就收不到最后一次ACK，所以会卡在TIME_WAIT状态(不是很理解)

**问题的关键点就在于被断开方最后一次ACK到底能否准确收到，我不知道怎么解释，但是TIME_WAIT会告诉我答案，这就是内核相关的东西了，我目前的水平达不到**

不明白其实没关系，只需要知道TIME_WAIT状态会导致端口占用就可以了，我们自己可以用网络命令查看，然后决定是否需要端口复用

## I/O多路复用(I/O多路转接)

### I/O含义

**I/O就是英文单词in out，但是不是指从键盘读入数据叫in或者输出到中断屏幕到out，它指的是我们操作文件或者管道或者套接字，依赖文件描述符，将数据和内存(比如缓冲区)进行通信，写入内存叫in，从内存中读取叫out；比如我们的TCP通信的例子，连接套接字的形式就是文件描述符fd，然后通过他的读写是先存到内存缓冲区的，这就有很多的I/O过程了**

**I/O 多路复用使得程序能同时监听多个文件描述符，能够提高程序的性能。Linux 下实现 I/O 多路复用的系统调用主要有 select、poll 和 epoll。**

### 几种常见的I/O模型

#### 阻塞等待

比如accept()和wait()就是阻塞等待的例子，阻塞等待客户端的连接和子进程的结束，优点就是阻塞在那里不吃CPU的时间片，缺点就是同一时刻只能处理一个操作，效率很低；所以想到用多进程或者多线程解决，但是缺点就是消耗资源

![image-20230821101046107](https://img-blog.csdnimg.cn/1c688cc1b8c2440d855581c4642f0a54.png)

##### BIO模型

这就是阻塞等待的例子，就是因为read()或者recv()是阻塞的，当我主程序接受客户端连接之后阻塞等待客户端的数据到达，这段时间内主程序没有办法接受其他客户端的连接，所以我们选择多进程或者多线程来解决问题，缺点也显而易见就是消耗资源，但是究其根本原因就是阻塞blocking的问题

![image-20230821101622911](https://img-blog.csdnimg.cn/2e88bc1613fe4da98a7f5e020f0efe38.png)

#### 非阻塞，忙轮询

因此我们可以选择就不阻塞了，忙着轮询来询问任务的情况，优点就是提高了程序的执行效率，但是缺点就是需要吃更多的CPU和系统资源

解决方案：使用I/O多路转接技术select/poll/epoll

![image-20230821102216612](https://img-blog.csdnimg.cn/dfbfb6da04e64f7185e71d1a033d0808.png)

##### NIO模型

这里把accept()和read()都设置成为非阻塞的，意味着我程序执行到这里的时候我不阻塞了，但是需要判断是否有客户端连接或者有数据到达，如果没有就继续循环直到有，有就进行相应的处理，但是这样消耗的CPU和资源代价非常大

![image-20230821102411708](https://img-blog.csdnimg.cn/51f830e8af8040bf9d82b1441a638404.png)

### I/O多路转接技术

**在NIO模型下，假设我们的用户非常多，因此每次执行到非阻塞的位置，比如read()我们都需要判断所有的客户是否有数据到达，我们的本意是通过非阻塞来提高程序的效率，但是我们现在每次到这里都要自己询问所有的客户数据是否到达，这不就消耗了CPU和资源嘛，违背了我们的初衷，I/O多路转接技术就是用来解决这个问题，他的目的是委托内核帮我们询问查看有多少客户的数据到达了，然后告诉我们，因此我们只需要调用一次就可以知道哪些客户数据到达了，大大提高了效率**

#### 简单理解

第一种 select/poll

![image-20230821102813978](https://img-blog.csdnimg.cn/ef95d11175684337bc47cf9fd72087f3.png)

第二种 epoll

![image-20230821102923936](https://img-blog.csdnimg.cn/b74bcb79a7974aff897882f972cb21ea.png)

#### select

主旨思想： 

1. **首先要构造一个关于文件描述符的列表，将要监听的文件描述符添加到该列表中。** 
2. **调用一个系统函数，监听该列表中的文件描述符，直到这些描述符中的一个或者多个进行 I/O 操作时，该函数才返回。 **
   **a.这个函数是阻塞的 **
   **b.函数对文件描述符的检测的操作是由内核完成的**
3. **在返回时，它会告诉进程有多少（哪些）描述符要进行I/O操作。**

~~~cpp
// sizeof(fd_set) = 128(个字节) 1024(个bit位)
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
    - 参数：
    	- nfds : 委托内核检测的最大文件描述符的值 + 1，传这个参数是是为了提高效率，没必要遍历最大文件描述符之后的，+1是底层实现的逻辑规定的要+1，我猜测可能类似于 for(int i = 0 ; i < nfds + 1 ;++i)，这样刚好最后一个能被遍历到
    	- readfds : 要检测的文件描述符的读的集合，委托内核检测哪些文件描述符的读的属性
    			 - 一般检测读操作
    			 - 对应的是对方发送过来的数据，因为读是被动的接收数据，检测的就是读缓冲区是否有数据，有的话就可以进行读取
    			 - 是一个传入传出参数
    	- writefds : 要检测的文件描述符的写的集合，委托内核检测哪些文件描述符的写的属性
            	  - 一般不检测写操作 
    			 - 委托内核检测写缓冲区是不是还可以写数据，没有满就可以继续向其中写入数据
    	- exceptfds : 检测发生异常的文件描述符的集合
    	- timeout : 设置的超时时间
        struct timeval {
            long tv_sec; /* seconds */
            long tv_usec; /* microseconds */
        };
            - NULL : 永久阻塞，直到检测到了文件描述符有变化，才会往下执行并且返回
            - tv_sec = 0 tv_usec = 0， 不阻塞
            - tv_sec > 0 tv_usec > 0， 阻塞对应的时间
            - 返回值 :
                  -1 : 失败
                  >0(n) : 检测的集合中有n个文件描述符发生了变化
// 将参数文件描述符fd对应的标志位设置为0
void FD_CLR(int fd, fd_set *set);
// 判断fd对应的标志位是0还是1， 返回值 ： fd对应的标志位的值，0，返回0， 1，返回1
int FD_ISSET(int fd, fd_set *set);
// 将参数文件描述符fd 对应的标志位，设置为1
void FD_SET(int fd, fd_set *set);
// fd_set一共有1024 bit, 全部初始化为0
void FD_ZERO(fd_set *set);
~~~

**在我们的例子当中，我们需要检测的是文件描述符中读的属性，因此我们就将 fd_set 类型中对应要检测的文件描述符的对应的标志位设为1表示我要检测，然后传给select()函数遍历，如果文件描述符为0则表示不用检测跳过，为1则委托内核去帮我们进行检测，如果确实有数据来了就将该标志位仍保持为1，没有则修改为0，最后把修改之后的 readfds 返回，就得到了有数据的集合，但是select()的返回值不会告诉我们哪些值发生了变化，只会告诉我们有几个，n个返回n，至于是那些需要我们自己遍历**

##### 工作过程分析

**在函数执行的过程中，系统先把用户区的这份文件描述符集合拷贝一份到内核当中，然后在内核当中检测标志位并且根据实际情况(比如这里就是哪些文件描述符的读端数据到达了)然后修改标志位，0就是没有，1就是有，然后从内核态重新拷贝到用户态，工作过程大致就是这样**

![image-20230821132937309](https://img-blog.csdnimg.cn/804d90a244f44ffebb08f73d08988b44.png)

##### 代码

~~~cpp
// Client_Info.h
#ifndef _CLIENT_INFO_
#define _CLIENT_INFO_

#include <arpa/inet.h>

#include <cstring>

#define MAX_IPV4_STRING 16

class Client_Info {
public:
    Client_Info() {
        __init__();
    };

    Client_Info& operator=(const Client_Info& _cli_info) {
        strcpy(this->client_ip, _cli_info.client_ip);
        this->client_port = _cli_info.client_port;

        return *this;
    }

    Client_Info(const char* _ip, const in_port_t& _port) {
        strcpy(this->client_ip, _ip);
        this->client_port = _port;
    }

    Client_Info(const Client_Info& _cli_info) {
        *this = _cli_info;
    }

    void __init__() {
        bzero(this->client_ip, sizeof(this->client_ip));
        this->client_port = 0;
    }

public:
    char client_ip[MAX_IPV4_STRING];
    in_port_t client_port;
};

#endif
~~~

以下是服务端和客户端

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#include "Client_Info.h"

#define MAXSIZE 1024
#define MAX_CLIENT_SIZE 1024

// 全局存放客户端连接的IP和端口
class Client_Info cli_infos[MAX_CLIENT_SIZE];

// 全局存放需要检测的文件描述符的数组
fd_set read_set;

int bigger(const int& val1, const int& val2) {
    return val1 > val2 ? val1 : val2;
}

void Communicate(const int& _connect_fd) {
    char* _client_ip = cli_infos[_connect_fd].client_ip;
    in_port_t& _client_port = cli_infos[_connect_fd].client_port;

    char buf[MAXSIZE] = {0};
    // 读
    bzero(buf, sizeof(buf));
    int len = read(_connect_fd, buf, sizeof(buf) - 1);
    if (-1 == len) {
        perror("read");
        exit(-1);
    }
    if (len > 0)
        printf("recv client (ip : %s , port : %d) : %s", _client_ip, _client_port, buf);
    else if (0 == len) {  // 客户端关闭
        printf("client ip : %s , port : %d has closed...\n", _client_ip, _client_port);
        // 这里关闭之后需要移除文件描述符集合中的标志位表示我不需要监听这个了
        FD_CLR(_connect_fd, &read_set);
        // 关闭文件描述符
        close(_connect_fd);
        return;
    }
    // 写
    write(_connect_fd, buf, strlen(buf));
}

int main() {
    // 1.创建socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listen_fd) {
        perror("socket");
        return -1;
    }

    // 设置一下端口复用
    int _optval = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &_optval, sizeof(_optval));

    // 2.绑定IP和端口
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // IP
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // 端口
    server_addr.sin_port = htons(9999);

    int ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    printf("server has initialized.\n");

    // 3.开始监听
    ret = listen(listen_fd, 8);
    if (-1 == ret) {
        perror("listen");
        return -1;
    }

    // 使用NIO模型，创建fd_set集合，存放的是需要检测的文件描述符
    // 全局定义 read_set
    // 初始化
    FD_ZERO(&read_set);
    // 添加需要检测的文件描述符
    FD_SET(listen_fd, &read_set);
	// 定义最大的文件描述符序号(参数里面要加1)
    int max_fd = listen_fd;

    // 这个地方我不能把read_set集合拿进去让内核进行拷贝修改然后覆盖我的这个
    // 我们设想这样一种情况，AB都检测，A发数据，B的被修改为0，但是下一次我肯定还要检测B的啊

    while (1) {
        fd_set tmp_set = read_set;
        // 调用select系统函数，让内核帮忙检测哪些文件描述符有数据
        // 这里是在检测listen_fd，因为如果有客户端请求连接了，那么这里listen_fd肯定会有数据进来
        ret = select(max_fd + 1, &tmp_set, nullptr, nullptr, nullptr);
        if (-1 == ret) {
            perror("select");
            return -1;
        } else if (0 == ret)
            // 为0表示超时并且没有检测到有改变的
            continue;  // 这里我们的设置因为是阻塞的，所以不会走到这里
        else if (ret > 0) {
            // 说明检测到了有文件描述符对应缓冲区的数据发生了改变
            if (FD_ISSET(listen_fd, &tmp_set)) {
                // 表示有新的客户端连接进来了
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int connect_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);

                if (-1 == connect_fd) {
                    perror("accept");
                    return -1;
                }

                // 获取客户端的信息
                char ip[MAX_IPV4_STRING] = {0};
                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip));

                in_port_t port = ntohs(client_addr.sin_port);

                // 打印信息
                printf("client ip : %s , port : %d has connected...\n", ip, port);

                // 将客户端的信息保存到全局数组中
                cli_infos[connect_fd] = Client_Info(ip, port);

                // 将新的文件描述符加入到集合中，这样select()就可以监听客户端的数据了
                FD_SET(connect_fd, &read_set);
                // 更新max_fd
                max_fd = bigger(connect_fd, max_fd);
            }

            // 看完监听的文件描述符，还要看其他的文件描述符标识位
            for (int i = listen_fd + 1; i < max_fd + 1; ++i) {
                if (FD_ISSET(i, &tmp_set))
                    // 表示有数据到来，进行通信，服务端只处理一次，然后又重新检测是否有数据，有数据则又走这段代码
                    // 并且如果服务端里面处理用循环处理，那么这个客户端一直抢占者服务端，其他服务端没办法发送数据
                    Communicate(i);
            }
        }
    }

    // 4.关闭连接
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

#define MAXSIZE 1024

int main() {
    // 1.创建套接字
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == connect_fd) {
        perror("socket");
        return -1;
    }

    // 2.建立连接
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // 端口
    server_addr.sin_port = htons(9999);
    // IP
    inet_pton(AF_INET, "127.0.0.2", &server_addr.sin_addr.s_addr);

    int ret = connect(connect_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("connect");
        return -1;
    }

    printf("connected successfully , waiting for communicating.\n");

    char buf[MAXSIZE] = {0};
    // 3.开始通信
    while (1) {
        // 写
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);

        // 增加退出功能
        if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "QUIT\n") == 0)
            goto END;

        write(connect_fd, buf, strlen(buf));
        printf("send : %s", buf);

        // 读
        bzero(buf, sizeof(buf));
        int len = read(connect_fd, buf, sizeof(buf) - 1);
        if (-1 == len) {
            perror("read");
            return -1;
        }
        if (len > 0)
            printf("recv : %s", buf);
        else if (0 == len) {  // 说明写端关闭，也就是服务端关闭
            printf("server has closed...\n");
            break;
        }
    }

END:
    // 4.关闭连接
    close(connect_fd);

    return 0;
}
~~~

##### 代码分析

好，现在我们来分析一下这段代码

首先我们使用的是，NIO模型，就是不阻塞，而是轮询，所以我们需要使用while循环来实现这个机制，然后在select()基础上我们要确认需要检测的文件描述符的读的状态，所以我们定义 fd_set read_set ，由于监听的listen_fd当有客户端连接的时候也是算有数据进入，对应read_set[]的标志位会改变，所以将其添加进去

~~~cpp
// 先初始化
FD_ZERO(&read_set);
// 添加需要检测的文件描述符
FD_SET(listen_fd, &read_set);
~~~

之后进入while循环我们检测是否有变化，有变化则说明有新客户端连接或者连接上的客户端有数据进入，这里我们设置阻塞等待变化，当然也可以设置一个等待的周期时间

注意返回值 ret 代表的是检测到变化的个数，-1表示错误，0表示没有，可以重开循环(但是我们这里不会，因为我们阻塞)；>0则表示有变化，我们可以进行后续处理

~~~cpp
ret = select(max_fd + 1, &tmp_set, nullptr, nullptr, nullptr);
~~~

可能是新客户端连接或者已连接的客户端发送数据，分别如下：

**新客户端连接**

~~~cpp
// 表示有新的客户端连接进来了
struct sockaddr_in client_addr;
socklen_t client_addr_len = sizeof(client_addr);
int connect_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);

if (-1 == connect_fd) {
    perror("accept");
    return -1;
}

// 获取客户端的信息
char ip[MAX_IPV4_STRING] = {0};
inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip));

in_port_t port = ntohs(client_addr.sin_port);

// 打印信息
printf("client ip : %s , port : %d has connected...\n", ip, port);

// 将客户端的信息保存到全局数组中
cli_infos[connect_fd] = Client_Info(ip, port);

// 将新的文件描述符加入到集合中，这样select()就可以监听客户端的数据了
FD_SET(connect_fd, &read_set);
// 更新max_fd
max_fd = bigger(connect_fd, max_fd);
~~~

我们不看上面打印信息的部分，看最后两句

- 我们将新的connect_fd添加到read_set当中，这样就可以检测了
- 我们更新的max_fd，这样可以提高效率

**已经连接上的客户端收到数据**

我们就从listen_fd开始遍历，因为listen_fd最开始创建，在普遍情况下是最小的，遍历到max_fd为止

~~~cpp
// 看完监听的文件描述符，还要看其他的文件描述符标识位
for (int i = listen_fd + 1; i < max_fd + 1; ++i) {
    if (FD_ISSET(i, &tmp_set))
        // 表示有数据到来，进行通信，服务端只处理一次，然后又重新检测是否有数据，有数据则又走这段代码
        // 并且如果服务端里面处理用循环处理，那么这个客户端一直抢占者服务端，其他服务端没办法发送数据
        Communicate(i);
}
~~~

接下来我们看通信函数

我们注意到一个细节，就是没有使用while循环，这是为什么呢？

因为如果服务端里面处理用循环处理，那么这个客户端一直抢占者服务端，其他服务端没办法发送数据；

并且我不用循环处理我把数据读了就结束函数，然后又重新开始检测，代码里移除标志位并且关闭文件描述符是在写端关闭的时候，这时候也是合情合理的

~~~cpp
void Communicate(const int& _connect_fd) {
    char* _client_ip = cli_infos[_connect_fd].client_ip;
    in_port_t& _client_port = cli_infos[_connect_fd].client_port;

    char buf[MAXSIZE] = {0};
    // 读
    bzero(buf, sizeof(buf));
    int len = read(_connect_fd, buf, sizeof(buf) - 1);
    if (-1 == len) {
        perror("read");
        exit(-1);
    }
    if (len > 0)
        printf("recv client (ip : %s , port : %d) : %s", _client_ip, _client_port, buf);
    else if (0 == len) {  // 客户端关闭
        printf("client ip : %s , port : %d has closed...\n", _client_ip, _client_port);
        // 这里关闭之后需要移除文件描述符集合中的标志位表示我不需要监听这个了
        FD_CLR(_connect_fd, &read_set);
        // 关闭文件描述符
        close(_connect_fd);
        return;
    }
    // 写
    write(_connect_fd, buf, strlen(buf));
}
~~~

我们的代码中还有一个细节

就是在这里为什么要用tmp_set，有的地方是read_set，有的地方是tmp_set

这个地方我不能把read_set集合拿进去让内核进行拷贝修改然后覆盖我的这个；

我们设想这样一种情况，AB都检测，A发数据，B的被修改为0，但是下一次我肯定还要检测B的啊，这就出现问题了

所以我们想到的解决方案就是使用临时变量，但是像新客户端连接，写端关闭的时候删除文件描述符的检测这些还是要操作read_set，也很好理解

<img src="https://img-blog.csdnimg.cn/91c795f91f9842abaae9ff0ec3e2552a.png" alt="image-20230821162010058" style="zoom: 80%;" />

#### poll

**poll技术是对select技术进行改进，所以select技术肯定具有缺点**

##### select技术的缺点

当客户端多了的时候，也就是fd多了的时候，就会出现如下的一系列问题

**其中的第四条就是不使用临时 tmp_set 的问题，read_set应该要继续检测的部分被置为0了，就因为这个时候没有数据进来，所以言下之意就是不能重用，每次都需要重置**

![image-20230821162714320](https://img-blog.csdnimg.cn/a90dd7c747d64f47850f7c74b17ce789.png)

##### poll()

**使用时引头文件 <poll.h>**

~~~cpp
#include <poll.h>
struct pollfd {
    int fd; /* 委托内核检测的文件描述符 */
    short events; /* 委托内核检测文件描述符的什么事件 */
    short revents; /* 文件描述符实际发生的事件 */
};

struct pollfd myfd;
myfd.fd = 5;
myfd.events = POLLIN | POLLOUT;

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
    - 参数：
        - fds : 是一个struct pollfd 结构体数组，这是一个需要检测的文件描述符的集合
        - nfds : 这个是第一个参数数组中最后一个有效元素的下标 + 1
        - timeout : 阻塞时长
            0 : 不阻塞
            -1 : 阻塞，当检测到需要检测的文件描述符有变化，解除阻塞
            >0 : 阻塞的时长，单位是毫秒
    - 返回值：
        -1 : 失败
        >0（n） : 成功,n表示检测到集合中有n个文件描述符发生变化
~~~

![image-20230821095539250](https://img-blog.csdnimg.cn/aff558d48a9347b2811ae03f6451333f.png)

##### 代码

代码的架构和前面的几乎没有区别，只有server.cpp进行了修改，这里只放出server.cpp

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#include "Client_Info.h"

#define MAXSIZE 1024
#define MAX_CLIENT_SIZE 1024
#define MAX_POLLFD_SIZE 1025

// 全局存放客户端连接的IP和端口
class Client_Info cli_infos[MAX_CLIENT_SIZE];

// 全局存放需要检测的文件描述符数组
struct pollfd fds[MAX_POLLFD_SIZE];

int bigger(const int& val1, const int& val2) {
    return val1 > val2 ? val1 : val2;
}

void Communicate(const int& _index) {
    int _connect_fd = fds[_index].fd;

    char* _client_ip = cli_infos[_connect_fd].client_ip;
    in_port_t& _client_port = cli_infos[_connect_fd].client_port;

    char buf[MAXSIZE] = {0};
    // 读
    bzero(buf, sizeof(buf));
    int len = read(_connect_fd, buf, sizeof(buf) - 1);
    if (-1 == len) {
        perror("read");
        exit(-1);
    }
    if (len > 0)
        printf("recv client (ip : %s , port : %d) : %s", _client_ip, _client_port, buf);
    else if (0 == len) {  // 客户端关闭
        printf("client ip : %s , port : %d has closed...\n", _client_ip, _client_port);
        // 关闭文件描述符
        close(_connect_fd);
        // 将对应的文件描述符置为-1
        fds[_index].fd = -1;
        return;
    }
    // 写
    write(_connect_fd, buf, strlen(buf));
}

int main() {
    // 1.创建socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listen_fd) {
        perror("socket");
        return -1;
    }

    // 设置一下端口复用
    int _optval = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &_optval, sizeof(_optval));

    // 2.绑定IP和端口
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // IP
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // 端口
    server_addr.sin_port = htons(9999);

    int ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    printf("server has initialized.\n");

    // 3.开始监听
    ret = listen(listen_fd, 8);
    if (-1 == ret) {
        perror("listen");
        return -1;
    }

    // 使用NIO模型，使用poll解决问题
    // 初始化检测的文件描述符数组
    for (int i = 0; i < MAX_POLLFD_SIZE; ++i) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;  // 表示一会儿要去检测读事件
    }
    // 加入监听的文件描述符
    fds[0].fd = listen_fd;

    // 定义最大的文件描述符的fds[]数组的索引
    int nfds = 0;

    while (1) {
        // 调用poll()函数，这是select()函数的改进版本
        ret = poll(fds, nfds + 1, -1);
        if (-1 == ret) {
            perror("select");
            return -1;
        } else if (0 == ret)
            // 为0表示超时并且没有检测到有改变的
            continue;  // 这里我们的设置因为是阻塞的，所以不会走到这里
        else if (ret > 0) {
            // 说明检测到了有文件描述符对应缓冲区的数据发生了改变
            if (fds[0].revents & POLLIN == POLLIN) {
                // 表示有新的客户端连接进来了
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int connect_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);

                if (-1 == connect_fd) {
                    perror("accept");
                    return -1;
                }

                // 获取客户端的信息
                char ip[MAX_IPV4_STRING] = {0};
                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip));

                in_port_t port = ntohs(client_addr.sin_port);

                // 打印信息
                printf("client ip : %s , port : %d has connected...\n", ip, port);

                // 将客户端的信息保存到全局数组中
                cli_infos[connect_fd] = Client_Info(ip, port);

                // 将新的文件描述符加入到事件中，注意文件描述符的优先用小的机制
                for (int i = 1; i < MAX_POLLFD_SIZE; ++i)
                    if (fds[i].fd == -1) {
                        fds[i].fd = connect_fd;
                        fds[i].events = POLLIN;
                        // 更新nfds
                        nfds = bigger(nfds, i);
                        break;
                    }
            }

            // 看完监听的文件描述符，看其他的文件描述符是否收到数据
            for (int i = 1; i < nfds + 1; ++i) {
                if (fds[i].revents & POLLIN == POLLIN)
                    Communicate(i);
            }
        }
    }

    // 4.关闭连接
    close(listen_fd);

    return 0;
}
~~~

##### 代码分析

首先我们要理解结构体 pollfd 的含义

**这是用来保存委托内核检测的文件描述符；委托内核检测的文件描述符的什么事件，比如读写，类似于select中的read_set；还有检测过后实际发生的事件，比如没有读，就修改，类似于select中的 tmp_set；的一个结构体**

~~~cpp
struct pollfd {
    int fd; /* 委托内核检测的文件描述符 */
    short events; /* 委托内核检测文件描述符的什么事件 */
    short revents; /* 文件描述符实际发生的事件 */
};
~~~

值得注意的是这些事件的类型和存储方法，是short类型的，我们来看它可以描述哪些事件

![image-20230821095539250](https://img-blog.csdnimg.cn/aff558d48a9347b2811ae03f6451333f.png)

**其实他和文件属性stat变量里面st_mode(表示文件类型和权限)是一个道理，一个bit位表示一个权限，1表示有，0表示没有，因此添加权限应该用 按位或 | ， 这里的事件也是一样的道理，我们一般判断读事件就POLLIN，写事件就POLLOUT**

**第三个参数就是经过检测之后的状态，可以用它来判断是否有检测到读；由于我们设置的event没有变化，所以相对于select()还是好了很多**

**其次，我们查看poll()接口的第一个参数是： struct pollfd *fds，需要一个结构体的数组传入进来，每一个元素就封装了一个文件描述符对应的信息，我们从0开始依次记录，如果该元素的fd为-1就表示没有使用，可以存放新的元素，注意这个下标，或者我们称他为索引，索引的值和文件描述符的值是不同的，为了提高效率我们这么设计，在代码中一定要注意，其他的逻辑没什么区别**

还有一点，我们看如何判断最后的 revents 检测到读信息

**还是前面的思想，每一位对应一个，读对应一位为1，其他为0；当然为什么不是直接相等呢？可能我们设置了其他性质也需要检测，内核处理后还是有了其他的性质为1，我们最好不要冒险，所以这里我们用 & **

~~~cpp
if (fds[i].revents & POLLIN == POLLIN)
    //下面的操作
~~~

#### epoll

**epoll和前面两种技术不同，epoll技术直接在内核态当中进行操作，完全省去了用户态到内核态拷贝的过程，并且由内核通知用户，实现了内核和用户的并发操作，提高了效率。**

##### 工作过程分析

select技术和poll技术虽然实现方式有所不同，poll技术是select技术的改进，但是他们在实际操作的时候都是先在用户区生成一个表，select就是文件描述符表，对应位置置为1，下标表示为文件描述符；poll技术是用事件表示的，并且定义了我们想要的检测事件和实际发生的事件供我们比对，比如我们想要检测读事件，他返回0则表示没发生，两个都定义出来就免去了我们用临时变量的麻烦；好，这两个都是先在用户区然后拷贝到内核区然后再回来，众所周知，从内核区到用户区的二者切换要消耗CPU资源，所以一旦文件描述符多了，检测的事件多了就会影响性能

**所以才有了epoll的技术，调用epoll之后会直接在内核区生成检测事件的东西，系统会提供给我们epoll的一系列API来帮助我们操作内核中的这块区域，从图中我们可以看出定义为 eventpoll ，里面比较重要的存着 rbr 和 rdlist，前者的数据结构是红黑树，是我们想要检测的事件体，红黑树使得查询和遍历非常快；后者的数据结构是双链表，是检测事件发生变化，在网络编程中就是有读的数据进来或者新客户端连接，用双链表可以直接了当的遍历出来**

**总结一下，二者的区别一是epoll省去了从用户态到内核态相互的切换，直接在内核态操作，效率更高；二是epoll不仅返回了检测到了多少个，而且还直接通过双链表的形式告诉我们哪几个返回了，而select和poll技术并未做到这一点，select技术修改了我们传给他的表，返回值是检测到了多少个，但是哪些变化了需要我们遍历这个以文件描述符为下标的表；poll技术给每个需要检测的文件描述符封装了一个结构体，并且保存了需要检测的事件和实际发生的事件，但是仍然需要我们去遍历poll()参数中的pollfd结构体数组才行；但是epoll却将检测到的事件封装在双链表 rdlist 中，由此可见这就是他的优势**

![image-20230821194458779](https://img-blog.csdnimg.cn/0dd32b37de6842c6ac965b85fa345e07.png)

##### epoll() 

**epoll创建在内核区的东西操作是通过API返回的文件描述符操作的，这也是和select和poll技术不同的地方，select技术用 fd_set 变量(本质是个数组)，poll技术用 pollfd 结构体数组，这些东西在代码中都是在用户区的；而epoll用文件描述符托管也代表内核区的性质**

~~~cpp
#include <sys/epoll.h>
// 创建一个新的epoll实例。在内核中创建了一个数据，这个数据中有两个比较重要的数据，一个是需要检测的文件描述符的信息（红黑树），还有一个是就绪列表，存放检测到数据发送改变的文件描述符信息（双向链表）。
int epoll_create(int size);
    - 参数：
    	size : 目前没有意义了。随便写一个数，必须大于0
    - 返回值：
        -1 : 失败
        > 0 : 文件描述符，操作epoll实例的
            
// 结构类型是联合union，我们一般使用 fd 参数就行了
typedef union epoll_data {
    void *ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

struct epoll_event {
    uint32_t events; /* Epoll events */
    epoll_data_t data; /* User data variable */
};
常见的Epoll检测事件events：
    - EPOLLIN
    - EPOLLOUT
    - EPOLLERR

// 对epoll实例进行管理：添加文件描述符信息，删除信息，修改信息
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    - 参数：
        - epfd : epoll实例对应的文件描述符
        - op : 要进行什么操作
            EPOLL_CTL_ADD: 添加
            EPOLL_CTL_MOD: 修改
            EPOLL_CTL_DEL: 删除
        - fd : 要检测的文件描述符
        - event : 检测文件描述符什么事情
            
// 检测函数
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    - 参数：
        - epfd : epoll实例对应的文件描述符
        - events : 传出参数，保存了发送了变化的文件描述符的信息，是一个结构体数组
        - maxevents : 第二个参数结构体数组的大小
        - timeout : 阻塞时间
              0 : 不阻塞
              -1 : 阻塞，直到检测到fd数据发生变化，解除阻塞
              > 0 : 阻塞的时长（毫秒）
        - 返回值：
            - 成功，返回发送变化的文件描述符的个数 > 0
            - 失败 -1
~~~

##### 代码和分析

还是只放server.cpp

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "Client_Info.h"

#define MAX_CLIENT_SIZE 1024
#define MAX_BUF_SIZE 1024

// 全局存放客户端连接的IP和端口
class Client_Info cli_infos[MAX_CLIENT_SIZE];

void Communicate(const struct epoll_event &_ret_event, const int &_epoll_fd) {
    int _connect_fd = _ret_event.data.fd;
    // 读
    char buf[MAX_BUF_SIZE] = {0};
    int len = read(_connect_fd, buf, sizeof(buf) - 1);
    if (-1 == len) {
        perror("read");
        exit(-1);
    }

    if (len > 0)
        printf("client (ip : %s , port : %d) recv : %s", cli_infos[_connect_fd].client_ip, cli_infos[_connect_fd].client_port, buf);
    else if (0 == len) {
        // 写端，客户端关闭连接
        printf("client (ip : %s , port : %d) has closed...\n", cli_infos[_connect_fd].client_ip, cli_infos[_connect_fd].client_port);
        // 从检测事件中删除他
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _connect_fd, nullptr);
        // 关闭文件描述符
        close(_connect_fd);

        return;
    }
    // 写
    write(_connect_fd, buf, strlen(buf));
}

int main() {
    // 1.创建socket套接字
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listen_fd) {
        perror("socket");
        return -1;
    }

    // 设置端口复用
    int _optval = 1;
    int ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &_optval, sizeof(_optval));
    if (-1 == ret) {
        perror("setsockopt");
        return -1;
    }

    // 2.绑定IP和端口
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // IP
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // 端口
    server_addr.sin_port = htons(9999);

    ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    // 3.监听端口
    ret = listen(listen_fd, 8);
    if (-1 == ret) {
        perror("listen");
        return -1;
    }

    printf("server has initialized.\n");

    // 4.用epoll技术实现接受客户端和进行通信
    // 创建epoll示例
    int epoll_fd = epoll_create(1);
    if (-1 == epoll_fd) {
        perror("epoll_create");
        return -1;
    }

    // 将监听套接字添加进入检测中
    struct epoll_event listen_event;
    listen_event.events = EPOLLIN;     // 检测读
    listen_event.data.fd = listen_fd;  // 文件描述符

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &listen_event);

    int _maxevents = MAX_CLIENT_SIZE;

    // 开始检测
    while (1) {
        // 这个结构体数组存放了检测到的文件描述符的信息，保存在这里面
        // 内核中是把双链表中的数据写入到这里
        struct epoll_event ret_events[_maxevents];

        // 返回值是表示有多少个被检测到了；第三个参数可以一般放数组的最大容量
        int ret = epoll_wait(epoll_fd, ret_events, _maxevents, -1);
        if (-1 == ret) {
            perror("epoll_wait");
            return -1;
        }

        // 检测到了，开始处理
        for (int i = 0; i < ret; ++i) {
            if (ret_events[i].events && EPOLLIN == EPOLLIN) {
                if (ret_events[i].data.fd == listen_fd) {
                    // 表示有新客户端连接
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);

                    int connect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                    if (-1 == connect_fd) {
                        perror("accept");
                        return -1;
                    }

                    // 设置read非阻塞
                    int flag = fcntl(connect_fd, F_GETFL);
                    flag |= O_NONBLOCK;
                    fcntl(connect_fd, F_SETFL, flag);
                    
                    // 将客户端信息存入结构体数组，下标用connect_fd代替
                    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, cli_infos[connect_fd].client_ip, sizeof(cli_infos[connect_fd].client_ip));
                    cli_infos[connect_fd].client_port = ntohs(client_addr.sin_port);

                    printf("client (ip : %s , port : %d) has connected...\n", cli_infos[connect_fd].client_ip, cli_infos[connect_fd].client_port);

                    // 添加到检测中
                    struct epoll_event connect_event;
                    connect_event.data.fd = connect_fd;
                    connect_event.events = EPOLLIN;

                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect_fd, &connect_event);
                } else  // 客户端接收到数据
                    Communicate(ret_events[i], epoll_fd);
            }
        }
    }

    // 5.关闭连接
    close(epoll_fd);
    close(listen_fd);

    return 0;
}
~~~

这段代码的思路和前面的基本没有区别，就是先创建epoll_create()的示例，由于内核区的数据用文件描述符操作

~~~cpp
int epoll_fd = epoll_create(1);
~~~

然后将监听套接字加入到检测当中

~~~cpp
// 将监听套接字添加进入检测中
struct epoll_event listen_event;
listen_event.events = EPOLLIN;     // 检测读
listen_event.data.fd = listen_fd;  // 文件描述符

epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &listen_event);
~~~

然后不断循环，调用epoll_wait()接口检测哪些发生了变化

我们来重点关注下这个接口的参数，第一个参数是epoll示例的文件描述符epoll_fd，第二个参数是保存检测到发生变化的结构体数组，类型是epoll_event，第三个参数是这个结构体数组的最大容量，可以自己设定，因为一般放不满，他也是从头开始放所以我们给你一个最大的值 _maxevents 就行，第四个参数是阻塞时间，这里设置-1表示阻塞

~~~cpp
while (1) {
    // 这个结构体数组存放了检测到的文件描述符的信息，保存在这里面
    // 内核中是把双链表中的数据写入到这里
    struct epoll_event ret_events[_maxevents];

    // 返回值是表示有多少个被检测到了；第三个参数可以一般放数组的最大容量
    int ret = epoll_wait(epoll_fd, ret_events, _maxevents, -1);
    if (-1 == ret) {
        perror("epoll_wait");
        return -1;
    }
    
    //后续代码
    ...
}
~~~

之后就是分新客户端连接和已连接客户端发送数据了

从0遍历到epoll_wait()接口的返回值，因为返回的是检测到的个数，刚好告诉我了我就用

~~~cpp
for (int i = 0; i < ret; ++i) {
    if (ret_events[i].events && EPOLLIN == EPOLLIN) {
        if (ret_events[i].data.fd == listen_fd) {
            // 表示有新客户端连接
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);

            int connect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            if (-1 == connect_fd) {
                perror("accept");
                return -1;
            }

            // 将客户端信息存入结构体数组，下标用connect_fd代替
            inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, cli_infos[connect_fd].client_ip, sizeof(cli_infos[connect_fd].client_ip));
            cli_infos[connect_fd].client_port = ntohs(client_addr.sin_port);

            printf("client (ip : %s , port : %d) has connected...\n", cli_infos[connect_fd].client_ip, cli_infos[connect_fd].client_port);

            // 添加到检测中
            struct epoll_event connect_event;
            connect_event.data.fd = connect_fd;
            connect_event.events = EPOLLIN;

            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect_fd, &connect_event);
        } else  // 客户端接收到数据
            Communicate(ret_events[i], epoll_fd);
    }
}
~~~

communicate()接口和之前的没什么区别，就针对修改了一点，注意还是不是循环，因为出去了我们走下一次检测也是一样的

~~~cpp
void Communicate(const struct epoll_event &_ret_event, const int &_epoll_fd) {
    int _connect_fd = _ret_event.data.fd;
    // 读
    char buf[MAX_BUF_SIZE] = {0};
    int len = read(_connect_fd, buf, sizeof(buf) - 1);
    if (-1 == len) {
        perror("read");
        exit(-1);
    }

    if (len > 0)
        printf("client (ip : %s , port : %d) recv : %s", cli_infos[_connect_fd].client_ip, cli_infos[_connect_fd].client_port, buf);
    else if (0 == len) {
        // 写端，客户端关闭连接
        printf("client (ip : %s , port : %d) has closed...\n", cli_infos[_connect_fd].client_ip, cli_infos[_connect_fd].client_port);
        // 从检测事件中删除他
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _connect_fd, nullptr);
        // 关闭文件描述符
        close(_connect_fd);

        return;
    }
    // 写
    write(_connect_fd, buf, strlen(buf));
}
~~~

##### Epoll 的工作模式

- > **LT 模式 （水平触发）** 
  > 假设委托内核检测读事件 -> 检测fd的读缓冲区 
  > 读缓冲区有数据 - > epoll检测到了会给用户通知 
  > 	a.用户不读数据，数据一直在缓冲区，epoll 会一直通知 
  > 	b.用户只读了一部分数据，epoll会通知 
  > 	c.缓冲区的数据读完了，不通知
  >
  > **LT（level - triggered）是缺省的工作方式，并且同时支持 block 和 no-block socket。**在这 种做法中，内核告诉你一个文件描述符是否就绪了，然后你可以对这个就绪的 fd 进行 IO 操作。**如果你不作任何操作，内核还是会继续通知你的。**

- > **ET 模式（边沿触发）** 
  > 假设委托内核检测读事件 -> 检测fd的读缓冲区 
  > 读缓冲区有数据 - > epoll检测到了会给用户通知
  > a.用户不读数据，数据一致在缓冲区中，epoll下次检测的时候就不通知了 
  > b.用户只读了一部分数据，epoll不通知
  > c.缓冲区的数据读完了，不通知
  >
  > **ET（edge - triggered）是高速工作方式，只支持 no-block socket。**在这种模式下，当描述符从未就绪变为就绪时，内核通过epoll告诉你。然后它会假设你知道文件描述符已经就绪， 并且不会再为那个文件描述符发送更多的就绪通知，直到你做了某些操作导致那个文件描述 符不再为就绪状态了。**但是请注意，如果一直不对这个 fd 作 IO 操作（从而导致它再次变成 未就绪），内核不会发送更多的通知（only once）**。 **ET 模式在很大程度上减少了 epoll 事件被重复触发的次数，因此效率要比 LT 模式高。epoll 工作在 ET 模式的时候，必须使用非阻塞套接口，以避免由于一个文件句柄的阻塞读/阻塞写操作把处理多个文件描述符的任务饿死。**

~~~cpp
struct epoll_event {
    uint32_t events; /* Epoll events */
    epoll_data_t data; /* User data variable */
};
常见的Epoll检测事件：
    - EPOLLIN
    - EPOLLOUT
    - EPOLLERR
    - EPOLLET //设置边沿触发，Epoll技术默认的是水平触发，也就是在读完之前一直通知
~~~

##### LT模式(水平触发)

**LT模式是检测到有数据，如果我们用户不读或者没有读完，那么下一次仍旧会通知，也就是检测到，直到缓冲区的数据读完了之后才停止通知**

在这里我们把缓冲区的数组大小弄小点

~~~cpp
// 我将一次读取的大小弄小点
#define MAX_BUF_SIZE 5
~~~

然后故意多写点数据来看看输出结果：

客户端

![image-20230822113418607](https://img-blog.csdnimg.cn/c6d045587bd6415e869b394b7049029e.png)

服务端

![image-20230822113459363](https://img-blog.csdnimg.cn/43747cf17d2c41e19fd09e6956a5af13.png)

**我们明显可以看出，缓冲区调小之后，一次读不完，然后循环之后仍然能够检测得到，直到将其全部读完**

但是客户端为什么第二次有一部分数据留在缓冲区中没输出出来我就不知道了(这个真不知道)

##### ET模式(边沿触发)

**LT模式循环每次都会被内核提醒，这样的重复提醒对资源还是有很多的浪费的，所以ET模式假设我们已经知道这个提醒了，并且放在心上，马上就去处理它，所以后续内核不会提醒，这就是区别，也是提升效率的关键。因此如何读取到正确完整的数据就成了我们的关键**

首先我们给通信的文件描述符 connect_fd 设置ET属性，也就是加上 EPOLLET 宏

~~~cpp
// 添加到检测中
struct epoll_event connect_event;
connect_event.data.fd = connect_fd;
connect_event.events = EPOLLIN | EPOLLET;  // 设置边沿触发，结合非阻塞的API使用!!!
~~~

我们的通信函数保持不变，也就是没有循环操作，并且read()函数阻塞，结果如下：

客户端

![image-20230822114219505](https://img-blog.csdnimg.cn/9da81dffc21e43969da760a8a7097026.png)

服务端

![image-20230822114228554](https://img-blog.csdnimg.cn/a1332276ea664b16bce8704c67c085f8.png)

可以看出只读取了一次，后续就没有下文了，只有当我第二次手动让客户端检测，比如这里我输入字符，服务端才会继续收到数据，注意上次通信没读完，数据还在缓冲区中，是接着缓冲区读的，如下：

客户端

![image-20230822114407108](https://img-blog.csdnimg.cn/08ee9bce838546ee9dd428ffe8a4c6bc.png)

服务端

![image-20230822114413347](https://img-blog.csdnimg.cn/11482b8639a24b5699b19ec887696cf1.png)

好，既然想到了读不完，那么我们就需要循环读，但是循环读如果配上阻塞的read()函数，读完了就会阻塞在那里，程序就尬住了，其他工作没办法做，这也是前面提到的需要使用非阻塞non-block的API，因此这里我们需要设置read()为非阻塞

设置read()为非阻塞需要依赖于他的文件描述符，而关于文件描述符有fcntl()函数可以获取或者设置信息

**注意看到这些什么属性或者标志位为int short这种数字类型，第一反应想到用二进制01表示，因为这样最省空间**

~~~cpp
// 设置非阻塞，否则读完就阻塞在这里，read非阻塞通过文件描述符操作
int _flag = fcntl(connect_fd, F_GETFL);
_flag |= O_NONBLOCK;  // 不能把原来的属性设置没了所以先获得
fcntl(connect_fd, F_SETFL, _flag);
~~~

紧接着我们也需要修改我们的通信函数

其他地方基本没什么变化，要注意一点：

**当我缓冲区的数据读完了，但是写端没关闭怎么办？因为我们知道写端关闭了返回0，那这里返回什么呢？**

**我们查看了man文档知道返回-1，并且errno会被设置为EAGAIN，这就是非阻塞情况下read()函数数据读完了的返回**

**和之前accept()函数在被软中断，信号处理回收子进程之后变成非阻塞，返回-1，设置errno为EINTR有点类似**

~~~cpp
void Communicate(const struct epoll_event &_ret_event, const int &_epoll_fd) {
    int _connect_fd = _ret_event.data.fd;

    // ET工作模式不会通知第二次，只有再次变化的时候才会检测到，因此我们需要调用非阻塞的接口把数据读完
    char buf[MAX_BUF_SIZE] = {0};
    while (1) {
        int len = read(_connect_fd, buf, sizeof(buf) - 1);

        if (-1 == len) {
            // 里面有一种情况就是我写端没有关闭但是我在非阻塞的情况下已经把数据读完了，这个时候就会产生EAGAIN的错误
            if (errno == EAGAIN) {
                printf("read data over.\n");
                return;
            }

            perror("read");
            exit(-1);
        }

        // 读到正确数据
        if (len > 0) {
            printf("client (ip : %s , port : %d) recv : %s\n", cli_infos[_connect_fd].client_ip, cli_infos[_connect_fd].client_port, buf);
            write(_connect_fd, buf, strlen(buf));
            bzero(buf, sizeof(buf));
        }

        else if (0 == len) {
            // 写端，客户端关闭连接
            printf("client (ip : %s , port : %d) has closed...\n", cli_infos[_connect_fd].client_ip, cli_infos[_connect_fd].client_port);
            // 从检测事件中删除他
            epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _connect_fd, nullptr);
            // 关闭文件描述符
            close(_connect_fd);

            return;
        }
    }
}
~~~

最终我们的程序运行结果就是这样：

客户端

![image-20230822115230225](https://img-blog.csdnimg.cn/af84c3353ab840b2b9062d6a2a300f89.png)

服务端

可以看出我做的特殊判断也被打印出来了

![image-20230822115243021](https://img-blog.csdnimg.cn/008fff4f7de64bf285b2b316206e2264.png)

这个程序应该还有小bug，但是大体逻辑是没有问题的，这个我就尚不知道了

## UDP

### UDP通信

**相比于TCP，UDP的通信就非常简单了，TCP的服务端需要创建监听的套接字用于监听建立连接，客户端需要使用connect()和服务端建立连接；而UDP创建了用于通信的文件描述符后直接通信即可，注意服务端还是要绑定bind()IP和端口**

![image-20230822145319237](https://img-blog.csdnimg.cn/29df015808b741549fc4f63009d33f43.png)

**在UDP中系统专门给我们提供了接口叫 sendto() 和 recvfrom() ；同样的在TCP当中，我们之前一直使用的是read()和write()来操作通信的文件描述符，没问题，但是系统当然也提供了专门的API，叫 send() 和 recv()**

~~~cpp
#include <sys/types.h>
#include <sys/socket.h>

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
    - 参数：
        - sockfd : 通信的fd
        - buf : 要发送的数据
        - len : 发送数据的长度
        - flags : 0，标志，没有什么用，我们设置为0就可以了
        - dest_addr : 通信的另外一端的地址信息，需要指定，因为没有建立连接不给不知道给谁发
        - addrlen : 地址的内存大小
            
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, 
                 struct sockaddr *src_addr, socklen_t *addrlen);
    - 参数：
        - sockfd : 通信的fd
        - buf : 接收数据的数组
        - len : 数组的大小
        - flags : 0
        - src_addr : 用来保存另外一端的地址信息，不需要可以指定为NULL
        - addrlen : 地址的内存大小
~~~

#### 代码

大体框架没有变，注意UDP中没有建立连接这个概念

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024
#define MAX_IPV4_STRING 16

int main() {
    // 1.创建通信的socket套接字
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == socket_fd) {
        perror("socket");
        return -1;
    }

    // 2.绑定IP和端口
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // IP
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // 端口
    server_addr.sin_port = htons(9999);

    int ret = bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == socket_fd) {
        perror("bind");
        return -1;
    }

    printf("server has initialized.\n");

    char buf[MAX_BUF_SIZE] = {0};

    // 3.开始通信
    while (1) {
        bzero(buf, sizeof(buf));
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        // 读
        int len = recvfrom(socket_fd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (-1 == len) {
            perror("recvfrom");
            return -1;
        }
        // 获得客户端信息
        char client_ip[MAX_IPV4_STRING] = {0};
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
        in_port_t client_port = ntohs(client_addr.sin_port);

        // recvfrom返回0是可以接受的，不像read返回0表示对端关闭连接。因为UDP是无连接的，也就没有所谓的关闭。
        printf("recv client (ip : %s , port : %d) : %s", client_ip, client_port, buf);

        // 写
        sendto(socket_fd, buf, strlen(buf), 0, (struct sockaddr*)&client_addr, client_addr_len);
    }

    // 4.关闭套接字
    close(socket_fd);

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

#define MAX_BUF_SIZE 1024
#define MAX_IPV4_STRING 16

int main() {
    // 1.创建通信的socket套接字
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == socket_fd) {
        perror("socket");
        return -1;
    }

    // 存储服务端地址信息
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // IP
    inet_pton(AF_INET, "127.0.0.2", &server_addr.sin_addr.s_addr);
    // 端口
    server_addr.sin_port = htons(9999);

    char buf[MAX_BUF_SIZE] = {0};

    static int num = 0;

    // 2.开始通信
    while (1) {
        // 写
        bzero(buf, sizeof(buf));
        sprintf(buf, "hello i am client , %d\n", num++);
        printf("send : %s", buf);
        sendto(socket_fd, buf, strlen(buf), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

        sleep(1);

        // 读
        bzero(buf, sizeof(buf));

        int len = recvfrom(socket_fd, buf, sizeof(buf) - 1, 0, nullptr, nullptr);
        if (-1 == len) {
            perror("recvfrom");
            return -1;
        }
        // recvfrom返回0是可以接受的，不像read返回0表示对端关闭连接。因为UDP是无连接的，也就没有所谓的关闭。
        printf("recv : %s", buf);
    }

    // 4.关闭套接字
    close(socket_fd);

    return 0;
}
~~~

##### 注意

- **recvfrom() 函数的参数，后面两项是可以选择接受对方的信息，可以获得对方的socket地址信息，不要就传nullptr就可以了**

- **sendto() 函数的参数，最后两项也是对方的信息，这是必须要给的，因为TCP没有建立连接同于通信的文件描述符，所以必须要给出对方的信息才可能正确发到；由此我们也可以推出我们的这个 sockfd 可以和很多客户端连接，因此UDP不用多进程或者多线程也可以实现，结果类似如下：**

  ![image-20230822154224472](https://img-blog.csdnimg.cn/f3b47e5abcf948a18ec5457436fc5409.png)

- **recvfrom 返回0是可以接受的，不像read返回0表示对端关闭连接。因为UDP是无连接的，也就没有所谓的关闭。我们的程序在另一方断开之后会卡住，这里我尚不知道为什么，应该是recvfrom() 内核里面的设计了；我们总之知道 UDP 提供的 recvfrom() 函数返回0是合法的**

- 另外，我想谈谈关于TCP和UDP双方的文件描述符，TCP里面就是connect_fd，UDP里面就是socket_fd
  我们通过程序查看是否相同，首先是TCP：

  服务端
  ![image-20230822155038817](https://img-blog.csdnimg.cn/08367d8edb32437fa7963c9269609071.png)

  客户端
  ![image-20230822155057787](https://img-blog.csdnimg.cn/28fc438c4b844619978db40f3394aee4.png)

  **他们是不相同的，如何理解？**
  **这是两个进程，TCP的3号文件描述符用在了监听，4号用来和客户端进行通信，客户端也具有自己的文件描述符表，用的自然就是3号文件描述符**

  然后是UDP：

  服务端
  ![image-20230822155443323](https://img-blog.csdnimg.cn/fbf4e6fd55b5445cba518665e9f1a727.png)

  客户端
  ![image-20230822155501635](https://img-blog.csdnimg.cn/d4248c3dd7d143bf9ed0fccae06c8811.png)

  **两个进程都只建立了一个文件描述符，所以当然各自进程都用自己最小可用的文件描述符就是3啊，这个跟文件描述符引用计数没关系，前提是需要是同一个进程，并且socket套接字指向的东西还是一样的**

### 广播和组播

**广播和多播就是发送方向多个接收方的主机发送消息，也就是一对多，广播是给所有的主机发送消息，只能用在局域网中；多播是给一个多播组中的所有主机发送消息，既可以用于广域网，也可以用于局域网；由于都是一对多，所以TCP的端对端的单播协议明显不适用，而只能用无连接不可靠的UDP协议**

#### 广播

向子网中多台计算机发送消息，并且子网中所有的计算机都可以接收到发送方发送的消息，每个广播消息都包含一个特殊的IP地址，这个IP中子网内主机标志部分的二进制全部为1。 

a.只能在局域网中使用。 

b.客户端需要绑定服务器广播使用的端口，才可以接收到广播消息。

<img src="https://img-blog.csdnimg.cn/6d54f94878654dd3ba7e7e853154cff0.png" alt="image-20230822160604851" style="zoom:67%;" />

~~~cpp
// 设置广播属性的函数
int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
    - sockfd : 文件描述符
    - level : SOL_SOCKET
    - optname : SO_BROADCAST
    - optval : int类型的值，为1表示允许广播
    - optlen : optval的大小
~~~

##### 代码(有一处不明白)

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024
#define MAX_IPV4_STRING 16

// 广播的IP地址
const char* Broadcast_IP = "127.255.255.255";

int main() {
    // 1.创建通信的socket套接字
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == socket_fd) {
        perror("socket");
        return -1;
    }

    // 开启广播设置
    int _optval = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &_optval, sizeof(_optval));

    // 2.绑定IP和端口，其实在这里我们不接受数据，帮不绑定其实无所谓
    struct sockaddr_in server_addr;
    // 地址族
    server_addr.sin_family = AF_INET;
    // IP
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
    // 端口
    server_addr.sin_port = htons(9999);

    int ret = bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == socket_fd) {
        perror("bind");
        return -1;
    }

    printf("server has initialized.\n");

    // 封装广播客户端的socket地址
    struct sockaddr_in All_Client_addr;
    All_Client_addr.sin_family = AF_INET;
    All_Client_addr.sin_port = htons(10000);
    inet_pton(AF_INET, Broadcast_IP, &All_Client_addr.sin_addr.s_addr);

    // 3.开始通信
    static int num = 0;
    char buf[MAX_BUF_SIZE] = {0};

    while (1) {
        // 服务端向所有的客户端广播数据
        bzero(buf, sizeof(buf));
        sprintf(buf, "hello , i am server , num = %d\n", num++);
        printf("send : %s", buf);

        sendto(socket_fd, buf, strlen(buf), 0, (struct sockaddr*)&All_Client_addr, sizeof(All_Client_addr));
        sleep(1);
    }

    // 4.关闭套接字
    close(socket_fd);

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

#define MAX_BUF_SIZE 1024
#define MAX_IPV4_STRING 16

int main() {
    // 1.创建通信的socket套接字
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == socket_fd) {
        perror("socket");
        return -1;
    }

    // 2.绑定端口信息，让发送方能够正确找到
    struct sockaddr_in client_addr;
    // 地址族
    client_addr.sin_family = AF_INET;
    // IP
    // inet_pton(AF_INET, "127.0.0.2", &client_addr.sin_addr.s_addr);  // 这行代码会出问题，但是我也不知道为什么
    client_addr.sin_addr.s_addr = INADDR_ANY;
    // 端口
    client_addr.sin_port = htons(10000);

    int ret = bind(socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    char buf[MAX_BUF_SIZE] = {0};

    // 2.开始通信
    while (1) {
        // 读数据
        recvfrom(socket_fd, buf, sizeof(buf) - 1, 0, nullptr, nullptr);
        printf("recv : %s", buf);
    }

    // 4.关闭套接字
    close(socket_fd);

    return 0;
}
~~~

我们的代码需要做的功能是服务端启动后，即可开始向局域网内的所有主机广播信息，当有客户端连接进来的时候可以收到客户端的信息

**我们先来解释bind()函数，为什么这里服务端和客户端都使用了bind()？**

**bind()函数可以给我们socket()创建出来的文件描述符绑定我们自己设定的IP和端口信息，比如这里我就给服务端绑定了"127.0.0.1"和9999的信息，客户端绑定了任意IP(局域网内)和10000端口，IP是次要的，bind()函数绑定socket的时候应该首先考虑到给优先接受数据的一方绑定，比如这里就是客户端，为什么呢？因为我发送方一定需要知道一个具体的端口号我才能发送，在UDP中IP倒不一定必须，因为有可能是广播或者组播，这就不是一个具体的IP了，但是端口号是标识不同主机的进程的，所以发送方一定是根据这个端口号找到你对应的进程的，然后如果我得客户端不绑定，就由系统给我自动分配，那就找不到了，所以这里其实服务端的绑定其实没有必要，但是为了习惯我还是加上了；在TCP中也是一样的，我客户端先向服务端发送数据，在这之前需要建立连接，我也是通过人为指定的端口连接服务端，所以服务端绑定了端口，也就调用了bind()**

但是这里我不明白我给客户端指定IP为 127.0.0.2 收不到服务端广播的消息，必须是局域网内的任意IP，也就是INADDR_ANY才行，这里我不明白

另外还有一点就是广播的发送方要给socket()设置广播属性，就像这样

~~~cpp
// 开启广播设置
int _optval = 1;
setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &_optval, sizeof(_optval));
~~~

#### 组播(多播)

**单播地址标识单个 IP 接口，广播地址标识某个子网的所有 IP 接口，多播地址标识一组 IP 接口。 单播和广播是寻址方案的两个极端（要么单个要么全部），多播则意在两者之间提供一种折中方案。多播数据报只应该由对它感兴趣的接口接收，也就是说由运行相应多播会话应用系统的主机上的接口接收。另外，广播一般局限于局域网内使用，而多播则既可以用于局域网，也可以跨广域网使用。** 

**a.组播既可以用于局域网，也可以用于广域网** 

**b.客户端需要加入多播组，才能接收到多播的数据**

<img src="https://img-blog.csdnimg.cn/b197ab6e1a274a7ca1c2a0d8e053f2ee.png" alt="image-20230822160649282" style="zoom:80%;" />

- 组播地址

IP 多播通信必须依赖于 IP 多播地址，在 IPv4 中它的范围从 224.0.0.0 到 239.255.255.255 ， 并被划分为局部链接多播地址、预留多播地址和管理权限多播地址三类:

![image-20230822160933192](https://img-blog.csdnimg.cn/d31c9e84fef34a0694f9f0c0b1d09868.png)

- 设置组播

  ![image-20230823114653916](https://img-blog.csdnimg.cn/77adeff9b9fa4420be25e2ca97d06fc1.png)

多播的API用的比较少，需要用的时候来查询就可以了，但是要知道工作原理

~~~cpp
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
// 服务器设置多播的信息，外出接口
    - level : IPPROTO_IP
    - optname : IP_MULTICAST_IF
    - optval : struct in_addr
// 客户端加入到多播组：
    - level : IPPROTO_IP
    - optname : IP_ADD_MEMBERSHIP
    - optval : struct ip_mreq
        
struct ip_mreq {
    /* IP multicast address of group. */
    struct in_addr imr_multiaddr; // 组播的IP地址
    /* Local IP address of interface. */
    struct in_addr imr_interface; // 本地的IP地址
};

typedef uint32_t in_addr_t;
struct in_addr {
    in_addr_t s_addr;
};
~~~

##### 代码(和前面同样的问题)

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024
#define MAX_IPV4_STRING 16

// 多播的IP地址
const char* Multicast_IP = "239.0.0.10";

int main() {
    // 1.创建通信的socket套接字
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == socket_fd) {
        perror("socket");
        return -1;
    }

    // 设置多播属性，设置外出接口
    struct in_addr _optval;
    // 初始化多播地址
    inet_pton(AF_INET, Multicast_IP, &_optval.s_addr);
    setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_IF, &_optval, sizeof(_optval));

    // 发送方，这里我就不绑定端口了

    printf("server has initialized.\n");

    // 封装广播客户端的socket地址
    struct sockaddr_in All_Client_addr;
    All_Client_addr.sin_family = AF_INET;
    All_Client_addr.sin_port = htons(10000);
    inet_pton(AF_INET, Multicast_IP, &All_Client_addr.sin_addr.s_addr);

    // 3.开始通信
    static int num = 0;
    char buf[MAX_BUF_SIZE] = {0};

    while (1) {
        // 服务端向所有的客户端广播数据
        bzero(buf, sizeof(buf));
        sprintf(buf, "hello , i am server , num = %d\n", num++);
        printf("send : %s", buf);

        sendto(socket_fd, buf, strlen(buf), 0, (struct sockaddr*)&All_Client_addr, sizeof(All_Client_addr));
        sleep(1);
    }

    // 4.关闭套接字
    close(socket_fd);

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

#define MAX_BUF_SIZE 1024
#define MAX_IPV4_STRING 16

// 多播的IP地址
const char* Multicast_IP = "239.0.0.10";

int main() {
    // 1.创建通信的socket套接字
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == socket_fd) {
        perror("socket");
        return -1;
    }

    // 加入多播组
    struct ip_mreq _optval;
    // 初始化
    _optval.imr_interface.s_addr = INADDR_ANY;
    inet_pton(AF_INET, Multicast_IP, &_optval.imr_multiaddr.s_addr);

    setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &_optval, sizeof(_optval));

    // 2.绑定端口信息，让发送方能够正确找到
    struct sockaddr_in client_addr;
    // 地址族
    client_addr.sin_family = AF_INET;
    // IP
    // inet_pton(AF_INET, "127.0.0.2", &client_addr.sin_addr.s_addr);  // 和之前一样的问题
    client_addr.sin_addr.s_addr = INADDR_ANY;
    // 端口
    client_addr.sin_port = htons(10000);

    int ret = bind(socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    char buf[MAX_BUF_SIZE] = {0};

    // 2.开始通信
    while (1) {
        // 读数据
        recvfrom(socket_fd, buf, sizeof(buf) - 1, 0, nullptr, nullptr);
        printf("recv : %s", buf);
    }

    // 4.关闭套接字
    close(socket_fd);

    return 0;
}
~~~

同样的客户端也能收到服务端发送而来的数据

我们同样注意服务端和客户端对于设置多播和加入多播的设置方法

服务端

~~~cpp
// 多播的IP地址
const char* Multicast_IP = "239.0.0.10";    

// 设置多播属性，设置外出接口
struct in_addr _optval;
// 初始化多播地址
inet_pton(AF_INET, Multicast_IP, &_optval.s_addr);
setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_IF, &_optval, sizeof(_optval));
~~~

客户端

~~~cpp
// 多播的IP地址
const char* Multicast_IP = "239.0.0.10";

// 加入多播组
struct ip_mreq _optval;
// 初始化
_optval.imr_interface.s_addr = INADDR_ANY;
inet_pton(AF_INET, Multicast_IP, &_optval.imr_multiaddr.s_addr);

setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &_optval, sizeof(_optval));
~~~

## 本地套接字

本地套接字的作用：本地的进程间通信 
	有关系的进程间的通信 
	没有关系的进程间的通信

**之前我们学过的本地间进程之间通信的方式有：管道(匿名管道pipe，有名管道fifo)；内存映射；信号，信号集；共享内存。记得去复习**

本地套接字实现流程和网络套接字类似，一般呢采用**TCP**的通信流程。

<img src="https://img-blog.csdnimg.cn/7d1897a4b2324c0d9740d51ed0e4d9fd.png" alt="image-20230823123131126" style="zoom: 80%;" />

### API

~~~cpp
// 本地套接字通信的流程 - tcp

// 服务器端
1. 创建监听的套接字
	int lfd = socket(AF_UNIX/AF_LOCAL, SOCK_STREAM, 0);
2. 监听的套接字绑定本地的套接字文件 -> server端
	struct sockaddr_un addr;
	// 绑定成功之后，指定的sun_path中的套接字文件会自动生成。addr里面存的就是他的路径
	bind(lfd, addr, len);
3. 监听
	listen(lfd, 100);
4. 等待并接受连接请求
	struct sockaddr_un cliaddr; // 用的时候记得引头文件 #include <sys/un.h>
	int cfd = accept(lfd, &cliaddr, len);
5. 通信
	接收数据：read/recv
	发送数据：write/send
6. 关闭连接
	close();

// 客户端的流程
1. 创建通信的套接字
	int fd = socket(AF_UNIX/AF_LOCAL, SOCK_STREAM, 0);
2. 监听的套接字绑定本地的套接字文件 -> client端
	struct sockaddr_un addr;
	// 绑定成功之后，指定的sun_path中的套接字文件会自动生成。
	bind(lfd, addr, len);
3. 连接服务器
	struct sockaddr_un serveraddr;
	connect(fd, &serveraddr, sizeof(serveraddr));
4. 通信
	接收数据：read/recv
	发送数据：write/send
5. 关闭连接
	close();
~~~

~~~cpp
// 头文件: sys/un.h
#define UNIX_PATH_MAX 108
struct sockaddr_un {
	sa_family_t sun_family; // 地址族协议 af_local
	char sun_path[UNIX_PATH_MAX]; // 套接字文件的路径, 这是一个伪文件, 大小永远=0
}
~~~

###  工作原理

**我们观察他的流程图以及结合上面API的注释，在本地文件socket地址中，由于是本地的通信，我们不使用IPV4地址或者IPV6地址，也就是sockaddr_in和sockadd_in6，我们使用 sockaddr_un 这个结构体来封装本地的信息，这个结构体一个参数是地址族，另一个参数就非常重要了，就是指定我们用于通信的套接字文件的路径，例如图中就是server.sock和client.sock，这是一个伪文件，大小永远都是0，是用来进行本地进程间通信的；这个文件会在磁盘中被创建出来，在通信的时候，在内核中对应了一块缓冲区，如图所示，客户端B发送数据，先将数据写道他的写缓冲区，在发送到服务端的读缓冲区，因此读写双方都必须有一个这个套接字文件的路径，也就是都需要绑定bind()，这就和一般的TCP通信有区别**

![image-20230823124612140](https://img-blog.csdnimg.cn/cd3dea22a8784da59ba9c9d2802429ec.png)

### 代码

这里我第一次使用TCP特有的send()和recv()函数，他们的返回值和什么时候返回我现在还不是很明白，和write()和read()大差不差，但是还是有区别，需要后续对内核源码的学习才能更好的理解

~~~cpp
// server.cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

int main() {
    // 1.创建socket本地套接字
    int listen_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (-1 == listen_fd) {
        perror("socket");
        return -1;
    }

    // 2.绑定本地套接字文件
    struct sockaddr_un server_addr;
    // 地址族
    server_addr.sun_family = AF_LOCAL;
    // 套接字文件绑定了之后自动生成一个文件用于通信
    strcpy(server_addr.sun_path, "server.sock");

    int ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    // 3.监听
    ret = listen(listen_fd, 5);
    if (-1 == ret) {
        perror("listen");
        return -1;
    }

    // 4.等待客户端连接
    struct sockaddr_un client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int connect_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (-1 == connect_fd) {
        perror("accept");
        return -1;
    }

    printf("client (socket filename : %s) has connected.\n", client_addr.sun_path);

    char buf[MAX_BUF_SIZE] = {0};
    // 5.开始通信
    while (1) {
        // 读
        bzero(buf, sizeof(buf));
        int len = recv(connect_fd, buf, sizeof(buf) - 1, 0);
        if (-1 == len) {
            if (errno == ECONNRESET)  // 报错处理
                goto CLOSE;
            perror("recv");
            return -1;
        }

        if (len > 0)
            printf("recv : %s", buf);
        else if (0 == len) {
        CLOSE:
            printf("client (socket filename : %s) has closed...\n", client_addr.sun_path);
            break;
        }

        // 写
        send(connect_fd, buf, strlen(buf), 0);
    }

    // 6.关闭连接
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
#include <sys/un.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

int main() {
    // 1.创建本地socket套接字
    int connect_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (-1 == connect_fd) {
        perror("socket");
        return -1;
    }

    // 2.绑定本地套接字文件
    struct sockaddr_un client_addr;
    client_addr.sun_family = AF_LOCAL;
    strcpy(client_addr.sun_path, "client.sock");

    int ret = bind(connect_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if (-1 == ret) {
        perror("bind");
        return -1;
    }

    // 3.连接客户端
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, "server.sock");

    ret = connect(connect_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == ret) {
        perror("connect");
        return -1;
    }

    char buf[MAX_BUF_SIZE] = {0};
    static int num = 0;
    // 4.开始通信
    while (1) {
        // 写
        bzero(buf, sizeof(buf));
        sprintf(buf, "hello , i am client , num : %d\n", num++);
        printf("send : %s", buf);
        send(connect_fd, buf, strlen(buf), 0);

        // 读
        bzero(buf, sizeof(buf));
        int len = recv(connect_fd, buf, sizeof(buf) - 1, 0);
        if (-1 == len) {
            perror("recv");
            return -1;
        }
        if (len > 0)
            printf("recv : %s", buf);
        else if (0 == len) {
            printf("server has closed...\n");
            break;
        }

        sleep(1);
    }

    return 0;
}
~~~

#### 代码分析(细节地方不是很明白)

代码中有很多地方值得我们推敲，我们先来看代码的执行结果：

注意，由于我们是通过套接字文件和内核缓冲区进行联系，因此会创建出来一个套接字文件，只能使用Linux的原生目录！

服务端

<img src="https://img-blog.csdnimg.cn/3d4aa523a5034c1390b1bc470d7709a3.png" alt="image-20230823151111152" style="zoom:80%;" />

客户端

![image-20230823151122954](https://img-blog.csdnimg.cn/c785d59985014383a8da975a7823395f.png)

可以看出能够正常跑出来，当我们绑定bind()套接字文件之后我们查看目录下多出了两个文件，这两个文件正是用来与内核中缓冲区直接联系并且用于通信的；可以看出他们并没有大小，是一个伪文件

![image-20230823151222376](https://img-blog.csdnimg.cn/874b16ab2bf14ba3a42fc3c14852e95c.png)

现在我再次运行服务端或者客户端，发现出现了这样的情况：

报错：bind Address already in use；这正是因为我们创建的套接字文件还在这里没有被释放(删除)导致被占据而没有办法bind()成功导致的，我们将其删除即可

![image-20230823151351101](https://img-blog.csdnimg.cn/ec19cfa44a2d46cea45647e73bc8fdec.png)

我们查看代码中的这一部分：

~~~cpp
// server.cpp
...

// 5.开始通信
while (1) {
	...
        
    int len = recv(connect_fd, buf, sizeof(buf) - 1, 0);
    if (-1 == len) {
        if (errno == ECONNRESET)  // 报错处理
            goto CLOSE;
        perror("recv");
        return -1;
    }

	...
        
    else if (0 == len) {
    CLOSE:
        printf("client (socket filename : %s) has closed...\n", client_addr.sun_path);
        break;
    }

	...
}

...
~~~

我们发现进行了一个报错的特殊处理，我们试着将其删除再来跑代码，这次我们强制停掉客户端，结果如下：

客户端

![image-20230823152331909](https://img-blog.csdnimg.cn/310ffa736819439db823ddf4c8d13baf.png)

服务端

![image-20230823152340686](https://img-blog.csdnimg.cn/2d7bdf2f570641c69f636158e6cd7c53.png)

可以看出在读数据的时候报错了，错误信息是 Connection rest by peer，我不知道具体原因，但是我猜测大致应该是强制停掉客户端，客户端结束后会给服务端发送一个信号，然后服务端这个时候也在阻塞读，收到这个信号后就不阻塞了(和之前那个软中断类似)，然后发生了错误，但是其实是客户端断开了连接导致的

上网查询后我们发现错误号是 ECONNRESET ，因此特殊处理即可

还是上面的代码，我不做处理，当我让client正常的结束，这里我让while()加了个条件，发现不报错了，走的是 0==len 写端关闭的这里，并且话也正常打印出来了，如下：

客户端

![image-20230823152830630](https://img-blog.csdnimg.cn/8432b977dee040368d13886a32e514d1.png)

服务端

![image-20230823152839250](https://img-blog.csdnimg.cn/c27e217324d64dec941708725af7b18f.png)

因此这个recv()函数什么时候返回，或者返回什么值，甚至read()的认知都有可能在这里和我的不完全一样，等待后续的进一步研究吧

现在我们在一切代码正确的情况下强制停掉服务端，结果如下：

服务端

![image-20230823153112240](https://img-blog.csdnimg.cn/0b9e16562a5647f5a37c98a205eff04f.png)

客户端

![image-20230823153119822](https://img-blog.csdnimg.cn/da48eaf0e71f43f4a0ea09d08f0b15ec.png)

我们发现客户端直接停止运行了，没有报错也没有正常的输出，肯定是异常退出，但是这里我确实不知道为什么，可见水很深

说了这么多，其实真正的开发能用就行，但是现在在学习的过程还是要尽量考虑完全，每一步都尽量弄明白

# 第五章 项目实战和总结

我们的最终目标是完成一个web服务器项目

## 阻塞/非阻塞、同步/异步(网络IO)

**典型的一次IO的两个阶段是什么？数据就绪 和 数据读写**

数据就绪：根据系统IO操作的就绪状态

- 阻塞
- 非阻塞

数据读写：根据应用程序和内核的交互方式

- 同步
- 异步

结合这张图理解：

- **数据就绪部分，每一个文件描述符在进行读写操作的时候在内核中都对应了一块缓冲区用于临时存放接受到或者写了要发出去的数据，例如，当有数据进来让读缓冲区不为空的时候，就是数据读的就绪的时候，发生在操作系统的内核部分**

  **数据读写部分，用户程序部分，就是用户向文件内写或者读数据，也就是写入缓冲区或者读入缓冲区，然后缓冲区再到内存或者通信就是另一方的缓冲区**

- **同步：我们用户自己定义一个buf[]来存放缓冲区中的数据，代码在执行的时候一点点从sockfd对应的内核缓冲区中搬到我们的buf[]当中，搬移完毕之前代码不会向下执行；这种过程是我们用户应用程序自己搬(因为我们读的时候停在这里了，在这个过程中我们没有办法执行其他工作)，不是操作系统搬完了通知我们**

  **异步：操作系统给我们搬，搬完了通知我们，比较常见的方式是sigio信号，相比同步效率更高，因为搬动过程中应用程序可以干自己的工作，但是要看系统有没有提供接口并且代码的编写也不简单**

![阻塞、非阻塞、同步、异步](https://img-blog.csdnimg.cn/33619d16a7f6437c93ea2845d42ecd0d.png)

陈硕：**在处理 IO 的时候，阻塞和非阻塞都是同步 IO，只有使用了特殊的 API 才是异步 IO。**

注意：I/O多路复用(多路转接)技术，select/poll/epoll 三种技术都是同步操作；首先它只能帮我们检测到有多少个客户端是数据就绪的，或者哪几个(epoll)，真正的数据读写操作还是我们自己解决，因此是同步；第二，根据陈硕老师说的话，一般情况我们都是使用同步的I/O，因为异步I/O会导致一些不可预期的问题，并且用的很少，一般只有使用特殊的API才是异步I/O

![image-20230823155027679](https://img-blog.csdnimg.cn/0c42e36286624af9b0d13588ae7cf346.png)

**一个典型的网络IO接口调用，分为两个阶段，分别是“数据就绪” 和“数据读写”，数据就绪阶段分为阻塞和非阻塞，表现得结果就是，阻塞当前线程或是直接返回。** 

**同步表示A向B请求调用一个网络IO接口时（或者调用某个业务逻辑API接口时），数据的读写都是由请求方A自己来完成的（不管是阻塞还是非阻塞）；异步表示A向B请求调用一个网络IO接口时 （或者调用某个业务逻辑API接口时），向B传入请求的事件以及事件发生时通知的方式，A就可以处理其它逻辑了，当B监听到事件处理完成后，会用事先约定好的通知方式，通知A处理结果。**

**我们为什么不建议使用异步接口？很大一个原因就是异步接口系统通知用户大部分使用信号，使用信号产生软中断之后，在多进程或者多线程当中可能会出现一些难以想到的问题，不好处理，这也加大了代码的编写复杂程度**

- 同步阻塞
- 同步非阻塞
- 异步阻塞
- 异步非阻塞 **(异步基本上伴着非阻塞使用，因为我们应用程序交给操作系统处理之后我们可以继续往下执行，提高效率)**

## Unix/Linux上的五种IO模型

### 阻塞 blocking

调用者调用了某个函数，等待这个函数返回，期间什么也不做，不停的去检查这个函数有没有返回，必须等这个函数返回才能进行下一步动作。

![image-20230823155137394](https://img-blog.csdnimg.cn/f31562a8068f4ad69677dd64060c0f76.png)

### 非阻塞 non-blocking（NIO）

非阻塞等待，每隔一段时间就去检测IO事件是否就绪。没有就绪就可以做其他事。非阻塞I/O执行系统调用总是立即返回**，不管事件是否已经发生，若事件没有发生，则返回-1，此时可以根据 errno 区分这两种情况，对于accept，recv 和 send，事件未发生时，errno 通常被设置成 EAGAIN。**

![image-20230823155156625](https://img-blog.csdnimg.cn/ecaead2b8a1e419980aad60e789cabed.png)

### IO复用（IO multiplexing）

Linux 用 select/poll/epoll 函数实现 IO 复用模型，这些函数也会使进程阻塞，但是和阻塞IO所不同的是 这些函数可以同时阻塞多个IO操作。而且可以同时对多个读操作、写操作的IO函数进行检测。直到有数 据可读或可写时，才真正调用IO操作函数。

![image-20230823155217650](https://img-blog.csdnimg.cn/20a9068f5875426383a8fbd8b4460c20.png)

### 信号驱动（signal-driven）

Linux 用套接口进行信号驱动 IO，安装一个信号处理函数，进程继续运行并不阻塞，当IO事件就绪，进程收到 SIGIO 信号，然后处理 IO 事件。

![image-20230823155237200](https://img-blog.csdnimg.cn/a15c5d95bfda44d5acfd5b55af381311.png)

内核在第一个阶段是异步，在第二个阶段是同步；与非阻塞IO的区别在于它提供了消息通知机制，不需要用户进程不断的轮询检查，减少了系统API的调用次数，提高了效率。 

### 异步（asynchronous）

Linux中，可以调用 aio_read 函数告诉内核描述字缓冲区指针和缓冲区的大小、文件偏移及通知的方式，然后立即返回，当内核将数据拷贝到缓冲区后，再通知应用程序

![image-20230823155255891](https://img-blog.csdnimg.cn/0d631b3be40c478396ccc406e68ee355.png)

~~~cpp
/* Asynchronous I/O control block. */
struct aiocb {
    int aio_fildes; /* File desriptor. */
    int aio_lio_opcode; /* Operation to be performed. */
    int aio_reqprio; /* Request priority offset. */
    volatile void *aio_buf; /* Location of buffer. */
    size_t aio_nbytes; /* Length of transfer. */
    struct sigevent aio_sigevent; /* Signal number and value. */
    
    /* Internal members. */
    struct aiocb *__next_prio;
    int __abs_prio;
    int __policy;
    int __error_code;
    __ssize_t __return_value;
    
    #ifndef __USE_FILE_OFFSET64
        __off_t aio_offset; /* File offset. */
        char __pad[sizeof (__off64_t) - sizeof (__off_t)];
    #else
        __off64_t aio_offset; /* File offset. */
    #endif
        char __glibc_reserved[32];
};
~~~

## Web Server(网页服务器)

一个 Web Server 就是一个服务器软件（程序），或者是运行这个服务器软件的硬件（计算机）。其主要功能是通过 HTTP 协议与客户端（通常是浏览器（Browser））进行通信，来接收，存储，处理来自客户端的 HTTP 请求，并对其请求做出 HTTP 响应，返回给客户端其请求的内容（文件、网页等）或返 回一个 Error 信息。

![image-20230825104334383](https://img-blog.csdnimg.cn/30c9bbc75f6840ea95d63ea714fd7573.png)

通常用户使用 Web 浏览器与相应服务器进行通信。在浏览器中键入“域名”或“IP地址:端口号”，浏览器则 先将你的域名解析成相应的 IP 地址或者直接根据你的IP地址向对应的 Web 服务器发送一个 HTTP 请求。这一过程首先要通过 TCP 协议的三次握手建立与目标 Web 服务器的连接，然后 HTTP 协议生成针 对目标 Web 服务器的 HTTP 请求报文，通过 TCP、IP 等协议发送到目标 Web 服务器上。

## HTTP协议(应用层的协议)

### 简介

超文本传输协议（Hypertext Transfer Protocol，HTTP）是一个简单的请求 - 响应协议，它通常运行在 TCP 之上。它指定了客户端可能发送给服务器什么样的消息以及得到什么样的响应。**请求和响应消息的头以 ASCII 形式给出；而消息内容则具有一个类似 MIME 的格式。HTTP是万维网的数据通信的基础。** 

HTTP的发展是由蒂姆·伯纳斯-李于1989年在欧洲核子研究组织（CERN）所发起。HTTP的标准制定由万 维网协会（World Wide Web Consortium，W3C）和互联网工程任务组（Internet Engineering Task Force，IETF）进行协调，最终发布了一系列的RFC，其中最著名的是1999年6月公布的 RFC 2616，定 义了HTTP协议中现今广泛使用的一个版本——HTTP 1.1。

### 概述

HTTP 是一个客户端终端（用户）和服务器端（网站）请求和应答的标准（TCP）。通过使用网页浏览器、网络爬虫或者其它的工具，客户端发起一个HTTP请求到服务器上指定端口（默认端口为80）。我们 称这个客户端为用户代理程序（user agent）。应答的服务器上存储着一些资源，比如 HTML 文件和图 像。我们称这个应答服务器为源服务器（origin server）。在用户代理和源服务器中间可能存在多个“中 间层”，比如代理服务器、网关或者隧道（tunnel）。 

尽管 TCP/IP 协议是互联网上最流行的应用，HTTP 协议中，并没有规定必须使用它或它支持的层。事实 上，HTTP可以在任何互联网协议上，或其他网络上实现。HTTP 假定其下层协议提供可靠的传输。因此，任何能够提供这种保证的协议都可以被其使用。因此也就是其在 TCP/IP 协议族使用 TCP 作为其传输层。 

通常，由HTTP客户端发起一个请求，创建一个到服务器指定端口（默认是80端口）的 TCP 连接。HTTP 服务器则在那个端口监听客户端的请求。一旦收到请求，服务器会向客户端返回一个状态，比 如"HTTP/1.1 200 OK"，以及返回的内容，如请求的文件、错误消息、或者其它信息。

### 工作原理

**HTTP 协议定义 Web 客户端如何从 Web 服务器请求 Web 页面，以及服务器如何把 Web 页面传送给客户端。HTTP 协议采用了请求/响应模型。客户端向服务器发送一个请求报文，请求报文包含请求的方 法、URL、协议版本、请求头部和请求数据。服务器以一个状态行作为响应，响应的内容包括协议的版 本、成功或者错误代码、服务器信息、响应头部和响应数据。**

以下是 HTTP 请求/响应的步骤： 

1. 客户端连接到 Web 服务器 一个HTTP客户端，通常是浏览器，与 Web 服务器的 HTTP 端口（默认为 80 ）建立一个 TCP 套接字连接。例如，http://www.baidu.com。（URL） 
2. 发送 HTTP 请求 通过 TCP 套接字，客户端向 Web 服务器发送一个文本的请求报文，一个请求报文由请求行、请求头部、空行和请求数据 4 部分组成。 
3. 服务器接受请求并返回 HTTP 响应 Web 服务器解析请求，定位请求资源。服务器将资源复本写到 TCP 套接字，由客户端读取。一个 响应由状态行、响应头部、空行和响应数据 4 部分组成。 
4. 释放连接 TCP 连接 若 connection 模式为 close，则服务器主动关闭 TCP连接，客户端被动关闭连接，释放 TCP 连 接；若connection 模式为 keepalive，则该连接会保持一段时间，在该时间内可以继续接收请求; 
5. 客户端浏览器解析 HTML 内容 客户端浏览器首先解析状态行，查看表明请求是否成功的状态代码。然后解析每一个响应头，响应 头告知以下为若干字节的 HTML 文档和文档的字符集。客户端浏览器读取响应数据 HTML，根据 HTML 的语法对其进行格式化，并在浏览器窗口中显示。

例如：在浏览器地址栏键入URL，按下回车之后会经历以下流程： 

1. 浏览器向 DNS 服务器请求解析该 URL 中的域名所对应的 IP 地址; 
2. 解析出 IP 地址后，根据该 IP 地址和默认端口 80，和服务器建立 TCP 连接; 
3. 浏览器发出读取文件（ URL 中域名后面部分对应的文件）的 HTTP 请求，该请求报文作为 TCP 三次握手的第三个报文的数据发送给服务器; 
4. 服务器对浏览器请求作出响应，并把对应的 HTML 文本发送给浏览器; 
5. 释放 TCP 连接; 
6. 浏览器将该 HTML 文本并显示内容。

![image-20230825104527500](https://img-blog.csdnimg.cn/53135713dd464ab8abca903c116e08e6.png)

HTTP 协议是基于 TCP/IP 协议之上的应用层协议，基于 请求-响应 的模式。HTTP 协议规定，请求从客 户端发出，最后服务器端响应该请求并返回。换句话说，肯定是先从客户端开始建立通信的，服务器端 在没有接收到请求之前不会发送响应。

### HTTP请求报文格式

![image-20230825104549067](https://img-blog.csdnimg.cn/42f0dffaf2e04514ae590d4ed5f83dd7.png)

请求头示例：

~~~http
GET / HTTP/1.1
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
Accept-Encoding: gzip, deflate, br
Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6
Cache-Control: max-age=0
Connection: keep-alive
Cookie: BAIDUID_BFESS=EDD638FDDEE39DA6726BEF39B766F772:FG=1; BIDUPSID=EDD638FDDEE39DA6726BEF39B766F772; PSTM=1692844550; BD_UPN=12314753; ZFY=k1Qc:BgGUBKTsi3GUtqJh0rkU8WABgG3nCPeWyqD:BLRs:C; BA_HECTOR=010g810l2ga525208lag0k0q1ieg58u1o; RT="z=1&dm=baidu.com&si=de024639-c7be-4913-b467-d3935e4a8d1b&ss=llpzta3z&sl=0&tt=0&bcn=https%3A%2F%2Ffclog.baidu.com%2Flog%2Fweirwood%3Ftype%3Dperf&ul=vdy&hd=vef"
Host: www.baidu.com
Sec-Fetch-Dest: document
Sec-Fetch-Mode: navigate
Sec-Fetch-Site: none
Sec-Fetch-User: ?1
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36 Edg/114.0.1823.37
sec-ch-ua: "Not.A/Brand";v="8", "Chromium";v="114", "Microsoft Edge";v="114"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "Windows"
~~~

### HTTP响应报文格式

![image-20230825104624298](https://img-blog.csdnimg.cn/e04c24b70d834ad4bab7dbd6cfc42a66.png)

响应头示例：

~~~http
HTTP/1.1 200 OK
Connection: keep-alive
Content-Encoding: gzip
Content-Security-Policy: frame-ancestors 'self' https://chat.baidu.com http://mirror-chat.baidu.com https://fj-chat.baidu.com https://hba-chat.baidu.com https://hbe-chat.baidu.com https://njjs-chat.baidu.com https://nj-chat.baidu.com https://hna-chat.baidu.com https://hnb-chat.baidu.com http://debug.baidu-int.com;
Content-Type: text/html; charset=utf-8
Date: Fri, 25 Aug 2023 03:37:36 GMT
Server: BWS/1.1
Traceid: 1692934656033891585015847248764579371337
X-Ua-Compatible: IE=Edge,chrome=1
Transfer-Encoding: chunked

// 响应正文(和响应头之间有一个空行)
...
~~~

### HTTP请求方法

HTTP/1.1 协议中共定义了八种方法（也叫“动作”）来以不同方式操作指定的资源：

1. **GET：向指定的资源发出“显示”请求。使用 GET 方法应该只用在读取数据，而不应当被用于产生“副作用”的操作中，例如在 Web Application 中。其中一个原因是 GET 可能会被网络蜘蛛(爬虫)等随意访问。** 
2. HEAD：与 GET 方法一样，都是向服务器发出指定资源的请求。只不过服务器将不传回资源的本文部分。它的好处在于，使用这个方法可以在不必传输全部内容的情况下，就可以获取其中“关于该 资源的信息”（元信息或称元数据）。 
3. **POST：向指定资源提交数据，请求服务器进行处理（例如提交表单或者上传文件）。数据被包含在请求本文中。这个请求可能会创建新的资源或修改现有资源，或二者皆有。** 
4. PUT：向指定资源位置上传其最新内容。 
5. DELETE：请求服务器删除 Request-URI 所标识的资源。 
6. TRACE：回显服务器收到的请求，主要用于测试或诊断。 
7. OPTIONS：这个方法可使服务器传回该资源所支持的所有 HTTP 请求方法。用'*'来代替资源名称， 向 Web 服务器发送 OPTIONS 请求，可以测试服务器功能是否正常运作。 
8. CONNECT：HTTP/1.1 协议中预留给能够将连接改为管道方式的代理服务器。通常用于SSL加密服 务器的链接（经由非加密的 HTTP 代理服务器）。

### HTTP状态码

所有HTTP响应的第一行都是状态行，依次是当前HTTP版本号，3位数字组成的状态代码，以及描述状态 的短语，彼此由空格分隔。

**状态代码的第一个数字代表当前响应的类型：**

- **1xx消息——请求已被服务器接收，继续处理** 
- **2xx成功——请求已成功被服务器接收、理解、并接受** 
- **3xx重定向——需要后续操作才能完成这一请求** 
- **4xx请求错误——请求含有词法错误或者无法被执行** 
- **5xx服务器错误——服务器在处理某个正确请求时发生错误**

虽然 RFC 2616 中已经推荐了描述状态的短语，例如"200 OK"，"404 Not Found"，但是WEB开发者仍 然能够自行决定采用何种短语，用以显示本地化的状态描述或者自定义信息。

![image-20230825104759498](https://img-blog.csdnimg.cn/53c866b9df924e858c23e050129f9585.png)

更多状态码: https://baike.baidu.com/item/HTTP%E7%8A%B6%E6%80%81%E7%A0%81/5053660?r=aladdin

## 服务器编程基本框架

虽然服务器程序种类繁多，但其基本框架都一样，不同之处在于逻辑处理。

![image-20230825114344227](https://img-blog.csdnimg.cn/290e332f1197464f9019cbb0c3b5af9c.png)

| 模块         | 功能                       |
| ------------ | :------------------------- |
| I/O 处理单元 | 处理客户连接，读写网络数据 |
| 逻辑单元     | 业务进程或线程             |
| 网络存储单元 | 数据库、文件或缓存         |
| 请求队列     | 各单元之间的通信方式       |

**I/O 处理单元是服务器管理客户连接的模块。它通常要完成以下工作：等待并接受新的客户连接，接收客户数据，将服务器响应数据返回给客户端。但是数据的收发不一定在 I/O 处理单元中执行，也可能在 逻辑单元中执行，具体在何处执行取决于事件处理模式。** 

**一个逻辑单元通常是一个进程或线程。它分析并处理客户数据，然后将结果传递给 I/O 处理单元或者直接发送给客户端（具体使用哪种方式取决于事件处理模式）。服务器通常拥有多个逻辑单元，以实现对多个客户任务的并发处理。** 

**网络存储单元可以是数据库、缓存和文件，但不是必须的。** 

**请求队列是各单元之间的通信方式的抽象。I/O 处理单元接收到客户请求时，需要以某种方式通知一个 逻辑单元来处理该请求。同样，多个逻辑单元同时访问一个存储单元时，也需要采用某种机制来协调处 理竞态条件。请求队列通常被实现为池的一部分。**

## 两种高效的事件处理模式

**服务器程序通常需要处理三类事件：I/O 事件、信号及定时事件。有两种高效的事件处理模式：Reactor 和 Proactor，同步 I/O 模型通常用于实现 Reactor 模式，异步 I/O 模型通常用于实现 Proactor 模式。**

### Reactor模式

**要求主线程（I/O处理单元）只负责监听文件描述符上是否有事件发生，有的话就立即将该事件通知工作 线程（逻辑单元），将 socket 可读可写事件放入请求队列，交给工作线程处理。除此之外，主线程不做任何其他实质性的工作。读写数据，接受新的连接，以及处理客户请求均在工作线程中完成。**

使用同步 I/O（以 epoll_wait 为例）实现的 Reactor 模式的工作流程是：

1. 主线程往 epoll 内核事件表中注册 socket 上的读就绪事件。 
2. 主线程调用 epoll_wait 等待 socket 上有数据可读。 
3. 当 socket 上有数据可读时， epoll_wait 通知主线程。主线程则将 socket 可读事件放入请求队列。 
4. 睡眠在请求队列上的某个工作线程被唤醒，它从 socket 读取数据，并处理客户请求，然后往 epoll 内核事件表中注册该 socket 上的写就绪事件。 
5. 当主线程调用 epoll_wait 等待 socket 可写。 
6. 当 socket 可写时，epoll_wait 通知主线程。主线程将 socket 可写事件放入请求队列。 
7. 睡眠在请求队列上的某个工作线程被唤醒，它往 socket 上写入服务器处理客户请求的结果。

Reactor 模式的工作流程：

![image-20230825114619086](https://img-blog.csdnimg.cn/012d2da2aa36421cbab009ef923457f3.png)

### Proactor模式

**Proactor 模式将所有 I/O 操作都交给主线程和内核来处理（进行读、写），工作线程仅仅负责业务逻辑。**

使用异步 I/O 模型（以 aio_read 和 aio_write 为例）实现的 Proactor 模式的工作流程是：

1. 主线程调用 aio_read 函数向内核注册 socket 上的读完成事件，并告诉内核用户读缓冲区的位置， 以及读操作完成时如何通知应用程序（这里以信号为例）。 
2. 主线程继续处理其他逻辑。 
3. 当 socket 上的数据被读入用户缓冲区后，内核将向应用程序发送一个信号，以通知应用程序数据已经可用。 
4. 应用程序预先定义好的信号处理函数选择一个工作线程来处理客户请求。工作线程处理完客户请求后，调用 aio_write 函数向内核注册 socket 上的写完成事件，并告诉内核用户写缓冲区的位置，以及写操作完成时如何通知应用程序。 
5. 主线程继续处理其他逻辑。 
6. 当用户缓冲区的数据被写入 socket 之后，内核将向应用程序发送一个信号，以通知应用程序数据已经发送完毕。 
7. 应用程序预先定义好的信号处理函数选择一个工作线程来做善后处理，比如决定是否关闭 socket。

Proactor 模式的工作流程：

![image-20230825114659772](https://img-blog.csdnimg.cn/b26447c143a746018e84fe581c62815a.png)

### 模拟 Proactor 模式

**使用同步 I/O 方式模拟出 Proactor 模式。原理是：主线程执行数据读写操作，读写完成之后，主线程向工作线程通知这一”完成事件“。那么从工作线程的角度来看，它们就直接获得了数据读写的结果，接下 来要做的只是对读写的结果进行逻辑处理。**

使用同步 I/O 模型（以 epoll_wait为例）模拟出的 Proactor 模式的工作流程如下：

1. **主线程往 epoll 内核事件表中注册 socket 上的读就绪事件。** 
2. **主线程调用 epoll_wait 等待 socket 上有数据可读。** 
3. **当 socket 上有数据可读时，epoll_wait 通知主线程。主线程从 socket 循环读取数据，直到没有更多数据可读，然后将读取到的数据封装成一个请求对象并插入请求队列。(注意是主线程读了封装好交给线程池去处理)** 
4. **睡眠在请求队列上的某个工作线程被唤醒，它获得请求对象并处理客户请求，然后往 epoll 内核事 件表中注册 socket 上的写就绪事件。** 
5. **主线程调用 epoll_wait 等待 socket 可写。** 
6. **当 socket 可写时，epoll_wait 通知主线程。主线程往 socket 上写入服务器处理客户请求的结果。**

同步 I/O 模拟 Proactor 模式的工作流程：

![image-20230825114740311](https://img-blog.csdnimg.cn/f51df872387c48df990e3a68e99f56a9.png)

## 线程池

**我们在之前写多线程的TCP通信的时候，处理客户端的请求是客户端来一个我接受一个，然后创建子线程去处理逻辑，这样的方式我们仔细想一下好嘛？其实并不好，为什么呢？因为我们假设某一时刻同时进来了很多个客户端，服务器在这一瞬间的压力是巨大的，而后因为客户端的不断增多，服务器的压力也在不断的增大，并且由于我们是需要用的时候创建子线程，用完就把子线程给销毁掉，在这个过程中也是非常消耗系统的资源和时间的，所以不推荐使用。所以我们这里用线程池解决这些问题**

**线程池是由服务器预先创建的一组子线程，线程池中的线程数量应该和 CPU 数量差不多。线程池中的所有子线程都运行着相同的代码。当有新的任务到来时，主线程将通过某种方式选择线程池中的某一个子线程来为之服务。相比与动态的创建子线程，选择一个已经存在的子线程的代价显然要小得多。**至于主线程选择哪个子线程来为新任务服务，则有多种方式：

- 主线程使用某种算法来主动选择子线程。最简单、最常用的算法是随机算法和 Round Robin（轮流 选取）算法，但更优秀、更智能的算法将使任务在各个工作线程中更均匀地分配，从而减轻服务器的整体压力。
- **主线程和所有子线程通过一个共享的工作队列来同步，子线程都睡眠在该工作队列上。当有新的任务到来时，主线程将任务添加到工作队列中。这将唤醒正在等待任务的子线程，不过只有一个子线程将获得新任务的”接管权“，它可以从工作队列中取出任务并执行之，而其他子线程将继续睡眠在工作队列上。**

线程池的一般模型为：

![image-20230825145330177](https://img-blog.csdnimg.cn/6258b706f1f04d4692f5155872a806ac.png)

线程池中的线程数量最直接的限制因素是中央处理器(CPU)的处理器(processors/cores)的数量 N ：如果你的CPU是4-cores的，对于CPU密集型的任务(如视频剪辑等消耗CPU计算资源的任务)来 说，那线程池中的线程数量最好也设置为4（或者+1防止其他因素造成的线程阻塞）；对于IO密集 型的任务，一般要多于CPU的核数，因为线程间竞争的不是CPU的计算资源而是IO，IO的处理一 般较慢，多于cores数的线程将为CPU争取更多的任务，不至在线程处理IO的过程造成CPU空闲导 致资源浪费。

- 空间换时间，浪费服务器的硬件资源，换取运行效率。 
- 池是一组资源的集合，这组资源在服务器启动之初就被完全创建好并初始化，这称为静态资源。 
- 当服务器进入正式运行阶段，开始处理客户请求的时候，如果它需要相关的资源，可以直接从池中获取，无需动态分配。 
- 当服务器处理完一个客户连接后，可以把相关的资源放回池中，无需执行系统调用释放资源。

## 有限状态机

逻辑单元内部的一种高效编程方法：有限状态机（finite state machine）。 

有的应用层协议头部包含数据包类型字段，每种类型可以映射为逻辑单元的一种执行状态，服务器可以 根据它来编写相应的处理逻辑。如下是一种状态独立的有限状态机：

~~~cpp
STATE_MACHINE( Package _pack ) {
    PackageType _type = _pack.GetType();
    switch( _type ) {
        case type_A:
            process_package_A( _pack );
            break;
        case type_B:
            process_package_B( _pack );
            break;
    }
}
~~~

这是一个简单的有限状态机，只不过该状态机的每个状态都是相互独立的，即状态之间没有相互转移。 状态之间的转移是需要状态机内部驱动，如下代码：

~~~cpp
STATE_MACHINE() {
	State cur_State = type_A;
    while( cur_State != type_C ) {
        Package _pack = getNewPackage();
        switch( cur_State ) {
        case type_A:
            process_package_state_A( _pack );
            cur_State = type_B;
            break;
        case type_B:
            process_package_state_B( _pack );
            cur_State = type_C;
            break;
        }
    }
}
~~~

该状态机包含三种状态：type_A、type_B 和 type_C，其中 type_A 是状态机的开始状态，type_C 是状 态机的结束状态。状态机的当前状态记录在 cur_State 变量中。在一趟循环过程中，状态机先通过 getNewPackage 方法获得一个新的数据包，然后根据 cur_State 变量的值判断如何处理该数据包。数据 包处理完之后，状态机通过给 cur_State 变量传递目标状态值来实现状态转移。那么当状态机进入下一 趟循环时，它将执行新的状态对应的逻辑。

## EPOLLONESHOT事件

我们即使可以使用 ET 模式，一个socket 上的某个事件还是可能被触发多次，因为可能这个socket上没读完就有新数据进来了，这在并发程序中就会引起一个问题。比如一个线程在读取完某个 socket 上的数据后开始处理这些数据，而在数据的处理过程中该 socket 上又有新数据可读（EPOLLIN 再次被触发），此时另外一个线程被唤醒来读取这些新的数据。于是就出现了两个线程同时操作一个 socket 的局面。一个socket连接在任一时刻都只被一个线程处理，可以使用 epoll 的 EPOLLONESHOT 事件实现。 

**对于注册了 EPOLLONESHOT 事件的文件描述符，操作系统最多触发其上注册的一个可读、可写或者异常事件，且只触发一次(加入了这个事件之后即使是LT工作模式也只能触发一次)，除非我们使用 epoll_ctl 函数重置该文件描述符上注册的 EPOLLONESHOT 事件。**这样，当一个线程在处理某个 socket 时，其他线程是不可能有机会操作该 socket 的。但反过来思考，注册了 EPOLLONESHOT 事件的 socket 一旦被某个线程处理完毕， 该线程就应该立即重置这个 socket 上的 EPOLLONESHOT 事件，以确保这个 socket 下一次可读时，其 EPOLLIN 事件能被触发，进而让其他工作线程有机会继续处理这个 socket。

## 服务器压力测试

Webbench 是 Linux 上一款知名的、优秀的 web 性能压力测试工具。它是由Lionbridge公司开发。 

- 测试处在相同硬件上，不同服务的性能以及不同硬件上同一个服务的运行状况。 
- 展示服务器的两项内容：每秒钟响应请求数和每秒钟传输数据量。 

基本原理：Webbench 首先 fork 出多个子进程，每个子进程都循环做 web 访问测试。子进程把访问的结果通过 pipe 告诉父进程，父进程做最终的统计结果。

测试示例

~~~bash
webbench -c 1000 -t 30 http://192.168.110.129:10000/index.html
    参数：
        -c 表示客户端数
        -t 表示时间
~~~

## 项目总体流程实现以及代码

这个项目目前是写的差不多了，但是还有bug，响应报文回写出了些问题，我还没来得及修，太难了！

TODO

### 链接和构建

项目文件放在github上，链接：[https://github.com/DavidingPlus/Linux_Projects/tree/web-server](https://github.com/DavidingPlus/Linux_Projects/tree/web-server)

使用cmake编译项目需要在根目录创建一个build目录，然后进入build目录，之后如下操作进行编译，我设置了项目生成的可执行文件路径在 build 目录下，名称为app

~~~bash
cmake ..
make
./app
~~~

