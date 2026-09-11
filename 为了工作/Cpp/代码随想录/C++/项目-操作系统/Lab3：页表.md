# Lab3：页表

实验难度出自官网，实验简述和实验耗时是主观体验

* pgtbl  `简述`：学习xv6的页表机制，提高用户空间和内核空间之间传递数据的效率
* `难度`：1 easy   2hard     最难的一个lab  `耗时`：21h
* `建议`：这是最难的一个lab，需要理解xv6的页表机制，需要阅读很多源码，但做完这个lab，再做后面的lab就得心应手了。建议通读`kernel/vm.c`。做完lab之后可以总结一下xv6的虚拟内存机制，面试常考题

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/pgtbl.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><http://xv6.dgs.zone/labs/requirements/lab3.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://blog.miigon.net/posts/s081-lab3-page-tables/>

开始实验前，**<font style="background-color:#FBDE28;">需要切换到本次实验的分支：pgtbl</font>\*\*\*\*。**

## 知识点

开始实验前，最好阅读[xv6手册](https://pdos.csail.mit.edu/6.S081/2020/xv6/book-riscv-rev1.pdf)的第三章（[翻译在这](http://xv6.dgs.zone/tranlate_books/book-riscv-rev1/c3/s0.html)），以及阅读源码`kernel/vm.c`。下面整理了一些比较重要的知识点。如果不理解，可以先做实验，在实践中体会页表的作用，之后再回来看这些内容。

### 页表

<font style="color:rgb(35, 38, 59);">页表是操作系统为每个进程提供私有地址空间和内存的机制。页表决定了内存地址的含义，以及物理内存的哪些部分可以访问。它们允许xv6隔离不同进程的地址空间，并将它们复用到单个物理内存上。页表还提供了一层抽象，这允许xv6执行一些特殊操作：映射相同的内存到不同的地址空间中，并用一个未映射的页面保护内核和用户栈区。</font>

<font style="color:rgb(35, 38, 59);">RISC-V指令（用户和内核指令）使用的是虚拟地址，而机器的RAM或物理内存是由物理地址索引的。RISC-V页表硬件通过将每个虚拟地址映射到物理地址来为这两种地址建立联系。</font>

<font style="color:rgb(35, 38, 59);">XV6基于Sv39 RISC-V运行，这意味着它只使用64位虚拟地址的低39位，高25位不使用。虚拟地址的前</font>**<font style="color:rgb(35, 38, 59);">27位</font>**<font style="color:rgb(35, 38, 59);">用于索引页表，页表在逻辑上视作由 2</font><sup><font style="color:rgb(35, 38, 59);">27</font></sup><font style="color:rgb(35, 38, 59);"> 个</font>**<font style="color:rgb(35, 38, 59);">页表条目（PTE）</font>**<font style="color:rgb(35, 38, 59);">组成的数组，每个PTE中包含一个</font>**<font style="color:rgb(35, 38, 59);">44位的物理页码（PPN）</font>**<font style="color:rgb(35, 38, 59);">，以及一些控制和状态的标志位。</font>

<font style="color:rgb(35, 38, 59);">地址转换过程：处理一个虚拟地址时，使用该虚拟地址的</font>**<font style="color:rgb(35, 38, 59);">前27位</font>**<font style="color:rgb(35, 38, 59);">在页表中查找对应的PTE。找到PTE后，PTE中的</font>**<font style="color:rgb(35, 38, 59);">44位PPN</font>**<font style="color:rgb(35, 38, 59);">与虚拟地址的</font>**<font style="color:rgb(35, 38, 59);">低12位</font>**<font style="color:rgb(35, 38, 59);">组合（即页内偏移），生成一个</font>**<font style="color:rgb(35, 38, 59);">56位的物理地址</font>**

<font style="color:rgb(35, 38, 59);"> 页表允许操作系统控制虚拟地址到物理地址的映射，并且以</font>**<font style="color:rgb(35, 38, 59);">4096字节（4KB）的页</font>**<font style="color:rgb(35, 38, 59);">为单位进行转换和管理，</font>**<font style="color:rgb(35, 38, 59);">以 4096 ( 2</font>**<sup>**<font style="color:rgb(35, 38, 59);">12</font>**</sup>**<font style="color:rgb(35, 38, 59);"> ) 字节的对齐块的粒度控制虚拟地址到物理地址的转换</font>**

<font style="color:rgb(35, 38, 59);">但实际在Sv39中，页表是一个</font>**<font style="color:rgb(35, 38, 59);">三级树型结构</font>**<font style="color:rgb(35, 38, 59);">，</font>**<font style="color:rgb(35, 38, 59);">根页表</font>**<font style="color:rgb(35, 38, 59);">是这棵树的根节点，它是一个4KB（4096字节）的页，每个页有512个PTE（每个PTE大小是8个字节（64位），总共可以有512个条目，4096字节 ÷ 8字节）。每个PTE记录了下一级页表的位置（也就是下一级页表的物理地址）。虚拟地址使用39位，其中的</font>**<font style="color:rgb(35, 38, 59);">前27位</font>**<font style="color:rgb(35, 38, 59);">被用来在三级页表结构中进行查找。在找到第三级的PTE之后，PTE中会有物理页号（PPN），这个物理页号提供了物理地址的</font>**<font style="color:rgb(35, 38, 59);">高44位</font>**<font style="color:rgb(35, 38, 59);">，虚拟地址的最后</font>**<font style="color:rgb(35, 38, 59);">12位</font>**<font style="color:rgb(35, 38, 59);">作页内偏移。实际的转换如下图所示：</font>

![1733383183337-4d67e5b9-633b-43a2-8342-30c03c4740cc.png](https://cdn.davidingplus.cn/images/2026/09/11/1733383183337-4d67e5b9-633b-43a2-8342-30c03c4740cc-426927.png)

### TLB

<font style="color:rgb(35, 38, 59);">对于一个虚拟内存地址的寻址，需要读三次内存，代价有点高。实际中，几乎所有的处理器都会对最近使用过的虚拟地址的翻译结果有缓存。这个缓存被称为：Translation Lookside Buffer，TLB。这就是Page Table Entry的缓存，也就是PTE的缓存。</font>

<font style="color:rgb(35, 38, 59);">当处理器第一次查找一个虚拟地址时，硬件通过3级page table得到最终的PPN，TLB会保存虚拟地址到物理地址的映射关系。这样下一次访问同一个虚拟地址时，处理器可以查看TLB，TLB会直接返回物理地址，而不需要通过page table得到结果。</font>

<font style="color:rgb(35, 38, 59);"> 如果切换了page table，操作系统需要告诉处理器当前正在切换page table，处理器会清空TLB。清空TLB的指令是</font><code><font style="color:rgb(35, 38, 59);">sfence_vma</font></code>

### <font style="color:rgb(35, 38, 59);">内核地址空间</font>

![1733383513099-c682ba03-7162-4041-958d-7917be5aa35d.png](https://cdn.davidingplus.cn/images/2026/09/11/1733383513099-c682ba03-7162-4041-958d-7917be5aa35d-111849.png)

<font style="color:rgb(35, 38, 59);">右边地址0x1000是boot ROM的物理地址。对主板上电，主板做的第一件事情就是运行存储在boot ROM中的代码，当boot完成之后，会跳转到地址0x80000000，操作系统需要确保那个地址有一些数据能够接着启动操作系统</font>

<font style="color:rgb(35, 38, 59);">当机器刚刚启动时，还没有可用的page，XV6操作系统会设置好内核使用的虚拟地址空间，也就是这张图左边的地址分布</font>

<font style="color:rgb(35, 38, 59);">操作系统启动时，会从地址0x80000000开始运行</font>

<font style="color:rgb(35, 38, 59);">左侧低于PHYSTOP的虚拟地址，与右侧使用的物理地址是一样的（直接映射）</font>

<font style="color:rgb(35, 38, 59);">在完成了虚拟到物理地址的翻译之后，如果得到的物理地址大于0x80000000会走向DRAM芯片，如果得到的物理地址低于0x80000000会走向不同的I/O设备</font>

<font style="color:rgb(35, 38, 59);">kernel stack在虚拟内存中的地址很靠后，这是因为在它之下有一个未被映射的Guard page，这个Guard page对应的PTE的Valid 标志位没有设置，这样，如果kernel stack耗尽了，它会溢出到Guard page，但是因为Guard page的PTE中Valid标志位未设置，会导致立即触发page fault，这样的结果好过内存越界之后造成的数据混乱。溢出时立即触发一个panic（也就是page fault），就知道kernel stack出错了。同时又不想浪费物理内存给Guard page，所以Guard page不会映射到任何物理内存，它只是占据了虚拟地址空间的一段靠后的地址。同时，kernel stack被映射了两次，在靠后的虚拟地址映射了一次，在PHYSTOP下的Kernel data中又映射了一次，但是实际使用的时候用的是上面的部分，因为有Guard page会更加安全</font>

<font style="color:rgb(35, 38, 59);">Kernel text被标为R-X，意味着你可以读它，也可以在这个地址段执行指令，但是你不能向Kernel text写数据。通过设置权限我们可以尽早的发现Bug从而避免Bug。对于Kernel data需要能被写入，所以它的标志位是RW-，但是你不能在这个地址段运行指令，所以它的X标志位未被设置。（所以，kernel text用来存代码，代码可以读，可以运行，但是不能篡改，kernel data用来存数据，数据可以读写，但是不能通过数据伪装代码在kernel中运行）</font>

<code><font style="color:rgb(35, 38, 59);">Free memory</font></code><font style="color:rgb(35, 38, 59);">：这段地址存放用户进程的page table（页表），text（代码段）和data（数据段）。如果运行了非常多的用户进程，某个时间点会耗尽这段内存，这个时候fork或者exec会返回错误。当kernel创建了一个进程，针对这个进程的page table也会从Free memory中分配出来。内核会为用户进程的页表分配几个page，并填入PTE。在某个时间点，当内核运行了这个进程，内核会将进程的根页表的地址加载到SATP中。从那个时间点开始，处理器会使用内核为那个进程构建的虚拟地址空间</font>

## 实验

### <font style="color:rgb(35, 38, 59);">Print a page table (easy)</font>

<font style="color:rgb(35, 38, 59);">该实验需要实现一个打印页表内容的函数，以题目所示的格式打印传进的页表。</font>

<font style="color:rgb(35, 38, 59);">在Sv39模式下，页表是一个</font>**<font style="color:rgb(35, 38, 59);">三级树型结构</font>**<font style="color:rgb(35, 38, 59);">，</font>**<font style="color:rgb(35, 38, 59);">根页表</font>**<font style="color:rgb(35, 38, 59);">是这棵树的根节点，它是一个4KB（4096字节）的页，每个页有512个PTE，每个PTE记录了下一级页表的位置（也就是下一级页表的物理地址，最后一级页表的PTE指向的是最终映射的物理地址）。</font>

<font style="color:rgb(35, 38, 59);">需要模拟查询页表的过程，对三级页表进行遍历并打印。在虚拟内存相关的</font><code><font style="color:rgb(35, 38, 59);">kernel/vm.c</font></code><font style="color:rgb(35, 38, 59);">中的</font><code><font style="color:rgb(35, 38, 59);">freewalk()</font></code><font style="color:rgb(35, 38, 59);">函数已经实现了递归遍历页表并将其释放，所以只要模仿其逻辑实现打印功能即可。代码：</font>

```c
// kernel/vm.c
// 递归打印页表
int kama_pgtblprint(pagetable_t pagetable, int depth) {
    // there are 2^9 = 512 PTEs in a page table.
    for (int i = 0; i < 512; i++) {
        pte_t pte = pagetable[i];

        if (pte & PTE_V) {      // 如果页表项有效，按格式打印页表项
            printf("..");
            for (int j = 0;j < depth;++j)
                printf(" ..");
            printf("%d: pte %p pa %p\n", i, pte, PTE2PA(pte));


            // 如果该节点不是叶节点，递归打印子节点
            if ((pte & (PTE_R | PTE_W | PTE_X)) == 0) {
                // this PTE points to a lower-level page table.
                uint64 child = PTE2PA(pte);
                kama_pgtblprint((pagetable_t)child, depth + 1);
            }
        }
    }

    return 0;
}

// 打印页表
int kama_vmprint(pagetable_t pagetable) {
    printf("page table %p\n", pagetable);
    return kama_pgtblprint(pagetable, 0);
}
```

<font style="color:rgb(35, 38, 59);">在内核头文件添加函数声明：</font>

```c
// kernel/defs.h
// vm.c
......
int             copyout(pagetable_t, uint64, char *, uint64);
int             copyin(pagetable_t, char *, uint64, uint64);
int             copyinstr(pagetable_t, char*, uint64, uint64);
int             kama_vmprint(pagetable_t pagetable);         //打印页表内容函数声明
```

<font style="color:rgb(35, 38, 59);">按照实验需求，在</font><code><font style="color:rgb(35, 38, 59);">kernel/exec.c</font></code><font style="color:rgb(35, 38, 59);">中exec函数的</font><code><font style="color:rgb(35, 38, 59);">return argc</font></code><font style="color:rgb(35, 38, 59);">之前插入</font><code><font style="color:rgb(35, 38, 59);">if(p->pid==1) kama_vmprint(p->pagetable)</font></code><font style="color:rgb(35, 38, 59);">，以打印第一个进程的页表：</font>

```c
// kernel/exec.c
int
exec(char *path, char **argv)
{
......
  if (p->pid == 1)
      kama_vmprint(p->pagetable);        // exec返回之前打印页表

  return argc; // this ends up in a0, the first argument to main(argc, argv)

 bad:
  if(pagetable)
    proc_freepagetable(pagetable, sz);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}
```

此时可验证该实验是否通过，该实验执行测评文件的命令为`./grade-lab-pgtbl pte`。也<font style="color:rgb(35, 38, 59);">可以执行</font><code><font style="color:rgb(35, 38, 59);">make qemu</font></code><font style="color:rgb(35, 38, 59);">启动xv6，验证是否得到和示例相似的结果，打印出了第一个进程的页表。</font>

### <font style="color:rgb(35, 38, 59);">A kernel page table per process (hard)</font>

<font style="color:rgb(35, 38, 59);">当前xv6操作系统中，在用户态下的每个用户进程都使用各自的用户态页表。一旦进入了内核态（例如系统调用）就会切换到内核态页表（通过修改 satp 寄存器，trampoline.S）。然而这个内核态页表是全局共享的，所有进程进入内核态之后都会共用一个内核态页表。</font>

<font style="color:rgb(35, 38, 59);">共享一个内核页表有什么弊端呢？</font>

<font style="color:rgb(35, 38, 59);">进程可能会意外或恶意地访问其他进程的内核数据。如果一个进程因为 bug 或恶意操作访问了内核中的敏感数据，它可能会影响其他进程或系统的整体稳定性。</font>

<font style="color:rgb(35, 38, 59);">每次创建或删除进程时，都需要小心更新共享的页表条目，以确保不同进程之间的内存不会冲突或被错误覆盖。这会增加系统的复杂性，并且在多核系统中，这种全局共享的管理会增加同步开销和冲突的可能性。</font>

<font style="color:rgb(35, 38, 59);">如果每个进程进入内核态之后，都能有自己独立的内核页表，可以避免很多麻烦，这就是这个实验的目的。</font>

<font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;">  
</font><font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;">	</font>`<font style="color:rgb(35, 38, 59);">kernel/proc.h</font>`<font style="color:rgb(35, 38, 59);">中，在进程的结构体proc中添加一个新的内核页表属性，用来存储进程独享的内核态页表：</font>

```c
// kernel/proc.h
// Per-process state
struct proc {
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
  pagetable_t kama_kernelpgtbl;     // 存储进程独享的内核态页表
};
```

<font style="color:rgb(35, 38, 59);">内核进程需要依赖内核页表内一些固定的映射才能正常工作，例如 UART 控制、硬盘界面、中断控制等。在</font><code><font style="color:rgb(35, 38, 59);">kernel/vm.c</font></code><font style="color:rgb(35, 38, 59);">中， </font><code><font style="color:rgb(35, 38, 59);">kvminit</font></code><font style="color:rgb(35, 38, 59);"> 函数原本只为全局内核页表 </font><code><font style="color:rgb(35, 38, 59);">kernel_pagetable</font></code><font style="color:rgb(35, 38, 59);"> 添加这些映射，所以接下来大幅度改动</font><code><font style="color:rgb(35, 38, 59);">kernel/vm.c</font></code><font style="color:rgb(35, 38, 59);">，使其他进程也可以创建独享的内核页表。</font>

<font style="color:rgb(35, 38, 59);">先将原本的</font><code><font style="color:rgb(35, 38, 59);">kvminit</font></code><font style="color:rgb(35, 38, 59);">抽象，全局内核页表仍然使用这个函数来初始化：</font>

```c
// kernel/vm.c
/*
 * create a direct-map page table for the kernel.
 */
void
kvminit()
{
    // 全局内核页表仍然使用kvminit函数来初始化
    kernel_pagetable = kama_kvminit_newpgtbl(); 
}
```

<font style="color:rgb(35, 38, 59);">实现</font><code><font style="color:rgb(35, 38, 59);">kama_kvminit_newpgtbl</font></code><font style="color:rgb(35, 38, 59);">函数，创建一个页表并初始化映射，返回这个页表：</font>

```c
// kernel/vm.c
pagetable_t
kama_kvminit_newpgtbl()
{
    pagetable_t pgtbl = (pagetable_t) kalloc();
    memset(pgtbl, 0, PGSIZE);

    kama_kvm_map_pagetable(pgtbl);

    return pgtbl;
}
```

这个函数在其他模块会使用到，**需要去内核头文件声明**。

实现<font style="color:rgb(35, 38, 59);">初始化页表映射函数</font><code><font style="color:rgb(35, 38, 59);">kama_kvm_map_pagetable</font></code><font style="color:rgb(35, 38, 59);">：</font>

```c
// kernel/vm.c
void kama_kvm_map_pagetable(pagetable_t pgtbl) {
    // 将各种内核需要的 direct mapping 添加到页表 pgtbl 中
    
    // uart registers
    kvmmap(pgtbl, UART0, UART0, PGSIZE, PTE_R | PTE_W);

    // virtio mmio disk interface
    kvmmap(pgtbl, VIRTIO0, VIRTIO0, PGSIZE, PTE_R | PTE_W);

    // CLINT
    kvmmap(pgtbl, CLINT, CLINT, 0x10000, PTE_R | PTE_W);

    // PLIC
    kvmmap(pgtbl, PLIC, PLIC, 0x400000, PTE_R | PTE_W);

    // map kernel text executable and read-only.
    kvmmap(pgtbl, KERNBASE, KERNBASE, (uint64)etext-KERNBASE, PTE_R | PTE_X);

    // map kernel data and the physical RAM we'll make use of.
    kvmmap(pgtbl, (uint64)etext, (uint64)etext, PHYSTOP-(uint64)etext, PTE_R | PTE_W);

    // map the trampoline for trap entry/exit to
    // the highest virtual address in the kernel.
    kvmmap(pgtbl, TRAMPOLINE, (uint64)trampoline, PGSIZE, PTE_R | PTE_X);
}
```

<font style="color:rgb(35, 38, 59);">现在普通进程也可以通过调用</font><code><font style="color:rgb(35, 38, 59);">kama_kvminit_newpgtbl</font></code><font style="color:rgb(35, 38, 59);">函数来创建自己的内核页表了，此时在内核态中就有两种页表：一种是内核进程独享的页表，另一种是其他进程各自独享的页表。所以关于内核页表处理的一些函数需要做一些改动。</font>

<code><font style="color:rgb(35, 38, 59);">kvmmap</font></code><font style="color:rgb(35, 38, 59);">函数：将虚拟地址映射到物理地址，之前只处理内核进程的页表。所以修改一下让这个函数可以处理所有的页表：</font>

```c
// kernel/vm.c
// 将某个虚拟地址映射到某个物理地址（添加第一个参数）
void
kvmmap(pagetable_t pgtbl, uint64 va, uint64 pa, uint64 sz, int perm)            
{
    if(mappages(pgtbl, va, sz, pa, perm) != 0)
        panic("kvmmap");
}
```

<code><font style="color:rgb(35, 38, 59);">kvmpa</font></code><font style="color:rgb(35, 38, 59);">函数：将虚拟地址翻译成物理地址，原来也是只处理内核进程的页表，同样修改一下：</font>

```c
// kernel/vm.c
// kvmpa 将虚拟地址翻译为物理地址（添加第一个参数）
uint64
kvmpa(pagetable_t pgtbl, uint64 va)         
{
    uint64 off = va % PGSIZE;
    pte_t *pte;
    uint64 pa;

    pte = walk(pgtbl, va, 0);			//kernel_pagetable改为参数pgtbl
    if (pte == 0)
        panic("kvmpa");
    if ((*pte & PTE_V) == 0)
        panic("kvmpa");
    pa = PTE2PA(*pte);
    return pa + off;
}
```

这里修改了kvmmap和kvmpa函数的入口参数，**需要在内核头文件中也做出修改**。

<font style="color:rgb(35, 38, 59);">这样可以创建进程间相互独立的内核页表了，接下来处理：内核栈。 原本的 xv6 设计中，所有处于内核态的进程都共享同一个页表，即意味着共享同一个地址空间。由于 xv6 支持多核/多进程调度，同一时间可能会有多个进程处于内核态，所以需要对所有处于内核态的进程创建其独立的内核态内的栈，也就是内核栈，供给其内核态代码执行过程。</font>

<font style="color:rgb(35, 38, 59);">在已经添加的新修改中，每一个进程都会有自己独立的内核页表。而现在需要每个进程只访问自己的内核栈，所以可以把每个进程的内核栈映射到各自内核页表的固定位置（不同页表内的同一逻辑地址，指向不同物理内存）</font>

<font style="color:rgb(35, 38, 59);">原本xv6在</font><code><font style="color:rgb(35, 38, 59);">kernel/proc.c</font></code><font style="color:rgb(35, 38, 59);">中的procinit函数为每一个进程分配好了内核栈（在共享空间中），所以先把这部分代码去掉：</font>

```c
// kernel/proc.c
// initialize the proc table at boot time.
void
procinit(void)
{
    struct proc *p;
    
    initlock(&pid_lock, "nextpid");
    for(p = proc; p < &proc[NPROC]; p++) {
        initlock(&p->lock, "proc");

        // Allocate a page for the process's kernel stack.
        // Map it high in memory, followed by an invalid
        // guard page.
        //   char *pa = kalloc();
        //   if(pa == 0)
        //     panic("kalloc");
        //   uint64 va = KSTACK((int) (p - proc));
        //   kvmmap(va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
        //   p->kstack = va;

        // 注释掉了上面的代码（为所有进程预分配内核栈的代码），变为创建进程的时候再创建内核栈
    }
    kvminithart();
}
```

<font style="color:rgb(35, 38, 59);">改为在创建进程的时候（</font><code><font style="color:rgb(35, 38, 59);">kernel/proc.c</font></code><font style="color:rgb(35, 38, 59);">中的allocproc函数），为进程创建独立的内核页表，然后将专属的内核栈固定到内核页表的固定位置，建立映射：</font>

```c
// kernel/proc.c
static struct proc*
allocproc(void)
{
    ......
        
    // An empty user page table.
    p->pagetable = proc_pagetable(p);
    if(p->pagetable == 0){
        freeproc(p);
        release(&p->lock);
        return 0;
    }

    // 为新进程创建独立的内核页表，并将内核所需要的各种映射添加到新页表上
    p->kama_kernelpgtbl = kama_kvminit_newpgtbl();

    // 分配一个物理页，作为新进程的内核栈使用
    char* pa = kalloc();
    if (pa == 0)
        panic("kallo");
    uint64 va = KSTACK((int)0);     // 将内核栈映射到固定的逻辑地址上
    kvmmap(p->kama_kernelpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
    p->kstack = va;     // 记录内核栈的虚拟地址

    // Set up new context to start executing at forkret,
    // which returns to user space.
    memset(&p->context, 0, sizeof(p->context));
    p->context.ra = (uint64)forkret;
    p->context.sp = p->kstack + PGSIZE;

    return p;
}
```

<font style="color:rgb(35, 38, 59);">现在进程的内核页表就创建完成了，但是进程进入内核态时还是会使用全局的内核进程页表，需要在 </font><code><font style="color:rgb(35, 38, 59);">kernel/proc.c</font></code><font style="color:rgb(35, 38, 59);">中的scheduler函数进行相关修改。在调度器将 CPU 交给进程执行之前，加载进程的内核页表到SATP寄存器，切换到该进程对应的内核页表：</font>

```c
// kernel/proc.c
void
scheduler(void)
{
		......
        p->state = RUNNING;
        c->proc = p;

        // 切换到进程独立的内核页表
        w_satp(MAKE_SATP(p->kama_kernelpgtbl));
        sfence_vma();       // 清除快表缓存，刷新TLB缓存，以确保地址转换表的更改生效

         // 调度，执行进程
        swtch(&c->context, &p->context);

        // 切换回全局内核页表
        kvminithart();

        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
		......
}
```

<font style="color:rgb(35, 38, 59);">现在，每个进程都会在内核态使用自己独立的内核页表了.</font>

<font style="color:rgb(35, 38, 59);">在进程结束后，应该释放进程独享的页表以及内核栈，回收资源，否则会导致内存泄漏。</font>

<font style="color:rgb(35, 38, 59);">原本释放内存的函数在</font><code><font style="color:rgb(35, 38, 59);">kernel/proc.c</font></code><font style="color:rgb(35, 38, 59);">中，在此修改。这里按创建的顺序反着来，先释放进程的内核栈，再释放进程的内核页表：</font>

```c
// kernel/proc.c
static void
freeproc(struct proc *p)
{
    if(p->trapframe)
        kfree((void*)p->trapframe);
    p->trapframe = 0;
    if(p->pagetable)
        proc_freepagetable(p->pagetable, p->sz);
    p->pagetable = 0;
    p->sz = 0;
    p->pid = 0;
    p->parent = 0;
    p->name[0] = 0;
    p->chan = 0;
    p->killed = 0;
    p->xstate = 0;

    // 释放进程的内核栈
    void* kstack_pa = (void*)kvmpa(p->kama_kernelpgtbl, p->kstack);
    kfree(kstack_pa);
    p->kstack = 0;

    // 不能使用 proc_freepagetable释放页表，因为其不仅会释放页表本身，还会把页表内所有的叶节点对应的物理页也释放掉。
    // 这会导致内核运行所需要的关键物理页被释放，造成内核崩溃。
    
    // 递归释放进程独享的页表，释放页表本身所占用的空间，但不释放页表指向的物理页
    kama_kvm_free_kernelpgtbl(p->kama_kernelpgtbl);
    p->kama_kernelpgtbl = 0;
    p->state = UNUSED;
}
```

<font style="color:rgb(35, 38, 59);">如果使用</font><code><font style="color:rgb(35, 38, 59);">proc_freepagetable</font></code><font style="color:rgb(35, 38, 59);">函数，会同时释放掉内核进程必要的映射，导致内核崩溃。</font><code><font style="color:rgb(35, 38, 59);">proc_freepagetable</font></code><font style="color:rgb(35, 38, 59);">函数如下：</font>

```c
// Free a process's page table, and free the
// physical memory it refers to.
void
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

```

<font style="color:rgb(35, 38, 59);">所以在</font><code><font style="color:rgb(35, 38, 59);">kernel/vm.c</font></code><font style="color:rgb(35, 38, 59);">中另写一个</font><code><font style="color:rgb(35, 38, 59);">kama_kvm_free_kernelpgtbl</font></code><font style="color:rgb(35, 38, 59);">函数，不释放页表指向的物理页：</font>

```c
// kernel/vm.c
// 递归释放一个内核页表中的所有映射，但是不释放其指向的物理页
void
kama_kvm_free_kernelpgtbl(pagetable_t pagetable) {
    for (int i = 0;i < 512;++i) {
        pte_t pte = pagetable[i];
        uint64 child = PTE2PA(pte);
        if ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {      // 如果该页表项指向更低一级的页表
            kama_kvm_free_kernelpgtbl((pagetable_t)child);                     // 递归释放低一级页表及其页表项
            pagetable[i] = 0;
        }
    }
    kfree((void*)pagetable);        // 释放当前级别页表所占用空间
}
```

**该函数需要去内核头文件声明**。

最后，<font style="color:rgb(35, 38, 59);">因为上面改</font>动了`kvmpa`函数，这个函数在`kernel/virtio_disk.c`中也调用了，所以在这里也做出对应<font style="color:rgb(35, 38, 59);">修改：</font>

```c
// kernel/virtio_disk.c
#include "proc.h" // 添加头文件引入

 ......

void
virtio_disk_rw(struct buf *b, int write)
{
 ......
// 调用 myproc()获取进程内核页表
disk.desc[idx[0]].addr = (uint64) kvmpa(myproc()->kama_kernelpgtbl, (uint64) &buf0); 
 ......
}
```

至此可以验证实验是否通过，该实验执行测评文件的命令为`./grade-lab-pgtbl usertests`。

### <font style="color:rgb(35, 38, 59);">Simplify copyin/copyinstr (hard)</font>

<font style="color:rgb(35, 38, 59);">上一个实验已经让每一个进程都有独立的内核态页表了，该实验需要将用户态的映射添加到每个进程的内核页表，也就是将用户态的页表复制到内核态的页表。这样使得内核态也可以对用户态传进来的指针（逻辑地址）进行解引用。</font><font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;">\ </font><font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;">	</font><font style="color:rgb(35, 38, 59);">原来的</font><code><font style="color:rgb(35, 38, 59);">copyin</font></code><font style="color:rgb(35, 38, 59);">函数通过软件模拟访问页表的过程获取物理地址的，而在内核页表内维护映射副本的话，可以利用 CPU 的硬件寻址功能进行寻址，效率更高并且可以通过快表加速。</font>

<font style="color:rgb(35, 38, 59);">要实现这样的效果，我们需要在每一处内核对用户页表进行修改的时候，将同样的修改也同步应用在进程的内核页表上，使得两个页表的程序段（0 到 PLIC 段）地址空间的映射同步。</font>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">首先在</font><code><font style="color:rgb(35, 38, 59);">kernel/vm.c</font></code><font style="color:rgb(35, 38, 59);">实现一个复制页表的函数：</font>

```c
// kernel/vm.c
// 将 src 页表的一部分页映射关系拷贝到 dst 页表中。只拷贝页表项，不拷贝实际的物理页内存
int
kama_kvmcopymappings(pagetable_t src, pagetable_t dst, uint64 start, uint64 sz) {
    pte_t* pte;
    uint64 pa, i;
    uint flags;

    // PGROUNDUP: 将地址向上取整到页边界，防止重新映射已经映射的页，特别是在执行growproc操作时
    for (i = PGROUNDUP(start);i < start + sz;i += PGSIZE) {
        if ((pte = walk(src, i, 0)) == 0)
            panic("kvmcopymappings: pte should exist");
        if ((*pte & PTE_V) == 0)
            panic("kvmcopymappings: page not present");
        pa = PTE2PA(*pte);

        // `& ~PTE_U` 表示将该页的权限设置为非用户页
        // 必须设置该权限，因为RISC-V 中内核是无法直接访问用户页的
        flags = PTE_FLAGS(*pte) & ~PTE_U;
        if (mappages(dst, i, PGSIZE, pa, flags) != 0)
            goto err;
    }

    return 0;

err:
    //解除目标页表中已映射的页表项
    uvmunmap(dst, PGROUNDUP(start), (i - PGROUNDUP(start)) / PGSIZE, 0);            
    return -1;
}
```

<font style="color:rgb(35, 38, 59);">再实现一个缩减内存的函数，用于内核页表和用户页表内存映射的同步：</font>

```c
// kernel/vm.c
// 与 uvmdealloc 功能类似，将程序内存从 oldsz 缩减到 newsz，但不释放实际内存
uint64
kama_kvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz) {
    if (newsz >= oldsz)
        return oldsz;

    if (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
        int npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
        uvmunmap(pagetable, PGROUNDUP(newsz), npages, 0);
    }

    return newsz;
}
```

<font style="color:rgb(35, 38, 59);">xv6内核中，用于映射程序内存的地址范围是\[0,PLIC)，PLIC：</font>

```c
// kernel/memlayout.h
#define PLIC 0x0c000000L
```

<font style="color:rgb(35, 38, 59);">需要把进程的程序内存映射到其内核页表的这个范围，首先确认这个范围内没有和其他映射冲突。</font>

<font style="color:rgb(35, 38, 59);">在xv6手册中，可以看到这个范围中有一个CLINT（核心本地中断器）的映射，这个映射和刚才说的程序内存映射有冲突了。</font>

![1733397226874-0d2fb8f5-ff92-4f21-a36c-86591570a3ce.png](https://cdn.davidingplus.cn/images/2026/09/11/1733397226874-0d2fb8f5-ff92-4f21-a36c-86591570a3ce-970523.png)

<font style="color:rgb(35, 38, 59);">不过在手册中也可知，CLINT映射只在内核启动的时候需要使用，在内核态的用户进程并不需要使用这个映射。</font>\ <font style="color:rgb(35, 38, 59);">	所以可以在上一个实验中的</font><code><font style="color:rgb(35, 38, 59);">kama_kvm_map_pagetable</font></code><font style="color:rgb(35, 38, 59);">函数中修改一下，把CLINT这个映射去掉：</font>

```c
// kernel/vm.c
void kama_kvm_map_pagetable(pagetable_t pgtbl) {
    // 将各种内核需要的 direct mapping 添加到页表 pgtbl 中
    
    ......

    // CLINT
    // kvmmap(pgtbl, CLINT, CLINT, 0x10000, PTE_R | PTE_W);  
    // CLINT 仅在内核启动的时候需要使用到，而用户进程在内核态中的操作并不需要使用到该映射，并且该映射会与要 map 的程序内存冲突    

    ......
}
```

<font style="color:rgb(35, 38, 59);">这样进程的内核页表中就不会有程序内存映射和CLINT映射冲突的问题了。但是这个映射是内核启动所必须的，所以可以在全局内核页表初始化中加上这个映射：</font>

```c
// kernel/vm.c

void
kvminit()
{
    // 全局内核页表仍然使用kvminit函数来初始化
    kernel_pagetable = kvminit_newpgtbl(); 
    // 全局内核页表仍需要映射 CLINT
    kvmmap(kernel_pagetable, CLINT, CLINT, 0x10000, PTE_R | PTE_W);
}

```

<font style="color:rgb(35, 38, 59);">接下来在 </font><code><font style="color:rgb(35, 38, 59);">kernel/exec.c</font></code><font style="color:rgb(35, 38, 59);">中的exex() 中加入检查，防止程序内存超过 PLIC：</font>

```c
// kernel/exec.c
int
exec(char *path, char **argv)
{
  ......

  // Load program into memory.
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    uint64 sz1;
    if((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    if(sz1 >= PLIC) // 防止程序内存大小超过 PLIC
      goto bad;
    sz = sz1;
    if(ph.vaddr % PGSIZE != 0)
      goto bad;
    if(loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
    
  .......
}

```

接下来<font style="color:rgb(35, 38, 59);">涉及到用户态页表的修改，都要把相应的修改同步到进程的内核页表中，包括：</font><code><font style="color:rgb(35, 38, 59);">fork()</font></code><font style="color:rgb(35, 38, 59);">、</font><code><font style="color:rgb(35, 38, 59);">exec()</font></code><font style="color:rgb(35, 38, 59);">、</font><code><font style="color:rgb(35, 38, 59);">growproc()</font></code><font style="color:rgb(35, 38, 59);">、</font><code><font style="color:rgb(35, 38, 59);">userinit()</font></code><font style="color:rgb(35, 38, 59);">：</font>

```c
// kernel/proc.c
int
fork(void)
{
  ......
  // Copy user memory from parent to child. 
  // 加入调用 kvmcopymappings，将新进程用户页表映射拷贝一份到新进程内核页表中
  if (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0 || 
      kama_kvmcopymappings(np->pagetable, np->kama_kernelpgtbl, 0, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    return -1;
  }
  np->sz = p->sz;

  ......
}
```

```c
// kernel/exec.c
int
exec(char *path, char **argv)
{
	......
  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(p->name, last, sizeof(p->name));

  // 清除内核页表中对程序内存的旧映射，然后重新建立映射
  uvmunmap(p->kama_kernelpgtbl, 0, PGROUNDUP(oldsz) / PGSIZE, 0);
  kama_kvmcopymappings(pagetable, p->kama_kernelpgtbl, 0, sz);

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry;  // initial program counter = main
  p->trapframe->sp = sp; // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  ......
}
```

```c
// kernel/proc.c
int
growproc(int n)
{
  uint sz;
  struct proc *p = myproc();

  sz = p->sz;
  if (n > 0) {
      uint64 newsz;
      if ((newsz = uvmalloc(p->pagetable, sz, sz + n)) == 0)
          return -1;

      // 内核页表中的映射同步扩大
      if (kama_kvmcopymappings(p->pagetable, p->kama_kernelpgtbl, sz, n) != 0) {
          uvmdealloc(p->pagetable, newsz, sz);
          return -1;
      }
      sz = newsz;
  }
  else if (n < 0) {
      uvmdealloc(p->pagetable, sz, sz + n);
      // 内核页表中的映射同步缩小
      sz = kama_kvmdealloc(p->kama_kernelpgtbl, sz, sz + n);
  }
  p->sz = sz;
  return 0;
}
```

```c
// kernel/proc.c
void
userinit(void)
{
  ......
  
  // allocate one user page and copy init's instructions
  // and data into it.
  uvminit(p->pagetable, initcode, sizeof(initcode));
  p->sz = PGSIZE;
  // 同步程序内存映射到进程内核页表中
  kama_kvmcopymappings(p->pagetable, p->kama_kernelpgtbl, 0, p->sz);      

  ......
}
```

<font style="color:rgb(35, 38, 59);">这样就实现了进程用户态页表和内核态页表的同步。</font>

<font style="color:rgb(35, 38, 59);">按照实验要求替换</font><code><font style="color:rgb(35, 38, 59);">copyin</font></code><font style="color:rgb(35, 38, 59);">、</font><code><font style="color:rgb(35, 38, 59);">copyinstr</font></code><font style="color:rgb(35, 38, 59);">：</font>

```c
// kernel/vm.c
// 将 copyin、copyinstr 改为转发到新函数
int
copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len)
{
  return copyin_new(pagetable, dst, srcva, len);
}

int
copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max)
{
  return copyinstr_new(pagetable, dst, srcva, max);
}
```

<code><font style="color:rgb(35, 38, 59);">copyin_new</font></code><font style="color:rgb(35, 38, 59);">、</font><code><font style="color:rgb(35, 38, 59);">copyinstr</font></code><font style="color:rgb(35, 38, 59);">已在</font><code><font style="color:rgb(35, 38, 59);">kernel/vmcopyin.c</font></code><font style="color:rgb(35, 38, 59);">中实现。</font>

<font style="color:rgb(35, 38, 59);">注意：新添加的函数、修改了传参的函数要去</font><code><font style="color:rgb(35, 38, 59);">defs.h</font></code><font style="color:rgb(35, 38, 59);">中做出对应的调整，不然程序会找不到对应的函数调用。</font>

<font style="color:rgb(35, 38, 59);">至此可以验证全部实验是否正确完成。</font>

## <font style="color:rgb(35, 38, 59);">可扩展的方向</font>

* 超级页减少页表中 PTE 的数量，从而减少页表占用的内存和 TLB 查找的开销
* 扩展您的解决方案以支持尽可能大的用户程序，也就是说，消除用户程序小于PLIC的限制
* 消映射用户进程的第一页，使对空指针的解引用引发错误。用户文本段必须从非0处开始，例如4096（ 在 C/C++ 中，空指针的地址是 `0`。如果访问地址为 `0` 的内存，可能导致未定义行为。xv6 中，用户进程的内存从地址0开始，这使得解引用空指针可能不会引发错误，而是访问合法的内存。所以要求取消用户进程地址空间中的第一个页面映射，使得访问地址 `0` 引发段错误）


> 更新: 2024-12-05 20:10:20  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/sr3f98yti6o7p708>