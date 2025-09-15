---
title: Virtual Filesystem
categories:
  - Linux 学习
  - 内核层
abbrlink: 68b0f3fd
date: 2024-12-24 16:05:00
updated: 2024-12-31 16:00:00
---

<meta name="referrer" content="no-referrer"/>

笔记摘抄自 [Linux 内核教学 — Linux 系统内核文档](https://linux-kernel-labs-zh.xyz/) 的 VFS 部分，并总结记录。

# 虚拟文件系统（VFS）

虚拟文件系统（VFS）是内核的组件，处理所有与文件和文件系统相关的系统调用。VFS 是用户与特定文件系统之间的通用接口。这种抽象简化了文件系统的实现，使得各种文件系统更容易集成。各种文件系统通过使用 VFS 提供的 API 来实现文件系统，通用硬件以及 I/O 子系统的通信部分由 VFS 处理。

文件系统按照功能可分为：

1. 磁盘文件系统（ext3、ext4、xfs、fat 以及 ntfs 等）。
2. 网络文件系统（nfs、smbfs/cifs、ncp 等）。
3. 虚拟文件系统（procfs、sysfs、sockfs、pipefs 等）。

<!-- more -->

Linux 内核使用 VFS 处理目录和文件的层次结构（其实是一棵树）。通过挂载操作，新的文件系统被添加为 VFS 子树。文件系统通常是从其所对应的环境中挂载的（从块类型设备、网络等）。**然而 VFS 可以将普通文件作为虚拟块设备使用，可以将普通文件挂载为磁盘文件系统。**

VFS 的基本思想是提供可以表示任何文件系统文件的单一文件模型。文件系统驱动程序需要遵守公共的基准。这样，内核可以创建包含整个系统的单一目录结构。其中一个文件系统将作为根文件系统，其他文件系统将挂载在其各个目录下。

# 存储栈整体结构

Linux 存储栈的整体结构图如下。从上到下分别是：**VFS、通用块层、SCSI 层、块设备层**。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241227142615377.png" alt="image-20241227142615377" style="zoom:75%;" />

各个层次的作用如下。

1. VFS：VFS 层是 Linux 最为津津乐道的设计，也就是所谓的一切皆文件。它通过统一的接口，底层封装了各种各样的文件系统。

2. 通用块层：文件系统将读/写请求转换成 bio 和 request，提交给通用块层，通用块层对 request 进行调度，发往下一层。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241227143348812.png" alt="image-20241227143348812" style="zoom:80%;" />

3. SCSI 层：SCSI（Small Computer Systems Interface）是一组标准集，定义了与大量设备（主要是与存储相关的设备）通信所需的接口和协议。Linux 提供了一种 SCSI 子系统，用于与这些设备通信。

4. 块设备层：块设备层负责对某种具体的物理设备进行处理，完成相应的读写请求。

# 常见的文件系统模型

任何实现的文件系统都需要包含这几种明确定义的类型：super_block、inode、file 和 dentry。这些也是文件系统的元数据。

模型实体间通过某些 VFS 子系统或内核子系统进行交互：dentry cache（目录项缓存）、inode cache（索引节点缓存）和 buffer cache（缓冲区缓存）。每个实体都被视为对象，具有关联的数据结构和指向方法表的指针。通过替换关联的方法来为每个组件引入特定的行为（类似于 C++ 的多态）。

## super_block

**super_block 超级块存储挂载文件系统需要的信息。**具体如下：

1. inode 和块的位置。
2. 文件系统块大小。
3. 最大文件名长度。
4. 最大文件大小。
5. 根 inode 的位置。

磁盘上的 super_block 通常存储在磁盘的第一个块中，即文件系统控制块。

在 VFS 中，super_block 实体都保留在类型为 `struct super_block` 的结构列表中，方法则保留在类型为 `struct super_operations` 的结构中。

## inode

**inode 索引节点存储有关文件的信息。**这里的文件泛指意义上的文件，常规文件、目录、特殊文件（如管道、fifo 等）、块设备、字符设备、链接或可以抽象为文件的任何内容都包括在内。

inode 存储了以下信息：

1. 文件类型。
2. 文件大小。
3. 访问权限。
4. 访问或修改时间。
5. 数据在磁盘上的位置（指向包含数据的磁盘块的指针）。

> inode 通常不包含文件名。文件名由 dentry 存储。一个 inode 可以有多个名称（如多个硬链接文件指向同一个 inode）。

磁盘上的 inode 通常分组存储在一个专用的 inode 区域中，与数据区域分开。

在 VFS 中，inode 实体由 `struct inode` 结构表示，由 `struct inode_operations` 结构定义与之相关的操作。

## file

file 是文件系统模型中距离用户最近的组件。**inode 抽象了磁盘上的文件，file 抽象了进程打开的文件。**与其他结构不同的是，file 结构体在内存中作为 VFS 的实体存在，但没有在磁盘上的物理物对应。

file 存储了以下信息：

1. 文件游标位置。
2. 文件打开权限。
3. 指向关联 inode 的指针（inode 的索引）。

在 VFS 中，file 实体由 `struct file` 结构表示，与之相关的操作由 `struct file_operations` 结构表示。

## dentry

**dentry 将 inode 和 文件名关联起来。**存储以下信息：

1. 用于标识 inode 的整数。
2. 表示文件名的字符串。

dentry 是目录或文件路径的特定部分。例如，对于路径 `/bin/vi`，为 `/`、`bin` 和 `vi` 创建共 3 个 dentry 对象。

dentry 在磁盘上有对应物，但对应关系不是直接的。每个文件系统都有特定的方式维护 dentry。

在 VFS 中，dentry 实体由 `struct dentry` 结构表示，与之相关的操作在 `struct dentry_operations` 结构中定义。

## 其他数据结构

除了上述，VFS 中还有一些数据结构。

1. address_space/mapping：表示一个文件缓存，结构体的名字称为 address_space。但在其它结构体中被引用时，该指针的名字通常是 mapping。
2. mount：表示一个文件系统被挂载的信息。
3. file_system_type：表示一个文件系统类型，例如 ext4、proc、sys 等。

# 注册和注销文件系统

## struct file_system_type

Linux 内核支持很多文件系统，包括 ext2/ext4、reiserfs、xfs、fat、ntfs 等。但在单个系统上，不太可能超过 5/6 个文件系统。文件系统在内核中被实现为内核模块，可以动态的加载和卸载。

描述特定文件系统的结构是 `struct file_system_type`，定义如下：

```c
// linux/fs.h


struct file_system_type
{
    // 表示该文件系统的名称（传递给 mount -t 的参数）。
    const char *name;

    // 指定文件系统必须以哪些标志挂载。例如标志 FS_REQUIRES_DEV，指定 VFS 文件系统需要一个磁盘（而不是虚拟文件系统）
    int fs_flags;

    // 在加载文件系统时从磁盘中读取超级块到内存中。每种文件系统的函数都是独一无二的。
    struct dentry *(*mount)(struct file_system_type *, int, const char *, void *);

    // 释放内存中的超级块。
    void (*kill_sb)(struct super_block *);

    // 如果是内核模块实现，则为 THIS_MODULE。如果直接写在内核中，则为 NULL。
    struct module *owner;

    struct file_system_type *next;

    // 一个列表，包含与该文件系统关联的所有超级块。由于同一文件系统可能会被多次挂载，因此每个挂载点都会有一个单独的超级块。
    struct hlist_head fs_supers;

    struct lock_class_key s_lock_key;

    struct lock_class_key s_umount_key;

    ...
};
```

内核中的所有 file_system_type 都是通过一根单向链表组织起来的，register_filesystem() 函数负责将新的 file_system_type 加入到这个链表中。

每个文件系统类型下都挂载了多个文件系统，比如 sda、sdb 都是 ext4 文件系统，这些 super_block 以链表的形式连接到 `file_system_type->fs_supers` 字段中。系统中所有的 super_block 也是通过一根双向链表进行连接。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241227120407496.png" alt="image-20241227120407496" style="zoom:75%;" />

在模块加载函数中，将文件系统注册到内核，需要做以下几步：

1. 初始化 `struct file_system_type` 结构体类型的实体，并填充相应的字段以及回调函数。
2. 调用 register_filesystem() 函数。

例如，ramfs 的部分代码如下：

```c
static struct file_system_type ramfs_fs_type = {
    .name = "ramfs",
    .mount = ramfs_mount,
    .kill_sb = ramfs_kill_sb,
    .fs_flags = FS_USERNS_MOUNT,
};

static int __init init_ramfs_fs(void)
{
    if (test_and_set_bit(0, &once)) return 0;


    return register_filesystem(&ramfs_fs_type);
}
```

## mount() 和 kill_sb()

加载文件系统时，内核调用 `struct file_system_type` 结构定义的 mount() 函数。此函数对每个文件系统都是唯一的，进行初始化操作以后返回挂载点的目录 dentry 指针。mount() 函数一般会调用以下函数之一：

1. mount_bdev()：挂载存储在块设备上的文件系统。
2. mount_single()：挂载一个在所有挂载操作之间是共享实例的文件系统。
3. mount_nodev()：挂载不在物理设备上的文件系统。
4. mount_pseudo()：用于伪文件系统的辅助函数（如 sockfs、pipefs 等无法被挂载的文件系统）。

这些函数的其中一个参数是指向 fill_super() 函数的指针，该函数在超级块初始化后被调用，借助驱动程序完成超级块的初始化。

卸载文件系统时，内核调用 kill_sb() 函数，执行清理动作。kill_sb() 函数一般会调用以下函数之一：

1. kill_block_super()：卸载块设备上的文件系统。
2. kill_anon_super()：卸载虚拟文件系统（当请求时生成信息）。
3. kill_litter_super()：卸载不在物理设备上的文件系统（信息保存在内存中）。

对没有磁盘支持的文件系统，一个实例是 ramfs 文件系统的 ramfs_mount() 函数：

```c
struct dentry *ramfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data)
{
    return mount_nodev(fs_type, flags, data, ramfs_fill_super);
}
```

对来自磁盘的文件系统，一个实例是 minix 文件系统的 minix_mount() 函数：

```c
struct dentry *minix_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data)
{
     return mount_bdev(fs_type, flags, dev_name, data, minix_fill_super);
}
```

# super_block

## struct super_block

超级块作为物理实体（磁盘上的实体）存在，也作为 VFS 实体（`struct super_block` 结构）存在。超级块仅包含元信息，并用于从磁盘中读取和写入元数据（如 inode、目录项）。超级块及 `struct super_block` 结构包含有关所使用的块设备、inode 列表、文件系统根目录的 inode 指针以及超级块操作的指针的信息。

struct super_block 定义如下：

```c
struct super_block
{
    ...

    dev_t s_dev;                     // 标识符
    unsigned char s_blocksize_bits;  // 块大小（以位为单位）
    unsigned long s_blocksize;       // 块大小（以字节为单位）
    unsigned char s_dirt;            // 脏标志
    loff_t s_maxbytes;               // 最大文件大小
    struct file_system_type *s_type; // 文件系统类型
    struct super_operations *s_op;   // 超级块方法

    ...
    unsigned long s_flags; // 挂载标志
    unsigned long s_magic; // 文件系统的魔数
    struct dentry *s_root; // 目录挂载点

    ...

    char s_id[32];   // 信息标识符
    void *s_fs_info; // 文件系统私有信息
};
```

超级块存储了文件系统的全局信息：

1. 所使用的物理设备。
2. 块大小。
3. 文件的最大大小。
4. 文件系统类型。
5. 支持的操作。
6. 魔数（用于标识文件系统）。
7. 根目录的 dentry。

另外，`void *s_fs_info` 可用于存储文件系统的私有数据，具体实现时候可加入自己的数据。类似于 `struct file` 的 `void *private_data`。

## super_block 操作

超级块操作由 super_block 描述，定义如下：

```c
struct super_operations
{
    ...

    // 写入与 inode 相关的资源。
    int (*write_inode)(struct inode *, struct writeback_control *wbc);

    // 分配与 inode 相关的资源。
    struct inode *(*alloc_inode)(struct super_block *sb);

    // 释放与 inode 相关的资源。
    void (*destroy_inode)(struct inode *);

    // 卸载时调用，释放文件系统私有数据的任何资源（通常是内存）。
    void (*put_super)(struct super_block *);

    // 在执行 statfs 系统调用时调用（尝试 stat - f 或 df）。此调用必须填充 struct kstatfs 结构的字段，就像在 ext4_statfs() 函数中所做的那样。
    int (*statfs)(struct dentry *, struct kstatfs *);

    // 在内核检测到重新挂载尝试（挂载标志 MS_REMOUNTM）时调用。大部分情况下，需要检测是否尝试从只读切换到读写或反之。这可以简单地通过访问旧标志（在 sb->s_flags 中）和新标志 (flags 参数) 来完成。data 是由 mount() 发送的表示文件系统特定选项的数据的指针。
    int (*remount_fs)(struct super_block *, int *, char *);

    ...
};
```

# fill_super()

fill_super() 函数用于在文件系统加载时的 mount() 函数中调用，**用于超级块初始化的最后一段，包括填充 struct super_block 字段和根目录的 inode 结构的初始化**。

一个实例是 ramfs_fill_super() 函数：

```c
#include <linux/pagemap.h>


#define RAMFS_MAGIC 0x858458f6


static const struct super_operations ramfs_ops = {
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
    .show_options = ramfs_show_options,
};

static int ramfs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct ramfs_fs_info *fsi;
    struct inode *inode;
    int err;

    save_mount_options(sb, data);

    fsi = kzalloc(sizeof(struct ramfs_fs_info), GFP_KERNEL);
    sb->s_fs_info = fsi;
    if (!fsi)
        return -ENOMEM;

    err = ramfs_parse_options(data, &fsi->mount_opts);
    if (err)
        return err;

    sb->s_maxbytes = MAX_LFS_FILESIZE;
    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = RAMFS_MAGIC;
    sb->s_op = &ramfs_ops;
    sb->s_time_gran = 1;

    inode = ramfs_get_inode(sb, NULL, S_IFDIR | fsi->mount_opts.mode, 0);
    sb->s_root = d_make_root(inode);
    if (!sb->s_root)
        return -ENOMEM;

    return 0;
}
```

内核提供了一些实现文件系统结构的通用函数，例如上面的 generic_delete_inode() 和 simple_statfs()。

上面的 ramfs_fill_super() 函数填充了超级块中的一些字段，然后读取根 inode 并分配根 dentry。读取根 inode 在 ramfs_get_inode() 函数中完成，包括使用 new_inode() 函数分配新的 inode 并进行初始化。为了释放 inode，使用了 iput()，并使用 d_make_root() 函数分配根 dentry。

VFS 函数通常以超级块、索引节点或包含指向超级块的指针的目录项作为实参，以便能够轻松访问这些私有数据。

# 缓冲区缓存

**缓冲区缓存是处理块设备读写缓存的内核子系统。**磁盘文件系统的功能与虚拟文件系统类似，唯一区别是使用了缓冲区缓存。基本结构体是 `struct buffer_head`，定义如下：

```c
struct buffer_head
{
    unsigned long b_state;           // 缓冲区的状态。
    struct buffer_head *b_this_page; // circular list of page's buffers
    struct page *b_page;             // the page this bh is mapped to

    sector_t b_blocknr; // 已加载或需要保存在磁盘上的设备的块号。
    size_t b_size;      // 缓冲区大小。
    char *b_data;       // 指向读取数据或写入数据的内存区域的指针。

    struct block_device *b_bdev;       // 块设备。
    bh_end_io_t *b_end_io;             // I/O completion
    void *b_private;                   // reserved for b_end_io
    struct list_head b_assoc_buffers;  // associated with another mapping
    struct address_space *b_assoc_map; // mapping this buffer is associated with
    atomic_t b_count;                  // users using this buffer_head
    spinlock_t b_uptodate_lock;        // Used by the first bh in a page, to serialise IO completion of other buffers in the page
};
```

以下函数一般会与 `struct buffer_head` 一起使用：

1. `__bread()`：读取具有给定编号和给定大小的块到一个 `struct buffer_head` 中。如果成功，则返回指向 `struct buffer_head` 的指针，否则返回 NULL。
2. sb_bread()：与前一个函数相同，但读取的块的大小从超级块中获取，读取的设备也从超级块中获取。
3. mark_buffer_dirty()：将缓冲区标记为脏（设置 BH_Dirty 位）。缓冲区将在稍后的时间写入磁盘 (bdflush 内核线程会定期唤醒并将缓冲区写入磁盘)。
4. brelse()：在先前将缓冲区写入磁盘（如果需要）后，释放缓冲区使用的内存。
5. map_bh()：将 buffer-head 与相应的扇区关联。

# 函数和有用的宏

超级块通常包含以位图（位向量）形式表示的占用块的映射（索引节点、目录条目、数据占用）。为处理这种映射，建议使用以下功能：

1. find_first_zero_bit()：用于在内存区域中查找第一个为零的位。size 参数表示搜索区域中的位数。
2. test_and_set_bit()：设置位并获取旧值。
3. test_and_clear_bit()：删除位并获取旧值。
4. test_and_change_bit()：取反位的值并获取旧值。

以下宏定义可用于验证索引节点的类型：

1. S_ISDIR(inode->i_mode)：用于检查索引节点是否为目录。
2. S_ISREG(inode->i_mode)：用于检查索引节点是否为普通文件（非链接或设备文件）。

# inode

inode 是文件系统的元数据（它包含信息的信息）。inode 是磁盘上文件的唯一标识，保存文件的信息（uid、gid、访问权限、访问时间以及指向数据块的指针等）。重要的一点是，inode 不保存文件名信息，文件名由相关的 `struct dentry` 结构保存。

inode 用于引用磁盘上的文件。对于进程打开的文件，使用 `struct file` 结构。一个 inode 可以关联一个或多个 `struct file` 结构。多个进程可以打开同一个文件，一个进程可以多次打开同一个文件。

inode 既存在于内存中的 VFS 结构，也存在于磁盘中（UNIX、HFS 以及 NTFS 等）。VFS 中的 inode 由 `struct inode` 结构表示。和 VFS 中的其他结构一样，`struct inode` 是通用结构，涵盖了所有支持的文件类型的选项，甚至包括那些没有关联磁盘实体的文件类型（例如 FAT 文件系统）。

## struct inode

`struct inode` 定义如下：

```c
struct inode
{
    umode_t i_mode; // i_uid 以及 i_gid：访问权限、uid 以及 gid。
    unsigned short i_opflags;
    kuid_t i_uid;
    kgid_t i_gid;
    unsigned int i_flags;

#ifdef CONFIG_FS_POSIX_ACL
    struct posix_acl *i_acl;
    struct posix_acl *i_default_acl;
#endif

    const struct inode_operations *i_op; // 指向结构 inode_operations 的指针。
    struct super_block *i_sb;            // inode 所属的文件系统的超级块结构。
    struct address_space *i_mapping;     // i_mapping->a_ops 包含指向 struct address_space_operations 的指针。

#ifdef CONFIG_SECURITY
    void *i_security;
#endif

    /* Stat data, not accessed from path walking */
    unsigned long i_ino; // inode 的编号（在文件系统内唯一标识 inode）。
    /*
     * Filesystems may only read i_nlink directly.  They shall use the
     * following functions for modification:
     *
     *    (set|clear|inc|drop)_nlink
     *    inode_(inc|dec)_link_count
     */

    // 使用此 inode 的名称条目（dentry）的数量；对于没有链接（既没有硬链接也没有符号链接）的文件系统，这个值总是设置为 1。
    union
    {
        const unsigned int i_nlink;
        unsigned int __i_nlink;
    };

    dev_t i_rdev;              // 挂载的文件系统所在的设备。
    loff_t i_size;             // 文件/目录等的大小（以字节为单位）。
    struct timespec64 i_atime; // 访问时间。
    struct timespec64 i_mtime; // 修改时间。
    struct timespec64 i_ctime; // 创建时间。
    spinlock_t i_lock;         /* i_blocks, i_bytes, maybe i_size */
    unsigned short i_bytes;
    u8 i_blkbits; // 块大小使用的比特数 == log2(块大小)。
    u8 i_write_hint;
    blkcnt_t i_blocks; // 文件使用的块数（所有块，不仅仅是数据块）。这仅由配额子系统使用。

#ifdef __NEED_I_SIZE_ORDERED
    seqcount_t i_size_seqcount;
#endif

    /* Misc */
    unsigned long i_state;
    struct rw_semaphore i_rwsem;

    unsigned long dirtied_when; /* jiffies of first dirtying */
    unsigned long dirtied_time_when;

    struct hlist_node i_hash;
    struct list_head i_io_list; /* backing dev IO list */
#ifdef CONFIG_CGROUP_WRITEBACK
    struct bdi_writeback *i_wb; /* the associated cgroup wb */

    /* foreign inode detection, see wbc_detach_inode() */
    int i_wb_frn_winner;
    u16 i_wb_frn_avg_time;
    u16 i_wb_frn_history;
#endif
    struct list_head i_lru; /* inode LRU list */
    struct list_head i_sb_list;
    struct list_head i_wb_list; /* backing dev writeback list */
    union
    {
        struct hlist_head i_dentry;
        struct rcu_head i_rcu;
    };
    atomic64_t i_version;
    atomic64_t i_sequence; /* see futex */
    atomic_t i_count;      // inode 计数器，指示有多少内核组件在使用它。
    atomic_t i_dio_count;
    atomic_t i_writecount;
#if defined(CONFIG_IMA) || defined(CONFIG_FILE_LOCKING)
    atomic_t i_readcount; /* struct files open RO */
#endif
    union
    {
        const struct file_operations *i_fop; // 指向结构 file_operations 的指针。former ->i_op->default_file_ops
        void (*free_inode)(struct inode *);
    };
    struct file_lock_context *i_flctx;
    struct address_space i_data;
    struct list_head i_devices;
    union
    {
        struct pipe_inode_info *i_pipe;
        struct cdev *i_cdev;
        char *i_link;
        unsigned i_dir_seq;
    };

    __u32 i_generation;

#ifdef CONFIG_FSNOTIFY
    __u32 i_fsnotify_mask; /* all events this inode cares about */
    struct fsnotify_mark_connector __rcu *i_fsnotify_marks;
#endif

#ifdef CONFIG_FS_ENCRYPTION
    struct fscrypt_info *i_crypt_info;
#endif

#ifdef CONFIG_FS_VERITY
    struct fsverity_info *i_verity_info;
#endif

    void *i_private; /* fs or device private pointer */
} __randomize_layout;
```

每个文件系统都缓存了一定的 inode 数量到内存中。同一个文件系统的 inode 以双向链表连接起来，挂在 `super_block->s_inodes` 字段中。同时，内核中所有的 inode 被组织在了一个哈希表 inode_hashtable 上。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241227141716401.png" alt="image-20241227141716401" style="zoom:75%;" />

一些可用于处理 inode 的函数如下：

1. new_inode()：创建新的 inode，将 i_nlink 字段设置为 1，并初始化 i_blkbits, i_sb 和 i_dev。
2. insert_inode_hash()：将 inode 添加到 inode 哈希表中。这个调用的一个有趣的效果是，如果 inode 被标记为脏，它将被写入磁盘。
3. mark_inode_dirty()：将 inode 标记为脏，稍后它将被写入磁盘。
4. iget_locked()：从磁盘加载具有给定编号的 inode，如果它尚未加载。
5. unlock_new_inode()：与 iget_locked() 一起使用，释放对 inode 的锁定。
6. iput()：告诉内核对 inode 的操作已经完成。若没有其他进程在使用，它将被销毁（如果被标记为脏，则写入磁盘后再销毁）。
7. make_bad_inode()：告诉内核该 inode 无法使用；通常在从磁盘读取 inode 时发现无法读取的情况下使用，表示该 inode 无效。

## inode 操作

### 获取 inode

获取 inode 是 inode 的主要操作之一。Linux 2.6 以前，存在 read_inode() 函数。Linux 2.6 以后，编程者必须自己定义 `<fsname>_get()` 函数，`fsname` 是文件系统的名称。此函数负责查找 VFS 中的 inode，如果存在则获取该 inode，否则创建一个新的 inode，并用磁盘中的信息填充它。

一般情况下，这个函数会调用 iget_locked() 从 VFS 中获取 inode 结构。如果 inode 是新创建的，则需要使用 sb_bread() 从磁盘中读取 inode，并填充有用的信息。

实例函数是 minix_iget()：

```c
static struct inode *V1_minix_iget(struct inode *inode)
{
    struct buffer_head *bh;
    struct minix_inode *raw_inode;
    struct minix_inode_info *minix_inode = minix_i(inode);
    int i;

    raw_inode = minix_V1_raw_inode(inode->i_sb, inode->i_ino, &bh);
    if (!raw_inode)
    {
        iget_failed(inode);
        return ERR_PTR(-EIO);
    }

    ...
}

// 此函数通过 iget_locked() 获取 inode。如果 inode 已经存在即不是新建的，则函数返回。否则使用 V1_minix_iget() 函数从磁盘读取 inode，然后使用读取的信息初始化 VFS inode。
struct inode *minix_iget(struct super_block *sb, unsigned long ino)
{
    struct inode *inode;

    inode = iget_locked(sb, ino);
    if (!inode)
        return ERR_PTR(-ENOMEM);
    // I_NEW 标志表示 inode 是否为新建的。
    if (!(inode->i_state & I_NEW))
        return inode;

    if (INODE_VERSION(inode) == MINIX_V1)
        return V1_minix_iget(inode);

    ...
}
```

### 超级块操作

许多超级块操作在处理 inode 的时候使用，如下：

1. alloc_inode()：分配 inode()。通常，此函数会分配一个 `struct <fsname>_inode_info` 结构，并使用 inode_init_once() 执行基本的 VFS inode 初始化。minix 文件系统使用 kmem_cache_alloc() 函数进行分配，该函数与 SLAB 子系统交互。对于每个分配，都会调用缓存构造函数，在 minix 下是 init_once() 函数。或者也可以使用 kmalloc()。在这种情况下，应调用 inode_init_once() 函数。alloc_inode() 函数将由 new_inode() 和 iget_locked() 函数调用。
2. write_inode()：将作为参数接收的 inode 保存或更新到磁盘。此函数要更新 inode，尽管效率不高。对初学者而言，建议使用以下操作：
   - 使用 sb_bread() 函数从磁盘加载 inode。
   - 根据保存的 inode 修改缓冲区。
   - 使用 mark_buffer_dirty() 将缓冲区标记为脏。内核将处理其在磁盘上的写入。
3. evict_inode()：从磁盘和内存中移除通过 i_ino 字段接收的 inode 的任何信息，包括磁盘上的 inode 和相关的数据块。涉及以下操作：
   - 从磁盘中删除 inode。
   - 更新磁盘位图（如果有）。
   - 通过调用 truncate_inode_pages() 从 page cache 中删除 inode。
   - 通过调用 clear_inode() 从内存中删除 inode。
4. destroy_inode()：释放 inode 占用的内存。

### inode_operations

inode 索引节点的相关操作由 `struct inode_operations` 结构描述。

索引节点分为多种类型：文件、目录、特殊文件（管道、FIFO）、块设备、字符设备以及链接等。每种类型需要实现的操作都不同。

访问 `struct inode` 中的 i_op 字段可以对索引节点的操作进行初始化和访问。

# mount

**mount 代表了一个文件系统被挂载到了某个地方。**只有被挂载的文件系统，才能通过 VFS 的目录树进行访问。

一个文件系统可能被多次 mount 到不同的地方，这样一个 super_block 会对应多个不同的 mount 结构，这些 mount 以双向链表的形式组织起来，挂在 super_block->s_mounts 字段。

被 mount 的目录称为一个 mount 点。目录也是一个 dentry，mount 通过 mnt_mountpoint 字段指向该 dentry。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241227145405084.png" alt="image-20241227145405084" style="zoom:75%;" />

挂载点用 mountpoint 结构体表示。所有的挂载点被放到一个哈希表 mountpoint_hashtable 中，以 dentry 为键（Key），mountpoint 为值（T）。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241230094606623.png" alt="image-20241230094606623" style="zoom:75%;" />

> 注：这里的 mountpoint 是一个结构体。与上面的 mount->mnt_mountpoint 不一样，上面的是一个指针，指向 dentry。

```c
struct mount {
	struct hlist_node mnt_hash;
	struct mount *mnt_parent;
	struct dentry *mnt_mountpoint;
	struct vfsmount mnt;
	union {
		struct rcu_head mnt_rcu;
		struct llist_node mnt_llist;
	};
#ifdef CONFIG_SMP
	struct mnt_pcp __percpu *mnt_pcp;
#else
	int mnt_count;
	int mnt_writers;
#endif
	struct list_head mnt_mounts;	/* list of children, anchored here */
	struct list_head mnt_child;	/* and going through their mnt_child */
	struct list_head mnt_instance;	/* mount instance on sb->s_mounts */
	const char *mnt_devname;	/* Name of device e.g. /dev/dsk/hda1 */
	struct list_head mnt_list;
	struct list_head mnt_expire;	/* link in fs-specific expiry list */
	struct list_head mnt_share;	/* circular list of shared mounts */
	struct list_head mnt_slave_list;/* list of slave mounts */
	struct list_head mnt_slave;	/* slave list entry */
	struct mount *mnt_master;	/* slave is on master->mnt_slave_list */
	struct mnt_namespace *mnt_ns;	/* containing namespace */
	struct mountpoint *mnt_mp;	/* where is it mounted */
	union {
		struct hlist_node mnt_mp_list;	/* list mounts with the same mountpoint */
		struct hlist_node mnt_umount;
	};
	struct list_head mnt_umounting; /* list entry for umount propagation */
#ifdef CONFIG_FSNOTIFY
	struct fsnotify_mark_connector __rcu *mnt_fsnotify_marks;
	__u32 mnt_fsnotify_mask;
#endif
	int mnt_id;			/* mount identifier */
	int mnt_group_id;		/* peer group identifier */
	int mnt_expiry_mark;		/* true if marked for expiry */
	struct hlist_head mnt_pins;
	struct hlist_head mnt_stuck_children;
} __randomize_layout;

struct mountpoint {
	struct hlist_node m_hash;
	struct dentry *m_dentry;
	struct hlist_head m_list;
	int m_count;
};
```

同一个目录可被多个文件系统 mount。这些文件系统会相互覆盖，通过 VFS 只能看到最近那个被 mount 的文件系统。

为了处理这种情况，文件系统中所有的 mount 都被组织到同一个哈希表 mount_hashtable 中。哈希表以 `<mount, dentry>` 为键（Key），以新的 mount 作为值（Value），将其组织起来。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241230101507978.png" alt="image-20241230101507978" style="zoom:70%;" />

将上述整理以后，能得到一个整体的架构图：

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241230102023524.png" alt="image-20241230102023524" style="zoom:70%;" />

# file

**file 结构对应于由进程打开的文件，仅存在于内存中，并与 inode 索引节点关联。**它是最接近用户空间的 VFS 实体。结构字段包含用户空间文件的熟悉信息（访问模式、文件位置等），与之相关的操作由已知的系统调用（read, write 等）执行。

文件操作由 `struct file_operations` 结构描述。文件系统的文件操作使用 `struct inode` 结构中的 i_fop 字段进行初始化。在打开文件时，VFS 使用 inode->i_fop 的地址初始化 `struct file` 结构的 f_op 字段。后续的系统调用使用存储在 file->f_op 中的值。

file 与 inode 的区别在于，一个文件的 inode 在内核中是唯一的，但 file 可以有多个。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241230104309655.png" alt="image-20241230104309655" style="zoom:70%;" />

# 常规文件索引节点

使用索引节点必须要填充 inode 结构的 i_op 和 i_fop 字段。索引节点的类型决定了他要实现的操作。

一个例子是 minix 文件系统的对象实例 minix_file_operations 和 minix_file_inode_operations。

Linux 内核实现了 generic_file_llseek()、generic_file_read_iter()、generic_file_write_iter()、generic_file_mmap() 函数，定义了一些通用的 file 操作，具体做了哪些处理可参见源码。

```c
const struct file_operations minix_file_operations = {
    .llseek = generic_file_llseek,
    .read_iter = generic_file_read_iter,
    //...
    .write_iter = generic_file_write_iter,
    //...
    .mmap = generic_file_mmap,
    //...
};

const struct inode_operations minix_file_inode_operations = {
    .setattr = minix_setattr,
    .getattr = minix_getattr,
};


{
    //...

    if (S_ISREG(inode->i_mode))
    {
        inode->i_op = &minix_file_inode_operations;
        inode->i_fop = &minix_file_operations;
    }

    //...
}
```

对于简单的文件系统，只需实现截断 truncate() 系统调用。从 Linux 3.14 开始，该操作已嵌入到 setattr() 中。如果粘贴大小与索引节点的当前大小不同，则必须执行截断操作。

一个例子是 minix_setattr() 函数：

```c
static int minix_setattr(struct dentry *dentry, struct iattr *attr)
{
    struct inode *inode = d_inode(dentry);
    int error;

    error = setattr_prepare(dentry, attr);
    if (error)
        return error;

    if ((attr->ia_valid & ATTR_SIZE) &&
        attr->ia_size != i_size_read(inode))
    {
        error = inode_newsize_ok(inode, attr->ia_size);
        if (error)
            return error;

        truncate_setsize(inode, attr->ia_size);
        minix_truncate(inode);
    }

    setattr_copy(inode, attr);
    mark_inode_dirty(inode);

    return 0;
}
```

截断操作涉及以下内容：

1. 释放磁盘上多余的数据块（如果新尺寸小于旧尺寸），或者分配新的数据块（当新尺寸较大时）。
2. 更新磁盘位图（如果使用）。
3. 更新索引节点。
4. 使用 block_truncate_page() 函数，将上一个块中未使用的空间填充为零。

# page cache

笔记摘抄自文章 [https://blog.ywang-wnlo.xyz/posts/9ba60726/](https://blog.ywang-wnlo.xyz/posts/9ba60726/)。

由于磁盘 HDD 以及现在广泛使用的固态硬盘 SSD 的读写速度都远小于内存 DRAM 的读写速度。为避免每次读取数据都要直接访问这些低速的底层存储设备，Linux 利用 DRAM 实现了一个缓存层，缓存的粒度是 page，也叫 page cache，也就是页（面）缓存。

经过这层 page cache 的作用，I/O 的性能得到了显著的提升。不过由于 DRAM 具有易失性，在掉电后数据会丢失，因此内核中的 回写机制定时将 page cache 中的数据下刷到设备上，保证数据的持久化。此外内核还在 page cache 中实现了巧妙的预读机制，提升了顺序读性能。

写入到 page cache 的数据不会立刻写入后端设备，而是标记为“脏”，并被加入到脏页链表，后续由内核中的回写进程周期性的将脏页写回到底层存储设备。

在拥有 page cache 这一层后，写数据就有了三种不同的策略：

1. **不经过缓存，直接写底层存储设备，但同时要使缓存中数据失效，也叫不缓存（nowrite）。**

2. **只写缓存，缓存中数据定期刷到底层存储设备上，也叫写回（write back）。**

3. **同时写缓存和底层存储设备，也叫写穿（write through）。**

前两种就是直接 I/O（direct_io）和缓存 I/O（buffer_io）。

第三种策略虽然能非常简单保证缓存和底层设备的一致性，不过基于时间局部性原理，page cache 中的数据可能只是中间态，会被频繁修改，每次写穿会产生大量的开销。

关于 page cache 的写回机制（write back），参考 [https://blog.ywang-wnlo.xyz/posts/646202b9/](https://blog.ywang-wnlo.xyz/posts/646202b9/)。

# address_space

**进程的地址空间与文件之间有着密切的联系：程序的执行几乎完全是通过将文件映射到进程的地址空间中进行的。**这种方法非常有效且相当通用，也可以用于常规的系统调用，如 read() 和 write()。

描述地址空间的结构是 `struct address_space`，与之相关的操作由结构 `struct address_space_operations` 描述。初始化 `struct address_space_operations` 需填充文件类型索引节点的 `inode->i_mapping->a_ops`。

> `struct address_space` 是 page cache 的核心结构体。每一个 address_space 与一个 inode 对应，同时 file 中的 f_mapping 字段通常由该文件的 inode 中 i_mapping 赋值。也就是说每个文件都会有独自的 file、inode 以及 address_space 结构体。
>
> `struct address_space` 中的 `struct xarray i_pages` 就是该文件的 page cache 中缓存的所有物理页。它是通过基数树结构进行管理的，而 xarray 只是在基数树上进行了一层封装。
>
> 通常 `struct address_space` 上会挂载一个 `struct address_space_operations`，自定义对 page cache 中的页面操作的函数。

```c
struct address_space {
	struct inode		*host;
	struct xarray		i_pages;
	struct rw_semaphore	invalidate_lock;
	gfp_t			gfp_mask;
	atomic_t		i_mmap_writable;
#ifdef CONFIG_READ_ONLY_THP_FOR_FS
	/* number of thp, only for non-shmem files */
	atomic_t		nr_thps;
#endif
	struct rb_root_cached	i_mmap;
	struct rw_semaphore	i_mmap_rwsem;
	unsigned long		nrpages;
	pgoff_t			writeback_index;
	const struct address_space_operations *a_ops;
	unsigned long		flags;
	errseq_t		wb_err;
	spinlock_t		private_lock;
	struct list_head	private_list;
	void			*private_data;
} __attribute__((aligned(sizeof(long)))) __randomize_layout;

...

struct address_space_operations {
	int (*writepage)(struct page *page, struct writeback_control *wbc);
	int (*readpage)(struct file *, struct page *);

	/* Write back some dirty pages from this mapping. */
    // writepage() 或 writepages() 负责对这些物理页的实际写入。
	int (*writepages)(struct address_space *, struct writeback_control *);

	/* Set a page dirty.  Return true if this dirtied it */
	int (*set_page_dirty)(struct page *page);

	/*
	 * Reads in the requested pages. Unlike ->readpage(), this is
	 * PURELY used for read-ahead!.
	 */
	int (*readpages)(struct file *filp, struct address_space *mapping,
			struct list_head *pages, unsigned nr_pages);
	void (*readahead)(struct readahead_control *);

    // 主要负责查找、或者分配新的物理页，并将其锁定，有时还需要先从底层读取最新的数据页。
	int (*write_begin)(struct file *, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned flags,
				struct page **pagep, void **fsdata);
    // 主要负责解锁这些物理页，并且更新 inode 中的元数据信息，例如 i_size。
	int (*write_end)(struct file *, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned copied,
				struct page *page, void *fsdata);

	/* Unfortunately this kludge is needed for FIBMAP. Don't use it */
	sector_t (*bmap)(struct address_space *, sector_t);
	void (*invalidatepage) (struct page *, unsigned int, unsigned int);
	int (*releasepage) (struct page *, gfp_t);
	void (*freepage)(struct page *);
	ssize_t (*direct_IO)(struct kiocb *, struct iov_iter *iter);
	/*
	 * migrate the contents of a page to the specified target. If
	 * migrate_mode is MIGRATE_ASYNC, it must not block.
	 */
	int (*migratepage) (struct address_space *,
			struct page *, struct page *, enum migrate_mode);
	bool (*isolate_page)(struct page *, isolate_mode_t);
	void (*putback_page)(struct page *);
	int (*launder_page) (struct page *);
	int (*is_partially_uptodate) (struct page *, unsigned long,
					unsigned long);
	void (*is_dirty_writeback) (struct page *, bool *, bool *);
	int (*error_remove_page)(struct address_space *, struct page *);

	/* swapfile support */
	int (*swap_activate)(struct swap_info_struct *sis, struct file *file,
				sector_t *span);
	void (*swap_deactivate)(struct file *file);
};
```

address_space 是以基数树进行组织的文件 Cache。以页为单位，`page->index = 该页在文件中的逻辑偏移 / page_size`。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241230103801350.png" alt="image-20241230103801350" style="zoom:30%;" />

例如 minix 文件系统的 minix_aops 结构如下：

```c
static const struct address_space_operations minix_aops = {
    .readpage = minix_readpage,
    .writepage = minix_writepage,
    .write_begin = minix_write_begin,
    .write_end = generic_write_end,
    .bmap = minix_bmap};


{
    ...

    if (S_ISREG(inode->i_mode))
    {
        inode->i_mapping->a_ops = &minix_aops;
    }

    ...
}
```

内核已实现 generic_write_end() 函数。并且上述的大多数函数的实现其实都非常简单：

```c
static int minix_writepage(struct page *page, struct writeback_control *wbc)
{
    return block_write_full_page(page, minix_get_block, wbc);
}

static int minix_readpage(struct file *file, struct page *page)
{
    return block_read_full_page(page, minix_get_block);
}

static void minix_write_failed(struct address_space *mapping, loff_t to)
{
    struct inode *inode = mapping->host;

    if (to > inode->i_size)
    {
        truncate_pagecache(inode, inode->i_size);
        minix_truncate(inode);
    }
}

static int minix_write_begin(struct file *file, struct address_space *mapping,
                             loff_t pos, unsigned len, unsigned flags,
                             struct page **pagep, void **fsdata)
{
    int ret;

    ret = block_write_begin(mapping, pos, len, flags, pagep,
                            minix_get_block);
    if (unlikely(ret))
        minix_write_failed(mapping, pos + len);

    return ret;
}

static sector_t minix_bmap(struct address_space *mapping, sector_t block)
{
    return generic_block_bmap(mapping, block, minix_get_block);
}
```

上面函数中能经常见到 minix_get_block 这个东西。查看 block_write_full_page() 函数定义发现是一个函数指针。

在 minix 文件系统中，minix_get_block() 函数将文件的一个数据块转换为设备上的一个数据块。如果接收到的 create 标志被设置，那么必须分配一个新的数据块。在创建新的数据块时，必须相应地更新位图。为通知内核不要从磁盘中读取该数据块，必须使用 set_buffer_new() 函数标记 bh。通过 map_bh() 函数，将缓冲区与数据块关联起来。

```c
int block_write_full_page(struct page *page, get_block_t *get_block, struct writeback_control *wbc);

typedef int (get_block_t)(struct inode *inode, sector_t iblock, struct buffer_head *bh_result, int create);
```

# dentry

## struct dentry

dentry 将 inode 和 文件名关联起来。VFS 中的 dentry 实体用 `struct dentry` 表示，相关操作用 `struct dentry_operations` 表示。

`struct dentry` 重要字段定义如下：

```c
struct dentry
{
    ...

    struct inode *d_inode; // 关联的索引节点。

    ...

    struct dentry *d_parent; // 父目录的 dentry 对象。
    struct qstr d_name;      // dentry 名称，struct qstr 类型，包含字段 name（名称）和 len（名称的长度）。

    ...

    struct dentry_operations *d_op; // 与 dentry 相关的操作。内核实现了默认操作，理论上无需重新实现它们。某些文件系统可以根据 dentry 的特定结构进行优化。
    struct super_block *d_sb;       // 文件的超级块。
    void *d_fsdata;                 // 文件系统特定的数据。

    ...
};
```

dentry 也有一个全局哈希表进行组织，与它对应的 inode 互指。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241230102953331.png" alt="image-20241230102953331" style="zoom:75%;" />

## dentry 操作

dentry 最常见的操作包括：

1. d_make_root()：分配根 dentry。通常在读取超级块的函数 fill_super() 中使用。此函数必须初始化根目录。一般从超级块获取根索引节点，并将其作为实参传递给此函数，以填充 struct super_block 结构的 s_root 字段。
2. d_add()：将 dentry 与索引节点关联起来。作为参数传递的 dentry 表示需要创建的条目（名称、长度）。在创建或加载尚未与任何 dentry 关联并尚未添加到索引节点哈希表中的新索引节点时，将使用此函数（在 lookup() 函数中）。
3. d_instantiate()：d_add() 的轻量级版本，其中 dentry 先前已添加到哈希表中。注意，d_instantiate() 必须用于实现创建调用 (mkdir, mknod, rename 以及 symlink)，而不是 d_add。

# 目录索引节点

## 目录索引节点操作

目录索引节点的操作比常规文件索引节点的操作要复杂的多。在 minix 中，由对象实例 minix_dir_inode_operations 和 minix_dir_operations 定义。

```c
struct inode_operations minix_dir_inode_operations = {
    .create = minix_create,
    .lookup = minix_lookup,
    .link = minix_link,
    .unlink = minix_unlink,
    .symlink = minix_symlink,
    .mkdir = minix_mkdir,
    .rmdir = minix_rmdir,
    .mknod = minix_mknod,
    //...
};

struct file_operations minix_dir_operations = {
    .llseek = generic_file_llseek,
    .read = generic_read_dir,
    .iterate = minix_readdir,
    //...
};


{
    //...

    if (S_ISDIR(inode->i_mode))
    {
        inode->i_op = &minix_dir_inode_operations;
        inode->i_fop = &minix_dir_operations;
        inode->i_mapping->a_ops = &minix_aops;
    }

    //...
}
```

## 相关函数

目录索引节点操作的相关函数如下所述。

### 创建索引节点

由 inode_operations 的 create 字段（回调函数）表示。此函数由 open() 和 creat() 系统调用调用，执行以下操作：

1. 在磁盘上的物理结构中引入新条目。不要忘记更新磁盘上的位图。
2. 使用传入函数的访问权限配置访问权限。
3. 使用 mark_inode_dirty() 函数将索引节点标记为脏。
4. 使用 d_instantiate() 函数实例化目录条目 (dentry)。

### 创建目录

由 mkdir 字段表示，由 mkdir() 系统调用调用，执行以下操作：

1. 调用 create 字段对应的回调函数。
2. 为目录分配一个数据块。
3. 创建 `"."` 和 `".."` 条目。

### 创建链接

由 link 字段表示，由 link() 系统调用调用，执行以下操作：

1. 将新的 dentry 绑定到索引节点。
2. 递增索引节点的 i_nlink 字段。
3. 使用 mark_inode_dirty() 函数将索引节点标记为脏。

### 创建符号链接

由 symlink 字段表示，由 symlink() 系统调用调用。执行操作与 link 的回调函数类似，区别在于此函数创建的是符号链接。

### 删除链接

由 unlink 字段表示，由 unlink() 系统调用调用，执行以下操作：

1. 从物理磁盘结构中删除作为参数给出的 dentry。
2. 将条目指向的索引节点的 i_nlink 计数器减一，否则该索引节点将永远不会被删除（引用计数无法减到 0）。

### 删除目录

由 rmdir 字段表示，由 rmdir() 系统调用调用，执行以下操作：

1. 执行 unlink 字段对应回调函数完成的操作。
2. 确保目录为空，否则返回 ENOTEMPTY。
3. 同时删除数据块。

### 在目录中搜索索引节点

由 lookup 字段表示。当需要有关与目录中条目关联的索引节点的信息时，会间接调用此函数。此函数执行以下操作：

1. 在由 dir 指示的目录中搜索具有名称 `dentry->d_name.name` 的条目。
2. 如果找到条目，则返回 NULL 并使用 d_add() 函数将索引节点与名称关联。
3. 否则，返回 ERR_PTR。

### 遍历目录中的条目

由 iterate 字段表示，由 readdir() 系统调用调用。

此函数返回目录中的所有条目，或者当为其分配的缓冲区不可用时，仅返回部分条目。可能的返回如下：

1. 如果对应的用户空间缓冲区有足够的空间，则返回与现有条目数相等的数字。
2. 小于实际条目数的数字，对应的用户空间缓冲区中有多少空间，就返回多少。
3. 0，表示没有更多条目可读取。

此函数会连续调用，知道读取完所有可用的条目，并且至少会调用 2 次。

1. 在以下情况下仅调用两次：
   - 第一次调用读取所有条目并返回它们的数量。
   - 第二次调用返回 0，表示没有其他条目可读取。
2. 如果第一次调用未返回总条目数，则会多次调用该函数。

此函数执行以下操作：

1. 遍历当前目录中的条目（dentry）。
2. 对于找到的每个 dentry，递增 `ctx->pos`。
3. 对于每个有效的 dentry（例如，除了 0 之外的索引节点），调用 dir_emit() 函数。
4. 如果 dir_emit() 函数返回非零值，表示用户空间的缓冲区已满，函数将返回。

dir_emit() 定义如下：

```c
// ctx：目录遍历上下文，作为参数传递给 iterate 函数。
// name：条目的名称。
// namelen：条目名称的长度。
// ino：与条目关联的 inode 索引节点号。
// type：标志条目类型，DT_REG（文件）、DT_DIR（目录）、DT_UNKNOWN（未知）等。
static inline bool dir_emit(struct dir_context *ctx, const char *name, int namelen, u64 ino, unsigned type)
{
	return ctx->actor(ctx, name, namelen, ctx->pos, ino, type) == 0;
}
```

# 位图操作

处理文件系统时，管理信息（哪个块是空闲的或忙碌的，哪个索引节点是空闲的或忙碌的）使用位图存储。因此需要使用位操作，包括：

1. 搜索第一个为 0 的位：表示一个空闲的块或索引节点。
2. 将位标记为 1：标记忙碌的块或索引节点。

位图操作常见的函数如下。这些函数定义在内核源码 include/asm-generic/bitops/ 目录下，特别是 find.h 和 atomic.h 中。

1. find_first_zero_bit()
2. find_first_bit()
3. set_bit()
4. clear_bit()
5. test_and_set_bit()
6. test_and_clear_bit()

这些函数通常接收位图的地址，可能还有其大小（以字节为单位）。如果需要，还要指定需要激活（设置）或停用（清除）的位的索引。

一个使用实例如下：

```c
unsigned int map;
unsigned char array_map[NUM_BYTES];
size_t idx;
int changed;

/* 在 32 位整数中找到第一个为 0 的位。 */
idx = find_first_zero_bit(&map, 32);
printk(KERN_ALERT "第 %zu 位是第一个为 0 的位。\n", idx);

/* 在 NUM_BYTES 字节的数组中找到第一个为 1 的位。 */
idx = find_first_bit(array_map, NUM_BYTES * 8);
printk(KERN_ALERT "第 %zu 位是第一个为 1 的位。\n", idx);

/*
 * 清除整数中的第 idx 位。
 * 假设 idx 小于整数的位数。
 */
clear_bit(idx, &map);

/*
 * 测试并设置数组中的第 idx 位。
 * 假设 idx 小于数组的位数。
 */
changed = __test_and_set_bit(idx, &sbi->imap);
if (changed)
    printk(KERN_ALERT "%zu 位已更改\n", idx);
```

# 流程分析

## 文件系统整体运行流程

1. 加载文件系统的内核模块，在 init 中，注册需要的 file_system_type。格式化过程在内核代码没有任何体现。

2. 调用 mount()，挂载文件系统，通过 file_system_type 的 mount() 回调加载对应的 super_block。

3. 通过 super_block 的 `s_op->alloc_inode()` 分配一个 inode。

4. 分配 root 的 dentry，调用 `dentry->d_op` 初始化 dentry。inode_operation 和 dentry_operation 都被记录在 super_block 中，inode 和 dentry 各自在初始化时拷贝了该指针。

5. 设置对应的挂载点，mount 过程完成。

6. 应用程序 open 文件，从指定路径一级一级向下读取对应的 dentry，直到找到需要的文件的 dentry。查找时优先从 dentry 的全局唯一哈希表上查。如果哈希表没有数据，则调用 `inode->i_op->lookup()` 查找。如果最后发现没有这样的文件，则可能调用 `inode->i_op->atomic_open()` 和 `inode->i_op->create()`。在确保有文件的情况下，调用 `file->f_op->open()` 来打开文件。file 的 address_space 和 f_op 由 inode 赋予。

7. read 文件。如果文件加了范围锁，则需判断是否有冲突，然后调用 `file->f_op->read()` 或者 `file->f_op->read_iter()`。

8. write 文件。如果文件加了范围锁，则需判断是否有冲突，然后调用 `file->f_op->write()` 或者 `file->f_op->write_iter()`。

9. close 文件，先调用 `file->f_op->flush()` 刷数据，然后进行异步关闭操作。

> 如果当前进程不在中断上下文且不是 kthread 线程，将该文件的 close() 操作注册到 current->task_works 中。否则，将该文件的 close() 操作注册到全局的 delayed_fput_work 中。最终，两个异步线程会调到相同的回收代码中来。如果文件设置了 FASYNC 标志，调用 `file->f_op->fasync()` 函数，否则调用 `file->f_op->release()` 函数。

10. 调用 umount()，卸载文件系统，触发 `super_block->s_op->kill_sb()` 回调。

## path lookup 过程

path lookup 是通过用户传递的一个绝对或相对路径，来找到对应文件的 inode 的过程。典型的应用如 open() 和 mount() 的查找。

**path lookup 总共可分为 ref-walk 和 rcu-walk 两种模式。**

RCU 模式对锁的争用更少，并发更好，但不适合所有场景（因为 RCU 可能会导致进程睡眠）。ref 模式是传统的 path lookup 方式，不容易失败。

RCU 模式为了检测 dentry 的修改（rename）带来的查询失败，每次查询都会记录 dentry->d_seq，在查询结束后会检测当前 dentry 和父目录 dentry 的 d_seq 是否改变。针对 ref 模式，每次都会锁住当前 dentry 的 d_lock，在成功查询到需要的 dentry 后，会将其引用计数加一。

当从当前目录跳转到下一层目录时，RCU 模式会丢弃掉原来的父目录的 d_seq 记录（因为不用关心祖父目录的引用计数），而 ref 模式则会丢弃对当前目录的引用。

## mount 过程

mount 系统调用定义如下。更多细节参考博客 [https://blog.csdn.net/bingyu880101/article/details/50481507](https://blog.csdn.net/bingyu880101/article/details/50481507)。

```c
#include <sys/mount.h>

// source：要挂上的文件系统的名字，通常是一个设备名。
// target：文件系统要挂在的目标目录。
// filesystemtype：挂载的文件系统类型，如 "ext4"、"btrfs"、"msdos"、"proc"、"nfs"、"iso9660"、"vfat" 等。
// mountflags：指定文件系统的读写访问标志。
// data：文件系统持有的参数。
int mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *_Nullable data);
```

mount 的过程具体如下：

1. 根据 dir_name，进行 path lookup。

2. 根据 type，查找对应的 file_system_type。

3. 拿到 file_system_type，调 mount() 回调，将 dev_name 对应的块设备和 data 传递给它，mount() 回调将建立好对应的 root 的 dentry，super_block 和 root 的 inode。

4. 新建 mount 结构体，将 dentry 与 mount 结构体绑定。

5. 在 mount_hashtable 中不断查找。如果找到匹配的 mount 结构体，说明该挂载点已被使用，需要继续查找。直到找不到对应的 mount 结构体，说明当前挂载点尚未被占用，系统可以在此挂载新的文件系统。此时系统最后得到有效的 mount 结构体就作为当前 mount 结构体的父挂载点，得到的 dentry 作为当前 mount 结构体的 mountpoint。

6. 建立父 mount 与当前 mount 的联系，建立 mountpoint 与当前 mount 的联系。

## open 过程

open 主要的流程如下：

1. 分析 open 传进来的 flags。

2. 分配 fd。

3. 对文件执行 open、create 等操作（视具体情况而定）。

4. 通知监控文件打开的回调。

5. 将打开得到的 file 结构体放到 fdtable 的 fd 数组中。

open 具体查找文件 inode 的过程，即是 path lookup 的过程。

`file->f_op` 有一个 atomic_open() 回调，允许文件系统以与原子的方式查找某个文件。如果该文件不存在，则文件系统尝试创建该文件（当设置了 O_CREAT 标志时）。故在尝试查找和创建文件时，VFS 优先使用 atomic_open()，当文件系统不支持该操作时，才回归到先 lookup，查找失败再 create 的模式。

## 通用 read 流程

这里不考虑文件的异步读写，也就是 aio 系列的 read 和 write。

所谓通用，是指某些文件系统不单独写 read() 或 read_iter() 回调，而是调 VFS 实现的默认 read 函数 generic_file_read_iter()。

在读缓存的过程中，如果不允许进程阻塞，且需要的数据不在内存中，会立即返回失败。

读分为两种，一种是 direct_io，另一种是走 address_space。

如果走 direct_io：

1. 如果这个 read 操作不能陷入等待（NO_WAIT），且要读取的文件范围内有缓存，则返回 -EAGAIN。

2. 否则，先通过 address_space 将缓存的数据刷下去。

3. 再调用 `mapping->a_ops->direct_io()` 读取数据。

如果走 address_space，用户需要的数据量可能很大，需要一页一页地处理。对于每一页：

1. 从 address_space 中查找对应 page。如果找不到，则以同步方式进行预读，如果这样也拿不到 page，跳转到步骤 6。

2. 如果拿到的 page 带有 readahead 标记，说明我们需要自己预读一些页面。

3. 如果 page 带有 uptodate 标志，则跳到下一步，否则：
   - 等待 page 的 lock 标志被清零（等待 page 被解锁）。
   - 如果 page 带有 uptodate 标志，则跳转到步骤 4。
   - 现在，文件可能被 truncate 了，需要进行检查。如果有 `mapping->a_ops->is_partially_uptodate()` 回调，且通过该回调发现我们需要读的范围内数据是 uptodate 的，则跳转到步骤 4，否则跳转到步骤 5。

4. 现在，数据是确保在内存中的，且是 uptodate 的。将 page 里面的数据拷贝到用户的 buffer 里面，然后进行下个 page 的处理或者退出循环。
5. 到这一步，说明有 page，但数据没有 uptodate。
   - 如果 `page->mapping` 为空，则说明这整个页都被 truncate 了，即可以考虑下一块页面的处理（进入 continue）。
   - 接下来需要调用 `mapping->a_ops->readpage()` 读取数据。
   - 回到步骤 4，进行数据拷贝。

6. 到这一步，说明没有对应的 page，需要先分配一个 page，加入到 address_space 和 lru 结构中，然后回到步骤 5。

## 通用 write 流程

write 操作会更新 inode 的 mtime 和 ctime，以及 version。同理分为 direct_io 和 address_space 两种。

如果走 direct_io：

1. 如果 address_space 中缓存有要写入范围的数据，且当前进程不能阻塞，则立即返回错误。

2. 否则，先通过 address_space 将缓存的数据刷下去。

3. 现在处理缓存数据的其他问题。对于处于 write 范围内的每一个被缓存的 page 而言：
   - 首先，确保 page 的数据被刷到了磁盘上（上一步已经确保了这一步）。
   - 如果这个 page 做了 mmap，取消这一页的 mmap。
   - 接下来将这个 page 从 address_page 中取下，分为两步：
     - 如果这个页是 dirty 的话，先调用 `mapping->a_ops->launder_page()` 将脏数据刷下去。这一回调与 writepage() 回调的不同在于，它不允许文件系统通过 redirty 的方式跳过对这一页的 flush 操作。
     - 接下来将 page 从 address_page 中取下，然后调用 `mapping->a_ops->freepage()` 释放掉这一 page。

4. 接下来，调用 `mapping->a_ops->direct_io()` 写数据。

5. 然后，继续调用步骤 3 来刷一次 page。这是因为可能有其他进程预读了这一部分的数据，或者因为 mmap 了，然后在访问时出现 page fault 导致这一部分的数据被拉进来了。

6. 如果 direct_io 调用失败了，则通过写 Cache、刷 Cache、再无效化 Cache 的方式写数据。
   - 写 Cache。对于写入范围内的每一页：
     - 调用 `mapping->a_ops->write_begin()`，通知文件系统准备往 page 上数据了。
     - kmap page 后，将数据从用户空间拷贝到 page 上，然后 kunmap page，刷 tlb。
     - 调用 `mapping->a_ops->write_end()`，通知文件系统往 page 上写数据的过程结束。
     - 判断脏数据是否超过某一阈值，以决定是否需要后台刷数据下去。
   - 刷 Cache 和无效化 Cache 的过程与步骤 2、3 类似。

7. 至此，direct_io 的过程结束。

如果是普通的写 Cache，而不是 direct_io，则与上述步骤 6 的写 Cache 步骤相同。

如果写入数据成功，且用户指定了需要 fsync，则通过 `file->f_op->fsync()` 回调将更新的数据刷下去。

## address_space 刷数据流程

1. 先调用 `mapping->a_ops->writepages` 刷数据。

2. 如果 writepages 回调不存在，只能使用 write_page 回调。
   - blk_start_plug()
   - 对于范围内的每个 page：
     - 如果要等待所有 page 完成（`wbc->sync_mode == WB_SYNC_ALL`）或者标记了 `wbc->tagged_writepages`，则将 address_space 中标记为 PAGECACHE_TAG_DIRTY 的页面再标记为 PAGECACHE_TAG_TOWRITE。原来的 DIRTY 标记不去掉。
     - 如果上一步设置了 TOWRITE 标记，则再次在 address_space 中搜索标记为 TOWRITE 的那些 page，否则搜索在 address_space 中标记为 DIRTY 的回调（仅在给定的范围内搜索）。对于搜索到的 page：
       - 如果 page 的（不是 address_space 的）DIRTY 标记被清掉了，说明其他进程先刷下去了，我们不需要对这个 page 做任何操作。
       - 如果 page 有 WRITEBACK 标记，如果 `wbc->sync_mode != WB_SYNC_NONE`，我们需要等待这个 page 的 writeback 操作完成。
       - 否则，由当前进程负责调用 `mapping->a_op->writepage()` 回调刷数据。
   - blk_finish_plug()

3. 如果回调函数返回了 -NOMEM，表示对应块设备繁忙。此时如果 `wbc->sync_mode == WB_SYNC_ALL`，代表可以在这里等待设备刷数据，因此调用 io_schedule() 稍微等待一段时间后回到第一步重试。

4. 搜索 address_space，对于范围内的每个标记为 PAGECACHE_TAG_WRITEBACK 的 page，等待 page 的 writeback 标记被清空。

> 我们并没有设置 address_space 的 PAGECACHE_TAG_WRITEBACK 标志，但是在等待数据被刷下去时，却是搜索的该标志。其实 PAGECACHE_TAG_WRITEBACK 标志是被 `a_op->readpage()` 或 `a_op->readpages()` 回调函数设置的。

## 等待 page 标志流程

等待 page 标志被清零，这是常见的操作。例如等待 page 的 writeback 标志被清零，表示 page 被写下去了。

page 的等待机制是用哈希表完成的，名字是 page_wait_table，共 256 根链表，以 page 的地址作为键（Key）。

1. 每次需要等待 page 的某个标志位被清零时，在栈上创建一个 wait_page_queue，作为 wait_queue 的一个 entry。

2. 让该 entry 将入到对应的 wait_page_queue 的尾部，然后调用 io_schedule() 进行等待。

3. 当有其他进程从哈希表的链表上唤醒某个 page 时，会判断当前 entry 等待的 page 是否与将要唤醒的 page 相同，等待标志是否相同。若相同，会先调用 wait_page_queue 的回调函数将该 entry 从链表上取下，然后再唤醒进程。

4. 当从阻塞中被唤醒后，判断标志位是否被清零。如果是，则从等待中返回。如果是被信号打断的，也需要返回，否则回到步骤 2 重新等待。

## plug 机制

### plug

**plug 机制用于缓存刷向通用块层的数据。**使用方法如下：

1. 调用 blk_start_plug() 初始化一个 plug。

2. 处理各种往通用块层读写数据的请求。

3. 调用 blk_finish_plug() 刷数据。

plug 仅有三根链表：

1. list：用于普通 request 的链表，上面串着单队列的 request。

2. mq_list：用于 multi-queue 的链表，上面串着多队列的 request。

3. cb_list：在 unplug 时需调用的回调函数链表。

```c
struct blk_plug {
	struct list_head mq_list; /* blk-mq requests */
	struct list_head cb_list; /* md requires an unplug callback */
	unsigned short rq_count;
	bool multiple_queues;
	bool nowait;
};

struct blk_plug_cb;
typedef void (*blk_plug_cb_fn)(struct blk_plug_cb *, bool);
struct blk_plug_cb {
	struct list_head list;
	blk_plug_cb_fn callback;
	void *data;
};
```

### blk_start_plug

plug 总是与进程绑在一起的，一个进程只会有一个 plug，但 plug 机制可以递归进入。

1. 判断 `current->plug` 是否存在，如果存在，直接返回。

2. 初始化 plug 的三根链表。

3. 将外界传递进来的 plug 作为 `task->plug`。

### blk_finish_plug

1. 如果外界传递进来的 plug 不是 `current->plug`，则处于递归调用 plug 中，直接返回。

2. 调用 `plug->cb_list` 中的所有回调函数。

3. 将 mq_list 的 request 刷下去。
   - 对 mq_list 进行排序。这样属于同一个 blk_mq_ctx 的那些 request 就被放在一起了。
   - 将属于同一个 blk_mq_ctx 的 request 搜集到一根链表上，统一提交到同一个 blk_mq_hw_ctx 中。

4. 将 list 的 request 刷下去。
   - 对 list 进行排序。这样属于同一个 request_queue 的那些 request 就被放在一起了。
   - 将属于同一个 request_queue 的 request 搜集到一根链表上，统一提交到对应的 request_queue 中。

5. 设置 `current->plug` 为 NULL。

## inode、super_block 和 dentry 的并发查找机制

以 inode 为例，经常会遇到 malloc() 一个 inode 的情况。一般来说，内核中对于一个文件只对应一个 inode，但如果两个进程同时想针对硬盘上的同一个文件创建 inode，就会造成冲突。

内核为解决这个问题，将所有的 inode 放在了 inode_hashtable，被自旋锁保护。当需要一个文件对应的 inode 时：

1. 加锁，从该哈希表查找对应的 inode，解锁。

2. 若没有，分配一个 inode。

3. 加锁。

4. 从该哈希表中再次查找对应的 inode。

5. 若不存在对应的 inode，将新的 inode 加入到哈希表中。

6. 解锁。

7. 若存在对应的 inode，将刚才分配的 inode 释放掉。

dentry 和 super_block 也有类似机制，设计内核的一系列函数 iget_locked()、sget()、d_alloc_parallel() 等。

dentry 比较特殊。它将要查找的 dentry 放到一个名为 in_lookup_hashtable 的哈希表中，而将所有的 dentry 放入到 dentry_hashtable 中。

# 参考文章

1. [Linux Kernel Teaching — The Linux Kernel documentation](https://linux-kernel-labs.github.io/refs/heads/master/)

2. [Linux 内核教学 — Linux 系统内核文档](https://linux-kernel-labs-zh.xyz/)

