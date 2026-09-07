# Lab1：熟悉开发环境

实验难度出自官网，实验简述和实验耗时是主观体验

* <font style="color:rgb(35, 38, 59);">Lab1-util  </font><code><font style="color:rgb(35, 38, 59);">简述</font></code><font style="color:rgb(35, 38, 59);">：使用几个常见的系统调用编程，熟悉开发环境  </font>
* <code><font style="color:rgb(35, 38, 59);">难度</font></code><font style="color:rgb(35, 38, 59);">：3 easy   3 moderate  </font><code><font style="color:rgb(35, 38, 59);">耗时</font></code><font style="color:rgb(35, 38, 59);">：5h  </font>
* <code><font style="color:rgb(35, 38, 59);">建议</font></code><font style="color:rgb(35, 38, 59);">：如果没用过这几个系统调用，做起来可能会有些困难，但不用太灰心，这个lab和后面的lab没有太大关联，不好做直接看参考答案就行，主要是熟悉开发环境</font>

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/util.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><http://xv6.dgs.zone/labs/requirements/lab1.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://blog.miigon.net/posts/s081-lab1-unix-utilities/>

因为实验题目和要求字数较多，放在这里的话文章会很臃肿，**因此请大家自行去上述链接查看实验要求**，后续实验也是如此。

## 实验准备

本次实验是用来熟悉xv6操作系统的，学会使用几个系统调用，为后续的实验热热身。

首先，让我们切换到第一个实验的git分支上，也就是util分支。使用命令行的同学，在`xv6-labs-2020`路径下执行命令：

```shell
git checkout util
```

就切换到了第一个实验--util实验的分支。切换成功后，可以看到路径下多出了很多文件，这就是xv6操作系统的相关文件：

![1733120147042-ebf8adda-48e8-42ee-b7b8-336bc9c028d9.png](./img/4j2FunEKTDI-0qQx/1733120147042-ebf8adda-48e8-42ee-b7b8-336bc9c028d9-295283.png)

如果是使用vscode和上一篇文章中我推荐的插件的同学，可以按以下步骤切换分支：

![1733121100161-e086d4a0-4368-4a2c-9129-991a72627e63.jpeg](./img/4j2FunEKTDI-0qQx/1733121100161-e086d4a0-4368-4a2c-9129-991a72627e63-375553.jpeg)

![1733121156713-e9373cf9-5dc5-4236-be76-ede1105f9c3d.jpeg](./img/4j2FunEKTDI-0qQx/1733121156713-e9373cf9-5dc5-4236-be76-ede1105f9c3d-306997.jpeg)

后续切换实验分支操作不再赘述。

介绍一下后续实验中经常会接触到的一些文件：

* `kernel`文件夹：里面包含了操作系统内核空间的源代码文件，负责底层的硬件管理和系统功能实现
* `user`文件夹：里面包含了用户空间的应用程序源文件代码，运行在 操作系统内核提供的环境上
* `Makefile`文件： 用于自动化构建的脚本文件，  在这个项目中用来构建xv6操作系统
* `grade-lab-xxx`文件：用来给实验评分的脚本文件。当完成了一个小实验或者完成了整个实验后，运行此脚本可以给本次实验打分

一些常用命令：

* 运行并构建xv6操作系统： `make qemu`
* 退出xv6： `Ctrl-a x` (先按Ctrl+a，再按x)
* 测试是否完成lab： `make grade`
* 测试是否完成lab的子任务： `make GRADEFLAGS=<lab name> grade`
  * 如在util lab中，想测试是否完成子任务sleep，运行`make GRADEFLAGS=sleep grade`
* gdb调试
  * 一个终端执行 `make CPUS=1 qemu-gdb`
  * 在另一个终端执行 `riscv64-unknown-elf-gdb kernel/kernel`
  * 如果报错`bash: riscv64-unknown-elf-gdb: command not found`可参考[此文](https://blog.csdn.net/csdndogo/article/details/130772956)解决

怎么调试：

为了快速上手，可以先跳过这部分，需要调试时再学

使用gdb进行调试\
[文章](http://xv6.dgs.zone/tranlate_books/Use%20GUN%20Debugger.html)\
[视频](https://www.youtube.com/watch?v=s-Z5t_yTyTM) 26:10处

## 实验

<font style="color:rgb(35, 38, 59);">本次实验主要是熟悉xv6开发环境，学会使用常见的几个系统调用来编写代码。如果觉得困难可以直接看答案，该实验和后面的实验关联不大。</font>

<font style="color:rgb(35, 38, 59);">这里再次说明一下，因为很多实验题目、要求以及提示等字数较多，</font>**因此请大家自行去文章顶部链接查看实验完整要求**<font style="color:rgb(35, 38, 59);">。后续实验也是如此，不再赘述。</font>

### <font style="color:rgb(35, 38, 59);">sleep (easy)</font>

按照题目要求，要在user文件夹里创建一个sleep.c文件，在其中编写代码实现sleep命令功能，需要检查参数是否存在。根据实验提示可以很简单的实现该功能：

```c
// user/sleep.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("usage: sleep <ticks>\n");
	}
	sleep(atoi(argv[1]));
	exit(0);
}
```

写完代码后，需要在Makefile文件中添加该功能：

```makefile
UPROGS=\
	$U/_cat\
	$U/_echo\
	$U/_forktest\
	$U/_grep\
	$U/_init\
	$U/_kill\
	$U/_ln\
	$U/_ls\
	$U/_mkdir\
	$U/_rm\
	$U/_sh\
	$U/_stressfs\
	$U/_usertests\
	$U/_grind\
	$U/_wc\
	$U/_zombie\
	$U/_sleep\			//添加  $U/_sleep\
```

完成一个小实验后，可以测试这个小实验是否完成。

比如现在完成了sleep实验，可以make qemu启动xv6，然后在命令行执行`sleep 10`，看看程序是否暂停（数字大一点效果更明显）。

或者我们可以在Linux命令行中执行：

```shell
./grade-lab-util sleep
```

执行后可以看到执行的测试：

```bash
== Test sleep, no arguments == sleep, no arguments: OK (1.6s)
== Test sleep, returns == sleep, returns: OK (0.3s)
== Test sleep, makes syscall == sleep, makes syscall: OK (0.9s)
```

如果提示：`/usr/bin/env: ‘python’: No such file or directory`，解决方案为：

将<code><font style="color:rgb(76, 73, 72);">grade-lab-util</font></code><font style="color:rgb(76, 73, 72);">文件开头的</font>`#!/usr/bin/env python`改为`#!/usr/bin/env python3`，再次执行

`./grade-lab-util sleep`

后续如果想测试其他小实验，更换命令后面的实验名称即可。后续实验的测试同理

### <font style="color:rgb(35, 38, 59);">pingpong (easy)</font>

<font style="color:rgb(35, 38, 59);">练习使用管道，通过管道实现父进程和子进程之间的简单通信。</font>

<font style="color:rgb(35, 38, 59);">父进程发送一个字符给子进程，子进程接收到后再发送回父进程，最后父进程接收到子进程的回复并打印消息。</font><font style="color:rgb(51, 51, 51);">需要注意的是如果管道的写端没有</font><code><font style="color:rgb(51, 51, 51);background-color:rgb(247, 247, 247);">close</font></code><font style="color:rgb(51, 51, 51);">，那么管道中数据为空时对管道的读取将会阻塞。因此对于不需要的管道描述符，要尽早的关闭（出自</font>[该博客](http://xv6.dgs.zone/labs/answers/lab1.html#pingpong)<font style="color:rgb(51, 51, 51);">）。代码如下：</font>

```c
// user/pingpong.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv) {
    // 创建两个管道：pp2c 用于父进程到子进程的通信，pc2p 用于子进程到父进程的通信
	int pp2c[2], pc2p[2];
	pipe(pp2c); 
	pipe(pc2p); 
	
    if (fork() != 0) {                                // 父进程
        // 父进程向子进程发送一个字符
        write(pp2c[1], ".", 1);
        close(pp2c[1]);

        // 父进程从子进程读取一个字符
        char buf;
        read(pc2p[0], &buf, 1);
        printf("%d: received pong\n", getpid());
        // 等待子进程结束
        wait(0);
    }
    else {                                           // 子进程
        // 子进程从父进程读取一个字符
        char buf;
        read(pp2c[0], &buf, 1);
        printf("%d: received ping\n", getpid());

        // 子进程向父进程发送一个字符
        write(pc2p[1], &buf, 1);
        close(pc2p[1]);
    }
    
    // 关闭管道的读端
    close(pp2c[0]);
    close(pc2p[0]);

    exit(0);
}
```

<font style="color:rgb(35, 38, 59);">写完代码后，同样需要在Makefile相关位置添加 </font><code><font style="color:rgb(35, 38, 59);">$U/_pingpong\</font></code>

### <font style="color:rgb(35, 38, 59);">primes (moderate) / (hard)</font>

<font style="color:rgb(35, 38, 59);">这道题很有意思。相信大家都写过筛质数相关的题目，而这个实验结合了管道和进程来筛质数。但是正如题目所标示的hard难度一样，该实验确实有点抽象不好理解，而我也很欠缺语言组织能力，如果大家有更好的理解方式，欢迎在评论区讨论！</font>

<font style="color:rgb(35, 38, 59);">结合这张图来分析：</font>

![1733142351891-1b7d823b-42a1-42ba-b8f1-2366490ce6a0.png](./img/4j2FunEKTDI-0qQx/1733142351891-1b7d823b-42a1-42ba-b8f1-2366490ce6a0-535145.png)

<font style="color:rgb(35, 38, 59);">这里我按题目所描述的那样，将一个进程的子进程称为右邻居，将一个进程的父进程称为左邻居，所以除了父进程，每一个进程都有左邻居，除了最后一个子进程，每一个进程都有右邻居。</font><font style="color:rgb(51, 51, 51);">由于xv6的文件描述符和进程数量有限，所以题目只要求打印2~35之间的质数。</font>

主进程遍历2~35的整数，通过向管道写入整数，给他的右邻居（子进程）传输这个整数。每一个子进程第一次读取到数字时，这个数字肯定是质数（比如上图第一个子进程接收到的第一个数字是2，第二个子进程接收到的数字是5）。之后每一个子进程收到左邻居传过来的数字n，判断n是不是第一次接收到的数字的倍数，如果是，则这个数字肯定不是质数，如果不是，就传给右邻居。如此递归调用下去。

有一个需要注意的点：上面也说了，<font style="color:rgb(51, 51, 51);">xv6的文件描述符有限，而fork创建子进程的操作，会把父进程的文件描述符也都复制给子进程。从上图也可以看出，当前进程只会给右邻居写，而不会从右邻居中读。所以很多管道文件描述符，是可以关闭掉读端或者写端的。如果不及时关闭，还没循环到35，文件描述符就会耗尽了。</font><font style="color:rgb(52, 52, 60);">解决方法有两部分：</font>

* <font style="color:rgb(52, 52, 60);">关闭管道的两个方向中不需要用到的方向的文件描述符（在具体进程中将管道变成只读/只写）</font>

> 原理：每个进程从左侧的读入管道中**只需要读数据**，并且**只需要写数据**到右侧的输出管道，所以可以把左侧管道的写描述符，以及右侧管道的读描述符关闭，而不会影响程序运行。
>
> 这里注意文件描述符是进程独立的，比如，父进程的管道pipe\[2]在fork作用下复制给了子进程，在子进程中关闭了pipe\[1]写端，并不会影响父进程在pipe\[1]写数据。管道的写端有一个引用计数，只有当所有指向写端的文件描述符都关闭时，管道的写端才会真正关闭。子进程关闭它的写端只是减少了引用计数，但父进程的写端仍然打开，所以管道的写端不会关闭。读端同理。

* <font style="color:rgb(52, 52, 60);">子进程创建后，关闭父进程与祖父进程之间的文件描述符（因为子进程并不需要用到之前的管道）</font>

<font style="color:rgb(52, 52, 60);"></font>

<font style="color:rgb(52, 52, 60);">两个难点：</font>

* <font style="color:rgb(52, 52, 60);">什么时候给右邻居传数字？</font>
* <font style="color:rgb(52, 52, 60);">及时关闭管道用不到的写端或读端</font>

<font style="color:rgb(52, 52, 60);">当然这道题不必死磕，想不出来就直接看参考答案吧，以后回顾的时候可以再挑战。细节可看注释：</font>

```c
// user/primes.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 筛选质数的函数，接收一个管道作为参数
void sieve(int pleft[2]) {
	// 从左邻居读取整数
	int p;
	read(pleft[0], &p, sizeof(p));
	if(p == -1) { 
		exit(0);    // 如果读取到-1，表示结束，退出进程
	}
	printf("prime %d\n", p);    // 此时接收到的数字肯定是质数

    // 创建一个新的管道
    int pright[2];
	pipe(pright); 

	if(fork() == 0) {       // 右邻居
		close(pright[1]);   // 右邻居用不到这个管道的写端，关闭
		close(pleft[0]);    // 右邻居用不到这个管道的读端，关闭
		sieve(pright);      // 递归调用筛选函数

	} else {
        close(pright[0]);       // 当前进程用不到这个管道的读端，关闭
        // 从左邻居接收数字
        int buf;
		while(read(pleft[0], &buf, sizeof(buf)) && buf != -1) { 
			if(buf % p != 0) {                              // 如果接收到的数字不是第一次接收到的数字的倍数
				write(pright[1], &buf, sizeof(buf));        // 才往管道中给右邻居写入这个数字
			}
        }

        // 此时接收到了左邻居传来的-1，要给右邻居也传-1，结束右邻居进程
        buf = -1;
		write(pright[1], &buf, sizeof(buf)); 
		wait(0); 
		exit(0);
	}
}

int main(int argc, char **argv) {
	// 创建初始管道
	int input_pipe[2];
	pipe(input_pipe); 

	if(fork() == 0) {				// 右邻居
		close(input_pipe[1]); 		// 右邻居用不到这个管道的写端，关闭右邻居的管道写文件描述符
		sieve(input_pipe);			// 调用筛选函数
		exit(0);
	} else {						// 父进程	
		close(input_pipe[0]); 		// 父进程只会向管道中给右邻居写数据，关闭父进程的管道读文件描述符
		int i;
		for(i=2;i<=35;i++){
			write(input_pipe[1], &i, sizeof(i));	// 向管道写入2~35的整数
		}
		// 写入结束标志
		i = -1;
		write(input_pipe[1], &i, sizeof(i)); 
	}
	wait(0); 	// 等待子进程结束
	// 注意：这里无法等待子进程的子进程，只能等待直接子进程，无法等待间接子进程。
	// 在 sieve() 中再各自执行 wait(0)，形成等待链
	exit(0);	// 退出进程
}
```

<font style="color:rgb(35, 38, 59);">写完代码后，同样需要在Makefile相关位置添加 </font><code><font style="color:rgb(35, 38, 59);">$U/_primes\</font></code>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">一点Q\&A：</font>

<font style="color:rgb(35, 38, 59);">Q：会不会重复打印质数？</font>

<font style="color:rgb(35, 38, 59);">A：不会。每个子进程第一个收到的数字肯定是质数，也只会打印这一个质数，每一个子进程第一个收到的数字都不一样</font>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">Q：sieve函数中的子进程处理里面（if(fork() == 0)部分），为什么只关闭了pleft\[0]读端，不关闭pleft\[1]写端？（来自</font>[该博客](https://blog.miigon.net/posts/s081-lab1-unix-utilities/#primes-moderate--hard)<font style="color:rgb(35, 38, 59);">评论区）</font>

<font style="color:rgb(35, 38, 59);">A：在这个if里面，我们首先关闭了pright\[1]，然后把这个管道传给了右邻居，此时pright就是这个右邻居里面的pleft，即pleft\[1]早已经在上一级中关闭了。也就是 pleft\[1] 无论在哪一级，在进入 sieve() 函数的时候就都已经是关闭状态了。尝试再次关闭 pleft\[1] 有可能返回失败，也有可能因为其中的 fd 号已经被后续新建立的其他管道占用，而导致误伤其他管道的正常工作</font>

### <font style="color:rgb(35, 38, 59);">find (moderate)</font>

这个实验可以参考同文件夹下的ls.c实现。细节可看注释：

```c
// user/find.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 递归查找函数，查找路径为 path 的目录下是否有目标文件 target
void find(char* path, char* target) {
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;
    // 打开目录
	if((fd = open(path, 0)) < 0){
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}
    // 获取目录的状态信息
	if(fstat(fd, &st) < 0){
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}
    // 文件、目录分别处理
    switch (st.type) {
    // 如果是文件，检查文件名是否与目标文件名匹配
    case T_FILE:
		if(strcmp(path+strlen(path)-strlen(target), target) == 0) {
			printf("%s\n", path);
		}
        break;
    // 如果是目录
    case T_DIR:
        // 检查路径长度是否超出缓冲区大小
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
			printf("find: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
        *p++ = '/';
        // 读取目录项
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            // 获取目录项的状态信息
            if (stat(buf, &st) < 0) {
				printf("find: cannot stat %s\n", buf);
				continue;
			}
            // 排除 "." 和 ".." 目录
			if(strcmp(buf+strlen(buf)-2, "/.") != 0 && strcmp(buf+strlen(buf)-3, "/..") != 0) {
				find(buf, target);      // 递归查找子目录
			}
		}
		break;
	}
	close(fd);      // 关闭目录
}

int main(int argc, char *argv[])
{
	if(argc < 3){                   // 如果参数不足，退出程序
		exit(0);
	}
	char target[512];
	target[0] = '/';                // 为查找的文件名添加 / 在开头
	strcpy(target+1, argv[2]);      // 将目标文件名存储在 target 中
	find(argv[1], target);          // 调用查找函数
	exit(0);
}
```

<font style="color:rgb(35, 38, 59);">别忘了在Makefile相关位置添加 </font><code><font style="color:rgb(35, 38, 59);">$U/_find\</font></code>

### <font style="color:rgb(35, 38, 59);">xargs (moderate)</font>

<font style="color:rgb(35, 38, 59);">这个实验需要实现：从标准输入读取参数，并将这些参数传递给指定的程序执行。这里多举几个例子，便于理解：</font>

* `echo"file1 file2 file3" | xargs rm`

echo 输出字符串 `file1 file2 file3`。xargs 将这些字符串作为参数传递给 rm，相当于执行了：

`rm file1 file2 file3`

* ` find . -name "*.log" | xargs rm`

find 查找当前目录及其子目录中扩展名为 `.log` 的文件。xargs 将这些文件名传递给 rm，批量删除它们

* `echo"file1 file2 file3" | xargs -n 1 echo`

<code><font style="color:rgb(35, 38, 59);">-n 1</font></code><font style="color:rgb(35, 38, 59);"> 表示每次只传递一个参数给命令 echo，所以效果为，每行分别输出file1、file2、file3</font>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">代码细节见注释：</font>

```c
// user/xargs.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 运行指定的程序，接收参数
void run(char* program, char** args) {
    // 创建子进程，在子进程中执行指定的程序
    if (fork() == 0) {
		exec(program, args);
		exit(0);
	}
	return; 
}

int main(int argc, char *argv[]){
    char buf[2048];
    char* p = buf, * last_p = buf;
    char* argsbuf[128];
    char** args = argsbuf;

    // 首先将 xargs 的参数复制到 argsbuf 中
    for (int i = 1;i < argc;i++) {
        *args = argv[i];
        args++;
    }

    // 记录当前参数位置
    char** pa = args;

    // 从标准输入读取数据，存储在缓冲区 buf 中
    while (read(0, p, 1) != 0) {
        // 使用指针 p 遍历缓冲区，遇到空格或换行符时，将其替换为字符串结束符 \0
        if (*p == ' ' || *p == '\n') {
			
            *p = '\0';

            // 将参数添加到参数缓冲区 argsbuf 中
            *(pa++) = last_p;
			last_p = p+1;

            // 每当遇到换行符 \n 时，表示一组参数读取完毕，调用 run 函数执行程序，传递参数
            if (*p == '\n') {
				
				*pa = 0; 
				run(argv[1], argsbuf); 
				pa = args; 
			}
        }
        // 继续读取数据
        p++;
    }

    // 如果最后一行不是空行，同样的逻辑再处理一次
    if (pa != args) {
		*p = '\0';
		*(pa++) = last_p;
		*pa = 0; 
		
        run(argv[1], argsbuf);
    }

    // 等待所有子进程结束
    while (wait(0) != -1) {};
	exit(0);
}
```

记得<font style="color:rgb(35, 38, 59);">在Makefile相关位置添加 </font>`$U/_xargs\`

到这里就完成了整个实验1，可以执行`./grade-lab-util`或`make grade`给本次实验打分。后续实验的测评打分同理。

注：有些实验的打分脚本中要求在`xv6-labs-2020`文件夹下创建一个`time.txt`，在其中记录完成这个实验的花费的小时数，如本实验中就会检查这个文件，如果没有的话分数就是99分，有强迫症想要拿满分的同学记得做一下这步hhh。

## 可扩展的实验

不影响本次实验能否拿满分，可以扩展思维，加强对实验的理解。如果扩展的好还可以在简历上强调自己做出的改进，增加项目的独特性与含金量。可扩展的方向不局限于这里列出来的，可自行思考或参考Linux来扩展项目

* 编写一个`uptime`程序， 通过调用 xv6 的 `uptime` 系统调用，打印出自 xv6 启动以来经过的时钟周期数（称为 "ticks"）<font style="color:rgb(51, 51, 51);">（easy）</font>
* <font style="color:rgb(51, 51, 51);"> 扩展已经实现的 </font><code><font style="color:rgb(51, 51, 51);">find</font></code><font style="color:rgb(51, 51, 51);"> 命令，使其支持通过正则表达式匹配文件名。</font><code><font style="color:rgb(51, 51, 51);">user/grep.c</font></code><font style="color:rgb(51, 51, 51);">对正则表达式有一些基本的支持，可参考其逻辑（easy）</font>
* <font style="color:rgb(51, 51, 51);"> xv6 的 shell 是一个非常简化的用户程序，具备基本的命令解析和执行功能，但缺乏许多常见的 shell 特性 ，可做的改进：</font>
  * <font style="color:rgb(51, 51, 51);"> 修改 shell 从文件中处理命令时不显示 </font><code><font style="color:rgb(51, 51, 51);">$</font></code><font style="color:rgb(51, 51, 51);"> 提示符（moderate)</font>
  * <font style="color:rgb(51, 51, 51);"> 增加对 </font><code><font style="color:rgb(51, 51, 51);">wait</font></code><font style="color:rgb(51, 51, 51);"> 命令的支持  （easy）</font>
  * <font style="color:rgb(51, 51, 51);"> 支持用 </font><code><font style="color:rgb(51, 51, 51);">;</font></code><font style="color:rgb(51, 51, 51);"> 分隔的命令列表  （moderate)</font>
  * <font style="color:rgb(51, 51, 51);"> 支持子 shell，增加 </font><code><font style="color:rgb(51, 51, 51);">()</font></code><font style="color:rgb(51, 51, 51);"> 功能  （moderate)</font>

<font style="color:rgb(51, 51, 51);">（ 实现类似 </font><code><font style="color:rgb(51, 51, 51);">(cmd1; cmd2)</font></code><font style="color:rgb(51, 51, 51);"> 的功能，将括号内的命令作为一个整体执行  ）</font>

```
- <font style="color:rgb(51, 51, 51);"> 支持 Tab 补全  （easy）</font>
- <font style="color:rgb(51, 51, 51);"> 实现命令历史功能  （moderate)</font>
```

鼓励在用户态实现功能，甚至在需要时修改内核


> 更新: 2024-12-09 08:41:38  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/bnhk7s3iwfbe8ysi>