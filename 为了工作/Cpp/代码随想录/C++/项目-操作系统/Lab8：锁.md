# Lab8：锁

实验难度出自官网，实验简述和实验耗时是主观体验

* lock`简述`: 分别在物理内存页管理和文件缓存页管理这两个场景中降低锁竞争，提高并发程度进而提升性能
* `难度`: 1 moderate   1 hard    难度仅次于pgtbl  `耗时`: 20h
* `建议`: 在学习课程的时候琢磨这两个问题：1.怎么降低锁竞争？ 2.怎么处理死锁问题？

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/lock.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><https://xv6.dgs.zone/labs/requirements/lab8.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://blog.miigon.net/posts/s081-lab8-locks/>

推荐本实验结合参考博客一起阅读。

开始实验前，**<font style="background-color:#FBDE28;">需要切换到本次实验的分支：lock</font>\*\*\*\*。**

## 实验

改善锁策略<font style="color:rgb(35, 38, 59);">以减少争用，提高并行性，提高性能</font>

### <font style="color:rgb(35, 38, 59);">Memory allocator(moderate)</font>

先看一下原本kalloc中的代码。原本代码中定义了一个结构体`kmem`，将里面的`freelist`字段作为空闲物理页作为链表项，使空闲页形成一个链表。分配物理页就是把`freelist`从链表移除，释放物理页就是把要释放的页连回链表：<font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;"></font>

```c
// kernel/kalloc.c
struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist; // 取出一个物理页。页表项本身就是物理页。
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
```

<font style="color:rgb(35, 38, 59);">分配和释放物理页都是操作共享数据，修改freelist链表，因此为了线程安全，这些操作都加上了锁。这样就导致了同一时刻只能有一个线程申请分配或释放内存，多线程没法并行执行这些操作，限制了并发效率。可以在实验题目中看到测试实例，kmem锁竞争很激烈：</font>

```bash
$ kalloctest
start test1
test1 results:
--- lock kmem/bcache stats
lock: kmem: #fetch-and-add 83375 #acquire() 433015
lock: bcache: #fetch-and-add 0 #acquire() 1260
--- top 5 contended locks:
lock: kmem: #fetch-and-add 83375 #acquire() 433015    //锁竞争最激烈
lock: proc: #fetch-and-add 23737 #acquire() 130718
lock: virtio_disk: #fetch-and-add 11159 #acquire() 114
lock: proc: #fetch-and-add 5937 #acquire() 130786
lock: proc: #fetch-and-add 4080 #acquire() 130786
tot= 83375
test1 FAIL
```

根据实验提示，可以为每一个CPU都声明一个freelist锁。CPU申请分配或释放内存，不会影响到另一个CPU执行相同的操作，因此给当前CPU上锁，不会影响到另一个CPU：

```c
struct {
    struct spinlock lock;
    struct run* freelist;
} kmem[NCPU];              // 每个CPU分配独立的freelist，多个CPU并发分配物理内存不会相互竞争

char* kmem_lock_names[] = {
    "kmem_cpu_0",
    "kmem_cpu_1",
    "kmem_cpu_2",
    "kmem_cpu_3",
    "kmem_cpu_4",
    "kmem_cpu_5",
    "kmem_cpu_6",
    "kmem_cpu_7",
};

void
kinit()
{
    for (int i = 0;i < NCPU;++i) {
        initlock(&kmem[i].lock, kmem_lock_names[i]);
    }
    freerange(end, (void*)PHYSTOP);
}
```

<font style="color:rgb(35, 38, 59);">相应的释放内存的代码：</font>

```c
void
kfree(void *pa)
{
  ......

  r = (struct run*)pa;

  push_off();

  int cpu = cpuid(); // 获取cpu编号。中断关闭时调用cpuid才是安全的，所以上面用push_off关闭中断

  acquire(&kmem[cpu].lock);         //将释放的页插入当前CPU的freelist中
  r->next = kmem[cpu].freelist;
  kmem[cpu].freelist = r;
  release(&kmem[cpu].lock);

  pop_off();                //重新打开中断
}
```

根据题目描述，分配内存`kalloc`的时候，可能会出现，在当前CPU已经没有`freelist`的情况（该CPU空闲内存不足），需要从其他CPU偷内存页。这涉及到共享数据的修改，<font style="color:rgb(35, 38, 59);">所以分配内存时需要加锁，偷页的时候也要加上锁：</font>

```c
void *
kalloc(void)
{
  struct run *r;

  push_off();						//关闭中断

  int cpu = cpuid();

  acquire(&kmem[cpu].lock);

  if (!kmem[cpu].freelist) {        // 当前CPU已经没有freelist的时候，去其他CPU偷内存页
      int steal_left = 64;          // 这里指定偷64个内存页
      for (int i = 0;i < NCPU;++i) {
          if (i == cpu)
              continue;             // 跳过当前CPU
          
          acquire(&kmem[i].lock);
          if (!kmem[i].freelist) {      // 如果在想要偷页的cpu也没有freelist了，就释放锁跳过
              release(&kmem[i].lock);
              continue;
          }

          struct run* rr = kmem[i].freelist;
          while (rr && steal_left) {           // 循环将kmem[i]的freelist移动到kmem[cpu]中
              kmem[i].freelist = rr->next;
              rr->next = kmem[cpu].freelist;
              kmem[cpu].freelist = rr;
              rr = kmem[i].freelist;
              steal_left--;
          }

          release(&kmem[i].lock);
          
          if (steal_left == 0)       // 偷到指定页数后退出循环
              break;
      }
  }

  r = kmem[cpu].freelist;
  if(r)
    kmem[cpu].freelist = r->next;
  release(&kmem[cpu].lock);

  pop_off();				//打开中断
  
  if (r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
```

<font style="color:rgb(35, 38, 59);">这是大多数博客中所用的方法，此时也确实能同通过该实验的测试。可以执行</font><code><font style="color:rgb(35, 38, 59);">make qemu</font></code><font style="color:rgb(35, 38, 59);">启动xv6，执行</font><code><font style="color:rgb(35, 38, 59);">kalloctest</font></code><font style="color:rgb(35, 38, 59);">验证：</font>

```bash
$ kalloctest
start test1
test1 results:
--- lock kmem/bcache stats
lock: kmem_cpu_0: #fetch-and-add 0 #acquire() 96312
lock: kmem_cpu_1: #fetch-and-add 0 #acquire() 164929
lock: kmem_cpu_2: #fetch-and-add 0 #acquire() 171779
lock: kmem_cpu_3: #fetch-and-add 0 #acquire() 2
lock: kmem_cpu_4: #fetch-and-add 0 #acquire() 2
lock: kmem_cpu_5: #fetch-and-add 0 #acquire() 2
lock: kmem_cpu_6: #fetch-and-add 0 #acquire() 2
lock: kmem_cpu_7: #fetch-and-add 0 #acquire() 2
lock: bcache: #fetch-and-add 0 #acquire() 1248
--- top 5 contended locks:
lock: proc: #fetch-and-add 25973 #acquire() 184026
lock: virtio_disk: #fetch-and-add 6584 #acquire() 114
lock: proc: #fetch-and-add 4434 #acquire() 184051
lock: proc: #fetch-and-add 4322 #acquire() 184046
lock: pr: #fetch-and-add 4296 #acquire() 5
tot= 0
test1 OK
start test2
total free number of pages: 32499 (out of 32768)
.....
test2 OK
```

可以发现kmem的锁竞争已经得到了改善，通过了测试。

但是还有一个潜在的问题：当一个<font style="color:rgb(35, 38, 59);">cpu1在持有自身的锁的时候去cpu2偷页，此时cpu2也在持有自身锁的时候去cpu1偷页，会造成死锁。</font>

不过一个cpu内存页不足的情况很少见，两个cpu内存页不足还互相偷对方内存页的情况就更少了。但既然是个潜在的问题，还是需要解决。请大家自行思考一下解决方案，或者在文章开头[参考博客](https://blog.miigon.net/posts/s081-lab8-locks/)的评论区中提到了这个问题，可以去参考。

### <font style="color:rgb(35, 38, 59);">Buffer cache(hard)</font>

<font style="color:rgb(35, 38, 59);">bcache.lock用来保护高速缓存区的缓存块，多个进程不能同时操作磁盘缓存。</font>

<font style="color:rgb(35, 38, 59);">先来看看原本获取缓存块的代码：</font>

```c
// kernel/bio.c
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  for(b = bcache.head.next; b != &bcache.head; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
    if(b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  panic("bget: no buffers");
}
```

<font style="color:rgb(35, 38, 59);">原本的设计中，当想要获取一个buf的时候，会给整个缓存区上锁</font>，根据块号`blockno`查找对<font style="color:rgb(35, 38, 59);">应块是否已经缓存区，若在的话就将块引用数+1，释放锁返回。不在的话，就遍历链表，将最近最久未使用的引用数为0的buf（后面称为</font>**<font style="color:rgb(35, 38, 59);">LRU-buf</font>**<font style="color:rgb(35, 38, 59);">）作为缓存区块，释放锁返回该区块。</font>

<font style="color:rgb(52, 52, 60);">当多个进程同时使用文件系统的时候，bcache.lock 上会发生严重的锁竞争。</font>

<font style="color:rgb(35, 38, 59);">bcache 中的区块缓存是会被多个进程、多个CPU共享的（多个进程可以同时访问同一个区块），所以不能模仿上一个实验，为每个CPU分配专属的块。</font>

<font style="color:rgb(35, 38, 59);"></font>

改进方案为：建立一个由`blockno`和`dev`到buf的哈希表，通过一个特定的哈希公式映射到哈希桶，由此在每个哈希桶上加锁。这样，只有在两个进程同时访问的区块同时哈希到同一个桶的时候，才会发生锁竞争。当桶中的空闲 buf 不足的时候，从其他的桶中获取 buf。

<font style="color:rgb(35, 38, 59);">先定义哈希表相关：</font>

```c
// kernel/bio.c
// 哈希表中的桶号索引。根据提示，设置质数个桶可以降低哈希冲突的可能性
#define NBUFMAP_BUCKET 13
// 哈希索引
#define BUFMAP_HASH(dev, blockno) ((((dev)<<27)|(blockno))%NBUFMAP_BUCKET)
```

<font style="color:rgb(35, 38, 59);">修改bcache结构体（驱逐锁后面解释），给每一个桶声明一个锁：</font>

```c
struct {
//   struct spinlock lock;
  struct buf buf[NBUF];
  // 哈希表
  struct buf bufmap[NBUFMAP_BUCKET];
  struct spinlock bufmap_locks[NBUFMAP_BUCKET];     // 桶锁
} bcache;
```

这时候的设计思路是：在`bget`中，获取当前的桶锁，索引到哈希表中看对应buf是否存在，存在就直接释放锁返回该块，不存在就在所有桶寻找一个LRU-buf，寻找的时候上对应的桶锁，找到后就释放这个桶锁，查询结束将该buf移出他原本所在的桶（称为缓存驱逐），然后加入到blockno对应的桶中，释放锁返回该地址。

![1733642818970-f07c4dab-9701-43ec-884a-dad8b8baddb1.jpeg](https://cdn.davidingplus.cn/images/2026/09/11/1733642818970-f07c4dab-9701-43ec-884a-dad8b8baddb1-335998.jpeg)

<font style="color:rgb(35, 38, 59);">这样会产生的问题如下：</font>

<font style="color:rgb(35, 38, 59);">1.</font>**<font style="color:rgb(35, 38, 59);">查询时刻的buf和驱逐时刻的buf状态不一致</font>**<font style="color:rgb(35, 38, 59);">。查询的时候该buf符合要求，但是查询完对应的桶之后会释放该桶锁，释放之后该buf就可以被其他进程访问了，在驱逐的时候就不一定符合要求了。</font>

<font style="color:rgb(35, 38, 59);">解决方法是，每一次查询桶都记录该桶的桶号，查询完该桶，如果在该桶中找到新的LRU-buf就不释放该桶锁，直到驱逐完再解锁。</font>

<font style="color:rgb(35, 38, 59);">2.</font>**<font style="color:rgb(35, 38, 59);">两个进程查询桶时会造成环路死锁</font>**<font style="color:rgb(35, 38, 59);">。如：CPU1在桶1中拿着1锁，要去桶2查询，CPU2在桶2拿着2锁，要去桶1查询，CPU1拿不到2锁，CPU2也拿不到1锁，就会造成死锁。</font>

<font style="color:rgb(35, 38, 59);">解决方法是，去查询其他桶之前，释放自己当前的桶锁，再去查询，查询结束后，再获取桶锁，把LRU-buf加入桶，这样就可以避免死锁。</font>

<font style="color:rgb(35, 38, 59);">但是这样会引入新的问题：CPU检查blockno的buf是否存在缓存区，不在的话就释放桶锁去查询其他桶。因为释放了该桶的桶锁，其他CPU也会通过同样的blockno索引到相同的桶中，拿到桶锁，检查到buf不在缓存区，然后释放锁区查询桶，也就是同样的操作执行多次，会导致一个区块有多份缓存的情况。</font>

<font style="color:rgb(35, 38, 59);">解决方法是，牺牲一点效率保证安全：添加一个新的锁</font><code><font style="color:rgb(35, 38, 59);">eviction_lock</font></code><font style="color:rgb(35, 38, 59);">，驱逐锁。释放桶锁后，加上驱逐锁（注意顺序），然后马上再次判断blockno的buf是否存在缓存区，确保不会创建重复的缓存buf。若不存在，就开始执行查询操作。最后把buf添加到桶后才释放锁。</font>

<font style="color:rgb(35, 38, 59);">这样，即使有多个线程同时用同一个blockno访问同一个桶，并都检查到blockno的buf不在缓存区，也都只会有一个线程能拿到驱逐锁，然后去其他桶拿buf，把buf放到自己的桶后才释放驱逐锁。其他被驱逐锁卡住的线程拿到驱逐锁后也会先检查buf是否存在缓存区，这个时候可以查询到，直接释放锁返回。</font>

<font style="color:rgb(35, 38, 59);">这样做的好处：保证了查找过程中不会出现死锁，并且不会出现极端情况下一个块产生多个缓存的情况。坏处：驱逐锁相当于全局锁，使得原本可并发的遍历驱逐过程的并行性降低了。并且每一次 cache miss 的时候，都会多一次额外的桶遍历开销。</font>

<font style="color:rgb(35, 38, 59);">具体代码实现：</font>

```c
// kernel/buf.h
struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?
  uint dev;
  uint blockno;
  struct sleeplock lock;
  uint refcnt;
  // struct buf *prev; // LRU cache list
  struct buf *next;
  uchar data[BSIZE];

  uint lastuse;     //用于跟踪LRU-buf
};
```

```c
// kernel/bio.c
// 哈希表中的桶号索引
#define NBUFMAP_BUCKET 13
// 哈希索引
#define BUFMAP_HASH(dev, blockno) ((((dev)<<27)|(blockno))%NBUFMAP_BUCKET)

struct {
//   struct spinlock lock;
  struct buf buf[NBUF];
  struct spinlock eviction_lock;        // 驱逐锁
  // 哈希表
  struct buf bufmap[NBUFMAP_BUCKET];
  struct spinlock bufmap_locks[NBUFMAP_BUCKET];     // 桶锁
} bcache;

void
binit(void)
{
    // 初始化桶锁
    for (int i = 0;i < NBUFMAP_BUCKET;++i) {
        initlock(&bcache.bufmap_locks[i], "bcache_bufmap");
        bcache.bufmap[i].next = 0;
    }

    for (int i = 0;i < NBUF;++i) {
        // 初始化缓存区块
        struct buf* b = &bcache.buf[i];
        initsleeplock(&b->lock, "buffer");
        b->lastuse = 0;
        b->refcnt = 0;

        // 将所有缓存区块添加到bufmap[0]
        b->next = bcache.bufmap[0].next;
        bcache.bufmap[0].next = b;
    }

    initlock(&bcache.eviction_lock, "bcache_eviction");
}

// Look through buffer cache for block on device dev.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  // 哈希获取桶号
  uint key = BUFMAP_HASH(dev, blockno);

  acquire(&bcache.bufmap_locks[key]);

  // blockno的缓存区块是否已经在缓存区中
  for (b = bcache.bufmap[key].next;b;b = b->next) {
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.bufmap_locks[key]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // 不在缓存区

  // 为了防止死锁，先释放当前桶锁
  release(&bcache.bufmap_locks[key]);
  // 为了防止blockno的缓存区块被重复创建，加上驱逐锁
  acquire(&bcache.eviction_lock);

  // 释放桶锁-->加驱逐锁的间隙可能创建了blocknod的缓存区块，因此再检查一次
  for (b = bcache.bufmap[key].next;b;b = b->next) {
      if (b->dev == dev && b->blockno == blockno) {
        acquire(&bcache.bufmap_locks[key]);     // 添加引用次数时必须加上桶锁
        b->refcnt++;
        release(&bcache.bufmap_locks[key]);
        release(&bcache.eviction_lock);
        acquiresleep(&b->lock);
        return b;
    }
  }

  // 仍然不在缓存区
  // 此时只持有驱逐锁，不持有任何桶锁。查询所有桶中的LRU-buf

  struct buf* before_least = 0;     // LRU-buf的前一个块
  uint holding_bucket = -1;         //记录当前持有哪个桶锁

  // 循环查询所有桶
  for (int i = 0;i < NBUFMAP_BUCKET;++i) {
      acquire(&bcache.bufmap_locks[i]);     // 获取当前遍历的桶锁(在找到下一个LRU-buf或驱逐内存之前都不释放)

      int newfound = 0;     // 是否在当前桶找到的新的LRU-buf

      for (b = &bcache.bufmap[i];b->next;b = b->next) {
          if (b->next->refcnt == 0 && (!before_least || b->next->lastuse < before_least->next->lastuse)) {
              before_least = b;
              newfound = 1;
          }
      }
      if (!newfound)                            // 如果没找到找到新的LRU-buf，就释放当前的桶锁
          release(&bcache.bufmap_locks[i]);
      else {                                                    // 找到了新的LRU-buf
          if (holding_bucket != -1)                             // 如果当前找到的不是第一个LRU-buf，之前肯定持有某个桶锁，需要释放  
              release(&bcache.bufmap_locks[holding_bucket]);
          holding_bucket = i;                                   // 把标记 holding_bucket 更改成当前桶锁编号
      }
  }

  // 如果没找到任何一个LRU-buf，表示没有空闲缓存块了
  if (!before_least)
      panic("bget: no buffuers");

  b = before_least->next;           // b=LRU-buf

  if (holding_bucket != key) {      // 想要偷的块如果不在key桶，就要把块从他所在的桶驱逐出来
      before_least->next = b->next;
      release(&bcache.bufmap_locks[holding_bucket]);

      //将LRU-buf添加到key桶
      acquire(&bcache.bufmap_locks[key]);
      b->next = bcache.bufmap[key].next;
      bcache.bufmap[key].next = b;
  }

  // 设置新buf的字段
  b->dev = dev;
  b->blockno = blockno;
  b->refcnt = 1;
  b->valid = 0;
  // 可以释放相关锁了
  release(&bcache.bufmap_locks[key]);
  release(&bcache.eviction_lock);
  acquiresleep(&b->lock);
  return b;
}

void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  uint key = BUFMAP_HASH(b->dev, b->blockno);

  acquire(&bcache.bufmap_locks[key]);
  b->refcnt--;
  if (b->refcnt == 0) {
      b->lastuse = ticks;
  }
  
  release(&bcache.bufmap_locks[key]);
}

void
bpin(struct buf* b) {
  uint key = BUFMAP_HASH(b->dev, b->blockno);
    
  acquire(&bcache.bufmap_locks[key]);
  b->refcnt++;
  release(&bcache.bufmap_locks[key]);
}

void
bunpin(struct buf* b) {
  uint key = BUFMAP_HASH(b->dev, b->blockno);
    
  acquire(&bcache.bufmap_locks[key]);
  b->refcnt--;
  release(&bcache.bufmap_locks[key]);
}
```

至此可验证该实验是否通过。

## 可扩展的实验

将缓冲区缓存中的查找变为无锁操作。提示：使用 GCC 的 `__sync_*` 函数。证明你的实现的正确性。


> 更新: 2024-12-08 16:04:40  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/chtytgblad2rs7vg>