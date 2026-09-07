# Lab5：惰性分配

实验难度出自官网，实验简述和实验耗时是主观体验

* lazy  `简述`：利用缺页故障实现内存分配的懒分配策略
* `难度`：1 easy   2 moderate     `耗时`：8h
* `建议`：可以总结一下使用懒分配策略的优缺点

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/lazy.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><http://xv6.dgs.zone/labs/requirements/lab5.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://www.cnblogs.com/weijunji/p/14338483.html>

开始实验前，**<font style="background-color:#FBDE28;">需要切换到本次实验的分支：lazy</font>\*\*\*\*。**

## 知识点

### <font style="color:rgb(35, 38, 59);">页面错误异常</font>

<font style="color:rgb(35, 38, 59);">Risc-v有三种不同的页面错误：</font>

**<font style="color:rgb(35, 38, 59);">加载页面错误</font>**<font style="color:rgb(35, 38, 59);">：当加载指令访问的虚拟地址找不到对应的物理地址时触发。</font>

**<font style="color:rgb(35, 38, 59);">存储页面错误</font>**<font style="color:rgb(35, 38, 59);">：当存储指令访问的虚拟地址找不到对应的物理地址时触发。</font>

**<font style="color:rgb(35, 38, 59);">指令页面错误</font>**<font style="color:rgb(35, 38, 59);">：当指令获取的虚拟地址找不到对应的物理地址时触发。</font>

<font style="color:rgb(35, 38, 59);">这些页面错误信息保存在 RISC-V 的两个寄存器中：</font><code><font style="color:rgb(35, 38, 59);">scause</font></code><font style="color:rgb(35, 38, 59);">：指示页面错误的类型（加载、存储或指令）；</font><code><font style="color:rgb(35, 38, 59);">stval</font></code><font style="color:rgb(35, 38, 59);">：保存无法转换的虚拟地址。</font>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">page fault（页面错误）可以让地址映射关系变得动态起来。通过page fault，内核可以更新page table。当发生page fault时，内核需要什么样的信息才能够响应page fault：</font>

* **<font style="color:rgb(35, 38, 59);">需要出错的虚拟地址</font>**<font style="color:rgb(35, 38, 59);">，或者是触发page fault的源。当出现page fault的时候，XV6内核会打印出错的虚拟地址，并且这个地址会被保存在STVAL寄存器中。所以，当一个用户应用程序触发了page fault，page fault会使用trap机制，将程序运行切换到内核，同时也会将出错的地址存放在STVAL寄存器中</font>
* **<font style="color:rgb(35, 38, 59);">出错的原因</font>**<font style="color:rgb(35, 38, 59);">，需要对不同场景的page fault有不同的响应。不同的场景是指，比如因为load指令触发的page fault、因为store指令触发的page fault又或者是因为jump指令触发的page fault。RISC-V文档在SCAUSE（Supervisor cause寄存器，保存了trap机制中进入到supervisor mode的原因）寄存器的介绍中，有多个与page fault相关的原因。比如，</font>**<font style="color:rgb(35, 38, 59);">13表示是因为load引起的page fault</font>**<font style="color:rgb(35, 38, 59);">；</font>**<font style="color:rgb(35, 38, 59);">15表示是因为store引起的page fault</font>**<font style="color:rgb(35, 38, 59);">；</font>**<font style="color:rgb(35, 38, 59);">12表示是因为指令执行引起的page fault</font>**<font style="color:rgb(35, 38, 59);">。所以第二个信息存在SCAUSE寄存器中，其中总共有3个类型的原因与page fault相关，分别是读、写和指令。ECALL进入到supervisor mode对应的是8。基本上来说，page fault和其他的异常使用与系统调用相同的trap机制来从用户空间切换到内核空间。如果是因为page fault触发的trap机制并且进入到内核空间，STVAL寄存器和SCAUSE寄存器都会有相应的值</font>
* **<font style="color:rgb(35, 38, 59);">触发page fault的指令的地址</font>**<font style="color:rgb(35, 38, 59);">。作为trap处理代码的一部分，这个地址存放在SEPC（Supervisor Exception Program Counter）寄存器中，并同时会保存在trapframe->epc中</font>

<font style="color:rgb(35, 38, 59);">所以，从硬件和XV6的角度来说，当出现了page fault，有3个极其有价值的信息，分别是：</font>

* <font style="color:rgb(35, 38, 59);">引起page fault的内存地址</font>
* <font style="color:rgb(35, 38, 59);">引起page fault的原因类型</font>
* <font style="color:rgb(35, 38, 59);">引起page fault时的程序计数器值，这表明了page fault在用户空间发生的位置</font>

<font style="color:rgb(35, 38, 59);"></font>

### <font style="color:rgb(35, 38, 59);">写时复制 COW（</font><font style="color:rgb(51, 51, 51);">copy on write</font><font style="color:rgb(35, 38, 59);">）</font>

<font style="color:rgb(35, 38, 59);">在标准 </font><code><font style="color:rgb(35, 38, 59);">fork</font></code><font style="color:rgb(35, 38, 59);"> 中，父进程将内存完全拷贝给子进程。这样可以确保父子进程的内存是独立的，即便彼此之间内容相同，双方的修改不会互相影响。然而，这种方法需要分配大量内存、耗费时间。而且，如果子进程第一件事就是执行exec，马上丢弃这个地址空间，就浪费了操作，浪费了空间。</font>

<font style="color:rgb(35, 38, 59);">在 </font><code><font style="color:rgb(35, 38, 59);">COW fork</font></code><font style="color:rgb(35, 38, 59);"> 中，父进程和子进程会共享相同的物理内存页面，而不是立刻复制内存。</font>**<font style="color:rgb(35, 38, 59);">共享的页面被标记为只读</font>**<font style="color:rgb(35, 38, 59);">，无论是父进程还是子进程，都无法直接写入这些共享的页面。当父进程或子进程修改页面内容时，就会触发</font>**<font style="color:rgb(35, 38, 59);">页面错误异常</font>**<font style="color:rgb(35, 38, 59);">，内核捕获到异常，并根据 </font><code><font style="color:rgb(35, 38, 59);">scause</font></code><font style="color:rgb(35, 38, 59);"> 和 </font><code><font style="color:rgb(35, 38, 59);">stval</font></code><font style="color:rgb(35, 38, 59);"> 的信息确认错误类型和故障地址，执行操作：1.为子进程分配一个新的物理内存副本，为父进程分配一个新的物理内存副本。2.将新页面的物理地址映射到父子进程中产生页面错误的虚拟地址，并且更新页表中的权限为可读/写。3.返回到引发异常的指令位置，重新执行导致页面错误的写操作。</font>

### <font style="color:rgb(35, 38, 59);">惰性分配（Lazy Allocation）</font>

<font style="color:rgb(35, 38, 59);">当应用程序请求额外内存时，比如通过 </font><code><font style="color:rgb(35, 38, 59);">sbrk</font></code><font style="color:rgb(35, 38, 59);"> 系统调用增加地址空间，内核调整进程的地址空间范围，但会把新地址标记为无效。在应用程序实际访问这些无效地址时，CPU 会因为找不到对应的物理地址而触发页面错误，内核捕获到异常，分析错误地址属于之前 </font><code><font style="color:rgb(35, 38, 59);">sbrk</font></code><font style="color:rgb(35, 38, 59);"> 增加的范围，说明这是惰性分配引发的页面错误，于是内核会分配一个新的物理页面，并将该虚拟地址映射到新页面上，更新页表中的该地址条目为有效状态，并重新执行触发异常的指令。</font>

<font style="color:rgb(35, 38, 59);">应用程序往往请求比实际需要更多的内存，通过惰性分配，系统仅在真正使用内存时才进行分配，避免了大量内存浪费。</font><font style="color:rgb(35, 38, 59);">\ </font><font style="color:rgb(35, 38, 59);"> 	</font>

### <font style="color:rgb(35, 38, 59);">页面换出（Paging Out）</font>

<font style="color:rgb(35, 38, 59);">当进程的内存需求超过物理内存的容量时，系统将部分不常用的内存页面</font>**<font style="color:rgb(35, 38, 59);">写入到磁盘</font>**<font style="color:rgb(35, 38, 59);">，释放出物理内存用于其他页面。被写到磁盘的页面的PTE会被标记为无效，这样当进程再次访问这些页面时会产生页面错误，内核捕获到页面错误异常，检查故障地址，属于换出的页面，内核会分配一个新的物理页面，并将该页面内容从磁盘读取回内存，更新PTE ，将该页面重新标记为有效，恢复进程的执行。</font>

<font style="color:rgb(35, 38, 59);">物理内存有限，页面换出机制可以让系统运行更多进程，或者让单个进程使用比实际物理内存更多的地址空间。</font>

## 实验

### <font style="color:rgb(35, 38, 59);">Eliminate allocation from sbrk() (easy)</font>

<font style="color:rgb(35, 38, 59);">实现</font><code><font style="color:rgb(35, 38, 59);">lazy page allocation</font></code><font style="color:rgb(35, 38, 59);">的第一步，只记录分配了多少内存，但是不做实际的分配。</font>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">改动系统调用</font><code><font style="color:rgb(35, 38, 59);">sys_sbrk</font></code><font style="color:rgb(35, 38, 59);">，不再调用分配内存的</font><code><font style="color:rgb(35, 38, 59);">growproc</font></code><font style="color:rgb(35, 38, 59);">，仅让进程的</font><code><font style="color:rgb(35, 38, 59);">sz</font></code><font style="color:rgb(35, 38, 59);">加上新增的内存：</font>

```c
// kernel/sysproc.c
uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;

  /*if(growproc(n) < 0)     // 惰性分配
    return -1;*/

  struct proc* p = myproc();

  if (n > 0)                                  //惰性分配，这里仅改变sz字段
      p->sz += n;                       
  else if (p->sz + n > 0)              //如果是减少内存，还是要马上执行,要检查减去内存后是否大于0
      p->sz = uvmdealloc(p->pagetable, p->sz, p->sz + n);
  else
      return -1;

  return addr;
}
```

<font style="color:rgb(35, 38, 59);">这个时候可以启动xv6执行一个简单的命令，可以发现会报错</font><code><font style="color:rgb(35, 38, 59);">panic: uvmunmap: not mapped</font></code><font style="color:rgb(35, 38, 59);">，系统未给进程分配内存：</font>

```bash
$ echo hi
usertrap(): unexpected scause 0x000000000000000f pid=3
            sepc=0x00000000000012ac stval=0x0000000000004008
panic: uvmunmap: not mapped
```

### <font style="color:rgb(51, 51, 51);">Lazy allocation (moderate)</font>

<font style="color:rgb(35, 38, 59);">接下来修改</font><code><font style="color:rgb(35, 38, 59);">usertrap</font></code><font style="color:rgb(35, 38, 59);">函数，处理缺页异常。</font>

<font style="color:rgb(35, 38, 59);"></font>

<code><font style="color:rgb(35, 38, 59);">r_scause</font></code><font style="color:rgb(35, 38, 59);">可以获取异常原因，其中13表示</font><code><font style="color:rgb(35, 38, 59);">page load fault</font></code><font style="color:rgb(35, 38, 59);">，15表示</font><code><font style="color:rgb(35, 38, 59);">page write fault</font></code><font style="color:rgb(35, 38, 59);">，</font><code><font style="color:rgb(35, 38, 59);">stval</font></code><font style="color:rgb(35, 38, 59);">表示引发缺页异常的虚拟地址，缺页异常就可以由</font><code><font style="color:rgb(35, 38, 59);">(r_scause() == 13 || r_scause() == 15)</font></code><font style="color:rgb(35, 38, 59);">表示。</font>

<font style="color:rgb(35, 38, 59);">先判断发生错误的虚拟地址是否位于栈空间之上，进程大小之下（虚拟地址从0开始，进程大小可以表示进程的最高虚拟地址），然后为其分配物理内存并添加映射：</font>

```c
// kernel/trap.c
void
usertrap(void)
{
  ......
  } else if((which_dev = devintr()) != 0){
    // ok
  }
  else if (r_scause() == 13 || r_scause() == 15) {      // 惰性分配导致的缺页异常
      uint64 fault_va = r_stval();      // 获取引发缺页异常的虚拟地址
      char* pa = 0;                     // 分配的物理地址
      // 判断fault_va是否在进程栈空间之中
      if (PGROUNDUP(p->trapframe->sp) - 1 < fault_va && fault_va < p->sz && (pa = kalloc()) != 0) {
          memset(pa, 0, PGSIZE);
          // 物理内存映射
          if (mappages(p->pagetable, PGROUNDDOWN(fault_va), PGSIZE, (uint64)pa, PTE_R | PTE_W | PTE_X | PTE_U) != 0) {      
              printf("lazy alloc: failed to map page\n");
              kfree(pa);
              p->killed = 1;
          }
      }
  }else {
    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    p->killed = 1;
  }

  ......
}
```

<font style="color:rgb(35, 38, 59);">惰性分配刚开始并未实际分配内存，解除映射关系时应直接跳过这部分内存，不然会导致系统崩溃：</font>

```c
// kernel/vm.c
void
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free)
{
  ......

  for(a = va; a < va + npages*PGSIZE; a += PGSIZE){
      if ((pte = walk(pagetable, a, 0)) == 0)
          //   panic("uvmunmap: walk");     惰性分配，遇到不存在的页表项就跳过
          continue;
      if ((*pte & PTE_V) == 0)
          //   panic("uvmunmap: not mapped");   同上
          continue;
      if (PTE_FLAGS(*pte) == PTE_V)
          panic("uvmunmap: not a leaf");
      if(do_free){
          uint64 pa = PTE2PA(*pte);
          kfree((void*)pa);
  }
    *pte = 0;
  }
}
```

<font style="color:rgb(35, 38, 59);">这个时候再启动xv6，执行</font><code><font style="color:rgb(35, 38, 59);">echo hi</font></code><font style="color:rgb(35, 38, 59);">，就可以正常运行了。</font>

<font style="color:rgb(35, 38, 59);"></font>

### <font style="color:rgb(35, 38, 59);">Lazytests and Usertests (moderate)</font>

<code><font style="color:rgb(35, 38, 59);">sbrk()</font></code><font style="color:rgb(35, 38, 59);">参数为负的情况上面已处理。</font>

<font style="color:rgb(35, 38, 59);">使用分配的虚拟内存时注意检查是否在进程空间中（</font><code><font style="color:rgb(35, 38, 59);">PGROUNDUP(p->trapframe->sp) - 1 < va && va < p->sz</font></code><font style="color:rgb(35, 38, 59);">）</font>

<font style="color:rgb(35, 38, 59);">这里设计两个函数，（1）</font><code><font style="color:rgb(35, 38, 59);">kama_uvmshouldallocate</font></code><font style="color:rgb(35, 38, 59);">：检测一个虚拟地址是不是因为惰性分配还没分配和映射的地址。（2）</font><code><font style="color:rgb(35, 38, 59);">kama_uvmlazyallocate</font></code><font style="color:rgb(35, 38, 59);">：给虚拟地址分配和映射物理内存.</font>

<font style="color:rgb(35, 38, 59);">其中，</font><code><font style="color:rgb(35, 38, 59);">kama_uvmshouldallocate</font></code><font style="color:rgb(35, 38, 59);">要检测的有：</font>

* 处于 `[0, p->sz)`地址范围之中（进程申请的内存范围）
* <font style="color:rgb(35, 38, 59);">不是栈的 guard page（见 xv6手册，栈页的低一页故意留成不映射，作为哨兵用于捕捉 stack overflow 错误。惰性分配不应该给这个地址分配物理页和建立映射，而应该直接抛出异常）\ </font><font style="color:rgb(35, 38, 59);">（解决 usertests 中的 stacktest 失败的问题）</font>
* <font style="color:rgb(35, 38, 59);">页表项不存在</font>

`kama_uvmshouldallocate`：

```c
// kernel/vm.c
#include "spinlock.h"
#include "proc.h"			// 加上两个需要的头文件
//判断页面是否是之前惰性分配的地址，是的话返回1
int kama_uvmshouldallocate(uint64 va) {
    pte_t* pte;
    struct proc* p = myproc();

    return va < p->sz                   //确保地址在进程的内存大小范围内
        && PGROUNDDOWN(va) != r_sp()    //确保地址不在 guard page 中
        && (((pte = walk(p->pagetable, va, 0)) == 0) || ((*pte & PTE_V) == 0));     //确保页表项确实不存在
}
```

`kama_uvmlazyallocate`：

```c
// kernel/vm.c
//给惰性分配的页面分配并映射物理地址
void kama_uvmlazyallocate(uint64 va) {
    struct proc* p = myproc();
    char* pa = kalloc();    //分配物理地址
    if (pa == 0) {
        printf("lazy alloc: out of memory\n");
        p->killed = 1;
    }
    else {
        memset(pa, 0, PGSIZE);
        if (mappages(p->pagetable, PGROUNDDOWN(va), PGSIZE, (uint64)pa, PTE_W | PTE_X | PTE_R | PTE_U) != 0) {      //映射物理地址
            printf("lazy alloc: failed to map page\n");
            kfree(pa);
            p->killed = 1;
        }
    }
}
```

实现这两个函数后就可以在上面的usertrap函数中简化代码：

```c
// kernel/trap.c
void
usertrap(void)
{
  ......
  } else if((which_dev = devintr()) != 0){
    // ok
  }
  else {
    uint64 va = r_stval();
    if((r_scause() == 13 || r_scause() == 15) && kama_uvmshouldallocate(va)){   // 缺页异常，并且发生异常的地址进行过惰性分配
        kama_uvmlazyallocate(va); // 分配物理内存，并在页表创建映射
    } else { // 如果不是缺页异常，或者是在非惰性分配地址上发生缺页异常，则抛出错误并杀死进程
      printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
      printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
      p->killed = 1;
    }
  }

  ......
}
```

<font style="color:rgb(35, 38, 59);">由于惰性分配的页，在刚分配的时候是没有对应的映射的，所以要把一些原本在遇到无映射地址时会 panic 的函数的行为改为直接忽略这样的地址。</font>

`vm.c`中的`uvmunmap`（这个已经在上面实现了）：

```c
// kernel/vm.c
void
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free)
{
  ......

  for(a = va; a < va + npages*PGSIZE; a += PGSIZE){
      if ((pte = walk(pagetable, a, 0)) == 0)
          //   panic("uvmunmap: walk");     惰性分配，遇到不存在的页表项就跳过
          continue;
      if ((*pte & PTE_V) == 0)
          //   panic("uvmunmap: not mapped");   同上
          continue;
      ......
  }
}
```

`vm.c`中的`uvmcopy`：

```c
// kernel/vm.c
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  ......

  for(i = 0; i < sz; i += PGSIZE){
      if ((pte = walk(old, i, 0)) == 0)
          // panic("uvmcopy: pte should exist");    惰性分配导致某些pte未分配，遇到不存在的pte就跳过
          continue;
      if ((*pte & PTE_V) == 0)
          // panic("uvmcopy: page not present");    同上
          continue;
      ......
}
```

`vm.c`中的`copyin` 和 `copyout`，如果遇到没分配的虚拟地址就马上分配：

```c
// kernel/vm.c
int
copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
{
  uint64 n, va0, pa0;

  if (kama_uvmshouldallocate(dstva))
      kama_uvmlazyallocate(dstva);

  ......
}

int
copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  if (kama_uvmshouldallocate(dstva))
      kama_uvmlazyallocate(dstva);

  ......
}
```

至此可以验证实验是否通过。

## 可扩展的实验

* <font style="color:rgb(51, 51, 51);">让惰性分配协同Lab3中简化版的</font><code><font style="background-color:rgb(247, 247, 247);">copyin</font></code><font style="color:rgb(51, 51, 51);">一起工作。</font>


> 更新: 2024-12-07 15:42:50  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/ehvb5i8lq2c8dugr>