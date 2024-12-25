---
title: Linux Virtual Filesystem
categories:
  - Linux 学习
  - 内核层
abbrlink: f548d964
date: 2024-12-24 16:05:00
updated: 2024-12-25 17:30:00
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

# 常见的文件系统模型

任何实现的文件系统都需要包含这几种明确定义的类型：superblock、inode、file 和 dentry。这些也是文件系统的元数据。

模型实体间通过某些 VFS 子系统或内核子系统进行交互：dentry cache（目录项缓存）、inode cache（索引节点缓存）和 buffer cache（缓冲区缓存）。每个实体都被视为对象，具有关联的数据结构和指向方法表的指针。通过替换关联的方法来为每个组件引入特定的行为（类似于 C++ 的多态）。

## superblock

**superblock 超级块存储挂载文件系统需要的信息。**具体如下：

1. inode 和块的位置。
2. 文件系统块大小。
3. 最大文件名长度。
4. 最大文件大小。
5. 根 inode 的位置。

磁盘上的 superblock 通常存储在磁盘的第一个块中，即文件系统控制块。

在 VFS 中，superblock 实体都保留在类型为 `struct super_block` 的结构列表中，方法则保留在类型为 `struct super_operations` 的结构中。

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

dentry 将 inode 和 文件名关联起来，存储以下信息：

1. 用于标识 inode 的整数。
2. 表示文件名的字符串。

dentry 是目录或文件路径的特定部分。例如，对于路径 `/bin/vi`，为 `/`、`bin` 和 `vi` 创建共 3 个 dentry 对象。

dentry 在磁盘上有对应物，但对应关系不是直接的。每个文件系统都有特定的方式维护 dentry。

在 VFS 中，dentry 实体由 `struct dentry` 结构表示，与之相关的操作在 `struct dentry_operations` 结构中定义。

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

# VFS 中的超级块

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

## 超级块操作

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

## inode 结构

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

一些可用于处理 inode 的函数如下：

1. new_inode()：创建新的 inode，将 i_nlink 字段设置为 1，并初始化 i_blkbits, i_sb 和 i_dev。
2. insert_inode_hash()：将 inode 添加到 inode 哈希表中。这个调用的一个有趣的效果是，如果 inode 被标记为脏，它将被写入磁盘。
3. mark_inode_dirty()：将 inode 标记为脏，稍后它将被写入磁盘。
4. iget_locked()：从磁盘加载具有给定编号的 inode，如果它尚未加载。
5. unlock_new_inode()：与 iget_locked() 一起使用，释放对 inode 的锁定。
6. iput()：告诉内核对 inode 的操作已经完成。若没有其他进程在使用，它将被销毁（如果被标记为脏，则写入磁盘后再销毁）。
7. make_bad_inode()：告诉内核该 inode 无法使用；通常在从磁盘读取 inode 时发现无法读取的情况下使用，表示该 inode 无效。

## inode 操作

TODO

# 参考文章

1. [Linux Kernel Teaching — The Linux Kernel documentation](https://linux-kernel-labs.github.io/refs/heads/master/)
2. [Linux 内核教学 — Linux 系统内核文档](https://linux-kernel-labs-zh.xyz/)

