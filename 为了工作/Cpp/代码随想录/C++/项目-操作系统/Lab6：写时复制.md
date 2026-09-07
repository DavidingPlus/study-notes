# Lab6：写时复制

实验难度出自官网，实验简述和实验耗时是主观体验

* cow  `简述`：利用缺页故障实现fork系统调用的写时复制机制
* `难度`：1 hard  `耗时`：2h
* `建议`：本lab和lazy非常相似，完成lazy后，cow就不太费时间了。可以总结一下fork写时复制的优缺点

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/cow.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><http://xv6.dgs.zone/labs/requirements/lab6.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://blog.miigon.net/posts/s081-lab6-copy-on-write-fork/>

开始实验前，**<font style="background-color:#FBDE28;">需要切换到本次实验的分支：cow</font>\*\*\*\*。**

## 知识点

<font style="color:rgb(35, 38, 59);">本次实验还是利用缺页故障实现一个有趣的机制：写时复制（Copy-on-Write，COW）。</font>

<font style="color:rgb(35, 38, 59);">在原始的XV6中，fork函数是通过直接对进程的地址空间完整地复制一份来实现的。但是，拷贝整个地址空间是十分耗时的，并且在很多情况下，程序</font>立即调用`exec`函数<font style="color:rgb(35, 38, 59);">来替换掉地址</font>空间，导致`fork`做了很<font style="color:rgb(35, 38, 59);">多无用功。</font>

<font style="color:rgb(35, 38, 59);">该实验的改进：COW fork()只为子进程创建一个页表，用户内存的PTE指向父进程的物理页。COW fork()将父进程和子进程中的所有用户PTE标记为不可写。当任一进程试图写入其中一个COW页时，CPU将强制产生页面错误。内核页面错误处理程序检测到这种情况，将为出错进程分配一页物理内存，将原始页复制到新页中，并修改出错进程中的相关PTE指向新的页面，将PTE标记为可写。当页面错误处理程序返回时，用户进程将能够写入其页面副本。</font>

<font style="color:rgb(35, 38, 59);">COW fork()将使得释放用户内存的物理页面变得更加棘手。给定的物理页可能会被多个进程的页表引用，并且只有在最后一个引用消失时才应该被释放，所以针对这个问题需要有引用计数机制。</font>

## 实验

## <font style="color:rgb(51, 51, 51);">Implement copy-on write (hard)</font>

首先给页表添加一个新的标志位，代表这个页是否是写时复制页。其他的标志位就是`PTE_W`、`PTE_R`这些，这里新的标志位就叫`PTE_COW`，以下就把写时复制页称为COW页：

```c
// kernel/riscv.h
#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // 1 -> user can access
#define PTE_COW (1L << 8) 
// 是否为COW页，使用页表项 flags 中保留的第 8 位表示
//（页表项 flags 中，第 8、9、10 位均为保留给操作系统使用的位，可以用作任意自定义用途）
```

父进程使用`fork`创建子进程时，会调用`uvmcopy`，把父进程映射的物理页全都复制一份新的给子进程。写时复制是写的时候才复制，所以这里去掉复制的功能，只给对应的页添加上`PTE_COW`标志位。这里原本父进程中只读的页，不做修改，也就是说，只读的页在父进程和子进程中一直都会是共享的。只有原本是可写的页做修改：

```c
// kernel/vm.c
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
//   char *mem;			//用不到了，去掉，避免编译报错

  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walk(old, i, 0)) == 0)
      panic("uvmcopy: pte should exist");
    if((*pte & PTE_V) == 0)
      panic("uvmcopy: page not present");
    pa = PTE2PA(*pte);

    // 清除父进程中所有PTE的PTE_W位，并且设置PTE_COW位，表示所在页是一个写时复制页（多个进程引用同个物理页）
    // 如果该页本身就是不可写（只读），则不会添加这个标志位
    if (*pte & PTE_W)
        *pte = (*pte & ~PTE_W) | PTE_COW;

    flags = PTE_FLAGS(*pte);        //获取当前父进程pte的标志位

    //将父进程映射的物理页直接map到子进程中，权限保持和父进程一致（注意现在都是不可写，而且原本是可写的页会有新增的PTE_COW写时复制标志）
    if (mappages(new, i, PGSIZE, (uint64)pa, flags) != 0)
        goto err;

    kama_krefpage((void*)pa);         //映射的物理页的引用数+1

    // 去掉以下复制内存的代码
    // if((mem = kalloc()) == 0)
    //   goto err;
    // memmove(mem, (char*)pa, PGSIZE);
    // if(mappages(new, i, PGSIZE, (uint64)mem, flags) != 0){
    //   kfree(mem);
    //   goto err;
    // }
  }
  return 0;

 err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  return -1;
}
```

`kama_krefpage`函数是使物理页引用数+1。为了避免内存泄漏，多个进程映射同一个物理页的时候，记录这个物理页的引用数，调用`kfree`释放物理页的时候，只对引用数-1，当引用数为0的时候，才释放物理页。这里先有个概念，`krefpage`函数在后面实现。

现在使用`fork`函数不会复制内存了，父进程和子进程都映射到同一片物理内存，且所有的页都是不可写的状态。这个时候某一个进程尝试执行写操作，就会出现页面错误被`usertrap`捕获。接下来修改`usertrap`函数，实现在修改页的时候，才创建和映射新的物理页。

和上一个实验惰性分配类似，在`usertrap`中添加对页面错误的检测，如果当前访问的地址符合COW页的条件时，就进程复制内存操作：

```c
// kernel/trap.c
void
usertrap(void)
{
  ......
    
  } else if((which_dev = devintr()) != 0){
    // ok
  }else if ((r_scause() == 13 || r_scause() == 15) && kama_uvmcheckcowpage(r_stval())) {
      // 发生页面错误，并且检测出错误是写时复制机制导致的页面不可写，则执行写时复制
      if (kama_uvmcowcopy(r_stval()) == -1)
          p->killed = 1;
  }else {
      printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    p->killed = 1;
  }

  ......
}
```

调用`r_scause`判断是否是页面错误，实现一个`kama_uvmcheckcowpage`函数判断是否是COW机制导致的页面错误，符合这些条件，再实现一个`kama_uvmcowcopy`函数执行写时复制操作。

实现`kama_uvmcheckcowpage`函数，检查这个地址是否在COW页：

```c
// kernel/vm.c
//检查虚拟地址所在页是否是COW页
int kama_uvmcheckcowpage(uint64 va) {
    pte_t* pte;
    struct proc* p = myproc();

    return va < p->sz
        && ((pte = walk(p->pagetable, va, 0)) != 0)
        && (*pte & PTE_V)
        && (*pte & PTE_COW);
    //地址在进程内存范围内  &&  地址有映射  && 地址有效且是COW页
}
```

实现`kama_uvmcowcopy`函数，复制一个新的物理页，创建新的映射。上面已经检查确认这个地址是COW页了，所以这里可以放心的恢复写权限，并清除掉PTE\_COW标志：

```c
// kernel/vm.c
// 实现写时复制
int kama_uvmcowcopy(uint64 va) {
    pte_t* pte;
    struct proc* p = myproc();

    if ((pte = walk(p->pagetable, va, 0)) == 0)     //获取虚拟地址的页表项
        panic("uvmcowcopy: walk");

    uint64 pa = PTE2PA(*pte);                     //获取映射的物理地址
    uint64 new = (uint64)kama_kcopy_n_deref((void*)pa);  //获取新分配的物理页（如果原本的物理页引用数为1，则获取到的还是原本的物理页）
    if (new == 0)       //内存不足的情况
        return -1;

    //修改新的映射，恢复写权限，清除COW标志
    uint64 flags = (PTE_FLAGS(*pte) | PTE_W) & ~PTE_COW;
    uvmunmap(p->pagetable, PGROUNDDOWN(va), 1, 0);              //清除旧的映射
    if (mappages(p->pagetable, va, PGSIZE, new, flags) == -1)        //新的映射
        panic("uvmcowcopy: mappages");

    return 0;
}
```

`kama_kcopy_n_deref`函数的思路是，当有进程通过COW复制新的物理页时，把原本物理页的引用数-1，返回新创建的物理页地址。如果原本的物理页引用数就是1了，那可以直接使用这个物理页了，因为也没有其他进程会使用这个物理页了。`kama_kcopy_n_deref`函数后面实现。

`copyout`函数是软件访问页表，不会触发缺页异常，这里添加上检测代码，检测复制的页是否是一个COW页，是的话就执行复制操作：

```c
// kernel/vm.c
int
copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
{
    uint64 n, va0, pa0;

    while (len > 0) {

        if (kama_uvmcheckcowpage(dstva))     //检查每一个页是否为COW页
            kama_uvmcowcopy(dstva);

        va0 = PGROUNDDOWN(dstva);
        pa0 = walkaddr(pagetable, va0);
        ......
}
```

到此，已实现了大致的逻辑：调用`fork`的时候不复制内存，只是把子进程的映射和父进程保持一致，并把可写的页变为不可写，添加`PTE_COW`标志位。某一进程进行写操作时产生页面错误，此时才调用相关函数实现复制内存，并把新的页面恢复写权限。

<font style="color:rgb(35, 38, 59);">接下来需要对页的生命周期进行管理，确保页没有被任何进程使用的时候才释放</font>

原本的xv6中，一个物理页只会在一个进程中有映射，`kalloc`用来分配物理页，`kfree`用来回收物理页。在COW机制中，一个物理页会在多个进程中有映射，所以要在最后一个映射释放的时候，才真正释放回收该物理页，结合这些，需要实现以下操作：

* `kalloc`: 分配物理页，将其引用数置为 1
* `kama_krefpage`: 创建物理页的一个新映射的时候，引用数+1
* `kama_kcopy_n_deref`: 将原物理页的数据复制到一个新物理页上（引用数为 1），返回得到的新物理页；并将原物理页的引用数-1
* `kfree`: 释放物理页的一个映射，引用数减 1；如果引用数变为 0，则释放回收物理页

一个物理页首先会被父进程使用 `kalloc`创建，fork 的时候，新创建的子进程的时候会调用`kama_krefpage` 声明对应物理页的引用数+1。当尝试修改父进程或子进程中的页时，`kama_kcopy_n_deref` 负责将想要修改的页复制到独立的副本，并记录解除旧的物理页的映射（引用数-1）。最后 `kfree` 保证只有在所有的引用者都释放该物理页的映射时，才释放回收该物理页

在`kallo.c`定义一些相关的宏：

```c
// kernel/kalloc.c
#define PA2PGREF_ID(p) (((p)-KERNBASE)/PGSIZE)      // 由物理地址获取物理页id
#define PGREF_MAX_ENTRIES PA2PGREF_ID(PHYSTOP)      // 物理页数上限

int pageref[PGREF_MAX_ENTRIES];         //每个物理页的引用数 数组（pageref[i]表示第i个物理页的引用数）
struct spinlock pgreflock;              //用于pageref数组的锁，防止竞态条件引起内存泄漏

#define PA2PGREF(p) pageref[PA2PGREF_ID((uint64)(p))]       //获取地址对应物理页引用数
```

<font style="color:rgb(35, 38, 59);">初始化锁：</font>

```c
// kernel/kalloc.c
void
kinit()
{
    initlock(&kmem.lock, "kmem");
    initlock(&pgreflock, "pgref");      //初始化锁
    freerange(end, (void*)PHYSTOP);
}
```

修改`kalloc`：

```c
// kernel/kalloc.c
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if (r) {
    memset((char*)r, 5, PGSIZE); // fill with junk
    PA2PGREF(r) = 1;        //将新分配的物理页的引用数设置为1（刚分配的页还没映射，不会有进程来使用，不用加锁）
  }
  return (void*)r;
}
```

修改`kfree`：

```c
// kernel/kalloc.c
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  //当页面的引用数<=0的时候释放页面
  acquire(&pgreflock);
  if (--PA2PGREF(pa) <= 0) {
    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
  release(&pgreflock);
}
```

实现`kama_krefpage`：

```c
// kernel/kalloc.c
//物理页引用数+1
void kama_krefpage(void* pa) {
    acquire(&pgreflock);
    PA2PGREF(pa)++;
    release(&pgreflock);
}
```

实现`kama_kcopy_n_deref`：

```c
// kernel/kalloc.c
// 写时复制一个新的物理地址返回
// 如果该物理页的引用数>1，则将引用数-1，并分配一个新的物理页返回
// 如果引用数 <=1，则无需操作直接返回该物理页
void* kama_kcopy_n_deref(void* pa) {
    acquire(&pgreflock);

    //当前物理页的引用数为1，无需分配新的物理页
    if (PA2PGREF(pa) <= 1) {
        release(&pgreflock);
        return pa;
    }

    //分配新的物理页，并把旧页中的数据复制到新页
    uint64 newpa = (uint64)kalloc();
    if (newpa == 0) {           //内存不够了
        release(&pgreflock);
        return 0;
    }
    memmove((void*)newpa, (void*)pa, PGSIZE);

    //旧页引用数-1
    PA2PGREF(pa)--;

    release(&pgreflock);
    return (void*)newpa;
}
```

这里定义了一个自旋锁`pgreflock`，通过`acquire(&pgreflock)`和`release(&pgreflock)`获取锁和释放锁来保护操作的代码，锁的作用是防止竞态条件下导致的内存泄漏。

> 不加锁的例子：
>
> 比如，父进程的一个物理页p，此时引用数是1。
>
> 父进程执行`fork`创建子进程，此时p的引用数是2。
>
> 父进程执行写操作，触发页面异常。因为p的引用数大于1，开始执行复制p的操作（p的引用数还是2）。
>
> 进程调度，切换到子进程（父进程复制操作中断）。子进程马上执行`exec`，释放所有旧的物理页，物理页引用数-1（p的引用数为1）。
>
> 进程调度，切换到父进程。父进程继续复制p的数据到新的物理页。复制完毕，p的引用数-1。之后回到`kama_uvmcowcopy`函数中，通过`uvmunmap`解除了对p的映射。物理页p并没有被释放回收，所有进程都丢失了对p的映射（页表中均没有指向 p 的页表项），造成内存泄漏

加上锁之后，可以防止这个现象发生。在使用`kalloc`分配内存时可以不用加锁，因为这个地址还没返回，没有哪个进程可以操作这个新分配的地址。

<font style="color:rgb(35, 38, 59);">至此可以执行验证实验是否完成。</font>

## 可扩展的实验

* <font style="color:rgb(51, 51, 51);">修改xv6以同时支持lazy allocation和COW。</font>
* <font style="color:rgb(51, 51, 51);">测量COW的实现减少了多少xv6拷贝的字节数以及分配的物理页数。寻找并利用机会进一步减少这些数字。</font>


> 更新: 2025-11-23 16:15:31  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/dh11xxmoa82i35t7>