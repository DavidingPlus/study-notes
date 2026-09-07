# Lab2：系统调用

实验难度出自官网，实验简述和实验耗时是主观体验

* syscall  `简述`：添加新的系统调用
* `难度`：2 moderate  `耗时`：5h
* `建议`：这个lab不难，和后面的traps lab有一定关联

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/syscall.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><http://xv6.dgs.zone/labs/answers/lab2.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://blog.miigon.net/posts/s081-lab1-unix-utilities/>

开始实验前，\*\*<font style="background-color:#FBDE28;">需要切换到本次实验的分支：syscall</font>\*\*\*\*。\*\*如果忘了怎么切换分支，请去上一篇文章开头查看

## 知识点

该实验主要是用来熟悉系统调用的流程。我们需要了解系统调用的步骤，然后为这个xv6系统增加新的系统调用函数。

在操作系统中，系统调用是用户程序与内核交互的关键机制。用户程序无法直接操作硬件，也不能直接访问内核提供的资源。系统调用充当桥梁，允许用户程序通过受控的接口向内核请求服务，这样可以保持系统的安全性和稳定性 。常见的系统调用函数有printf、open、read等

操作系统通过 CPU 的特权级模式 将用户态与内核态隔离，保证了用户程序无法直接访问内核资源，避免了误操作和恶意行为。 用户程序通过调用库函数间接发起系统调用，系统调用触发软件中断或陷阱，引发CPU切换到内核态，内核根据系统调用号确定要执行的服务，完成服务后，将结果或错误码存入用户程序的寄存器，并返回到用户态继续执行程序。<font style="color:rgb(35, 38, 59);">这么繁琐的调用流程的主要目的是实现用户态和内核态的良好隔离。</font>该流程可参考下图：

![1733301853107-1d401da8-c301-4fd0-be3b-4ca9f9f813b1.png](./img/JkAv4Qk3hQHCXEvf/1733301853107-1d401da8-c301-4fd0-be3b-4ca9f9f813b1-819391.png)

目前可以先留个印象，在后面的“Lab4：中断处理”章节还会详细讲解。

添加新的系统调用涉及到的文件如下（仅包括添加系统调用，不包括系统调用的实现）：

```plain
user/user.h:		用户态程序调用跳板函数

user/usys.pl:		跳板函数使用 CPU 提供的 ecall 指令，切换到内核态

kernel/syscall.c：	内核态下调用系统调用处理函数 syscall()，所有系统调用都会跳到这里来处理

kernel/syscall.h：	设定系统调用号，syscall()根据跳板传进来的系统调用编号，查询 syscalls[] 表，
                    找到对应的内核函数并调用
```

## 实验

### <font style="color:rgb(35, 38, 59);">System call tracing（moderate）</font>

这个实验要求实现trace功能，跟踪用户程序中调用的系统调用，并将调用的相关信息打印出来。目前仓库中已经写好了用户态的trace函数（请先去阅读代码：`user/trace.c`)，但是实际的trace系统调用还没有实现。

从题目给的例子来说明这个实验：

```bash
$ trace 32 grep hello README
3: syscall read -> 1023
3: syscall read -> 966
3: syscall read -> 70
3: syscall read -> 0
```

trace 32表示启用了跟踪功能，32表示跟踪 系统调用号掩码为5的系统调用（1<<5=32，这需要一点位运算知识），在`kernel/syscall.h`中，可以看到`#define SYS_read    5`，所以这里表示要跟踪read系统调用（这部分不理解的话，可以看完后面实现的代码再来理解）。然后执行`grep hello README`命令，

即，用 `grep` 搜索 `README` 文件中的 `hello`。

执行`trace 32 grep hello README`命令后，需要实现的效果是：在打印输出中，3表示 运行 `grep` 的进程 ID ，`syscall read`表示发生了read系统调用，`->1023`，表示调用read的返回值，这里表示 读取了 1023 字节，后面的输出也是类似。

```bash
$ trace 2147483647 grep hello README
4: syscall trace -> 0
4: syscall exec -> 3
4: syscall open -> 3
4: syscall read -> 1023
4: syscall read -> 966
4: syscall read -> 70
4: syscall read -> 0
4: syscall close -> 0
```

同理，在这个例子中，`2147483647` 的二进制中所有低 31 位都为 1，表示跟踪所有系统调用。因此在输出中可以看到，执行`grep hello README`命令涉及到的所有系统调用都会被打印出来。

接下来开始修改代码实现这个功能。

***

按照系统调用的执行顺序，先从用户空间开始修改代码。

首先在<font style="color:rgb(35, 38, 59);">用户空间的头文件</font><code><font style="color:rgb(35, 38, 59);">user/user.h</font></code><font style="color:rgb(35, 38, 59);">加入函数声明，让用户态程序可以找到这个跳板入口函数：</font>

```c
// user/user.h
// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);

......
    
int sleep(int);
int uptime(void);
int trace(int);         // 用户态程序可以找到trace系统调用的跳板入口函数
```

<font style="color:rgb(35, 38, 59);">在</font><code><font style="color:rgb(35, 38, 59);">user/usys.pl</font></code><font style="color:rgb(35, 38, 59);">中，加入用户态到内核态的跳板函数:</font>

```perl
# user/usys.pl 
......
entry("sbrk");
entry("sleep");
entry("uptime");
entry("trace");		# 为trace添加系统调用接口
```

<font style="color:rgb(35, 38, 59);">该文件是一个 Perl 脚本，用于生成 </font><code><font style="color:rgb(35, 38, 59);">usys.S</font></code><font style="color:rgb(35, 38, 59);"> 文件，包含用户态程序调用内核态系统调用的接口。</font>

<font style="color:rgb(35, 38, 59);">因为</font><code><font style="color:rgb(35, 38, 59);">user/trace.c</font></code><font style="color:rgb(35, 38, 59);">实现的是一个用户空间的函数（性质和上一个实验实现的sleep、pingpong一样），所以需要在Makefile中添加</font><code><font style="color:rgb(35, 38, 59);">$U/_trace\</font></code><font style="color:rgb(35, 38, 59);">：</font>

```makefile
UPROGS=\
......
	$U/_zombie\
	$U/_trace\
```

<font style="color:rgb(35, 38, 59);">以上是用户态的修改，接下来是内核态。</font>

<font style="color:rgb(35, 38, 59);">首先在</font><code><font style="color:rgb(35, 38, 59);">kernel/syscall.h</font></code><font style="color:rgb(35, 38, 59);">中添加trace的系统调用号（掩码）：</font>

```c
// kernel/syscall.h
// System call numbers
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3

......
    
#define SYS_mkdir  20
#define SYS_close  21
#define SYS_trace  22       // trace系统调用号
```

<font style="color:rgb(35, 38, 59);">在</font><code><font style="color:rgb(35, 38, 59);">kernel/syscall.c</font></code><font style="color:rgb(35, 38, 59);">中全局声明trace系统调用处理函数，并且把系统调用号与处理函数关联：</font>

```c
// kernel/syscall.c
extern uint64 sys_chdir(void);
extern uint64 sys_close(void);
extern uint64 sys_dup(void);
extern uint64 sys_exec(void);
extern uint64 sys_exit(void);
extern uint64 sys_fork(void);
extern uint64 sys_fstat(void);
extern uint64 sys_getpid(void);
extern uint64 sys_kill(void);
extern uint64 sys_link(void);
extern uint64 sys_mkdir(void);
extern uint64 sys_mknod(void);
extern uint64 sys_open(void);
extern uint64 sys_pipe(void);
extern uint64 sys_read(void);
extern uint64 sys_sbrk(void);
extern uint64 sys_sleep(void);
extern uint64 sys_unlink(void);
extern uint64 sys_wait(void);
extern uint64 sys_write(void);
extern uint64 sys_uptime(void);
extern uint64 sys_trace(void);      // 全局声明trace系统调用处理函数

static uint64 (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_trace]   sys_trace,        // 系统调用号与处理函数关联
};
```

<font style="color:rgb(35, 38, 59);">在 </font><code><font style="color:rgb(35, 38, 59);">kernel/proc.h</font></code><font style="color:rgb(35, 38, 59);"> 里，在进程类proc 结构中，添加 kama\_syscall\_trace属性，用掩码的方式记录要跟踪的系统调用:</font>

```c
// kernel/proc.h
// Per-process state
struct proc {
  struct spinlock lock;

  ......

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  struct trapframe *trapframe; // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  uint64 kama_syscall_trace;   // 存储进程的系统调用跟踪掩码,用于记录哪些系统调用需要被跟踪
};
```

这里新添加了进程的属性，所以在一些相关的地方也要做些修改。首先是新进程的初始化中，对这个属性设置默认值0（否则初始状态下会是随机数值），在<code>kernel/<font style="color:rgb(35, 38, 59);">proc.c</font></code>中：

```c
// kernel/proc.c
static struct proc*
allocproc(void)
{
  ......

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  p->kama_syscall_trace = 0;         //创建新进程的时候，kama_syscall_trace 设置为默认值0

  return p;
}
```

在fork函数中，子进程能继承父进程的这个属性：

```c
int
fork(void)
{
  ......

  // increment reference counts on open file descriptors.
  for(i = 0; i < NOFILE; i++)
    if(p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, sizeof(p->name));

  pid = np->pid;

  np->state = RUNNABLE;

  release(&np->lock);

  np->kama_syscall_trace = p->kama_syscall_trace;      //子进程继承父进程的syscall_trace

  return pid;
}
```

因为是在一个进程中跟踪系统调用，所以此处在`kernel/sysproc.c`中实现这个系统调用（其实在哪里实现都可以，因为上面已经用extern声明了，但是一般都会在相关的模块中实现）。这里实现的是  为进程的kama\_syscall\_trace属性  赋值  用户程序传进来的要跟踪的系统调用号：

```c
// kernel/sysproc.c
// 当前进程的系统调用跟踪掩码
uint64
sys_trace(void)
{
    int mask;

    if(argint(0, &mask) < 0)                // 获取用户程序传入的数据
        return -1;

    myproc()->kama_syscall_trace = mask;    // 设置调用进程的kama_syscall_trace掩码mask
    return 0;
}
```

因为是跟踪系统调用，所以可以在触发系统调用后就按题设要求打印数据。所有的系统调用都会在`kernel/syscall.c`中的`syscall()`函数中进行处理，因此在此处打印跟踪信息：

```c
// kernel/syscall.c
void
syscall(void)
{
  int num;
  struct proc *p = myproc();

  // 获取系统调用号
  num = p->trapframe->a7;
  // 如果系统调用编号有效（大于 0 且小于 syscalls 数组的长度，并且对应的处理函数存在）
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
      // 调用对应的处理函数，并将返回值存储在 a0 寄存器中
      p->trapframe->a0 = syscalls[num]();                           

      // 如果当前进程启用了trace跟踪，则按照题设要求打印信息
      if ((p->kama_syscall_trace >> num) & 1) {				
          printf("%d: syscall %s -> %d\n",p->pid, kama_syscall_names[num], p->trapframe->a0); 
      }
  }
  else {
    printf("%d %s: unknown sys call %d\n",
            p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}
```

代码中关于trapframe的点先按注释的字面意思来理解，后面Lab4会详细解释。

此处将进程的kama\_syscall\_trace属性值右移num位，然后和1做与运算，以此来判断当前进程是否跟踪系统调用号为num的系统调用（这里需要一点位运算的知识）。为了方便打印，这里还定义了一个kama\_syscall\_names<font style="color:rgb(35, 38, 59);">字符串数组映射</font>，用来快速 获取系统调用号 对应的 系统调用函数名：

```c
// kernel/syscall.c
// 定义系统调用名称的字符串数组
const char* kama_syscall_names[] = {
[SYS_fork]    "fork",
[SYS_exit]    "exit",
[SYS_wait]    "wait",
[SYS_pipe]    "pipe",
[SYS_read]    "read",
[SYS_kill]    "kill",
[SYS_exec]    "exec",
[SYS_fstat]   "fstat",
[SYS_chdir]   "chdir",
[SYS_dup]     "dup",
[SYS_getpid]  "getpid",
[SYS_sbrk]    "sbrk",
[SYS_sleep]   "sleep",
[SYS_uptime]  "uptime",
[SYS_open]    "open",
[SYS_write]   "write",
[SYS_mknod]   "mknod",
[SYS_unlink]  "unlink",
[SYS_link]    "link",
[SYS_mkdir]   "mkdir",
[SYS_close]   "close",
[SYS_trace]   "trace",
};

```

至此，可以验证该实验是否通过（验证打分的方法见上一个实验）

可以把这个实验当成添加系统调用的模板，后面的实验中还时不时要添加新的系统调用，如果忘了怎么添加就回来看一看这个实验。

### <font style="color:rgb(35, 38, 59);">Sysinfo (moderate)</font>

该实验需要添加一个系统调用`sysinfo`，用来获取空闲的内存、已创建的进程数量。

在`kernel/sysinfo.h`中已经声明好了`freemem`空闲内存属性和`nproc`进程数量属性，我们需要把获取的数据赋值在其中。相关的知识点在代码中描述。

在用户空间相关文件中添加代码：

```c
// user/user.h
struct sysinfo;         		   // 声明sysinfo结构体，使用户程序可以使用这个结构体
int sysinfo(struct sysinfo*);      // 用户态程序可以找到sysinfo系统调用的跳板入口函数

// user/usys.pl
entry("sysinfo");

// Makefile
UPROGS=\
	......
	$U/_zombie\
	$U/_trace\
	$U/_sysinfotest\
```

在内核态添加sysinfo系统调用号：

```c
// kernel/syscall.h
// System call numbers
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3

......
    
#define SYS_close  21
#define SYS_trace  22       // trace系统调用号
#define SYS_sysinfo  23     // sysinfo系统调用号
```

添加系统调用映射：

```c
// kernel/syscall.c
extern uint64 sys_sysinfo(void);    //全局声明sysinfo系统调用处理函数

......

static uint64 (*syscalls[])(void) = {
......
[SYS_sysinfo]   sys_sysinfo,
};
```

接下来实现sysinfo函数。这里先宏观的实现sysinfo功能，然后再分解实现获取空闲内存和获取进程数的功能。

这里同样在`kernel/sysproc.c`中实现sysinfo。需要注意的是，获取到信息后的sysinfo结构体是在内核空间的，需要把他拷贝到用户空间进程的内存中：

```c
// kernel/sysproc.c
// 文件开头加上sysinfo结构体的头文件
#include "sysinfo.h"

// 收集系统信息
uint64
sys_sysinfo(void) {
    struct sysinfo info;
    kama_freebytes(&info.freemem);	// 获取空闲内存
    kama_procnum(&info.nproc);		// 获取进程数量

    //获取用户虚拟地址
    uint64 dstaddr;
    argaddr(0, &dstaddr);

    //从内核空间拷贝数据到用户空间
    if (copyout(myproc()->pagetable, dstaddr, (char*)&info, sizeof info) < 0)
        return -1;

    return 0;
}
```

在`user/sysinfotest.c`中可以看到，用户程序调用sysinfo系统调用的时候，传入的参数是sysinfo结构体。因此在上面的代码中，获取到用户空间的sysinfo结构体地址后，要把内核空间的sysinfo结构体的数据复制过去，这样用户程序才能拿到数据。

接下来实现获取空闲内存的kama\_freebytes函数。因为和内存相关，这个函数就在内存相关的`kernel/kalloc.c`中实现。 xv6 采用了一种简单的空闲链表机制来记录空闲的物理内存页，空闲页自身作为链表节点，指向下一个空闲页。因此只要遍历这个链表就能计算空闲内存：

```c
// kernel/kalloc.c
// 获取空闲内存
void kama_freebytes(uint64* dst) {
    *dst = 0;
    struct run* p = kmem.freelist;

    acquire(&kmem.lock);		// 加锁保证线程安全
    while (p) {
        *dst += PGSIZE;			// 统计空闲字节数
        p = p->next;
    }
    release(&kmem.lock);
}
```

页表的知识在后面的实验中还会经常介绍。

接下来实现统计进程数量的函数kama\_procnum。该函数在进程相关的`kernel/proc.c`中实现。在这个文件中有一个进程表`struct proc proc[NPROC]`，记录了所有的进程，而每一个进程都有一个state属性，表示该进程是否在使用。因此可以得出思路：遍历proc进程表，判断当前进程是否在使用，是的话数量+1。代码：

```c
// kernel/proc.c
// 统计处于活动状态的进程
void
kama_procnum(uint64* dst) {
    *dst = 0;
    struct proc* p;
    for (p = proc;p < &proc[NPROC];p++) {
        if (p->state != UNUSED)
            (*dst)++;
    }
}
```

因为这两个函数不像系统调用那样使用了extern全局声明，`kernel/sysproc.c`中的sys\_sysinfo是找不到这两个函数的。这里要在内核头文件`kernel/defs.h`声明这两个函数：

```c
// kernel/defs.h
// kalloc.c
void*           kalloc(void);
void            kfree(void *);
void            kinit(void);
void            kama_freebytes(uint64* dst);     //获取空闲内存

......

// proc.c
......
void            procdump(void);
void            kama_procnum(uint64* dst);       //统计处于活动状态的进程
```

至此可验证该实验是否通过

到此实验2完成。

## 可扩展的实验

* <font style="color:rgb(51, 51, 51);">打印所跟踪的系统调用的参数（easy）</font>
* <font style="color:rgb(51, 51, 51);">计算平均负载并通过</font><code><font style="background-color:rgb(247, 247, 247);">sysinfo</font></code><font style="color:rgb(51, 51, 51);">导出（moderate）</font>


> 更新: 2024-12-05 19:46:26  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/izig2n7zvers6ygt>