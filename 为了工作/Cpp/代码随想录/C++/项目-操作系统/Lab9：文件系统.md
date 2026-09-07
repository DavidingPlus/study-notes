# Lab9：文件系统

实验难度出自官网，实验简述和实验耗时是主观体验

* fs  `简述`: 让xv6支持大文件（原来只支持268MB以下的文件）；实现符号链接（软链接）
* `难度`: 2 moderate  `耗时`: 7h
* `建议`: 借助这个lab梳理xv6的文件系统，面试常考题

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/fs.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><https://xv6.dgs.zone/labs/requirements/lab9.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://blog.miigon.net/posts/s081-lab9-file-system/>

开始实验前，**<font style="background-color:#FBDE28;">需要切换到本次实验的分支：fs</font>\*\*\*\*。**

## 知识点

内容较多，可以在做实验遇到不懂的地方再来看。

<font style="color:rgb(35, 38, 59);">文件系统的目的是组织和存储数据，通常支持用户和应用程序之间的数据共享。文件系统解决了几个难题：</font>

* <font style="color:rgb(35, 38, 59);">文件系统需要磁盘上的数据结构来表示目录和文件名称树，记录保存每个文件内容的块的标识，以及记录磁盘的哪些区域是空闲的。</font>
* <font style="color:rgb(35, 38, 59);">文件系统必须支持崩溃恢复（crash recovery）。也就是说，如果发生崩溃（比如电源故障），文件系统必须在重新启动后仍能正常工作。风险在于崩溃可能会中断一系列更新，并使磁盘上的数据结构不一致（比如一个块在某个文件中使用但同时仍被标记为空闲）。</font>
* <font style="color:rgb(35, 38, 59);">不同的进程可能同时在文件系统上运行，因此文件系统代码必须协调以保持不变量。</font>
* <font style="color:rgb(35, 38, 59);">访问磁盘的速度比访问内存慢几个数量级，因此文件系统必须保持常用块的内存缓存。</font>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">xv6文件系统实现分为七层</font>

| <font style="color:rgb(34, 34, 34);">文件描述符（File descriptor）</font> |
| :---: |
| <font style="color:rgb(34, 34, 34);">路径名（Pathname）</font> |
| <font style="color:rgb(34, 34, 34);">目录（Directory）</font> |
| <font style="color:rgb(34, 34, 34);">索引结点（Inode）</font> |
| <font style="color:rgb(34, 34, 34);">日志（Logging）</font> |
| <font style="color:rgb(34, 34, 34);">缓冲区高速缓存（Buffer cache）</font> |
| <font style="color:rgb(34, 34, 34);">磁盘（Disk）</font> |

**<font style="color:rgb(35, 38, 59);">磁盘层 </font>**<font style="color:rgb(35, 38, 59);">读取和写入virtio硬盘上的块。</font>

**<font style="color:rgb(35, 38, 59);">缓冲区高速缓存层 </font>**<font style="color:rgb(35, 38, 59);">缓存磁盘块并同步对它们的访问，确保每次只有一个内核进程可以修改存储在任何特定块中的数据。</font>

**<font style="color:rgb(35, 38, 59);">日志记录层 </font>**<font style="color:rgb(35, 38, 59);">允许更高层在一次事务（transaction）中将更新包装到多个块，并确保在遇到崩溃时自动更新这些块（即，所有块都已更新或无更新）。</font>

**<font style="color:rgb(35, 38, 59);">索引结点层 </font>**<font style="color:rgb(35, 38, 59);">提供单独的文件，每个文件表示为一个索引结点，其中包含唯一的索引号（i-number）和一些保存文件数据的块。</font>

**<font style="color:rgb(35, 38, 59);">目录层 </font>**<font style="color:rgb(35, 38, 59);">将每个目录实现为一种特殊的索引结点，其内容是一系列目录项，每个目录项包含一个文件名和索引号。</font>

**<font style="color:rgb(35, 38, 59);">路径名层 </font>**<font style="color:rgb(35, 38, 59);">提供了分层路径名，如</font>**<font style="color:rgb(35, 38, 59);">/usr/rtm/xv6/fs.c</font>**<font style="color:rgb(35, 38, 59);">，并通过递归查找来解析它们。</font>

**<font style="color:rgb(35, 38, 59);">文件描述符层 </font>**<font style="color:rgb(35, 38, 59);">使用文件系统接口抽象了许多Unix资源（比如管道、设备、文件等），简化了程序员的工作</font>

<font style="color:rgb(35, 38, 59);">文件系统必须有将索引节点和内容块存储在磁盘上哪些位置的方案。为此，xv6将磁盘划分为几个部分：</font>

![1733646741129-6097e554-b5b4-4470-b606-59b758f3203f.png](./img/3aPBqjgGlVCslHt7/1733646741129-6097e554-b5b4-4470-b606-59b758f3203f-841863.png)

<font style="color:rgb(35, 38, 59);">文件系统不使用块0（它保存引导扇区）</font>

<font style="color:rgb(35, 38, 59);">块1称为超级块：它包含有关文件系统的元数据（文件系统大小（以块为单位）、数据块数、索引节点数和日志中的块数）。超级</font>块由一个名为`mkfs`的单独的程序填<font style="color:rgb(35, 38, 59);">充，该程序构建初始文件系统</font>

<font style="color:rgb(35, 38, 59);">从2开始的块保存日志</font>

<font style="color:rgb(35, 38, 59);">日志之后是索引节点，每个块有多个索引节点</font>

<font style="color:rgb(35, 38, 59);">然后是位图块，跟踪正在使用的数据块</font>

<font style="color:rgb(35, 38, 59);">其余的块是数据块：每个都要么在位图块中标记为空闲，要么保存文件或目录的内容</font>

#### <font style="color:rgb(35, 38, 59);">缓冲区高速缓存层（Buffer cache）</font>

相关代码：`kernel/bio.c`。Buffer cache有两个任务：

1. 同步对磁盘块的访问，以确保磁盘块在内存中只有一个副本，并且一次只有一个内核线程使用该副本
2. 缓存常用块，以便不需要从慢速磁盘重新读取它们

Buffer cache层导出的主接口主要是`bread`和`bwrite`：

* `bread`获取一个buf，其中包含一个可以在内存中读取或修改的块的副本
* `bwrite`将修改后的缓冲区写入磁盘上的相应块

内核线程必须通过调用`brelse`释放缓冲区。Buffer cache每个缓冲区使用一个睡眠锁，以确保每个缓冲区（每个磁盘块）每次只被一个线程使用；`bread`返回一个上锁的缓冲区，`brelse`释放该锁。

<font style="color:rgb(35, 38, 59);">Buffer cache中保存磁盘块的缓冲区数量固定，这意味着如果文件系统请求还未存放在缓存中的块，Buffer cache必须回收当前保存其他块内容的缓冲区。Buffer cache为新块回收最近使用最少的缓冲区。这样做的原因是认为最近使用最少的缓冲区是最不可能近期再次使用的缓冲区。</font>

<font style="color:rgb(35, 38, 59);">Buffer cache是以 双链表 表示的缓冲区。</font>`main`<font style="color:rgb(35, 38, 59);">（</font>**<font style="color:rgb(35, 38, 59);">kernel/main.c</font>**\_<font style="color:rgb(35, 38, 59);">）调用</font>*的函数`binit`使用静*<font style="color:rgb(35, 38, 59);">态数</font>\_组`buf`（**kernel/bio.c**）中的`NBUF`个缓冲区初始化列表。对Buffer cache的所有其他访问都通过`bcache.head`引用链表，而不是`buf`数组。

<font style="color:rgb(35, 38, 59);">缓冲区有两个与之关联的状态字段。</font>字段`valid`表示缓冲区是否包含块的副本。字段`disk`表示缓冲区内容是否已交给磁盘，这可能会更改缓冲区（例如，将数据从磁盘写入`data`）

`bread`调用`bget`为给定扇区获取缓冲区。如果缓冲区需要从磁盘进行读取，`bread`会在返回缓冲区之前调用`virtio_disk_rw`来执行此操作。

`bget`扫描缓冲区列表，查找具有给定设备和扇区号的缓冲区。如果存在这样的缓冲区，`bget`将获取缓冲区的睡眠锁。然后`bget`返回锁定的缓冲区。

如果对于给定的扇区没有缓冲区，`bget`必须创建一个，这可能会重用包含其他扇区的缓冲区。再次扫描缓冲区列表，查找未在使用中的缓冲区（`b->refcnt = 0`）：任何这样的缓冲区都可以使用。`bget`编辑缓冲区元数据以记录新设备和扇区号，并获取其睡眠锁。注意，`b->valid = 0`的布置确保了`bread`将从磁盘读取块数据，而不是错误地使用缓冲区以前的内容。

一旦`bread`读取了磁盘并将缓冲区返回给其调用者，调用者就可以独占使用缓冲区，并可以读取或写入数据字节。如果调用者确实修改了缓冲区，则必须在释放缓冲区之前调用`bwrite`将更改的数据写入磁盘。`bwrite`调用`virtio_disk_rw`与磁盘硬件对话。

当调用方使用完缓冲区后，它必须调用`brelse`来释放缓冲区。`brelse`释放睡眠锁并将缓冲区移动到链表的前面。移动缓冲区会使列表按缓冲区的使用频率排序：列表中的第一个缓冲区是最近使用的，最后一个是最近使用最少的。`bget`中的两个循环利用了这一点：在最坏的情况下，对现有缓冲区的扫描必须处理整个列表，但首先检查最新使用的缓冲区（从`bcache.head`开始，然后是下一个指针），在引用局部性良好的情况下将减少扫描时间。选择要重用的缓冲区时，通过自后向前扫描（跟随`prev`指针）选择最近使用最少的缓冲区。

#### <font style="color:rgb(35, 38, 59);">inode</font>

<font style="color:rgb(35, 38, 59);">inode是一个64字节的数据结构，包含：</font>

* <font style="color:rgb(35, 38, 59);">type字段，表明inode是文件还是目录</font>
* <font style="color:rgb(35, 38, 59);">nlink字段，也就是link计数器，用来跟踪究竟有多少文件名指向了当前的inode</font>
* <font style="color:rgb(35, 38, 59);">size字段，表明了文件数据有多少个字节</font>
* <font style="color:rgb(35, 38, 59);">12个direct block number，这些直接块编号直接指向文件的前 12 个磁盘块。当文件较小时，直接块可以满足文件存储的需求，每个块的大小在 xv6 中是 1KB，因此使用 12 个直接块意味着最多可以存储 12 \* 1KB = 12KB 的数据</font>
* <font style="color:rgb(35, 38, 59);">1个indirect block number，指向一个间接块，间接块本身是一个磁盘块，其中包含了 256 个条目，每个条目存储一个数据块编号，这些编号依次指向文件的数据块</font>

<font style="color:rgb(35, 38, 59);">因此XV6中最大文件尺寸：（256 + 12） \* 1KB = 268KB。</font>

> 1个block是1KB，一个 block number 是4字节，1024 / 4 = 256

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">inode有两种含义：磁盘上的数据结构inode，以及内存中的inode，包含磁盘inode的副本以及内核中所需的额外信息</font>

<font style="color:rgb(35, 38, 59);"> </font>**<font style="color:rgb(35, 38, 59);">磁盘inode</font>**<font style="color:rgb(35, 38, 59);">：</font>inode 在磁盘上被存储为固定大小的结构，并被连续存储在一个区域中。

`struct dinode`（kernel/fs.h）是磁盘 inode 的数据结构，字段`type`表示 inode 类型（文件、目录、特殊文件），字段`nlink`记录引用该 inode 的目录条目的数量，减为零时释放该 inode 和其占用的数据块，字段`size`记录文件内容的字节数，字段`addrs`为一个数组，记录了保存文件内容的磁盘块号。

<font style="color:rgb(35, 38, 59);"> </font>**<font style="color:rgb(35, 38, 59);">内存inode</font>**<font style="color:rgb(35, 38, 59);">：</font>当文件被访问（如打开或修改）时，系统会将磁盘上的 inode 加载到内存中，并在内存中维护它的一个副本。`struct inode`（kernel/file.h） 是磁盘 inode 的内存副本，并包含额外字段来支持内核的操作。只有当某个进程需要访问文件时，系统才会将磁盘 inode 加载到内存，并创建 `struct inode`。`ref`字段统计引用内存中inode的C指针的数量，如果引用计数降至零，内核将从内存中丢弃该inode。`iget` 和 `iput` 函数分别获取和释放指向 inode 的指针，修改引用计数。

内存inode中的锁机制：

* `icache.lock`：保护全局 inode 缓存（icache），确保每个 inode 在缓存中最多只有一个副本，保护 inode中`ref` 字段的正确性，即记录指向该 inode 的内存指针的正确数量
* inode的`struct sleeplock lock`字段：每个 inode 的独立锁，用于对该 inode 的独占访问，确保对 inode 的元数据字段和其内容块的修改是线程安全的
* inode的`ref`字段<font style="color:rgb(35, 38, 59);">：</font>**<font style="color:rgb(35, 38, 59);">管理内存 inode 的生命周期</font>**<font style="color:rgb(35, 38, 59);">，记录指向该 inode 的 C 指针数量。如果 ref 减少到 0，表示内存中已没有任何代码持有该 inode 的引用，缓存可以移除该inode，但 </font>**<font style="color:rgb(35, 38, 59);">inode 在磁盘上仍然存在</font>**
* <font style="color:rgb(35, 38, 59);">inode的</font>`nlink`<font style="color:rgb(35, 38, 59);">字段：</font>**<font style="color:rgb(35, 38, 59);">管理磁盘 inode 的生命周期</font>**<font style="color:rgb(35, 38, 59);">，记录指向该inode的目录项（创建硬链接）。当 nlink 减少到 0，表示没有任何目录项指向该 inode，如果 inode 的内容已经不在内存中（即 </font>`ref == 0`<font style="color:rgb(35, 38, 59);">），该 inode 将被完全释放，包括 inode 本身和它占用的所有数据块</font>

<font style="color:rgb(35, 38, 59);"></font>

`iget(uint dev, uint inum)`函数返回一个inode指针，在调用`iput(struct inode *ip)`函数之前该inode始终有效（iput对ref-1，若ref为0会释放该indoe）。不锁inode，保证将 inode 加载到内存，并增加其引用计数。多个线程或进程可以同时持有指向同一 inode 的指针，提高访问效率，但在对 inode 数据进行修改或读取时，需要调用 `ilock` 来加锁。`iget`函数中的部分代码如下：

```c
empty = 0;
  for(ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++){
    if(ip->ref > 0 && ip->dev == dev && ip->inum == inum){
      ip->ref++;
      release(&icache.lock);
      return ip;
    }
    if(empty == 0 && ip->ref == 0)    // Remember empty slot.
      empty = ip;
  }
```

如果没有找到匹配的inode，会返回一个之前未被引用的inode（当然返回之前会初始化这个inode，ref为1），这个inode的`valid`字段为0，表示未从磁盘中读取到内存中。为了确保该inode保存磁盘inode的副本，代码必须调用`ilock`函数。`ilock`函数确保 inode 被独占访问，如果 inode 的 `valid` 标志为0（未加载内容），则从磁盘读取 inode 的内容到内存。`iunlock`函数用于释放 inode 上的锁，解锁后，其他进程或线程可以访问该 inode。

**<font style="color:rgb(35, 38, 59);">内存inode</font>**<font style="color:rgb(35, 38, 59);">的</font>主要功能是确保对 inode 的并发访问是安全的，只要有 C 指针引用 inode（即 `ref > 0`），该 inode 就会保留在缓存中，不会被释放。inode 缓存是直写的（write-through），对 inode 的任何修改都会通过 `iupdate()` 立即同步到磁盘。

`ialloc`是分配一个新的磁盘inode的函数，用于创建文件、目录或设备节点时初始化inode数据结构。`ialloc`函数遍历所有inode，寻找字段type==0的空闲inode。找到之后，将该inode初始化，设置其字段type为指定的type，在通过`iget`函数将该磁盘inode读入内存。在遍历过程中，会使用`bread`函数读取磁盘块bp，此时bp是上锁的，确保`ialloc`的分配过程是原子的。

`iput`<font style="color:rgb(35, 38, 59);">函数会将一个内存inode的ref字段-1。如果ref减为零，意味着没有任何指针引用该 inode，此时该 inode 的</font>**<font style="color:rgb(35, 38, 59);">缓存槽位</font>**<font style="color:rgb(35, 38, 59);">可以被其他 inode 重用。如果ref减为零且没有任何链接指向该 inode（nlink == 0），</font>`iput`<font style="color:rgb(35, 38, 59);"> 将释放该 inode 及其占用的数据块。</font>

* 当 `ip->ref == 1` 且 `nlink == 0` 时，只有调用 `iput` 的线程持有该 inode 的引用，其他线程无法再获取指向该 inode 的指针。iput中，通过先获取 `icache.lock`，然后获取睡眠锁，确保在操作过程中不会出现死锁。
* 当 `ialloc` 尝试分配新的 inode 时，如果正好选择了一个正在被 `iput` 释放的 inode，分配线程会等待 `inode.lock`，确保在读取或写入该 inode 时不会发生冲突。这种竞争是良性的，因为分配线程会在 `iput` 完成后继续进行。
* 由于 `iput` 可能进行磁盘写操作，所有涉及文件系统的系统调用（包括只读操作）都需要在事务中执行，确保文件系统的一致性和完整性。

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">即使文件的链接计数（</font>`nlink`<font style="color:rgb(35, 38, 59);">）降为 0，某些进程仍可能持有该 inode 的引用（比如进程打开了该文件并正在读写）。因此，不能在 </font>`iput`<font style="color:rgb(35, 38, 59);"> 中立刻释放 inode 和相关资源。如果在最后一个进程关闭该文件之前发生崩溃，inode 会以一种“不一致”状态留存在磁盘上：文件在磁盘上被标记为“已分配”，但磁盘上没有任何目录项指向该 inode，导致用户无法访问该磁盘inode。随着时间推移，磁盘空间会被这些“不再使用但未释放”的 inode 逐渐耗尽。</font>

<font style="color:rgb(35, 38, 59);">现代文件系统通过两种方式解决这一问题：</font>

* <font style="color:rgb(35, 38, 59);">文件系统在崩溃后重新启动时，扫描整个磁盘上的所有 inode，查找标记为“已分配”但没有任何目录项引用的 inode，释放它们的空间，回收资源</font>
* <font style="color:rgb(35, 38, 59);">文件系统维护一个列表（比如记录在超级块中），用来跟踪nlink为 0，但引用计数不为 0 的 inode。当系统重新启动时，只需检查这个列表中的 inode 是否仍然有进程引用，如果没有引用（ref为 0），释放它们，如果仍有引用，则无需操作</font>

<font style="color:rgb(35, 38, 59);">（XV6没有实现上面两种方案，没有解决这个问题。对于这个实验来说是一个扩展点）</font>

## 实验

<font style="color:rgb(35, 38, 59);">增加XV6系统文件的最大大小（当前只支持268kb以下的文件）；实现软链接</font>

### <font style="color:rgb(35, 38, 59);">Large files(moderate)</font>

<font style="color:rgb(35, 38, 59);"> 原本的XV6文件系统中，inode的结构如下：</font>

```c
// kernel/fs.h
#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEVICE only)
  short minor;          // Minor device number (T_DEVICE only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+1];   // Data block addresses
};
```

`addrs`<font style="color:rgb(35, 38, 59);">字段用来索引记录数据的所在盘块号。每个文件所占用的前 12 个盘块的盘块号是直接记录在 inode 中的（每个盘块 1024 字节），所以对于任何文件的前 12 KB 数据，都可以通过访问 inode 直接得到盘块号。这一部分称为直接记录盘块（addr\[0~11]，12个直接块)</font>

<font style="color:rgb(35, 38, 59);">·	对于大于 12 个盘块的文件，大于 12 个盘块的部分，会分配一个额外的一级索引表（addr\[11]，1024Byte），用于存储这部分数据的所在盘块号。一个索引项是4字节，一级索引表可以包含 BSIZE(1024) / 4 = 256 个盘块号，加上 inode 中的 12 个盘块号，一个文件最多可以使用 12+256 = 268 个盘块，也就是 268KB。</font>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">根据题目提示，该实验需要修改inode的结构，修改成11个直接块，1个一级间接块，1个二级间接块。</font>

<font style="color:rgb(35, 38, 59);">所以可以先修改inode的结构体，注意这是磁盘inode的结构体：</font>

```c
// kernel/fs.h
#define NDIRECT 11                                               // 直接块数量
#define NINDIRECT (BSIZE / sizeof(uint))                         // 一级间接块数量
#define MAXFILE (NDIRECT + NINDIRECT + NINDIRECT * NINDIRECT)    // 二级间接块数量

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEVICE only)
  short minor;          // Minor device number (T_DEVICE only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+2];   // 0~10：直接索引    11：一级间接索引    12：二级间接索引
};
```

<font style="color:rgb(35, 38, 59);">然后还需要修改内存中inode的副本结构体：</font>

```c
// kernel/file.h
// in-memory copy of an inode
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 2];	// 修改
};
```

<font style="color:rgb(35, 38, 59);">直接块腾一个位置出来给二级间接块。</font>

<font style="color:rgb(35, 38, 59);"></font>

<font style="color:rgb(35, 38, 59);">需要修改的第一个函数是</font>`bmap`<font style="color:rgb(35, 38, 59);">，这个函数是将inode中的第bn个逻辑块号和物理块号建立映射，其原理备注在源代码中（这里是解释原本的</font>`bmap`<font style="color:rgb(35, 38, 59);">）：</font>

```c
// kernel/fs.c
static uint
bmap(struct inode *ip, uint bn)
{
  uint addr, *a;
  struct buf *bp;

  // 如果bn小于直接块数量，则按直接映射处理
  if(bn < NDIRECT){
    if((addr = ip->addrs[bn]) == 0)				// 如果对应的物理块号为0，表示还没分配，则分配一个新的物理块，建立映射
      ip->addrs[bn] = addr = balloc(ip->dev);
    return addr;
  }
    
  // 到这里已经是间接块了，减去直接块的数量得到间接块的逻辑块号
  bn -= NDIRECT;

  if(bn < NINDIRECT){
    // 如果间接块还没分配，分配一个
    if((addr = ip->addrs[NDIRECT]) == 0)			// 0~NDIRECT-1是直接块，NDIRECT是间接块
      ip->addrs[NDIRECT] = addr = balloc(ip->dev);	// 此时这个块可以映射256个块
      
    // 获取刚分配的缓存块，检查bn对应的块，如果为0则没有分配，建立映射
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;
    if((addr = a[bn]) == 0){
      a[bn] = addr = balloc(ip->dev);
      log_write(bp);
    }
    brelse(bp);			// 建立映射结束，释放掉缓存块（可以通过ip->dev和bn找到这个块了）
    return addr;
  }

  panic("bmap: out of range");
}
```

<font style="color:rgb(35, 38, 59);">清楚了原理，加上刚才增加的二级映射，就很好修改了：</font>

```c
// kernel/fs.c
static uint
bmap(struct inode *ip, uint bn)
{
  uint addr, *a;
  struct buf *bp;

  // 如果bn小于直接块数量，则按直接映射处理
  if(bn < NDIRECT){
    if((addr = ip->addrs[bn]) == 0)				// 如果对应的物理块号为0，表示还没分配，则分配一个新的物理块，建立映射
      ip->addrs[bn] = addr = balloc(ip->dev);
    return addr;
  }
    
  // 到这里已经是一级间接块了，减去直接块的数量得到间接块的逻辑块号
  bn -= NDIRECT;

  if(bn < NINDIRECT){
    // 如果间接块还没分配，分配一个
    if((addr = ip->addrs[NDIRECT]) == 0)			// 0~NDIRECT-1是直接块，NDIRECT是间接块
      ip->addrs[NDIRECT] = addr = balloc(ip->dev);	// 此时这个块可以映射256个块
      
    // 获取刚分配的缓存块，检查bn对应的块，如果为0则没有分配，建立映射
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;
    if((addr = a[bn]) == 0){
      a[bn] = addr = balloc(ip->dev);
      log_write(bp);
    }
    brelse(bp);			// 建立映射结束，释放掉缓存块（可以通过ip->dev和bn找到这个块了）
    return addr;
  }

  // 到这里已经是二级间接块了，减去一级间接块的数量得到二级件结块的逻辑块号
  bn -= NINDIRECT;

  // 原理很上面的一样
  if (bn < NINDIRECT * NINDIRECT) {
      if ((addr = ip->addrs[NDIRECT + 1]) == 0)
          ip->addrs[NDIRECT + 1] = addr = balloc(ip->dev);

      bp = bread(ip->dev, addr);
      a = (uint*)bp->data;
      if ((addr = a[bn / NINDIRECT]) == 0) {        // bn处在二级索引中间级的第 bn/NINDIRECT 个索引处
          a[bn / NINDIRECT] = addr = balloc(ip->dev);
          log_write(bp);
      }
      brelse(bp);

      // 最后一级索引
      bn %= NINDIRECT;
      bp = bread(ip->dev, addr);
      a = (uint*)bp->data;
      if ((addr = a[bn]) == 0) {
          a[bn] = addr = balloc(ip->dev);
          log_write(bp);
      }
      brelse(bp);
      return addr;
  }

  panic("bmap: out of range");
}
```

<font style="color:rgb(35, 38, 59);"> 	还有一个函数</font>`itrunc`<font style="color:rgb(35, 38, 59);">，释放该inode所映射的所有数据块。原理类似，直接循环释放所有数据块：</font>

```c
// kernel/fs.c
void
itrunc(struct inode *ip)
{
  int i, j;
  struct buf *bp;
  uint *a;

  // 释放直接块的映射
  for (i = 0; i < NDIRECT; i++) {
    if(ip->addrs[i]){
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  // 释放一级间接块的映射
  if (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint*)bp->data;
    for(j = 0; j < NINDIRECT; j++){
      if(a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  // 释放二级间接块的映射
  if (ip->addrs[NDIRECT + 1]) {
      bp = bread(ip->dev, ip->addrs[NDIRECT + 1]);
      a = (uint*)bp->data;
      for (int i = 0;i < NINDIRECT;++i) {
          if (a[i]) {
              struct buf* bp2 = bread(ip->dev, a[i]);
              uint* a2 = (uint*)bp2->data;
              for (int j = 0;j < NINDIRECT;++j) {
                  if (a2[j])
                      bfree(ip->dev, a2[j]);
              }
              brelse(bp2);
              bfree(ip->dev, a[i]);
          }
      }
      brelse(bp);
      bfree(ip->dev, ip->addrs[NDIRECT + 1]);
      ip->addrs[NDIRECT + 1] = 0;
  }

  ip->size = 0;
  iupdate(ip);
}
```

<font style="color:rgb(35, 38, 59);">至此可以验证该实验是否通过。</font><font style="color:rgb(85, 85, 85);background-color:rgb(247, 247, 247) !important;"></font>

### <font style="color:rgb(35, 38, 59);">Symbolic links(moderate)</font>

<font style="color:rgb(35, 38, 59);">该实验需要实现软链接机制，实现一个软链接系统调用函数</font>`symlink`。以下不再赘述添加新的系统调用的步骤。

<font style="color:rgb(35, 38, 59);">根据提示，添加新的文件类型</font>`T_SYMLINK`<font style="color:rgb(35, 38, 59);">，表示文件为软链接文件：</font>

```c
// kernel/stat.h
#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device
#define T_SYMLINK 4   // 软链接
```

<font style="color:rgb(35, 38, 59);">添加新的文件标志位</font>`O_NOFOLLOW`<font style="color:rgb(35, 38, 59);">，该标志可用于</font>`open`<font style="color:rgb(35, 38, 59);">系统调用：</font>

```c
// kernel/fcntl.h
#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400
#define O_NOFOLLOW 0x800 
```

<font style="color:rgb(35, 38, 59);"> 实现具体的</font>`sys_symlink`<font style="color:rgb(35, 38, 59);">：使用</font>`create`<font style="color:rgb(35, 38, 59);">创建一个</font>**<font style="color:rgb(35, 38, 59);">加了锁的</font>**<font style="color:rgb(35, 38, 59);">指向源文件的inode，再将链接目标文件地址</font>通过`writei`写入inode，操作完之后使用`iunlockput`解锁创建的inode<font style="color:rgb(35, 38, 59);">并使其引用计数-1，表示创建软链接的操作结束，释放锁。</font>

```c
// kernel/sysfile.c
// 软链接
uint64
sys_symlink(void) {
    struct inode* ip;
    char target[MAXPATH], path[MAXPATH];
    if (argstr(0, target, MAXPATH) < 0 || argstr(1, path, MAXPATH) < 0)
        return -1;

    begin_op();

    ip = create(path, T_SYMLINK, 0, 0);     // 创建一个新的inode，类型为T_SYMLINK，指向path文件
    if (ip == 0) {
        end_op();
        return -1;
    }

    if (writei(ip, 0, (uint64)target, 0, strlen(target)) < 0) {     // 将target路径写入inode
        end_op();
        return -1;
    }

    iunlockput(ip);
    end_op();

    return 0;
}
```

`create` 在文件系统中创建一个新的文件，设置其类型是 `T_SYMLINK`,使用一个新的inode关联这个文件， inode 的初始状态为空，没有任何数据。`create` 还在目录中为这个新文件创建了一个目录项，使得路径名 `path` 可以找到这个 inode。使用 `writei` 函数将符号链接的目标路径（`target`）作为数据，写入到新创建的 inode 的数据块中，文件的内容就变成了目标路径 `target`。

修改`sys_open`函数，通过循环调用`namei` 获取对应的inode，如果指向的仍是软链接，就继续循环读取，直到找到真正指向的文件，或者超过了一定的链接深度：

```c
// kernel/sysfile.c
uint64
sys_open(void)
{
  ......

  if(omode & O_CREATE){
    ip = create(path, T_FILE, 0, 0);
    if(ip == 0){
      end_op();
      return -1;
    }
  }
  else {
      int symlink_depth = 0;
      while (1) {
          if ((ip = namei(path)) == 0) {    // 解析路径，获取对应的inode
              end_op();
              return -1;
          }

          ilock(ip);
          if (ip->type == T_SYMLINK && (omode & O_NOFOLLOW) == 0) {     //如果当前指向的仍是软链接，则继续循环
              if (++symlink_depth > 10) {               // 链接深度超过10层就退出
                  iunlockput(ip);
                  end_op();
                  return -1;
              }
              if (readi(ip, 0, (uint64)path, 0, MAXPATH) < 0) {     // 读取链接的目标路径
                  iunlockput(ip);
                  end_op();
                  return -1;
              }
              iunlockput(ip);
          }
          else
              break;
      }

      if (ip->type == T_DIR && omode != O_RDONLY){
          iunlockput(ip);
          end_op();
          return -1;
    }
  }

  if(ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)){
    iunlockput(ip);
    end_op();
    return -1;
  }

  ......
}
```

至此可以验证实验是否通过。

## 可扩展的实验

<font style="color:rgb(51, 51, 51);">实现三级间接块</font>


> 更新: 2024-12-08 18:59:28  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/hkmr58mdzz00gorb>