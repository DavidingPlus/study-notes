---
title: UNIX 环境高级编程
categories:
  - Linux 学习
  - 用户层
abbrlink: 6fbfe9af
date: 2024-06-20 23:05:00
updated: 2024-11-07 16:25:00
---

<meta name="referrer" content="no-referrer"/>

# 第一章 UNIX基础知识

## UNIX体系结构

操作系统是计算机裸机上的第一个软件，他控制计算机硬件资源，提供程序运行环境，通常我们称之为**内核**

以下是UNIX系统的体系结构图：

![image-20230827151538210](https://image.davidingplus.cn/images/2025/01/31/image-20230827151538210.png)

<!-- more -->

内核的接口被称为**系统调用**，公共函数库，比如C语言的C runtime library就是依托在操作系统提供的API上的，这导致C语言的标准库能够完美适配各种系统，我们的程序既可以使用公共函数库，也可以使用操作系统的API，也就是系统调用

shell，也就是我们常用的终端，就是一个特殊的程序，它可以在里面调用其他应用程序，也就是启动其他进程

## shell

上面说到，shell终端是一个特殊的程序，的确；用户登录之后，通常先显示一些系统信息，接着用户就可以向shell输入命令了；也就是说，shell是一个命令行解释器，他读取用户的输入，然后就可以执行相关的操作命令；shell的用户输入可以来自用户在终端命令行的输入，也可以来自shell脚本，我们可以把需要执行的shell命令写成一个文件交给shell终端去读取然后执行

例如，这是我的shell终端截图：

![image-20230827153122412](https://image.davidingplus.cn/images/2025/01/31/image-20230827153122412.png)

不同的Linux系统一般默认使用不同的shell，但是 POSIX 1003.2 标准对shell进行了标准化

以下是一些UNIX系统中常见的shell：

![image-20230827152956059](https://image.davidingplus.cn/images/2025/01/31/image-20230827152956059.png)

## 文件和目录

### 文件系统

我们打开一个shell终端，他的默认路径是 **~** 符号表示的，如图：

![image-20230827153213499](https://image.davidingplus.cn/images/2025/01/31/image-20230827153213499.png)

这不是我们的根目录，浏览文件目录之后发现这是我们的 home 目录下的用户目录：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230827153303414.png" alt="image-20230827153303414" style="zoom:67%;" />

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230827153312742.png" alt="image-20230827153312742" style="zoom:67%;" />

也就是说，我们 ~ 目录对应的实际上是 /home/lzx0626 目录在这里

好，我们现在来看一下根目录，根目录的名称是字符 **/**，我们切换过去：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230827153445254.png" alt="image-20230827153445254" style="zoom:67%;" />

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230827153453150.png" alt="image-20230827153453150" style="zoom:67%;" />

**目录项的逻辑视图(就是如图)和实际上在硬盘上的存放方式是不同的**，这一点我们后面细谈

### 文件名

**文件名字在命名的时候，斜线 ( / ) 和空字符 ( 注意不是空格 ) 不能出现在文件名当中，斜线用来区分路径，空字符用来终止一个路径名**

**创建新目录的时候会自动创建两个文件名，就是 . 和 .. ，. (点)用来指向当前目录， .. (点点)用来指向上级目录；在最高等级的根目录当中，这两个相同。以下是一个空目录中的例子：**

![image-20230827154133983](https://image.davidingplus.cn/images/2025/01/31/image-20230827154133983.png)

### 路径名

路径名分为绝对路径和相对路径，绝对路径以根目录为起始，也就是以 / 斜线开头，因为斜线 / 是根目录的路径，不以斜线 / 开头的就是相对路径，基准路径是当前路径的文件；文件系统根的名字( / )是一个特殊的绝对路径名，它不包含文件名

#### <span id='ls -l'>例子</span>

模拟实现 ls 命令，就是列出目录当中的所有文件名

需要用到 `opendir`()，`readdir`()，`closedir`() 函数，来读取，他们的功能可以类比读取文件的操作，只不过这里是操作目录的路径

~~~cpp
//打开目录
#include <dirent.h>
#include <sys/types.h>
DIR *opendir(const char *name);
//参数：
    //name：需要打开的目录的名称
//返回值：
    //DIR * 类型，理解为目录流，对比打开文件返回文件描述符，这个返回的是一个目录流指针
    //错误 返回nullptr

//读取目录中的数据
#include <dirent.h>
struct dirent *readdir(DIR *dirp);
//参数：
    //dirp是通过opendir返回的结果
//返回值：
    //struct dirent 代表读取到的文件的信息
    //读取到了文件末尾或者失败了，返回Null，区别是读到文件末尾不会修改errno，失败会修改
	//一次只能读取一个文件的信息，读取一个目录最好循环读取

//关闭目录
#include <dirent.h>
int closedir(DIR *dirp);
~~~

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <dirent.h>

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("usage : %s  <pathname>\n", argv[0]);
        return -1;
    }

    // 打开文件目录
    const char *pathname = argv[1];

    DIR *_dir = opendir(pathname);
    if (nullptr == _dir) {
        perror("opendir");
        return -1;
    }

    // 开始循环读取，因为一次只能读一个
    while (1) {
        struct dirent *_dirent = readdir(_dir);
        if (nullptr == _dirent)
            break;

        cout << _dirent->d_name << '\t';
    }

    // 关闭文件目录
    closedir(_dir);

    cout << endl;

    return 0;
}
~~~

执行结果：

![image-20230827162316877](https://image.davidingplus.cn/images/2025/01/31/image-20230827162316877.png)

注意一个有趣的地方，我们程序读取文件信息的顺序排列不是按照字母顺序排列的，但是ls命令一般是按照字母顺序排列的，例如：

![image-20230827162546548](https://image.davidingplus.cn/images/2025/01/31/image-20230827162546548.png)

## 输入和输出

### 文件描述符

文件描述符(file descriptor)是一个小的非负整数，内核用以标识一个进程访问的文件；当内核打开或者创建一个文件的时候，都返回文件描述符；在我们相对文件读写的时候都可以使用这个文件描述符

### 标准输入，标准输出和标准错误

**我们一个进程分为用户区和内核区，在内核区中有一张文件描述表，注意是每个进程都具有自己的内核区，进程之间互不干涉，因此我用两个程序打开同一个文件返回的文件描述符没有其他特殊情况应该是同一个值，都应该是3；我们的进程默认 标准输入 STDIN_FILENO 是0；标准输出 STDOUT_FILENO 是1；标准错误 STDERR_FILENO 是2；**这三个文件描述符默认是定向到终端的，我们的shell命令中可以使用 > 可以让他重定向到文件当中，例如如下：

![image-20230827164726247](https://image.davidingplus.cn/images/2025/01/31/image-20230827164726247.png)

现在我打开 a.txt：

![image-20230827164746803](https://image.davidingplus.cn/images/2025/01/31/image-20230827164746803.png)

### 不带缓冲的`I/O`

取名为不带缓冲的`I/O`，并不是指数据的传递过程当中不存在缓冲区，当然存在缓冲机制，但是这里的不带缓冲特指的是进程不提供缓冲区，也就是在用户区没有缓冲机制，但是内核还是提供缓冲的，因为我们的文件描述符对应的就是读写缓冲区，这个是在内核当中的

**因此不带缓冲的概念要弄明白了：所谓不带缓冲，并不是指内核不提供缓冲，而是只单纯的系统调用，不是函数库的调用。**

系统调用就比如我们Linux下的read()和write()函数，函数库的调用就比如C标准库当中的fread()和fwrite()函数，下面我们具体聊一聊他们的工作机制：

- 系统内核对磁盘的读写都会提供一个块缓冲（在有些地方也被称为内核高速缓存），当用write函数对其写数据时，直接调用系统调用，将数据写入到块缓冲进行排队，当块缓冲达到一定的量时，才会把数据写入磁盘。因此所谓的不带缓冲的`I/O`是指进程不提供缓冲功能（但内核还是提供缓冲的）。每调用一次write或read函数，直接系统调用。
- 而带缓冲的`I/O`是指进程对输入输出流进行了改进，提供了一个流缓冲，当用fwrite函数网磁盘写数据时，先把数据写入流缓冲区中，当达到一定条件，比如流缓冲区满了，或刷新流缓冲，这时候才会把数据一次送往内核提供的块缓冲，再经块缓冲写入磁盘。（双重缓冲）。因此，带缓冲的`I/O`在往磁盘写入相同的数据量时，会比不带缓冲的`I/O`调用系统调用的次数要少。

比如我们Linux系统提供的API：open，read，write，lseek，close都是不带缓冲的`I/O`，并且对应的都是内核区的文件描述符

#### 例子

我们从标准输入读取数据，然后将其原封不动的输出到屏幕上，这里就使用了 STDIN_FILENO 和 STDOUT_FILENO 的固定文件描述符

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    char buf[MAX_BUFFER_SIZE] = {0};

    while (1) {
        bzero(buf, sizeof(buf));

        int len = read(STDIN_FILENO, buf, sizeof(buf) - 1);
        if (-1 == len) {
            perror("read");
            return -1;
        }

        if (0 == len)  // 读取到末尾
            break;

        write(STDOUT_FILENO, buf, strlen(buf));
    }

    return 0;
}
~~~

执行结果：

标准输入是用回车符 '\n' 来判断是否结束输入的

![image-20230827182021232](https://image.davidingplus.cn/images/2025/01/31/image-20230827182021232.png)

#### 扩展

我们用过这个例子玩一些花活，比如使用shell命令重定向该进程的标准输入和标准输出

例如，我可以将标准输出定向到文件当中，使用命令

~~~bash
./a.out > 02.txt
~~~

这个时候该进程的标准输入和标准错误还是定向到终端的，但是标准输出重定向到这个文件中了，如果这个文件不存在，那么会将其创建出来

![image-20230827182352781](https://image.davidingplus.cn/images/2025/01/31/image-20230827182352781.png)

我们可以看一下这个文件的内容

![image-20230827182523353](https://image.davidingplus.cn/images/2025/01/31/image-20230827182523353.png)

或者，我们可以把标准输入也给重定向一下，刚才标准输出用的是 > ，类比一下标准输入应该用的是 <

~~~bash
./a.out < 02_src.txt > 02_dest.txt
~~~

执行结果：

可以看出实现了这个需求

![image-20230827182917510](https://image.davidingplus.cn/images/2025/01/31/image-20230827182917510.png)

### 标准`I/O`

标准`I/O`就是指的是在系统提供的API基础上进行封装之后给我们用户使用的函数库，比较典型的就是C语言的c runtime library，里面的很多函数都和Linux下的系统API非常类似，但是他们的参数和返回值有所区别，具体可以查询man文档；例如，Linux下读写操作使用的是read()和write()接口，在C标准库当中对应的是fread()和fwrite()

标准`I/O`和不带缓冲的`I/O`的巨大区别就在于标准`I/O`在用户区提供了供`I/O`操作的缓冲区，而不带缓冲的`I/O`对应系统调用则没有，只有内核区的缓冲；**使用标准`I/O`的函数有一个好处就是我们可以不用考虑如何选取缓冲区的最佳大小，例如我们read()需要给出buf数组的大小，他帮我们做了更多的事情**

典型的标准`I/O`函数就是printf()和scanf()函数，这里我们用getc()和putc()每次读写一个字符的函数举例

#### 例子

我们读键盘的字符然后输出到屏幕上

~~~cpp
#include <iostream>
using namespace std;

int main() {
    while (1) {
        int c = getc(stdin);
        if (EOF == c)  // getc()读取完毕的标志是EOF
            break;
        putc(c, stdout);
    }

    return 0;
}
~~~

执行结果：

![image-20230827184247517](https://image.davidingplus.cn/images/2025/01/31/image-20230827184247517.png)

同样，可以用shell命令进行一些重定向的操作，这里就不写了

## 程序和进程

### 程序

程序是在磁盘上某个目录当中的可执行文件；内核调用exec函数族，可以将这个程序读入内存，然后执行程序

exec函数族一般有下面这几个

![image-20230718154816547](https://image.davidingplus.cn/images/2025/02/01/image-20230718154816547.png)

### 进程和进程ID

程序是死的，他需要被读进内存才会变成进程，UNIX系统给每个进程都有一个独立的编号，就是进程号，或者叫进程ID，他总是一个非负整数。Linux操作系统的第一个进程是init进程，其他的所有进程都是这个进程创建或者这个进程的子孙进程创建出来的

我们可以用 ps aux 来查看我们操作系统中的进程状态

我们可以看到第一个进程就是 init ，他的进程号是1

![image-20230828105813917](https://image.davidingplus.cn/images/2025/01/31/image-20230828105813917.png)

### 进程控制

有三个用于进程控制的主要函数：fork，exec函数族和waitpid

#### 例子

我们从标准输入中读取命令，然后执行这些命令，这些命令都比较简单，我们规定他们不带有执行参数，比如 pwd ; ls 等等，这里我们就可以用到exec函数族

代码中需要注意一点是fgets()读取到的字符串末尾结束符之前有一个 '\n' 回车符，需要将其删掉才是正确的命令

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <sys/wait.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    char buf[MAX_BUFFER_SIZE] = {0};

    while (1) {
        // 读取命令(简单的没有参数的)
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf) - 1, stdin);
        // 将buf中的'\n'字符替换成'\0'，不然命令会出错
        buf[strlen(buf) - 1] = 0;

        pid_t pid = fork();
        if (-1 == pid) {
            perror("fork");
            return -1;
        }

        if (pid > 0) {
            // 父进程回收子进程，等待执行，回收一次读取下一次命令
            waitpid(pid, nullptr, 0);
            printf("child process has been recycled, pid :%d\n", pid);
        } else if (0 == pid)
            // 子进程调用exec函数族执行命令
            execlp(buf, buf, nullptr);
    }

    return 0;
}
~~~

### 线程和线程ID

通常一个进程只有一个控制线程，但是对于一个复杂问题，如果我们能够用多个线程分别作用于他的不同部分，那么解决起来就容易得多；另外，多线程也可以充分利用多处理器系统的并行能力

一个进程内的所有线程共享同一地址空间，文件描述符以及与进程相关的属性；**最重要的一点就是数据是共享的，但是这就带来了一个问题，如何保证线程在访问共享数据的时候保证线程的数据同步**

线程也用线程ID标识，但是线程ID只在本进程当中起作用，一个线程ID在其他进程当中没有作用

线程也有相关的系统API，这个后面再说

## 出错处理

**当UNIX系统函数出错的时候，通常函数会返回一个负数值，并且会修改错误号 errno 为特定的值**

errno定义在头文件 <errno.h> 中，当中定义了各种常量，这些常量都以 **E** 开头

在多线程当中，每个线程都有自己的局部 errno ，以避免一个线程干扰了另一个线程

注意，对于errno应该注意两条规则：

- 如果没有出错，errno的值不应该被进程或者线程清除，我们只有在调用系统函数的返回值出错的时候，才会检验 errno 的值
- 任何函数都不会将errno的值设置为 0 ，并且在头文件中定义的所有错误号宏都不为0

### 两个错误信息打印函数

C语言给出了两个错误信息打印函数，strerror()和perror()

~~~cpp
char* strerror(int errnum);
// 作用：根据我们给出的错误号，将其映射为一个错误消息的字符串并将其返回
// 参数：错误号

void perror(const char* msg);
// 作用：基于当前我们的errno值，在标准错误的文件描述符上面生成一条错误消息，输出到终端
// 参数：一个字符串，在输出的时候首先输出 msg 字符串，然后输出一个冒号，后面就是我们的错误信息，最后换行
~~~

#### 例子

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;

int main() {
    char* error_msg = strerror(EACCES);  // Permission denied
    cerr << error_msg << endl;

    errno = ENOENT;  // No such file or directory
    perror("msg");

    return 0;
}
~~~

### 出错恢复

错误一般可以分为致命性和非致命性的，对于致命性的错误，没有办法进行恢复，我们最多能进行的操作就是在屏幕上打印一条出错消息或者将一条出错消息写入日志文件，然后退出程序；对于非致命性的错误，我们可以较为妥善的进行处理，并且大多数的非致命性错误都是暂时的，例如资源短缺，对于资源类的非致命性错误的典型恢复操作是延迟一段时间，然后重试

我们自己在写程序的时候有时候也会遇到非致命错误，这个时候我们可能需要做一些手动的特殊判断处理，这样能够增强我们程序的健壮性

## 用户标识

### 用户ID

用户ID是一个数值，它可以标识不同的用户；用户不能更改用户ID，通常每个用户有一个唯一的用户ID

用户ID为0的用户是根用户或者超级用户；我们称登录名为root的用户为超级用户特权，超级用户对系统有绝对自由的支配权

### 组ID

口令文件登录项中也包括用户的组ID，他也是一个数值；**组用于将若干用户集合到项目或者部门当中去，这种机制允许同组的各个成员之间共享资源，例如文件**，我们可以设置文件的权限让组内的所有成员都能访问该文件，但是组外的成员不能访问

组文件在我们的系统中通常是 /etc/group ，打开之后长这样，下面还有数据，截图截不全，可以看出他把我们的组名映射成了组ID

![image-20230828144329771](https://image.davidingplus.cn/images/2025/01/31/image-20230828144329771.png)

我们可以用如下的代码打印我们的用户ID和所在的组ID

~~~cpp
printf("uid = %d , gid = %d\n", getuid(), getgid());
~~~

在我的Linux上打印出来是1000

![image-20230828144714174](https://image.davidingplus.cn/images/2025/01/31/image-20230828144714174.png)

### 附属组ID

除了在口令文件对一个登录名指定一个组ID之外，**大多数UNIX系统还允许一个用户属于另外一些组**

## 信号

信号是用于通知给我们的进程发生了某种情况，举一个例子，例如我们现在执行了除法除以0的操作，系统会给进程发出 SIGEPE (浮点异常)，我们一般有三种方式来处理信号：

- **忽略信号**，我们不推荐使用这种方式，因为忽略系统发送的信号听起来就是个不明智的选择，但是在某些情况下有使用

- **系统按照默认方式处理**，对于除数为0，系统默认的方式是终止该进程

- **提供一个函数，信号发生的时候就调用这个函数，我们称为信号捕捉，这个函数我们可以自己编写，通过这个函数我们就可以知道信号发生的时候我们怎么具体处理**

  我一般用的比较多的是就是 sigaction() 函数

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

  **在很多情况下都能产生信号，在终端上可以有两种从键盘录入产生信号的方法，分别称为中断键，通常是 ctrl + C 和退出键，通常是 ctrl + \，都可以用于结束当前的进程，他们发出的信号分别是 SIGINT 和 SIGQUIT；**

  **我们也可以调用系统提供的 kill() 函数来产生信号，比如我们可以产生9号信号 SIGKILL，就可以无条件终止进程，这个信号不能被阻塞或者捕获；当然我们通过进程向另一个进程发送信号的时候也不是随便发的，也有权限要求，我们必须是这个进程的所有者或者是超级用户权限才行**

### 例子

下面我们用sigaction()函数来捕捉一下SIGINT信号，我从键盘录入字符，然后遇到 ctrl + C 就打印一句话，但是不终止程序

书上用的是signal()函数，但是我觉得用sigaction()更好一些

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <signal.h>

#define MAX_BUFFER_SIZE 1024

void my_hander(int signum);

int main() {
    // 设置一些参数
    struct sigaction act;
    act.sa_handler = my_hander;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, nullptr);

    char buf[MAX_BUFFER_SIZE] = {0};
    while (1) {
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf) - 1, stdin);

        write(STDOUT_FILENO, buf, strlen(buf));
    }

    return 0;
}

void my_hander(int signum) {
    printf("signal SIGINT got.\n");
}
~~~

执行结果：

![image-20230828151530963](https://image.davidingplus.cn/images/2025/01/31/image-20230828151530963.png)

## 时间值

UNIX系统使用过两种不同的时间值

### 日历时间

这个时间是自世界协调时 1970年1月1日 00:00:00 到现在所经过的秒数，系统用 time_t 保存这种时间值

### 进程时间

也叫做CPU时间，是用来度量进程使用的中央处理器资源，系统用 clock_t 保存这种时间值

当度量一个进程的执行时间的时候，UNIX提供了三个进程时间值：

- 时钟时间
- 用户CPU时间
- 系统CPU时间

关于时间的说明我们后面再谈

## 系统调用和库函数

各种UNIX都提供了定义好的，数量有限，直接进入内核的入口点，这些称作系统调用；系统调用接口在UNIX中是用C语言定义的，但是早期的操作系统那些系统是按照传统方式的用机器的汇编语言定义的

**在Linux当中我们的man文档，第二章记录的是Linux的系统API，第三章记录的是C标准库的一些内容，这就是我们前面提到的系统调用和库函数的区别**；库函数在调用的时候不一定经过了系统调用，但是要实现某些功能，比如打印到屏幕，就会经过系统调用。例如我们的printf()函数，底层调用了write()；但是函数strcpy()和atoi()函数是C标准库的函数，他们并不调用任何内核的系统调用

**我们举一个例子来更进一步，以存储空间的分配函数malloc为例，这是C的库函数；但是在UNIX当中处理存储空间分配的系统调用是 sbrk() ，而我们的malloc()很可能底层调用了sbrk()函数作为支撑，我不知道是不是对的，只是我的猜测，库函数在系统调用的基础上省去了很多面向内核的直接操作，在用户层面更加方便了我们的使用**

我们用一个图来展示系统调用和库函数的关系：

![image-20230828154002555](https://image.davidingplus.cn/images/2025/01/31/image-20230828154002555.png)

**另一个区别是：系统调用通常提供一个最小的接口，实现的功能比较简单；而库函数大多数是在系统调用的基础上，可以提供更加复杂的功能供用户使用**

# 第二章 UNIX标准及实现(当小说看)

## UNIX 标准

1. ISO C：

   意图是提供C程序的可移植性，使得它能够适合于大量不同的操作系统。

   - 该标准不仅定义了C程序设计语言的语法和语义，还定义了标准库

2. POSIX(Portable Operating System Interface): 

   指的是**可移植操作系统接口**。该标准的目的是提升应用程序在各种UNIX系统环境之间的可移植性。它定义了“符合POSIX”的操作系统必须提供的各种服务。

   - POSIX 包含了 ISO C 标准库函数

3. SUS(Single Unix Specification)：

   是 POSIX 标准的一个超集，他定义了一些附加接口扩展了 POSIX 规范提供的功能。

4. 上述三个标准只是定义了接口的规范，而具体的实现由厂商来完成。目前UNIX主要有以下实现：

   - SVR4(UNIX System V Release 4)
   - 4.4 BSD(Berkeley Software Distribution)
   - FreeBSD
   - Linux
   - Mac OS X
   - Solaris

## 限制

1. UNIX 系统实现定义了很多幻数和常量。有两种类型的限制是必须的：

   - 编译时限制，如 `short int` 最大值是多少
   - 运行时限制，如文件名最长多少个字符

2. 通常编译时限制可以在头文件中定义；运行时限制则要求进程调用一个函数获得限制值。

3. 某些限制在一个给定的 UNIX 实现中可能是固定的（由头文件定义），在另一个 UNIX 实现中可能是动态的（需要由进程调用一个函数获得限制值）。如文件名的最大字符数在不同的操作系统中，是属于动态/静态限制。因此提供了三种限制：

   - 编译时限制（由头文件给定）
   - 与文件或者目录无关的运行时限制（由 `sysconf`函数给定）
   - 与文件或者目录相关的运行时限制（由 `pathconf`函数以及`fpathconf`函数给定）

4. ISO C 限制：ISO C 所有编译时限制都在头文件 `<limits.h>` 中,如整型大小。

   - 这些限制常量在一个给定的操作系统中不会改变
   - 关于浮点数的最大最小值的编译时限制，在`<float.h>`头文件中定义
   - `<stdio.h>` 头文件还定义了三个编译时限制：
     - `FOPEN_MAX`：可同时打开的标准`I/O` 流限制的最小数
     - `TMP_MAX`：由`tmpnam`函数产生的唯一文件名的最大个数
     - `FILENAME_MAX`：虽然 ISO C 定义了该常量，但是要避免使用。而要用 POSIX 提供的 `NAME_MAX`和 `PATH_MAX` 常量

5. POSIX 限制：POSIX定义了很多涉及操作系统实现限制的常量。这些常量大多数在`<limits.h>`中，也有的按照具体条件定义在其他头文件中

6. 获取运行时限制：

   ```cpp
   #include<unistd.h>
   long sysconf(int name); 
   long pathconf(const char*pathname,int name);
   long fpathconf(int fd,int name); //fd 为文件描述符
   ```

   - 参数：
     - `name`：指定的限制值。`name`参数是系统定义的常量
       - 以 `_SC_`开头的常量用于 `sysconf`函数
       - 以 `-PC_`开头的常量用于`pathconf`和`fpathconf`函数
     - `pathname`：文件名
     - `fd`：打开文件的文件描述符
   - 返回值：
     - 成功：返回对应的限制值
     - 失败： 返回 -1

   这三个函数失败的情况：

   - 若`name`参数并不是一个合适的常量，则这三个函数返回-1，并将 `errno`设置为 `EINVAL`
   - 有些`name`会返回一个不确定的值，这通过返回 -1 来体现，同时不改变`errno`	

# 第三章 文件`I/O`

大多数文件`I/O`只需要用到5个函数，`open`,`read`,`write`,`lseek`和`close`，这五个函数都是不带缓冲的`I/O`，这个前面进行过了说明，

补充一点，不带缓冲也可以说是调用内核当中的一个系统调用，因为在用户层没有缓冲区，但是在内核区存在缓冲机制

## 文件描述符

对于内核而言，打开的文件用文件描述符来进行标识；文件描述符是一个非负整数，当打开一个文件或者创建一个文件的时候，系统向进程返回文件描述符，我们的`read`和`write`函数是以文件描述符作为文件的依据的

按照惯例，UNIX系统一般把0 1 2 当作标准输入，标准输出和标准错误的文件描述符，他们默认是定向到终端的；他们当然也存在自己的宏，分别是`STDIN_FILENO`，`STDOUT_FILENO`，`STDERR_FILENO`

文件描述符的变化范围是：0 ~ `OPEN_MAX` -1，任何一个运行中的程序能够同时打开的文件数是有限制的，这个限制通常是由<`limits.h`>头文件中的常量`OPEN_MAX`定义的，它的值随系统的不同而不同，但POSIX要求它至少为**16**！

## 函数open和openat

~~~cpp
#include <fcntl.h>

int open(const char *pathname, int flags, mode_t mode);

int openat(int dirfd, const char *pathname, int flags, mode_t mode);
~~~

我们先看`flags`参数，分为必须有的和可选的：

必须有的

我们一般用的多的就是前三个，只读，只写和读写

![image-20230828191437922](https://image.davidingplus.cn/images/2025/01/31/image-20230828191437922.png)

可选的

可选的里面用的比较多的就是 O_APPEND，O_CREAT，一个表示进行追加，另一个表示创建文件，值得一提的是，创建文件的时候第三个参数 mode，需要给出指定的权限，比如0664(八进制数)

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230828192132187.png" alt="image-20230828192132187" style="zoom:80%;" />

![image-20230828192152274](https://image.davidingplus.cn/images/2025/01/31/image-20230828192152274.png)

由open和openat函数返回的文件描述符一定是最小的未用描述符数值，也就是说一般来说是按照顺序来的，但是如果前面的文件描述符被关闭之后，新用的文件描述符应该优先考虑最小的文件描述符值

下面我们来看一下`open`和`openat`的区别：

- `path`参数如果指定的是绝对路径名，在这种情况下，`fd`参数被忽略，`openat`函数相当于`open`函数
- `path`参数如果指定的是相对路径名，`fd`参数指出了相对路径名在文件系统中的开始地址，但是为什么是`fd`文件描述符呢？因为这个开始地址是我们通过打开相对路径名所在的目录获取的，需要用到`O_DIRECTORY`参数
- path参数指定了相对路径名，fd参数具有特殊值`AF_FDCWD`，在这种情况下，路径名从当前工作目录中获取，这一点和open函数的性能比较类似

### 例子

下面我们举个例子来看看`openat`函数的使用，`open`函数我用了很多次了，这里就不演示了

两个例子都是用了相对路径，第一个是使用我们指定的相对路径作为起始地址打开文件，第二个是使用当前工作目录作为起始地址

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUF1FER_SIZE 1024

int main() {
    // 通过open函数打开项目中的01目录
    int dir_fd = open("./01", O_DIRECTORY);
    if (-1 == dir_fd) {
        perror("open");
        return -1;
    }

    // 通过openat函数使用相对路径打开其他目录(01)的文件
    int fd1 = openat(dir_fd, "01.txt", O_RDONLY);
    if (-1 == fd1) {
        perror("openat");
        return -1;
    }

    char buf1[MAX_BUF1FER_SIZE] = {0};
    int len = read(fd1, buf1, sizeof(buf1) - 1);
    if (-1 == len) {
        perror("read");
        return -1;
    }
    printf("fd1 read : %s", buf1);

    // 通过openat函数使用相对路径打开当前目录的文件
    int fd2 = openat(AT_FDCWD, "01.txt", O_RDONLY);
    if (-1 == fd1) {
        perror("openat");
        return -1;
    }

    char buf2[MAX_BUF1FER_SIZE] = {0};
    len = read(fd2, buf2, sizeof(buf2) - 1);
    if (-1 == len) {
        perror("read");
        return -1;
    }
    printf("fd2 read : %s", buf2);

    close(dir_fd);
    close(fd1);
    close(fd2);

    return 0;
}
~~~

其中`read`函数的返回值相关的讨论，我们后面再聊

使用`openat`函数可以看出，我们可以使用相对路径名打开目录中的文件，特别是其他目录中的，或者说我们可以把寻找文件的起始目录给改变，这一点是非常不错的，可以在多线程中使用相对路径名打开目录中的文件，同一进程中的所有线程共享相同的工作目录，但是很难让同一进程的不同线程在同一时间内工作在不同的目录当中；

第二就是可以避免`TOCTTOU`错误，以下是解释，了解即可：

![image-20230828200348003](https://image.davidingplus.cn/images/2025/01/31/image-20230828200348003.png)

## 函数creat

用来创建一个文件

~~~cpp
#include <fcntl.h>

int creat(const char *pathname, mode_t mode);

// 这个函数等效于
open(path, O_RDONLY | O_CREAT, mode);
~~~

**对于这个函数我们要注意一点，这个函数只能是以只读的方式创建文件的**，因此我们如果要写文件的话，还需要先将这个文件描述符关闭，然后再重新以读写的方式打开该文件才行，所以就有点麻烦

## 函数close

用来关闭一个文件描述符

~~~cpp
#include <unistd.h>

int close(int fd);
~~~

**关闭文件的同时还会释放该进程在该文件上的所有记录锁**，这个我们后面讨论

**当一个进程终止的时候，内核会自动关闭它所有打开的文件；很多程序都自动调用了这个功能而不显示调用`close`关闭文件，这是内核对我们的负责，但是我们在写代码的时候要注意代码的健壮性，有打开就有关闭**

但是内核的的确确帮我们做了很多事情，比如我们在利用标准输入输出`STDIN_FILENO`和`STDOUT_FILENO`的时候，这也是文件描述符，只不过是内核帮我们打开的，但是我们最后没有关闭它，而是在进程结束的时候内核关闭了

## 函数lseek

每个打开的文件描述符，都有一个与其相关联的当前文件偏移量，用来标识我们操作文件的时候从哪里开始，大小就是从文件开始处到此处的字节数；我们的读写操作都是从文件偏移量开始的，当到文件末尾之后函数就会返回，下一次就会重置

按照系统默认的情况，当打开一个文件的时候，除非是使用`O_APPEND`选项，否则偏移量都应该被设置为0

**`lseek`函数仅将当前的文件偏移量记录在内核当中，并不引起任何`I/O`操作，然后，该偏移量就用于下一个读或者写的操作**

~~~cpp
//标准C库的函数
#include <stdio.h>

int fseek(FILE *stream, long offset, int whence);

//Linux系统函数
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
~~~

标准库的`fseek`和`Linux`下面的系统调用`lseek`很类似，我们这里就说`lseek`

第一个参数是文件描述符，第二个参数是我们给的偏移量，这个偏移量可正可负，也就是说，我们的指针可以往前移

第三个参数`whence`：

- `SEEK_SET` 设置偏移量，从头开始
- `SEEK_CUR` 设置偏移量：当前位置 + 第二参数`offset`的值
- `SEEK_END` 设置偏移量：文件大小 + 第二参数`offset`的值

返回值是文件指针的新位置，失败返回-1并且修改`errno`

### 作用

那么这个函数就有很多作用了

- 移动文件指针到头部
  ~~~cpp
  lseek(fd, 0, SEEK_SET);
  ~~~

- 获取当前文件指针的位置
  ~~~cpp
  lseek(fd, 0, SEEK_CUR);
  ~~~

- 获取文件的长度
  ~~~cpp
  lseek(fd, 0, SEEK_END);
  ~~~

- 扩展文件的长度

  **为什么能扩展呢？我们想一下第三个参数我传入`SEEK_CUR`，然后这不是已经到文件末尾了吗？然后我第二个自己给的偏移量我给正数，按理来说不就超出文件边界了吗？但是内核帮我们把文件进行了扩展**

  ~~~cpp
  #include <iostream>
  using namespace std;
  #include <fcntl.h>
  #include <unistd.h>
  
  int main() {
      int fd = open("02.txt", O_RDWR);
      if (-1 == fd) {
          perror("open");
          return -1;
      }
  
      // 将文件扩展
      off_t offset = lseek(fd, 100, SEEK_END);
      if (-1 == offset) {
          perror("lseek");
          return -1;
      }
  
      // 需要立即进行一次写的操作，否则修改不会成功
      write(fd, " ", 1);
  
      close(fd);
  
      return 0;
  }
  ~~~

  我们发现在扩展了之后需要**立即**进行一次写的操作，否则修改就会失败，请注意我用的立即这个词语，这可能是因为如果没有立即进行写，内核会认为我们这样扩张文件是没有意义的，所以就会把扩展的行为给撤回，因此就会失败；

  **扩展之后的文件我们一般称作具有空洞的文件，因为我们加长了文件但是没有往其中写入任何内容**

  执行结果：

  其中包含了我们扩张的100字节和一次写的1个字节

  ![image-20230828203838354](https://image.davidingplus.cn/images/2025/01/31/image-20230828203838354.png)

当然并不是所有的文件描述符都可以被我们设置偏移量，比如匿名管道`PIPE`，有名管道`FIFO`，网络套接字等等都不能被设置，这时候`lseek`函数会返回-1，并且`errno`会被设置为``ESPIPE`

我们可以用一个程序来看下我们能否修改标准输入的偏移量

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    // 我们看能否修改标准输入的偏移量
    off_t offset = lseek(STDIN_FILENO, 100, SEEK_CUR);
    if (-1 == offset)
        perror("lseek");
    else
        printf("lseek ok.\n");

    return 0;
}
~~~

显而易见是不可以的的，执行结果：

![image-20230828204419474](https://image.davidingplus.cn/images/2025/01/31/image-20230828204419474.png)

### 补充(od命令)

我们可以用`od`命令去观察文件的实际内容，命令行中可以加上`-c`表示以字符方式打印文件内容，这个应该用的比较常见

命令如下：

~~~bash
od -c <filename>
~~~

例如，我用它来观察一个文本文件：

![image-20230828204950430](https://image.davidingplus.cn/images/2025/01/31/image-20230828204950430.png)

我也用它来观察可执行的二进制文件`a.out`

内容太多了包含不下

![image-20230828205027807](https://image.davidingplus.cn/images/2025/01/31/image-20230828205027807.png)

## 函数read

~~~cpp
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
// 参数：
	// fd：文件描述符，通过open得到的，通过文件描述符操作某个文件
	// buf：缓冲区，需要读取数据存放的地方，数组的地方(传出参数)
	// count：指定的数组的大小

// 返回值：
	// 成功 >0 返回实际读取到的字节数
	// ==0 文件已经读取完了(注意是在调用read函数之前文件指针就在末尾了才会返回0，一次性从头读完是会返回读取的字节数的)
	// 失败 -1 并且修改errno
~~~

当然在某些情况下实际读取到的字节数可能会小于我们要求读的字节数，这里我截图如下：

![image-20230829150051220](https://image.davidingplus.cn/images/2025/01/31/image-20230829150051220.png)

第三个参数的类型是`size_t`，当然我们也有类似的`ssize_t`，他们的原型如下：

~~~cpp
typedef unsigned long size_t; // 无符号整型
typedef long ssize_t; // 有符号整型
~~~

当然看到`long`，他和`int`类型还是有区别的，列图如下：

在我的电脑上`long`占据8个字节，`int`类型占据4个字节；当然还有`short`，占据2个字节

![image-20230829150521471](https://image.davidingplus.cn/images/2025/01/31/image-20230829150521471.png)

## 函数write

~~~cpp
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
// 参数：
    // fd：文件描述符，通过open得到，通过文件描述符操作某个文件
    // buf：要往磁盘写入的数据
    // count：要写入的实际的大小

// 返回值：
    // 成功 >0 返回实际写入的字节数
    // ==0 文件已经写入完了
    // 失败 -1 并且修改errno
~~~

`write`函数出错的一个常见原因就是磁盘已写满了，或者超过了一个给定进程的最大文件长度限制

### 例子

下面我们举一些例子，来看看`write`函数的写入到底是从哪里开始的

现在我准备一个文件`03.txt`，里面写入内容：

![image-20230829153031374](https://image.davidingplus.cn/images/2025/01/31/image-20230829153031374.png)

然后我们编写程序如下：

我们不用 `O_APPEND`，打开文件写一次，看发生什么现象

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>

int main() {
    // 打开一个已经存在的文件，不带 O_APPEND，然后看是从哪里开始写的
    int fd = open("03.txt", O_RDWR);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    // 开始写入
    const char* buf = "my name is your father.\n";
    write(fd, buf, strlen(buf));
    
    close(fd);

    return 0;
}
~~~

结果：

可见是从头开始写的

![image-20230829152129366](https://image.davidingplus.cn/images/2025/01/31/image-20230829152129366.png)

现在我尝试在一个程序当中多次调用write函数，但是我还是不加O_APPEND标志，我看后面的write会不会接着上一次的写入呢？

修改程序如下：

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>

int main() {
    // 打开一个已经存在的文件，不带 O_APPEND，然后看是从哪里开始写的
    int fd = open("03.txt", O_RDWR);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    // 开始写入
    const char* buf = "my name is your father.\n";
    // 在一个程序中连续不断的写入
    write(fd, buf, 5);
    write(fd, buf, 5);
    
    close(fd);

    return 0;
}
~~~

结果：

可见第二次的写入是跟着当前的文件偏移量走的

![image-20230829152416691](https://image.davidingplus.cn/images/2025/01/31/image-20230829152416691.png)

**因此，对于一般的文件，不加入`O_APPEND`，文件偏移量是从文件开头开始的，在一个程序中多次调用`write`函数，注意要操作的是一个文件描述符，关闭了再打开会被重置，又从头开始了，多次调用`write`函数的时候文件偏移量第一次从头开始，后面就是按照正常的偏移走，该怎么样就怎么样**

现在我们恢复文件中的内容，加上`O_APPEND`来看看会怎么样：

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>

int main() {
    // 打开一个已经存在的文件，带或者不带O_APPEND，然后看是从哪里开始写的
    // int fd = open("03.txt", O_RDWR);
    int fd = open("03.txt", O_RDWR | O_APPEND);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    // 开始写入
    const char* buf = "my name is your father.\n";
    write(fd, buf, strlen(buf));
    close(fd);

    return 0;
}
~~~

结果：

可见是追加上去的

![image-20230829153056681](https://image.davidingplus.cn/images/2025/01/31/image-20230829153056681.png)

多次调用也是追加效果，我就不演示了

**总结以下，不管是带不带`O_APPEND`，就一个区别，就是写入文件的起始文件偏移量的位置，不带就是从头开始，带的话就是在文件末尾开始追加，在关闭文件描述符之前后续的操作就是正常的该怎么样就怎么样**

### 补充

我们根据这个例子来看看`strlen`和`sizeof`的区别：

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;

int main() {
    const char* str = "helloworld\n";
    cout << strlen(str) << endl
         << sizeof(str) << endl;

    cout << endl;

    char buf[1023] = {0};
    cout << strlen(buf) << endl
         << sizeof(buf) << endl;

    return 0;
}
~~~

结果：

![image-20230829154907298](https://image.davidingplus.cn/images/2025/01/31/image-20230829154907298.png)

我们先看第一个`str`字符串，`strlen()`是获取他的长度，我们也可以实现这个函数，判断`'\0'`即可；`sizeof()`在这里放入`str`，`str`是一个指针，类型是`char*`，64位操作系统上指针是8个字节，所以返回8

第二个是开的`char`数组，大小预设为1023，`strlen()`仍然是获取长度，为0；`sizeof()`这时候就指的是数组的整体大小了，所以返回1023

## 文件共享

UNIX系统支持在不同进程间共享打开文件；我们了解一下内核用于打开文件的数据结构是什么

先画一张图：

![image-20230829160203372](https://image.davidingplus.cn/images/2025/01/31/image-20230829160203372.png)

**进程表，也称进程控制块（`PCB`），是由操作系统维护的，每个进程占用其中一个表项。该表项包含了操作系统对进程进行描述和控制的全部信息，从而保证该进程换出后再次启动时，就像从未中断过一样。**

**每个进程在进程表当中都有自己的一个记录项，在文件方面记录项会维护一张文件描述符表，里面的索引就是文件描述符的`int`值，每项存储的包括文件描述符标志（目前只定义了一个标志`FD_CLOEXEC`）和指向一个文件表的指针**

**文件表还没有到存放文件实际数据的地方，它是由内核维护的，里面记录的内容包括文件状态标志，就是读，写，读写这些；还有就是当前的文件偏移量；最后是指向一个v结点的指针**

我们来看看v结点是指什么？我截了书上的原图：

**v结点和i结点的信息是打开文件的时候就从磁盘读入到内存里了，是属于用户区的；前面的文件描述符表和文件表项是属于内核区**

![image-20230829161117279](https://image.davidingplus.cn/images/2025/01/31/image-20230829161117279.png)

如果有两个独立进程各自打开了磁盘上的同一个文件，那么他们的图是这样的：

**由于只有一份文件，因此v结点的信息只有一份，两个进程中的文件表项的v结点指针指向的是同一处位置**

**由于我们每个进程都可以获得自己对这份文件的文件偏移量，因此我们每个进程针对这份同个文件都可以获得自己的文件项**

![image-20230829161314772](https://image.davidingplus.cn/images/2025/01/31/image-20230829161314772.png)

**当然很可能有多个文件描述符指向同一个文件表项**，这个我们后面再说

## 原子操作

### 例子

**顾名思义，原子操作就是不能被分割的操作，否则就会出现问题；书上的定义是原子操作是由多步组成的一个操作，说的没错，他们分开执行的话有时容易出问题**；我们举一个例子来说明：

独立的进程A和B对同一文件进行追加操作，进程都打开了文件，但是没使用`O_APPEND`标志，那么怎么办呢？

那肯定是先移动文件偏移量到末尾，然后开始写文件

~~~cpp
lseek(fd, SEEK_END, 0);
write(fd, buf, strlen(buf));
~~~

对单个程序而言，这个似乎没有什么问题，但是多个进程可能会出问题

**我们设想A先执行，设置到末尾，然后这个时候内核切换到进程B，B也设置到末尾，然后开始写，假设B写了100个字节；然后切换，A开始写，没有人通知A进程B已经修改了文件，那么A就在刚才的基础上进行修改，B的内容就会被覆盖，这就是问题**

问题就是先定位到末端在写入数据，这是两个分开的系统调用，解决问题的办法是把这两个操作合在一起成为一个原子操作，要么执行就两步一起执行完，要么就不执行，因此内核在打开文件的时候提供了`O_APPEND`标志，合理的解决了这个可能出现的问题(可能实际情况基本不会发生，但是还是有这种可能性)

### 函数pread和pwrite

这两个函数可以指定从某个偏移量位置开始读或者写

~~~cpp
#include <unistd.h>

ssize_t pread(int fd, void buf[.count], size_t count, off_t offset);

ssize_t pwrite(int fd, const void buf[.count], size_t count, off_t offset);
~~~

参数中的`offset`就是我们可以指定的偏移位置

**`pread`相当于先调用`lseek`再调用read，但是区别是`pread`调用的时候无法被中断操作，也就是是一个原子操作，这就可以用来解决前面的问题；`pwrite`同理类似**

**并且这两个函数执行完毕之后，调用之前的文件偏移量不会被修改**，我们相当于进行了一次不影响大局的额外操作

### 另一个例子

书上有另一个例子，是关于`O_CREAT`标志的，这里就不赘述了

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230829164021537.png" alt="image-20230829164021537" style="zoom:80%;" />

## 函数dup和dup2

### dup

~~~cpp
#include <unistd.h>
int dup(int oldfd);
// 作用：复制一个新的文件描述符，他们是指向同一个文件的，只是用了两个文件描述符
// 新的文件描述符会使用空闲的文件描述符表中最小的那个!!!
~~~

看到这个函数的作用就知道上面说可能有多个文件描述符指向同一个文件表项的意思了，这里就体现出来了

### dup2

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

总结`dup2`的作用就是：**关闭`fd2`指向指向的文件的连接，然后用当前的文件描述符索引，指向`fd1`指向的文件**；如果fd1和fd2相等，那么就啥也没干；这个函数可以使用`fd2`参数之前使用的文件描述符索引值而不是文件描述符中最小的那个

这两个函数执行过后，文件描述符和文件表项大致长这个样子：

可以看出有多个文件描述符指向了同一块文件表

![image-20230829165153954](https://image.davidingplus.cn/images/2025/01/31/image-20230829165153954.png)

### 关于fcntl

另一个复制文件描述符的方法是调用`fcntl`函数，这个后面具体说明

对于`dup`函数，这样是等效的：

~~~cpp
dup(fd);

fcntl(fd, F_DUPFD, 0);
~~~

对于`dup2`函数，这样是等效的：

~~~cpp
dup2(fd1,fd2);

close(fd2);
fcntl(fd1, F_DUPFD, fd2);
~~~

其实这两个并不完全等效，`dup2`是个原子操作，下面的两句话是分开的；另外一个就是`dup2`和`fcntl`的错误号`errno`有一些区别

## 函数sync，fsync和fdatasync(了解)

这部分内容了解即可，我就截图了

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230829170048031.png" alt="image-20230829170048031" style="zoom:80%;" />

## 函数fcntl

`fcntl`函数可以改变已经打开的文件的属性；**这个函数可以用来复制文件描述符或者设置/获取文件的状态标志**

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

除了我们平时用的很多的这两个作用，书上总结了以下五个作用，有重复：

![image-20230829195127023](https://image.davidingplus.cn/images/2025/01/31/image-20230829195127023.png)

**注意第二条和第三条的区别，一个是文件描述符的标志，目前只定义了一个文件描述符标志`FD_CLOEXEC`，这个是干嘛的我现在不知道；另一个是文件状态的标志，这就是我们`open`函数的第二个参数`flags`，比如`O_RDONLY`，`O_WRONLY`，`O_RDWR`；注意这两个的区别**

我们看一下一些可能用到的cmd的用法：

复制文件描述符的操作前面有提到过，用的比较多的就是复制，重定向用的比较少，因为需要经过两步操作，我们最好用dup2的原子操作

文件描述符复制可以这么写，这种情况下函数返回新的文件描述符标志`newfd`

~~~cpp
dup(fd);
// 等效于
fcntl(fd, F_DUPFD, 0);
~~~

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230829200156270.png" alt="image-20230829200156270" style="zoom:80%;" />

这个`F_GETFL`用的比较多，作用是用来获取文件状态标志，这些标志是用二进制存储的，表现出来就是一个数字，如果我们想要叠加状态标志的话就用按位或 `|` ，以下列出了文件状态的很多标志，这个在前面`open`函数的时候也提到过

![image-20230829200209943](https://image.davidingplus.cn/images/2025/01/31/image-20230829200209943.png)

`F_SETFL`是用来设置文件状态标志，我们要对已有的文件描述符进行操作的话，一般需要先获取现在的文件状态标志，然后再进行按位或叠加 `|` ，然后再设置进去

例如，这里我们将`read`函数要读的文件设置为非阻塞读取

~~~cpp
int flag = fcntl(fd, F_GETFL);
flag |= O_NONBLOCK;
fcntl(fd, F_SETFL, flag);
~~~

![image-20230829201513184](https://image.davidingplus.cn/images/2025/01/31/image-20230829201513184.png)

![image-20230829201520327](https://image.davidingplus.cn/images/2025/01/31/image-20230829201520327.png)

`fcntl`函数的返回复制和修改文件状态标志用的比较多，这个用的时候自己就明白了；所有情况下错误返回-1，并且修改`errno`

### 例子

我们写一个程序来观察以下文件状态标志

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>

void set_flag(const int& fd, const int& add_flag) {
    int flag = fcntl(fd, F_GETFL);
    flag |= add_flag;
    fcntl(fd, F_SETFL, flag);
}

int main() {
    // 如果不存在则创建
    if (0 != access("04.txt", F_OK)) {
        int fd1 = open("04.txt", O_RDWR | O_CREAT, 0664);
        if (-1 == fd1) {
            perror("open");
            return -1;
        }

        close(fd1);
    }

    // 打开文件
    int fd = open("04.txt", O_RDWR);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    // 查看文件状态标志
    int flag = fcntl(fd, F_GETFL);
    // 先看必有的三个状态，只读，只写，读写
    if (flag & O_RDONLY)
        cout << "read only" << endl;
    else if (flag & O_WRONLY)
        cout << "write only" << endl;
    else if (flag & O_RDWR)
        cout << "read write" << endl;
    else
        cout << "unknown access mode" << endl;

    // 可以选择再看一些其他的状态标志，这里我就看O_NONBLOCK
    if (flag & O_NONBLOCK)
        cout << "nonblock" << endl;
    else
        cout << "not nonblock" << endl;

    // 设置fd为非阻塞状态标志
    set_flag(fd, O_NONBLOCK);

    // 然后我再看一次
    flag = fcntl(fd, F_GETFL);
    if (flag & O_NONBLOCK)
        cout << "nonblock" << endl;
    else
        cout << "not nonblock" << endl;

    return 0;
}
~~~

结果：

可见我们的设置是成功的

![image-20230829203611074](https://image.davidingplus.cn/images/2025/01/31/image-20230829203611074.png)

我们在设置文件状态标志`flag`的时候，我自己写了一个函数

这很好理解，我先获取，然后修改，最后设置进去

~~~cpp
void set_flag(const int& fd, const int& add_flag) {
    int flag = fcntl(fd, F_GETFL);
    flag |= add_flag;
    fcntl(fd, F_SETFL, flag);
}
~~~

当然，如果我们把第二行修改为：

这就是把这个标志从当中删除，原理就是这一位为0，其他为1，然后和标志相与，就可以只把这一位设置为0

~~~cpp
void del_flag(const int& fd, const int& add_flag) {
    int flag = fcntl(fd, F_GETFL);
    flag &= ~add_flag;
    fcntl(fd, F_SETFL, flag);
}
~~~

### 补充(了解)

~~~cpp
void set_flag(const int& fd, const int& add_flag) {
    int flag = fcntl(fd, F_GETFL);
    flag |= add_flag;
    fcntl(fd, F_SETFL, flag);
}

// 我们调用这行代码
set_flag(fd, O_SYNC);
~~~

我们看`O_SYNC`状态标志：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230829204551172.png" alt="image-20230829204551172" style="zoom:150%;" />

**其实在UNIX系统中，通常write只是将数据排入队列，而实际写的操作可能在之后的某个时刻完成，这个就叫延迟写；但是在这段时间内write函数已经返回并且程序继续执行了**

**这样的话有可能会造成数据丢失，因为我们没有办法知道数据是否写入，只有当我们打开文件的时候才能检查。所以这里我们加入`O_SYNC`状态标志，这个标志要求每次`write`都要等待数据写到磁盘之上然后再返回，这个就叫同步写；数据库系统一般需要使用`O_SYNC`，这样我们能知道确实数据写到了磁盘上**

## 函数ioctl(了解)

`itocl`函数是`I/O`操作的杂物箱；不能用其他函数表示的`I/O`操作通常都用`ioctl`表示；终端`I/O`是`ioctl`用的最多的地方

~~~cpp
#include <sys/ioctl.h>

int ioctl(int fd, unsigned long request, ...);
~~~

这个函数看不懂书上讲的什么，我把图贴上来：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230829205620460.png" alt="image-20230829205620460" style="zoom:80%;" />

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230829205626464.png" alt="image-20230829205626464" style="zoom:80%;" />

## /dev/fd

我的系统上提供了`/dev/fd`目录，我们打开它看一下有什么内容：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230829210020506.png" alt="image-20230829210020506" style="zoom: 80%;" />

可以看出，目录项名为0，1，2的文件在里面，我猜测他们对应标准输入，标准输出和标准错误，打开`/dev/fd/n`等效于复制文件描述符n(我们假设文件描述符n是打开的)

我们用程序来说明：

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>

int main() {
    int fd = open("/dev/fd/0", O_RDONLY);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    cout << fd << endl;  // 3 说明是复制了这个文件描述符，他们两个指向同一个文件表项

    return 0;
}
~~~

**我们在`open`函数的第二个参数传入了我们打开文件的文件状态标志，但是对于`/dev/fd/n`来说，系统基本上是忽略这个标志的，因为我们打开的是系统文件，对应的是系统的文件描述符，不能这么随意就让状态标志让我们给改了**

**所以，如果我们执行下面的代码，如果文件描述符0之前就被标记为只读，即使我们这行代码执行成功，我们依然没有办法对`fd`进行写操作**

~~~cpp
int fd = open("/dev/fd/0", O_RDWR);
~~~

**在Linux上面这么做要非常小心，因为Linux上在打开这些文件的时候实际上是把文件描述符映射成为了指向底层物理文件的符号链接(软链接)，比如上面就是`/dev/pts/4`，意识是比如我们打开0，我们在操作和标准输入相关的系统文件了，如果操作不得当很可能出现问题，所以我们要格外注意**

# 第四章 文件和目录

## 函数stat，fstat，fstatat和lstat

~~~cpp
#include <sys/stat.h>

int stat(const char *restrict pathname, struct stat *restrict statbuf);

int fstat(int fd, struct stat *statbuf);

int lstat(const char *restrict pathname, struct stat *restrict statbuf);

#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>

int fstatat(int dirfd, const char *restrict pathname, struct stat *restrict statbuf, int flags);
~~~

这里里面用的最多的就是`stat`函数，它可以用来得到与文件相关的信息，并且存储在传出参数`statbuf`当中，我们先谈谈`stat`函数

### 函数stat

~~~cpp
int stat(const char *pathname, struct stat *statbuf);
//作用：获取一个文件的相关的信息
//参数：
    //pathname：操作的文件路径
    //statbuf：结构体变量，传出参数，用于保存获取到的文件信息
//返回值：
    //成功 0
    //失败 -1，并且修改errno
~~~

里面又有一个参数结构体叫`stat`结构体，与函数同名，但是这里是一个结构体，我们看看这个结构体里面存放的是什么东西

![image-20230714152755154](https://image.davidingplus.cn/images/2025/01/31/image-20230714152755154.png)

**里面用的比较多的是类型是`mode_t`的`st_mode`变量，这个变量用来记录文件的类型和存取的权限，`mode_t`实际上是`typedef unsigned int mode_t`，因此又是用每一位来表示有无的例子，合起来就是一个数字，加入记得用按位或`|`**

**下面的这些数字全部都是八进制数字，前四位表示的是文件类型，紧接着三位表示特殊权限位，然后后面的每三位分别表示用户，组和其他的对该文件的权限，可读r，可写w，可执行x**

**前四位里面存在一个掩码，我们要判断文件类型的时候就用值和掩码想与然后判断是哪一个类型就好了；后面权限的判断也同理**

![image-20230714153204023](https://image.davidingplus.cn/images/2025/01/31/image-20230714153204023.png)

当然`shell`终端也可以使用`stat`命令，例如：

![image-20230830150500142](https://image.davidingplus.cn/images/2025/01/31/image-20230830150500142.png)

### 其他函数

1. `fstat`函数

   **`fstat`函数用来获得已经打开的文件的相关信息，它的参数需要的是一个已经打开的文件描述符**

   **它和`stat`函数对比的话，`stat`就相当于是先打开这个文件获取文件描述符然后获取这个文件的信息结构，是一个原子操作**

   ~~~cpp
   int fstat(int fd, struct stat *statbuf);
   //作用：获取一个已经打开的文件描述符相关的信息
   //参数：
       //fd：文件描述符
       //statbuf：结构体变量，传出参数，用于保存获取到的文件信息
   //返回值：
       //成功 0
       //失败 -1，并且修改errno
   ~~~

2. `lstat`函数

   **`lstat`函数基本功能和stat函数没有区别，但是遇到我们要获取的文件是一个符号链接(软链接)的时候就有区别了，下面是一个例子**

   **`2.txt`文件指向`1.txt`，这是一个链接文件，并且是软链接；这时候我们通过`lstat`函数获取`2.txt`就是获取`2.txt`这个链接文件本身的信息，如果我们用`stat`函数获取的就是`1.txt`的信息，这就是区别**

   ![image-20230714154550861](https://image.davidingplus.cn/images/2025/01/31/image-20230714154550861.png)

3. `fstatat`函数

   这个函数提供了更多的功能，但是一般我们都可以用前面的函数来代替，所以这里我就简单写一下就好，用的时候来查询就好了

   ~~~cpp
   int fstatat(int dirfd, const char *restrict pathname, struct stat *restrict statbuf, int flags);
   // 参数：
   	// dirfd：文件目录的文件描述符，注意打开的是文件目录，为什么我们后面看
   	// pathname：
   		// 如果是绝对路径，那么dirfd参数会被忽略
   		// 如果是相对路径，并且dirfd参数是AT_FDCWD(和openat有点类似)，那么在dirfd的目录基础上进行定位
   	// statbuf：传出参数，存储文件的信息的数据结构
   	// flags：当设置为AT_SYMLINK_NOFOLLOW的时候，对于软链接文件，不会继续定位他指向的文件，会记录这个文件本身的信息
   	//        默认情况下返回这个符号链接(软链接)实际指向的文件信息
   ~~~

以上这四个函数**成功都返回0，失败都返回-1并且设置错误号**

### 例子

下面我们写一个程序来模拟实现 `ls -l` 命令，命令行参数传入文件名

给一个`ls -l` 的例子：

![image-20230830150739907](https://image.davidingplus.cn/images/2025/01/31/image-20230830150739907.png)

我们就试着实现这个程序

~~~cpp
#include <ctime>
#include <iostream>
#include <string>
using namespace std;
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>

#define MAX_FILEINFO_SIZE 1024

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        printf("usage: %s  <filename>\n", argv[0]);
        return -1;
    }

    const char* filename = argv[1];

    struct stat stat_buf;

    int ret = stat(filename, &stat_buf);
    if (-1 == ret) {
        perror("stat");
        return -1;
    }

    // 开始解读stat_buf数组的文件信息内容
    string file_info;

    // 获取文件类型
    switch (stat_buf.st_mode & S_IFMT) {
    case S_IFSOCK:
        file_info.append("s");
        break;
    case S_IFLNK:
        file_info.append("1");
        break;
    case S_IFREG:
        file_info.append("-");
        break;
    case S_IFBLK:
        file_info.append("b");
        break;
    case S_IFDIR:
        file_info.append("d");
        break;
    case S_IFCHR:
        file_info.append("c");
        break;
    case S_IFIFO:
        file_info.append("p");
        break;
    default:
        file_info.append("?");
        break;
    }

    // 获取三个权限
    file_info.append(stat_buf.st_mode & S_IRUSR ? "r" : "-");
    file_info.append(stat_buf.st_mode & S_IWUSR ? "w" : "-");
    file_info.append(stat_buf.st_mode & S_IXUSR ? "x" : "-");

    file_info.append(stat_buf.st_mode & S_IRGRP ? "r" : "-");
    file_info.append(stat_buf.st_mode & S_IWGRP ? "w" : "-");
    file_info.append(stat_buf.st_mode & S_IXGRP ? "x" : "-");

    file_info.append(stat_buf.st_mode & S_IROTH ? "r" : "-");
    file_info.append(stat_buf.st_mode & S_IWOTH ? "w" : "-");
    file_info.append(stat_buf.st_mode & S_IXOTH ? "x" : "-");

    // 获取文件硬连接数
    nlink_t nlink_num = stat_buf.st_nlink;

    // 获取文件所有者
    // getpwuid()通过用户ID反向获得用户名称
    // 引头文件 <grp.h>
    string User = getpwuid(stat_buf.st_uid)->pw_name;

    // 获取文件所在组
    // getgrgid()通过用户组ID反向获得组名称
    // 引头文件 <pwd.h>
    string Group = getgrgid(stat_buf.st_gid)->gr_name;

    // 获得文件的大小
    off_t file_size = stat_buf.st_size;

    // 获取修改时间
    // ctime()函数可以将时间差值转化为本地时间
    string mtime = string(ctime(&stat_buf.st_mtime));
    // 这个时间格式化之后回车换行了，将其去掉
    mtime.pop_back();

    // 格式化信息
    char buf[MAX_FILEINFO_SIZE] = {0};
    sprintf(buf, " %ld %s %s %ld %s %s", nlink_num, User.c_str(), Group.c_str(), file_size, mtime.c_str(), filename);

    file_info += buf;

    // 输出信息
    cout << file_info << endl;

    return 0;
}
~~~

结果：

可见实现的还不错，注意代码中怎么通过用户ID和组ID反向获得用户名和组名，以及对时间的格式化

![image-20230830153051111](https://image.davidingplus.cn/images/2025/01/31/image-20230830153051111.png)

## 文件类型

**我们一般用的比较多的两个文件类型就是普通文件和目录**，当然还有一些其他的类型

截图如下：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230830154128435.png" alt="image-20230830154128435" style="zoom:80%;" />

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230830154139467.png" alt="image-20230830154139467" style="zoom:80%;" />

**从第一条我们知道，普通文件里面存放的是什么数据，比如是文本还是二进制数据，对内核而言无所紧要；但是内核想要执行二进制文件，那肯定需要对二进制文件的格式要进行规范标准化，这样才能确定程序文本和数据的加载位置**

**目录文件，里面包含了目录当中文件的名字和指向这些文件信息的指针；对一个目录文件具有读权限的进程能够读目录下的任意文件的内容，但是只有内核能直接对目录文件进行写操作，进程要写需要调用系统API通过内核操作才能进行修改**

其他的文件看图即可

关于查看文件的类型，书上给出了一个例子，这个例子已经在我们模拟`ls -l`命令的时候已经做过了，见前面

## 设置用户ID和组ID

与进程相关的ID有6个之多，截书上的图说明如下：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230830171807717.png" alt="image-20230830171807717" style="zoom:80%;" />

通常，有效用户ID等于实际用户ID，有效组ID等于实际组ID

每一个文件都有一个所有者和一个组所有者，所有者由结构体`stat`中的变量`st_uid`指定，组所有者由变量`st_gid`指定

## 文件访问权限

`st_mode`里面也包含了对文件的访问权限；**值得注意的是，这里的文件不仅仅是指普通文件，其他的任何类型，包括目录，字符特别文件等等，都具有访问权限**

文件访问权限有9位，每三位分别代表用户，组，其他的读，写，可执行权限，具体如下：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230830184628138.png" alt="image-20230830184628138" style="zoom:80%;" />

### 目录的可读和可执行权限

我们要说明一下一个目录的可读和可执行权限

例如，我们要打开文件`/usr/include/stdio.h`，我们需要依次访问目录 `/`(根目录)，`/usr`和`/usr/include`这三个目录；对于`stdio.h`文件的访问权限我们不做讨论；

**对于目录而言，可读的权限代表这我们可以读这个目录，获得这个目录下的所有文件列表；可执行权限代表我们可以搜索这个目录，找到我们需要找到的指定的文件或者目录**，这两个不要弄混了

### 文件的访问权限和open函数中的文件状态标志之间的关系

提到这里我们不得不提一下文件自身的访问权限和我们`open`函数打开文件指定的文件状态标志的关系了

比如我们`open`函数用读写的方式打开一个文件，按照常理来说是没问题的，但是如果这个文件是只读的，也就是不允许我们写；那么这个时候就会有所不同，我们用读写的方式打开文件就会报错，见下面程序：

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>

int main() {
    int fd = open("02.txt", O_RDWR);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    return 0;
}
~~~

结果：

可见并不是我们指定怎么访问就怎么访问，还得看文件本身的访问权限

![image-20230830190310031](https://image.davidingplus.cn/images/2025/01/31/image-20230830190310031.png)

### 关于进程打开文件

当一个进程需要打开，创建或者删除一个文件的时候，内核就对文件进行访问权限的设置，具体测试如下(了解)：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230830190756047.png" alt="image-20230830190756047" style="zoom:80%;" />

我们只需要知道的就是，**如果想要正确的访问文件，这个文件就应该提供相应的权限给我，否则即使我要求用这种方式访问也是不行的**

## 新文件和目录的所有权

新文件的用户ID设置为进程的有效用户ID

组ID选择下面二者其一：

- 可以是进程组的有效组ID
- 可以是所在目录的组ID

## 函数access和faccessat

~~~cpp
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
~~~

`faccessat`函数和`access`函数类似，但是有一些区别：

~~~cpp
int faccessat(int dirfd, const char *pathname, int mode, int flags);
// dirfd，看这个形式，就是知道我们需要传入的是一个目录的文件描述符，所以open函数记得加上O_DIRECTORY
// pathname：
	// 绝对路径，那么这个时候dirfd被忽略，就找指定绝对路径的文件，来看他的权限
	// 相对路径，如果dirfd传入AF_FDCWD，那么相对路径从本程序的工作目录开始计算；不是就从dirfd打开的目录开始计算
// mode：需要检测的一些权限，同上面
// flags：
	// 这个参数一般我们用不上，默认给0就行了
	// 但是我们传入AT_EACCESS，访问检查用的是调用进程的有效用户ID和组ID，而不是实际用户ID和组ID(我目前不知道作用)
~~~

### 例子

我们写一个程序来看一下该怎么使用

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* const argv[]) {
    if (argc < 2) {
        printf("usage: %s  <filename>\n", argv[0]);
        return -1;
    }

    const char* filename = argv[1];

    // 检测这个文件是否具有读权限
    if (0 == faccessat(AT_FDCWD, filename, R_OK, 0))
        cout << "read access ok" << endl;
    else
        perror("read access error");

    // 看打开这个文件会不会被拒绝
    if (-1 != open(filename, O_RDONLY))
        cout << "open for reading ok" << endl;
    else
        perror("open for reading error");

    return 0;
}
~~~

结果：

![image-20230830195427643](https://image.davidingplus.cn/images/2025/01/31/image-20230830195427643.png)

## 函数umask

这个函数用来**设置进程在创建文件时候的访问权限屏蔽字**，什么意思呢？

我们先来看看open函数创建文件时候的情况

~~~cpp
int fd = open("test.txt", O_RDWR, 0664);
~~~

第三个参数，这里我给的是八进制数0664，664每位数字分别代表3位，就是用户，组和其他的可读，可写和可执行权限

### 进制数表示

说到这里我就必须提一下C语言对二进制，八进制和十六进制数的表示方法：

- **二进制由 0 和 1 两个数字组成，使用时必须以`0b`或`0B`（不区分大小写）开头，例如：0b101**
- **八进制由 0~7 八个数字组成，使用时必须以`0`开头（注意是数字 0，不是字母 o），例如：0664**
- **十六进制由数字 0~9、字母 A~F 或 a~f（不区分大小写）组成，使用时必须以`0x`或`0X`（不区分大小写）开头，例如：0x2A**

所以这里的0664显然是一个八进制数

### 实际的文件访问权限

我们给他一个八进制数，那么内核是不是就把我们的这个数应用到文件的访问权限上了呢？

其实不是，在我们终端的进程打开的时候系统会初始化一个变量叫`umask`，我的电脑上是0022，这也是一个八进制数：

![image-20230830200859566](https://image.davidingplus.cn/images/2025/01/31/image-20230830200859566.png)

它有什么用呢？

我们先看书上的定义，`umask`函数是设置进程在创建文件时候的访问权限屏蔽字，请注意这里的**屏蔽**二字，**实际上我们设置的权限要和系统的`umask`进行一下运算，得到的`mode`才是真正的访问权限**

~~~cpp
mode & = ~umask;
~~~

**我们想一下，取反就是把`umask`想要屏蔽的位置为0，不屏蔽的位置为1；然后与我们设置的想与，实际上就是想把它想屏蔽的权限设置位0，也就是我们想设置这个权限，但是他不给我们，我们设置了也没用**

**因此`umask`函数的实际作用其实就相当于在进程中修改`umask`，也就是他说的创建访问权限屏蔽字，这样屏蔽的权限我们没办法设置出来**

### 例子

我们先不设置`umask`，然后设置一定的屏蔽字，看看执行结果：

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// 定义一个权限宏
#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int main() {
    // 先将umask置为0
    umask(0);

    // 先正常创建一个文件，这里我用creat函数
    if (0 != access("foo.txt", F_OK))
        remove("foo.txt");

    int fd1 = creat("foo.txt", RWRWRW);
    if (-1 == fd1) {
        perror("creat");
        return -1;
    }

    // 然后调用umask创建屏蔽字
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (0 != access("bar.txt", F_OK))
        remove("bar.txt");

    int fd2 = creat("bar.txt", RWRWRW);
    if (-1 == fd2) {
        perror("creat");
        return -1;
    }

    return 0;
}
~~~

我的电脑自己的用户是超级用户，给了满权限，所以这里用书上的结果：

![image-20230830202932811](https://image.davidingplus.cn/images/2025/01/31/image-20230830202932811.png)

但是无关紧要，我们知道`umask`函数和`umask`屏蔽字在文件创建的时候的作用就好

### shell命令

我们可以用`shell`命令来查看系统给我们创建的`umask`屏蔽字并且进行修改，

我们可以用`-S`参数来直观的查看屏蔽了哪些，具体如下：

![image-20230830204021285](https://image.davidingplus.cn/images/2025/01/31/image-20230830204021285.png)

## 函数chmod，fchmod和fchmodat

又是一组类似的函数，显然`chomd`函数是基础，后面的两个都是在这个基础上进行改造

通过这三个函数，我们可以**更改当前文件的访问权限**

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

另外两个函数也整理如下：

~~~cpp
#include <sys/stat.h>

int fchmod(int fd, mode_t mode);
// 是对一个已经打开的文件进行操作，所以传入的是文件描述符fd；
// chmod是open和fchmod合并起来的原子操作

int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);
// 看到这个函数和前面at后缀结尾的函数族非常类似，他们的模式是一样的，我直接复制笔记
// dirfd，看这个形式，就是知道我们需要传入的是一个目录的文件描述符，所以open函数记得加上O_DIRECTORY
// pathname：
	// 绝对路径，那么这个时候dirfd被忽略，就找指定绝对路径的文件，来看他的权限
	// 相对路径，如果dirfd传入AF_FDCWD，那么相对路径从本程序的工作目录开始计算；不是就从dirfd打开的目录开始计算
// mode：需要检测的一些权限，同上面
// flags：这里按照具体的功能有区别
	// 这个参数一般我们用不上，默认给0就行了
	// 但是我们传入AT_SYMLINK_NOFOLLOW(参考lstat)，遇到符号链接(软链接)不会继续追踪，而是就处理这个符号链接文件
~~~

**为了改变一个文件的访问权限，进程的有效用户ID必须等于文件的所有者ID，或者该进程是超级用户权限**，我的就是

以下是可能设置的一些权限宏：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230830205536347.png" alt="image-20230830205536347" style="zoom:80%;" />

### S_ISUID和S_ISGID

可以参考博客：[https://blog.csdn.net/u011580175/article/details/107749465](https://blog.csdn.net/u011580175/article/details/107749465)，这里我结合他的笔记和书上的例子进行总结

- 对于文件

  对于文件的`S_ISUID`，我们举个例子：

  **`passwd`，原理是通过修改`/etc/shadow`文件从而实现用户账号密码的修改。而`shadow`文件只有`root`用户可以修改的。那普通用户为什么可以通过`passwd`修改字节的账号密码呢。**

  **其原理其实就是，`passwd`程序`st_mode`被设置了，`S_ISUID`。此时，因为`passwd`文件的所有者是`root`用户，由于设置了`S_ISUID`，在程序运行时，程序的有效用户`ID`会被设置为`root`。所以，程序拥有`root`用户的权限，可以修改`shadow`文件。**

  **如果没有设置`S_ISUID`，则`passwd`被A用户运行时，有效用户`ID`会被设置为A的``uid。则只会有A用户的权限。**

- 对于文件夹

  **对于文件夹的`S_ISGID`，如果文件夹A设置了`S_ISGID`，如果在A文件夹内创建B文件夹，则B文件夹的组`ID`默认为A文件夹的组`ID`。如果，没设置，则为创建文件夹的进程的有效组`ID`。**这里和书上讲的一致

## 黏着位(了解)

黏着位叫`S_ISVTX`；如果黏着位被设置了，只有对该目录具有写权限的用户并且满足下列条件之一，才能删除或者重命名该目录下的文件：

- 拥有此文件
- 拥有此目录
- 是超级用户

## 函数chown，fchown，fchownat和lchown

又是一组类似的函数，毫无疑问我们先研究函数`chown`，**它可以用来修改文件的用户ID和组ID**

~~~cpp
#include <unistd.h>

int chown(const char *pathname, uid_t owner, gid_t group);
// 作用：可以用来修改文件的用户ID和组ID
// 参数：(从左到右)文件路径，绝对路径或者相对路径；用户ID；组ID
	 // 如果参数用户ID owner和组ID group被设置为-1，则对应的ID不做修改
~~~

其他三个函数和前面的族如出一辙，我就简单写了：

~~~cpp
#include <unistd.h>

int fchown(int fd, uid_t owner, gid_t group);
// 用来修改一个已经打开的文件的用户ID和组ID，所以传入的参数是文件描述符fd

int lchown(const char *pathname, uid_t owner, gid_t group);
// 和chown功能基本相同
// 区别就在于链接文件，lchown函数会作用于该链接文件本身而不是他指向的文件

int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags);
// 首先dirfd，我们需要的是一个已经打开的文件目录，所以open函数记得加上参数O_DIRECTORY
// pathname：
	// 绝对路径，这个时候dirfd参数会被忽略
	// 相对路径：dirfd如果设置为AF_FDCWD，则从当前工作目录开始找；否则就是我们自己打开的目录开始找
// flags：
	// 如果设置了AT_SYMLINK_NOFOLLOW标志，则会作用于链接文件本身而不是指向的文件
	// 如果没设置，例如0，那么就是一般的行为
~~~

## 文件长度

**`stat`的结构成员`st_size`表示以字节为单位的文件的长度。此字段只对普通文件，目录文件和符号链接(软链接)有意义!**

例如：

在这个文件夹当中，目录文件 `./` 和 `../` 的大小是有意义的；普通文件`text.txt`和链接文件`test2.txt`的大小是有意义的；但是我创建了一个有名管道`FIFO`，从结果中可以看出大小是0，代表着这个参数对他没有意义

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230831133347919.png" alt="image-20230831133347919" style="zoom:80%;" />

**对于普通文件，其文件长度与里面存放的数据的大小相关，是以字节数为单位；对于链接文件，文件长度是指向的文件名的实际字节数**，比如上面的`test2.txt`是链接文件，他指向文件`test.txt`，他的文件名长度是8个字节，所以`test2.txt`的文件长度是8个字节；**对于目录，文件长度通常是16或者512的整数倍**，这里是4096，符合要求

### 文件中的空洞

我们前面也提到过，用`lseek`函数可以修改文件的偏移量，当修改的偏移量超过文件本身的大小的时候，内核就会有帮我们扩充文件大小的想法，当我们写入一个字符的时候就会真正扩充这个文件的大小

**但是相应的，扩充的这段空间里面啥也没有，当我们调用`read`函数读取的时候读到的数据(字节为单位)是0，我们形象的称他们为空洞**

以下是一个例子：

**图中文件的长度是`8483248B`，换算过来差不多`8.09MB`，但是`du`命令看到文件占据磁盘总量是`272`个`512`字节数，也就是`136KB`，这是实际有数据的部分，可以文件当中存在多么多么大的空洞**

![image-20230831200037933](https://image.davidingplus.cn/images/2025/01/31/image-20230831200037933.png)

如果我们不用`ls`命令，我们用`wc`命令，也可以读出文件的实际大小：

![image-20230831201205022](https://image.davidingplus.cn/images/2025/01/31/image-20230831201205022.png)

现在我们调用`cat`命令将带有空洞的`core`文件拷贝一份，我们来看看结果：

![image-20230831201245761](https://image.davidingplus.cn/images/2025/01/31/image-20230831201245761.png)

**我们发现拷贝出来的`core.copy`实际占据磁盘的大小似乎变大了，这是因为在拷贝的时候所有文件中的空洞都会被填满，其中的空洞的数据字节全部都填充为0，所以占据磁盘的大小变大了**

## 函数truncate和ftruncate(文件截断)

基础的函数是`truncate`，我们来看看他的性质：

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

int ftruncate(int fd, off_t length);
// 参数：fd，已经打开的文件描述符，可见是我们已经打开的文件，其他没什么
~~~

我们有的时候需要在文件末尾截取一些数据来缩短文件的长度，**比如我们可以把文件长度缩减为0，这是一个比较特别的情况，我们在打开文件的时候使用状态标志`O_TRUNC`也可以做到这一点**

![image-20230901092349390](https://image.davidingplus.cn/images/2025/01/31/image-20230901092349390.png)

**我们设置的偏移量`length`如果小于文件本身的长度，那么文件新的长度设置为`length`，超过`length`的部分就没有办法访问；**

**如果设置的`length`大于文件本身的长度，文件长度就将增加，就和`lseek`函数扩充文件是一个道理，`truncate`函数帮我们进行了一步写的操作，这是个原子操作，所以新多出来的空间都将读作0，也就是形成了一个空洞**

## 文件系统(重要，理解!)

**我们可以把磁盘进行分区，每个分区里面就可以说是一个独立的文件系统，我们通过前面知道`i`结点可以存储文件的相关信息**

### 硬链接和软链接

我们来看一下什么是硬链接和软链接(符号链接)

硬链接：

- **一个文件有多个名字时，名字之间互为硬链接，他们的数据对应磁盘当中的同一块数据块。**

- **当删除有硬链接的文件时，只要没有删除掉最后一个，文件仍然存在并能打开。只是我们通过哪个文件名打开不同而已，对应的文件数据块是相同的**

- 文件详细信息中第二列的数值：**源文件+硬链接的和（`N-1`=硬链接数）**

  **每个`i`结点中都有一个链接计数，其值是指向这个`i`结点的目录项数，只有当链接计数减少为0的时候，才可以删除这个文件**

  **这也是为什么删除一个目录项的函数被称为`unlink`，`unlink`函数的作用就是让链接计数减1，当减为0的时候就将其删除；在`stat`结构中，链接计数包含在`st_nlink`成员中，类型是`nlink_t`，这种链接就叫硬链接；**

  **链接计数==硬链接数+1**

- 拥有硬链接的文件，在修改了内容之后，`Inode`号不变。有多处位置保存了文件指向位置。

- **不能对目录创建硬链接(对，就是不可以，书上说超级用户可以，但是我的电脑底层文件系统不支持)，并且不能跨区创建。因不同分区有各自的`Inode`号，跨区可能冲突。**

软连接(符号链接)：

- **创建软链接时一定要使用绝对路径，否则创建出来的软链接文件是失效文件。**
- 软链接文件不存储具体数据，**只存储原文件的绝对路径**。（**大小是指向的文件的文件名的大小，一旦创建大小不变**）
- **当软链接对应的原文件被删除时，软连接仍然存在，但失效了。**
- **源文件和软链接各自拥有不同的`Inode`号和`Block`块数。**
- **两个文件不管修改哪一个均可同步**
- 软链接文件权限为rwxrwxrwx,但最终生效权限以源文件为准。
- **软链接既可以链接文件，也可以链接目录，并可以跨区创建。**

#### 创建方式

软链接

我们可以对不存在的文件创建软链接，**但是是失效的**

~~~bash
 格式：ln -s 源文件（绝对路径） 目标位置（目标位置/新名字）
$ ln -s /root/1.txt /tmp
~~~

硬链接

~~~bash
 ln 源文件 目标位置（目标位置/新名字）
$ ln /root/1.txt /tmp
~~~

#### 例子

我们来创建一下硬链接和软链接

从图中可以看出，`test.txt`是源文件，`test_hard.txt`是我们创建出来的硬链接文件，他和源文件互为硬链接，然后他们的内容是相同的；对应的都是磁盘中的同一块空间，所以我随意修改哪一个文件这两个文件的内容都会同步

`test_soft.txt`则是创建源文件创建出来的软链接文件，他指向了文件`test.txt`，并且软链接文件的存储内容就是指向文件的文件名，大小也就是指向文件的文件名的大小

![image-20230901101558329](https://image.davidingplus.cn/images/2025/01/31/image-20230901101558329.png)

现在我用`stat`命令看一下：

硬链接文件之间所用的`Inode`号是相同的，他们都指向了同一个文件，并且他们的`Links`都是2，所以硬链接数也就是`2-1==1`，就是`test_hard.txt`文件

软链接文件和源文件的`Inode`号不同，但是他存储了源文件的文件名和路径信息，可以很方便的找到源文件的位置，所以可以跨区建立

![image-20230901102054279](https://image.davidingplus.cn/images/2025/01/31/image-20230901102054279.png)

### i 结点

`i`结点包含了文件有关的所有信息，`stat`中的大部分信息都是来自`i`结点：**文件类型，文件访问权限位，文件长度和指向文件数据块的指针**等等，下面是一个图表示了磁盘中的存储结构：

![image-20230901103020737](https://image.davidingplus.cn/images/2025/01/31/image-20230901103020737.png)

**目录项(放在磁盘中，和前面的文件共享的地方记得区分)中存放了两个重要的内容，就是文件名和i结点编号，因为目录项中的`i`节点编号指向同一文件系统中的相应`i`节点，而在其他文件系统当中，`i`结点的编号就不统一了，因此这也就是硬链接文件不能跨区建立的原因**

**如果我们想对文件进行移动或者重命名，我们只需要把旧的文件目录项删除然后建立一个新的目录项指向对应的`i`结点就行了，磁盘中数据块的位置根本不用动，并且链接计数不会变；例如：我们把文件`/usr/lib/foo`重命名为`/usr/foo`，在我们的眼里就是移动了位置，如果`/usr/lib`和`/usr`在一个文件系统中，当然一般都是，foo的内容，也就是数据块位置不用移动，修改目录块的内容即可，就按照我前面所说的，就给了我们以为文件位置移动的假象**

## 函数link，linkat，unlink，unlinkat和remove

### link系列

`link`系列函数可以用来给指定的文件项创建硬链接文件：

**要注意一点，创建硬链接文件必须在同一个磁盘分区，也就是同一个文件系统中创建，否则i结点编号不统一就出问题了**

**还有就是创建了硬链接文件之后，链接计数也需要加1，在`link`函数当中这是原子操作**

~~~cpp
#include <unistd.h>

int link(const char *oldpath, const char *newpath);
// oldpath：旧的目录项或者目录，newpath当然就是新的
// 只有root用户才能对目录创建硬链接，当然不建议，因为有可能形成文件循环，所以我们一般不对目录创建硬链接
// 创建硬链接必须在同一个磁盘分区

int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags);
// 这个就是对已经打开的文件目录的基础上进行硬链接的创建
// path如果是绝对路径，那么dirfd参数会被忽略
// path如果是相对路径：dirfd指定为AF_FDCWD，那么对应的path就从当前工作目录的基础开始找；否则就从指定的目录开始找
// flags：如果设置了AF_SYMLINK_NOFOLLOW，针对软连接文件创建硬链接，那么就是作用于该软链接文件本身，否则就是指向目标
~~~

#### 例子

我们对目录中的文件分别用两个函数创建两个硬链接文件来熟悉用法

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>

int main() {
    // link函数
    int ret = link("07.txt", "07_1.txt");
    if (-1 == ret) {
        perror("link");
        return -1;
    }

    // linkat函数
    int dir_fd = open("./", O_DIRECTORY);
    if (-1 == dir_fd) {
        perror("open");
        return -1;
    }

    ret = linkat(dir_fd, "07.txt", AT_FDCWD, "07_2.txt", 0);
    if (-1 == ret) {
        perror("linkat");
        return -1;
    }

    return 0;
}
~~~

结果：

可见成功了，这三个文件互为硬链接文件并且链接计数为3，用的是用一个`Inode`

![image-20230901111815358](https://image.davidingplus.cn/images/2025/01/31/image-20230901111815358.png)

### unlink系列

`remove`的功能和`unlink`相同，区别是`remove`是C标准的函数，`unlink`是系统调用

**`unlink`相反就可以解除硬链接，他的机制是把链接计数减1，只有当减到0的时候才会删除该文件，删除文件指的是把文件的信息从磁盘中删除，在删除的过程可能我把文件最开始命名的文件删除了，这是正常的，因为其他硬链接的文件也指向同一块文件块，就相当于是变了个名字，这一点要理解，不存在源文件地位高一点什么的，都是平等的，只是名字不同**

~~~cpp
#include <unistd.h>

int unlink(const char *pathname);
// 删除一个现有的文件项，对应磁盘中i结点信息修改，链接计数-1，当链接计数减到0的时候，会把磁盘中关于该文件的数据块和其他信息删除

int unlinkat(int dirfd, const char *pathname, int flags);
// dirfd：打开的文件目录
// pathname：分绝对路径和相对路径，dirfd的取值注意被忽略，还是打开的目录或者AF_FDCWD
// flags：注意不会删除软链接指向的文件，而是删除软链接的文件本身！！！这一点和前面不同
		// 另外，如果设置了AF_REMOVEDIR，那么这个函数可以删除目录
~~~

我再强调一下，对于多个文件项，他们有不同的名字，但是他们对应的是同一个文件块，他们的数据部分是共享的，修改也是同步的

**现在我用`unlink`函数把一个其中一个文件项删除，这个硬链接的文件项就被删除了，文件块对应的链接计数会减1，只有当所有的文件项都被删除了，也就是链接计数减到0，真实的文件块内容会被删除**

**注意文件项和真实的文件块的区别，文件项就是我们看到的文件结构，但是这个真正存储的结构是不一致的，文件块就是磁盘当中保存这个文件的信息**

#### 例子

我们针对上面的例子继续编写代码

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    int ret = unlink("07.txt");
    if (-1 == ret) {
        perror("unlink");
        return -1;
    }

    return 0;
}
~~~

结果：

当我执行`unlink`代码之后，`07.txt`文件项被删除了，同时文件块的链接计数变为了2，意思是我们仍然可以通过剩下的这两个文件项访问文件块的内容

![image-20230901113952445](https://image.davidingplus.cn/images/2025/01/31/image-20230901113952445.png)

**另外，还有一个条件可以阻止程序删除文件块，就是进程打开了这个文件，其内容就不能删除；所以内核先检查有无进程打开这个文件，当打开的个数变成0，也就是没有进程打开的时候，再去检查文件的链接计数，为0则将其文件块的内容删除**

## 函数rename和renameat

这两个函数可以对文件或者目录进行重命名

~~~cpp
#include <stdio.h>

int rename(const char *oldpath, const char *newpath);
// 传入旧的文件名将其修改为新的

int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);
// dirfd：绝对路径的时候被忽略；相对路径根据是否AF_FDCWD判断是从工作目录开始还是从指定的目录开始
~~~

关于`rename`有几点需要注意：

- **如果`oldpath`是一个文件而不是目录，那么就对该文件本身进行重命名，对于符号链接也是如此，对符号链接本身进行命名**

  **在这种情况下，`newpath`肯定最好是一个目录里不存在的文件名，如果已存在，`newpath`肯定不能为一个目录，假如我们想要命名的`newpath`已经存在，就是重名，那么内核会将`newpath`的目录项删除并且将`oldpath`的文件重命名为`newpath`，这意味着原来叫`newpath`的文件会被删除，然后被替代**

  当然我们要对两个文件项进行修改操作，肯定需要具有读写权限

  我们通过代码来看：

  ~~~cpp
  #include <iostream>
  using namespace std;
  
  int main() {
      int ret = rename("08_1.txt", "08.txt");
      if (-1 == ret) {
          perror("rename");
          return -1;
      }
  
      return 0;
  }
  ~~~

  在执行之前这是我的两个文件内容：

  <img src="https://image.davidingplus.cn/images/2025/01/31/image-20230901132559546.png" alt="image-20230901132559546" style="zoom:80%;" />

  执行过后，结果：

  原`08.txt`已经被删掉，然后`08_1.txt`重命名为了`08.txt`

  ![image-20230901132633366](https://image.davidingplus.cn/images/2025/01/31/image-20230901132633366.png)

  ![image-20230901132653264](https://image.davidingplus.cn/images/2025/01/31/image-20230901132653264.png)

- **如果`oldpath`是一个目录，`newpath`肯定最好也是一个名字不重复的新目录名字；如果重名了，那么这个目录需要是一个空目录才行，这时就会先把空目录删除然后将`oldpath`重命名，对于有内容的目录是不行的**

  例如：`newpath`不能包含`oldpath`的路径前缀，比如把`/usr/foo`重命名为`/usr/foo/test`，因为会把`foo`目录删除然后再创建新目录，这显然是不对的

  **不能对`./`目录和`../`目录重命名**，比如我不能打开一个目录，然后使用`./`给这个目录重命名，不可以的

  我们同样写一个程序来看一下

  此时我的目录中只有文件夹`test`，并且里面是空的

  ~~~cpp
  #include <iostream>
  using namespace std;
  #include <fcntl.h>
  #include <unistd.h>
  
  int main() {
      int dir_fd = open("../08/", O_DIRECTORY);
      if (-1 == dir_fd) {
          perror("open");
          return -1;
      }
  
      int ret = renameat(dir_fd, "./test", AT_FDCWD, "./test2/");
      // int ret = renameat(dir_fd, "./test", AT_FDCWD, "./test/test2/");
      if (-1 == ret) {
          perror("rename");
          return -1;
      }
  
      close(dir_fd);
  
      return 0;
  }
  ~~~

  结果修改成功了，但是如果我尝试让`newpath`包含`oldpath`路径，就是注释的那段代码，就会报错：

  ![image-20230901144324802](https://image.davidingplus.cn/images/2025/01/31/image-20230901144324802.png)

  当然我们也要试一下重命名成为一个有文件的目录，这里我创建一个目录`test2`，`test`目录里存放了一些文件

  这是原来的目录tree：

  ![image-20230901144650852](https://image.davidingplus.cn/images/2025/01/31/image-20230901144650852.png)

  代码：

  ~~~cpp
  #include <iostream>
  using namespace std;
  
  int main() {
      int ret = rename("test", "test2");
      if (-1 == ret) {
          perror("rename");
          return -1;
      }
  
      return 0;
  }
  ~~~

  在空目录`test2`下，执行结果：

  可见`test2`被删除，`test2`目录重命名并且文件也在，**当然文件块的位置和`i`结点没变，只是文件目录的命名变了**

  ![image-20230901144731958](https://image.davidingplus.cn/images/2025/01/31/image-20230901144731958.png)

  现在我让`test2`加上数据，再来尝试：

  ![image-20230901144925401](https://image.davidingplus.cn/images/2025/01/31/image-20230901144925401.png)

  执行结果：

  函数返回了-1，错误信息说目录不为空，可见结果在预期内

  ![image-20230901144951938](https://image.davidingplus.cn/images/2025/01/31/image-20230901144951938.png)

## 符号链接(软链接)

符号链接是对一个文件的间接指针，它与硬链接有所不同，硬链接是一个文件块有多个文件项指向它，他们有着不同的命名，但是`Inode`是相同的，里面就存在这链接计数；而软链接就是指向一个文件或者目录的一个间接指针，主要是为了避开硬链接的一些限制：

- **硬链接要求创建的硬链接文件在同一文件系统内，因为不在同一文件系统和分区，Inode不通用**

- **在底层文件系统支持的前提下，只有超级用户能够创建指向目录的硬链接，其他的是不允许的，我的系统就不行**

  我们为什么不推荐创建指向目录的硬链接或者软连接呢？

  因为有可能发生循环，我们举个例子：

  在我的09目录中存在空目录`test`，现在我在test目录中创建一个指向09目录的软连接文件

  ![image-20230901151254437](https://image.davidingplus.cn/images/2025/01/31/image-20230901151254437.png)

  如下：

  可见`foo`文件指向了外层目录09，这不就是一个循环吗？实际操作的过程中很可能就出现问题了

  所以不少系统不允许用户创建指向目录的硬链接，即使是超级用户也不可以

  ![image-20230901151357940](https://image.davidingplus.cn/images/2025/01/31/image-20230901151357940.png)

**对于符号链接，倒是并未有这些限制，任何用户都可以创建指向文件或者目录的软链接，只是我们要注意不要操作失误导致文件循环；但是也不是没有办法，我们可以用`unlink`函数消除，因为他是不跟随符号链接的，就可以删除链接文件本身；但是对于硬链接文件就不好办了，因为想要将其彻底删除得让链接计数为0，而且还是在卡入循环的情况下，这就更加复杂了，总之就是最好不要建对目录建立硬链接**

### 跟随，不跟随软链接？

以下是书上的图，它记录了大部分函数对软链接的处理：

前面基本都提到过了

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230901151720798.png" alt="image-20230901151720798" style="zoom:80%;" />

## 创建和读取符号链接

### symlink系列

前面的`link`系列函数是用来创建硬链接，这里的`symlink`系列函数是用来创建软链接

**值得注意的是，`target`路径可以不存在，这时仍然可以创建出来软链接，只是这个软链接文件失效**

~~~cpp
#include <unistd.h>

int symlink(const char *target, const char *linkpath);
// target路径想要被指向的路径，可以不存在；两个路径不需要在同一个文件系统中

int symlinkat(const char *target, int newdirfd, const char *linkpath);
// newdirfd：允许我们操作一个已经打开的想存放软链接文件的目录，AF_FDCWD表示从当前工作目录找起；如果是绝对路径将被忽略
~~~

#### 例子

我们分别创建一个有效的和一个失效的软链接

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("./", O_DIRECTORY);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    // 创建一个指向文件存在的
    int ret = symlinkat("10.txt", fd, "10_1.txt");
    if (-1 == ret) {
        perror("symlink");
        return -1;
    }

    // 创建一个指向文件不存在的
    ret = symlink("11.txt", "10_1.txt");
    if (-1 == ret) {
        perror("symlink");
        return -1;
    }

    return 0;
}
~~~

结果：

`10_1.txt`正常指向`10.txt`，但是`10_2.txt`指向了一个不存在的`11.txt`，虽然创建链接成功了，但是他是失效的

![image-20230901153517526](https://image.davidingplus.cn/images/2025/01/31/image-20230901153517526.png)

当我们尝试通过软链接文件去查看指向文件的内容的时候，就会凸显出差异：

失效的软链接文件`10_2.txt`指向了一个不存在的文件，读取不出来

![image-20230901153642964](https://image.davidingplus.cn/images/2025/01/31/image-20230901153642964.png)

### readlink系列

**当然上面的情况我们用`open`函数打开软链接文件也会出现类似的结果，失效的软链接文件没有办法打开，因为`open`函数是跟随符号链接的**

**因此我们需要有一种方法来打开软链接文件本身，软链接文件里面就存储者指向文件的路径，我们就像把这个东西读取出来**，因此就有了`readlink`系列函数

~~~cpp
#include <unistd.h>

ssize_t readlink(const char *restrict pathname, char *restrict buf, size_t bufsize);
// 用buf接受读取到的数据，bufsize是buf的大小
// 成功返回读取到的字节数，失败返回-1并且设置错误号errno

ssize_t readlinkat(int dirfd, const char *restrict pathname, char *restrict buf, size_t bufsize);
// 经典dirfd，绝对路径忽略；相对路径AF_FDCWD从当前工作目录找起，否则从指定目录找起
~~~

#### 例子(续上)

我们跟着上面的例子继续走，这时候我想要读取软链接文件`10_1.txt`存储的内容

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    char buf[MAX_BUFFER_SIZE] = {0};

    int len = readlink("10_1.txt", buf, sizeof(buf) - 1);
    if (-1 == len) {
        perror("readlink");
        return -1;
    }
    printf("%s\n", buf);

    return 0;
}
~~~

结果：

**里面就存储的是指向文件的路径，我们给的是相对路径，是因为链接文件和指向文件在同一目录下，下面会细讲**

![image-20230901154605717](https://image.davidingplus.cn/images/2025/01/31/image-20230901154605717.png)

#### 软链接文件存储的内容(重要)

从上面的例子我们看到，软链接文件存储的内容似乎是指向文件的名字，但是不是的，**软链接文件存储的其实是指向文件的路径**，记住，一定是路径，为什么？因为我要通过存储的内容其访问指向的文件，仅仅有个文件名字怎么可能呢？，下面我们来举例子说明：

我在目录下面建立一个`a.txt`，和一个建立软链接文件的目录，这样我就把软链接文件和目标文件分开了：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230901155847954.png" alt="image-20230901155847954" style="zoom:80%;" />

现在我分别在soft目录中用三种方式创建软链接文件，名字，绝对路径和相对路径：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230901160019983.png" alt="image-20230901160019983" style="zoom:80%;" />

我们现在分别打开这三个文件来看下能否读出对应内容呢？

2和3成功了，1失败了，这已经够明显了，**存储的是路径，访问他们的时候就是通过读取他们存储的路径内容去查找的，绝对路径从根目录开始找，相对路径就从该文件的目录开始找**

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230901160110538.png" alt="image-20230901160110538" style="zoom:80%;" />

我们同样可以查看文件存储的大小来看一下他们是否存储的是路径，我就不写程序去readlink了

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230901160316403.png" alt="image-20230901160316403" style="zoom:80%;" />

**所以软链接文件存储的内容是指向的路径，这个路径在软链接文件和指向文件在同一目录的时候，命名就是相对路径，但是一旦分开，我建议用绝对路径，因为这样不管链接文件到哪里去，只要指向文件位置不变，就能找到**

## 文件的时间

对于每个文件维护了三个时间字段，在stat结构中可以获得：

![image-20230901161513319](https://image.davidingplus.cn/images/2025/01/31/image-20230901161513319.png)

我们要注意这分为两类时间，**一类是文件内容或者文件数据的时间，一类是`Inode`的时间；时间又可以分为两类，最后一次的访问时间和最后一次的修改时间，图中并没有维护`Inode`的最后访问时间，所以在`stat`和`access`获取文件属性和权限的时候并不会对`Inode`不会进行修改操作，不会更新`st_ctim`的值**

然后我们要注意一下文件数据最后修改时间`st_mtim`和`Inode`最后更改时间`st_ctim`：

**修改时间是文件内容最后一次被修改的时间，`Inode`最后修改时间也叫状态更改时间，因为`Inode`里面存储了文件的各种属性，stat函数获取stat结构体的数据大部分是来自于`Inode`的，有很多操作都能影响`Inode`，例如：更改文件的访问权限，更改用户ID，更改链接数等等，但是这些操作并没有修改文件数据块的内容；这也是因为`Inode`中的所有信息和文件的实际内容在磁盘中是分开存储的，这两者都有可能被修改，所以需要维护各自的最后修改时间**

**我们一般称这三个时间(图中顺序)为：文件的访问，修改和状态更改时间**，因为用户一般不知道`Inode`，我们就称文件的状态更改时间

下面是一个书上的一个表用来记录不同系统调用对这三个时间的是否修改：

<img src="https://image.davidingplus.cn/images/2025/02/01/image-20230904105617965.png" alt="image-20230904105617965" style="zoom:80%;" />

## 函数futimens，utimesat和utimes

一个文件的访问和修改时间(不是上面的第三个状态更改时间)可以由下面的系统调用修改

~~~cpp
#include <fcntl.h>
#include <sys/stat.h>

int futimens(int fd, const struct timespec times[_Nullable 2]);
// fd：通过打开指定文件的文件描述符来进行修改
// times：数组，类型是stat结构体中timespec结构体类型，第一个元素表示访问时间，第二个元素表示修改时间
	// 这两个时间是日历时间，也就是从1970年1月1日00:00:00到现在经过的秒数

int utimensat(int dirfd, const char *pathname, const struct timespec times[_Nullable 2], int flags);
// dirfd：老规矩，需要我们打开一个目录，open函数记得加上O_DIRCTORY
// pathname：如果是绝对路径，dirfd参数被忽略；相对路径如果传入AF_FDCWD则从当前工作目录找起，否则从指定目录
// times：同上
// flags：设置AT_SYMLINK_NOFOLLOW，针对符号链接则作用于符号链接本身而不是指向文件
~~~

关于时间戳的设置值，给出书上的图进行理解：

<img src="https://image.davidingplus.cn/images/2025/02/01/image-20230904111133258.png" alt="image-20230904111133258" style="zoom:80%;" />

同时，执行这些函数所要求的权限取决于`times`参数的值：

我们都修改文件`stat`的内容了，那肯定我们进程肯定需要相应的权限才行啊，不然怎么保证安全

<img src="https://image.davidingplus.cn/images/2025/02/01/image-20230904111206863.png" alt="image-20230904111206863" style="zoom:80%;" />

我们来看另一个函数`utimes`

~~~cpp
#include <sys/time.h>

int utimes(const char *filename, const struct timeval times[_Nullable 2]);
// filename：就是指定的文件
// times：数组，第一个参数是文件访问时间，第二个是文件修改时间
	// 它的类型和前面的不同，是timeval结构体

// timeval结构体
struct timeval {
	long tv_sec;        /* seconds */
	long tv_usec;       /* microseconds */
};
~~~

**注意，我们不能对文件的状态更改时间(`Inode`的最后修改时间)进行指定，因为我们在调用这些函数的时候，肯定修改了`Inode`的值，所以这个字段会自动更新**

### 例子

我们创建一个文件，然后过一段时间打开它，我们尝试修改他的访问时间和修改时间，具体如下：

~~~cpp
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    // 11.txt文件是之前创建出来的，我们先获取它的信息，然后修改这个文件，然后再修改文件访问和修改时间看是否成功
    struct stat stat_buf;

    int ret = stat("11.txt", &stat_buf);
    if (-1 == ret) {
        perror("stat");
        return -1;
    }

    // 获得两个修改时间
    struct timespec last_visited = stat_buf.st_atim;
    struct timespec last_modified = stat_buf.st_mtim;
    struct timespec times[2] = {last_visited, last_modified};

    // 修改文件，将其截断为0
    int fd = open("11.txt", O_RDWR | O_TRUNC);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    // 修改文件的访问时间和修改时间
    ret = futimens(fd, times);
    if (-1 == ret) {
        perror("futimens");
        return -1;
    }

    return 0;
}
~~~

结果：

文件长度被截断为0，如果我们不修改，那么时间会被自动更新，但是我们修改了，成功：

![image-20230904133117860](https://image.davidingplus.cn/images/2025/02/01/image-20230904133117860.png)

## 函数mkdir，mkdirat和rmdir

### mkdir系列

`mkdir`系列函数创建一个新的空目录，这个空目录里面会自动加入 `./` 和 `../` 目录项(很合理)

~~~cpp
#include <sys/stat.h>

int mkdir(const char *pathname, mode_t mode);
// pathname：需要创建的目录路径，相对路径或者绝对路径
// mode：权限，八进制数；目录获得的实际权限要和umask屏蔽字想与
// 返回值：成功0；失败-1并且修改errno

int mkdirat(int dirfd, const char *pathname, mode_t mode);
// dirfd：目录的文件描述符，open函数记得加上O_DIRECTORY
// pathname：绝对路径的话dirfd将被忽略；相对路径的话dirfd若AF_FDCWD则从当前工作目录找起，不是则从指定的目录找起
~~~

### 目录的读和执行权限(复习)

我们来复习一下目录的这两个权限，写权限不说了，就是用户可以向目录当中创建文件或者子目录

**目录的读权限是指能够知道这个目录当中的所有文件名，我知道有哪些；**

**目录的执行权限是指能够访问这个目录中的文件名，通过这个文件名找到对应的`Inode`，然后有机会访问文件，注意访问文件就和目录的权限没关系了**

总结：

- **目录读权限：表示用户可以用`ls`命令将目录下的具体子目录和文件罗列出来。**
- **目录写权限：表示用户可以在该目录下可创建子目录或者文件。**
- **目录执行权限：表示可以用`cd`进入该目录，知道里面有哪些文件**

这两点需要格外注意

### rmdir

函数`rmdir`可以删除一个空目录，空目录就是目录项只有 `./` 和 `../` 的目录

~~~cpp
   #include <unistd.h>

   int rmdir(const char *pathname);
~~~

#### 例子

值得注意的是，只有目录为空才会删除，我们可以写一个程序来验证

我们先用一个程序创建一个目录，然后创建一个文件；另一个进程就尝试删除这个目录

~~~cpp
// mkdir.cpp
#include <iostream>
using namespace std;
#include <sys/stat.h>
#include <unistd.h>

int main() {
    // 创建一个新目录，这次我用绝对路径
    int ret = mkdirat(-1, "/mnt/d/Code/Cpp/为了工作/Linux/UNIX 环境高级编程/第4章-文件和目录/12/test", 0664);
    if (-1 == ret) {
        perror("mkdirat");
        return -1;
    }

    // 调用exec函数族在目录里面创建一个文件
    execlp("touch", "touch", "./test/12.txt", nullptr);

    return 0;
}
~~~

执行过后目录项是这样：

![image-20230904155757324](https://image.davidingplus.cn/images/2025/02/01/image-20230904155757324.png)

~~~cpp
// rmdir.cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    // 尝试删除test文件夹
    int ret = rmdir("test/");
    if (-1 == ret) {
        perror("rmdir");
        return -1;
    }

    return 0;
}
~~~

结果：

可以看出没办法删除非空目录

![image-20230904155841171](https://image.davidingplus.cn/images/2025/02/01/image-20230904155841171.png)

#### 补充

**和文件一样，目录也存在链接(硬链接)，但是我们一般不建议这么做，只有部分操作系统并且是超级用户允许这么做，因为可能会发生文件循环；但是删除的机制是一样的，和文件类似，存在一个链接计数，删除目录是将链接计数减一并且将这一个名字的目录删除，我们以后没办法通过这个目录访问，但是我们可以用过其他名字的目录访问目录中的文件，只有当目录为空并且链接计数为0的时候才会真正释放这个目录块在磁盘上的空间；**

**当然，如果遇到了删除的时候某些进程正在使用这个目录，那么并不立刻释放这个目录，这些进程仍然可以看到这些目录，但是没办法对这个目录进行后续操作了，当所有进程结束的时候就会释放掉这个目录块的内容**

## 读目录

**写目录就是对目录本身进行操作，和目录的写权限(可以创建子目录或者文件)不同，并且只有内核才能写目录，是为了防止文件系统紊乱**

读目录和读文件类似，但是有区别，我们前面模拟实现过`ls -l`的命令，这里复习一遍里面的系统调用：

### opendir

如你所见，`Unix`提供给我们访问目录的`API`是`DIR`结构

~~~cpp
#include <sys/types.h>
#include <dirent.h>

DIR *opendir(const char *name);
// 打开一个目录，返回一个可以操作目录的指针 DIR*

DIR *fdopendir(int fd);
// 类似，只不过是传入一个打开的目录的文件描述符
~~~

### readdir

~~~cpp
#include <dirent.h>

struct dirent *readdir(DIR *dirp);
// 接受opendir返回的DIR流结构体指针，然后开始读取目录中的文件项
// 这个函数一次只能读取目录中一个文件的dirent结构体，并且有偏移指针记录，读到末尾或者出错会返回nullptr

// dirent结构体
struct dirent {
   ino_t          d_ino;       /* Inode number */
   off_t          d_off;       /* Not an offset; see below */
   unsigned short d_reclen;    /* Length of this record */
   unsigned char  d_type;      /* Type of file; not supported
                                  by all filesystem types */
   char           d_name[256]; /* Null-terminated filename */
};
// 这个结构体需要注意两个值，一个是类型是ino_t的d_ino，表示的是i结点的编号；另一个是d_name[]，就是文件名字
// 我们是通过dirent结构体里面的Inode去寻找对应的文件块访问的

int closedir(DIR *dirp);
// 关闭打开的文件目录
~~~

### 其他函数

~~~cpp
#include <sys/types.h>
#include <dirent.h>

void rewinddir(DIR *dirp);
// 设置目录流dirp目前的读取位置为目录流的起始位置

long telldir(DIR* dirp);
// 获取目录流当前的读取当前位置(偏移量)，错误返回-1

void seekdir(DIR *dirp, long loc);
// 设置目录流的偏移量，下一次readdir将会从新位置开始读取
~~~

我本来准备写例子的，但是前面写过了模拟实现`ls -l`命令，这里就不写了，[点这里跳转过去](#ls -l)

## 函数chdir，fchdir和getcwd

### chdir系列

我们一个进程执行的时候默认的工作目录就是进程可执行文件的存放目录，但是有时候我们想要更改目录怎么办呢？

~~~cpp
#include <unistd.h>

int chdir(const char *path);
// 更改当前的工作目录为path指向的路径

int fchdir(int fd);
// 给的参数是打开目录的文件描述符，记得open函数加上O_DIRECTORY
~~~

#### 例子

我们通过终端启动一个程序，然后更改进程的工作目录干一些事情

我的目录情况：

![image-20230904170648902](https://image.davidingplus.cn/images/2025/02/01/image-20230904170648902.png)

代码：

我想通过`chdir.cpp`程序去向`12.txt`中写入一些数据，当然我是通过修改工作目录来的，从`open`函数用的修改之后的相对路径就可以看出来

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    chdir("./test");

    char buf[MAX_BUFFER_SIZE] = {0};

    int fd = open("12.txt", O_RDWR | O_TRUNC);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    while (1) {
        static int count = 0;

        bzero(buf, sizeof(buf));
        sprintf(buf, "hello, count: %d\n", count++);

        write(fd, buf, strlen(buf));
        sleep(1);
    }

    return 0;
}
~~~

结果：

写入成功，没有问题

![image-20230904170803393](https://image.davidingplus.cn/images/2025/02/01/image-20230904170803393.png)

#### 思考

在这里我们通过`shell`终端启动了一个进程来修改工作目录，注意修改的工作目录只是这个进程的，我们终端的工作目录并没有修改哦，这一点从终端提示符或者`pwd`命令都可以看出来：

![image-20230904170951129](https://image.davidingplus.cn/images/2025/02/01/image-20230904170951129.png)

并且，我最开始向通过向标准输出重定向，就是这样：

~~~bash
./chdir > 12.txt
~~~

**但是这样不行，就是因为我修改的是通过shell终端启动的进程的工作目录，不是shell终端本身，所以会失败；他会在我当前目录新建一个12.txt的文件来写入**

所以我们可以怎么修改终端的工作目录呢？

##### cd命令

所以`cd`命令就自然而然被定义出来了，`cd`命令也就内建在`shell`中了，并且我们还无时不刻不在使用

### getcwd

我们上面通过pwd命令获得了工作目录的绝对路径，我们也可以通过系统调用来实现

~~~cpp
#include <unistd.h>

char *getcwd(char *buf, size_t size);
// 获得当前的工作目录
~~~

他的实现思路如图：

我来解释一下为什么要匹配`Inode`编号：

**对于当前的目录，我们目录项中记录的是`./`，而不是目录名字，当前目录的名字是由上一级目录中保存的，但是二者的`Inode`是相同的(但是二者不是链接关系，是同一个，链接数也是1，`./`目录比较特殊)，这样查询就能找到当即目录的名字，循环反复遇到根就找到了**

![image-20230904171646868](https://image.davidingplus.cn/images/2025/02/01/image-20230904171646868.png)

我后续一定要自己实现一下`getcwd`这个函数

### 补充

**`chdir`函数是要跟随符号链接的，因为我们可以很方便的通过符号链接找到另一个工作目录然后修改；**

**`getcwd`函数不会跟随符号链接，他就返回当前目录的绝对路径**

## 设备特殊文件(了解)

这个不是很重要，我也没看懂，截图吧

![image-20230904172357819](https://image.davidingplus.cn/images/2025/02/01/image-20230904172357819.png)

![image-20230904172404121](https://image.davidingplus.cn/images/2025/02/01/image-20230904172404121.png)

![image-20230904172411254](https://image.davidingplus.cn/images/2025/02/01/image-20230904172411254.png)

## 文件访问权限位小结

如下：

![image-20230904172514551](https://image.davidingplus.cn/images/2025/02/01/image-20230904172514551.png)

![image-20230904172520134](https://image.davidingplus.cn/images/2025/02/01/image-20230904172520134.png)

# 第五章 标准`I/O`

这一章涉及到C标准库当中的内容，是在`Linux`操作系统之上进行的封装扩充，牛客`Linux`的课程没有涉及到，所以认真学

不仅是UNIX，很多其他操作系统都实现了标准`I/O`库，这个库由`ISO C`标准进行说明

**标准库的`I/O`处理了很多细节，比如缓冲区分配，以优化的块长度执行`I/O`等等，在系统调用的基础上使得用户的调用更加的方便和严谨安全；这些处理使得用户不用担心如何正确选择正确的块长度；但是我们也要深入了解以下标准`I/O`库函数的操作，以及是如何与系统调用联系起来的，否则出了问题不知道怎么办**

## 流和FILE对象

前面提到的`I/O`都是围绕文件描述符的，我们打开一个文件返回给我们一个文件描述符，然后我们通过对文件描述符对文件进行后续的操作；

**但是对于标准`I/O`，所有的操作都是围绕流(`stream`)展开的，我们打开一个文件，标准`I/O`返回我们一个流用于进行和文件的关联**

下面我们看一下单字节流和多字节流：

- **对于ASCII字符集，一个字符用一个字节表示。对于国际字符集，一个字符可以用多个字节表示。标准`I/O`文件流可用于单字节或多(宽)字节字符集。流的定向决定了所读、写的字符是单字节还是多字节。**
- **当一个流最初被创建时，它并没有定向。如若在未定向的流上使用一个多字节`I/O`函数，则将该流的定向设置为宽定向的。若在未定向的流上使用一个单字节`I/O`函数，则将该流的定向设置为字节定向的。**
- **只有两个函数可以改变流的定向。`freopen`函数清除一个流的定向；`fwide`函数设置流的定向。**

~~~cpp
#include <wchar.h>

int fwide(FILE *stream, int mode);
// stream：打开文件返回的流对象
// mode：不同的值有不同的作用
	// 0，则表示不试图设置流的走向，返回现在流走向的值
	// 正数，则表示试图设置流的走向为宽(多字节)走向
	// 负数，则表示试图设置流的走向为单字节走向
// 返回值：返回设置之后的字节流走向(传入正数或者负数) 或者 现在流走向的值(传入0)
~~~

我们写一个程序来加深一下印象：

代码中`fopen`是标准`I/O`库提供的打开文件的函数，其中第二个参数表示只读，对应`open`函数的`O_RDONLY`，这个后面再说

~~~cpp
#include <iostream>
using namespace std;
#include <wchar.h>

int main() {
    FILE* file = fopen("01.txt", "r");
    if (nullptr == file) {
        perror("fopen");
        return -1;
    }

    int ret = fwide(file, 100);
    cout << ret << endl;

    // 现在我在已经设置宽定向的基础上再设置一次
    ret = fwide(file, -100);
    cout << ret << endl;

    return 0;
}
~~~

结果：

返回1，文件刚打开的时候是未定向的流，标志值是0，然后我进行设置之后就返回宽定向的标志值，正数，这里返回了1；

但是第二次我设置未单字节流，为什么还是返回1呢？**这表示我们尝试设置不成功，但是程序没有异常终止**，这就是这个函数的特性了

![image-20230905200000664](https://image.davidingplus.cn/images/2025/01/31/image-20230905200000664.png)

从上面我们可以看出，**`fwide`函数没有办法改变已定向流的定向，并且没有出错返回；**

如果流无效的时候，我们该怎么办呢？我们可以在调用`fwide`之前，清除`errno`，然后执行函数之后检查`errno`的值，这个倒不算重要

**另外，当我们用`fopen`打开一个文件时，返回给我们一个操作文件的流，这个流包含了`I/O`管理这个文件的所有信息，包括内核实际上使用的文件描述符，指向该流使用缓冲区(这个缓冲区是用户区的那个)的指针，缓冲区的长度，当前缓冲区的字符数以及出错的标志等等**

### 单字节和多字节

所以说了这么多，我们还是要区分一下单字节和多字节：

- **单字节就是用一个字节就可以表示出所有的字符，也就是8位，也就是可以表示最多256个字符，这一点在英语当中是没有问题的，这也是ASCII字符集使用的字节表示方式**
- **但是单字节没有办法统一表示国际上的所有字符，比如不同国家就有自己的字符，汉字也有自己的字符，所以这个时候单字节就显得少了，所以引入了多字节，多字节中又可以分为统一有多少个字节表示的标准和可以由一个或者多个字符表示的标准，但是这不是我们了解的重点；**
- **重点是不同的标准下就对文件有了不同的编码，如果我们不使用统一的编码，文件中就很可能会出现乱码，现在普遍使用的编码方式就是`utf`-8，就是对应8位，单字节，对应的就是`ASCII`编码集；像`GB2312`这些就是多字节编码，我们后面都不考虑，只考虑单字节**

## 标准输入，标准输出和标准错误(流)

与文件描述符的0 1 2类似的，**内核对进程预定义了三个流，`stdin`，`stdout`和`stderr`**，这三个流就对应了系统调用中的文件描述符`STDIN_FILENO`，`STDOUT_FILENO`和`STDERR_FILENO`

## 缓冲

**标准`I/O`库提供缓冲的目的就是尽可能少的调用`write`和`read`函数，也是对每个`I/O`流自动进行缓冲管理，而不需要应用程序考虑这一点，可能带来麻烦。**

### 缓冲区的目的（重要）

当我们向文件中写入数据时，这是一个 I/O 的过程。I/O 的次数越多，那么时间浪费越多，效率越低。如果使用一个东西预先存储这些数据，先不将这些数据写入文件，而是当到达一定规模时统一写入文件，那么 I/O 的次数就会减少，进而效率提升。

因此，**缓冲区存在的意义就是通过减少 I/O 次数达到效率上的提升**。

更多细节请参考博客：[https://blog.csdn.net/weixin_61857742/article/details/127424052](https://blog.csdn.net/weixin_61857742/article/details/127424052)

读了这篇文章以后，再来考虑一下读取数据的过程。和写入类似，为了减少 I/O 请求，读取的时候可能会一次性读取一大块数据到 I/O 缓冲区中。当用户请求读取的时候，优先查询缓冲区，如果有就可以有效的减少 I/O 操作了，而不用每次读取都从文件中读取。由于 I/O 缓冲区的数据和用户的存储结构都是在内存中，因此 fread() 执行以后，用户的存储结构的数据更新是实时的，而非 fwrite() 函数先写到 I/O 缓冲区中，需要 fflush() 才刷新。这和 fread() 首先就需要从文件中读取数据有关，也和 fread() 和 fwrite() 的语义不同有关。

因此，理解到这里，应该知道，用户程序中的接收数组，例如 char[] 字符串最好不要叫做 buff 了，免得与 I/O 缓冲区相混淆。

### 三种类型的缓冲

标准`I/O`库提供了三种不同的缓冲：

- 全缓冲

  在这种情况下，**当缓冲区被填满之后才会进行相应的`I/O`操作**，比如读需要等缓冲区被写满了再去读，写需要等缓冲区被读完了再去写；**对于磁盘上的文件通常是由标准`I/O`库实现全缓冲的；对于一个流，第一次执行`I/O`操作的时候，通过`malloc`函数去获得其需要使用的缓冲区**

  **当然我们可以手动的冲洗缓冲，冲洗(`flush`)这个术语用来说明标准`I/O`的写操作；缓冲区可以由标准`I/O`自动冲洗，比如缓冲区被填满的时候；我们也可以手动调用`fflush`函数冲洗一个流**

  在UNIX中，冲洗有两种意思：**在标准`I/O`方面，表示将缓冲区的数据写到磁盘中(缓冲区可能是部分填满的)，在终端驱动程序方面，`flush`意味着丢弃存储在缓冲区的数据**，这个我们后面再说

- 行缓冲

  在这种情况下，**在输入和输出遇到换行符的时候**，标准库`I/O`自动执行相应的`I/O`操作；

  **当流涉及一个终端的时候，就是标准输入和标准输出，通常对应的就是行缓冲**

  我们写一个程序来验证：

  ~~~cpp
  #include <iostream>
  using namespace std;
  
  int main() {
      // cout << "hello";
      printf("hello");
  
      while (1)
          ;
  
      cout << endl;
  
      return 0;
  }
  ~~~

  结果：

  可见标准输出对应的是行缓冲，当我把字符串和换行符分开，就没办法及时输出了，遇到换行符就会立即刷新缓冲区

  ![image-20230906145510209](https://image.davidingplus.cn/images/2025/01/31/image-20230906145510209.png)
  
  注意：
  
  - **标准库`I/O`提供的行缓冲区大小是有限的，所以如果填满了缓冲区，即使没有换行符，也会进行`I/O`操作**(这里有点像上面的行缓冲)
  - **任何时候通过标准`I/O`库从一个不带缓冲的流，或者一个行缓冲的流中得到输入数据，那么系统会冲洗缓冲区输出流进行输出**；对于第二个行缓冲，它可能需要从内核中读取数据，也可能不需要，因为数据可能在缓冲区中，但是对于不带缓冲的流肯定需要从内核的缓冲中获得数据
  
- 不带缓冲

  **标准`I/O`库不对字符进行缓冲存储(内核中还有缓冲)，意思是我们希望数据尽快的输入或者输出**；例如我们调用`fputs`函数输出一些字符到不带缓冲的流当中，我们就期望这些数据能够尽快输出，这时候在底层很可能就调用了系统API的write进行后续操作

  **标准错误stderr通常是不带缓冲的，因为错误信息应该尽可能快的显示出来，而不需要管他们有没有换行符**

ISO C标准要求缓冲有下列特征：

- **标准输入和标准输入通常是行缓冲的，当且仅当他们不指向交互式设备(例如，键盘，鼠标，显示器等等)，才是全缓冲**
- **标准错误绝不可能是全缓冲的**

但是上面的说法并没有告诉我们，当标准输入和输出指向交互式设备的时候是行缓冲还是不带缓冲；标准错误是行缓冲还是不带缓冲，所以一般来说系统默认使用如下的缓冲：

- **标准输入和标准输出，指向交互式设备的时候是行缓冲(例如在终端屏幕上输出信息)，否则就是全缓冲**
- **标准错误是不带缓冲的**

关于标准`I/O`缓冲后续会说的更具体

### 函数setbuf和setvbuf

对于一个流，系统一般会默认给流缓冲的方式，比如标准输入输出使用行缓冲默认定向到终端，如果我们想要修改可以通过如下的函数进行修改

这两个函数的第一个参数都要求传入要给已经打开的流

~~~cpp
#include <stdio.h>

void setbuf(FILE *restrict stream, char *restrict buf);
// stream：想要修改的流
// buf：我们用户手动指定的用户缓冲区，系统在输入输出的时候数据都是先到这里
// 这个函数设置之后默认该流就是全缓冲的(一般是)，想要设置行缓冲或者不带缓冲需要用下面的函数
// 当然如果buf传递的是nullptr，那么这个流会被设置未不带缓冲

int setvbuf(FILE *restrict stream, char *restrict buf, int mode, size_t size);
// 作用和上面的函数类似，但是有一些其他的功能
// mode：使用这个函数，我们可以具体的设置缓冲类型，具体如下:
	// _IOFBF 全缓冲
	// _IOLBF 行缓冲
	// _IONBF 不带缓冲
// 如果我们指定为全缓冲或者行缓冲，则buf和size我们可以指定选择一个缓冲区和大小
// 系统给我们提供了一个默认的缓冲区大小的宏 BUFSIZ ，指是8192个字节
// 当然我们指定这个流是带缓冲的，但是我们给的buf是nullptr，那么标准`I/O`会自动分配给该流适当长度的缓冲区，比如可以是BUFSIZ
~~~

我们可以用下面这个图进行更详细的总结他们的运作：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230907145440431.png" alt="image-20230907145440431" style="zoom:80%;" />

我们要知道，虽然我们指定了缓冲区的大小，但是缓冲区中还可能存放着他自己的管理操作信息，所以可以存放在当中的实际字节数要小于缓冲区的大小，这个并不是很重要，因为缓冲区的大小一般都是往上开够用了

**一般而言，我们可以让系统自己选择缓冲区的长度，然后自动分配缓冲区，这样关闭流的时候，标准`I/O`库会自动释放缓冲区**

### 函数fflush

任何时候，我们都可以强制冲洗一个流

**这个函数会让所有未写的数据都被送至内核当汇总(内核中也有缓冲区，然后就可以进行后续的操作)**

**如果传递的是`nullptr`，那么所有输出流都会被冲洗**

~~~cpp
#include <stdio.h>

int fflush(FILE *stream);
~~~

### 例子

#### 全缓冲

我们写一个例子来实际操作一下

在这个程序当中，我将标准输出的缓冲区定向为我设置的`outbuf`数组，然后分两次`puts`一些内容；然后刷新，然后再写

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

char outbuf[BUFSIZ];  // BUFSIZ是指默认给的缓冲区大小，是8192个字节

int main() {
    setbuf(stdout, outbuf);  // 把缓冲区与流相连

    puts("This is a test of buffered output.");  // puts最后会自带一个换行符
    puts(outbuf);                                // 这里我除了写入自定义字符串，我把outbuf的内容也写一遍，所以会有两个换行符

    sleep(3);

    fflush(stdout);  // 刷新

    puts("This is a test of buffered output.");

    sleep(3);

    return 0;
}

~~~

结果：

前面三行(包括第三行的空行)经过3秒输出，最后一行又经过3行输出，符合我们的预期

我们可以看出这里的`setbuf`给我们的是全缓冲

![image-20230907150903812](https://image.davidingplus.cn/images/2025/01/31/image-20230907150903812.png)

#### 不带缓冲

下面我给一个不带缓冲的例子

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    puts("This is a test of buffered output.");  // puts最后会自带一个换行符

    sleep(3);

    fflush(stdout);  // 刷新

    puts("This is a test of buffered output.");

    sleep(3);

    return 0;
}
~~~

结果：

这两行都是执行`puts`函数之后立即输出的，可以看出不带缓冲

![image-20230907151214997](https://image.davidingplus.cn/images/2025/01/31/image-20230907151214997.png)

#### 行缓冲

那怎么能少了行缓冲呢？还是全缓冲的例子，我这次设置为行缓冲

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

char buf[BUFSIZ] = {0};

int main() {
    setvbuf(stdout, buf, _IOLBF, sizeof(buf));

    puts("This is a test of buffered output.");  // puts最后会自带一个换行符

    sleep(3);

    fflush(stdout);  // 刷新

    puts("This is a test of buffered output.");

    sleep(3);

    return 0;
}
~~~

结果：

由于`puts`会自带一个换行符，所以也是立即输出的

![image-20230907151444480](https://image.davidingplus.cn/images/2025/01/31/image-20230907151444480.png)

## 打开流

我们可以用以下的函数来打开一个标准`I/O`流

~~~cpp
#include <stdio.h>

FILE *fopen(const char *restrict pathname, const char *restrict mode);
// 打开指定路径的文件，返回用于操作的标准I/O流，可以是绝对或者相对路径

FILE *fdopen(int fd, const char *mode);
// 通过一个已有的文件描述符，并用一个标准的I/O流和该文件描述符进行结合，让我们可以通过流的方式操作文件描述符
// 但是为什么我们不直接打开文件呢？因为这个函数主要是用于一些特殊的文件描述符的，比如创建管道和创建socket套接字获得的文件描述符，这些文件描述符没有路径，所以我们可以通过fdopen来与之结合

FILE *freopen(const char *restrict pathname, const char *restrict mode, FILE *restrict stream);
// 一般用来重定向标准输入，标准输出和标准错误(重要！)
// 意思是我们可以把输出到终端的数据输出到文件中；把从终端中输入变为从文件中输入读取
// pathname：我们指定的文件，可以替代标准输出的位置和标准输入的来源
~~~

上面的`mode`参数是一个字符串类型，用于指定标准`I/O`流的读写方式，具体如下图，他们和**文件状态标志**对应：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230907152718910.png" alt="image-20230907152718910" style="zoom:80%;" />

书上还给出了一些注意事项，如下：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230907153412560.png" alt="image-20230907153412560" style="zoom:80%;" />

### 例子

打开文件返回标准`I/O`流的例子就不写了，这里写一个重定向标准输入和标准输出的例子

这里我从文件中读取数据并经过简单计算然后写入另一个文件

**注意打开之后需要关闭，这里就是关闭标准输入和输出，注意最好是在程序末尾，不然关闭了之后就没办法正常输入输出了**，下面也给出了具体说明

~~~cpp
#include <iostream>
using namespace std;

int main() {
    // 重定向stdin和stdout
    freopen("03_src.txt", "r", stdin);
    freopen("03_dest.txt", "w", stdout);

    int a, b;
    cin >> a >> b;

    cout << a + b << endl;

    // 关闭重定向的标准输入输出流，注意放在末尾，不然关了之后标准输入输出用不了
    fclose(stdout);
    fclose(stdin);

    cout << "hello" << endl;

    return 0;
}
~~~

结果：

确实关闭了之后`hello`没有打印出来

![image-20230907153827144](https://image.davidingplus.cn/images/2025/01/31/image-20230907153827144.png)

### 其他注意事项(了解)

我就放书上的图了

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230907154448173.png" alt="image-20230907154448173" style="zoom:80%;" />

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230907154453879.png" alt="image-20230907154453879" style="zoom:80%;" />

### 函数fclose

我们可以用`flose`函数来关闭一个打开的流，这也是为什么前面关闭之后就没办法打印到终端了

~~~cpp
#include <stdio.h>

int fclose(FILE *stream);
~~~

注意：

- **在文件关闭之前，系统会自动冲洗输出缓冲中的所有数据，然后进行输出；输入缓冲区中的所有数据将被丢弃**
- **如果标准I/O库已经为该流自动分配了一个缓冲区，那么系统会自动释放该缓冲区**
- **同样当一个进程终止或者结束的时候，也会自动冲洗输出缓冲中的所有数据，输入缓冲区的数据会被丢弃；所有打开的标准I/O流都会被关闭**

## 读和写流

一旦打开了流，我们有三种不同的方式的非格式化`I/O`进行选择，然后进行读写操作：

- **每次一个字符的`I/O`，意思是一次读写一个字符；如果流是带缓冲的，那么标准`I/O`会处理缓冲**
- **每次一行的`I/O`，我们可以借助标准`I/O`函数`fgets`和`fputs`来实现，每行都以一个换行符终止**
- **直接`I/O`，每次`I/O`操作的时候读或者写指定的对象，这个对象具有一定的长度；比如`fread`函数和`fwrite`函数，他们可以让底层的`read`和`write`函数调用次数更少，因此执行效率更高**

### 输入函数

#### getc系列

以下的函数可以用于一次读一个字符

~~~cpp
#include <stdio.h>

// getc和fgetc都可以用于从一个流当中读取一个字符
// 区别是：getc可以被实现为宏，fgetc不可以
int getc(FILE *stream);
// 返回值：由于我们使用的编码是utf-8，所以是单字节定向，对应的是ASCII码集，返回的值是int类型，也就是字符的ASCII码值
// 当读取到文件末尾或者发生错误的时候，返回EOF，EOF是标准I/O定义的一个宏，值为-1

int fgetc(FILE *stream);
// 除了上面的一个区别其他相同

int getchar(void);
// 等效于getc(stdin); 就不赘述了
~~~

所以和`read`函数类似的，我们要检验读取到文件末尾，**但是这里不同的是当读取到文件末尾或者发生错误的时候这三个函数都返回相同的值，都是`EOF`**，所以我们要想办法区分

#### 函数ferror和feof

这两个函数就是用来检测到底是发生错误还是读取到末尾了；

**标准`I/O`为每个`FILE`对象维护了两个标志，也就是出错标志和文件末尾标志，他们就可以分别被下面的`ferror`和`feof`来获得**，这样我们就可以判断到底是什么状况

~~~cpp
#include <stdio.h>

// 获得文件的出错标志 1真 0假
int ferror(FILE *stream);

// 获得文件末尾标志 1真 0假
int feof(FILE *stream);

// 可以清除这两个标志
void clearerr(FILE *stream);
~~~

#### 例子

我们写一个程序来实操一下

我们做两次测试，一次正常读完，一次读取出错，如下所示：

~~~cpp
#include <iostream>
using namespace std;

int main() {
    // 复习一下，将标准输入重定向
    freopen("04.txt", "r", stdin);
    // freopen("05.txt", "r", stdin);

    int ret;
    while (EOF != (ret = getc(stdin)))
        cout << (unsigned char)ret;

    // 我们来看一下EOF对应的是哪个状态
    if (ferror(stdin))
        cout << "ferror: read error" << endl;
    if (feof(stdin))
        cout << "feof: end of file" << endl;

    // 关闭
    fclose(stdin);

    return 0;
}
~~~

结果：

正常读完

![image-20230907170621432](https://image.davidingplus.cn/images/2025/01/31/image-20230907170621432.png)

读取错误

![image-20230907170649491](https://image.davidingplus.cn/images/2025/01/31/image-20230907170649491.png)

#### 函数ungetc

**当从流当中读取数据之后，我们可以调用`ungetc`把字符压回流当中**

这里我准备从一个文件当中读取数据，然后读取之前我压入了一些字符到流中

~~~cpp
#include <iostream>
using namespace std;

int main() {
    // 打开一个文件
    FILE* file_stream = fopen("04.txt", "r");

    // 先压入几个字符
    for (int i = 0; i < 10; ++i)
        ungetc((int)'a' + i, file_stream);

    int ret = EOF;
    while (EOF != (ret = getc(file_stream)))
        cout << (unsigned char)ret;

    return 0;
}
~~~

结果：

**我压入的顺序是`a`到`j`，但是输出的顺序是`j`到`a`，可以压入和输出的关系是一个栈的关系，这一点注意一下**

![image-20230907171454664](https://image.davidingplus.cn/images/2025/01/31/image-20230907171454664.png)

注意：

- **我们不能回送`EOF`，因为下一次就读到他表示错误或者文件结束，但是其实并没有，就会出问题**
- **当读取到文件末尾的时候，我们可以回送一个字符，这样下次就会读取这个字符，再次读就会返回`EOF`，之所以能这样做是一次成功的`ungetc`会调用`clearerr`函数清除两个标志，因为压入之后这两个标志应该会不存在**

### 输出函数

对应上面的输入函数，也有一个输出函数，就是一次输出一个字符到输出缓冲区

~~~cpp
#include <stdio.h>

// 完全和上面的对应，这里就不写了
int fputc(int c, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
~~~

**`putc`函数和`ungetc`函数都可以向流中写数据，`putc`一次写一个字符到输出流缓冲区中，`ungetc`和他的区别大了，他是往读取缓冲区写，这样读的时候就会先读取我写的数据，注意如果不加设置为不带缓冲他们会先写到缓冲区当中**

#### 例子

我们用一个程序深刻理解上面的意思

这里我打开一个文件用读和追加的方式打开，然后向文件中写数据，我人为指定为全缓冲，由系统给我分配，然后写完我休眠几秒，然后再读取，我们看一下程序的结果

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    // 打开一个文件
    FILE* file_stream = fopen("04.txt", "a+");

    setvbuf(file_stream, nullptr, _IOFBF, 0);

    // 先写入几个字符
    for (int i = 0; i < 10; ++i)
        putc((int)'a' + i, file_stream);

    // 最后写一个换行符
    putc((int)'\n', file_stream);

    sleep(5);

    // 重置文件偏移量指针
    fseek(file_stream, 0, SEEK_SET);

    int ret = EOF;
    while (EOF != (ret = getc(file_stream)))
        cout << (unsigned char)ret;

    return 0;
}
~~~

结果：

**程序睡了5秒之后才输出，这也证明了是数据是先到缓冲区当中了，然后后面调用了`fseek`或者`getc`函数刷新了缓冲区，因此就能正常读到数据，注意这里`putc`和`getc`使用的都是系统给我分配的全缓冲，我们在两次操作之间缓冲区被冲洗了；最后输出到终端上用的标准输出的行缓冲，这个与上面无关**

![image-20230907173421891](https://image.davidingplus.cn/images/2025/01/31/image-20230907173421891.png)

## 每行一行I/O

### 每次输入一行(fgets系列)

下面的函数提供了每次输入一行的功能

~~~cpp
#include <stdio.h>

// 这两个函数都制定了缓冲区的地址，然后将读到的行送到buf中
// fgets函数可以指定从指定的流读取
char* fgets(char* restrict buf, int n, FILE* restrict fp);

// gets从标准输入读
char* gets(char* buf);
~~~

注意：

- **注意这里`buf`是我们存储数据的地方，不是缓冲区，就像`read`函数需要`buf`来存放数据一样，缓冲区需要我们指定大小，就是`n`，如果我们没用`setvbuf`设置缓冲区的属性，那么默认就由系统给我们分配，然后程序结束系统给我们回收(这里要理解好)**
- **对于`fgets`，我们必须指定缓冲区的长度，这个函数直到读到下一个换行符为止，由于字符串的末尾有一个`'\0'`符号，所以我们实际上只能读取到`n-1`个字符，这也是标准`I/O`为我们提供的保护；但是如果该行包括最后一个换行符超出了`n-1`个字符，则`fgets`只返回一个不正常的行，但是缓冲区只以`null`字节结尾，下一次的调用会从这里继续**

- **我们不推荐使用`gets`函数，因为他没有手动指定缓冲区的大小，这就可能造成缓冲区溢出，然后导致内存泄漏，这一点的危害是致命的，所以我们不推荐，而且它也很局限，只能从标准输入中读取**

#### 例子

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    FILE* file_stream = fopen("05.txt", "r");
    if (!file_stream) {
        perror("fopen");
        return -1;
    }

    char buf[1024] = {0};

    fgets(buf, 5, file_stream);  // 如果读的数据超过缓冲区大小，那么最后一个字符会留为'\0'，然后冲洗缓冲区到buf当中

    cout << buf;

    sleep(5);

    return 0;
}
~~~

结果：

程序睡了五秒之后然后输出，为什么只有4个字符？**因为缓冲区满了，但是他会留出末尾一个'\0'符号，然后送到存储位置`buf`中**，然后由于标准输出是行缓冲，没有换行符，所以就睡了五秒再输出

![image-20230909093226773](https://image.davidingplus.cn/images/2025/01/31/image-20230909093226773.png)

### 每次输出一行(fputs系列)

~~~cpp
#include <stdio.h>

int fputs(const char *restrict s, FILE *restrict stream);
// 将一个以null字节结尾的字符串写入到指定的流，尾部的null('\0')不写出
// 注意，这个函数的判断是以null字节，他不会在末尾给你补上换行符'\n'
// 还有，是把这个字符串写到目标流的缓冲区中，那什么时候写入就需要看流是什么缓冲了
// 通常我们都喜欢在null字节前，放一个回车换行符'\n'

int puts(const char *s);
// 写到标准输出，并且在后面自带一个换行符，可能是写到标准输出默认写道终端，换行更美观
// 我们尽量避免使用puts函数，虽然他并不像gets可能让缓冲区爆掉，因为这样我们就可以不用关心是否需要手动写一个换行符
// 但是有时候我们打印一行数据用puts还是挺香的
~~~

#### 例子

~~~cpp
#include <iostream>
using namespace std;
#include <unistd.h>

int main() {
    const char* str = "helloworld";

    fputs(str, stdout);  // 不带换行符

    sleep(3);

    puts(str);  // 自带换行符

    return 0;
}
~~~

结果：

程序睡了3秒输出，因为标准输出是行缓冲，`fputs`函数输出的字符串不带回车符，`puts`带有

![image-20230909095214017](https://image.davidingplus.cn/images/2025/01/31/image-20230909095214017.png)

## 二进制I/O

上面的函数以一次一个字符或者一次一行进行操作，如果我们进行二进制I/O操作，那么我们更愿意一次读写一个完整的结构

- **如果使用`getc`和`putc`函数，那么我们一次只能读或者写一个字符，必须通过循环进行整个结构的读写；**
- **如果使用`fputs`和`fgets`函数，`fgets`函数遇到换行符'\n'或者缓冲区满或者遇到`null`字节会停止，`fputs`函数遇到null字节就会停止，这样想要读完也要循环，也是相对比较麻烦的**

因此类似于系统调用的`read`和`write`函数，我们这里有两个二进制`I/O`操作可以读取一个完整的结构

~~~cpp
#include <stdio.h>

size_t fread(void *restrict ptr, size_t size, size_t count, FILE *restrict stream);

size_t fwrite(const void *restrict ptr, size_t size, size_t count, FILE *restrict stream);
// 参数：
	// ptr：要存放读取数据的存储区或者写出数据的数据来源区
	// size：读取或者写入的数据单元的大小，我们一般读文件都是读字符串，然后就给char的大小1就好
	// count：需要读或者写的大小，读的话我们不知道stream的数据有多大，可以给存储区ptr的大小，这也说明了返回值小于count不一定是错误，也有可能是文件读到末尾了，因为我们事先不知道stream的数据有多大；写的话就给ptr数组数据的个数就好了，如果不相等就是错误
	// stream：标准I/O文件流
~~~

这两个函数的作用在下面给出：

- 有两种常见的用法：

  - **读或者写一个二进制数组**

    ![image-20230909103740225](https://image.davidingplus.cn/images/2025/01/31/image-20230909103740225.png)

  - **读或者写一个结构**

    ![image-20230909103759521](https://image.davidingplus.cn/images/2025/01/31/image-20230909103759521.png)

  **这两种用法其实有相同的地方，第一个参数传入的是想要读或者写入的结构单位，第二个参数传入的是这个单位的大小，第三个参数传入的是想要读或者写的个数，第四个参数给定的是指定的流，当然首先是读入或者写入缓冲区，后面根据情况判断什么时候才会到达目标位置**

- 这两个函数**返回读或者写的对象个数**

  - **对于读，如果出错或者到达文件末尾，返回的值不为`count`，这个时候可以用`ferror`或者`feof`来判断是哪一种情况**
  - **对于写，如果返回的值不为`count`，那么就是出错了**

### 例子

第一个例子，我们把一些二进制数据和结构进行标准输出

~~~cpp
#include <iostream>
using namespace std;
#include <string>

struct Person {
    string name;
    string sex;
    double height;
    double weight;
};

int main() {
    // 将二进制数组的一些元素写到标准输出
    char data[10] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};

    fwrite(&data[2], sizeof(char), 3, stdout);

    puts("");  // 输出一个空行

    // 写一个结构到标准输出
    Person p{"Paul", "boy", 190, 88.5};

    fwrite(&p, sizeof(p), 1, stdout);

    puts("");  // 输出一个空行

    return 0;
}
~~~

结果：

**字符串是可以正常输出的**，其他类型的数据可能因为编码或者类型问题会出现乱码，但是我们一般都是处理字符串，所以问题不大

![image-20230909105848043](https://image.davidingplus.cn/images/2025/01/31/image-20230909105848043.png)

第二个例子，我们从文件当中读取数据，然后输出，注意体会第二个参数`size`和第三个参数`count`的含义

~~~cpp
#include <cstring>
#include <iostream>
using namespace std;

#define MAX_BUFFER_SIZE 1024

int main() {
    // 从06.txt中读取数据，然后输出到屏幕上
    FILE* file_stream = fopen("06.txt", "r");
    if (!file_stream) {
        perror("fopen");
        return -1;
    }

    char buf[MAX_BUFFER_SIZE] = {0};
    // 读到buf中
    fread(buf, 1, sizeof(buf), file_stream);

    // 标准输出
    int ret = fwrite(buf, 1, strlen(buf), stdout);
    if (ret != strlen(buf)) {
        perror("fwrite");
        return -1;
    }

    return 0;
}
~~~

结果：

**我们读取和写出的单位都是字符，所以`size`给的是1，读取的时候不知道`stream`数据到底有多少，所以我们给大一点，可以给`buf`数据区的大小，所以返回值不为count有可能是读到末尾或者失败；写的时候就给`buf`的实际长度。**

**注意，`buf`可不是`file_stream`的读写缓冲区，`buf`是我们指定的存储数据的地方，从写之后`strlen(buf)`不为0这一点也可以看出来**

![image-20230909110026630](https://image.davidingplus.cn/images/2025/01/31/image-20230909110026630.png)

注意`fread`，`fwrite`函数和`read`，`write`函数的联系和区别，他们的使用方式还是有区别的

## 定位流

有三种方法定位I/O流，如下图：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230909110812555.png" alt="image-20230909110812555"  />

我们了解下面的函数即可：

~~~cpp
#include <stdio.h>

long ftell(FILE *stream);
// 返回当前文件位置的偏移量，错误则返回long(-1)，并且修改错误号

int fseek(FILE *stream, long offset, int whence);
// 和lseek函数一样，可以设置文件偏移量指针
// 第一个参数是文件流指针，第二个参数是我们给的偏移量，这个偏移量可正可负，也就是说，我们的指针可以往前移

// 第三个参数`whence`：
	// - `SEEK_SET` 设置偏移量，从头开始
	// - `SEEK_CUR` 设置偏移量：当前位置 + 第二参数`offset`的值
	// - `SEEK_END` 设置偏移量：文件大小 + 第二参数`offset`的值

// 返回值是文件指针的新位置，失败返回-1并且修改`errno`

void rewind(FILE *stream);
// 移动文件指针到开头
~~~

还有两个函数，他们除了类型和前面的函数不一样之外，是off_t，其他相同，但是我们还是倾向于用上面的函数：

~~~cpp
#include <stdio.h>

typedef long off_t

// 这里的类型是off_t，前面是long
off_t ftello(FILE *stream);

int fseeko(FILE *stream, off_t offset, int whence);
~~~

### 例子

~~~cpp
#include <iostream>
using namespace std;

#define MAX_BUFFER_SIZE 1024

int main() {
    FILE* file_stream = fopen("07.txt", "r");
    if (!file_stream) {
        perror("fopen");
        return -1;
    }

    long pos = ftell(file_stream);
    cout << pos << endl;

    // 现在我读取一个字符
    char buf[MAX_BUFFER_SIZE] = {0};
    fread(buf, 1, 1, file_stream);

    cout << buf << endl;
    pos = ftell(file_stream);
    cout << pos << endl;

    // 设置文件偏移指针到开头
    // rewind(file_stream);
    fseek(file_stream, 0, SEEK_SET);

    fread(buf, 1, 2, file_stream);  // 这里第二次读从buf的地址位置开始写入，所以之前的数据会被覆盖

    cout << buf << endl;
    pos = ftell(file_stream);
    cout << pos << endl;

    return 0;
}
~~~

注意里面第二次fread的注释，为什么会被覆盖

结果：

显然在我们的预期内

![image-20230909113626829](https://image.davidingplus.cn/images/2025/01/31/image-20230909113626829.png)

## 格式化I/O

### 格式化输出

#### printf系列

格式化`I/O`是通过`printf`系列函数来处理的

~~~cpp
#include <stdio.h>

// 将格式化数据写到标准输出
int printf(const char *restrict format, ...);

// 将格式化数据写到指定的流
int fprintf(FILE *restrict stream, const char *restrict format, ...);

// 将格式化数据写到指定的文件描述符
int dprintf(int fd, const char *restrict format, ...);

// 前三个函数成功返回输出字符数，如果输出错误，返回负值

// 将格式化数据写到我们指定的buf存储区
int sprintf(char *restrict buf, const char *restrict format, ...);
// 成功返回输入的字符数，如果编码错误，返回负值

// 为了避免sprintf函数可能造成的buf装满而爆掉，引入了snprintf函数，需要给出指定的长度
int snprintf(char *restrict buf, size_t size, const char *restrict format, ...);
// 如果给定的size足够大，返回将要存入数组的字符数，如果编码错误，返回负值
~~~

#### 格式化标准

这显然就涉及到格式化的标准了，当然这里需要我们自己进行对数据进行合适的格式化处理

这些标志其中括号里面是可选的，`convtype`是不可选的，如下图：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230909142120139.png" alt="image-20230909142120139" style="zoom:80%;" />

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230909142131548.png" alt="image-20230909142131548" style="zoom:80%;" />

![image-20230909142140609](https://image.davidingplus.cn/images/2025/01/31/image-20230909142140609.png)

#### vprintf函数(了解)

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230909144901212.png" alt="image-20230909144901212" style="zoom: 80%;" />

### 格式化输入

#### scanf系列

以下几个函数用作格式化输入

~~~cpp
#include <stdio.h>

// 从标准输入中读取
int scanf(const char *restrict format, ...);

// 从标准输入中读取写入流中(是先写入流缓冲区中)
int fscanf(FILE *restrict stream, const char *restrict format, ...);

// 从标准输入中读取写入字符串str中
int sscanf(const char *restrict str, const char *restrict format, ...);
~~~

这几个函数返回值我们可以不用判断，判断了也没有什么作用，所以只提一嘴：返回赋值的输入项数，若输入出错或者在任一转换之前已经到达文件末端，则返回EOF

#### 格式化标准

同前面，截图：

![image-20230909145428023](https://image.davidingplus.cn/images/2025/01/31/image-20230909145428023.png)

![image-20230909145440049](https://image.davidingplus.cn/images/2025/01/31/image-20230909145440049.png)

#### vscanf系列(了解)

![image-20230909145502533](https://image.davidingplus.cn/images/2025/01/31/image-20230909145502533.png)

![image-20230909145508121](https://image.davidingplus.cn/images/2025/01/31/image-20230909145508121.png)

## 实现细节

在标准I/O中，每一个标准I/O流都对应着一个与其相关联的文件描述符，我们可以调用以下函数来进行获得

~~~cpp
#include <stdio.h>

int fileno(FILE *stream);
~~~

### 例子

我们写个程序简单看一下

~~~cpp
#include <iostream>
using namespace std;

int main() {
    FILE* file_stream = fopen("07.txt", "r");
    if (!file_stream) {
        perror("fopen");
        return -1;
    }

    int fd = fileno(file_stream);
    if (-1 == fd) {
        perror("fileno");
        return -1;
    }

    printf("file descriptor: %d\n", fd);

    return 0;
}
~~~

结果：

应该返回文件描述符的值是3，结果是，符合预期

![image-20230911103808954](https://image.davidingplus.cn/images/2025/01/31/image-20230911103808954.png)

我们这里再复习一下，标准错误默认是不带缓冲的，因为我们想要错误信息尽快显示到屏幕上，而不是关心是否具有换行符；标准输入和输出在连接到终端的时候默认是行缓冲的，如果定向到文件就是全缓冲

## 临时文件(了解)

### tmpnam系列

有两个库函数可以用来帮助创建临时文件

~~~cpp
#include <stdio.h>

char *tmpnam(char *s);
// tmpnam函数产生一个与现有文件名不同的一个有效路径名字符串，给的是路径名字
// 每次调用的时候都产生一个不同的路径名，最多的次数是TMP_MAX(238328)
// 现在tmpnam函数已经被弃用了，所以在使用的时候会报警告，但是能用，不推荐使用
// 在文件关闭或者程序结束的时候就自动删除这个临时文件

FILE *tmpfile(void);
// 用来创建一个二进制临时文件，同样在文件关闭或者程序结束的时候就自动删除这个临时文件
// UNIX对二进制文件不做区分
~~~

#### 例子

我们先写一个关于tmpnam的例子

~~~cpp
#include <iostream>
using namespace std;

int main() {
    char name[1024] = {0};

    // c++使用这个函数会被警告说不安全，tmpnam函数返回一个临时文件的路径名，这个路径名中对应的文件名和本文件不相同
    const char* tmp_path = tmpnam(name);
    if (!tmp_path) {
        perror("tmpnam");
        return -1;
    }

    cout << TMP_MAX << endl;

    cout << tmp_path << endl;
    cout << name << endl;

    return 0;
}
~~~

结果：

- 在编译的时候，说这个函数不推荐，很危险
- 输出结果可以看出`TMP_MAX`的值是238328，产生的有效路径字符串路径是`/tmp/file6PoHsM`，我们可以推测`/tmp`路径就是产生临时文件路径的，并且我传进去的`name`字符串数组也被赋值为了路径名，返回值也是这个，双重保险

![image-20230915150756157](https://image.davidingplus.cn/images/2025/01/31/image-20230915150756157.png)

我们来看下`/tmp`路径：

文件在程序结束之后就是释放掉了，确实是临时文件，做得很好

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230915150920192.png" alt="image-20230915150920192" style="zoom:80%;" />

### mkdtemp系列

刚才使用`tmpnam`的时候编译器就提醒我们`tmpnam`函数不安全，建议使用`mkdtemp`函数

~~~cpp
#include <stdlib.h>

char *mkdtemp(char *template);
// 这个函数可以用来创建一个目录，这个目录有一个唯一的名字
// 这个名字是根据我传入的template路径指定的，我的template可以是一个绝对路径也可以是一个相对路径
// 但是为了保证名字唯一，必须要在这个字符串后面加上XXXXXX(6个)的路径名，系统会替换这六个字符让目录名字唯一

int mkstemp(char* template);
// 以唯一的名字创建一个普通文件并且打开文件，返回的值是文件描述符
// 成功，返回文件描述符；失败，返回-1

// 这两个函数创建的目录和文件都不会自动删除，如果想要删除我们必须要手动删除才可以！
~~~

#### 例子

我们先试一下`mkdtemp`函数

~~~cpp
#include <iostream>
using namespace std;

int main() {
    char name[1024] = {"./mydirXXXXXX"};

    char *file_path = mkdtemp(name);
    if (!file_path) {
        perror("mkdtemp");
        return -1;
    }

    cout << name << endl;
    cout << file_path << endl;

    return 0;
}

~~~

结果：

创建了一个目录，并且`name`字符串和函数的返回值都是这个路径，并且路径的位置也是正确的，当然我要选择绝对路径或者相对路径都是可以的

![image-20230915161012243](https://image.davidingplus.cn/images/2025/01/31/image-20230915161012243.png)

当然`mkdtemp`函数还有其他的作用：

![image-20230915161221228](https://image.davidingplus.cn/images/2025/01/31/image-20230915161221228.png)

## 内存流(了解)

这部分看不懂思密达。。。

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230915163646515.png" alt="image-20230915163646515" style="zoom:80%;" />

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230915163654809.png" alt="image-20230915163654809" style="zoom:80%;" />

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20230915163712024.png" alt="image-20230915163712024" style="zoom:80%;" />

# 后续

TODO

