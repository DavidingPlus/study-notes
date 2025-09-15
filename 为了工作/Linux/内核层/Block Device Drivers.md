---
title: Block Device Drivers
categories:
  - Linux 学习
  - 内核层
abbrlink: 69869e7f
date: 2025-01-14 10:20:00
updated: 2025-01-14 10:30:00
---

<meta name="referrer" content="no-referrer"/>

块设备是与字符设备并列的概念，这两类设备（一共三类，还有一类是网络设备）在 Linux 中的驱动结构有较大差异。总体而言，块设备驱动比字符设备驱动复杂得多，在 I/O 操作上也有极大的不同。缓冲、I/O 调度、请求队列等都是与块设备驱动相关的概念。

# 块设备的 I/O 操作特点

块设备 I/O 操作与字符设备的不同之处如下：

1. **块设备只能以块为单位接收输入和返回输出，而字符设备则以字节为单位。大多数设备是字符设备，因为它们不需要缓冲而且不以固定块大小进行操作。**
2. **块设备对于 I/O 请求有对应的缓冲区，故它们可以选择以什么顺序进行响应，而字符设备无须缓冲且被直接读写。对存储设备而言，调整读写的顺序作用巨大，因为在读写连续的扇区的存储速度比分离的扇区更快。**
3. **字符设备只能顺序读写，而块设备能随机访问。**

<!-- more -->

虽然块设备可随机访问，但对于磁盘这类机械设备而言，顺序地组织块设备的访问可以提高性能。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241211095908580.png" alt="image-20241211095908580" style="zoom:70%;" />

在 Linux 中通常通过磁盘文件系统 EXT4、UBIFS 等访问磁盘，但磁盘也有一种原始设备的访问方式，如直接访问 /dev/sdb1 等。所有的 EXT4、UBIFS、原始块设备都工作于 VFS 之下，而 EXT4、UBIFS、原始块设备之下又包含块 I/O 调度层以进行排序和合并。**I/O 调度层的基本目的是将请求按照它们对应在块设备上的扇区号进行排列，以减少磁头的移动，提高效率。**

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241211100707232.png" alt="image-20241211100707232" style="zoom:75%;" />

# 块设备驱动结构

## block_device_operations 结构体

块设备驱动有个类似字符设备驱动 file_operations 的 block_device_operations 结构体，是对块设备操作的集合。

```c
struct block_device_operations {
	blk_qc_t (*submit_bio) (struct bio *bio);
	int (*open) (struct block_device *, fmode_t);
	void (*release) (struct gendisk *, fmode_t);
	int (*rw_page)(struct block_device *, sector_t, struct page *, unsigned int);
	int (*ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
	int (*compat_ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
	unsigned int (*check_events) (struct gendisk *disk,
				      unsigned int clearing);
    /* ->media_changed() is DEPRECATED, use ->check_events() instead */
	void (*unlock_native_capacity) (struct gendisk *);
	int (*getgeo)(struct block_device *, struct hd_geometry *);
	int (*set_read_only)(struct block_device *bdev, bool ro);
	/* this callback is with swap_lock and sometimes page table lock held */
	void (*swap_slot_free_notify) (struct block_device *, unsigned long);
	int (*report_zones)(struct gendisk *, sector_t sector,
			unsigned int nr_zones, report_zones_cb cb, void *data);
	char *(*devnode)(struct gendisk *disk, umode_t *mode);
	struct module *owner;
	const struct pr_ops *pr_ops;

	/*
	 * Special callback for probing GPT entry at a given sector.
	 * Needed by Android devices, used by GPT scanner and MMC blk
	 * driver.
	 */
	int (*alternative_gpt_sector)(struct gendisk *disk, sector_t *sector);
};
```

1. 打开与释放

当设备被打开和关闭的时会调用这两个函数。

```c
int (*open) (struct block_device *, fmode_t);
void (*release) (struct gendisk *, fmode_t);
```

2. I/O 控制

同字符设备，进行系统调用 ioctl() 时会走到驱动的这两个函数，具体调用哪个看具体情况。块设备包含大量的标准请求，这些请求由 Linux 通用块设备层处理，故大部分块设备驱动的 ioctl() 函数相当短。一个 64 位系统内的 32 位进程调用 ioctl() 时，调用的是 compat_ioctl()。

```c
int (*ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
int (*compat_ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
```

3. 介质改变

老版本使用的是函数 media_changed()，Linux 4.0 以后已废除，改而使用 check_events()。

内核调用此函数检查驱动器中的介质是否已经改变。若是，返回一个非 0 值，否则返回 0。此函数仅适用于支持可移动介质的驱动器，通常需要在驱动中增加一个表示介质状态是否改变的标志变量，非可移动设备的驱动无需实现此方法。

media_changed() 在用户空间轮询可移动磁盘介质是否存在，而 check_events() 在内核空间里轮询。check_events() 检查有无挂起事件，若有 DISK_EVENT_MEDIA_CHANGE 和 DISK_EVENT_EJECT_REQUEST 事件，就返回。

```c
int (*media_changed) (struct gendisk *gd);
unsigned int (*check_events) (struct gendisk *disk, unsigned int clearing);
```

4. 使介质有效

Linux 5.15 以前存在接口 revalidate_disk()，目前已移除。暂未找到替代接口，可能牵扯到了部分设计。

```c
// 用于响应一个介质改变，它给驱动一个机会进行必要的工作以使新介质准备好。
int (*revalidate_disk) (struct gendisk *gd);
```

5. 获得驱动器信息

getgeo() 函数根据驱动器的几何信息填充一个 hd_geometry 结构体，包含磁头、扇区、柱面等信息。

```c
int (*getgeo)(struct block_device *, struct hd_geometry *);

struct hd_geometry {
      unsigned char heads;
      unsigned char sectors;
      unsigned short cylinders;
      unsigned long start;
};
```

6. 模块指针

拥有这个结构体的模块的指针，通常被初始化为 THIS_MODULE。

```c
struct module *owner;
```

## gendisk 结构体

Linux 内核使用 gendisk（通用磁盘）结构体表示一个独立的磁盘设备（例如分区）。

```c
struct gendisk {
	/* major, first_minor and minors are input parameters only,
	 * don't use directly.  Use disk_devt() and disk_max_parts().
	 */
	int major;			/* major number of driver */
	int first_minor;
	int minors;                     /* maximum number of minors, =1 for
                                         * disks that can't be partitioned. */

	char disk_name[DISK_NAME_LEN];	/* name of major driver */

	unsigned short events;		/* supported events */
	unsigned short event_flags;	/* flags related to event processing */

	struct xarray part_tbl;
	struct block_device *part0;

	const struct block_device_operations *fops;
	struct request_queue *queue;
	void *private_data;

	int flags;
	unsigned long state;
#define GD_NEED_PART_SCAN		0
#define GD_READ_ONLY			1
#define GD_DEAD				2
#define GD_NATIVE_CAPACITY		3

	struct mutex open_mutex;	/* open/close mutex */
	unsigned open_partitions;	/* number of open partitions */

	struct backing_dev_info	*bdi;
	struct kobject *slave_dir;
#ifdef CONFIG_BLOCK_HOLDER_DEPRECATED
	struct list_head slave_bdevs;
#endif
	struct timer_rand_state *random;
	atomic_t sync_io;		/* RAID */
	struct disk_events *ev;
#ifdef  CONFIG_BLK_DEV_INTEGRITY
	struct kobject integrity_kobj;
#endif	/* CONFIG_BLK_DEV_INTEGRITY */
#if IS_ENABLED(CONFIG_CDROM)
	struct cdrom_device_info *cdi;
#endif
	int node_id;
	struct badblocks *bb;
	struct lockdep_map lockdep_map;
	u64 diskseq;
};
```

major、first_minor 和 minors 共同代表磁盘的主、次设备号，同一个磁盘的各个分区共享一个主设备号，而次设备号则不同。fops 为 block_device_operations，即块设备的操作集合。queue 是内核用来管理这个设备的 I/O 请求队列的指针。private_data 指向磁盘的任何私有数据，用法类似字符设备的 private_data。

Linux 提供了一组函数操作 gendisk。

1. 分配 gendisk

gendisk 结构体是个动态分配的结构体，需要特别的内核操作来初始化，驱动不能自己分配这个结构体。

旧版使用接口 alloc_disk() 分配 gendisk。注意下面新版和旧版代码的区别。 

```c
// 旧版
// minors 参数是这个磁盘使用的次设备号的数量，一般也是磁盘分区的数量，此后不能被修改。
#define alloc_disk(minors) alloc_disk_node(minors, NUMA_NO_NODE)

#define alloc_disk_node(minors, node_id)				\
({									\
	static struct lock_class_key __key;				\
	const char *__name;						\
	struct gendisk *__disk;						\
									\
	__name = "(gendisk_completion)"#minors"("#node_id")";		\
									\
	__disk = __alloc_disk_node(minors, node_id);			\
									\
	if (__disk)							\
		lockdep_init_map(&__disk->lockdep_map, __name, &__key, 0); \
									\
	__disk;								\
})
```

在 __alloc_disk_node() 函数中将 minors 参数赋值给 disk->minors。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241211113728841.png" alt="image-20241211113728841" style="zoom:85%;" />

Linux 5.15 以后已移除 alloc_disk() 接口，转而使用宏函数 blk_alloc_disk()。

新版本让用户传入参数 node_id，根据注释知道是类似之前 NUMA_NO_NODE 的东西，一般传这个就可以了，当然也可以手动指定。关于 NUMA 的含义，请参考 [https://www.kernel.org/doc/html/latest/translations/zh_CN/mm/numa.html](https://www.kernel.org/doc/html/latest/translations/zh_CN/mm/numa.html)。至于 minors 参数，结合旧版的处理，猜测新版中在分配以后对 gendisk 结构体的属性赋值即可。

```c
// 新版
/**
 * blk_alloc_disk - allocate a gendisk structure
 * @node_id: numa node to allocate on
 *
 * Allocate and pre-initialize a gendisk structure for use with BIO based
 * drivers.
 *
 * Context: can sleep
 */
#define blk_alloc_disk(node_id)						\
({									\
	static struct lock_class_key __key;				\
									\
	__blk_alloc_disk(node_id, &__key);				\
})

// 在 __blk_alloc_disk() 中调用了 __alloc_disk_node()。
struct gendisk *__blk_alloc_disk(int node, struct lock_class_key *lkclass)
{
	struct request_queue *q;
	struct gendisk *disk;

	q = blk_alloc_queue(node);
	if (!q)
		return NULL;

	disk = __alloc_disk_node(q, node, lkclass);
	if (!disk) {
		blk_cleanup_queue(q);
		return NULL;
	}
	return disk;
}
EXPORT_SYMBOL(__blk_alloc_disk);

struct gendisk *__alloc_disk_node(struct request_queue *q, int node_id, struct lock_class_key *lkclass);
```

2. 增加 gendisk

gendisk 结构体被分配之后，系统还不能使用这个磁盘，需调用 add_disk() 函数来注册这个磁盘设备。

```c
int add_disk(struct gendisk *disk)
{
	return device_add_disk(NULL, disk, NULL);
}

int device_add_disk(struct device *parent, struct gendisk *disk, const struct attribute_group **groups);
```

> 特别注意：add_disk() 的调用必须在驱动程序的初始化工作完成并能响应磁盘的请求之后。

3. 释放 gendisk

不再需要磁盘时，使用 del_gendisk() 函数释放 gendisk。

```c
void del_gendisk(struct gendisk *gp);
```

4. gendisk 引用计数

使用 put_disk() 可操作 gendisk 的引用计数，这个工作一般无需驱动亲自做。

```c
void put_disk(struct gendisk *disk)
{
	if (disk)
		put_device(disk_to_dev(disk));
}
EXPORT_SYMBOL(put_disk);
```

旧版还有 get_disk() 函数，可以获取引用计数，返回 kobject * 类型。新版中已移除，可能可以直接通过 gendisk 结构体获得相关结果。

```c
// 旧版
struct kobject *get_disk(struct gendisk *disk);
```

## bio、request和request_queue

### bio

通常一个 bio 对应上层传给块层的 I/O 请求。每个 bio 结构体及其包含的 bvec_iter、bio_vec 结构体描述了该 I/O 请求的开始扇区、数据方向（读还是写）、数据放入的页等。

```c
struct bio {
	struct bio		*bi_next;	/* request queue link */
	struct block_device	*bi_bdev;
	unsigned int		bi_opf;		/* bottom bits REQ_OP, top bits
						 * req_flags.
						 */
	unsigned short		bi_flags;	/* BIO_* below */
	unsigned short		bi_ioprio;
	unsigned short		bi_write_hint;
	blk_status_t		bi_status;
	atomic_t		__bi_remaining;

	struct bvec_iter	bi_iter;

	bio_end_io_t		*bi_end_io;

	void			*bi_private;
#ifdef CONFIG_BLK_CGROUP
	/*
	 * Represents the association of the css and request_queue for the bio.
	 * If a bio goes direct to device, it will not have a blkg as it will
	 * not have a request_queue associated with it.  The reference is put
	 * on release of the bio.
	 */
	struct blkcg_gq		*bi_blkg;
	struct bio_issue	bi_issue;
#ifdef CONFIG_BLK_CGROUP_IOCOST
	u64			bi_iocost_cost;
#endif
#endif

#ifdef CONFIG_BLK_INLINE_ENCRYPTION
	struct bio_crypt_ctx	*bi_crypt_context;
#endif

	union {
#if defined(CONFIG_BLK_DEV_INTEGRITY)
		struct bio_integrity_payload *bi_integrity; /* data integrity */
#endif
	};

	unsigned short		bi_vcnt;	/* how many bio_vec's */

	/*
	 * Everything starting with bi_max_vecs will be preserved by bio_reset()
	 */

	unsigned short		bi_max_vecs;	/* max bvl_vecs we can hold */

	atomic_t		__bi_cnt;	/* pin count */

	struct bio_vec		*bi_io_vec;	/* the actual vec list */

	struct bio_set		*bi_pool;

	/*
	 * We can inline a number of vecs at the end of the bio, to avoid
	 * double allocations for a small number of bio_vecs. This member
	 * MUST obviously be kept at the very end of the bio.
	 */
	struct bio_vec		bi_inline_vecs[];
};

struct bvec_iter {
	sector_t		bi_sector;	/* device address in 512 byte
						   sectors */
	unsigned int		bi_size;	/* residual I/O count */

	unsigned int		bi_idx;		/* current index into bvl_vec */

	unsigned int            bi_bvec_done;	/* number of bytes completed in
						   current bvec */
};
```

**与 bio 对应的数据每次存放的内存不一定是连续的。**bio_vec 结构体用来描述与这个 bio 请求对应的所有的内存，它可能不总是在一个页面里面，故需要一个向量来记录。向量中的每个元素实际是一个 [page，offset，len]，也称为一个片段。

```c
struct bio_vec {
	struct page	*bv_page;
	unsigned int	bv_len;
	unsigned int	bv_offset;
};
```

### request 和 request_queue

**I/O 调度算法可将连续的 bio 合并成一个请求。请求是 bio 经 I/O 调度调整后的结果，这是二者的区别。**一个 request 可包含多个 bio。当 bio 被提交给 I/O 调度器时，I/O 调度器可能会将这个 bio 插入现存的请求中，也可能生成新的请求。

每个块设备或者块设备的分区都有自身的 request_queue，从 I/O 调度器合并和排序出来的请求会被分发（Dispatch）到设备级别的 request_queue。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20241211121458861.png" alt="image-20241211121458861" style="zoom:70%;" />

**随着高速 SSD 的出现并展现出越来越高的性能，传统的块设备层已无法满足这么高的 IOPS（IOs per second），逐渐成为系统 I/O 性能的瓶颈。故在 Linux 5 后废弃了原有的 blk-sq（block single queue）架构，而采用新的 blk-mq（block multi queue）架构。**API 发生了非常大的变化。关于更多 blk-mq 的细节，可参考 [https://blog.csdn.net/Wang20122013/article/details/120544642](https://blog.csdn.net/Wang20122013/article/details/120544642)。

下面是涉及处理 bio、request 和 request_queue 的 API。

1. 初始化请求队列

blk_mq_init_queue() 一般在块设备的初始化过程中使用。此函数会发生内存分配的行为，可能会失败，需检查它的返回值。

```c
struct request_queue *blk_mq_init_queue(struct blk_mq_tag_set *set)
{
	return blk_mq_init_queue_data(set, NULL);
}
EXPORT_SYMBOL(blk_mq_init_queue);

// 此函数的主要流程：
// 1. 调用 blk_alloc_queue() 分配请求队列的内存，分配的内存节点与设备连接的 NUMA 节点一致，避免远端内存访问问题。
// 2. 调用 blk_mq_init_allocated_queue() 初始化分配的请求队列。
struct request_queue *blk_mq_init_queue_data(struct blk_mq_tag_set *set, void *queuedata)
{
	struct request_queue *q;
	int ret;

	q = blk_alloc_queue(set->numa_node);
	if (!q)
		return ERR_PTR(-ENOMEM);
	q->queuedata = queuedata;
	ret = blk_mq_init_allocated_queue(set, q);
	if (ret) {
		blk_cleanup_queue(q);
		return ERR_PTR(ret);
	}
	return q;
}
```

其中 blk_mq_tag_set 结构体定义为：

```c
struct blk_mq_tag_set {
	struct blk_mq_queue_map	map[HCTX_MAX_TYPES];
	unsigned int		nr_maps;
	const struct blk_mq_ops	*ops;
	unsigned int		nr_hw_queues;
	unsigned int		queue_depth;
	unsigned int		reserved_tags;
	unsigned int		cmd_size;
	int			numa_node;
	unsigned int		timeout;
	unsigned int		flags;
	void			*driver_data;
	atomic_t		active_queues_shared_sbitmap;

	struct sbitmap_queue	__bitmap_tags;
	struct sbitmap_queue	__breserved_tags;
	struct blk_mq_tags	**tags;

	struct mutex		tag_list_lock;
	struct list_head	tag_list;
};
```

2. 清除请求队列

此函数将请求队列归还给系统，一般在块设备驱动卸载过程中调用。

```c
static inline void blk_mq_cleanup_rq(struct request *rq)
{
	if (rq->q->mq_ops->cleanup_rq)
		rq->q->mq_ops->cleanup_rq(rq);
}
```

3. 分配请求队列

此函数在初始化请求队列的 blk_mq_init_queue_data() 函数中被调用过。

```c
struct request_queue *blk_alloc_queue(int node_id);
```

4. 提取请求

TODO。暂未找到替代函数。可能是设计和语义发生了改变导致的。后续调研。

5. 启动请求

```c
// 启动并从请求队列中移除请求。
void blk_mq_start_request(struct request *rq);
```

6. 遍历 I/O 和片段

`__rq_for_each_bio()` 遍历一个请求的所有 bio。

```c
#define __rq_for_each_bio(_bio, rq)	\
	if ((rq->bio))			\
		for (_bio = (rq)->bio; _bio; _bio = _bio->bi_next)
```

bio_for_each_segment() 遍历一个 bio 的所有 bio_vec。

```c
#define __bio_for_each_segment(bvl, bio, iter, start)			\
	for (iter = (start);						\
	     (iter).bi_size &&						\
		((bvl = bio_iter_iovec((bio), (iter))), 1);		\
	     bio_advance_iter_single((bio), &(iter), (bvl).bv_len))

#define bio_for_each_segment(bvl, bio, iter)				\
	__bio_for_each_segment(bvl, bio, iter, (bio)->bi_iter)
```

rq_for_each_segment() 遍历一个请求所有 bio 中的所有 segment。

```c
#define rq_for_each_segment(bvl, _rq, _iter)			\
	__rq_for_each_bio(_iter.bio, _rq)			\
		bio_for_each_segment(bvl, _iter.bio, _iter.iter)
```

7. 报告完成

这两个函数用于报告请求是否完成，error 为 0 表示成功，小于 0 表示失败。

```c
void blk_mq_end_request(struct request *rq, blk_status_t error)
{
	if (blk_update_request(rq, error, blk_rq_bytes(rq)))
		BUG();
	__blk_mq_end_request(rq, error);
}
EXPORT_SYMBOL(blk_mq_end_request);

void __blk_mq_end_request(struct request *rq, blk_status_t error)
{
	u64 now = 0;

	if (blk_mq_need_time_stamp(rq))
		now = ktime_get_ns();

	if (rq->rq_flags & RQF_STATS) {
		blk_mq_poll_stats_start(rq->q);
		blk_stat_add(rq, now);
	}

	blk_mq_sched_completed_request(rq, now);

	blk_account_io_done(rq, now);

	if (rq->end_io) {
		rq_qos_done(rq->q, rq);
		rq->end_io(rq, error);
	} else {
		blk_mq_free_request(rq);
	}
}
EXPORT_SYMBOL(__blk_mq_end_request);
```

## I/O 调度器

Linux 2.6 以后的内核包含 4 个 I/O 调度器，分别是 Noop I/O 调度器、Anticipatory I/O 调度器、Deadline I/O 调度器与 CFQ I/O 调度器。其中，Anticipatory I/O 调度器算法已经在 2010 年从内核中去掉了。

Noop I/O 调度器是一个简化的调度程序，实现了一个简单 FIFO 队列，它只进行最基本的合并，比较适合基于 Flash 的存储器。

Anticipatory I/O 调度器算法推迟 I/O 请求，以期能对它们进行排序，获得最高的效率。在每次处理完读请求之后，不是立即返回，而是等待几个微秒。在这段时间内，任何来自临近区域的请求都被立即执行。超时以后，继续原来的处理。

Deadline I/O 调度器针对 Anticipatory I/O 调度器的缺点进行改善而得来，试图把每次请求的延迟降至最低，并重排了请求的顺序来提高性能。它使用轮询的调度器，简洁小巧，提供最小的读取迟和尚佳的吞吐量，特别适合于读取较多的环境（例如数据库）。

CFQ I/O 调度器为系统内的所有任务分配均匀的 I/O 带宽，提供一个公平的工作环境，在多媒体应用中，能保证音、视频及时从磁盘中读取数据。

