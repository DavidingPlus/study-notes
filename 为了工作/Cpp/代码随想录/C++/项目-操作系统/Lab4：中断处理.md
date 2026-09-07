# Lab4：中断处理

实验难度出自官网，实验简述和实验耗时是主观体验

* traps  `简述`：从系统调用入手梳理中断的全流程
* `难度`：1 easy   1 moderate   1hard  `耗时`：7h
* `建议`：做完lab后可以梳理总结一下系统调用的全流程，面试常考题

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/traps.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><http://xv6.dgs.zone/labs/requirements/lab4.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://mit-public-courses-cn-translatio.gitbook.io/mit6-s081/lec06-isolation-and-system-call-entry-exit-robert>

```
[https://blog.miigon.net/posts/s081-lab4-traps/](https://blog.miigon.net/posts/s081-lab4-traps/)
```

开始实验前，**<font style="background-color:#FBDE28;">需要切换到本次实验的分支：traps</font>\*\*\*\*。**

## 知识点

<font style="color:rgb(35, 38, 59);">该实验从系统调⽤⼊⼿梳理中断的全流程。</font>

<font style="color:rgb(35, 38, 59);">最好先阅读</font>[xv6手册](https://pdos.csail.mit.edu/6.S081/2020/xv6/book-riscv-rev1.pdf)的第四章（[翻译在这](http://xv6.dgs.zone/tranlate_books/book-riscv-rev1/c4/s0.html)），以及阅读源码`kernel/trap.c`。

下面我整理了本实验的一些知识点，内容较多，如果没时间阅读可以先做实验。

### trap机制

<font style="color:rgb(35, 38, 59);">每当</font>

* <font style="color:rgb(35, 38, 59);">程序执行系统调用</font>
* <font style="color:rgb(35, 38, 59);">程序出现了类似page fault、运算时除以0的错误</font>
* <font style="color:rgb(35, 38, 59);">一个设备触发了中断使得当前程序运行需要响应内核设备驱动</font>

<font style="color:rgb(35, 38, 59);">都会发生用户空间和内核空间的切换，这种切换通常被称为trap。很多应用程序，要么因为系统调用，要么因为page fault，都会频繁的切换到内核中。</font>

<font style="color:rgb(35, 38, 59);"> 需要清楚如何让程序的运行，从只拥有user权限并且位于用户空间的程序，切换到拥有supervisor权限的内核。在这个过程中，硬件的状态将会非常重要，因为很多的工作都是将硬件从适合运行用户应用程序的状态，改变到适合运行内核代码的状态。</font>

<font style="color:rgb(35, 38, 59);">用户应用程序可以使用全部的32个寄存器，很多寄存器都有特殊的作用。其中</font>

* <font style="color:rgb(35, 38, 59);">程序计数寄存器（Program Counter Register）</font>
* <font style="color:rgb(35, 38, 59);">表明当前mode的标志位，这个标志位表明了当前是supervisor mode还是user mode</font>
* <font style="color:rgb(35, 38, 59);">还有一堆控制CPU工作方式的寄存器，比如SATP（Supervisor Address Translation and Protection）寄存器，包含了指向page table的物理内存地址</font>
* <font style="color:rgb(35, 38, 59);">STVEC（Supervisor Trap Vector Base Address Register）寄存器，指向了内核中处理trap的指令的起始地址</font>
* <font style="color:rgb(35, 38, 59);">SEPC（Supervisor Exception Program Counter）寄存器，在trap的过程中保存程序计数器的值</font>
* <font style="color:rgb(35, 38, 59);">SSCRATCH（Supervisor Scratch Register）寄存器，这也是个非常重要的寄存器</font>

<font style="color:rgb(35, 38, 59);">这些寄存器表明了</font>**<font style="color:rgb(35, 38, 59);">执行系统调用时计算机的状态</font>**<font style="color:rgb(35, 38, 59);">。</font>

<font style="color:rgb(35, 38, 59);">在trap处理的过程中需要更改一些状态，或者对状态做一些操作。这样才可以运行系统内核中的程序。先来预览一下需要做的操作：</font>

* <font style="color:rgb(35, 38, 59);">首先需要保存32个用户寄存器。因为很显然我们需要恢复用户应用程序的执行，尤其是当用户程序随机的被设备中断所打断时。我们希望内核能够响应中断，之后在用户程序完全无感知的情况下再恢复用户代码的执行。所以这意味着32个用户寄存器不能被内核弄乱。但是这些寄存器又要被内核代码所使用，所以在trap之前必须先在某处保存这32个用户寄存器</font>
* <font style="color:rgb(35, 38, 59);">程序计数器也需要在某个地方保存，它几乎跟一个用户寄存器的地位是一样的，需要能够在用户程序运行中断的位置继续执行用户程序</font>
* <font style="color:rgb(35, 38, 59);">需要将mode改成supervisor mode，因为需要使用内核中的各种各样的特权指令</font>
* <font style="color:rgb(35, 38, 59);">SATP寄存器现在正指向user page table，而user page table只包含了用户程序所需要的内存映射和一两个其他的映射，它并没有包含整个内核数据的内存映射。所以在运行内核代码之前，需要将SATP指向kernel page table</font>
* <font style="color:rgb(35, 38, 59);">需要将堆栈寄存器指向位于内核的一个地址，因为需要一个堆栈来调用内核的函数</font>
* <font style="color:rgb(35, 38, 59);">一旦设置好了，并且所有的硬件状态都适合在内核中使用， 需要跳入内核的代码</font>

<font style="color:rgb(35, 38, 59);">不能让用户代码介入到这里的user/kernel切换，否则有可能会破坏安全性。所以这意味着，trap中涉及到的硬件和内核机制不能依赖任何来自用户空间东西。比如不能依赖32个用户寄存器，它们可能保存的是恶意的数据，所以，XV6的trap机制不会查看这些寄存器，而只是将它们保存起来</font>

### <font style="color:rgb(35, 38, 59);">Trap代码执行流程</font>

<font style="color:rgb(35, 38, 59);">用户程序执行系统调用函数（实际上通过执行</font>**<font style="color:rgb(35, 38, 59);">ECALL</font>**<font style="color:rgb(35, 38, 59);">指令来执行系统调用）</font>

<font style="color:rgb(35, 38, 59);">用户程序 → ECALL → uservec（在trampoline中） → usertrap（在trap.c中） → syscall → sys\_xxx（对应的系统调用） →执行结果返回给syscall → usertrapret（在trap.c中） → userret（在trampoline中） → 系统调用完成，返回到用户空间，恢复ECALL之后的用户程序的执行</font>

![1733301853107-1d401da8-c301-4fd0-be3b-4ca9f9f813b1.png](./img/Vv9wfwnBoqDA-rb4/1733301853107-1d401da8-c301-4fd0-be3b-4ca9f9f813b1-975355.webp)

#### <font style="color:rgb(35, 38, 59);">ECALL</font>

<font style="color:rgb(35, 38, 59);">以shell调用write为例。作为用户代码的Shell调用write时，实际上调用的是关联到Shell的一个库函数，在</font><code><font style="color:rgb(35, 38, 59);">usys.s</font></code><font style="color:rgb(35, 38, 59);">，首先将SYS\_write加载到a7寄存器（SYS\_write映射常量16，表示第16个系统调用，即</font><code><font style="color:rgb(35, 38, 59);">write</font></code><font style="color:rgb(35, 38, 59);">），之后这个函数中执行ecall指令（此时进入了supervisor mode）</font>

<font style="color:rgb(35, 38, 59);">ecall指令会让程序计数器跳转到用户空间顶部</font>**<font style="color:rgb(35, 38, 59);">trampoline page</font>**<font style="color:rgb(35, 38, 59);">的位置，所以现在指令正运行在内存的trampoline page中，这个page包含了内核的trap处理代码。</font>**<font style="color:rgb(35, 38, 59);">ecall并不会切换page table</font>**<font style="color:rgb(35, 38, 59);">，这是ecall指令的一个非常重要的特点。所以这意味着，trap处理代码必须存在于每一个user page table中。因为ecall并不会切换page table，我们需要在user page table中的某个地方来执行最初的内核代码。而这个trampoline page，是由内核小心的映射到每一个user page table中，以使得当我们仍然在使用user page table时，内核在一个地方能够执行trap机制的最开始的一些指令</font>

> <font style="color:rgb(35, 38, 59);">所有进程的 trampoline page 都映射到同一个物理地址</font>

<font style="color:rgb(35, 38, 59);">这里的控制是通过STVEC寄存器完成的，这是一个只能在supervisor mode下读写的特权寄存器。在从用户空间进入到内核空间之前，内核会设置好STVEC寄存器指向内核希望trap代码运行的位置，这就是trampoline page的起始位置。STVEC寄存器的内容，就是在ecall指令执行之后，我们会在这个特定地址执行指令的原因。</font>

<font style="color:rgb(35, 38, 59);">即使trampoline page是在用户地址空间的user page table完成的映射，但用户代码并不能写它，因为这些page对应的PTE并没有设置PTE\_U标志位。</font>

<font style="color:rgb(35, 38, 59);">我们是通过ecall走到trampoline page的，而ecall实际上只会改变三件事情：</font>

<font style="color:rgb(35, 38, 59);">第一，ecall将代码从user mode改到supervisor mode</font>

<font style="color:rgb(35, 38, 59);">第二，ecall将程序计数器的值保存在了SEPC寄存器</font>

<font style="color:rgb(35, 38, 59);">第三，ecall会将STVEC拷贝到程序计数器，也就是程序开始执行STVEC所指向的代码，即trampoline page的位置</font>

#### <font style="color:rgb(35, 38, 59);">uservec函数</font>

<font style="color:rgb(35, 38, 59);">现在程序位于trampoline page的起始，也是uservec函数的起始，现在需要做的第一件事情就是保存寄存器的内容。在一些其他的机器中或许直接就将32个寄存器中的内容写到物理内存中某些合适的位置。但是不能在RISC-V中这样做，因为在RISC-V中，supervisor mode下的代码不允许直接访问物理内存，所以只能使用page table中的内容。</font>

<font style="color:rgb(35, 38, 59);">对于保存用户寄存器，XV6在RISC-V上的实现包括了两个部分。第一个部分是，XV6在每个user page table映射了trapframe page，这样每个进程都有自己的trapframe page。这个page包含了很多有趣的数据，但是现在最重要的是用来保存用户寄存器的32个空槽位。所以，在trap处理代码中，user page table有一个之前由kernel设置好的映射关系，这个映射关系指向了一个可以用来存放这个进程的用户寄存器的内存位置。所以，保存用户寄存器之前，内核已经将trapframe page映射到了每个user page table。</font>

<font style="color:rgb(35, 38, 59);">另一部分在于SSCRATCH寄存器。在进入到user space之前，内核会将trapframe page的地址保存在这个寄存器中。更重要的是，RISC-V有一个指令允许交换任意两个寄存器的值。而SSCRATCH寄存器的作用就是保存另一个寄存器的值，并将自己的值加载给另一个寄存器。</font>

<font style="color:rgb(35, 38, 59);">trampoline.S代码要做的第一件事就是执行csrrw指令，这个指令交换了a0和sscratch两个寄存器的内容。这样，a0寄存器保存的就是trapframe page的地址，sscratch寄存器保存的就是原本a0寄存器的值。这样就可以通过a0寄存器指向的trapframe page的地址保存其他用户寄存器的数据。trampoline.S中的代码如下：</font>

```plain
    		# save the user a0 in p->trapframe->a0
        csrr t0, sscratch
        sd t0, 112(a0)

        # restore kernel stack pointer from p->trapframe->kernel_sp
        ld sp, 8(a0)

        # make tp hold the current hartid, from p->trapframe->kernel_hartid
        ld tp, 32(a0)

        # load the address of usertrap(), p->trapframe->kernel_trap
        ld t0, 16(a0)

        # restore kernel page table from p->trapframe->kernel_satp
        ld t1, 0(a0)
        csrw satp, t1
        sfence.vma zero, zero

        # a0 is no longer valid, since the kernel page
        # table does not specially map p->tf.

        # jump to usertrap(), which does not return
        jr t0
```

<font style="color:rgb(35, 38, 59);">保存其他寄存器的数据后，还需要保存a0寄存器原本的数据（此时是存在sscratch寄存器中）。在汇编代码中，先执行了</font><code><font style="color:rgb(35, 38, 59);">csrr t0, sscratch</font></code><font style="color:rgb(35, 38, 59);">，从sscratch中读取值，并存储到t0寄存器中。然后执行</font><code><font style="color:rgb(35, 38, 59);">sd t0, 112(a0)</font></code><font style="color:rgb(35, 38, 59);">，将t0寄存器的值存储到a0寄存器指向的内存地址的偏移量为112的位置（此时a0指向的是trapframe page）。这样trapframe page中就备份好了所有寄存器的数据。</font>

<font style="color:rgb(35, 38, 59);">下一句指令是</font><code><font style="color:rgb(35, 38, 59);">ld sp, 8(a0)</font></code><font style="color:rgb(35, 38, 59);">，这条指令将a0指向的内存地址往后数的第8个字节开始的数据加载到Stack Pointer寄存器（反复强调一下，此时a0指向的是trapframe page）。第8个字节开始的数据是内核的Stack Pointer（kernel\_sp）。trapframe中的kernel\_sp是由kernel在进入用户空间之前就设置好的，它的值是这个进程的kernel stack。所以这条指令的作用是初始化Stack Pointer指向这个进程的kernel stack的最顶端。</font>

<font style="color:rgb(35, 38, 59);">下一条指令是</font><code><font style="color:rgb(35, 38, 59);">ld tp, 32(a0)</font></code><font style="color:rgb(35, 38, 59);">，向tp寄存器写入数据。在RISC-V中，没有一个直接的方法来确认当前运行在多核处理器的哪个核上，XV6会将CPU核的编号也就是hartid保存在tp寄存器。</font>

<font style="color:rgb(35, 38, 59);">下一条指令是</font><code><font style="color:rgb(35, 38, 59);">ld t0, 16(a0)</font></code><font style="color:rgb(35, 38, 59);">，向t0寄存器写入数据。这里写入的是将要执行的第一个函数的指针，也就是函数usertrap的指针，在后面会使用这个指针。</font>

<font style="color:rgb(35, 38, 59);">下一条指令是</font><code><font style="color:rgb(35, 38, 59);">ld t1, 0(a0)</font></code><font style="color:rgb(35, 38, 59);">，向t1寄存器写入数据，写入的是kernel page table的地址。实际上严格来说，t1的内容并不是kernel page table的地址，是需要向SATP寄存器写入的数据。它包含了kernel page table的地址，但是移位了，并且包含了各种标志位。下一条指令是交换SATP和t1寄存器。这条指令执行完成之后，当前程序会从user page table切换到kernel page table。</font>

<font style="color:rgb(35, 38, 59);">现在，Stack Pointer指向了kernel stack；有了kernel page table，可以读取kernel data，已经准备好了执行内核中的代码了。</font>

<font style="color:rgb(35, 38, 59);">最后一条指令是</font><code><font style="color:rgb(35, 38, 59);">jr t0</font></code><font style="color:rgb(35, 38, 59);">。执行了这条指令，就要从trampoline跳到内核的代码中。这条指令的作用是跳转到t0指向的函数中，即usertrap函数。</font>

#### <font style="color:rgb(35, 38, 59);">usertrap函数</font>

```c
// kernel/trap.c
void
usertrap(void)
{
  int which_dev = 0;

  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();
  
  // save user program counter.
  p->trapframe->epc = r_sepc();
  
  if(r_scause() == 8){
    // system call

    if(p->killed)
      exit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sstatus &c registers,
    // so don't enable until done with those registers.
    intr_on();

    syscall();
  } else if((which_dev = devintr()) != 0){
    // ok
  }else {
      printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    p->killed = 1;
  }

  if(p->killed)
    exit(-1);

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();

  usertrapret();
}
```

<font style="color:rgb(35, 38, 59);">usertrap函数位于trap.c。有很多原因会导致程序运行进入到usertrap函数中来，比如系统调用，运算时除以0，使用了一个未被映射的虚拟地址，或者是设备中断。usertrap某种程度上存储并恢复硬件状态，但是它也需要检查触发trap的原因，以确定相应的处理方式。</font>

<font style="color:rgb(35, 38, 59);">usertrap做的第一件事情是更改STVEC寄存器。取决于trap是来自于用户空间还是内核空间，实际上XV6处理trap的方法是不一样的。目前为止，我们只讨论过当trap是由用户空间发起时会发生什么。如果trap从内核空间发起，将会是一个非常不同的处理流程，因为从内核发起的话，程序已经在使用kernel page table。所以当trap发生时，程序执行仍然在内核的话，很多处理都不必存在。</font>

<font style="color:rgb(35, 38, 59);">在内核中执行任何操作之前，usertrap中先将STVEC指向了kernelvec变量，这是内核空间trap处理代码的位置，而不是用户空间trap处理代码的位置。</font>

<font style="color:rgb(35, 38, 59);">我们需要知道当前运行的是什么进程，可以通过调用myproc函数来做到这一点。myproc函数会查找一个根据当前CPU核的编号索引的数组，CPU核的编号是hartid，这是myproc函数找出当前运行进程的方法。</font>

<font style="color:rgb(35, 38, 59);">接下来要保存用户程序计数器，它虽然已经保存在SEPC寄存器中，但是可能发生这种情况：当程序还在内核中执行时，由于进程调度可能切换到另一个进程，并进入到那个程序的用户空间，然后那个进程可能再调用一个系统调用进而导致SEPC寄存器的内容被覆盖。所以，我们需要保存当前进程的SEPC寄存器到一个与该进程关联的内存中，这样这个数据才不会被覆盖。这里使用trapframe来保存这个程序计数器：</font><code><font style="color:rgb(35, 38, 59);">p->trapframe->epc = r_sepc();</font></code>

<font style="color:rgb(35, 38, 59);">接下来需要找出现在会在usertrap函数的原因： </font><code><font style="color:rgb(35, 38, 59);">if(r_scause() == 8)</font></code><font style="color:rgb(35, 38, 59);">。根据触发trap的原因，RISC-V的SCAUSE寄存器会有不同的数字（比如8表示系统调用）。接下来检查是不是有其他的进程杀掉了当前进程：</font><code><font style="color:rgb(35, 38, 59);">if(p->killed)</font></code>

<code><font style="color:rgb(35, 38, 59);">p->trapframe->epc += 4;</font></code><font style="color:rgb(35, 38, 59);">在RISC-V中，存储在SEPC寄存器中的程序计数器，是用户程序中触发trap的指令的地址。但是恢复用户程序时，我们希望在下一条指令恢复，也就是ecall之后的一条指令。所以对于系统调用，对于保存的用户程序计数器加4，这样会在ecall的下一条指令恢复，而不是重新执行ecall指令。</font>

<code><font style="color:rgb(35, 38, 59);">intr_on();</font></code><font style="color:rgb(35, 38, 59);">XV6会在处理系统调用的时候使能中断，这样中断可以更快的服务，有些系统调用需要许多时间处理。中断总是会被RISC-V的trap硬件关闭，所以在这个时间点，需要显式的打开中断</font>

<font style="color:rgb(35, 38, 59);"> </font><code><font style="color:rgb(35, 38, 59);">syscall();</font></code><font style="color:rgb(35, 38, 59);">调用syscall函数，从syscall表单中，根据系统调用的编号查找相应的系统调用函数，syscall函数的工作就是获取由trampoline代码保存在trapframe中a7的数字，然后用这个数字索引实现了每个系统调用的表单。系统调用需要找到它们的参数，通过trapframe来获取这些参数。所有的系统调用都有一个返回值，比如write会返回实际写入的字节数，而RISC-V上的C代码的习惯是函数的返回值存储于寄存器a0，所以为了模拟函数的返回，这里将返回值存储在trapframe的a0中。之后，当返回到用户空间，trapframe中的a0槽位的数值会写到实际的a0寄存器，Shell会认为a0寄存器中的数值是write系统调用的返回值。</font>

<font style="color:rgb(35, 38, 59);"> </font><code><font style="color:rgb(35, 38, 59);">if(p->killed)</font></code><font style="color:rgb(35, 38, 59);">再次检查当前用户进程是否被杀掉了。</font>

<font style="color:rgb(35, 38, 59);"> 最后，usertrap调用usertrapret函数。</font>

#### <font style="color:rgb(35, 38, 59);">usertrapret函数</font>

```c
// kernel/trap.c
void
usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();

  // send syscalls, interrupts, and exceptions to trampoline.S
  w_stvec(TRAMPOLINE + (uservec - trampoline));

  // set up trapframe values that uservec will need when
  // the process next re-enters the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);

  // tell trampoline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  uint64 fn = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64,uint64))fn)(TRAPFRAME, satp);
}
```

<font style="color:rgb(35, 38, 59);">usertrapret函数的内容是在返回到用户空间之前内核要做的工作。</font>

<font style="color:rgb(35, 38, 59);"> </font><code><font style="color:rgb(35, 38, 59);">intr_off();</font></code><font style="color:rgb(35, 38, 59);">首先关闭了中断。之前在系统调用的过程中打开了中断，这里关闭中断是因为要更新STVEC寄存器来指向用户空间的trap处理代码，而之前在内核中的时候，我们指向的是内核空间的trap处理代码。将STVEC更新到指向用户空间的trap处理代码时，我们仍然在内核中执行代码。如果这时发生了一个中断，那么程序执行会走向用户空间的trap处理代码，即便现在仍然在内核中，出于各种各样具体细节的原因，这会导致内核出错。所以这里关闭中断。</font>

<code><font style="color:rgb(35, 38, 59);">w_stvec(TRAMPOLINE + (uservec - trampoline));</font></code><font style="color:rgb(35, 38, 59);">设置了STVEC寄存器指向trampoline代码，在那里最终会执行sret指令返回到用户空间。位于trampoline代码最后的sret指令会重新打开中断。这样，即使刚刚关闭了中断，在执行用户代码时中断也是打开的。</font>

<font style="color:rgb(35, 38, 59);">接下来的几行填入了trapframe的内容，这些内容对于执行trampoline代码非常有用：</font>

* <font style="color:rgb(35, 38, 59);">存储了kernel page table的指针</font>
* <font style="color:rgb(35, 38, 59);">存储了当前用户进程的kernel stack</font>
* <font style="color:rgb(35, 38, 59);">存储了usertrap函数的指针，这样trampoline代码才能跳转到这个函数</font>
* <font style="color:rgb(35, 38, 59);">从tp寄存器中读取当前的CPU核编号，并存储在trapframe中，这样trampoline代码才能恢复这个数字，因为用户代码可能会修改这个数字</font>

<font style="color:rgb(35, 38, 59);">设置好trapframe中的这些数据，这样下一次从用户空间转换到内核空间时可以用到这些数据。</font>

<font style="color:rgb(35, 38, 59);">接下来要设置SSTATUS寄存器，这是一个控制寄存器。这个寄存器的SPP bit位控制了sret指令的行为，该bit为0表示下次执行sret的时候想要返回user mode而不是supervisor mode。这个寄存器的SPIE bit位控制了，在执行完sret之后，是否打开中断。因为在返回到用户空间之后，我们的确希望打开中断，所以这里将SPIE bit位设置为1。修改完这些bit位之后会把新的值写回到SSTATUS寄存器。</font>

<code><font style="color:rgb(35, 38, 59);">w_sepc(p->trapframe->epc);</font></code><font style="color:rgb(35, 38, 59);">在trampoline代码的最后执行了sret指令，这条指令会将程序计数器设置成SEPC寄存器的值，所以现在将SEPC寄存器的值设置成之前保存的用户程序计数器的值。之前已在usertrap函数中将用户程序计数器保存在trapframe中的epc字段。</font>

<font style="color:rgb(35, 38, 59);"> </font><code><font style="color:rgb(35, 38, 59);">uint64 satp = MAKE_SATP(p->pagetable);</font></code><font style="color:rgb(35, 38, 59);">根据user page table地址生成相应的SATP值，这样在返回到用户空间的时候才能完成page table的切换。实际上，我们会在汇编代码trampoline中完成page table的切换，并且也只能在trampoline中完成切换，因为只有trampoline中代码是同时在用户和内核空间中映射。但是现在还没有在trampoline代码中，现在还在一个普通的C函数中，所以这里先将page table指针准备好，并将这个指针作为第二个参数传递给汇编代码，这个参数会出现在a1寄存器。</font>

<font style="color:rgb(35, 38, 59);"> </font><code><font style="color:rgb(35, 38, 59);">uint64 fn = TRAMPOLINE + (userret - trampoline);</font></code><font style="color:rgb(35, 38, 59);">计算出将要跳转的汇编代码的地址。我们期望跳转的地址是tampoline中的userret函数，这个函数包含了所有回到用户空间的指令。</font>

<font style="color:rgb(35, 38, 59);"> </font><code><font style="color:rgb(35, 38, 59);">((void (*)(uint64,uint64))fn)(TRAPFRAME, satp);</font></code><font style="color:rgb(35, 38, 59);">将fn指针作为一个函数指针，执行相应的函数（也就是userret函数）并传入两个参数，两个参数存储在a0，a1寄存器中。</font>

#### <font style="color:rgb(35, 38, 59);">userret函数</font>

<font style="color:rgb(35, 38, 59);">现在程序执行又到了trampoline代码。</font>

```plain
.globl userret
userret:
        # userret(TRAPFRAME, pagetable)
        # switch from kernel to user.
        # usertrapret() calls here.
        # a0: TRAPFRAME, in user page table.
        # a1: user page table, for satp.

        # switch to the user page table.
        csrw satp, a1
        sfence.vma zero, zero

        # put the saved user a0 in sscratch, so we
        # can swap it with our a0 (TRAPFRAME) in the last step.
        ld t0, 112(a0)
        csrw sscratch, t0

        # restore all but a0 from TRAPFRAME
        ld ra, 40(a0)
        ld sp, 48(a0)
        ......
        
        # restore user a0, and save TRAPFRAME in sscratch
        csrrw a0, sscratch, a0
        
        # return to user mode and user pc.
        # usertrapret() set up sstatus and sepc.
        sret
```

<font style="color:rgb(35, 38, 59);"> </font><code><font style="color:rgb(35, 38, 59);">csrw satp, a1</font></code><font style="color:rgb(35, 38, 59);">：第一步是切换page table。在执行这条指令之前，page table还是巨大的kernel page table。这条指令会将user page table（在usertrapret中作为第二个参数传递给了这里的userret函数，所以存在a1寄存器中）存储在SATP寄存器中。执行完这条指令之后，page table就变成了小得多的user page table。user page table也映射了trampoline page，所以程序还能继续执行而不是崩溃。（sfence.vma是清空页表缓存）</font>

<font style="color:rgb(35, 38, 59);"> </font><code><font style="color:rgb(35, 38, 59);">ld t0, 112(a0)</font></code><font style="color:rgb(35, 38, 59);">：在uservec函数中，第一件事情就是交换SSRATCH和a0寄存器。而这里，先将SSCRATCH寄存器恢复成保存好的用户的a0寄存器。在这里a0是trapframe的地址，因为C代码usertrapret函数中将trapframe地址作为第一个参数传递过来了。112是a0寄存器在trapframe中的位置。（就是通过当前的a0寄存器找出存在trapframe中的a0寄存器）先将这个地址里的数值保存在t0寄存器中，之后再将t0寄存器的数值保存在SSCRATCH寄存器中。</font>

> <font style="color:rgb(35, 38, 59);">注：现在trapframe中的a0寄存器是执行系统调用的返回值。系统调用的返回值覆盖了保存在trapframe中的a0寄存器的值。我们希望用户程序在a0寄存器中看到系统调用的返回值。所以，现在SSCRATCH寄存器中也是系统调用的返回值</font>

<font style="color:rgb(35, 38, 59);">到目前为止，所有的寄存器内容还是属于内核。接下来的指令将a0寄存器指向的trapframe中，之前保存的寄存器的值加载到对应的各个寄存器中。恢复好寄存器的数据，a0寄存器现在仍然是指向trapframe的指针，而不是保存了的用户数据（注意和trapframe中的a0寄存器区分开来）。</font>

<font style="color:rgb(35, 38, 59);"> </font><code><font style="color:rgb(35, 38, 59);">csrrw a0, sscratch, a0</font></code><font style="color:rgb(35, 38, 59);">：在返回到用户空间之前，交换SSCRATCH寄存器和a0寄存器的值。SSCRATCH现在的值是系统调用的返回值，a0寄存器是trapframe的地址。交换完成之后，a0持有的是系统调用的返回值，SSCRATCH持有的是trapframe的地址。之后trapframe的地址会一直保存在SSCRATCH中，直到用户程序执行了另一次trap。现在还在kernel mode中。</font>

<code><font style="color:rgb(35, 38, 59);">sret</font></code><font style="color:rgb(35, 38, 59);">是在kernel中的最后一条指令，当执行完这条指令：</font>

* <font style="color:rgb(35, 38, 59);">程序会切换回user mode</font>
* <font style="color:rgb(35, 38, 59);">SEPC寄存器的数值会被拷贝到PC寄存器（程序计数器）</font>
* <font style="color:rgb(35, 38, 59);">重新打开中断</font>

<font style="color:rgb(35, 38, 59);">现在回到了用户空间。</font>

## <font style="color:rgb(35, 38, 59);">实验</font>

### <font style="color:rgb(35, 38, 59);">RISC-V assembly (easy)</font>

这个实验只是回答一些问题，可以通过这些题了解一些汇编代码。

先按题目要求：当前<font style="color:rgb(51, 51, 51);">仓库中有一个文件</font>***<font style="color:rgb(51, 51, 51);">user/call.c</font>***<font style="color:rgb(51, 51, 51);">。执行</font><code><font style="color:rgb(51, 51, 51);background-color:rgb(247, 247, 247);">make fs.img</font></code><font style="color:rgb(51, 51, 51);">编译它，并在</font>***<font style="color:rgb(51, 51, 51);">user/call.asm</font>***<font style="color:rgb(51, 51, 51);">中生成可读的汇编版本。</font>

下面是参考答案。

**<font style="color:rgb(35, 38, 59);">1.哪些寄存器保存函数的参数？例如，在main对printf的调用中，哪个寄存器保存13？</font>**

<font style="color:rgb(35, 38, 59);">在RISC-V架构中，函数参数保存在</font><code>**<font style="color:rgb(216, 59, 100);background-color:rgb(249, 242, 244);">a0</font>**</code><font style="color:rgb(35, 38, 59);">到</font><code>**<font style="color:rgb(216, 59, 100);background-color:rgb(249, 242, 244);">a7</font>**</code><font style="color:rgb(35, 38, 59);">寄存器中。查看调用</font><code>**<font style="color:rgb(216, 59, 100);background-color:rgb(249, 242, 244);">printf</font>**</code><font style="color:rgb(35, 38, 59);">的汇编源码：</font>

```plain
printf("%d %d\n", f(8)+1, 13);
  24:	4635                	li	a2,13
  26:	45b1                	li	a1,12
  28:	00000517          	auipc	a0,0x0
  2c:	7b050513          	addi	a0,a0,1968 # 7d8 <malloc+0xea>
  30:	00000097          	auipc	ra,0x0
  34:	600080e7          	jalr	1536(ra) # 630 <printf>
```

<font style="color:rgb(35, 38, 59);">可知13保存在</font><code>**<font style="color:rgb(216, 59, 100);background-color:rgb(249, 242, 244);">a2</font>**</code><font style="color:rgb(35, 38, 59);">寄存器中。</font>

<font style="color:rgb(35, 38, 59);"></font>

**<font style="color:rgb(35, 38, 59);">2.main的汇编代码中对函数f的调用在哪里？对g的调用在哪里(提示：编译器可能会将函数内联（inline））</font>**

<font style="color:rgb(35, 38, 59);">没有调用f函数对应的汇编源码，因为g(x) 被内联到 f(x) 中，然后 f(x) 又被进一步内联到 main() 中。</font>

<font style="color:rgb(35, 38, 59);"></font>

**<font style="color:rgb(35, 38, 59);">3.printf函数位于哪个地址？</font>**

<font style="color:rgb(35, 38, 59);">汇编源码对应部分：</font>

```plain
30:	00000097          	auipc	ra,0x0
34:	600080e7          	jalr	1536(ra) # 630 <printf>
```

<code><font style="color:rgb(35, 38, 59);">auipc</font></code><font style="color:rgb(35, 38, 59);">(Add Upper Immediate to PC)得到pc的值</font>**<font style="color:rgb(35, 38, 59);">0x30</font>**<font style="color:rgb(35, 38, 59);">存储在</font><code><font style="color:rgb(35, 38, 59);">ra</font></code><font style="color:rgb(35, 38, 59);">寄存器，</font><code><font style="color:rgb(35, 38, 59);">jalr</font></code><font style="color:rgb(35, 38, 59);">(jump and link register)指令跳转到</font><code><font style="color:rgb(35, 38, 59);">ra</font></code><font style="color:rgb(35, 38, 59);">寄存器的值并加上偏移量1536（即</font>**<font style="color:rgb(35, 38, 59);">0x600</font>**<font style="color:rgb(35, 38, 59);">），所以</font><code><font style="color:rgb(35, 38, 59);">printf</font></code><font style="color:rgb(35, 38, 59);">的地址是</font>**<font style="color:rgb(35, 38, 59);">0x630</font>**

**<font style="color:rgb(35, 38, 59);">4.在main中printf的jalr之后的寄存器ra中有什么值？</font>**

<code><font style="color:rgb(35, 38, 59);">auipc</font></code><font style="color:rgb(35, 38, 59);">和</font><code><font style="color:rgb(35, 38, 59);">jalr</font></code><font style="color:rgb(35, 38, 59);">指令用于生成基于程序计数器（</font><code><font style="color:rgb(35, 38, 59);">pc</font></code><font style="color:rgb(35, 38, 59);">）的地址和跳转，用于计算相对地址。</font>

<code><font style="color:rgb(35, 38, 59);">auipc</font></code><font style="color:rgb(35, 38, 59);">指令格式：</font><code><font style="color:rgb(35, 38, 59);">auipc rd, imm</font></code><font style="color:rgb(35, 38, 59);">。将20位的立即数</font><code><font style="color:rgb(35, 38, 59);">imm</font></code><font style="color:rgb(35, 38, 59);">左移12位后加上当前</font><code><font style="color:rgb(35, 38, 59);">pc</font></code><font style="color:rgb(35, 38, 59);">值，结果存入</font><code><font style="color:rgb(35, 38, 59);">rd</font></code><font style="color:rgb(35, 38, 59);">寄存器。</font><code><font style="color:rgb(35, 38, 59);">rd = PC + (imm << 12)</font></code>

<code><font style="color:rgb(35, 38, 59);">jalr</font></code><font style="color:rgb(35, 38, 59);">指令格式：</font><code><font style="color:rgb(35, 38, 59);">jalr rd, offset(rs1) </font></code><font style="color:rgb(35, 38, 59);">。跳转到</font><code><font style="color:rgb(35, 38, 59);">rs1</font></code><font style="color:rgb(35, 38, 59);">寄存器中的地址加上偏移</font><code><font style="color:rgb(35, 38, 59);">offset</font></code><font style="color:rgb(35, 38, 59);">的位置，将跳转的下一条指令的地址存储在</font><code><font style="color:rgb(35, 38, 59);">rd</font></code><font style="color:rgb(35, 38, 59);">寄存器中。</font>

<font style="color:rgb(35, 38, 59);">回到</font><code><font style="color:rgb(35, 38, 59);">printf</font></code><font style="color:rgb(35, 38, 59);">汇编源码：</font>

```plain
  30:	00000097          	auipc	ra,0x0
  34:	600080e7          	jalr	1536(ra) # 630 <printf>
```

<font style="color:rgb(35, 38, 59);">第1行代码：对比指令格式，这行代码将0x0左移12位（还是0x0）加到pc（当前为0x30）并存入ra中，即ra中保存的是0x30</font>

<font style="color:rgb(35, 38, 59);">第2行代码：这对比jalr的标准格式有所不同，可能是此两处使用寄存器相同时，汇编中可以省略</font><code><font style="color:rgb(35, 38, 59);">rd</font></code><font style="color:rgb(35, 38, 59);">部分。ra中保存的是0x30，加上0x600后为0x630，即</font><code><font style="color:rgb(35, 38, 59);">printf</font></code><font style="color:rgb(35, 38, 59);">的地址，执行此行代码后，将跳转到printf函数执行，并将当前pc+4=0X34+0X4=0X38保存到</font><code><font style="color:rgb(35, 38, 59);">ra</font></code><font style="color:rgb(35, 38, 59);">中，供之后返回使用。</font>

**<font style="color:rgb(35, 38, 59);">5.运行以下代码，程序的输出是什么？这是将字节映射到字符的ASCII码表。输出取决于RISC-V小端存储的事实。如果RISC-V是大端存储，为了得到相同的输出，你会把</font>**<code>**<font style="color:rgb(35, 38, 59);">i</font>**</code>**<font style="color:rgb(35, 38, 59);">设置成什么？是否需要将</font>**<code>**<font style="color:rgb(35, 38, 59);">57616</font>**</code>**<font style="color:rgb(35, 38, 59);">更改为其他值？</font>**

```c
unsigned int i = 0x00646c72;
printf("H%x Wo%s", 57616, &i);
```

<font style="color:rgb(35, 38, 59);">57616=0xE110，0x00646c72小端存储为72-6c-64-00，对照ASCII码表：72:r 6c:l 64:d 00:充当字符串结尾标识</font>

<font style="color:rgb(35, 38, 59);">因此输出为：HE110 World</font>

<font style="color:rgb(35, 38, 59);">若为大端存储，i应改为0x726c6400，不需改变57616</font>

<font style="color:rgb(35, 38, 59);"></font>

**<font style="color:rgb(35, 38, 59);">6.在下面的代码中，“</font>**<code>**<font style="color:rgb(35, 38, 59);">y=</font>**</code>**<font style="color:rgb(35, 38, 59);">”之后将打印什么(注：答案不是一个特定的值）？为什么会发生这种情况？</font>**

```c
printf("x=%d y=%d", 3);
```

<font style="color:rgb(35, 38, 59);">原本需要两个参数，却只传入了一个，因此y=后面打印的结果取决于之前a2中保存的数据。</font>

<font style="color:rgb(35, 38, 59);"></font>

### <font style="color:rgb(35, 38, 59);">Backtrace(moderate)</font>

<font style="color:rgb(35, 38, 59);">该实验要实现打印曾经调用过的函数的地址。打印出调用栈，用于调试。</font>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">先添加函数获取当前函数的fp（frame pointer）：</font>

```c
// kernel/riscv.h
static inline uint64
r_fp(){
  uint64 x;
  asm volatile("mv %0, s0" : "=r" (x));
  return x;
}
```

<font style="color:rgb(35, 38, 59);"> fp 指向当前栈帧的开始地址，sp 指向当前栈帧的结束地址。 （栈从高地址往低地址生长，所以 fp 虽然是帧开始地址，但是地址比 sp 高）</font>\ <font style="color:rgb(35, 38, 59);">	栈帧中从高到低第一个 8 字节 </font><code><font style="color:rgb(35, 38, 59);">fp-8</font></code><font style="color:rgb(35, 38, 59);"> 是 return address，也就是当前调用层应该返回到的地址。</font>\ <font style="color:rgb(35, 38, 59);">	栈帧中从高到低第二个 8 字节 </font><code><font style="color:rgb(35, 38, 59);">fp-16</font></code><font style="color:rgb(35, 38, 59);"> 是 previous address，指向上一层栈帧的 fp 开始地址。</font>\ <font style="color:rgb(35, 38, 59);">	剩下的为保存的寄存器、局部变量等。一个栈帧的大小不固定，但是至少 16 字节。</font>\ <font style="color:rgb(35, 38, 59);">	在 xv6 中，</font>**<font style="color:rgb(35, 38, 59);">使用一个页来存储栈</font>**<font style="color:rgb(35, 38, 59);">，如果 fp 不在页的有效范围内，说明遍历完了栈帧。</font>

![1733485550898-4eb5402f-9c29-439e-a6f9-f1ca1ff10204.png](./img/Vv9wfwnBoqDA-rb4/1733485550898-4eb5402f-9c29-439e-a6f9-f1ca1ff10204-110155.png)

查看 call.asm，可以看到，一个函数的函数体最开始首先会扩充一个栈帧给该层调用使用，在函数执行完毕后再回收：

```c
int g(int x) {
   0:	1141                  addi  sp,sp,-16  // 扩张调用栈，得到一个 16 字节的栈帧
   2:	e422                  sd    s0,8(sp)   // 将返回地址存到栈帧的第一个 8 字节中
   4:	0800                  addi  s0,sp,16
  return x+3;
}
   6:	250d                  addiw a0,a0,3
   8:	6422                  ld    s0,8(sp)   // 从栈帧读出返回地址
   a:	0141                  addi  sp,sp,16   // 回收栈帧
   c:	8082                  ret              // 返回
```

<font style="color:rgb(35, 38, 59);">栈的生长方向是从高地址到低地址，所以扩张是 -16，而回收是 +16</font>

<font style="color:rgb(35, 38, 59);">据此实现</font><code><font style="color:rgb(35, 38, 59);">backtrace</font></code><font style="color:rgb(35, 38, 59);">函数：</font>

```c
// kernel/printf.c
// 遍历帧指针打印函数地址
void kama_backtrace() {
    uint64 fp = r_fp();
    printf("backtrace:\n");
    while (PGROUNDDOWN(fp) != PGROUNDUP(fp)) {          //当前帧指针fp是否在有效的页范围内
        uint64 ra = *(uint64*)(fp - 8); // return address
        printf("%p\n", ra);
        fp = *(uint64*)(fp - 16); // previous fp
  }
}
```

<font style="color:rgb(35, 38, 59);">需要在头文件</font><code><font style="color:rgb(35, 38, 59);">defs.h</font></code><font style="color:rgb(35, 38, 59);">中声明这个函数：</font>

```c
// kernel/defs.h
// printf.c
void            printf(char*, ...);
void            panic(char*) __attribute__((noreturn));
void            printfinit(void);
void            kama_backtrace();		//加上
```

<font style="color:rgb(35, 38, 59);">在</font><code><font style="color:rgb(35, 38, 59);">kernel/sysproc.c</font></code><font style="color:rgb(35, 38, 59);">的</font><code><font style="color:rgb(35, 38, 59);">sys_sleep</font></code><font style="color:rgb(35, 38, 59);">中调用</font><code><font style="color:rgb(35, 38, 59);">backtrace</font></code><font style="color:rgb(35, 38, 59);">：</font>

```c
uint64
sys_sleep(void)
{
    int n;
    uint ticks0;

    kama_backtrace();			//加上
    
    if (argint(0, &n) < 0)
        return -1;
    ......
}
```

<font style="color:rgb(35, 38, 59);">至此可以验证实验是否通过。</font>

### <font style="color:rgb(35, 38, 59);">Alarm(Hard)</font>

<font style="color:rgb(35, 38, 59);">该实验需要实现</font><code><font style="color:rgb(35, 38, 59);">sigalarm</font></code><font style="color:rgb(35, 38, 59);"> 和 </font><code><font style="color:rgb(35, 38, 59);">sigreturn</font></code><font style="color:rgb(35, 38, 59);">两个系统调用， 为用户进程添加定期通知功能，使得进程在一段时间内使用 CPU 后，会被定期“提醒”， 类似于一种</font>**用户态的中断处理**<font style="color:rgb(35, 38, 59);">，用来模拟用户级的异常处理。 </font>

<font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;"></font>

<code><font style="color:rgb(35, 38, 59);">sigalarm</font></code><font style="color:rgb(35, 38, 59);"> 和 </font><code><font style="color:rgb(35, 38, 59);">sigreturn</font></code><font style="color:rgb(35, 38, 59);">都是系统调用。这里把alarm相关的字段添加到进程的结构体中：</font>

* <font style="color:rgb(35, 38, 59);">kama\_alarm\_interval：时钟周期，0 为禁用</font>
* <font style="color:rgb(35, 38, 59);">kama\_alarm\_handler：时钟回调处理函数</font>
* <font style="color:rgb(35, 38, 59);">kama\_alarm\_ticks：下一次时钟响起前还剩下的 ticks 数</font>
* <font style="color:rgb(35, 38, 59);">kama\_alarm\_trapframe：时钟中断时刻的 陷阱帧，用于中断处理完成后恢复原程序的正常执行</font>
* <font style="color:rgb(35, 38, 59);">kama\_alarm\_goingoff：是否已经有一个时钟回调正在执行且还未返回（用于防止在 alarm\_handler 中途闹钟到期再次调用 alarm\_handler，导致 alarm\_trapframe 被覆盖，</font><code><font style="color:rgb(35, 38, 59);">user/alarmtest.c</font></code><font style="color:rgb(35, 38, 59);">中的test2就是测这个的）</font><font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;">
  </font>

```c
// kernel/proc.h
// Per-process state
struct proc {
  ......

  //时钟相关
  int kama_alarm_interval;          //时钟周期，为0时表示禁用时钟
  void(*kama_alarm_handler)();      //时钟回调处理函数
  int kama_alarm_ticks;             //当前时钟信号数(ticks数)
  struct trapframe* kama_alarm_trapframe;    //时钟中断时刻进程的陷阱帧，用于恢复进程中断前的状态
  int kama_alarm_goingoff;          //是否已经有一个时钟中断正在执行且还未返回
};
```

<font style="color:rgb(35, 38, 59);">在进程初始化和释放进程时，也要对这些字段进行初始化和释放：</font>

```c
// kernel/proc.c
static struct proc*		//初始化进程
allocproc(void)
{
  ......

found:
  p->pid = allocpid();

  // Allocate a trapframe page.
  if((p->trapframe = (struct trapframe *)kalloc()) == 0){
    release(&p->lock);
    return 0;
  }

  // 给alarm_trapflame分配陷阱帧
  if((p->kama_alarm_trapframe = (struct trapframe *)kalloc()) == 0){
    release(&p->lock);
    return 0;
  }
    
  // 进程创建时初始化alarm相关
  p->kama_alarm_interval = 0;
  p->kama_alarm_handler = 0;
  p->kama_alarm_ticks = 0;
  p->kama_alarm_goingoff = 0;

  // ......

  return p;
}

static void
freeproc(struct proc *p)		//释放进程中的资源
{
  // ......

  if(p->kama_alarm_trapframe)
    kfree((void*)p->kama_alarm_trapframe);
  p->kama_alarm_trapframe = 0;
  
  // ......
  
  p->kama_alarm_interval = 0;
  p->kama_alarm_handler = 0;
  p->kama_alarm_ticks = 0;
  p->kama_alarm_goingoff = 0;
  p->state = UNUSED;
}

```

<font style="color:rgb(35, 38, 59);">实现</font><code><font style="color:rgb(35, 38, 59);">sigalarm</font></code><font style="color:rgb(35, 38, 59);"> 与 </font><code><font style="color:rgb(35, 38, 59);">sigreturn</font></code><font style="color:rgb(35, 38, 59);">的系统调用，细节可看注释：</font>

```c
// kernel/sysproc.c
uint64 sys_sigalarm(void) {
    int n;          //n个ticks
    uint64 fn;      //时钟回调函数
    if (argint(0, &n) < 0)          //获取第一个参数
        return -1;
    if (argaddr(1, &fn) < 0)        //获取第二个参数
        return -1;

    return kama_sigalarm(n, (void(*)())(fn));        //调用并返回kama_sigalarm函数
}

uint64 sys_sigreturn(void) {
    return kama_sigreturn();
}
```

<font style="color:rgb(35, 38, 59);">在 </font><code><font style="color:rgb(35, 38, 59);">kernel/trap.c</font></code><font style="color:rgb(35, 38, 59);">中实现</font><code><font style="color:rgb(35, 38, 59);">sigalarm</font></code><font style="color:rgb(35, 38, 59);"> 与 </font><code><font style="color:rgb(35, 38, 59);">sigreturn</font></code><font style="color:rgb(35, 38, 59);">的具体代码（要在内核头文件声明）：</font>

```c
// kernel/trap.c
// 设置进程中时钟的相关属性
int kama_sigalarm(int ticks, void(*handler)()) {
    struct proc* p = myproc();
    p->kama_alarm_interval = ticks;
    p->kama_alarm_handler = handler;
    p->kama_alarm_ticks = ticks;
    return 0;
}

//将进程恢复到alarm中断前的状态
int kama_sigreturn() {
    struct proc* p = myproc();
    *p->trapframe = *p->kama_alarm_trapframe;
    p->kama_alarm_goingoff = 0;
    return 0;
}
```

<font style="color:rgb(35, 38, 59);">上面的代码注册好了时钟，接下来实现触发时钟中断时的代码。</font>

<font style="color:rgb(35, 38, 59);">在</font><code><font style="color:rgb(35, 38, 59);">kernel/trap.c</font></code><font style="color:rgb(35, 38, 59);">中的</font><code><font style="color:rgb(35, 38, 59);">usertrap</font></code><font style="color:rgb(35, 38, 59);">实现该时钟中断的代码：</font>

```c
// kernel/trap.c
void
usertrap(void)
{
  ......
  // give up the CPU if this is a timer interrupt.
  if (which_dev == 2) {
      if (p->kama_alarm_interval != 0 && --p->kama_alarm_ticks <= 0 && p->kama_alarm_goingoff == 0) {
      	  // 是否设置了时钟 && 时钟倒计时是否结束 && 没有其他时钟正在运行
          // 如果一个时钟到期的时候已经有一个时钟处理函数正在运行，
          // 则会推迟到原处理函数运行完成后的下一个 tick 才触发这次时钟
          p->kama_alarm_ticks = p->kama_alarm_interval;      // 重置时钟倒计时
          *p->kama_alarm_trapflame = *p->trapframe;          // 保存当前进程陷阱帧
          p->trapframe->epc = (uint64)p->kama_alarm_handler; // 跳转到时钟回调函数
          p->kama_alarm_goingoff = 1;                        // 标记当前已有时钟正在运行
      }
    yield();
  }

  usertrapret();
}
```

到此<code><font style="color:rgb(35, 38, 59);">sigalarm</font></code><font style="color:rgb(35, 38, 59);"> 与 </font><code><font style="color:rgb(35, 38, 59);">sigreturn</font></code><font style="color:rgb(35, 38, 59);">系统调用的功能实现完成了，但是这里只是实现，用户空间还是找不到这个系统调用，需要声明和添加这两个系统调用。这里请大家尝试自己实现声明和添加的步骤，当作复习前面的知识。如果想不起来可以去参考Lab2的内容。</font>

<font style="color:rgb(35, 38, 59);">之后就可以验证该实验以及整个实验是否通过。</font>

## <font style="color:rgb(35, 38, 59);">可扩展的实验</font>

* <font style="color:rgb(51, 51, 51);">在</font><code><font style="background-color:rgb(247, 247, 247);">backtrace()</font></code><font style="color:rgb(51, 51, 51);">中打印函数的名称和行号，而不仅仅是数字化的地址。(hard)</font>


> 更新: 2024-12-07 11:10:17  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/pvm7zr9vpso7mez6>