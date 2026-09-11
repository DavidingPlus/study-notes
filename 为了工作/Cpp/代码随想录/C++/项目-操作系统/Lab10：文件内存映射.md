# Lab10：文件内存映射

实验难度出自官网，实验简述和实验耗时是主观体验

* mmap  `简述`: 实现内存映射功能（即mmap系统调用）
* `难度`: 1 hard  `耗时`: 6.5h
* `建议`: 这个lab综合了系统调用、内存管理、文件系统、并发编程等各部分内容，算是对是否通过前面的这些lab掌握了这些知识的一个检验，在顺利通关了前面的lab后，这个lab并不难。

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/mmap.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><https://xv6.dgs.zone/labs/requirements/lab10.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://blog.miigon.net/posts/s081-lab10-mmap/>

开始实验前，**<font style="background-color:#FBDE28;">需要切换到本次实验的分支：mmap</font>\*\*\*\*。**

## 实验

<font style="color:rgb(35, 38, 59);">仿照Linux实现mmap功能，即 将文件映射到进程地址空间，如果进程修改了这部分内存，并且内存标记为</font>**<font style="color:rgb(35, 38, 59);">映射内存的修改应写回文件</font>**<font style="color:rgb(35, 38, 59);">，那么释放映射前需要把修改写入源文件。这样与文件交互的时候就可以减少磁盘操作。该实验需要用到很多前面实验的知识点。</font>

<font style="color:rgb(35, 38, 59);"></font>

![1733657477651-5bb18f39-6820-42dd-b084-001f1ccecdce.png](https://cdn.davidingplus.cn/images/2026/09/11/1733657477651-5bb18f39-6820-42dd-b084-001f1ccecdce-777481.png)

进程所使用的内存空间从低地址向高地址生长（`sbrk`调用），范围是stack到trapframe。为了不和进程使用的内存空间冲突，将mmap使用的地址空间映射到trapframe下面的页，从上往下生长。先定义mmap最后一页的地址：

```c
// kernel/memlayout.h
// MMAP 进程映射文件内存最后一个页(开区间)
#define MMAPEND TRAPFRAME
```

<font style="color:rgb(35, 38, 59);">定义一个kama\_vma结构体，表示虚拟内存区域，用来记录mmap创建的虚拟内存地址的范围、长度、权限、文件等。再声明一个kama\_vma结构体的数组，当mmap映射操作时，就来这个数组获取vma虚拟内存区域：</font>

```c
// kernel/proc.h
// 定义一个虚拟内存区域结构体，用来记录mmap创建的虚拟内存地址的范围、长度、权限、文件等
struct kama_vma {
    int valid;              // 该虚拟内存区域是否已被映射
    uint64 vastart;         // 该虚拟内存区域开始地址
    uint64 sz;              // 该虚拟内存区域大小
    struct file* f;         // 该虚拟内存区域映射的文件
    int prot;               // 该虚拟内存区域权限
    int flags;              // 标记映射内存的修改是否写回文件
    uint64 offset;          // 映射文件的起点
};

#define NVMA 16             // VMA数组大小

// Per-process state
struct proc {
  struct spinlock lock;

  ......
  struct kama_vma vmas[NVMA];       // mmap虚拟内存映射地址数组
};

```

<font style="color:rgb(35, 38, 59);">接下来实现mmap的系统调用。参考Linux的mmap函数，需要在进程的vmas数组中遍历寻找空闲的vma，遍历的过程中也要计算当前正在使用的所有vma的最低地址，这是为了后面添加新的vma。找到空闲的vma后，设置他的地址为刚才找到的最低地址减去sz（因为mmap的地址是从高到低生长）。然后需要调用</font>`filedup`<font style="color:rgb(35, 38, 59);">函数将映射文件的引用数+1。</font>

<font style="color:rgb(35, 38, 59);">调用mmap函数的时候需要注意文件权限问题。如果文件不可读，vma映射为可读，则mmap失败；如果文件不可写，vma映射为可写，并且开启了回盘标志（MAP\_SHARED），则mmap失败。据此写出mmap系统调用函数：</font>

```c
// kernel/sysfile.c
#include "memlayout.h"
// mmap系统调用实现
uint64 sys_mmap(void) {
    uint64 addr, sz, offset;
    int prot, flag, fd;
    struct file* f;

    // 读取传入参数
    if (argaddr(0, &addr) < 0 || argaddr(1, &sz) < 0 || argint(2, &prot) < 0 || argint(3, &flag) < 0 || argfd(4, &fd, &f) < 0 || argaddr(5, &offset) < 0 || sz == 0)
        return -1;

    // 以下情况直接返回-1：
    if ((!f->readable && ((prot & (PROT_READ))))                                // 源文件不可读，vma映射为可读
        || (!f->writable && (prot & PROT_WRITE) && !(flag & MAP_PRIVATE)))      // 源文件不可写 ，vam映射为可写并且设置了将修改写回源文件
        return -1;

    sz = PGROUNDUP(sz);
    struct proc* p = myproc();
    struct kama_vma* v = 0;
    uint64 vaend = MMAPEND;

    // 遍历查询未被使用的vma，并且计算当前已使用的va的最低地址
    for (int i = 0;i < NVMA;++i) {
        struct kama_vma* vv = &p->vmas[i];
        if (vv->valid == 0) {           // 若找到了空闲vma就保存下来
            if (v == 0) {
                v = &p->vmas[i];
                v->valid = 1;
            }
        }
        else if (vv->vastart < vaend) {
            vaend = PGROUNDDOWN(vv->vastart);
        }
    }

    // 没找到空闲的vma
    if (v == 0)
        panic("mmap: no free vma");

    // 设置vma属性
    v->vastart = vaend - sz;
    v->sz = sz;
    v->f = f;
    v->prot = prot;
    v->flags = flag;
    v->offset = offset;

    // 增加源文件引用数
    filedup(v->f);

    return v->vastart;
}
```

<font style="color:rgb(35, 38, 59);"> </font>`kernel/fcntl.h`<font style="color:rgb(35, 38, 59);">中定义好了相关宏，编译器提示未定义标识符可以不用管：</font>

```c
// kernel/fcntl.h
#ifdef LAB_MMAP
#define PROT_NONE       0x0
#define PROT_READ       0x1
#define PROT_WRITE      0x2
#define PROT_EXEC       0x4

#define MAP_SHARED      0x01
#define MAP_PRIVATE     0x02
#endif
```

<font style="color:rgb(35, 38, 59);">映射功能使用惰性分配机制实现，即只有在访问的时候才进行磁盘操作。具体原理参考Lab5：</font>

```c
// kernel/sysfile.c
// 通过虚拟地址寻到对应的vma
struct kama_vma* findvma(struct proc* p, uint64 va) {
    for (int i = 0;i < NVMA;++i) {
        struct kama_vma* vv = &p->vmas[i];
        // 如果va地址在某一个vma范围内，则返回这个vma
        if (vv->valid == 1 && va >= vv->vastart && va < vv->vastart + vv->sz) {
            return vv;
        }
    }

    return 0;
}

// 给虚拟地址分配物理页并建立映射
int vmaalloc(uint64 va) {
    struct proc* p = myproc();
    struct kama_vma* v = findvma(p, va);
    if (v == 0)
        return 0;

    // 分配物理地址
    void* pa = kalloc();
    if (pa == 0)
        panic("vmaalloc:kalloc");
    memset(pa, 0, PGSIZE);

    // 从磁盘读取文件
    begin_op();
    ilock(v->f->ip);
    readi(v->f->ip, 0, (uint64)pa, v->offset + PGROUNDDOWN(va - v->vastart), PGSIZE);
    iunlock(v->f->ip);
    end_op();

    // 建立映射
    if (mappages(p->pagetable, va, PGSIZE, (uint64)pa, PTE_R | PTE_W | PTE_U) < 0)
        panic("vmaalloc: mappages");

    return 1;
}
```

```c
// kernel/trap.c
void
usertrap(void)
{
  ......
  else if ((which_dev = devintr()) != 0) {
      // ok
  }
  else if (r_scause() == 13 || r_scause() == 15){
      uint64 va = r_stval();        // 读取当前发生页面错误的地址
      if (vmaalloc(va) == 0)
          panic("usertrap: wrong va");
  }
  ......
}
```

<font style="color:rgb(35, 38, 59);">接下来需要实现另一个系统调用</font>`munmap`<font style="color:rgb(35, 38, 59);">，释放所有的vma。如果设置了MAP\_SHARED，还需要将修改写回磁盘源文件。</font>

`munmap`<font style="color:rgb(35, 38, 59);">传入的参数为释放映射的地址addr，释放地址的范围大小sz。需要检测释放的区域，取消映射的位置要么在区域起始位置，要么在区域结束位置，要么就是整个区域，但是不能在vma中间“打洞”。页有可能不是完整释放，如果 addr 处于一个页的中间，则那个页的后半部分释放，但是前半部分不释放，此时该页整体不应该被释放：</font>

```c
// kernel/sysfile.c
// 释放vma映射的页
uint64 sys_munmap(void) {
    uint64 addr, sz;

    if (argaddr(0, &addr) < 0 || argaddr(1, &sz) < 0 || sz == 0)
        return -1;

    struct proc* p = myproc();
    struct kama_vma* v = findvma(p, addr);
    if (v == 0)
        return -1;

    if (addr > v->vastart && addr + sz < v->vastart + v->sz)        // 释放的区域不能在vma中“打洞”
        return -1;

    uint64 addr_alinged = addr;
    if (addr > v->vastart)
        addr_alinged = PGROUNDUP(addr);

    int nunmap = sz - (addr_alinged - addr);            // 计算要释放的字节数
    if (nunmap < 0)
        nunmap = 0;

    vmaunmap(p->pagetable, addr_alinged, nunmap, v);    // 从addr_alinged开始释放nunmap字节数

    if (addr <= v->vastart && addr + sz > v->vastart) {
        v->offset += addr + sz - v->vastart;
        v->vastart = addr + sz;
    }
    v->sz -= sz;

    if (v->sz <= 0) {
        fileclose(v->f);
        v->valid = 0;
    }

    return 0;
}
```

其中的`vmaunmap`<font style="color:rgb(35, 38, 59);">函数实现释放映射功能。释放映射之后，需要更新对应vma的offset、vastart、sz字段。如果释放完了vma的sz大小的范围，则应该关闭文件的引用，释放该vma。</font>

`vmaunmap`<font style="color:rgb(35, 38, 59);">函数仿照</font>`uvmunmap`<font style="color:rgb(35, 38, 59);">函数实现， 从传入的参数虚拟地址va开始遍历，查找va + nbytes范围内的每一个页，检查这个页是否被修改过，并且该vma设置了回盘MAP\_SHARED，则需要把修改写回磁盘。注意不是每一个页都需要完整的写回，这里需要处理开头页不完整、结尾页不完整以及中间完整页的情况。</font>

<font style="color:rgb(35, 38, 59);">先加上PTE\_D标志位，表示页表被修改过：</font>

```c
// kernel/riscv.h
#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // 1 -> user can access
#define PTE_G (1L << 5) 
#define PTE_A (1L << 6) 
#define PTE_D (1L << 7) // 页表被修改过
```

<font style="color:rgb(35, 38, 59);">实现</font>`vmaunmap`<font style="color:rgb(35, 38, 59);">函数：</font>

```c
// kernel/vm.c
// 添加必要的头文件
#include "fcntl.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "proc.h"

// 释放mmap映射的页，根据PTE_D和MAP_SHARED判断是否将修改写回磁盘
void vmaunmap(pagetable_t pagetable, uint64 va, uint64 nbytes, struct kama_vma* v) {
    uint64 a;
    pte_t* pte;

    for (a = va;a < va + nbytes;a += PGSIZE) {
        if ((pte = walk(pagetable, a, 0)) == 0)     // 读取va对应pte
            continue;

        if (PTE_FLAGS(*pte) == PTE_V)
            panic("sys_munmap: not a leaf");

        if (*pte & PTE_V) {
            uint64 pa = PTE2PA(*pte);
            if ((*pte & PTE_D) && (v->flags & MAP_SHARED)) {        // 将修改写回磁盘
                begin_op();
                ilock(v->f->ip);
                uint64 aoff = a - v->vastart;                       // 相对于vma的偏移量
                if (aoff < 0)
                    writei(v->f->ip, 0, pa + (-aoff), v->offset, PGSIZE + aoff);        // 第一页是不满PGSIZE的一个页
                else if (aoff + PGSIZE > v->sz)
                    writei(v->f->ip, 0, pa, v->offset + aoff, v->sz - aoff);            // 最后一页是不满PGSIZE的一个页
                else
                    writei(v->f->ip, 0, pa, v->offset + aoff, PGSIZE);
                
                iunlock(v->f->ip);
                end_op();
            }
            kfree((void*)pa);
            *pte = 0;
        }
    }
}
```

<font style="color:rgb(35, 38, 59);">在proc.c中需要添加对vma的处理。</font>

<font style="color:rgb(35, 38, 59);">首先是初始化进程时，需要初始化一个进程的vmas数组：</font>

```c
// kernel/proc.c
static struct proc*
allocproc(void)
{
  ......

  // 初始化时清空vmas数组
  for (int i = 0;i < NVMA;++i)
      p->vmas[i].valid = 0;

  return p;
}
```

<font style="color:rgb(35, 38, 59);"> 释放进程时，要在释放页表前清空vmas数组：</font>

```c
// kernel/proc.c
static void
freeproc(struct proc *p)
{
  if(p->trapframe)
    kfree((void*)p->trapframe);
  p->trapframe = 0;
  for (int i = 0;i < NVMA;++i) {                        // 释放页表前把vmas数组清空
      struct kama_vma* v = &p->vmas[i];
      vmaunmap(p->pagetable, v->vastart, v->sz, v);
  }
  if (p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  ......
}
```

<font style="color:rgb(35, 38, 59);">fork创建子进程时，子进程复制父进程的vmas数组，不复制物理页：</font>

```c
// kernel/proc.c
int
fork(void)
{
  ......

  // 父进程vmas复制到子进程中，实际内存页和pte不会被复制
  for (int i = 0;i < NVMA;++i) {
      struct kama_vma* v = &p->vmas[i];
      if (v->valid) {
          np->vmas[i] = *v;
          filedup(v->f);
      }
  }

  safestrcpy(np->name, p->name, sizeof(p->name));

  pid = np->pid;

  np->state = RUNNABLE;

  release(&np->lock);

  return pid;
}
```

添加`mmap`和`munmap`两个系统调用的步骤不再赘述。

之后可以即可验证实验是否通过。

## 可扩展的实验

* 如果两个进程映射了相同的文件（如 `fork_test` 中），让它们共享物理页面。你需要为物理页面添加引用计数机制。
* 你的解决方案可能会给  从映射的文件中读取的每一页  分配一个新的物理页面，即使这些数据已经存在于内核缓冲区缓存中：修改你的实现以使用内核缓冲区缓存中的物理内存，而不是分配新页面。这需要文件块的大小与页面大小相同（设置 `BSIZE` 为 4096）。你需要将映射的文件块固定到缓冲区缓存中，并管理好引用计数。
* 消除惰性分配实现和文件映射实现之间的冗余。提示：为惰性分配区域创建一个虚拟内存区域（VMA，Virtual Memory Area）。
* 修改 `exec`，为 可执行文件的不同部分 创建 VMA，这样可以实现按需分页的可执行文件。这将加快程序启动速度，因为 `exec` 不需要立即从文件系统中读取任何数据。
* 实现页面换出和换入：让内核在物理内存不足时将某些进程部分移动到磁盘。当进程访问换出的内存时，再将其换入。


> 更新: 2024-12-08 20:44:05  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/ik87k673yh4z893m>