---
title: Rtems Source Code
categories:
  - Linux 学习
  - 内核层
abbrlink: 4936fe45
date: 2025-05-19 12:50:00
updated: 2025-05-21 23:00:00
---

<meta name="referrer" content="no-referrer"/>

# Rtems 源码阅读

RTEMS（Real‑Time Executive for Multiprocessor Systems）是一款始于 1988 年、1993 年正式发布的开源实时操作系统，专为多处理器嵌入式环境设计，支持 POSIX 和 BSD 套接字等开放标准 API，并可运行于 ARM、PowerPC、SPARC、MIPS、RISC‑V 等 18 种处理器架构及近 200 个 BSP（Board Support Package）上。它以库形式发布，应用程序与内核静态链接为单一映像，采用单地址空间、无用户/内核隔离设计，从而简化资源管理并确保确定性响应。2025 年 1 月 22 日发布的 6.1 版本全面将构建系统由 GNU Autotools 切换到基于 Python 的 Waf，大幅提升了构建速度并优化了依赖管理，同时引入了改进的调度算法和增强的 SMP 支持。

本文章用于记录阅读 Rtems 内核源码的笔记，尝试理解其中的逻辑。Rtems 内核的版本是 6.1，在线代码网站见 [https://rtems.davidingplus.cn/lxr/source/](https://rtems.davidingplus.cn/lxr/source/)。

<!-- more -->

# 文件系统流程

## open 函数

open() 函数的源码如下：

```c
int open(const char *path, int oflag, ...)
{
    int rv = 0;
    va_list ap;
    mode_t mode = 0;
    rtems_libio_t *iop = NULL;

    // 处理可变参数，获取文件创建模式（mode）。
    va_start(ap, oflag);
    mode = va_arg(ap, mode_t);

    // 分配一个文件描述符结构。
    iop = rtems_libio_allocate();
    if (iop != NULL)
    {
        // 调用底层实现打开文件。
        rv = do_open(iop, path, oflag, mode);
    }
    else
    {
        // 文件描述符耗尽，设置错误码。
        errno = ENFILE;
        rv = -1;
    }

    va_end(ap);

    return rv;
}
```

### struct rtems_libio_t

rtems_libio_t 结构体定义如下。该结构体用于表示一个文件描述符的内部状态，Rtems 中每打开一个文件都会关联一个该结构体的实例，通常简称为 iop（I/O pointer）。

```c
typedef struct rtems_libio_tt rtems_libio_t;

struct rtems_libio_tt
{
    // 文件状态标志，使用原子类型以支持线程安全操作。
    // 可能标志：是否打开、读/写权限、文件类型等。
    Atomic_Uint flags;

    // 当前文件偏移量，用于读写操作时定位文件指针位置。
    off_t offset;

    // 文件路径定位信息，类似于 inode。
    // 包含挂载点、节点、驱动等信息，用于实际文件访问。
    rtems_filesystem_location_info_t pathinfo;

    // 驱动或文件系统使用的私有字段。
    // 通常用于存储轻量级状态、句柄或标志值。
    uint32_t data0;

    // 驱动或文件系统使用的扩展字段。
    // 可指向任意类型数据，支持更复杂的上下文管理。
    void *data1;
};
```

#### struct rtems_filesystem_location_info_t

rtems_filesystem_location_info_t 结构体定义如下。它表示一个路径位置，用于描述文件系统中某个具体节点（如文件或目录）的位置及其访问方式。

```c
// 表示文件系统中一个节点（如文件或目录）的位置及其访问信息。
typedef struct rtems_filesystem_location_info_tt
{
    // 用于将该节点插入到挂载点的链表中（如目录项列表）。
    rtems_chain_node mt_entry_node;

    // 指向具体节点的访问结构，一般是与具体文件系统实现相关的 inode 或数据结构。
    void *node_access;

    // 可选的第二个访问字段，供文件系统使用，如软链接或扩展元数据。
    void *node_access_2;

    // 指向该节点所使用的文件操作处理器集合（如 open、read、write、close 等函数指针）。
    const rtems_filesystem_file_handlers_r *handlers;

    // 当前节点所属的挂载表条目，表示该节点来自哪个挂载的文件系统。
    rtems_filesystem_mount_table_entry_t *mt_entry;

} rtems_filesystem_location_info_t;
```

比较重要的成员是 `const rtems_filesystem_file_handlers_r *handlers`，该结构类似于 Linux 内核中的 file_operations，定义如下：

```c
struct _rtems_filesystem_file_handlers_r
{
    rtems_filesystem_open_t open_h;
    rtems_filesystem_close_t close_h;
    rtems_filesystem_read_t read_h;
    rtems_filesystem_write_t write_h;
    rtems_filesystem_ioctl_t ioctl_h;
    rtems_filesystem_lseek_t lseek_h;
    rtems_filesystem_fstat_t fstat_h;
    rtems_filesystem_ftruncate_t ftruncate_h;
    rtems_filesystem_fsync_t fsync_h;
    rtems_filesystem_fdatasync_t fdatasync_h;
    rtems_filesystem_fcntl_t fcntl_h;
    rtems_filesystem_poll_t poll_h;
    rtems_filesystem_kqfilter_t kqfilter_h;
    rtems_filesystem_readv_t readv_h;
    rtems_filesystem_writev_t writev_h;
    rtems_filesystem_mmap_t mmap_h;
};
```

另一个成员是 struct rtems_filesystem_mount_table_entry_tt。这个结构体代表一个完整的挂载项（mount entry），Rtems 会为每个挂载的文件系统维护一份这样的结构体实例，挂载后保存在全局挂载表中。它贯穿于路径解析、节点定位、读写操作、卸载管理等核心逻辑中。简而言之，它是挂载文件系统的核心描述对象。

```c
// 表示一个挂载的文件系统实例，是 Rtems 文件系统挂载表中的一项。
struct rtems_filesystem_mount_table_entry_tt
{
    // 用于将该挂载点插入全局挂载链表。
    rtems_chain_node mt_node;

    // 文件系统私有信息，由具体文件系统实现定义，如 ext2 的 superblock 信息。
    void *fs_info;

    // 指向文件系统操作函数表，定义如 mount、unmount、eval_path 等。
    const rtems_filesystem_operations_table *ops;

    // 文件系统的常量信息，不可变，例如初始挂载参数。
    const void *immutable_fs_info;

    // 该文件系统中所有节点的全局链表，便于遍历。
    rtems_chain_control location_chain;

    // 表示该文件系统挂载在哪个目录（挂载点）上。
    rtems_filesystem_global_location_t *mt_point_node;

    // 表示该文件系统的根节点位置。
    rtems_filesystem_global_location_t *mt_fs_root;

    // 是否已挂载成功。
    bool mounted;

    // 是否支持写操作。
    bool writeable;

    // 是否禁止创建设备节点和普通文件（mknod）。
    bool no_regular_file_mknod;

    // 该文件系统的路径名限制和选项。
    const rtems_filesystem_limits_and_options_t *pathconf_limits_and_options;

    // 挂载点路径字符串，例如 "/mnt/usb"。
    const char *target;

    // 文件系统类型名称，例如 "imfs"、"devfs"、"nfs" 等。
    const char *type;

    // 设备名称，如 "/dev/sda1"，以字符串形式表示，供底层文件系统使用。
    char *dev;

    // 发起卸载操作的任务 ID，卸载完成后通过事件通知该任务。
    rtems_id unmount_task;
};
```

### rtems_libio_allocate()

open 函数中分配文件描述符结构使用的函数是 rtems_libio_allocate()，定义如下：

```c
rtems_libio_t *rtems_libio_allocate(void)
{
    rtems_libio_t *iop;

    // 加锁，保护全局空闲链表。
    rtems_libio_lock();

    // 从空闲链表头获取一个可用的文件描述符结构。
    iop = rtems_libio_iop_free_head;

    if (iop != NULL)
    {
        void *next;

        // 获取下一个空闲节点。
        next = iop->data1;

        // 更新空闲链表头指针。
        rtems_libio_iop_free_head = next;

        // 如果空闲链表已空，更新尾指针。
        if (next == NULL)
        {
            rtems_libio_iop_free_tail = &rtems_libio_iop_free_head;
        }
    }

    // 解锁，释放对空闲链表的访问。
    rtems_libio_unlock();

    // 返回分配到的文件描述符结构（可能为 NULL）。
    return iop;
}
```

#### rtems_libio_iop_free_head

rtems_libio_iop_free_head 是一个全局变量，用于维护 Rtems 文件描述符（rtems_libio_t）的空闲链表头指针。

在初始化阶段，Rtems 会预分配一定数量的 rtems_libio_t 结构，并通过 data1 字段将它们串成一个单向链表。rtems_libio_iop_free_head 指向第一个可用节点。每次 rtems_libio_allocate() 被调用时，从头部取出一个节点，并更新链表。如果分配后链表为空，rtems_libio_iop_free_tail 会被指向 &rtems_libio_iop_free_head，表示空了。释放节点时会调用一个对应的 rtems_libio_free(iop)，将节点重新挂回链表尾部。

初始化阶段的函数逻辑如下：

```c
static void rtems_libio_init(void)
{
    uint32_t i;
    rtems_libio_t *iop;

    // 如果 I/O 对象数量大于 0，才进行初始化。
    if (rtems_libio_number_iops > 0)
    {
        // 把空闲链表的头指针指向数组中第一个 I/O 对象。
        iop = rtems_libio_iop_free_head = &rtems_libio_iops[0];

        // 把当前 I/O 对象的 data1 成员指向数组中的下一个 I/O 对象，实现链表链接。
        for (i = 0; (i + 1) < rtems_libio_number_iops; i++, iop++)
            iop->data1 = iop + 1;

        // 最后一个 I/O 对象的 data1 设置为 NULL，表示链表末尾。
        iop->data1 = NULL;

        // 记录链表尾部指针，指向最后一个 I/O 对象的 data1 成员地址。
        rtems_libio_iop_free_tail = &iop->data1;
    }
}
```

rtems_libio_iops 是 Rtems 预先分配的 I/O 控制块数组，配置了 CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 以后，会预先创建出这个数组。

问题来了：有了这个数组，为什么还要一个额外的 free list 链表来管理呢？

```c
#if CONFIGURE_MAXIMUM_FILE_DESCRIPTORS > 0
	rtems_libio_t rtems_libio_iops[ CONFIGURE_MAXIMUM_FILE_DESCRIPTORS ];
```

### do_open()

open() 函数中分配好文件描述符结构以后，最终会到达 do_open() 函数的位置进行处理。

函数开始时，从 iop 获取文件描述符 fd，并根据 oflag 解析读写权限、创建、独占、截断和目录打开等标志。然后确定是否跟随符号链接，组合路径解析所需的权限标志 eval_flags。

接着初始化路径解析上下文，解析路径并获取当前文件系统位置。若支持创建普通文件且路径未结束，则调用创建文件的函数。随后判断是否以目录方式打开，并检查写权限和目录类型的合法性，防止写目录或以目录方式打开非目录。

路径信息保存到 iop->pathinfo，清理路径解析上下文后，设置文件控制标志，调用底层驱动的 open 函数打开文件。若成功且指定截断，则调用 ftruncate 截断文件内容，截断失败时关闭文件。

最后，若操作全部成功，设置文件打开标志并返回文件描述符；失败时释放资源并返回错误。整个过程确保了路径解析、权限检查和文件打开的正确性和安全性。

```c
// 执行打开文件操作的核心函数。
static int do_open(
    rtems_libio_t *iop, // I/O 控制块，表示打开的文件或设备。
    const char *path,   // 要打开的文件路径。
    int oflag,          // 打开标志（如只读、只写、创建等）。
    mode_t mode         // 创建文件时的权限模式。
)
{
    // 返回值，初始化为 0，后续存储函数调用结果。
    int rv = 0;

    // 将 iop 转换成文件描述符。
    // #define rtems_libio_iop_to_descriptor(_iop) ((_iop) - &rtems_libio_iops[0])
    // 直接数组地址相减就拿到了文件描述符，666。
    int fd = rtems_libio_iop_to_descriptor(iop);

    // 计算读写标志的辅助变量。
    int rwflag = oflag + 1;

    // 是否有读权限。
    bool read_access = (rwflag & _FREAD) == _FREAD;

    // 是否有写权限。
    bool write_access = (rwflag & _FWRITE) == _FWRITE;

    // 是否需要创建新文件。
    bool make = (oflag & O_CREAT) == O_CREAT;

    // 是否独占创建。
    bool exclusive = (oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL);

    // 是否需要截断文件。
    bool truncate = (oflag & O_TRUNC) == O_TRUNC;

    // 是否以目录方式打开文件。
    bool open_dir;

#ifdef O_NOFOLLOW
    // 是否允许跟随符号链接，O_NOFOLLOW 表示不跟随。
    int follow = (oflag & O_NOFOLLOW) == O_NOFOLLOW ? 0 : RTEMS_FS_FOLLOW_LINK;
#else
    // 默认允许跟随符号链接。
    int follow = RTEMS_FS_FOLLOW_LINK;
#endif

    // 组合权限和标志，用于路径解析。
    int eval_flags = follow | (read_access ? RTEMS_FS_PERMS_READ : 0) | (write_access ? RTEMS_FS_PERMS_WRITE : 0) | (make ? RTEMS_FS_MAKE : 0) | (exclusive ? RTEMS_FS_EXCLUSIVE : 0);

    // 路径解析上下文。
    rtems_filesystem_eval_path_context_t ctx;

    // 当前路径位置结构体指针。
    const rtems_filesystem_location_info_t *currentloc;

    // 是否创建普通文件。
    bool create_reg_file;

    // 启动路径解析，准备解析文件路径。
    rtems_filesystem_eval_path_start(&ctx, path, eval_flags);

    // 获取解析后的当前路径位置信息。
    currentloc = rtems_filesystem_eval_path_get_currentloc(&ctx);

    // 判断当前路径所在文件系统是否允许创建普通文件。
    create_reg_file = !currentloc->mt_entry->no_regular_file_mknod;

    // 如果允许创建普通文件且路径中还有后续路径分段，则创建普通文件。
    if (create_reg_file && rtems_filesystem_eval_path_has_token(&ctx))
    {
        create_regular_file(&ctx, mode);
    }

#ifdef O_DIRECTORY
    // 判断是否以目录方式打开。
    open_dir = (oflag & O_DIRECTORY) == O_DIRECTORY;
#else
    open_dir = false;
#endif

    // 如果有写权限或以目录方式打开，需要额外检查路径类型和权限。
    if (write_access || open_dir)
    {
        // 获取当前路径类型。
        mode_t type = rtems_filesystem_location_type(currentloc);

        // 如果创建普通文件时路径是目录，禁止写操作，返回 EISDIR 错误。
        if (create_reg_file && write_access && S_ISDIR(type))
        {
            rtems_filesystem_eval_path_error(&ctx, EISDIR);
        }

        // 如果以目录方式打开，但路径不是目录，返回 ENOTDIR 错误。
        if (open_dir && !S_ISDIR(type))
        {
            rtems_filesystem_eval_path_error(&ctx, ENOTDIR);
        }
    }

    // 将路径解析得到的当前路径信息保存到 iop 的 pathinfo 中。
    rtems_filesystem_eval_path_extract_currentloc(&ctx, &iop->pathinfo);

    // 清理路径解析上下文，释放资源。
    rtems_filesystem_eval_path_cleanup(&ctx);

    // 设置 iop 的文件控制标志，转换 POSIX oflag 为 LibIO 内部标志。
    rtems_libio_iop_flags_set(iop, rtems_libio_fcntl_flags(oflag));

    // 调用底层文件系统的 open 函数打开文件。
    rv = (*iop->pathinfo.handlers->open_h)(iop, path, oflag, mode);

    // 如果打开成功。
    if (rv == 0)
    {
        /*
         * 延迟设置 LIBIO_FLAGS_OPEN 标志，直到文件截断完成。
         * 这样可避免在截断过程中被其他线程使用或关闭文件描述符。
         */
        if (truncate)
        {
            // 如果有写权限，调用底层 ftruncate 截断文件。
            if (write_access)
            {
                rv = (*iop->pathinfo.handlers->ftruncate_h)(iop, 0);
            }
            else
            {
                // 如果无写权限，截断操作非法，返回错误。
                rv = -1;
                errno = EINVAL;
            }

            // 如果截断失败，则关闭文件。
            if (rv != 0)
            {
                (*iop->pathinfo.handlers->close_h)(iop);
            }
        }

        // 如果截断成功或不需要截断。
        if (rv == 0)
        {
            // 设置文件已打开的标志。
            rtems_libio_iop_flags_set(iop, LIBIO_FLAGS_OPEN);

            // 返回文件描述符。
            rv = fd;
        }
        else
        {
            // 失败返回 -1。
            rv = -1;
        }
    }

    // 如果打开失败，释放 iop 资源。
    if (rv < 0)
    {
        rtems_libio_free(iop);
    }

    // 返回文件描述符或错误码。
    return rv;
}
```

#### 路径解析过程

do_open() 涉及到的路径解析代码片段如下：

```c
// 启动路径解析，准备解析文件路径。
rtems_filesystem_eval_path_start(&ctx, path, eval_flags);

// 获取解析后的当前路径位置信息。
currentloc = rtems_filesystem_eval_path_get_currentloc(&ctx);
```

rtems_filesystem_eval_path_start() 代码如下：

```c
// 初始化路径解析上下文，并从根目录或当前目录开始解析路径。
rtems_filesystem_location_info_t *
rtems_filesystem_eval_path_start(
    rtems_filesystem_eval_path_context_t *ctx,
    const char *path,
    int eval_flags)
{
    // 实际调用带 root 和 current 参数的版本，使用全局根目录和当前目录。
    return rtems_filesystem_eval_path_start_with_root_and_current(
        ctx,
        path,
        strlen(path), // 计算路径长度。
        eval_flags,
        &rtems_filesystem_root,    // 指向全局根目录。
        &rtems_filesystem_current  // 指向全局当前目录。
    );
}

// 初始化路径解析上下文，并以给定的 root 和 current 为起点解析路径。
rtems_filesystem_location_info_t *
rtems_filesystem_eval_path_start_with_root_and_current(
    rtems_filesystem_eval_path_context_t *ctx,
    const char *path,
    size_t pathlen,
    int eval_flags,
    rtems_filesystem_global_location_t *const *global_root_ptr,
    rtems_filesystem_global_location_t *const *global_current_ptr)
{
    // 将整个上下文结构清零，确保起始状态干净。
    memset(ctx, 0, sizeof(*ctx));

    // 设置路径字符串及其长度。
    ctx->path = path;
    ctx->pathlen = pathlen;

    // 设置路径解析标志（如是否跟随符号链接、是否创建等）。
    ctx->flags = eval_flags;

    // 根据传入的根目录和当前目录，初始化起始路径位置。
    set_startloc(ctx, global_root_ptr, global_current_ptr);

    // 为路径解析的起点加锁，避免多线程同时访问。
    rtems_filesystem_instance_lock(&ctx->startloc->location);

    // 拷贝起始位置为当前解析位置，作为路径遍历起点。
    rtems_filesystem_location_clone(
        &ctx->currentloc,
        &ctx->startloc->location
    );

    // 开始路径的逐层解析过程。
    rtems_filesystem_eval_path_continue(ctx);

    // 返回当前解析到的位置（可表示最终文件、目录或中间节点）。
    return &ctx->currentloc;
}
```

可以看出最后进入了 rtems_filesystem_eval_path_continue() 函数，嵌套太深了。目前看不懂整个路径的解析过程。

路径解析完毕后，do_open() 函数中执行以下函数用于维护状态：

```c
// 获取解析后的当前路径位置信息。
currentloc = rtems_filesystem_eval_path_get_currentloc(&ctx);

// 判断当前路径所在文件系统是否允许创建普通文件。
create_reg_file = !currentloc->mt_entry->no_regular_file_mknod;

......


// 将路径解析得到的当前路径信息保存到 iop 的 pathinfo 中。
rtems_filesystem_eval_path_extract_currentloc(&ctx, &iop->pathinfo);

// 清理路径解析上下文，释放资源。
rtems_filesystem_eval_path_cleanup(&ctx);
```

#### 底层文件系统的 open 函数

拿到所有信息以后，do_open() 函数中调用底层文件系统的 open() 函数真正打开文件：

```c
// 调用底层文件系统的 open 函数打开文件。
rv = (*iop->pathinfo.handlers->open_h)(iop, path, oflag, mode);
```

## close 函数

close() 函数的源码如下。在前面更改完状态标志位后，还是会进入到底层文件系统的 close_h 函数。

```c
int close(int fd)
{
    rtems_libio_t *iop; // 指向文件描述符对应的 I/O 对象。
    unsigned int flags; // 当前 I/O 对象的状态标志位。
    int rc;             // 用于保存最终返回值。

    // 检查文件描述符是否越界。
    if ((uint32_t)fd >= rtems_libio_number_iops)
    {
        rtems_set_errno_and_return_minus_one(EBADF); // 错误：Bad file descriptor。
    }

    // 根据 fd 获取对应的 I/O 对象。
    /**
     * 这个实现如我所料。
     * static inline rtems_libio_t *rtems_libio_iop(int fd)
     * {
     *     return &rtems_libio_iops[fd];
     * }
     */
    iop = rtems_libio_iop(fd);

    // 读取该对象当前的标志。
    flags = rtems_libio_iop_flags(iop);

    // 这段循环代码的作用是：在线程安全的前提下，把 I/O 对象的 “打开” 标志（LIBIO_FLAGS_OPEN）给清除掉，并且检测有没有正在并发操作导致的冲突。
    while (true)
    {
        unsigned int desired; // 期望写入的新标志。
        bool success;         // CAS 成功与否。

        // 如果文件未被标记为已打开，返回 EBADF。
        if ((flags & LIBIO_FLAGS_OPEN) == 0)
        {
            // #define EBADF 9 /* Bad file number */
            rtems_set_errno_and_return_minus_one(EBADF);
        }

        // 清除引用计数部分，仅保留控制标志。
        flags &= LIBIO_FLAGS_REFERENCE_INC - 1U;

        // 构造期望的新状态：去掉 OPEN 标志（标记为关闭）。
        desired = flags & ~LIBIO_FLAGS_OPEN;

        // 使用原子操作尝试替换标志，确保线程安全。
        success = _Atomic_Compare_exchange_uint(
            &iop->flags,          // 要更新的目标变量。
            &flags,               // 当前预期值，会被更新为实际值（若失败）。
            desired,              // 想要写入的新值。
            ATOMIC_ORDER_ACQ_REL, // 成功时的内存顺序。
            ATOMIC_ORDER_RELAXED  // 失败时的内存顺序。
        );

        if (success)
        {
            // 成功清除 OPEN 标志，跳出循环。
            break;
        }

        // 如果标志中有非法或冲突的状态，返回 EBUSY。
        if ((flags & ~(LIBIO_FLAGS_REFERENCE_INC - 1U)) != 0)
        {
            rtems_set_errno_and_return_minus_one(EBUSY);
        }
    }

    // 调用具体文件系统提供的 close 方法。
    rc = (*iop->pathinfo.handlers->close_h)(iop); // 关闭文件，通常会执行文件系统特定的清理工作。

    rtems_libio_free(iop); // 释放 I/O 对象资源，回收到 I/O 对象池中以供复用。

    return rc; // 返回关闭操作的结果，通常为 0 表示成功，-1 表示失败（并设置 errno）。
}
```

## read 函数

read() 函数的源码如下。可以看出除了做了一些检查以外，直接调用了底层文件系统的 read_h() 函数。

```c
ssize_t read(
    int fd,       // 文件描述符，标识要读取的文件或设备。
    void *buffer, // 指向用户提供的内存缓冲区。
    size_t count  // 期望读取的字节数。
)
{
    rtems_libio_t *iop; // 指向文件描述符对应的 I/O 对象结构体。
    ssize_t n;          // 实际读取的字节数或错误代码。

    // 检查 buffer 是否为 NULL，防止非法内存访问。
    rtems_libio_check_buffer(buffer);

    // 检查读取字节数是否为 0 或超出合理范围。
    rtems_libio_check_count(count);

    // 获取对应的 I/O 对象，并检查是否具有可读权限。
    // 若无效或不可读，则自动设置 errno = EBADF 并返回 -1。
    LIBIO_GET_IOP_WITH_ACCESS(fd, iop, LIBIO_FLAGS_READ, EBADF);

    /*
     * 正式执行读取操作：
     * 调用底层文件系统或设备驱动提供的 read 函数。
     * 由 handlers->read_h 函数指针调用完成具体的读取逻辑。
     */
    n = (*iop->pathinfo.handlers->read_h)(iop, buffer, count);

    // 读取完成后释放 I/O 对象（减少引用计数等）。
    rtems_libio_iop_drop(iop);

    return n; // 返回实际读取的字节数，若出错则为负值并设置 errno。
}
```

## write 函数

write() 函数的源码如下。大致逻辑同样同 read 函数。

```c
ssize_t write(
    int fd,             // 文件描述符，表示要写入的目标文件或设备。
    const void *buffer, // 用户数据缓冲区的地址。
    size_t count        // 要写入的字节数。
)
{
    rtems_libio_t *iop; // 指向文件描述符关联的 I/O 对象。
    ssize_t n;          // 实际写入的字节数或错误码。

    // 检查 buffer 是否为 NULL，防止非法内存访问。
    rtems_libio_check_buffer(buffer);

    // 检查写入的字节数是否合理（非零、未超限）。
    rtems_libio_check_count(count);

    // 获取 I/O 对象，并检查是否具有写权限。
    // 若非法或不可写，则设置 errno = EBADF，并返回 -1。
    LIBIO_GET_IOP_WITH_ACCESS(fd, iop, LIBIO_FLAGS_WRITE, EBADF);

    /*
     * 调用底层设备或文件系统提供的写入实现。
     * 实际写入的逻辑由 write_h 函数指针指定。
     */
    n = (*iop->pathinfo.handlers->write_h)(iop, buffer, count);

    // 操作完成后释放 I/O 对象（例如减少引用计数）。
    rtems_libio_iop_drop(iop);

    return n; // 返回写入的字节数，失败时为负值并设置 errno。
}
```

