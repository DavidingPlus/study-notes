---
title: Linux 设备驱动开发详解
categories:
  - Linux学习
abbrlink: 484892ff
date: 2024-10-24 15:00:00
updated: 2024-11-07 18:10:00
---

<meta name="referrer" content="no-referrer"/>

本书基于 Linux 4.0 内核编写，个人学习测试的内核版本是 5.15.167。

# Linux 内核及内核编程

## Linux 2.6 后的内核特点

Linux 2.6 相对于 Linux 2.4 有着相当大的改进，主要表现为以下几个方面。

### 新的调度器

Linux 2.6 以后版本的 Linux 内核使用了新的进程调度算法，它在高负载的情况下有极其出色的性能，并且当有很多处理器时也可以很好地扩展。在 Linux 内核 2.6 的早期采用了 O（1）算法，之后转移到 CFS（Completely Fair Scheduler，完全公平调度）算法。在 Linux 3.14 中，也增加了一个新的调度类：SCHED_DEADLINE，它实现了 EDF（Earliest Deadline First，最早截止期限优先）调度算法。

<!-- more -->

### 内核抢占

在 Linux 2.6 以后版本的 Linux 内核中，一个内核任务可以被抢占，从而提高系统的实时性。这样做最主要的优势在于，可以极大地增强系统的用户交互性，用户将会觉得鼠标单击和击键的事件得到了更快速的响应。Linux 2.6 以后的内核版本还是存在一些不可抢占的区间，如中断上下文、软中断上下文和自旋锁锁住的区间。如果给 Linux 内核打上 RT-Preempt 补丁，则中断和软中断都被线程化了，自旋锁也被互斥体替换，Linux 内核变得能支持硬实时。

### 改进的线程模型

Linux 2.6 以后版本中的线程采用 NPTL（Native POSIX Thread Library，本地 POSIX 线程库）模型，操作速度得以极大提高，相比于 Linux 2.4 内核时代的 LinuxThreads 模型，它也更加遵循 POSIX 规范的要求。NPTL 没有使用 LinuxThreads 模型中采用的管理线程，内核本身也增加了 FUTEX（Fast Userspace Mutex，快速用户态互斥体），从而减小多线程的通信开销。

### 虚拟内存的变化

从虚拟内存的角度来看，新内核融合了 r-map（反向映射）技术，显著改善虚拟内存在一定大小负载下的性能。在 Linux 2.4 中，要回收页时，内核的做法是遍历每个进程的所有 PTE 以判断该 PTE 是否与该页建立了映射，如果建立了，则取消该映射，最后无 PTE 与该页相关联后才回收该页。在 Linux 2.6 后，则建立反向映射，可以通过页结构体快速寻找到页面的映射。

### 文件系统

Linux 2.6 版内核增加了对日志文件系统功能的支持，解决了 Linux 2.4 版本在这方面的不足。Linux 2.6 版内核在文件系统上的关键变化还包括对扩展属性及 POSIX 标准访问控制的支持。ext2/ext3/ext4 作为大多数 Linux 系统默认安装的文件系统，在 Linux 2.6 版内核中增加了对扩展属性的支持，可以给指定的文件在文件系统中嵌入元数据。 

在文件系统方面，基于 B 树的 Btrfs，称为是下一代 Linux 文件系统，它在扩展性、数据一致性、多设备管理和针对 SSD 的优化等方面都优于 ext4。

## 内核的组成

### 内核源码的目录结构

Linux 内核源代码包含以下目录，可通过网站 [https://elixir.bootlin.com/](https://elixir.bootlin.com/) 浏览。

- arch：包含和硬件体系结构相关的代码，每种平台占一个相应的目录，如 i386、arm、arm64、powerpc、mips 等。Linux 内核目前已经支持 30 种左右的体系结构。在 arch 目录下，存放的是各个平台以及各个平台的芯片对 Linux 内核进程调度、内存管理、中断等的支持，以及每个具体的 SoC 和电路板的板级支持代码。
- block：块设备驱动程序 I/O 调度。
- crypto：常用加密和散列算法（如 AES、SHA 等），还有一些压缩和 CRC 校验算法。
- documentation：内核各部分的通用解释和注释。
- drivers：设备驱动程序，每个不同的驱动占用一个子目录，如 char、block、net、mtd、i2c 等。
- fs：所支持的各种文件系统，如 EXT、FAT、NTFS、JFFS2 等。
- include：头文件，与系统相关的头文件放置在 include/linux 子目录下。
- init：内核初始化代码。著名的 start_kernel() 就位于 init/main.c 文件中。
- ipc：进程间通信的代码。
- kernel：内核最核心的部分，包括进程调度、定时器等，而和平台相关的一部分代码放在 arch/*/kernel 目录下。
- lib：库文件代码。
- mm：内存管理代码，和平台相关的一部分代码放在 arch/*/mm 目录下。
- net：网络相关代码，实现各种常见的网络协议。
- scripts：用于配置内核的脚本文件。
- security：主要是一个 SELinux 的模块。
- sound：ALSA、OSS 音频设备的驱动核心代码和常用设备驱动。
- usr：实现用于打包和压缩的 cpio 等。

内核一般要做到 drivers 与 arch 的软件架构分离，驱动中不包含板级信息，让驱动跨平台。同时内核的通用部分（如 kernel、fs、ipc、net 等）则与具体的硬件（arch 和 drivers）剥离。

### 内核的组成部分

Linux 内核主要由**进程调度（SCHED）、内存管理（MM）、虚拟文件系统（VFS）、网络接口（NET）和进程间通信（IPC）**5 个子系统组成，如图所示：

<img src="https://img-blog.csdnimg.cn/direct/61cd8ad93fac4afc918d33ae0cf7486b.png" alt="image-20241024101324120" style="zoom:70%;" />

#### 进程调度

多个进程在 CPU 中“微观串行、宏观并行”的执行。进程调度处于系统的中心位置，内核中其他的子系统都依赖它，因为每个子系统都需要挂起或恢复进程。

Linux 的进程在几个状态间进行切换。在设备驱动编程中，当请求的资源不能得到满足时，驱动一般会调度其他进程执行，并使本进程进入睡眠状态，直到它请求的资源被释放，才会被唤醒而进入就绪状态。睡眠分成可中断的睡眠和不可中断的睡眠，两者的区别在于可中断的睡眠在收到信号的时候会醒。状态转化图如图：

<img src="https://img-blog.csdnimg.cn/direct/7e179aedbdb44f71acfb88d548bba0ea.png" alt="image-20241024102027520" style="zoom:70%;" />

完全处于 TASK_UNINTERRUPTIBLE 状态的进程甚至都无法被“杀死” ，所以 Linux 2.6.26 之后的内核 也存在一种 TASK_KILLABLE 的状态，它等于 TASK_WAKEKILL|TASK_UNINTERRUPTIBLE，可以响应致命信号。

在 Linux 内核中，**使用 task_struct 结构体来描述进程**。该结构体中包含描述该进程内存资源、文件系统资源、文件资源、tty 资源、信号处理等的指针。Linux 的线程采用轻量级进程模型来实现，在用户空间通过 pthread_create() API 创建线程的时候，本质上内核只是创建了一个新的 task_struct，并将新 task_struct 的所有资源指针都指向创建它的那个 task_struct 的资源指针。

绝大多数进程（以及进程中的多个线程）是由用户空间的应用创建的，当它们存在底层资源和硬件访问的需求时，会通过系统调用进入内核空间。有时候，在内核编程中，如果需要几个并发执行的任务，可以启动内核线程，这些线程没有用户空间。启动内核线程的函数如下：

```c
// #include <linux/sched/task.h>

pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
```

#### 内存管理

内存管理的主要作用是控制多个进程安全地共享主内存区域。当 CPU 提供内存管理单元（MMU）时，Linux 内存管理对于每个进程完成从虚拟内存到物理内存的转换。

一般而言，32 位处理器的 Linux 的每个进程享有 4GB 的内存空间，0~3GB 属于用户空间，3~4GB 属于内核空间，内核空间对常规内存、I/O 设备内存以及高端内存有不同的处理方式。当然，内核空间和用户空间的具体界限是可以调整的，在内核配置选项 Kernel Features → Memory split 下，可以设 置界限为 2GB 或者 3GB。

<img src="https://img-blog.csdnimg.cn/direct/6552a1294ea44f97853131dc9716abc5.png" alt="image-20241024104326004" style="zoom:75%;" />

Linux 内核的内存管理总体比较庞大，包含底层的 Buddy 算法，它用于管理每个页的占用情况，内核空间的 slab 以及用户空间的 C 库的二次管理。另外，内核也提供了页缓存的支持，用内存来缓存磁盘，per-BDI flusher 线程用于刷回脏的页缓存到磁盘。Kswapd（交换进程）则是 Linux 中用于页面回收（包括 file-backed 的页和匿名页）的内核线程，它采用最近最少使用（LRU）算法进行内存回收。

<img src="https://img-blog.csdnimg.cn/direct/b231e7e91f6a408a8f7b39ca5e463a3e.png" alt="image-20241024104439338" style="zoom:65%;" />

#### 虚拟文件系统（VFS）

Linux 虚拟文件系统隐藏了各种硬件的具体细节，为所有设备提供了统一的接口。而且，它独立于各个具体的文件系统，是对各种文件系统的一个抽象。它为上层的应用程序提供了统一的 vfs_read()、vfs_write() 等接口，并调用具体底层文件系统或者设备驱动中实现的 file_operations 结构体的成员函数。

<img src="https://img-blog.csdnimg.cn/direct/797a517415e0444f9c12a6afebf71dfb.png" alt="image-20241024104730858" style="zoom:70%;" />

#### 网络接口

网络接口提供了对各种网络标准的存取和各种网络硬件的支持。在 Linux 中网络接口可分为网络协议和网络驱动程序，网络协议部分负责实现每一种可能的网络传输协议，网络设备驱动程序负责与硬件设备通信，每一种可能的硬件设备都有相应的设备驱动程序。

<img src="https://img-blog.csdnimg.cn/direct/f31053c9c9164926a1542b337276f3fe.png" alt="image-20241024110756075" style="zoom:63%;" />

Linux 内核支持的协议栈种类较多，如 Internet、UNIX、CAN、NFC、Bluetooth、WiMAX、IrDA 等，上层的应用程序统一使用套接字接口。

#### 进程间通信

进程间通信支持进程之间的通信，Linux 支持进程间的多种通信机制，包含信号量、共享内存、消息队列、管道、UNIX 域套接字等，这些机制可协助多个进程、多资源的互斥访问、进程间的同步和消息传递。在实际的 Linux 应用中，人们更多地趋向于使用 UNIX 域套接字，而不是 System V IPC 中的消息队列等机制。

#### 组成部分的依赖关系

Linux 内核 5 个组成部分之间的依赖关系如下：

- 进程调度与内存管理之间的关系：这两个子系统互相依赖。在多程序环境下，程序要运行，则必须为之创建进程，而创建进程的第一件事情，就是将程序和数据装入内存。
- 进程间通信与内存管理的关系：进程间通信子系统要依赖内存管理支持共享内存通信机制，这种机制允许两个进程除了拥有自己的私有空间之外，还可以存取共同的内存区域。
- 虚拟文件系统与网络接口之间的关系：虚拟文件系统利用网络接口支持网络文件系统（NFS），也利用内存管理支持 RAMDISK 设备。
- 内存管理与虚拟文件系统之间的关系：内存管理利用虚拟文件系统支持交换，交换进程定期由调度程序调度，这也是内存管理依赖于进程调度的原因。当一个进程存取的内存映射被换出时，内存管理向虚拟文件系统发出请求，同时，挂起当前正在运行的进程。

除了这些依赖关系外，内核中的所有子系统还要依赖于一些共同的资源。这些资源包括所有子系统都用到的API，如分配和释放内存空间的函数、输出警告或错误消息的函数及系统提供的调试接口等。

### 内核空间与用户空间

在 Linux 系统中，内核可进行任何操作，而应用程序则被禁止对硬件的直接访问和对内存的未授权访问。

内核空间和用户空间这两个名词用来区分程序执行的两种不同状态，它们使用不同的地址空间。Linux 只能通过**系统调用**和**硬件中断**完成从用户空间到内核空间的控制转移。

## 内核的编译及加载

### 编译

既然要学习内核，首先肯定需要动手编译一个内核出来。在编译内核之前，首先下载想要编译安装的内核源代码到本地。然后就需要配置内核，可使用以下命令配置：

```bash
make config #（基于文本的最为传统的配置界面，不推荐使用）
make menuconfig #（基于文本菜单的配置界面）
make xconfig #（要求 QT 被安装）
make gconfig #（要求 GTK+ 安装）
```

Linux 内核的配置系统有三个部分组成：

- Makefile：分布在 Linux 内核源代码中，定义 Linux 内核的编译规则。
- 配置文件（Kconfig）：给用户提供配置选择的功能。
- 配置工具：包括配置命令解释器（对配置脚本中使用的配置命令进行解释）和配置用户界面（提供字符界面和图形界面）。这些配置工具使用的都是脚本语言，如用 Tcl/TK、Perl 等。

执行完配置命令以后，会生成一个 .config 配置文件，记录了哪些部分被编译入内核、哪些部分被编译为内核模块。当然一般情况下，我们可以沿用本机的设置，将其拷贝到想要想要安装的内核源代码目录，并重命名为 .config 文件：

```bash
cp -v /boot/config-`uname -r` .config
```

更完整的编译和调试内核的办法请参考文章 [https://ignotusjee.github.io/2024/10/18/Linux-Debug/](https://ignotusjee.github.io/2024/10/18/Linux-Debug/)。

另外的，在 Linux 内核中增加程序需要完成以下 3 项工作：

- 将编写的源代码复制到 Linux 内核源代码的相应目录中。
- 在目录的 Kconfig 文件中增加关于新源代码对应项目的编译配置选项。
- 在目录的 Makefile 文件中增加对新源代码的编译条目。

### Makefile

这里主要涉及内核专有的 Kbuild Makefile 编译系统。

1. 目标定义

定义哪些内容是编译并链接入内核，哪些是作为内核模块编译。

例如这段代码：

```makefile
obj-y += foo.o
```

表示需要编译 foo.c 或者 foo.s 文件得到 foo.o 并链接进内核。这是无条件编译，所以不需要 Kconfig 配置选项。

`obj-m` 表示作为内核模块编译，`obj-n` 表示不会被编译。

2. 多模块文件的定义

如果一个模块由多个文件组成，Makefile 会稍微复杂一点。这时应该采用模块名加 -y 或 -objs 后缀的形式定义模块的组成文件，例如：

```makefile
#
# Makefile for the linux ext2-filesystem routines.
#
obj-$(CONfiG_EXT2_FS) += ext2.o
ext2-y := balloc.o dir.o file.o fsync.o ialloc.o inode.o \
ioctl.o namei.o super.o symlink.o
ext2-$(CONfiG_EXT2_FS_XATTR) += xattr.o xattr_user.o xattr_trusted.o
ext2-$(CONfiG_EXT2_FS_POSIX_ACL) += acl.o
ext2-$(CONfiG_EXT2_FS_SECURITY) += xattr_security.o
ext2-$(CONfiG_EXT2_FS_XIP) += xip.o
```

模块的名字是 ext2，由 balloc.o dir.o file.o 等多个目标文件最终链接生成 ext2.o 或者 ext2.ko 的目标文件（当然 Linux 2.6 以后是 .ko）。其他的目标文件是否编译生成取决于配置文件。

3. 目录层次的迭代

当源代码比较多的时候，处于设计和美观的目的，按照目录分层次结构是有必要的。例如：

```makefile
obj-m += ext2/
```

这代表 Kbuild 会把 ext2/ 目录列入向下迭代的目标。ext2/ 目录中理应有自己的 Kbuild Makefile 的子构建系统。

### Kconfig

1. 配置选项

配置选项通过 config 关键字定义，例如：

```kconfig
config MODVERSIONS
    bool "Module versioning support"
    help
        Usually, you have to use modules compiled with your kernel.
        Saying Y here makes it ...
```

config 关键字定义新的配置选项，之后的几行代码定义了该配置选项的属性。配置选项的属性包括类型、数据范围、输入提示、依赖关系、选择关系及帮助信息、默认值等。

每个配置选项都必须指定类型，类型包括 bool、tristate、string、hex 和 int，其中 tristate 和 string 是两种基本类型，其他类型都基于这两种基本类型。类型定义后可以紧跟输入提示，下面两段代码是等价的：

```kconfig
bool “Networking support”

# 等价于

bool
prompt "Networking support"
```

输入提示使用 prompt 关键字，一般格式如下，其中可选 if 用于表示该提示的依赖关系。

```kconfig
prompt <prompt> [if <expr>]
```

默认值的格式如下。如果用户不设置对应的选项，配置选项的值就是默认值。

```kconfig
default <expr> [if <expr>]
```

依赖关系的格式如下。如果定义了多重依赖关系，它们之间用 `&&` 间隔。

```kconfig
depends on <expr>
```

依赖关系也可以应用到该菜单中所有的其他选项 （同样接受 if 表达式）内，因此下面两段脚本是等价的：

```kconfig
bool "foo" if BAR
default y if BAR


# 等价于

depends on BAR
bool "foo"
default y
```

选择关系，也成为反向依赖关系，格式如下。如果 A 选择了 B，那么 A 在被选中的情况下，B 也会自动被选中。

```kconfig
select <symbol> [if <expr>]
```

数据范围的格式为：

```kconfig
range <symbol> <symbol> [if <expr>]
```

expr 表达式定义为：

```kconfig
<expr> ::= <symbol>
            <symbol> '=' <symbol>
            <symbol> '!=' <symbol>
            '(' <expr> ')'
            '!' <expr>
            <expr> '&&' <expr>
            <expr> '||' <expr>
```

也就是说，expr 是由 symbol、两个 symbol 相等、两个 symbol 不等以及 expr 的赋值、非、与或运算构成。symbol 分为两类，一类是由菜单入口配置选项定义的非常数 symbol，另一类是作为 expr 组成部分的常数 symbol。

举个例子就明白了，如下 expr 表示依赖条件是 ARCH_R8A73A4 被选中以及 SH_DMAE 未被选中，才能出现 SHDMA_R8A73A4。

```kconfig
config SHDMA_R8A73A4
    bool y
    depends on ARCH_R8A73A4 && SH_DMAE == n
```

帮助信息的格式为，完全靠文本缩进识别结束。

```kconfig
help
# 或者：---help---
    开始
    …
    结束
```

2. 菜单结构

配置选项在菜单树结构的位置可以由两种方法决定。

第一种如下，所有处于 menu 和 endmenu 之间的配置选项都会成为 Network device support 的子菜单，而且，所有子菜单（config）选项都会继承父菜单（menu）的依赖关系。菜单 Network device support 对 NET 的依赖会加到配置选项 NETDEVICES 的依赖列表中。

```kconfig
menu "Network device support"
    depends on NET
config NETDEVICES

	...

endmenu
```

> 注：menu 后面跟的 Network device support 项仅仅是 1 个菜单，没有对应真实的配置选项，也不具备 3 种不同的状态。这是 menu 和 config 的区别。

另一种方式是通过分析依赖关系生成菜单结构。如果菜单项在一定程度上依赖于前面的选项，它就能成为该选项的子菜单。例如这里，config MODVERSIONS 直接依赖于 MODULES，只有当 MODULES 不为 n 的时候，MODVERSIONS 才可见。

```kconfig
config MODULES
    bool "Enable loadable module support"
config MODVERSIONS
    bool "Set version information on all module symbols"
    depends on MODULES
```

更详细的编写细节，请参考内核文档 Documentation 目录内的 kbuild 子目录下的 Kconfig-language.rst 和 Makefiles.rst 文件。

# Linux 内核模块

## 内核模块简介

如果我们自己想要在 Linux 内核当中添加功能，有两种方法。

1. 一是把所有需要的功能都编译到 Linux 内核中。但这样会导致生成的内核很大，并且如果需要增加或删除功能，将不得不重新编译内核。
2. 二是让内核提供某种方法，使得原本的内核并不需要包含所有的功能，而是在需要使用的时候，将代码动态的加载到内核当中。

巧了，Linux 内核就提供了第二种方法，这样的机制成为模块。模块具有如下特点：

1. 模块本身不被编译入内核映像，从而控制了内核的大小。
2. 模块一旦被加载，它就和内核中的其他部分完全一样。

以一个最简单的 Hello World 模块为例，展示一下内核模块编程的大致认识：

本程序通过内核的 Makefile + Kbuild 系统编译以后会生成 hello.ko 目标文件，通过 insmod 命令加载到内核中，rmmod 命令卸载。

```c
#include <linux/init.h>
#include <linux/module.h>


MODULE_VERSION("v1.0.0");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("DavidingPlus");
MODULE_DESCRIPTION("A Simple Hello World Module");


static int __init hello_init(void)
{
    // 内核模块中用于输出的函数是内核空间的 printk() 而不是用户空间的 printf()，printk() 的用法和 printf() 基本相似，但前者可定义输出级别。
    printk(KERN_INFO "hello: Hello World\n");


    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "hello: Goodbye World\n");
}


module_init(hello_init);
module_exit(hello_exit);
```

在 Linux 中，可通过 lsmod 命令查看系统中已加载的所有模块以及之间的依赖关系：

```bash
sudo lsmod

# Module                  Size  Used by
# ufs                    81920  0
# qnx4                   16384  0
# hfsplus               110592  0
# hfs                    61440  0
# minix                  40960  0
# ntfs                  106496  0
# msdos                  20480  0
```

lsmod 实际上是读取并分析 `/proc/modules` 文件，对应结果如下：

```bash
cat /proc/modules

# ufs 81920 0 - Live 0x0000000000000000
# qnx4 16384 0 - Live 0x0000000000000000
# hfsplus 110592 0 - Live 0x0000000000000000
# hfs 61440 0 - Live 0x0000000000000000
# minix 40960 0 - Live 0x0000000000000000
# ntfs 106496 0 - Live 0x0000000000000000
# msdos 20480 0 - Live 0x0000000000000000
```

内核中已加载模块的信息也保存在 `/sys/modules/` 目录下。加载上面的 hello.ko 以后，内核中将包含 /sys/modules/hello/ 目录，该目录的结构图如下：

```markdown
.
├── coresize
├── holders
├── initsize
├── initstate
├── notes
│   ├── .note.gnu.build-id
│   └── .note.Linux
├── refcnt
├── sections
│   ├── .exit.data
│   ├── .exit.text
│   ├── .gnu.linkonce.this_module
│   ├── .init.data
│   ├── .init.text
│   ├── .note.gnu.build-id
│   ├── .note.Linux
│   ├── .rodata.str1.1
│   ├── .strtab
│   ├── .symtab
│   └── .text
├── srcversion
├── taint
├── uevent
└── version

3 directories, 21 files
```

除了 insmod 命令，还有一个 modprobe 命令。与 insmod 不同的是，**modprobe 在加载某模块时，会同时加载该模块所依赖的其他模块**。使用 modprobe 命令加载的模块若以 `modprobe -r filename` 的方式卸载，将同时卸载其依赖的模块。

模块之间的依赖关系存放在根文件系统的 `/lib/modules/<kernel-version>/modules.dep` 文件中，实际上是在整体编译内核的时候由 depmod 工具生成的，它的格式非常简单：

```markdown
kernel/arch/x86/events/intel/intel-cstate.ko:
kernel/arch/x86/events/rapl.ko:
kernel/arch/x86/kernel/cpu/mce/mce-inject.ko:
kernel/arch/x86/kernel/msr.ko:
kernel/arch/x86/kernel/cpuid.ko:
kernel/arch/x86/crypto/twofish-x86_64.ko: kernel/crypto/twofish_common.ko
kernel/arch/x86/crypto/twofish-x86_64-3way.ko: kernel/arch/x86/crypto/twofish-x86_64.ko kernel/crypto/twofish_common.ko
kernel/arch/x86/crypto/twofish-avx-x86_64.ko: kernel/crypto/crypto_simd.ko kernel/crypto/cryptd.ko kernel/arch/x86/crypto/twofish-x86_64-3way.ko kernel/arch/x86/crypto/twofish-x86_64.ko kernel/crypto/twofish_common.ko
...
```

使用 modinfo 命令可以获得模块的信息，包括模块作者、模块的说明、模块所支持的参数以及 vermagic：

```bash
modinfo hello.ko

# filename:       /home/lzx0626/DavidingPlus/linux-kernel-learning/build/linux/x86_64/debug/hello.ko
# description:    A Simple Hello World Module
# author:         DavidingPlus
# license:        Dual BSD/GPL
# version:        1.0.0
# srcversion:     533BB7E5866E52F63B9ACCB
# depends:
# retpoline:      Y
# name:           hello
# vermagic:       5.15.167 SMP mod_unload modversions
```

## 内核模块程序结构

一个 Linux 内核模块主要由以下几部分组成：

1. 模块加载函数

当通过 insmod 或 modprobe 命令加载内核模块时，模块的加载函数会自动被内核执行，完成本模块的相关初始化工作。

2. 模块卸载函数

当通过 rmmod 命令卸载某模块时，模块的卸载函数会自动被内核执行，完成与模块卸载函数相反的功能。

3. 模块许可证声明

许可证（LICENSE）声明描述内核模块的许可权限，如果不声明，模块被加载时，将收到内核被污染（Kernel Tainted）的警告。

4. 模块参数（可选）

模块参数是模块被加载的时候可以传递给它的值，它本身对应模块内部的全局变量。

5. 模块导出符号（可选）

内核模块可以导出的符号（symbol，对应于函数或变量）。若导出，其他模块则可以使用本模块中的变量或函数。

6. 模块作者等信息声明（可选）

## 模块加载函数

Linux 内核模块加载函数一般以 __init 标识声明，例如：

```c
static int __init initialization_function(void)
{
	...
}


module_init(initialization_function);
```

模块加载函数 initialization_function(void) 在内核被加载的时候，通过宏 module_init 进行调用。加载函数应该有一个返回值 int，成功返回 0，失败返回一个错误编码。该错误编码是一个接近 0 的负数，在 `<linux/errno.h>` 中定义，如 -ENODEV、-ENOMEM 等。强烈建议返回有意义的错误编码宏，因为这样用户可以通过 perror() 的方法将其转化为有意义的字符串。

在 Linux 内核程序代码中，可以通过 request_module(const char*fmt, …) 函数加载内核模块，例如：

```c
request_module(module_name);
```

在 Linux 内核中，所有标识为 __init 的函数如果直接编译进入内核，那么会成为内核镜像的一部分，连接的时候会放在 .init.text 区段内。

```c
#define __init		__section(".init.text") __cold  __latent_entropy __noinitretpoline __nocfi
```

所有的 `__init` 函数在区段 .initcall.init 中还保存了一份函数指针，在初始化时内核会通过这些函数指针调用这些 `__init` 函数，并在初始化完成后，释放 `__init` 区段（包括 .init.text、.initcall.init 等）的内存。

除了函数以外，数据也可以被定义为 __initdata。表示**只是初始化阶段需要的数据，初始化完成以后，内核会自动释放他们占用的内存。**

## 模块卸载函数

Linux 内核卸载加载函数一般以 __exit 标识声明，例如：

```c
static void __exit cleanup_function(void)
{
    ...
}


module_exit(cleanup_function);
```

模块卸载函数在模块卸载的时候执行，不返回任何值。通过宏 module_exit 的形式指定调用。通常来讲会完成模块加载函数相反的功能。

与模块加载函数对应的，可以使用 `__exit` 修饰模块卸载函数。如果告诉内核相关模块直接被编译进内核。这种情况下 `__exit` 修饰的模块卸载函数会被忽略，不会被链进最后的镜像。因为既然都编译进内核，被内置了，那么肯定不会被卸载了。

另外对应的，数据也可以被定义为 __exitdata。

## 模块参数

可以使用宏 `module_param(参数名，参数类型，参数读/写权限)` 为模块指定一个参数。宏函数参数分别对应模块参数名、参数类型以及参数读/写权限。

在装载内核模块的时候，可以向模块提供参数，格式如下。多个参数传递用空格隔开。如果不传递参数，将使用模块内定义的默认值。如果模块被内置，则无法被 insmod ，但是 bootloader 可以通过在 bootargs 里设置`“模块名.参数名=值”`的形式给该内置模块传递参数。

```bash
sudo insmod <param_name>=<param_value>
```

参数类型可以是 byte、short、ushort、int、uint、long、ulong、charp（字符指针）、bool 或 invbool（bool 的反）。模块在编译的时候会检查传入变量的类型和传入的类型是否一致。

另外，模块也可以拥有参数数组，通过宏 `module_param_array(数组名，数组类型，数组长，参 数读/写权限)` 指定，用法类似。

> 传递数组参数的时候需要注意一下格式。参数之间用 `,` 隔开，并且不加大括号，参数之间没有空格，例如传递一个 int 数组：
>
> ```bash
> sudo insmod hello.ko array=1,2,3,4
> ```

如果指定了模块参数，**并且参数读/写权限不为 0**，在装入模块以后会在 `/sys/modules/<module_name>` 下出现 parameters 子目录，用于记录内核模块的参数。

```markdown
.
...
├── parameters
│   ├── helloExitParam
│   └── helloInitParam
...
```

例如这里，就记录了内核模块参数 helloInitParam 和 helloExitParam，使用 cat 命令能够得到他们分别记录的值，其中数组的表示方法和上面提到的一致。

<img src="https://img-blog.csdnimg.cn/direct/5aa7af05728a4dcd804e5e8f16f2f25b.png" alt="image-20241105100931281" style="zoom:75%;" />

实例程序如下：

```c
#include <linux/init.h>
#include <linux/module.h>


MODULE_VERSION("1.0.0");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("DavidingPlus");
MODULE_DESCRIPTION("A Simple Hello World Module");


static int helloInitData __initdata = -114514;

static const char *helloExitData __exitdata = "foo";

static char *helloInitParam = "gee";
module_param(helloInitParam, charp, S_IRUGO);

static int helloExitParam[2] = {-10086, 10086};
int helloExitParamSize = sizeof(helloExitParam) / sizeof(int);
module_param_array(helloExitParam, int, &helloExitParamSize, S_IRUGO);


static int __init hello_init(void)
{
    printk(KERN_INFO "hello: Hello World %d %s\n", helloInitData, helloInitParam);


    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "hello: Goodbye World %s %d %d\n", helloExitData, helloExitParam[0], helloExitParam[1]);
}


module_init(hello_init);
module_exit(hello_exit);
```

## 导出符号

导出符号即内核可以将函数导出被其他模块使用。Linux 中 `/proc/kallsyms` 文件对应内核符号表，记录了符号以及符号所在的内存地址。

使用如下宏函数导出符号到内核符号表中：

```c
EXPORT_SYMBOL(符号名);

// EXPORT_SYMBOL_GPL() 只适用于包含 GPL 许可权的模块。
EXPORT_SYMBOL_GPL(符号名);
```

例如这里导出函数 add() 和 sub()：

```c
...

int add(int a, int b) { return a + b; }

int sub(int a, int b) { return a - b; }


EXPORT_SYMBOL(add);
EXPORT_SYMBOL(sub);

...
```

装载好内核模块以后查看 /proc/kallsyms 文件能得到对应的符号信息：

```bash
cat /proc/kallsyms | grep hello

# ...
# 0000000000000000 r __kstrtab_add        [hello]
# 0000000000000000 r __kstrtabns_add      [hello]
# 0000000000000000 r __ksymtab_add        [hello]
# 0000000000000000 r __kstrtab_sub        [hello]
# 0000000000000000 r __kstrtabns_sub      [hello]
# 0000000000000000 r __ksymtab_sub        [hello]
# 0000000000000000 T add  [hello]
# 0000000000000000 T sub  [hello]
```

## 模块声明与描述

在 Linux 内核模块中，我们可以用 MODULE_AUTHOR、MODULE_DESCRIPTION、MODULE_VERSION、MODULE_DEVICE_TABLE、MODULE_ALIAS 分别声明模块的作者、描述、版本、设备表和别名，例如：

```c
MODULE_AUTHOR(author);
MODULE_DESCRIPTION(description);
MODULE_VERSION(version_string);
MODULE_DEVICE_TABLE(table_info);
MODULE_ALIAS(alternate_name);
```

## 模块的使用计数

Linux 2.4 内核中，模块自身通过 MOD_INC_USE_COUNT、MOD_DEC_USE_COUNT 宏来管理自己被使用的计数。

Linux 2.6 以后的内核提供了模块计数管理接口 try_module_get(&module) 和 module_put(&module)，从而取代 Linux 2.4 内核中的模块使用计数管理宏。模块的使用计数一般不必由模块自身管理，而且模块计数管理还考虑了 SMP 与 PREEMPT 机制的影响。

```c
// 用于增加模块使用计数。若返回为0，表示调用失败，希望使用的模块没有被加载或正在被卸载中。
int try_module_get(struct module *module);

// 用于减少模块使用计数。
void module_put(struct module *module);
```

Linux 2.6 以后的内核为不同类型的设备定义了 struct module *owner 域，用来指向管理此设备的模块。当开始使用某个设备时，内核使用 try_module_get(dev->owner) 去增加管理此设备的 owner 模块的使用计数；当不再使用此设备时，内核使用 module_put(dev->owner) 减少对管理此设备的管理模块的使用计数。这样，当设备在使用时，管理此设备的模块将不能被卸载。只有当设备不再被使用时，模块才允许被卸载。

# Linux 文件系统与设备文件

Linux 之下一些皆文件，并且由于字符设备和块设备都良好的体现了一切皆文件的思想，因此 Linux 文件系统与设备文件的基础知识就非常重要了。

## 文件操作

### 系统调用

1. 创建

```c
int creat(const char *pathname, mode_t mode);
```

参数 mode 代表创建文件的权限，和 umask 一起共同决定文件的最终权限（`mode & ~umask`）。umask 代表文件创建的时候需要去掉的一些权限，让最终的权限合理一些。可通过 umake() 系统调用改变：

```c
mode_t umask(mode_t mask);
```

umask() 函数将 umask 设置为 newmask，然后返回旧的 umask，它只影响读、写和执行权限。

2. 打开

```c
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

flags 参数表示文件的打开标志，可以是如下的一个或者几个的组合：

<img src="https://img-blog.csdnimg.cn/direct/0f8f0abc88214db386d1501b89c64b9d.png" alt="image-20241105144109024" style="zoom:80%;" />

其中，O_RDONLY、O_WRONLY、O_RDWR 三者是互斥的，只可选择其一。

如果使用了 O_CREAT 标志，还需要提供第三个参数 mode 来表示新文件的权限，同前面的 creat() 函数。mode 权限如下图：

<img src="https://img-blog.csdnimg.cn/direct/dbb4cddec38941e4adcd0e41f27a9795.png" alt="image-20241105144550547" style="zoom:70%;" />

关于权限 mode，举个例子就明白了。Linux 下的文件权限是一个八进制数，例如 0777。3 个 7 分别表示对不同的用户（所有者，组成员，其他用户的权限）的权限。每一个都是 3 位 ，第一位表示读 R，第二位表示写 W，第三位表示可执行 X，7 对应就是 111 全有。

最后，open() 函数返回一个进程唯一的文件描述符，对文件的所有操作都通过该文件描述符实现。

3. 读写

打开文件获得文件描述符以后，通过 read() 和 write() 函数对文件进行读写。

```c
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
```

参数 buf 为存放读取数据的内存的地址指针，count 为长度。

函数 read() 从文件描述符 fd 所指定的文件中读取 count 个字节到 buf 所指向的内存中，返回值为实际读取的字节数。

函数 write() 实现把 count 个字节从 buf 中写到文件描述符 fd 所指向的文件中，返回值为实际写入的字节数。

4. 定位

对于随机文件，可以指定随机位置进行读写，使用 lseek() 函数：

```c
off_t lseek(int fd, off_t offset, int whence);
```

lseek() 函数将文件指针相对 whence 位置移动 offset 个字节，操作成功后返回文件指针现在的位置。

参数 whence 可使用如下值。

- SEEK_SET：相对文件开头。
- SEEK_CUR：相对文件读写指针的当前位置。
- SEEK_END：相对文件末尾。

当然 lseek() 的 offset 可以取负值，表示往前移动。一般可以通过此函数获得文件的大小：

```c
int size = lseek(fd, 0, SEEK_END);
```

5. 关闭

有始有终，在结束对文件的操作以后，需要关闭文件，释放文件描述符。通过 close() 实现：

```c
int close(int fd);
```

6. 实例 Demo

编写一个程序，在当前目录下创建用户可读写文件 hello.txt，在其中写入"He\nllo, software weekly."，关闭该文件。再次打开该文件，读取其中的内容并输出在屏幕上。最后删除该文件。

比较简单，cpp 程序如下：

```cpp
// 使用系统调用。
#include <iostream>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>


// 故意搞一个不能一次读完的数组。
#define MAX_SIZE 10


int main()
{
    const char *filePath = "hello.txt";
    const char *writeStr = "He\nllo, software weekly.";


    int fd = open(filePath, O_RDWR | O_CREAT | O_TRUNC, 0755);
    if (-1 == fd)
    {
        perror("open");


        return -1;
    }

    write(fd, writeStr, strlen(writeStr));

    close(fd);


    int fd2 = open(filePath, O_RDONLY);
    if (-1 == fd2)
    {
        perror("open");


        return -1;
    }


    char readStr[MAX_SIZE] = {0};
    ssize_t len = 0;

    // 为保证读完，需循环读取。
    while (true)
    {
        bzero(readStr, sizeof(readStr));

        len = read(fd2, readStr, sizeof(readStr) - 1);
        if (-1 == len)
        {
            perror("read");


            return -1;
        }

        std::cout << readStr;

        if (0 == len) break;
    }

    std::cout << std::endl;

    close(fd2);


    unlink(filePath);


    return 0;
}
```

正确的输出预期应该如图：

![image-20241105160211266](https://img-blog.csdnimg.cn/direct/70e36518faf34bd4b91e1471d68d207e.png)

### 标准 C 库函数

1. 创建和打开

这两个操作都使用 fopen() 函数：

```c
FILE *fopen(const char *pathname, const char *mode);
```

mode 表示打开模式，支持的模式如图所示：

<img src="https://img-blog.csdnimg.cn/59ee39f0f9724e5aa9f7d579eae550e4.png" style="zoom:80%;" />

其中，b 用于区分二进制文件和文本文件，这一点在 DOS、Windows 系统中是有区分的，但 Linux 不区分二进制文件和文本文件。

2. 读写

标准 C 库的 IO 函数会自动在用户层分配缓冲区，而系统调用不具有用户层缓冲区。二者都具有内核层的缓冲区。其中缓冲分为三种，**全缓冲、行缓冲和无缓冲**。

罗列部分 API 接口如下：

```cpp
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int fputs(const char *s, FILE *stream);
int fprintf(FILE *stream, const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
```

另外，标准 C 库函数还提供了读写过程的定位能力，包括：

```cpp
int fgetpos(FILE *stream, fpos_t *pos);
int fsetpos(FILE *stream, const fpos_t *pos);
int fseek(FILE *stream, long offset, int whence);
```

更多的具体用法和细节请参考另一篇博客 [https://blog.davidingplus.cn/posts/9e44c3b9.html](https://blog.davidingplus.cn/posts/9e44c3b9.html) 的第五章。

3. 关闭

同上，是非常简单的操作。

```cpp
int fclose(FILE *stream);
```

4. 实例 Demo

将前面的实例 Demo 换成标准 C 库的实现方法如下：

```cpp
// 使用标准 C 库函数。
#include <iostream>
#include <cstring>


#define MAX_SIZE 10


int main()
{
    const char *filePath = "hello.txt";
    const char *writeStr = "He\nllo, software weekly.";

    FILE *file = fopen(filePath, "w");
    if (!file)
    {
        perror("fopen");


        return -1;
    }

    fputs(writeStr, file);

    fclose(file);


    FILE *file2 = fopen(filePath, "r");
    if (!file2)
    {
        perror("fopen");


        return -1;
    }

    char readStr[MAX_SIZE] = {0};

    while (true)
    {
        bzero(readStr, sizeof(readStr));

        fgets(readStr, sizeof(readStr) - 1, file2);
        if (ferror(file2))
        {
            perror("fgets");


            return -1;
        }

        std::cout << readStr;

        if (feof(file2)) break;
    }

    std::cout << std::endl;

    fclose(file2);


    remove(filePath);


    return 0;
}
```

同样输出和上面一样的结果。

## 文件系统

### 目录结构

进入 Linux 根目录（即 `/`，Linux 文件系统的入口，也是处于最高一级的目录），运行 `ls -la` 命令，包含以下目录：

1. /bin

包含基本命令，如 ls、cp、mkdir 等，这个目录中的文件都是可执行的。

2. /sbin

包含系统命令，如 modprobe、hwclock、ifconfig 等，大多是涉及系统管理的命令，这个目录中的文件都是可执行的。

3. /dev

设备文件存储目录，应用程序通过对这些文件的读写和控制以访问实际的设备。

4. /etc

系统配置文件的所在地，一些服务器的配置文件也在这里，如用户账号及密码配置文件。busybox 的启动脚本也存放在该目录。

5. /lib

系统库文件存放目录等。

6. /mnt

这个目录一般是用于存放挂载储存设备的挂载目录，比如含有 cdrom 等目录。可以参看 /etc/fstab 的定义。有时我们可以让系统开机自动挂载文件系统，并把挂载点放在这里。

7. /opt

opt 是可选的意思，有些软件包会被安装在这里。

8. /proc

操作系统运行时，进程及内核信息（比如 CPU、硬盘分区、内存信息等）存放在这里。/proc 目录为伪文件系统 proc 的挂载目录，proc 并不是真正的文件系统，它存在于内存之中。

9. /tmp

用户运行程序的时候，有时会产生临时文件，/tmp 用来存放临时文件。

10. /usr

这个是系统存放程序的目录，比如用户命令、用户库等。

11. /var

var 表示的是变化的意思，这个目录的内容经常变动，如 /var/log/ 目录被用来存放系统日志。

12. /sys

Linux 2.6 以后的内核所支持的 sysfs 文件系统被映射在此目录上。Linux 设备驱动模型中的总线、驱动 和设备都可以在 sysfs 文件系统中找到对应的节点。当内核检测到在系统中出现了新设备后，内核会在 sysfs 文件系统中为该新设备生成一项新的记录。

### 文件系统与设备驱动

如图所示是 Linux 下虚拟文件系统、磁盘/Flash 文件系统及一般的设备文件与设备驱动程序之间的关系。

<img src="https://img-blog.csdnimg.cn/direct/82d58b796bbf4354857488563e36c251.png" alt="image-20241106093412285" style="zoom: 70%;" />

应用程序和 VFS 之间的接口是系统调用，而 VFS 与文件系统以及设备文件之间的接口是 file_operations 结构体成员函数，这个结构体包含对文件进行打开、关闭、读写、控制的一系列成员函数。

<img src="https://img-blog.csdnimg.cn/direct/9027b6a03dba4d2c9935f01074419824.png" alt="image-20241106093650313" style="zoom:75%;" />

字符设备的上层没有类似于磁盘 ext2 等文件系统，所以字符设备的 file_operations 成员函数就直接由设备驱动提供了，并且是字符驱动的核心部分。

块设备有两种访问方法，一种方法是不通过文件系统直接访问裸设备，在 Linux 内核实现了统一的 def_blk_fops 这一 file_operations，它的源代码位于 fs/block_dev.c，所以当我们运行类似于 `dd if=/dev/sdb1of=sdb1.img` 的命令把整个 /dev/sdb1 裸分区复制到 sdb1.img 的时候，内核走的是 def_blk_fops 这个 file_operations；另外一种方法是通过文件系统来访问块设备，file_operations 的实现则位于文件系统内，文件系统会把针对文件的读写转换为针对块设备原始扇区的读写。ext2、fat、Btrfs 等文件系统中会实现针对 VFS 的 file_operations 成员函数，设备驱动层将看不到 file_operations 的存在。

#### file 结构体

file 结构体代表一个打开的文件。系统中每个打开的文件在内核空间都会对应一个 struct file。它由内核在打开文件的时候创建，并传递给文件上进行操作的任何函数，在文件的所有实例关闭以后，内核会释放这个数据结构。一般将 struct file 的指针命名为 file 或者 filp。

在内核 5.15 版本中如下定义：

```c
struct file {
	union {
		struct llist_node	fu_llist;
		struct rcu_head 	fu_rcuhead;
	} f_u;
	struct path		f_path;
	struct inode		*f_inode;	/* cached value */
	const struct file_operations	*f_op; // 和文件关联的操作

	/*
	 * Protects f_ep, f_flags.
	 * Must not be taken from IRQ context.
	 */
	spinlock_t		f_lock;
	enum rw_hint		f_write_hint;
	atomic_long_t		f_count;
	unsigned int 		f_flags; // 文件标志，如 O_RDONLY、O_NONBLOCK、O_SYNC 等
	fmode_t			f_mode; // 文件读/写模式，如 FMODE_READ、FMODE_WRITE 等
	struct mutex		f_pos_lock;
	loff_t			f_pos; // 当前读写位置
	struct fown_struct	f_owner;
	const struct cred	*f_cred;
	struct file_ra_state	f_ra;

	u64			f_version;
#ifdef CONFIG_SECURITY
	void			*f_security;
#endif
	/* needed for tty driver, and maybe others */
	void			*private_data; // 文件私有数据

#ifdef CONFIG_EPOLL
	/* Used by fs/eventpoll.c to link all the hooks to this file */
	struct hlist_head	*f_ep;
#endif /* #ifdef CONFIG_EPOLL */
	struct address_space	*f_mapping;
	errseq_t		f_wb_err;
	errseq_t		f_sb_err; /* for syncfs */
} __randomize_layout
  __attribute__((aligned(4)));	/* lest something weird decides that 2 is OK */
```

文件读/写模式 f_mode、标志 f_flags 都是设备驱动关心的内容。私有数据指针 private_data 在设备驱动中被广泛应用，大多被指向设备驱动自定义以用于描述设备的结构体。

#### inode 结构体

inode 结构体包含文件访问权限、属主、组、大小、生成时间、访问时间、最后修改时间等信息。它是 Linux 管理文件系统的最基本单位，也是文件系统连接任何子目录、文件的桥梁。定义如下：

```c
struct inode {
	umode_t			i_mode; // inode 的权限
	unsigned short		i_opflags;
	kuid_t			i_uid; // inode 所有者 id
	kgid_t			i_gid; // inode 所属的群组 id
	unsigned int		i_flags;

#ifdef CONFIG_FS_POSIX_ACL
	struct posix_acl	*i_acl;
	struct posix_acl	*i_default_acl;
#endif

	const struct inode_operations	*i_op;
	struct super_block	*i_sb;
	struct address_space	*i_mapping;

#ifdef CONFIG_SECURITY
	void			*i_security;
#endif

	/* Stat data, not accessed from path walking */
	unsigned long		i_ino;
	/*
	 * Filesystems may only read i_nlink directly.  They shall use the
	 * following functions for modification:
	 *
	 *    (set|clear|inc|drop)_nlink
	 *    inode_(inc|dec)_link_count
	 */
	union {
		const unsigned int i_nlink;
		unsigned int __i_nlink;
	};
	dev_t			i_rdev; // 若是设备文件，此字段会记录设备的设备号
	loff_t			i_size; // inode 代表的文件大小
	struct timespec64	i_atime; // inode 最近一次的存取时间
	struct timespec64	i_mtime; // inode 最近一次的修改时间
	struct timespec64	i_ctime; // inode 的产生时间
	spinlock_t		i_lock;	/* i_blocks, i_bytes, maybe i_size */
	unsigned short          i_bytes;
	u8			i_blkbits;
	u8			i_write_hint;
	blkcnt_t		i_blocks; // inode 使用的 block 数，一个 block 为 512 个字节

#ifdef __NEED_I_SIZE_ORDERED
	seqcount_t		i_size_seqcount;
#endif

	/* Misc */
	unsigned long		i_state;
	struct rw_semaphore	i_rwsem;

	unsigned long		dirtied_when;	/* jiffies of first dirtying */
	unsigned long		dirtied_time_when;

	struct hlist_node	i_hash;
	struct list_head	i_io_list;	/* backing dev IO list */
#ifdef CONFIG_CGROUP_WRITEBACK
	struct bdi_writeback	*i_wb;		/* the associated cgroup wb */

	/* foreign inode detection, see wbc_detach_inode() */
	int			i_wb_frn_winner;
	u16			i_wb_frn_avg_time;
	u16			i_wb_frn_history;
#endif
	struct list_head	i_lru;		/* inode LRU list */
	struct list_head	i_sb_list;
	struct list_head	i_wb_list;	/* backing dev writeback list */
	union {
		struct hlist_head	i_dentry;
		struct rcu_head		i_rcu;
	};
	atomic64_t		i_version;
	atomic64_t		i_sequence; /* see futex */
	atomic_t		i_count;
	atomic_t		i_dio_count;
	atomic_t		i_writecount;
#if defined(CONFIG_IMA) || defined(CONFIG_FILE_LOCKING)
	atomic_t		i_readcount; /* struct files open RO */
#endif
	union {
		const struct file_operations	*i_fop;	/* former ->i_op->default_file_ops */
		void (*free_inode)(struct inode *);
	};
	struct file_lock_context	*i_flctx;
	struct address_space	i_data;
	struct list_head	i_devices;
	union {
		struct pipe_inode_info	*i_pipe;
		struct cdev		*i_cdev; // 若是字符设备，对应其 cdev 结构体指针
		char			*i_link;
		unsigned		i_dir_seq;
	};

	__u32			i_generation;

#ifdef CONFIG_FSNOTIFY
	__u32			i_fsnotify_mask; /* all events this inode cares about */
	struct fsnotify_mark_connector __rcu	*i_fsnotify_marks;
#endif

#ifdef CONFIG_FS_ENCRYPTION
	struct fscrypt_info	*i_crypt_info;
#endif

#ifdef CONFIG_FS_VERITY
	struct fsverity_info	*i_verity_info;
#endif

	void			*i_private; /* fs or device private pointer */
} __randomize_layout;
```

对于表示设备文件的 inode 结构，i_rdev 字段包含设备编号。**Linux 内核设备编号分为主设备编号和次设备编号。设备号用 dev_t 类型表示，实际上是 `unsigned int`。前者为 dev_t 的高 12 位，后者为 dev_t 的低 20 位。**使用如下函数从 inode 获得主设备号和次设备号：

```c
static inline unsigned iminor(const struct inode *inode)
{
	return MINOR(inode->i_rdev);
}

static inline unsigned imajor(const struct inode *inode)
{
	return MAJOR(inode->i_rdev);
}
```

查看 /proc/devices 文件可以看到当前系统注册的设备，第一列为主设备号，第二列为设备名，例如：

```bash
cat /proc/devices

# Character devices:
#   1 mem
#   4 /dev/vc/0
#   4 tty
#   4 ttyS
#   5 /dev/tty
#   5 /dev/console
#   5 /dev/ptmx
#   5 ttyprintk
#   7 vcs
#  10 misc
#  13 input
#  21 sg
#  29 fb
#  89 i2c
# 108 ppp
# 128 ptm
# 136 pts
# 180 usb
# 189 usb_device
# 202 cpu/msr
# 204 ttyMAX
# 226 drm
# 241 aux
# 242 cec
# 243 hidraw
# 244 vfio
# 245 bsg
# 246 watchdog
# 247 ptp
# 248 pps
# 249 rtc
# 250 dax
# 251 dimmctl
# 252 ndctl
# 253 tpm
# 254 gpiochip

# Block devices:
#   7 loop
#   8 sd
#   9 md
#  11 sr
#  65 sd
#  66 sd
#  67 sd
#  68 sd
#  69 sd
#  70 sd
#  71 sd
# 128 sd
# 129 sd
# 130 sd
# 131 sd
# 132 sd
# 133 sd
# 134 sd
# 135 sd
# 253 device-mapper
# 254 mdp
# 259 blkext
```

查看 /dev 目录可以查看系统中在上述注册的设备上建立的设备文件。一个注册的设备可以有多个设备文件，日期前面的两列分别对应主设备号和次设备号。

主设备号是与驱动对应的概念。同一类设备一般使用相同的主设备号，不同类设备一般使用不同的主设备号（但是不排除在同一主设备号下包含有一定差异的设备）。**因为同一驱动可支持多个同类设备，因此用次设备号来描述使用该驱动的设备的序号**。序号一般从 0 开始。

## devfs

devfs（设备文件系统）是 Linux 内核 2.4 版本引入的，使得设备驱动程序能自主地管理自己的设备文件。具体来讲，有以下优点：

1. 可以通过程序在设备初始化的时候在 /dev 目录下创建设备文件，卸载设备的时候删除。
2. 设备驱动程序可以指定设备名、所有者和权限位，用户空间程序仍可以修改所有者和权限位。
3. 不再需要为设备驱动程序分配主设备号以及处理次设备号，在程序中可以直接给 register_chrdev() 传递 0 主设备号以获得可用的主设备号，并在 devfs_register() 中指定次设备号。

## udev

### udev 和 devfs 的区别

尽管 devfs 有很多的优点，但在 Linus 2.6 中，devfs 被认为是过时的方法，并最终被抛弃了，udev 取代了它。

Linux 设计中强调的一个基本观点是机制和策略的分离。机制是做某样事情的固定步骤、方法，而策略就是每一个步骤所采取的不同方式。机制是相对固定的，而每个步骤采用的策略是不固定、灵活的。在 Linux 内核中，应该实现机制而非策略。

例如，Linux 提供 API 可以让人把线程的优先级调高或者调低，或者调整调度策略为 SCHED_FIFO 什么的，但是 Linux 内核本身却不管谁高谁低。提供 API 属于机制，谁高谁低这属于策略，所以应该是应用程序自己去告诉内核要高或低，而内核不管这些杂事。属于策略的东西应该被移到用户空间中，谁爱给哪个设备创建什么名字或者想做更多的处理，谁自己去设定。内核只管把这些信息告诉用户就行了。这就是位于内核空间的 devfs 应该被位于用户空间的 udev 取代的原因，应该 devfs 管了一些它实际上不该管的事情。

udev 是 Linux 系统下的一套**设备管理系统**，能在 /dev 目录下动态创建和删除设备节点。

**udev 完全在用户态工作。它利用设备加入或移除时内核所发送的热插拔事件（Hotplug Event）来工作。在热插拔时，设备的详细信息会由内核通过 netlink 套接字发送出来，发出的事情叫 uevent。**udev 的设备命名策略、权限控制和事件处理都是在用户态下完成的，它利用从内核收到的信息来进行创建设备文件节点等工作。对于冷插拔的设备，在开机的时候就已经存在，在 udev 启动之前就已被插入。Linux 下提供了 sysfs 下面一个 uevent 节点。可以往该节点写一个 add，导致内核重新发送 netlink，之后 udev 就可以收到冷插拔的 netlink 消息了。

udev 和 devfs 另一个区别在于：采用 devfs，当一个并不存在的 /dev 节点**被打开**的时候，devfs 能自动加载对应的驱动，而 udev 则不这么做。udev 的设计者认为 Linux 应该在设备**被发现**的时候加载驱动模块，而不是当它被访问的时候。系统中所有的设备都应该产生热插拔事件并加载恰当的驱动，而不是在设备被打开的时候。udev 能注意到这点并且为它创建对应的设备节点。

### sysfs 文件系统与 Linux 设备模型

Linux 2.6 以后的内核引入了 sysfs 文件系统，是一个虚拟文件系统，可以产生**包括所有系统硬件的层级视图**，与**提供进程和状态信息**的 proc 非常类似。

sysfs 把连接在系统上的设备和总线组织成为一个分级的文件。它们可以由用户空间存取，向用户空间导出内核数据结构以及它们的属性。sysfs 的一个目的就是展示设备驱动模型中各组件的层次关系。

其目录结构类似如下。block 目录包含所有的块设备；devices 目录包含系统所有的设备，并根据设备挂载的总线类型组织成层次结构；bus 目录包含系统中所有的总线类型；class 目录包含系统中的设备类型，如网卡设备、声卡设备、输入设备等。

<img src="https://img-blog.csdnimg.cn/direct/2814968d5df14815936dfd8e0a3f636d.png" alt="image-20241107100021716" style="zoom:75%;" />

在 /sys/bus/ 的 pci 等子目录下，又会再分出 drivers 和 devices 目录，而 devices 目录中的文件是对 /sys/devices/ 目录中文件的符号链接。同样地，/sys/class/ 目录下也包含许多对 /sys/devices/ 下文件的链接。Linux 设备模型与设备、驱动、总线和类的现实状况是直接对应的，如图所示：

<img src="https://img-blog.csdnimg.cn/direct/bd429fa34bba4bccb6bea45b80e1e0a1.png" alt="image-20241107100653352" style="zoom:70%;" />

**在 Linux 内核中，使用 bus_type、device_driver、device 来描述总线、驱动和设备。**其定义如下：

```c
// include/linux/device/bus.h
struct bus_type {
	const char		*name;
	const char		*dev_name;
	struct device		*dev_root;
	const struct attribute_group **bus_groups;
	const struct attribute_group **dev_groups;
	const struct attribute_group **drv_groups;

	int (*match)(struct device *dev, struct device_driver *drv);
	int (*uevent)(struct device *dev, struct kobj_uevent_env *env);
	int (*probe)(struct device *dev);
	void (*sync_state)(struct device *dev);
	void (*remove)(struct device *dev);
	void (*shutdown)(struct device *dev);

	int (*online)(struct device *dev);
	int (*offline)(struct device *dev);

	int (*suspend)(struct device *dev, pm_message_t state);
	int (*resume)(struct device *dev);

	int (*num_vf)(struct device *dev);

	int (*dma_configure)(struct device *dev);

	const struct dev_pm_ops *pm;

	const struct iommu_ops *iommu_ops;

	struct subsys_private *p;
	struct lock_class_key lock_key;

	bool need_parent_lock;
};

// include/linux/device/driver.h
struct device_driver {
	const char		*name;
	struct bus_type		*bus;

	struct module		*owner;
	const char		*mod_name;	/* used for built-in modules */

	bool suppress_bind_attrs;	/* disables bind/unbind via sysfs */
	enum probe_type probe_type;

	const struct of_device_id	*of_match_table;
	const struct acpi_device_id	*acpi_match_table;

	int (*probe) (struct device *dev);
	void (*sync_state)(struct device *dev);
	int (*remove) (struct device *dev);
	void (*shutdown) (struct device *dev);
	int (*suspend) (struct device *dev, pm_message_t state);
	int (*resume) (struct device *dev);
	const struct attribute_group **groups;
	const struct attribute_group **dev_groups;

	const struct dev_pm_ops *pm;
	void (*coredump) (struct device *dev);

	struct driver_private *p;
};

// include/linux/device.h
struct device {
	struct kobject kobj;
	struct device		*parent;

	struct device_private	*p;

	const char		*init_name; /* initial name of the device */
	const struct device_type *type;

	struct bus_type	*bus;		/* type of bus device is on */
	struct device_driver *driver;	/* which driver has allocated this
					   device */
	void		*platform_data;	/* Platform specific data, device
					   core doesn't touch it */
	void		*driver_data;	/* Driver data, set and get with
					   dev_set_drvdata/dev_get_drvdata */
#ifdef CONFIG_PROVE_LOCKING
	struct mutex		lockdep_mutex;
#endif
	struct mutex		mutex;	/* mutex to synchronize calls to
					 * its driver.
					 */

	struct dev_links_info	links;
	struct dev_pm_info	power;
	struct dev_pm_domain	*pm_domain;

#ifdef CONFIG_ENERGY_MODEL
	struct em_perf_domain	*em_pd;
#endif

#ifdef CONFIG_GENERIC_MSI_IRQ_DOMAIN
	struct irq_domain	*msi_domain;
#endif
#ifdef CONFIG_PINCTRL
	struct dev_pin_info	*pins;
#endif
#ifdef CONFIG_GENERIC_MSI_IRQ
	raw_spinlock_t		msi_lock;
	struct list_head	msi_list;
#endif
#ifdef CONFIG_DMA_OPS
	const struct dma_map_ops *dma_ops;
#endif
	u64		*dma_mask;	/* dma mask (if dma'able device) */
	u64		coherent_dma_mask;/* Like dma_mask, but for
					     alloc_coherent mappings as
					     not all hardware supports
					     64 bit addresses for consistent
					     allocations such descriptors. */
	u64		bus_dma_limit;	/* upstream dma constraint */
	const struct bus_dma_region *dma_range_map;

	struct device_dma_parameters *dma_parms;

	struct list_head	dma_pools;	/* dma pools (if dma'ble) */

#ifdef CONFIG_DMA_DECLARE_COHERENT
	struct dma_coherent_mem	*dma_mem; /* internal for coherent mem
					     override */
#endif
#ifdef CONFIG_DMA_CMA
	struct cma *cma_area;		/* contiguous memory area for dma
					   allocations */
#endif
#ifdef CONFIG_SWIOTLB
	struct io_tlb_mem *dma_io_tlb_mem;
#endif
	/* arch specific additions */
	struct dev_archdata	archdata;

	struct device_node	*of_node; /* associated device tree node */
	struct fwnode_handle	*fwnode; /* firmware device node */

#ifdef CONFIG_NUMA
	int		numa_node;	/* NUMA node this device is close to */
#endif
	dev_t			devt;	/* dev_t, creates the sysfs "dev" */
	u32			id;	/* device instance */

	spinlock_t		devres_lock;
	struct list_head	devres_head;

	struct class		*class;
	const struct attribute_group **groups;	/* optional groups */

	void	(*release)(struct device *dev);
	struct iommu_group	*iommu_group;
	struct dev_iommu	*iommu;

	enum device_removable	removable;

	bool			offline_disabled:1;
	bool			offline:1;
	bool			of_node_reused:1;
	bool			state_synced:1;
	bool			can_match:1;
#if defined(CONFIG_ARCH_HAS_SYNC_DMA_FOR_DEVICE) || \
    defined(CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU) || \
    defined(CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU_ALL)
	bool			dma_coherent:1;
#endif
#ifdef CONFIG_DMA_OPS_BYPASS
	bool			dma_ops_bypass : 1;
#endif
};
```

device_driver 和 device 分别表示驱动和设备，而这两者都必须依附于一种总线，因此都包含 bus_type 指针。**在 Linux 内核中，设备和驱动是分开注册的。**注册 1 个设备的时候，并不需要驱动已经存在。而 1 个驱动被注册的时候，也不需要对应的设备已经被注册。设备和驱动各自涌向内核，而每个设备和驱动涌入内核的时候，都会去寻找自己的另一半。而正是 bus_type 的 match() 成员函数将两者捆绑在一起。一旦匹配成功，xxx_driver 的 probe() 函数就会被执行（xxx 是总线名， 如 platform、pci、i2c、spi、usb 等）。

**总线、驱动和设备最终都会落实为 sysfs 中的 1 个目录。**它们实际上都可以认为是 kobject 的派生类，**kobject 可看作是所有总线、设备和驱动的抽象基类**，1个 kobject 对应 sysfs 中的 1 个目录。

另外，**总线、驱动和设备中的各个 attribute 直接落实为 sysfs 中的一个文件。**结构体中含有 show() 和 store() 两个函数，分别用于读写对应的 sysfs 文件。这几个 attribute 相关结构体的定义如下：

```c
struct attribute {
	const char		*name;
	umode_t			mode;
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	bool			ignore_lockdep:1;
	struct lock_class_key	*key;
	struct lock_class_key	skey;
#endif
};

struct bus_attribute {
	struct attribute	attr;
	ssize_t (*show)(struct bus_type *bus, char *buf);
	ssize_t (*store)(struct bus_type *bus, const char *buf, size_t count);
};

struct driver_attribute {
	struct attribute attr;
	ssize_t (*show)(struct device_driver *driver, char *buf);
	ssize_t (*store)(struct device_driver *driver, const char *buf,
			 size_t count);
};

struct device_attribute {
	struct attribute	attr;
	ssize_t (*show)(struct device *dev, struct device_attribute *attr,
			char *buf);
	ssize_t (*store)(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count);
};
```

### udev 的组成

udev 目前和 systemd 项目已合并，地址：[https://github.com/systemd/systemd](https://github.com/systemd/systemd)

udev 在用户空间中执行，动态建立/删除设备文件，允许每个人都不用关心主/次设备号而提供 Linux 标准规范的名称，并且可以根据需要固定名称。

udev 的工作机制如下：

1. udev 以守护进程的方式运行，工作于用户空间，它监听一个 netlink socket。
2. 当有新设备接入或设备从系统中移除时，系统内核通过这个 socket 向 udev 发送 uevent。
3. udev 获取内核发送的信息，进行规则的匹配。匹配的事物包括 SUBSYSTEM、ACTION、atttribute、内核提供的名称（通过 KERNEL =）以及其他的环境变量。

### udev 规则文件

udev 规则文件以行为单位，一行代表一个规则。每个规则分为一个或多个匹配部分和赋值部分。二者带有不同的关键字。

匹配关键字包括：ACTION（行为）、KERNEL（匹配内核设备名）、BUS（匹配总线类型）、SUBSYSTEM（匹配子系统名）、ATTR（属性）等。

赋值关键字包括：NAME（创建的设备文件名）、SYMLINK（符号创建链接名）、OWNER（设置设备的所有者）、GROUP（设置设备的组）、IMPORT（调用外部程序）、MODE（节点访问权限）等。

举个例子看一下：

```markdown
SUBSYSTEM=="net", ACTION=="add", DRIVERS==" *", ATTR{address}=="08:00:27:35:be:ff", ATTR{dev_id}=="0x0", ATTR{type}=="1", KERNEL=="eth*", NAME="eth1"
```

匹配部分包括 SUBSYSTEM、ACTION、ATTR、KERNEL 等，赋值部分有一项，是 NAME。这个规则的意思是：当系统中出现的新硬件属于 net 子系统范畴，系统对该硬件采取的动作是 add 这个硬件，且这个硬件的 address 属性信息等于 "08:00:27:35:be:ff"，dev_id 属性等于 "0x0" 、type 属性为 1 等。此时，对这个硬件在 udev 层实行的动作是创建 /dev/eth1。

# Linux 字符设备驱动

Linux 系统将设备分为3类：**字符设备、块设备、网络设备**。架构图如下：

![](https://img-blog.csdn.net/20160309214506200)

**字符设备是指只能一个字节一个字节读写的设备，不能随机读取设备内存中的某一数据，读取数据需要按照先后数据。**字符设备是面向流的设备。常见的字符设备有鼠标、键盘、串口、控制台和 LED 设备等。

**块设备是指可以从设备的任意位置读取一定长度数据的设备。**块设备包括硬盘、磁盘、U 盘和 SD 卡等。

每一个字符设备或块设备都在 /dev 目录下对应一个设备文件。Linux 用户程序通过设备文件（或称设备节点）来使用驱动程序操作字符设备和块设备。

## 字符设备驱动结构

### cdev 结构体

在 Linux 内核中，**使用 cdev 结构体描述字符设备**，定义如下：

```c
struct cdev {
	struct kobject kobj; // 内嵌的 kobject 对象
	struct module *owner; // 所属模块
	const struct file_operations *ops; // 文件操作结构体
	struct list_head list;
	dev_t dev; // 设备号
	unsigned int count;
} __randomize_layout;
```

cdev 的一个重要成员 file_operations 定义了字符设备驱动需要提供给虚拟文件系统 VFS 的接口函数。

cdev 的成员 dev_t 定义了设备号，包含主设备号和次设备号。dev_t 实际上就是 unsigned int，32 位。其中高 12 位是主设备号，低 20  位是次设备号。如下是设备号相关的宏函数：

```c
MAJOR(dev) // 通过 dev_t 获得主设备号。
MINOR(dev) // 通过 dev_t 获得次设备号。
MKDEV(major, minor) // 通过主设备号和次设备号构造 dev_t 的设备号。
```

Linux 提供了一系列函数用于操作 cdev 结构体：

```c
// 初始化 cdev 成员，并建立 cdev 和 file_operations 的连接。
void cdev_init(struct cdev *, const struct file_operations *);

// 用于动态申请 cdev 内存。
struct cdev *cdev_alloc(void);

void cdev_put(struct cdev *p);

// 向系统中添加 cdev，完成字符设备的注册。一般用于模块加载函数中。
int cdev_add(struct cdev *, dev_t, unsigned);

// 从系统中删除 cdev，完成字符设备的注销。一般用于模块卸载函数中。
void cdev_del(struct cdev *);
```

### 分配和释放设备号

在调用 cdev_add() 函数注册字符设备之前，需要先向系统申请设备号，即主设备号和次设备号。调用函数 register_chrdev_region() 或 alloc_chrdev_region() 实现：

```c
// 用于已知起始设备的设备号的情况。
int register_chrdev_region(dev_t from, unsigned count, const char *name);

// 用于起始设备号未知，向系统动态申请设备号的情况。
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
```

注意函数中的参数 count，结合设备号的主设备号和次设备号的概念。我们发现整个流程中，首先需要向内核申请 dev_t 类型的设备号，包括主设备号和次设备号。同一类型的字符设备可共同同一个主设备号，而次设备号不同。因此，这样在添加注册字符设备的时候可以同时注册几个字符设备，这也是 count 的含义。

相应的，在初始释放的时候，除了需要从系统中删除 cdev 完成字符设备的注销，还需要释放原先申请的设备号，函数如下：

```c
void unregister_chrdev_region(dev_t from, unsigned count);
```

### file_operations 结构体

file_operations 结构体中的成员函数是字符设备驱动程序设计的主体内容，这些函数实际会在应用程序进行 Linux 的 open()、write()、read()、close() 等系统调用时最终被内核调用。其定义如下：

```c
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iopoll)(struct kiocb *kiocb, bool spin);
	int (*iterate) (struct file *, struct dir_context *);
	int (*iterate_shared) (struct file *, struct dir_context *);
	__poll_t (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	unsigned long mmap_supported_flags;
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in,
				   struct file *file_out, loff_t pos_out,
				   loff_t len, unsigned int remap_flags);
	int (*fadvise)(struct file *, loff_t, loff_t, int);
} __randomize_layout;
```

下面对其进行简要分析：

1. llseek()：修改一个文件的当前读写位置，并返回新位置偏移指针。出错时函数返回负值。
2. read()：**从设备读取数据。**成功时返回读取的字节数，出错时返回一个负值。与用户空间的 `ssize_t read(int fd, void *buf, size_t count);` 函数对应。返回 0 代表 EOF（end of file）。
3. write()：**向设备发送数据。**成功时返回写入的字节数，出错时返回一个负值。如果未实现此函数，用户进行 write() 系统调用时，将得到 -EINVAL 返回值。与用户空间的 `ssize_t write(int fd, const void *buf, size_t count);` 函数对应。同样返回 0 代表 EOF（end of file）。
4. unlocked_ioctl()：提供**设备相关控制命令的实现**（既不是读操作，也不是写操作）。成功时返回一个非负值。与用户空间的 `int fcntl(int fd, int cmd, ... /* arg */ );` 函数对应。
5. mmap()：**将设备内存映射到进程的虚拟地址空间中。**如果未实现此函数，用户进行 mmap() 系统调用的时，会获得 -ENODEV 返回值。这个函数对于帧缓冲等设备特别有意义，帧缓冲被映射到用户空间后，应用程序可以直接访问它而无须在内核和应用间进行内存复制。与用户空间的 `void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);` 函数对应。
6. open()、release()：当用户空间调用 open() 打开设备文件时，设备驱动的 open() 函数最终被调用。驱动程序可以不实现这个函数，在这种情况下，设备的打开操作永远成功。与 open() 函数对应的是 release() 函数，也对应用户空间的 close() 函数。
7. poll()：一般用于询问设备是否可被非阻塞地立即读写。当询问的条件未触发时，用户空间进行 select() 和 poll() 系统调用将引起进程的阻塞。
8. aio_read()、aio_write()：对与文件描述符对应的设备进行**异步**读、写操作。设备实现这两个函数后，用户空间可以对该设备文件描述符执行 SYS_io_setup、SYS_io_submit、SYS_io_getevents、SYS_io_destroy 等系统调用进行读写。

### 字符设备驱动的组成

#### 模块加载与卸载函数

在模块加载函数中应实现设备号的申请和 cdev 的注册，在模块卸载函数中应实现 cdev 的注销和设备号的释放。

类似的模板如下：

```c
// 设备结构体
struct xxx_dev_t
{
    struct cdev cdev;

    ...
} xxx_dev;


// 设备驱动模块加载函数
static int __init xxx_init(void)
{
    ... 

    cdev_init(&xxx_dev.cdev, &xxx_fops); // 初始化cdev
    xxx_dev.cdev.owner = THIS_MODULE;

    // 获取字符设备号
    if (xxx_major)
    {
        register_chrdev_region(xxx_dev_no, 1, DEV_NAME);
    }
    else
    {
        alloc_chrdev_region(&xxx_dev_no, 0, 1, DEV_NAME);
    }

    res = cdev_add(&xxx_dev.cdev, xxx_dev_no, 1); // 注册设备

    ...
}

// 设备驱动模块卸载函数
static void __exit xxx_exit(void)
{
    unregister_chrdev_region(xxx_dev_no, 1); // 释放占用的设备号
    cdev_del(&xxx_dev.cdev);                 // 注销设备

    ...
}
```

#### file_operations 结构体的成员函数

file_operations 结构体的成员函数是字符设备驱动与内核虚拟文件系统的接口，是用户空间对 Linux 系统调用最终的落实者。

```c
// 读设备
// filp：文件结构体指针，buf：用户空间内存地址；count：要读的字节数；f_pos：读的位置相对文件开头的偏移。
ssize_t xxx_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ...

    copy_to_user(buf, ..., ...);

    ...
}

// 写设备
// filp：文件结构体指针，buf：用户空间内存地址；count：要写的字节数；f_pos：读的位置相对文件开头的偏移。
ssize_t xxx_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    ...

    copy_from_user(..., buf, ...);

    ...
}

// ioctl 函数
// cmd 参数为事先定义的 I/O 控制命令，而 arg 为对应于该命令的参数。
// 例如对于串行设备，若 SET_BAUDRATE 是一道设置波特率的命令，arg 就应该是波特率值。
long xxx_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    ...

    switch (cmd)
    {
        case XXX_CMD1:
        {
            ...

            break;
        }
        case XXX_CMD2:
        {
            ...

            break;
        }
        default:
            // 不能支持的命令
            return -ENOTTY;
    }


    return 0;
}
```

由于**用户空间不能直接访问内核空间的内存**，因此借助了函数 copy_from_user() 完成用户空间缓冲区到内核空间的复制，以及 copy_to_user() 完成内核空间到用户空间缓冲区的复制。这里注意一下语义，以读为例，注意主谓。xxx_read() 的含义是从内核空间中读，因此用户空间是接收的对象，因此使用 copy_to_user()，对应的也是用户空间的系统调用 read()。整个流程就是内核读 I/O（先不考虑用户区缓冲的事情），然后 copy_to_user() 到用户空间的内存。

这两个函数的声明如下：

```c
// 两个函数均返回不能被复制的字节数。如果成功，返回 0；如果失败，返回负值。
int copy_from_user(void *to, const void __user volatile *from, unsigned long n);
int copy_to_user(void __user volatile *to, const void *from, unsigned long n);
```

如果复制的内存是简单类型，如 char、int、long 等，可以使用简化的 put_user() 和 get_user()：

```c
int val; // 内核空间整型变量。

get_user(val, (int *) arg); // 用户 -> 内核，arg 是用户空间的地址。

put_user(val, (int *) arg); // 内核 -> 用户，arg 是用户空间的地址。
```

上面读和写函数的参数中 `__user` 是一个宏，代表后面的指针属于用户空间，用于假注释和提醒的作用。类似于模块加载函数的 `__init` 和模块卸载函数的 `__exit`。

内核空间虽然可以访问用户空间的数据，但是在访问之前，一般需要**检查合法性**。使用 `access_ok(addr, size)` 接口，以保证传入的数据的确属于用户空间。**此宏函数返回非 0 表示检查通过。**

另外，`put_user()` 和 `__put_user()` 的区别在于，`put_user()` 的实现中调用了 `__put_user()`，并且已经手动做了 `access_ok()` 的检查。所以一般推荐使用 `put_user()` 和 `get_user()`。同时，也存在 `copy_from_user()` 和 `__copy_from_user()`，区别同上。`copy_to_user()` 同理。但是，总的来讲为了保险，内核空间在访问用户空间数据以前，我们的驱动程序中应再次手动检查一下合法性。

在字符设备驱动中，需要定义一个 file_operations 的实例，将我们自己实现的函数注册给这些成员：

```c
// xxx_fops 在 cdev_init(&xxx_dev.cdev, &xxx_fops) 时同字符设备建立起连接。
struct file_operations xxx_fops = {
    .owner = THIS_MODULE,
    .read = xxx_read,
    .write = xxx_write,
    .unlocked_ioctl = xxx_ioctl,

    ...
};
```

下面是字符设备驱动的结构示意图：

<img src="https://img-blog.csdnimg.cn/direct/f0c4207c95a14c3bb342970f131afd7e.png" alt="image-20241108100951215" style="zoom:75%;" />

## globalmem 虚拟设备

globalmem 意为全局内存。在 globalmem 字符设备驱动中会分配一片大小为 GLOBALMEM_SIZE(4 KB) 的内存空间，并在驱动中提供针对该片内存的读写、控制和定位函数，以供用户空间的进程能通过 Linux 系统调用获取或设置这片内存的内容。

