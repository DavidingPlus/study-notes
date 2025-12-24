---
title: F2FS 文件系统
categories:
  - Linux 学习
  - 内核层
abbrlink: 257909f0
date: 2025-12-24 17:30:00
updated: 2025-12-24 17:30:00
---

<meta name="referrer" content="no-referrer"/>

# 为什么需要 F2FS？

传统的文件系统（如 FAT32, EXT4）是为“旋转的磁碟（HDD）”设计的，而 F2FS 是为“闪存（NAND Flash）”设计的。

在存储领域，传统的文件系统如 FAT32 或 EXT4 最初都是为机械硬盘这种“旋转磁碟”设计的。这类设备拥有物理磁头，在磁道上移动时可以直接覆盖旧数据。然而，F2FS 则是专门为 NAND 闪存设计的，它深刻理解闪存与磁盘在物理特性上的巨大差异。闪存最核心的限制在于它无法像磁碟那样进行原地“覆盖写”。一旦一个存储位置写入了数据，必须先执行擦除操作才能再次写入。更棘手的是，闪存写入的最小单位通常很小，只有几 KB 左右的“页”，但擦除的最小单位却非常大，往往达到数 MB 级别的“块”。如果文件系统像处理磁盘那样频繁地在同一个逻辑地址修改数据，底层的闪存就不得不不断执行繁重的“读-改-写”循环，这会导致整体写入性能随时间推移而急剧下降。

<!-- more -->

为了掩盖闪存这些古怪的物理特性，所有的闪存设备，无论是你手中的 SD 卡还是高性能的 SSD，内部都内置了一个微控制器来运行闪存转换层，也就是 FTL。FTL 的本质是一个欺骗机制，它让操作系统觉得这块闪存依然是一个可以随意覆盖写的普通磁盘。然而，当传统文件系统进行大量的随机写入时，FTL 必须在后台疯狂地进行“垃圾回收”和“磨损均衡”，试图在不断变化的物理空间中寻找空白区域来安置这些新数据。由于 FTL 并不了解文件系统的上层布局，它的这种盲目搬运效率极低。这就是为什么很多嵌入式设备或智能手机在使用一段时间后会变得卡顿，其深层原因往往是文件系统的随机写入行为让底层的 FTL 陷入了几乎崩溃的处理链条中。

F2FS 解决这一难题的核心武器是采用了**日志结构文件系统**，即 LFS 架构。我们可以做一个形象的比喻：传统文件系统就像一块黑板，哪里写错了就擦掉哪里，然后在原地重写；而 F2FS 则像是一个永远不使用橡皮擦的笔记本，无论你是新增文件还是修改旧文件，它永远不会去动之前写过的内容，而是不断在笔记本最后一页的空白处按顺序往后写。这种“追加式写入”的做法将所有的逻辑修改转换成了物理上的顺序写。对于底层闪存来说，顺序写入是最完美的运行模式，它极大地减轻了 FTL 的管理压力，让后台垃圾回收变得非常简单高效，同时也因为数据分布更加均匀而天然地实现了磨损均衡，从而显著延长了昂贵闪存硬件的寿命。

虽然 LFS 架构的概念很早就有，但早期的实现一直被“漫游树（wandering tree）”这一难题所困扰，导致无法大规模商用。在传统的 LFS 中，如果你修改了一个数据块，由于它必须写到新位置，物理地址就变了。这意味着指向这个块的上一级索引节点也必须修改地址来指向它，而索引节点的地址改变又会触发上上级节点的修改，最终这种递归更新会一直蔓延到文件系统的根节点，造成巨大的额外开销。**F2FS 的绝活在于它引入了节点地址表，也就是 NAT。**NAT 像一个稳固的中转站，它给每个索引节点分配一个固定的 ID，并记录这个 ID 目前对应的物理地址。当一个数据块移动时，F2FS 只需要更新 NAT 表里对应的一个条目即可，彻底切断了向上递归的“漫游”效应。正是这个核心发明，让 F2FS 能够保持 LFS 的高性能，又避开了复杂的连锁更新，成为了现代闪存存储的基石。

# 总体结构

相对于传统的日志结构型文件系统，F2FS 在 wandering tree 和 gc 的高时间开销等问题，有一定的改进和优化。

1. wandering tree 问题: 在传统的 LFS 中，由于采用异地更新（Out-of-place Update），当一个数据块被修改并写到新地址时，其父节点的指针也必须随之更新以指向新地址。这种更新会沿着文件系统的索引树一路向上递归，直到根节点。这种现象会导致极高的元数据（Metadata）写入开销，原本只改一个字节，最后可能导致整个树路径上的所有块都要重写。F2FS 通过引入 **NAT（Node Address Table）**层，成功切断了这种递归关联，实现了“不动声色”的地址转换。
2. 高 gc 开销问题: LFS 随着时间推移会产生大量无效数据（旧版本的块）。当磁盘空间不足时，必须启动垃圾回收。传统的 LFS 往往在 GC 时面临不可预知的长延迟，严重影响实时性能。F2FS 通过 **Multi-head Logging（多头日志写入）**和多种 GC 策略（贪婪算法与成本收益算法），根据数据的冷热程度进行分类存储，极大地降低了 GC 时的有效数据搬迁量，减少了系统的波动性。

## 系统特性

### 基本数据单位

分为四类：

1. **Block（块）：**F2FS 读写操作的最小原子单位，大小固定为 **4KB**。这一设计与 Linux 的 Page Cache 以及大多数闪存设备的 Page 大小高度契合。所有的元数据结构和用户数据均以 Block 为单位组织。
2. **Segment（段）：**管理 Block 的基本容器，大小为 **2MB**（包含 512 个 Block）。Segment 是 F2FS 进行空间管理和分配的基石。之所以选择 2MB，是为了适配主流闪存芯片的物理擦除块（Erase Block）大小，从而减少擦除时的干扰。
3. **Section（节）：**由连续的若干个 Segment 组成，默认情况下 **1 Section = 1 Segment**。Section 是垃圾回收（GC）的物理操作单位。F2FS 会根据数据的冷热属性，将数据分配到不同的 Section 中，以便在 GC 时能以更高的效率释放整块空间。
4. **Zone（区）：**由多个 Section 组成。在某些高级配置或特定硬件（如具有隔离特性的闪存阵列）上，Zone 用于进一步的物理隔离，以防止不同类型的数据在物理层面上产生写干扰。

### LFS 异地更新特性

F2FS 遵循“永远不要覆盖旧数据”的原则。这种**异地更新（Out-of-place Update）**策略是 F2FS 性能和可靠性的根源：

- **传统就地更新（In-place）：**如 FAT/EXT4。当修改文件 A 的第 1 块时，直接在原物理位置重写。对于闪存，这会触发底层昂贵的“读取-擦除-修改-写回”过程。
- **F2FS 异地更新：**当修改发生时，F2FS 直接在当前的 Log 尾部找一个干净的物理块写入新数据。旧块被标记为“Invalid（无效）”，留待未来的 GC 处理。

**优点：**

1. **写聚合：**将碎片化的随机写转换为大吞吐量的顺序写，极大提升 IOPS。
2. **磨损均衡：**数据自动在全盘范围内散布，避免了因频繁更新特定元数据（如文件表）导致的局部闪存颗粒提前损坏。

这样做的问题是：这种设计依赖 **Checkpoint（检查点）**机制来保证一致性。F2FS 不会像 JFS（日志文件系统）那样在每次操作时写日志，而是定期创建快照。这意味着如果突发断电且此时还没到 Checkpoint 时刻，可能会丢失极短时间内的数据。

### Multi-head Logging（多头日志写入）特性的分类

F2FS 的核心精髓在于它能感知数据的“温度”。Log 区域指的是文件系统中用于分配 free block(空闲的且没有写入数据的 block)的区域，例如 F2FS 的一个文件需要写入新数据，它就要去 Log 区域请求 free block，然后再将数据写入这个 free block 中。传统的 LFS j往往会维护一个大的日志区域，一切数据的分配都从这个大的日志区域中进行处理。它同时维护了 6 个活动的 Log 区域（活跃段），将数据精准分类：

- **节点类（Node）：存储文件的索引结构**
  - **HOT NODE**：目录文件的直接索引块（Direct Node）。由于目录操作极其频繁且对用户感知影响大，将其单独存放以加快访问速度。
  - **WARM NODE**：普通文件的直接索引块。
  - **COLD NODE**：间接索引块（Indirect Node）。这些块通常属于超大文件，修改频率相对较低。
- **数据类（Data）：存储用户实际内容**
  - **HOT DATA**：目录项数据（Dentry）。目录下的文件名、inode 号等信息经常变动。
  - **WARM DATA**：普通文件的内容。这是系统中最主要的数据流。
  - **COLD DATA**：多媒体文件、只读文件或 GC 搬迁产生的数据。这类数据一旦写入，很难再次被修改。

## 闪存设备物理布局

![image-20251224164153966](./F2FS 文件系统.assets/image-20251224164153966.png)

通过 mkfs.f2fs 格式化后，整个存储空间被划分为六大功能区。除了主数据区外，前五个区域统称为元数据区。

1. Superblock（超级块，SB）

位于分区的最开始，存储了文件系统的关键几何参数（块大小、段数量、区域边界）。为了保证安全性，F2FS 会保存两个 SB 备份。在挂载（Mount）时，驱动首先读取 SB 并初始化内存中的 f2fs_sb_info 结构。

2. Checkpoint（检查点，CP）

这是系统的“大脑”。CP 记录了当前文件系统的最新一致状态。它包含了两份备份（A/B 切换写入），以防在写 CP 过程中突然断电。CP 存储了 NAT 和 SIT 的位图快照、当前活跃段的列表以及 Orphan Inode（孤儿节点）等信息。在 RTEMS 重启后，挂载过程会根据 CP 来快速恢复整个文件系统的逻辑拓扑。

3. Segment Information Table（段信息表，SIT）

SIT 负责监控 **Main Area** 中每个 Segment 的使用情况。它包含一个位图，指示该段内 512 个块中哪些是有效的（Valid）。主要作用是 SIT 是 GC 决策的关键依据。系统通过查询 SIT 找到那些无效块最多的段进行回收。

4. Node Address Table（节点地址表，NAT）

建立了一张表保存了每一个 node 的物理地址信息。这是 F2FS 解决漫游树问题的秘密武器。

- **机制：**每个 Node（索引节点）都有一个唯一的逻辑 ID（NID）。NAT 就像一个巨大的映射表，记录了 NID -> 物理地址 的转换。
- **优势：**当文件数据更新导致 Node 本身位置发生移动时，只有 NAT 表中的物理地址需要更新，而指向该 NID 的父节点无需改动。这大大减少了元数据连锁更新的负担。

5. Segment Summary Area（段摘要区，SSA）

这个区域主要保存了 jounal(SIT/NAT 临时的修改信息)以及 summary(记录了逻辑地址和物理地址关系的结构，主要用于 GC)。SSA 区域在内存中没有专门的数据结构。SSA 存储了块的“身份证明”。

- **反向查找：**SSA 记录了主数据区每个物理块对应的父 Node ID 及其在文件内部的偏移。
- **主要用途：**在 GC 搬迁数据时，系统必须确定这个物理块到底是哪个文件的，SSA 提供了这种反向检索能力，确保搬迁后能正确更新对应的索引指针。

6. Main Area（主数据区）

这是文件系统最广阔的腹地，所有的用户数据、目录项、以及各种 Node 块（Inode 块、索引块）都存储在这里。所有的块分配都遵循前面提到的 6 个 Log 区域的冷热分类策略。

# SuperBlock 区域

SuperBlock 区域的结构如下：

![image-20251224165214273](./F2FS 文件系统.assets/image-20251224165214273.png)

Superblock区域是由两个 `struct f2fs_super_block` 结构组成，互为备份。

## struct f2fs_super_block

`struct f2fs_super_block` 定义了文件系统的几何参数和各功能区的物理偏移。我们可以观察到，F2FS 的许多设计都采用了 log2 的指数形式，例如 log_blocksize 和 log_blocks_per_seg。这种设计允许内核在计算物理地址时使用位移操作（Shift）代替昂贵的除法运算，这对于 CPU 资源有限的嵌入式设备（如运行 RTEMS 的环境）非常友好。此外，该结构体包含了一系列以 _blkaddr 结尾的字段，它们精确指向了 Checkpoint、SIT、NAT、SSA 以及 Main Area 的起始块地址。magic 字段（魔数）则用于标识这确实是一个 F2FS 卷，是驱动程序进行合法性检查的第一道关卡。定义如下：

```c
struct f2fs_super_block {
	__le32 magic;			/* Magic Number */
	__le16 major_ver;		/* Major Version */
	__le16 minor_ver;		/* Minor Version */
	__le32 log_sectorsize;		/* log2 sector size in bytes */
	__le32 log_sectors_per_block;	/* log2 # of sectors per block */
	__le32 log_blocksize;		/* log2 block size in bytes */
	__le32 log_blocks_per_seg;	/* log2 # of blocks per segment */
	__le32 segs_per_sec;		/* # of segments per section */
	__le32 secs_per_zone;		/* # of sections per zone */
	__le32 checksum_offset;		/* checksum offset inside super block */
	__le64 block_count;		/* total # of user blocks */
	__le32 section_count;		/* total # of sections */
	__le32 segment_count;		/* total # of segments */
	__le32 segment_count_ckpt;	/* # of segments for checkpoint */
	__le32 segment_count_sit;	/* # of segments for SIT */
	__le32 segment_count_nat;	/* # of segments for NAT */
	__le32 segment_count_ssa;	/* # of segments for SSA */
	__le32 segment_count_main;	/* # of segments for main area */
	__le32 segment0_blkaddr;	/* start block address of segment 0 */
	__le32 cp_blkaddr;		/* start block address of checkpoint */
	__le32 sit_blkaddr;		/* start block address of SIT */
	__le32 nat_blkaddr;		/* start block address of NAT */
	__le32 ssa_blkaddr;		/* start block address of SSA */
	__le32 main_blkaddr;		/* start block address of main area */
	__le32 root_ino;		/* root inode number */
	__le32 node_ino;		/* node inode number */
	__le32 meta_ino;		/* meta inode number */
	__u8 uuid[16];			/* 128-bit uuid for volume */
	__le16 volume_name[MAX_VOLUME_NAME];	/* volume name */
	__le32 extension_count;		/* # of extensions below */
	__u8 extension_list[F2FS_MAX_EXTENSION][F2FS_EXTENSION_LEN];/* extension array */
	__le32 cp_payload;
	__u8 version[VERSION_LEN];	/* the kernel version */
	__u8 init_version[VERSION_LEN];	/* the initial kernel version */
	__le32 feature;			/* defined features */
	__u8 encryption_level;		/* versioning level for encryption */
	__u8 encrypt_pw_salt[16];	/* Salt used for string2key algorithm */
	struct f2fs_device devs[MAX_DEVICES];	/* device list */
	__le32 qf_ino[F2FS_MAX_QUOTAS];	/* quota inode numbers */
	__u8 hot_ext_count;		/* # of hot file extension */
	__le16  s_encoding;		/* Filename charset encoding */
	__le16  s_encoding_flags;	/* Filename charset encoding flags */
	__u8 s_stop_reason[MAX_STOP_REASON];	/* stop checkpoint reason */
	__u8 s_errors[MAX_F2FS_ERRORS];		/* reason of image corrupts */
	__u8 reserved[258];		/* valid reserved region */
	__le32 crc;			/* checksum of superblock */
} __packed;
```

## struct f2fs_sb_info

当 RTEMS 成功从磁盘读取了原始的超级块数据后，它并不会直接频繁操作磁盘上的那段字节流，而是会初始化一个内存管理结构体 `struct f2fs_sb_info`（简称 SBI）。SBI 是 F2FS 在运行时的“指挥部”，它不仅包含了原始超级块的指针 raw_super，还整合了节点管理器（NM）、段管理器（SM）以及用于保证线程安全的各类锁机制（如 sb_lock、gc_mutex）。对于移植工作而言，SBI 是最频繁被打交道的数据结构，它将静态的磁盘布局转换成了动态的内存对象，负责维护文件系统的实时挂载状态。`struct f2fs_super_block` 在内存中对应的结构是 `struct f2fs_sb_info`，它除了包含了`struct f2fs_super_block`的信息以外，还包含了一些额外的功能，如锁、SIT、NAT 对应的内存管理结构等。定义如下：

```c
struct f2fs_sb_info {
	struct super_block *sb;			/* pointer to VFS super block */
	struct proc_dir_entry *s_proc;		/* proc entry */
	struct f2fs_super_block *raw_super;	/* raw super block pointer */
	struct f2fs_rwsem sb_lock;		/* lock for raw super block */
	int valid_super_block;			/* valid super block no */
	unsigned long s_flag;				/* flags for sbi */
	struct mutex writepages;		/* mutex for writepages() */

#ifdef CONFIG_BLK_DEV_ZONED
	unsigned int blocks_per_blkz;		/* F2FS blocks per zone */
	unsigned int max_open_zones;		/* max open zone resources of the zoned device */
	/* For adjust the priority writing position of data in zone UFS */
	unsigned int blkzone_alloc_policy;
#endif

	/* for node-related operations */
	struct f2fs_nm_info *nm_info;		/* node manager */
	struct inode *node_inode;		/* cache node blocks */

	/* for segment-related operations */
	struct f2fs_sm_info *sm_info;		/* segment manager */

	/* for bio operations */
	struct f2fs_bio_info *write_io[NR_PAGE_TYPE];	/* for write bios */
	/* keep migration IO order for LFS mode */
	struct f2fs_rwsem io_order_lock;
	pgoff_t page_eio_ofs[NR_PAGE_TYPE];	/* EIO page offset */
	int page_eio_cnt[NR_PAGE_TYPE];		/* EIO count */

	/* for checkpoint */
	struct f2fs_checkpoint *ckpt;		/* raw checkpoint pointer */
	int cur_cp_pack;			/* remain current cp pack */
	spinlock_t cp_lock;			/* for flag in ckpt */
	struct inode *meta_inode;		/* cache meta blocks */
	struct f2fs_rwsem cp_global_sem;	/* checkpoint procedure lock */
	struct f2fs_rwsem cp_rwsem;		/* blocking FS operations */
	struct f2fs_rwsem node_write;		/* locking node writes */
	struct f2fs_rwsem node_change;	/* locking node change */
	wait_queue_head_t cp_wait;
	unsigned long last_time[MAX_TIME];	/* to store time in jiffies */
	long interval_time[MAX_TIME];		/* to store thresholds */
	struct ckpt_req_control cprc_info;	/* for checkpoint request control */
	struct cp_stats cp_stats;		/* for time stat of checkpoint */

	struct inode_management im[MAX_INO_ENTRY];	/* manage inode cache */

	spinlock_t fsync_node_lock;		/* for node entry lock */
	struct list_head fsync_node_list;	/* node list head */
	unsigned int fsync_seg_id;		/* sequence id */
	unsigned int fsync_node_num;		/* number of node entries */

	/* for orphan inode, use 0'th array */
	unsigned int max_orphans;		/* max orphan inodes */

	/* for inode management */
	struct list_head inode_list[NR_INODE_TYPE];	/* dirty inode list */
	spinlock_t inode_lock[NR_INODE_TYPE];	/* for dirty inode list lock */
	struct mutex flush_lock;		/* for flush exclusion */

	/* for extent tree cache */
	struct extent_tree_info extent_tree[NR_EXTENT_CACHES];
	atomic64_t allocated_data_blocks;	/* for block age extent_cache */
	unsigned int max_read_extent_count;	/* max read extent count per inode */

	/* The threshold used for hot and warm data seperation*/
	unsigned int hot_data_age_threshold;
	unsigned int warm_data_age_threshold;
	unsigned int last_age_weight;

	/* control donate caches */
	unsigned int donate_files;

	/* basic filesystem units */
	unsigned int log_sectors_per_block;	/* log2 sectors per block */
	unsigned int log_blocksize;		/* log2 block size */
	unsigned int blocksize;			/* block size */
	unsigned int root_ino_num;		/* root inode number*/
	unsigned int node_ino_num;		/* node inode number*/
	unsigned int meta_ino_num;		/* meta inode number*/
	unsigned int log_blocks_per_seg;	/* log2 blocks per segment */
	unsigned int blocks_per_seg;		/* blocks per segment */
	unsigned int unusable_blocks_per_sec;	/* unusable blocks per section */
	unsigned int segs_per_sec;		/* segments per section */
	unsigned int secs_per_zone;		/* sections per zone */
	unsigned int total_sections;		/* total section count */
	unsigned int total_node_count;		/* total node block count */
	unsigned int total_valid_node_count;	/* valid node block count */
	int dir_level;				/* directory level */
	bool readdir_ra;			/* readahead inode in readdir */
	u64 max_io_bytes;			/* max io bytes to merge IOs */

	block_t user_block_count;		/* # of user blocks */
	block_t total_valid_block_count;	/* # of valid blocks */
	block_t discard_blks;			/* discard command candidats */
	block_t last_valid_block_count;		/* for recovery */
	block_t reserved_blocks;		/* configurable reserved blocks */
	block_t current_reserved_blocks;	/* current reserved blocks */

	/* Additional tracking for no checkpoint mode */
	block_t unusable_block_count;		/* # of blocks saved by last cp */

	unsigned int nquota_files;		/* # of quota sysfile */
	struct f2fs_rwsem quota_sem;		/* blocking cp for flags */
	struct task_struct *umount_lock_holder;	/* s_umount lock holder */

	/* # of pages, see count_type */
	atomic_t nr_pages[NR_COUNT_TYPE];
	/* # of allocated blocks */
	struct percpu_counter alloc_valid_block_count;
	/* # of node block writes as roll forward recovery */
	struct percpu_counter rf_node_block_count;

	/* writeback control */
	atomic_t wb_sync_req[META];	/* count # of WB_SYNC threads */

	/* valid inode count */
	struct percpu_counter total_valid_inode_count;

	struct f2fs_mount_info mount_opt;	/* mount options */

	/* for cleaning operations */
	struct f2fs_rwsem gc_lock;		/*
						 * semaphore for GC, avoid
						 * race between GC and GC or CP
						 */
	struct f2fs_gc_kthread	*gc_thread;	/* GC thread */
	struct atgc_management am;		/* atgc management */
	unsigned int cur_victim_sec;		/* current victim section num */
	unsigned int gc_mode;			/* current GC state */
	unsigned int next_victim_seg[2];	/* next segment in victim section */
	spinlock_t gc_remaining_trials_lock;
	/* remaining trial count for GC_URGENT_* and GC_IDLE_* */
	unsigned int gc_remaining_trials;

	/* for skip statistic */
	unsigned long long skipped_gc_rwsem;		/* FG_GC only */

	/* free sections reserved for pinned file */
	unsigned int reserved_pin_section;

	/* threshold for gc trials on pinned files */
	unsigned short gc_pin_file_threshold;
	struct f2fs_rwsem pin_sem;

	/* maximum # of trials to find a victim segment for SSR and GC */
	unsigned int max_victim_search;
	/* migration granularity of garbage collection, unit: segment */
	unsigned int migration_granularity;
	/* migration window granularity of garbage collection, unit: segment */
	unsigned int migration_window_granularity;

	/*
	 * for stat information.
	 * one is for the LFS mode, and the other is for the SSR mode.
	 */
#ifdef CONFIG_F2FS_STAT_FS
	struct f2fs_stat_info *stat_info;	/* FS status information */
	atomic_t meta_count[META_MAX];		/* # of meta blocks */
	unsigned int segment_count[2];		/* # of allocated segments */
	unsigned int block_count[2];		/* # of allocated blocks */
	atomic_t inplace_count;		/* # of inplace update */
	/* # of lookup extent cache */
	atomic64_t total_hit_ext[NR_EXTENT_CACHES];
	/* # of hit rbtree extent node */
	atomic64_t read_hit_rbtree[NR_EXTENT_CACHES];
	/* # of hit cached extent node */
	atomic64_t read_hit_cached[NR_EXTENT_CACHES];
	/* # of hit largest extent node in read extent cache */
	atomic64_t read_hit_largest;
	atomic_t inline_xattr;			/* # of inline_xattr inodes */
	atomic_t inline_inode;			/* # of inline_data inodes */
	atomic_t inline_dir;			/* # of inline_dentry inodes */
	atomic_t compr_inode;			/* # of compressed inodes */
	atomic64_t compr_blocks;		/* # of compressed blocks */
	atomic_t swapfile_inode;		/* # of swapfile inodes */
	atomic_t atomic_files;			/* # of opened atomic file */
	atomic_t max_aw_cnt;			/* max # of atomic writes */
	unsigned int io_skip_bggc;		/* skip background gc for in-flight IO */
	unsigned int other_skip_bggc;		/* skip background gc for other reasons */
	unsigned int ndirty_inode[NR_INODE_TYPE];	/* # of dirty inodes */
	atomic_t cp_call_count[MAX_CALL_TYPE];	/* # of cp call */
#endif
	spinlock_t stat_lock;			/* lock for stat operations */

	/* to attach REQ_META|REQ_FUA flags */
	unsigned int data_io_flag;
	unsigned int node_io_flag;

	/* For sysfs support */
	struct kobject s_kobj;			/* /sys/fs/f2fs/<devname> */
	struct completion s_kobj_unregister;

	struct kobject s_stat_kobj;		/* /sys/fs/f2fs/<devname>/stat */
	struct completion s_stat_kobj_unregister;

	struct kobject s_feature_list_kobj;		/* /sys/fs/f2fs/<devname>/feature_list */
	struct completion s_feature_list_kobj_unregister;

	/* For shrinker support */
	struct list_head s_list;
	struct mutex umount_mutex;
	unsigned int shrinker_run_no;

	/* For multi devices */
	int s_ndevs;				/* number of devices */
	struct f2fs_dev_info *devs;		/* for device list */
	unsigned int dirty_device;		/* for checkpoint data flush */
	spinlock_t dev_lock;			/* protect dirty_device */
	bool aligned_blksize;			/* all devices has the same logical blksize */
	unsigned int first_seq_zone_segno;	/* first segno in sequential zone */
	unsigned int bggc_io_aware;		/* For adjust the BG_GC priority when pending IO */
	unsigned int allocate_section_hint;	/* the boundary position between devices */
	unsigned int allocate_section_policy;	/* determine the section writing priority */

	/* For write statistics */
	u64 sectors_written_start;
	u64 kbytes_written;

	/* Precomputed FS UUID checksum for seeding other checksums */
	__u32 s_chksum_seed;

	struct workqueue_struct *post_read_wq;	/* post read workqueue */

	/*
	 * If we are in irq context, let's update error information into
	 * on-disk superblock in the work.
	 */
	struct work_struct s_error_work;
	unsigned char errors[MAX_F2FS_ERRORS];		/* error flags */
	unsigned char stop_reason[MAX_STOP_REASON];	/* stop reason */
	spinlock_t error_lock;			/* protect errors/stop_reason array */
	bool error_dirty;			/* errors of sb is dirty */

	struct kmem_cache *inline_xattr_slab;	/* inline xattr entry */
	unsigned int inline_xattr_slab_size;	/* default inline xattr slab size */

	/* For reclaimed segs statistics per each GC mode */
	unsigned int gc_segment_mode;		/* GC state for reclaimed segments */
	unsigned int gc_reclaimed_segs[MAX_GC_MODE];	/* Reclaimed segs for each mode */

	unsigned long seq_file_ra_mul;		/* multiplier for ra_pages of seq. files in fadvise */

	int max_fragment_chunk;			/* max chunk size for block fragmentation mode */
	int max_fragment_hole;			/* max hole size for block fragmentation mode */

	/* For atomic write statistics */
	atomic64_t current_atomic_write;
	s64 peak_atomic_write;
	u64 committed_atomic_block;
	u64 revoked_atomic_block;

	/* carve out reserved_blocks from total blocks */
	bool carve_out;

#ifdef CONFIG_F2FS_FS_COMPRESSION
	struct kmem_cache *page_array_slab;	/* page array entry */
	unsigned int page_array_slab_size;	/* default page array slab size */

	/* For runtime compression statistics */
	u64 compr_written_block;
	u64 compr_saved_block;
	u32 compr_new_inode;

	/* For compressed block cache */
	struct inode *compress_inode;		/* cache compressed blocks */
	unsigned int compress_percent;		/* cache page percentage */
	unsigned int compress_watermark;	/* cache page watermark */
	atomic_t compress_page_hit;		/* cache hit count */
#endif

#ifdef CONFIG_F2FS_IOSTAT
	/* For app/fs IO statistics */
	spinlock_t iostat_lock;
	unsigned long long iostat_count[NR_IO_TYPE];
	unsigned long long iostat_bytes[NR_IO_TYPE];
	unsigned long long prev_iostat_bytes[NR_IO_TYPE];
	bool iostat_enable;
	unsigned long iostat_next_period;
	unsigned int iostat_period_ms;

	/* For io latency related statistics info in one iostat period */
	spinlock_t iostat_lat_lock;
	struct iostat_lat_info *iostat_io_lat;
#endif
};
```

## init_sb_info()

init_sb_info() 函数承担了从“原始字节”到“逻辑对象”的转换任务。首先，它会执行大量的字节序转换（如 le32_to_cpu），确保无论磁盘是在大端还是小端机器上格式化的，在当前 CPU 内存中都能正确解析。接着，它会计算出一些关键的导出参数，如总节点数量、根目录索引号（root_ino）等。除此之外，该函数还会初始化文件系统的同步原语，包括各种读写信号量、互斥锁和自旋锁。在 RTEMS 移植过程中，你需要将这些 Linux 特有的锁接口（如 mutex_init 或 init_rwsem）映射到 RTEMS 的信号量或互斥量 API 上。只有正确完成了这一步，F2FS 才能在多线程任务环境下安全地操作元数据。定义如下：

```c
static void init_sb_info(struct f2fs_sb_info *sbi)
{
	struct f2fs_super_block *raw_super = sbi->raw_super;
	int i;

	sbi->log_sectors_per_block =
		le32_to_cpu(raw_super->log_sectors_per_block);
	sbi->log_blocksize = le32_to_cpu(raw_super->log_blocksize);
	sbi->blocksize = BIT(sbi->log_blocksize);
	sbi->log_blocks_per_seg = le32_to_cpu(raw_super->log_blocks_per_seg);
	sbi->blocks_per_seg = BIT(sbi->log_blocks_per_seg);
	sbi->segs_per_sec = le32_to_cpu(raw_super->segs_per_sec);
	sbi->secs_per_zone = le32_to_cpu(raw_super->secs_per_zone);
	sbi->total_sections = le32_to_cpu(raw_super->section_count);
	sbi->total_node_count = SEGS_TO_BLKS(sbi,
			((le32_to_cpu(raw_super->segment_count_nat) / 2) *
			NAT_ENTRY_PER_BLOCK));
	sbi->allocate_section_hint = le32_to_cpu(raw_super->section_count);
	sbi->allocate_section_policy = ALLOCATE_FORWARD_NOHINT;
	F2FS_ROOT_INO(sbi) = le32_to_cpu(raw_super->root_ino);
	F2FS_NODE_INO(sbi) = le32_to_cpu(raw_super->node_ino);
	F2FS_META_INO(sbi) = le32_to_cpu(raw_super->meta_ino);
	sbi->cur_victim_sec = NULL_SECNO;
	sbi->gc_mode = GC_NORMAL;
	sbi->next_victim_seg[BG_GC] = NULL_SEGNO;
	sbi->next_victim_seg[FG_GC] = NULL_SEGNO;
	sbi->max_victim_search = DEF_MAX_VICTIM_SEARCH;
	sbi->migration_granularity = SEGS_PER_SEC(sbi);
	sbi->migration_window_granularity = f2fs_sb_has_blkzoned(sbi) ?
		DEF_MIGRATION_WINDOW_GRANULARITY_ZONED : SEGS_PER_SEC(sbi);
	sbi->seq_file_ra_mul = MIN_RA_MUL;
	sbi->max_fragment_chunk = DEF_FRAGMENT_SIZE;
	sbi->max_fragment_hole = DEF_FRAGMENT_SIZE;
	spin_lock_init(&sbi->gc_remaining_trials_lock);
	atomic64_set(&sbi->current_atomic_write, 0);

	sbi->dir_level = DEF_DIR_LEVEL;
	sbi->interval_time[CP_TIME] = DEF_CP_INTERVAL;
	sbi->interval_time[REQ_TIME] = DEF_IDLE_INTERVAL;
	sbi->interval_time[DISCARD_TIME] = DEF_IDLE_INTERVAL;
	sbi->interval_time[GC_TIME] = DEF_IDLE_INTERVAL;
	sbi->interval_time[DISABLE_TIME] = DEF_DISABLE_INTERVAL;
	sbi->interval_time[ENABLE_TIME] = DEF_ENABLE_INTERVAL;
	sbi->interval_time[UMOUNT_DISCARD_TIMEOUT] =
				DEF_UMOUNT_DISCARD_TIMEOUT;
	clear_sbi_flag(sbi, SBI_NEED_FSCK);

	for (i = 0; i < NR_COUNT_TYPE; i++)
		atomic_set(&sbi->nr_pages[i], 0);

	for (i = 0; i < META; i++)
		atomic_set(&sbi->wb_sync_req[i], 0);

	INIT_LIST_HEAD(&sbi->s_list);
	mutex_init(&sbi->umount_mutex);
	init_f2fs_rwsem(&sbi->io_order_lock);
	spin_lock_init(&sbi->cp_lock);

	sbi->dirty_device = 0;
	spin_lock_init(&sbi->dev_lock);

	init_f2fs_rwsem(&sbi->sb_lock);
	init_f2fs_rwsem(&sbi->pin_sem);
}
```

# CheckPoint 区域

Checkpoint（检查点，简称 CP）是 F2FS 维护数据一致性的灵魂结构，它记录了文件系统在某一时刻的完整“快照”状态。由于 F2FS 采用日志结构（LFS）不断进行异地更新，系统的元数据（如 NAT、SIT）和数据位置总是在动态变化。为了保证在系统突然断电或崩溃后能够恢复，F2FS 会定期或在特定条件下（如 Umount、Sync）将当前活跃的段信息、节点分配情况以及位图快照写入 Checkpoint 区域。为了极致的可靠性，F2FS 维护了两份互为备份的 CP 结构（通常称为 CP #0 和 CP #1）。**系统始终保持一个为“稳定版本”，另一个为“正在写入版本”。**如果在更新 CP 时发生故障，系统可以安全地回退到上一个稳定的 CP，确保文件系统不会因为元数据损坏而无法挂载。

CheckPoint 区域的结构如下。从图中看出分别是 checkpoint 元数据区域(f2fs_checkpoint)、orphan node 区域、active segments 区域。同时 active segments 区域在不同的情况下，会有不同的形式，目的是减少 IO 的写入。

![image-20251224170347186](./F2FS 文件系统.assets/image-20251224170347186.png)

## struct f2fs_checkpoint

`struct f2fs_checkpoint` 定义了恢复系统所需的最少信息量。其中最重要的字段是 checkpoint_ver（版本号），系统通过比较两个 CP 备份的版本号来确定哪一个是最新的。此外，cur_node_segno 和 cur_node_blkoff（以及对应的 Data 字段）记录了上次 CP 时系统 6 个写入头的精确坐标。如果没有这些坐标，挂载后文件系统将不知道该从哪个物理块开始继续追加数据。同时，结构体末尾的 sit_nat_version_bitmap 记录了 SIT 和 NAT 区域的版本位图，帮助系统快速定位元数据表的最前沿更新。F2FS 必须定时执行 Checkpoint 去记录当前系统的 log 分配到哪个位置，否则在系统宕机的时候，会出现数据丢失等一致性问题，因此 cur_xxx_segno 以及 cur_xxx_blkoff 记录了上次 Checkpoint 时，系统正在使用的 log 的 segment number，以及分配到这个 segment 的哪个位置。定义如下：

```c
struct f2fs_checkpoint {
	__le64 checkpoint_ver;		/* checkpoint block version number */
	__le64 user_block_count;	/* # of user blocks */
	__le64 valid_block_count;	/* # of valid blocks in main area */
	__le32 rsvd_segment_count;	/* # of reserved segments for gc */
	__le32 overprov_segment_count;	/* # of overprovision segments */
	__le32 free_segment_count;	/* # of free segments in main area */

	/* information of current node segments */
	__le32 cur_node_segno[MAX_ACTIVE_NODE_LOGS];
	__le16 cur_node_blkoff[MAX_ACTIVE_NODE_LOGS];
	/* information of current data segments */
	__le32 cur_data_segno[MAX_ACTIVE_DATA_LOGS];
	__le16 cur_data_blkoff[MAX_ACTIVE_DATA_LOGS];
	__le32 ckpt_flags;		/* Flags : umount and journal_present */
	__le32 cp_pack_total_block_count;	/* total # of one cp pack */
	__le32 cp_pack_start_sum;	/* start block number of data summary */
	__le32 valid_node_count;	/* Total number of valid nodes */
	__le32 valid_inode_count;	/* Total number of valid inodes */
	__le32 next_free_nid;		/* Next free node number */
	__le32 sit_ver_bitmap_bytesize;	/* Default value 64 */
	__le32 nat_ver_bitmap_bytesize; /* Default value 256 */
	__le32 checksum_offset;		/* checksum offset inside cp block */
	__le64 elapsed_time;		/* mounted time */
	/* allocation type of current segment */
	unsigned char alloc_type[MAX_ACTIVE_LOGS];

	/* SIT and NAT version bitmap */
	unsigned char sit_nat_version_bitmap[];
} __packed;
```

## Orphan Node

这是一个动态的区域，如果没有 orphan node list 则不会占用空间。

## Active Segments

Active Segments 又被称为 **CURSEG**，是 F2FS 实施 Multi-head Logging 策略的直接体现。为了提高效率，F2FS 并非每次分配一个块就去更新一次磁盘上的 SIT（段信息表）或 NAT（节点地址表），因为这样会带来巨大的写压力。相反，它利用 CP 区域中的 **Journal（日志）**和 **Summary（摘要）**结构作为临时缓存。Journal 记录了活跃段中元数据的频繁修改，而 Summary 记录了逻辑地址与物理地址的映射。只有当触发 Checkpoint 操作时，这些暂存在内存和 CP 区域的信息才会被批量持久化。这种设计极大地平衡了闪存的寿命与系统的实时性能。

CP 的主要任务是维护数据一致性，因此 CP 的 active segment 区域的主要任务是维护 Active Segment 的分配状态，使系统宕机时候可以恢复正常。维护 active segment 需要维护三种信息，分别是 f2fs_checkpoint 的信息，以及该 segment 对应的 journal 和 summary 的信息。

- **f2fs_checkpoint 中 Active Segment 信息**：从上面给出的 f2fs_checkpoint 定义，cur_node_segno[MAX_ACTIVE_NODE_LOGS] 和 cur_data_segno[MAX_ACTIVE_DATA_LOGS] 表示 node 和 data 当前的 Active Segment 的编号(segment number, segno)，系统可以通过这个编号找到对应的 segment。MAX_ACTIVE_NODE_LOGS 以及 MAX_ACTIVE_NODE_LOGS 分别表示 data 和 node 有多少种类型，F2FS 默认情况下都等于 3，表示 HOT、WARM、COLD 类型数据。cur_node_blkoff[MAX_ACTIVE_NODE_LOGS] 以及 cur_data_blkoff[MAX_ACTIVE_DATA_LOGS] 则分别表示当前 active segment 分配到哪一个 block(一个 segment 包含了 512 个 block)。
- **Segment 对应的 Journal 信息**：Journal 在两处地方都有出现，分别是 CP 区域以及 SSA 区域。CP 区域的 journal 主要用来保存 **active segment** 的修改信息，而 SSA 区域的则是持久化保存的**所有的 segment** 的 journal 信息。如系统分配出一个 block 给用户，那么就要将这个 block 所在的 segment 的 bitmap 中标记为已分配，防止其他写请求使用。分两个区域存放 journal 是为了减轻频繁更新导致的系统性能下降。例如，当系统写压力很大的时候，bitmap 就会频繁被更新，如果这个时候频繁将 bitmap 写入 SSA，就会加重写压力。因此 CP 区域的 Journal 的作用就是维护这些经常修改的数据，等待 CP 被触发的时候才回写到闪存设备，从而减少写压力，提高闪存寿命。
- **Segment 对应的 Summary 信息**：summary 同样在 CP 区域和 SSA 区域有出现，它表示的是逻辑地址和物理地址的映射关系，这个映射关系会使用到 GC 流程中。summary 与 segment 是一对一的关系，一个 summary 保存了一个 segment 所有的 block 的物理地址和逻辑地址的映射关系。summary 保存在 CP 区域中同样是出于减少 IO 的写入。

## 内存管理结构

Checkpoint 的内存管理结构是 `struct f2fs_checkpoint` 本身，因为 Checkpoint 一般只在 F2FS 启动的时候被读取数据，用于数据恢复，而在运行过程中大部分情况都是被写，用于记录恢复信息。因此，Checkpoint 不需要过于复杂的内存管理结构，因此使用 `struct f2fs_checkpoint` 本身即可以满足需求。

F2FS 的 log，即 active segments，主要用于系统 free block 的分配，因此需要特定的管理结构 `struct curseg_info` 进行管理。定义如下：

```c
/* for active log information */
struct curseg_info {
	struct mutex curseg_mutex;		/* lock for consistency */
	struct f2fs_summary_block *sum_blk;	/* cached summary block */
	struct rw_semaphore journal_rwsem;	/* protect journal area */
	struct f2fs_journal *journal;		/* cached journal info */
	unsigned char alloc_type;		/* current allocation type */
	unsigned short seg_type;		/* segment type like CURSEG_XXX_TYPE */
	unsigned int segno;			/* current segment number */
	unsigned short next_blkoff;		/* next block offset to write */
	unsigned int zone;			/* current zone number */
	unsigned int next_segno;		/* preallocated segment */
	int fragment_remained_chunk;		/* remained block size in a chunk for block fragmentation mode */
	bool inited;				/* indicate inmem log is inited */
};
```

从结构分析可以直到，`curseg_info` 记录当前的 segment 的分配信息，当系统出现宕机的时候，可以从 CP 记录的 `curseg_info` 恢复当上一次 CP 点的状态。每一种类型的 active segment 就对应一个 `struct curseg_info` 结构。在 F2FS 中，使用一个数组来表示:

```c
struct f2fs_sm_info {
	...
	struct curseg_info *curseg_array; // 默认是分配6个curseg_info，分别对应不同类型
	...
}
```

`struct f2fs_sm_info`是 SIT 的管理结构，它也管理了 CP 最终的 active segment 的信息，是一个跨区域的管理结构。

`struct f2fs_checkpoint` 通过 get_checkpoint_version() 函数从磁盘读取出来：

```c
static int get_checkpoint_version(struct f2fs_sb_info *sbi, block_t cp_addr,
		struct f2fs_checkpoint **cp_block, struct page **cp_page,
		unsigned long long *version)
{
	unsigned long blk_size = sbi->blocksize;
	size_t crc_offset = 0;
	__u32 crc = 0;

	*cp_page = f2fs_get_meta_page(sbi, cp_addr); // 根据CP所在的地址cp_addr从磁盘读取一个block
	*cp_block = (struct f2fs_checkpoint *)page_address(*cp_page); // 直接转换为数据结构

	crc_offset = le32_to_cpu((*cp_block)->checksum_offset);
	if (crc_offset > (blk_size - sizeof(__le32))) {
		f2fs_msg(sbi->sb, KERN_WARNING,
			"invalid crc_offset: %zu", crc_offset);
		return -EINVAL;
	}

	crc = cur_cp_crc(*cp_block);
	if (!f2fs_crc_valid(sbi, crc, *cp_block, crc_offset)) { // 比较CRC的值，进而知道是否成功读取出来
		f2fs_msg(sbi->sb, KERN_WARNING, "invalid crc value");
		return -EINVAL;
	}

	*version = cur_cp_version(*cp_block);
	return 0;
}
```

`struct curseg_info` 则是通过 build_curseg() 函数进行初始化:

```c
static int build_curseg(struct f2fs_sb_info *sbi)
{
	struct curseg_info *array;
	int i;

	array = f2fs_kzalloc(sbi, array_size(NR_CURSEG_TYPE, sizeof(*array)),
			     GFP_KERNEL); // 根据active segment类型的数目分配空间
	if (!array)
		return -ENOMEM;

	SM_I(sbi)->curseg_array = array; // 赋值到f2fs_sm_info->curseg_array

	for (i = 0; i < NR_CURSEG_TYPE; i++) { // 为curseg的其他信息分配空间
		mutex_init(&array[i].curseg_mutex);
		array[i].sum_blk = f2fs_kzalloc(sbi, PAGE_SIZE, GFP_KERNEL);
		if (!array[i].sum_blk)
			return -ENOMEM;
		init_rwsem(&array[i].journal_rwsem);
		array[i].journal = f2fs_kzalloc(sbi,
				sizeof(struct f2fs_journal), GFP_KERNEL);
		if (!array[i].journal)
			return -ENOMEM;
		array[i].segno = NULL_SEGNO;
		array[i].next_blkoff = 0;
	}
	return restore_curseg_summaries(sbi); // 从f2fs_checkpoint恢复上一个CP点CURSEG的状态
}

static int restore_curseg_summaries(struct f2fs_sb_info *sbi)
{
	struct f2fs_journal *sit_j = CURSEG_I(sbi, CURSEG_COLD_DATA)->journal;
	struct f2fs_journal *nat_j = CURSEG_I(sbi, CURSEG_HOT_DATA)->journal;
	int type = CURSEG_HOT_DATA;
	int err;

	...
	for (; type <= CURSEG_COLD_NODE; type++) { // 按类型逐个恢复active segment的信息
		err = read_normal_summaries(sbi, type);
		if (err)
			return err;
	}
	...

	return 0;
}

static int read_normal_summaries(struct f2fs_sb_info *sbi, int type)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	struct f2fs_summary_block *sum;
	struct curseg_info *curseg;
	struct page *new;
	unsigned short blk_off;
	unsigned int segno = 0;
	block_t blk_addr = 0;

	...
	segno = le32_to_cpu(ckpt->cur_data_segno[type]); // 从CP读取segno
	blk_off = le16_to_cpu(ckpt->cur_data_blkoff[type - CURSEG_HOT_DATA]); // 从CP读取blk_off
	blk_addr = sum_blk_addr(sbi, NR_CURSEG_DATA_TYPE, type); // 获取summary block地址	

	// 读取&转换结构
	new = f2fs_get_meta_page(sbi, blk_addr);
	sum = (struct f2fs_summary_block *)page_address(new);

	curseg = CURSEG_I(sbi, type); // 根据type找到对应的curseg
	mutex_lock(&curseg->curseg_mutex);

	/* 复制&恢复数据 */
	down_write(&curseg->journal_rwsem);
	memcpy(curseg->journal, &sum->journal, SUM_JOURNAL_SIZE);
	up_write(&curseg->journal_rwsem);

	memcpy(curseg->sum_blk->entries, sum->entries, SUM_ENTRY_SIZE);
	memcpy(&curseg->sum_blk->footer, &sum->footer, SUM_FOOTER_SIZE);
	curseg->next_segno = segno;
	reset_curseg(sbi, type, 0);
	curseg->alloc_type = ckpt->alloc_type[type];
	curseg->next_blkoff = blk_off; // 恢复上次的分配状态
	mutex_unlock(&curseg->curseg_mutex);
	f2fs_put_page(new, 1);
	return 0;
}
```

# 参考文档

1. [RiweiPan/F2FS-NOTES: F2FS的学习笔记以及源码分析](https://github.com/RiweiPan/F2FS-NOTES)

