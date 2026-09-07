# Lab7：多线程

实验难度出自官网，实验简述和实验耗时是主观体验

* thread  `简述`: 实现用户级线程；优化并发程序；实现同步屏障
* `难度`: 3 moderate  `耗时`: 3.5h

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/thread.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><https://xv6.dgs.zone/labs/requirements/lab7.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://blog.miigon.net/posts/s081-lab7-multithreading/>

开始实验前，**<font style="background-color:#FBDE28;">需要切换到本次实验的分支：thread</font>\*\*\*\*。**

## 知识点

<font style="color:rgb(35, 38, 59);">线程具有状态，可以随时保存线程的状态并暂停线程的运行，并在之后通过恢复状态来恢复线程的运行。线程的状态包含了三个部分：</font>

* <font style="color:rgb(35, 38, 59);">程序计数器（Program Counter），它表示当前线程执行指令的位置</font>
* <font style="color:rgb(35, 38, 59);">保存变量的寄存器</font>
* <font style="color:rgb(35, 38, 59);">程序的Stack。每个线程都有属于自己的Stack，Stack记录了函数调用的记录，并反映了当前线程的执行点</font>

<font style="color:rgb(35, 38, 59);">线程会运行在所有可用的CPU核上，每个CPU核会在多个线程之间切换</font>

## 实验

### <font style="color:rgb(35, 38, 59);">Uthread: switching between threads (moderate)</font>

该实验需要完善`user/uthread.c`中的`thread_create()`和`thread_schedule()`，以及`user/uthread_switch.S`中的`thread_switch`，实现创建线程的初始化工作，以及实现线程调度。最好先把测试代码`user/uthread.c`代码读一遍。

> `context` 结构体用于进程间/线程间的上下文切换，切换到不同的进程/线程时，保存进程、线程的最小上下文信息。`context` 只需要保存 callee-saved 寄存器、`sp` 和 `ra`，因为这些寄存器在函数调用之间需要保持一致。当发生进程间/线程切换时，调用者在调用切换函数前已经保存了 caller-saved 寄存器，因此只需保存这些 callee-saved 、ra和sp寄存器即可
>
> 和下面的代码结合来看

<font style="color:rgb(35, 38, 59);">内核已经有scheduler() 和 swtch() 的功能，可以直接参考来写。uthread\_switch.S中实现上下文切换的功能，保存当前线程寄存器，读取新线程的的寄存器。可以看着swtch.S来写：</font>

```plain
// user/uthread_switch.S
	.text

	/*
         * save the old thread's registers,
         * restore the new thread's registers.
         */

	.globl thread_switch
thread_switch:
	sd ra, 0(a0)
	sd sp, 8(a0)
	sd s0, 16(a0)
	sd s1, 24(a0)
	sd s2, 32(a0)
	sd s3, 40(a0)
	sd s4, 48(a0)
	sd s5, 56(a0)
	sd s6, 64(a0)
	sd s7, 72(a0)
	sd s8, 80(a0)
	sd s9, 88(a0)
	sd s10, 96(a0)
	sd s11, 104(a0)

	ld ra, 0(a1)
	ld sp, 8(a1)
	ld s0, 16(a1)
	ld s1, 24(a1)
	ld s2, 32(a1)
	ld s3, 40(a1)
	ld s4, 48(a1)
	ld s5, 56(a1)
	ld s6, 64(a1)
	ld s7, 72(a1)
	ld s8, 80(a1)
	ld s9, 88(a1)
	ld s10, 96(a1)
	ld s11, 104(a1)
	
	ret    /* return to ra */
```

<font style="color:rgb(35, 38, 59);">和进程的context结构体一样，给线程也写一个context结构体：</font>

```c
// user/uthread.c
// 线程切换需要保存的寄存器
struct context {
    uint64 ra;
    uint64 sp;

    // callee-saved
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};
```

在线程结构体中加入context结构体：

```c
// user/uthread.c
struct thread {
    char       stack[STACK_SIZE]; /* the thread's stack */
    int        state;             /* FREE, RUNNING, RUNNABLE */
    struct context context;       // 在线程结构体中添加 context 结构体
};
```

这样就可以在调度函数`thread_schedule`中加入线程切换函数`thread_switch`，在上面`thread_switch`函数的声明中修改传入参数：

```c
// user/uthread.c
extern void thread_switch(struct context* old, struct context* new);

...
    
void 
thread_schedule(void)
{
  ......

  if (current_thread != next_thread) {         /* switch threads?  */
    next_thread->state = RUNNING;
    t = current_thread;
    current_thread = next_thread;
    thread_switch(&t->context, &next_thread->context);		//加上
  } else
    next_thread = 0;
}
```

切换线程的工作就完成了。再来是创建线程时，需要对线程初始化。线程创建函数`thread_create(void (*func)())`，线程创建后，就要执行函数func：

```c
// user/uthread.c
void 
thread_create(void (*func)())
{
  struct thread *t;

  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }
  t->state = RUNNABLE;

  // 返回地址,thread_switch线程切换执行完后返回到ra，设置成线程函数func，就可以切换后执行func
  t->context.ra = (uint64)func;

  // 栈指针，将线程的栈指针指向其独立的栈，栈的生长是从高地址到低地址，所以要将 sp 设置为指向 stack 的最高地址
  t->context.sp = (uint64)&t->stack + (STACK_SIZE - 1);
}
```

至此，可以验证该实验是否通过。

### <font style="color:rgb(35, 38, 59);">Using threads (moderate)</font>

示例中，两个线程插入丢失了很多原本应该插入的键值，这是因为源代码`notxv6/ph.c`中，不同的线程可以同时执行put和get。假如线程1执行put函数，发现key1不存在，要执行insert在一个散列桶插值的时候，切换到了线程2，线程2也是要插入同一个散列桶的同一个位置，完成了key2的插入，切换回了线程1，线程1继续执行insert，导致key2被覆盖。

<font style="color:rgb(35, 38, 59);">因此要在并发写共享数据的地方加锁，首先是声明一个线程锁：</font>

```c
// notxv6/ph.c
pthread_mutex_t lock;
```

<font style="color:rgb(35, 38, 59);">main函数中初始化锁：</font>

```c
// notxv6/ph.c
int
main(int argc, char *argv[])
{
  pthread_t *tha;
  void *value;
  double t1, t0;
  pthread_mutex_init(&lock, NULL);
  ......
```

<font style="color:rgb(35, 38, 59);">在put函数执行的时候加上锁：</font>

```c
// notxv6/ph.c
static 
void put(int key, int value)
{
  pthread_mutex_lock(&lock);		// 获取锁
    
  int i = key % NBUCKET;

  // is the key already present?
  struct entry *e = 0;
  for (e = table[i]; e != 0; e = e->next) {
    if (e->key == key)
      break;
  }
  if(e){
    // update the existing key.
    e->value = value;
  } else {
    // the new is new.
    insert(key, value, &table[i], table[i]);
  }

  pthread_mutex_unlock(&lock);		// 释放锁
}
```

> put相当于写操作，get相当于读操作。在多线程环境中，读写操作一般都是并发进行的，在读操作中也是会发生写操作的，为了确保数据一致性和线程安全，读操作也需要加锁。在这个实验中，main函数里，get操作是在put操作结束后才执行的，即可以保证，get执行的时候不会有线程执行put操作共享数据，所以这里可以不用给get加锁。但为了保持良好的线程编程思想，最好还是加上锁。

这个时候可以通过 `ph_safe`测试：

```bash
$ ./ph 1
100000 puts, 4.855 seconds, 20597 puts/second
0: 0 keys missing
100000 gets, 4.847 seconds, 20629 gets/second
$ ./ph 2
100000 puts, 5.685 seconds, 17591 puts/second
1: 0 keys missing
0: 0 keys missing
200000 gets, 11.184 seconds, 17882 gets/second
```

<font style="color:rgb(35, 38, 59);">从上面的数据也可以看到，多线程已经不会丢失键了，保证了线程安全。但是也可以看到，两个线程的性能比单线的还要低，没有实现多线程提升性能的目的。因为上面的代码中给整个put操作加上了锁，每一个时刻只能有一个线程执行put操作，这和单线程就没什么区别了，加上添加了锁操作，上锁、释放锁、锁竞争都是有开销的，所以会比单线程性能更低。</font>

<font style="color:rgb(35, 38, 59);">多线程提升效率常用的一个做法是降低锁的粒度，减少加锁的范围。观察代码可以知道，不同散列桶的put操作不会互相影响，同一时刻操作不同的散列桶不会造成线程安全问题，所以在这里只要给散列桶加锁，保证不同的线程不会同时操作同一个散列桶就可以了，这样就降低了锁的粒度。</font>\ <font style="color:rgb(35, 38, 59);">修改代码，把线程锁改成散列桶数量大小的锁类型的数组，即给每一个散列桶声明一把锁：</font>

```c
// notxv6/ph.c
pthread_mutex_t lock[NBUCKET];
```

<font style="color:rgb(35, 38, 59);"> 初始化锁：</font>

```c
// notxv6/ph.c
int
main(int argc, char *argv[])
{
  pthread_t *tha;
  void *value;
  double t1, t0;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s nthreads\n", argv[0]);
    exit(-1);
  }
  nthread = atoi(argv[1]);
  tha = malloc(sizeof(pthread_t) * nthread);
  srandom(0);
  assert(NKEYS % nthread == 0);
  for (int i = 0; i < NKEYS; i++) {
    keys[i] = random();
  }

  for (int i = 0;i < NBUCKET;++i)				//在执行put前初始化锁
      pthread_mutex_init(&lock[i], NULL);
    
  ......
```

<font style="color:rgb(35, 38, 59);">在执行put操作的时候，针对散列桶上锁：</font>

```c
static 
void put(int key, int value)
{
  int i = key % NBUCKET;

  pthread_mutex_lock(&lock[i]);

  ......

。、
}
```

<font style="color:rgb(35, 38, 59);">重新编译执行测试：</font>

```bash
$ ./ph 1
100000 puts, 4.944 seconds, 20227 puts/second
0: 0 keys missing
100000 gets, 4.870 seconds, 20532 gets/second
$ ./ph 2
100000 puts, 2.832 seconds, 35310 puts/second
1: 0 keys missing
0: 0 keys missing
200000 gets, 4.529 seconds, 44156 gets/second
$ ./ph 4
100000 puts, 2.018 seconds, 49552 puts/second
0: 0 keys missing
1: 0 keys missing
2: 0 keys missing
3: 0 keys missing
400000 gets, 4.684 seconds, 85399 gets/second
```

从数据可以看出，不仅保证了线程安全，多线程也有了性能上的提升，可以通过`ph_safe`和`ph_fast`测试。

### <font style="color:rgb(35, 38, 59);">Barrier(moderate)</font>

线程调用`barrier`之后，`bstate`中的线程数`nthread`应该+1。然后判断当前进入屏障的线程数是否达到全局的`nthread`，如果未达到，就调用`pthread_cond_wait`，让当前线程睡眠，等待线程数达到要求。如果达到了，需要做的事有：`bstate`中的轮数`round`+1；清空`bstate`的线程数；唤醒其他正在睡眠的线程。

<font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;"></font>

需要考虑的是上锁的位置。可能出现的情况：线程1进入屏障后，`bstate`中的线程数`nthread`+1，但未达到全局`nthread`，线程1正要睡眠，此时线程2进入屏障，达到了全局`nthread`，调用`pthread_cond_wait`唤醒所有线程，之后线程1才进入睡眠，导致线程1没被唤醒。因此在`bstate`中的线程数`nthread`+1，到调用`pthread_cond_wait`进入睡眠这个过程应该上锁。`pthread_cond_wait`函数中会释放当前的锁，避免其他线程拿不到锁<font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;">\ </font><font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;">	</font><font style="color:rgb(35, 38, 59);">据此可以写出代码：</font>

```c
// notxv6/barrier.c
static void 
barrier()
{
  // YOUR CODE HERE
  //
  // Block until all threads have called barrier() and
  // then increment bstate.round.
  //
    pthread_mutex_lock(&bstate.barrier_mutex);
    if (++bstate.nthread < nthread)
        pthread_cond_wait(&bstate.barrier_cond, &bstate.barrier_mutex);
    else {
        bstate.nthread = 0;
        bstate.round++;
        pthread_cond_broadcast(&bstate.barrier_cond);
    }
    pthread_mutex_unlock(&bstate.barrier_mutex);
}
```

至此可以验证整个实验是否通过。

## 可扩展的实验

用户级线程包与操作系统的交互存在多个问题。比如，如果一个用户级线程在系统调用中阻塞，另一个用户级线程将无法运行，因为用户级线程调度器不知道它的线程之一已经被 xv6 的调度器取消调度。再比如，两个用户级线程无法在不同的CPU上并发运行，因为 xv6 的调度器并不知道有多个可以并行运行的线程。需要注意的是，如果两个用户级线程确实并行运行，那么当前实现将无法正常工作，因为存在多个竞争条件（比如，不同处理器上的两个线程可能会同时调用 thread\_schedule，选择相同的可运行线程，并在不同处理器上同时运行它）。

有几种方法可以解决这些问题。一种方法是使用调度器激活，另一种方法是为每个用户级线程使用一个内核线程（Linux 内核采用此方法）。在 xv6 中实现这些方法之一。这并不容易正确实现，比如，在为多线程用户进程更新页表时，你需要实现 TLB 清除。

为线程包添加锁、条件变量、屏障等功能。


> 更新: 2024-12-07 20:24:42  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/tgy5stsevsgzccr9>