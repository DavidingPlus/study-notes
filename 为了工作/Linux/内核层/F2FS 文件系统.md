---
title: F2FS 文件系统
categories:
  - Linux 学习
  - 内核层
abbrlink: 257909f0
date: 2025-12-24 17:30:00
updated: 2025-12-25 16:40:00
---

<meta name="referrer" content="no-referrer"/>

# 为什么需要 F2FS？

传统的文件系统（如 FAT32, EXT4）是为“旋转的磁碟（HDD）”设计的，而 F2FS 是为“闪存（NAND Flash）”设计的。

在存储领域，传统的文件系统如 FAT32 或 EXT4 最初都是为机械硬盘这种“旋转磁碟”设计的。这类设备拥有物理磁头，在磁道上移动时可以直接覆盖旧数据。然而，F2FS 则是专门为 NAND 闪存设计的，它深刻理解闪存与磁盘在物理特性上的巨大差异。闪存最核心的限制在于它无法像磁碟那样进行原地“覆盖写”。一旦一个存储位置写入了数据，必须先执行擦除操作才能再次写入。更棘手的是，闪存写入的最小单位通常很小，只有几 KB 左右的“页”，但擦除的最小单位却非常大，往往达到数 MB 级别的“块”。如果文件系统像处理磁盘那样频繁地在同一个逻辑地址修改数据，底层的闪存就不得不不断执行繁重的“读-改-写”循环，这会导致整体写入性能随时间推移而急剧下降。

<!-- more -->

为了掩盖闪存这些古怪的物理特性，所有的闪存设备，无论是你手中的 SD 卡还是高性能的 SSD，内部都内置了一个微控制器来运行闪存转换层，也就是 FTL。FTL 的本质是一个欺骗机制，它让操作系统觉得这块闪存依然是一个可以随意覆盖写的普通磁盘。然而，当传统文件系统进行大量的随机写入时，FTL 必须在后台疯狂地进行“垃圾回收”和“磨损均衡”，试图在不断变化的物理空间中寻找空白区域来安置这些新数据。由于 FTL 并不了解文件系统的上层布局，它的这种盲目搬运效率极低。这就是为什么很多嵌入式设备或智能手机在使用一段时间后会变得卡顿，其深层原因往往是文件系统的随机写入行为让底层的 FTL 陷入了几乎崩溃的处理链条中。

F2FS 解决这一难题的核心武器是采用了**日志结构文件系统**，即 LFS 架构。我们可以做一个形象的比喻：传统文件系统就像一块黑板，哪里写错了就擦掉哪里，然后再原地重写；而 F2FS 则像是一个永远不使用橡皮擦的笔记本，无论你是新增文件还是修改旧文件，它永远不会去动之前写过的内容，而是不断在笔记本最后一页的空白处按顺序往后写。这种“追加式写入”的做法将所有的逻辑修改转换成了物理上的顺序写。对于底层闪存来说，顺序写入是最完美的运行模式，它极大地减轻了 FTL 的管理压力，让后台垃圾回收变得非常简单高效，同时也因为数据分布更加均匀而天然地实现了磨损均衡，从而显著延长了昂贵闪存硬件的寿命。

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

### Multi-head Logging（多头日志写入）特性

F2FS 的核心精髓在于它能感知数据的“温度”。Log 区域指的是文件系统中用于分配 free block(空闲的且没有写入数据的 block)的区域，例如 F2FS 的一个文件需要写入新数据，它就要去 Log 区域请求 free block，然后再将数据写入这个 free block 中。传统的 LFS 往往会维护一个大的日志区域，一切数据的分配都从这个大的日志区域中进行处理。它同时维护了 6 个活动的 Log 区域（活跃段），将数据精准分类：

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

## f2fs_super_block 结构

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

## f2fs_sb_info 结构

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

## f2fs_checkpoint 结构

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

# Segment Infomation Table 区域

Segment Infomation Table，简称 SIT，是 F2FS 用于集中管理 segment 状态的结构。它的主要作用是维护的 segment 的分配信息，它的作用可以使用两个常见例子进行描述:

- 用户进行写操作，那么 segment 会根据用户写入的数据量分配特定数目的 block 给用户进行数据写入，SIT 会将这些已经被分配的 block 标记为"已经使用(valid 状态)"，那么之后的写操作就不会再使用这些 block。
- 用户进行了**覆盖写**操作以后，由于 F2FS **异地更新**的特性，F2FS 会分配新 block 给用户写入，同时会将旧 block 置为"无效状态(invalid 状态)"，这样 gc 的时候可以根据 segment 无效的 block 的数目，采取某种策略进行回收。

综上所述，SIT 的作用是维护每一个 segment 的 block 的使用状态以及有效无效状态。

## 元数据的物理结构

SIT 区域的结构如下。SIT 区域由 N 个 `struct f2fs_sit_block` 组成，每一个 `struct f2fs_sit_block` 包含了 55 个 `struct f2fs_sit_entry`，每一个 entry 对应了一个 segment 的管理状态。每一个 entry 包含了三个变量: vblocks(记录这个 segment 有多少个 block 已经被使用了)，valid_map(记录这个 segment 里面的哪一些 block 是无效的)，mtime(表示修改时间)。

<img src="./F2FS 文件系统.assets/image-20251225152548700.png" alt="image-20251225152548700" style="zoom:67%;" />

SIT 的基本存放单元是 `struct f2fs_sit_block`，定义如下:

```c
struct f2fs_sit_block {
	struct f2fs_sit_entry entries[SIT_ENTRY_PER_BLOCK];
} __packed;
```

由于一个 block 的尺寸是 4 KB，因此跟根据 `sizeof(struct f2fs_sit_entry entries)` 的值，得到 `SIT_ENTRY_PER_BLOCK` 的值为 55。`struct f2fs_sit_entry entries `用来表示每一个 segment 的状态信息，定义如下：

```c
struct f2fs_sit_entry {
	__le16 vblocks;				/* reference above */
	__u8 valid_map[SIT_VBLOCK_MAP_SIZE];	/* bitmap for valid blocks */
	__le64 mtime;				/* segment age for cleaning */
} __packed;
```

第一个参数 `vblocks` 表示当前 segment 有多少个 block 已经被使用，第二个参数 `valid_map` 表示 segment 内的每一个 block 的有效无效信息; 由于一个 segment 包含了 512 个 block，因此需要用 512 个 bit 去表示每一个 block 的有效无效状态，因此 `SIT_VBLOCK_MAP_SIZE` 的值是 64(8*64=512)。最后一个参数 `mtime` 表示这个 entry 被修改的时间，用于挑选 GC 时需要使用的 segment。

## 内存管理结构

SIT 在内存中对应的管理结构是 `struct f2fs_sm_info`，它在 build_segment_manager() 函数进行初始化：

```c
struct f2fs_sm_info {
	struct sit_info *sit_info;		/* whole segment information */
	struct free_segmap_info *free_info;	/* free segment information */
	struct dirty_seglist_info *dirty_info;	/* dirty segment information */
	struct curseg_info *curseg_array;	/* active segment information */

	struct f2fs_rwsem curseg_lock;	/* for preventing curseg change */

	block_t seg0_blkaddr;		/* block address of 0'th segment */
	block_t main_blkaddr;		/* start block address of main area */
	block_t ssa_blkaddr;		/* start block address of SSA area */

	unsigned int segment_count;	/* total # of segments */
	unsigned int main_segments;	/* # of segments in main area */
	unsigned int reserved_segments;	/* # of reserved segments */
	unsigned int ovp_segments;	/* # of overprovision segments */

	/* a threshold to reclaim prefree segments */
	unsigned int rec_prefree_segments;

	struct list_head sit_entry_set;	/* sit entry set list */

	unsigned int ipu_policy;	/* in-place-update policy */
	unsigned int min_ipu_util;	/* in-place-update threshold */
	unsigned int min_fsync_blocks;	/* threshold for fsync */
	unsigned int min_seq_blocks;	/* threshold for sequential blocks */
	unsigned int min_hot_blocks;	/* threshold for hot block allocation */
	unsigned int min_ssr_sections;	/* threshold to trigger SSR allocation */

	/* for flush command control */
	struct flush_cmd_control *fcc_info;

	/* for discard command control */
	struct discard_cmd_control *dcc_info;
};

int build_segment_manager(struct f2fs_sb_info *sbi)
{
	struct f2fs_super_block *raw_super = F2FS_RAW_SUPER(sbi);
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	struct f2fs_sm_info *sm_info;
	int err;

    /* 分配空间 */
	sm_info = kzalloc(sizeof(struct f2fs_sm_info), GFP_KERNEL);

	/* 初始化一些地址信息，基础信息 */
	sbi->sm_info = sm_info;
	INIT_LIST_HEAD(&sm_info->wblist_head);
	spin_lock_init(&sm_info->wblist_lock);
	sm_info->seg0_blkaddr = le32_to_cpu(raw_super->segment0_blkaddr);
	sm_info->main_blkaddr = le32_to_cpu(raw_super->main_blkaddr);
	sm_info->segment_count = le32_to_cpu(raw_super->segment_count);
	sm_info->reserved_segments = le32_to_cpu(ckpt->rsvd_segment_count);
	sm_info->ovp_segments = le32_to_cpu(ckpt->overprov_segment_count);
	sm_info->main_segments = le32_to_cpu(raw_super->segment_count_main);
	sm_info->ssa_blkaddr = le32_to_cpu(raw_super->ssa_blkaddr);

    /* 初始化内存中的entry数据结构 */
	err = build_sit_info(sbi);
    
    /* 初始化可用segment的数据结构 */
	err = build_free_segmap(sbi);

    /* 恢复checkpoint active segment区域的信息，参考checkpoint结构那一节 */
	err = build_curseg(sbi);

	/* 从磁盘中将SIT物理区域记录的 物理区域sit_entry与只存在于内存的sit_entry建立联系 */
	build_sit_entries(sbi);

    /* 根据checkpoint记录的恢复信息，恢复可用segment的映射关系 */
	init_free_segmap(sbi);
    
    /* 恢复脏segment的映射关系 */
	err = build_dirty_segmap(sbi);

    /* 初始化最大最小的修改时间 */
	init_min_max_mtime(sbi);
	return 0;
}
```

build_sit_info() 用于初始化内存区域的 entry，这里需要注意的是注意区分内存 entry 以及物理区域的 entry：

```c
static int build_sit_info(struct f2fs_sb_info *sbi)
{
	struct f2fs_super_block *raw_super = F2FS_RAW_SUPER(sbi);
	struct sit_info *sit_i;
	unsigned int sit_segs, start;
	char *src_bitmap, *bitmap;
	unsigned int bitmap_size, main_bitmap_size, sit_bitmap_size;
	unsigned int discard_map = f2fs_block_unit_discard(sbi) ? 1 : 0;

	/* allocate memory for SIT information */
	sit_i = f2fs_kzalloc(sbi, sizeof(struct sit_info), GFP_KERNEL);
	if (!sit_i)
		return -ENOMEM;

	SM_I(sbi)->sit_info = sit_i;

	sit_i->sentries =
		f2fs_kvzalloc(sbi, array_size(sizeof(struct seg_entry),
					      MAIN_SEGS(sbi)),
			      GFP_KERNEL);
	if (!sit_i->sentries)
		return -ENOMEM;

	main_bitmap_size = f2fs_bitmap_size(MAIN_SEGS(sbi));
	sit_i->dirty_sentries_bitmap = f2fs_kvzalloc(sbi, main_bitmap_size,
								GFP_KERNEL);
	if (!sit_i->dirty_sentries_bitmap)
		return -ENOMEM;

#ifdef CONFIG_F2FS_CHECK_FS
	bitmap_size = MAIN_SEGS(sbi) * SIT_VBLOCK_MAP_SIZE * (3 + discard_map);
#else
	bitmap_size = MAIN_SEGS(sbi) * SIT_VBLOCK_MAP_SIZE * (2 + discard_map);
#endif
	sit_i->bitmap = f2fs_kvzalloc(sbi, bitmap_size, GFP_KERNEL);
	if (!sit_i->bitmap)
		return -ENOMEM;

	bitmap = sit_i->bitmap;

	for (start = 0; start < MAIN_SEGS(sbi); start++) {
		sit_i->sentries[start].cur_valid_map = bitmap;
		bitmap += SIT_VBLOCK_MAP_SIZE;

		sit_i->sentries[start].ckpt_valid_map = bitmap;
		bitmap += SIT_VBLOCK_MAP_SIZE;

#ifdef CONFIG_F2FS_CHECK_FS
		sit_i->sentries[start].cur_valid_map_mir = bitmap;
		bitmap += SIT_VBLOCK_MAP_SIZE;
#endif

		if (discard_map) {
			sit_i->sentries[start].discard_map = bitmap;
			bitmap += SIT_VBLOCK_MAP_SIZE;
		}
	}

	sit_i->tmp_map = f2fs_kzalloc(sbi, SIT_VBLOCK_MAP_SIZE, GFP_KERNEL);
	if (!sit_i->tmp_map)
		return -ENOMEM;

	if (__is_large_section(sbi)) {
		sit_i->sec_entries =
			f2fs_kvzalloc(sbi, array_size(sizeof(struct sec_entry),
						      MAIN_SECS(sbi)),
				      GFP_KERNEL);
		if (!sit_i->sec_entries)
			return -ENOMEM;
	}

	/* get information related with SIT */
	sit_segs = le32_to_cpu(raw_super->segment_count_sit) >> 1;

	/* setup SIT bitmap from ckeckpoint pack */
	sit_bitmap_size = __bitmap_size(sbi, SIT_BITMAP);
	src_bitmap = __bitmap_ptr(sbi, SIT_BITMAP);

	sit_i->sit_bitmap = kmemdup(src_bitmap, sit_bitmap_size, GFP_KERNEL);
	if (!sit_i->sit_bitmap)
		return -ENOMEM;

#ifdef CONFIG_F2FS_CHECK_FS
	sit_i->sit_bitmap_mir = kmemdup(src_bitmap,
					sit_bitmap_size, GFP_KERNEL);
	if (!sit_i->sit_bitmap_mir)
		return -ENOMEM;

	sit_i->invalid_segmap = f2fs_kvzalloc(sbi,
					main_bitmap_size, GFP_KERNEL);
	if (!sit_i->invalid_segmap)
		return -ENOMEM;
#endif

	sit_i->sit_base_addr = le32_to_cpu(raw_super->sit_blkaddr);
	sit_i->sit_blocks = SEGS_TO_BLKS(sbi, sit_segs);
	sit_i->written_valid_blocks = 0;
	sit_i->bitmap_size = sit_bitmap_size;
	sit_i->dirty_sentries = 0;
	sit_i->sents_per_block = SIT_ENTRY_PER_BLOCK;
	sit_i->elapsed_time = le64_to_cpu(sbi->ckpt->elapsed_time);
	sit_i->mounted_time = ktime_get_boottime_seconds();
	init_rwsem(&sit_i->sentry_lock);
	return 0;
}
```

build_free_segmap() 用于初始化 segment 的分配状态：

```c
static int build_free_segmap(struct f2fs_sb_info *sbi)
{
	struct free_segmap_info *free_i;
	unsigned int bitmap_size, sec_bitmap_size;

	/* allocate memory for free segmap information */
	free_i = f2fs_kzalloc(sbi, sizeof(struct free_segmap_info), GFP_KERNEL);
	if (!free_i)
		return -ENOMEM;

	SM_I(sbi)->free_info = free_i;

	bitmap_size = f2fs_bitmap_size(MAIN_SEGS(sbi));
	free_i->free_segmap = f2fs_kvmalloc(sbi, bitmap_size, GFP_KERNEL);
	if (!free_i->free_segmap)
		return -ENOMEM;

	sec_bitmap_size = f2fs_bitmap_size(MAIN_SECS(sbi));
	free_i->free_secmap = f2fs_kvmalloc(sbi, sec_bitmap_size, GFP_KERNEL);
	if (!free_i->free_secmap)
		return -ENOMEM;

	/* set all segments as dirty temporarily */
	memset(free_i->free_segmap, 0xff, bitmap_size);
	memset(free_i->free_secmap, 0xff, sec_bitmap_size);

	/* init free segmap information */
	free_i->start_segno = GET_SEGNO_FROM_SEG0(sbi, MAIN_BLKADDR(sbi));
	free_i->free_segments = 0;
	free_i->free_sections = 0;
	spin_lock_init(&free_i->segmap_lock);
	return 0;
}
```

build_sit_entries() 的作用是从 SIT 的物理区域存放的物理 entry 与内存的 entry 建立联系，首先看看物理 entry 和内存 entry 的差异在哪里。

```c
static int build_sit_entries(struct f2fs_sb_info *sbi)
{
	struct sit_info *sit_i = SIT_I(sbi);
	struct curseg_info *curseg = CURSEG_I(sbi, CURSEG_COLD_DATA);
	struct f2fs_journal *journal = curseg->journal;
	struct seg_entry *se;
	struct f2fs_sit_entry sit;
	int sit_blk_cnt = SIT_BLK_CNT(sbi);
	unsigned int i, start, end;
	unsigned int readed, start_blk = 0;
	int err = 0;
	block_t sit_valid_blocks[2] = {0, 0};

	do {
		readed = f2fs_ra_meta_pages(sbi, start_blk, BIO_MAX_VECS,
							META_SIT, true);

		start = start_blk * sit_i->sents_per_block;
		end = (start_blk + readed) * sit_i->sents_per_block;

		for (; start < end && start < MAIN_SEGS(sbi); start++) {
			struct f2fs_sit_block *sit_blk;
			struct folio *folio;

			se = &sit_i->sentries[start];
			folio = get_current_sit_folio(sbi, start);
			if (IS_ERR(folio))
				return PTR_ERR(folio);
			sit_blk = folio_address(folio);
			sit = sit_blk->entries[SIT_ENTRY_OFFSET(sit_i, start)];
			f2fs_folio_put(folio, true);

			err = check_block_count(sbi, start, &sit);
			if (err)
				return err;
			seg_info_from_raw_sit(se, &sit);

			if (se->type >= NR_PERSISTENT_LOG) {
				f2fs_err(sbi, "Invalid segment type: %u, segno: %u",
							se->type, start);
				f2fs_handle_error(sbi,
						ERROR_INCONSISTENT_SUM_TYPE);
				return -EFSCORRUPTED;
			}

			sit_valid_blocks[SE_PAGETYPE(se)] += se->valid_blocks;

			if (!f2fs_block_unit_discard(sbi))
				goto init_discard_map_done;

			/* build discard map only one time */
			if (is_set_ckpt_flags(sbi, CP_TRIMMED_FLAG)) {
				memset(se->discard_map, 0xff,
						SIT_VBLOCK_MAP_SIZE);
				goto init_discard_map_done;
			}
			memcpy(se->discard_map, se->cur_valid_map,
						SIT_VBLOCK_MAP_SIZE);
			sbi->discard_blks += BLKS_PER_SEG(sbi) -
						se->valid_blocks;
init_discard_map_done:
			if (__is_large_section(sbi))
				get_sec_entry(sbi, start)->valid_blocks +=
							se->valid_blocks;
		}
		start_blk += readed;
	} while (start_blk < sit_blk_cnt);

	down_read(&curseg->journal_rwsem);
	for (i = 0; i < sits_in_cursum(journal); i++) {
		unsigned int old_valid_blocks;

		start = le32_to_cpu(segno_in_journal(journal, i));
		if (start >= MAIN_SEGS(sbi)) {
			f2fs_err(sbi, "Wrong journal entry on segno %u",
				 start);
			err = -EFSCORRUPTED;
			f2fs_handle_error(sbi, ERROR_CORRUPTED_JOURNAL);
			break;
		}

		se = &sit_i->sentries[start];
		sit = sit_in_journal(journal, i);

		old_valid_blocks = se->valid_blocks;

		sit_valid_blocks[SE_PAGETYPE(se)] -= old_valid_blocks;

		err = check_block_count(sbi, start, &sit);
		if (err)
			break;
		seg_info_from_raw_sit(se, &sit);

		if (se->type >= NR_PERSISTENT_LOG) {
			f2fs_err(sbi, "Invalid segment type: %u, segno: %u",
							se->type, start);
			err = -EFSCORRUPTED;
			f2fs_handle_error(sbi, ERROR_INCONSISTENT_SUM_TYPE);
			break;
		}

		sit_valid_blocks[SE_PAGETYPE(se)] += se->valid_blocks;

		if (f2fs_block_unit_discard(sbi)) {
			if (is_set_ckpt_flags(sbi, CP_TRIMMED_FLAG)) {
				memset(se->discard_map, 0xff, SIT_VBLOCK_MAP_SIZE);
			} else {
				memcpy(se->discard_map, se->cur_valid_map,
							SIT_VBLOCK_MAP_SIZE);
				sbi->discard_blks += old_valid_blocks;
				sbi->discard_blks -= se->valid_blocks;
			}
		}

		if (__is_large_section(sbi)) {
			get_sec_entry(sbi, start)->valid_blocks +=
							se->valid_blocks;
			get_sec_entry(sbi, start)->valid_blocks -=
							old_valid_blocks;
		}
	}
	up_read(&curseg->journal_rwsem);

	/* update ckpt_valid_block */
	if (__is_large_section(sbi)) {
		unsigned int segno;

		for (segno = 0; segno < MAIN_SEGS(sbi); segno += SEGS_PER_SEC(sbi)) {
			set_ckpt_valid_blocks(sbi, segno);
			sanity_check_valid_blocks(sbi, segno);
		}
	}

	if (err)
		return err;

	if (sit_valid_blocks[NODE] != valid_node_count(sbi)) {
		f2fs_err(sbi, "SIT is corrupted node# %u vs %u",
			 sit_valid_blocks[NODE], valid_node_count(sbi));
		f2fs_handle_error(sbi, ERROR_INCONSISTENT_NODE_COUNT);
		return -EFSCORRUPTED;
	}

	if (sit_valid_blocks[DATA] + sit_valid_blocks[NODE] >
				valid_user_blocks(sbi)) {
		f2fs_err(sbi, "SIT is corrupted data# %u %u vs %u",
			 sit_valid_blocks[DATA], sit_valid_blocks[NODE],
			 valid_user_blocks(sbi));
		f2fs_handle_error(sbi, ERROR_INCONSISTENT_BLOCK_COUNT);
		return -EFSCORRUPTED;
	}

	return 0;
}

// 物理entry
struct f2fs_sit_entry {
	__le16 vblocks;				/* reference above */
	__u8 valid_map[SIT_VBLOCK_MAP_SIZE];	/* bitmap for valid blocks */
	__le64 mtime;				/* segment age for cleaning */
} __packed;

// 内存entry
struct seg_entry {
	unsigned short valid_blocks;	/* # of valid blocks */
	unsigned char *cur_valid_map;	/* validity bitmap of blocks */
	unsigned short ckpt_valid_blocks;
	unsigned char *ckpt_valid_map;
	unsigned char type;		/* segment type like CURSEG_XXX_TYPE */
	unsigned long long mtime;	/* modification time of the segment */
};
```

两者之间的差异主要是多了表示 segment 类型的 type 变量，以及多了两个与 checkpoint 相关的内容。

其实物理 entry 也包含了 segment type 的信息，但是为了节省空间，将 segment type 于 vblocks 存放在了一起，及 vblocks 的前 10 位表示数目，后 6 位表示 segment type，关系如下：

```c
#define SIT_VBLOCKS_SHIFT	10
#define SIT_VBLOCKS_MASK	((1 << SIT_VBLOCKS_SHIFT) - 1)
#define GET_SIT_VBLOCKS(raw_sit)				\
	(le16_to_cpu((raw_sit)->vblocks) & SIT_VBLOCKS_MASK)
#define GET_SIT_TYPE(raw_sit)					\
	((le16_to_cpu((raw_sit)->vblocks) & ~SIT_VBLOCKS_MASK)	\
	 >> SIT_VBLOCKS_SHIFT)
```

因此，内存 entry 实际上仅仅多了 2 个与 checkpoint 相关的信息，即 `ckpt_valid_blocks` 与 `ckpt_valid_map` 。在系统执行 checkpoint 的时候，会将 `valid_blocks` 以及 `cur_valid_map` 的值分别写入 `ckpt_valid_blocks` 与 `ckpt_valid_map`，当系统出现宕机的时候根据这个值恢复映射信息。

init_free_segmap() 从内存 entry 以及 checkpoint 中恢复 free segment 的信息。

```c
static void init_free_segmap(struct f2fs_sb_info *sbi)
{
	unsigned int start;
	int type;
	struct seg_entry *sentry;

	for (start = 0; start < MAIN_SEGS(sbi); start++) {
		if (f2fs_usable_blks_in_seg(sbi, start) == 0)
			continue;
		sentry = get_seg_entry(sbi, start);
		if (!sentry->valid_blocks)
			__set_free(sbi, start);
		else
			SIT_I(sbi)->written_valid_blocks +=
						sentry->valid_blocks;
	}

	/* set use the current segments */
	for (type = CURSEG_HOT_DATA; type <= CURSEG_COLD_NODE; type++) {
		struct curseg_info *curseg_t = CURSEG_I(sbi, type);

		__set_test_and_inuse(sbi, curseg_t->segno);
	}
}
```

init_dirty_segmap() 函数恢复脏 segment 的信息。

```c
static void init_dirty_segmap(struct f2fs_sb_info *sbi)
{
	struct dirty_seglist_info *dirty_i = DIRTY_I(sbi);
	struct free_segmap_info *free_i = FREE_I(sbi);
	unsigned int segno = 0, offset = 0, secno;
	block_t valid_blocks, usable_blks_in_seg;

	while (1) {
		/* find dirty segment based on free segmap */
		segno = find_next_inuse(free_i, MAIN_SEGS(sbi), offset);
		if (segno >= MAIN_SEGS(sbi))
			break;
		offset = segno + 1;
		valid_blocks = get_valid_blocks(sbi, segno, false);
		usable_blks_in_seg = f2fs_usable_blks_in_seg(sbi, segno);
		if (valid_blocks == usable_blks_in_seg || !valid_blocks)
			continue;
		if (valid_blocks > usable_blks_in_seg) {
			f2fs_bug_on(sbi, 1);
			continue;
		}
		mutex_lock(&dirty_i->seglist_lock);
		__locate_dirty_segment(sbi, segno, DIRTY);
		mutex_unlock(&dirty_i->seglist_lock);
	}

	if (!__is_large_section(sbi))
		return;

	mutex_lock(&dirty_i->seglist_lock);
	for (segno = 0; segno < MAIN_SEGS(sbi); segno += SEGS_PER_SEC(sbi)) {
		valid_blocks = get_valid_blocks(sbi, segno, true);
		secno = GET_SEC_FROM_SEG(sbi, segno);

		if (!valid_blocks || valid_blocks == CAP_BLKS_PER_SEC(sbi))
			continue;
		if (is_cursec(sbi, secno))
			continue;
		set_bit(secno, dirty_i->dirty_secmap);
	}
	mutex_unlock(&dirty_i->seglist_lock);
}
```

# Node Address Table 区域

Node Address Table，简称 NAT，是 F2FS 用于集中管理 node 的结构。**它的主要维护了一张表(如下图)，记录了每一个 node 在 flash 设备的物理地址。**F2FS 给每一个 node 分配了一个 node ID(nid)，系统可以根据 nid 从 NAT 查找到该 node 在 flash 设备上的物理地址，然后从 flash 设备读取出来。表的结构如下：

![image-20251225153846231](./F2FS 文件系统.assets/image-20251225153846231.png)

## 元数据的物理结构

NAT 区域由 N 个 `struct f2fs_nat_block` 组成，每一个 `struct f2fs_nat_block` 包含了 455 个 `struct f2fs_nat_entry`。每一个 nid 对应了一个 entry，每一个 entry 记录了这个 node 的在 flash 设备上的物理地址 block_addr。同时 entry 也记录了一个 ino 的值，这个值用于找到这个 node 的 parent node，如果 nid == ino 则表示这个 node 是 inode，如果 nid != ino，则表示这是一个 direct_node 或者 indrect_node。version 变量用于系统恢复。

<img src="./F2FS 文件系统.assets/image-20251225153933456.png" alt="image-20251225153933456" style="zoom:65%;" />

## 内存管理结构

NAT 在内存中对应的管理结构是 `struct f2fs_nm_info`，它在 build_node_manager() 函数进行初始化。`struct f2fs_nm_info` 不会将所有的 NAT 的数据都读取出来，而是读取 NAT 的一部分，然后构建 free nid 表，用于给新的 node 分配 nid。

```c
struct f2fs_nm_info {
	block_t nat_blkaddr;		/* base disk address of NAT */
	nid_t max_nid;			/* maximum possible node ids */
	nid_t available_nids;		/* # of available node ids */
	nid_t next_scan_nid;		/* the next nid to be scanned */
	nid_t max_rf_node_blocks;	/* max # of nodes for recovery */
	unsigned int ram_thresh;	/* control the memory footprint */
	unsigned int ra_nid_pages;	/* # of nid pages to be readaheaded */
	unsigned int dirty_nats_ratio;	/* control dirty nats ratio threshold */

	/* NAT cache management */
	struct radix_tree_root nat_root;/* root of the nat entry cache */
	struct radix_tree_root nat_set_root;/* root of the nat set cache */
	struct f2fs_rwsem nat_tree_lock;	/* protect nat entry tree */
	struct list_head nat_entries;	/* cached nat entry list (clean) */
	spinlock_t nat_list_lock;	/* protect clean nat entry list */
	unsigned int nat_cnt[MAX_NAT_STATE]; /* the # of cached nat entries */
	unsigned int nat_blocks;	/* # of nat blocks */

	/* free node ids management */
	struct radix_tree_root free_nid_root;/* root of the free_nid cache */
	struct list_head free_nid_list;		/* list for free nids excluding preallocated nids */
	unsigned int nid_cnt[MAX_NID_STATE];	/* the number of free node id */
	spinlock_t nid_list_lock;	/* protect nid lists ops */
	struct mutex build_lock;	/* lock for build free nids */
	unsigned char **free_nid_bitmap;
	unsigned char *nat_block_bitmap;
	unsigned short *free_nid_count;	/* free nid count of NAT block */

	/* for checkpoint */
	char *nat_bitmap;		/* NAT bitmap pointer */

	unsigned int nat_bits_blocks;	/* # of nat bits blocks */
	unsigned char *nat_bits;	/* NAT bits blocks */
	unsigned char *full_nat_bits;	/* full NAT pages */
	unsigned char *empty_nat_bits;	/* empty NAT pages */
#ifdef CONFIG_F2FS_CHECK_FS
	char *nat_bitmap_mir;		/* NAT bitmap mirror */
#endif
	int bitmap_size;		/* bitmap size */
};

int build_node_manager(struct f2fs_sb_info *sbi)
{
	int err;

    /* 分配空间 */
	sbi->nm_info = kzalloc(sizeof(struct f2fs_nm_info), GFP_KERNEL);
	if (!sbi->nm_info)
		return -ENOMEM;

    /* 初始化sbi->nm_info的信息 */
	err = init_node_manager(sbi);
	if (err)
		return err;

    /* 构建free nids表，用于给新的node分配nid */
	build_free_nids(sbi);
	return 0;
}
```

init_node_manager() 函数主要用于初始化 `sbi->nm_info` 内的变量信息。

```c
static int init_node_manager(struct f2fs_sb_info *sbi)
{
	struct f2fs_super_block *sb_raw = F2FS_RAW_SUPER(sbi);
	struct f2fs_nm_info *nm_i = NM_I(sbi);
	unsigned char *version_bitmap;
	unsigned int nat_segs;
	int err;

	nm_i->nat_blkaddr = le32_to_cpu(sb_raw->nat_blkaddr);

	/* segment_count_nat includes pair segment so divide to 2. */
	nat_segs = le32_to_cpu(sb_raw->segment_count_nat) >> 1;
	nm_i->nat_blocks = nat_segs << le32_to_cpu(sb_raw->log_blocks_per_seg);
	nm_i->max_nid = NAT_ENTRY_PER_BLOCK * nm_i->nat_blocks;

	/* not used nids: 0, node, meta, (and root counted as valid node) */
	nm_i->available_nids = nm_i->max_nid - sbi->total_valid_node_count -
						F2FS_RESERVED_NODE_NUM;
	nm_i->nid_cnt[FREE_NID] = 0;
	nm_i->nid_cnt[PREALLOC_NID] = 0;
	nm_i->ram_thresh = DEF_RAM_THRESHOLD;
	nm_i->ra_nid_pages = DEF_RA_NID_PAGES;
	nm_i->dirty_nats_ratio = DEF_DIRTY_NAT_RATIO_THRESHOLD;
	nm_i->max_rf_node_blocks = DEF_RF_NODE_BLOCKS;

	INIT_RADIX_TREE(&nm_i->free_nid_root, GFP_ATOMIC);
	INIT_LIST_HEAD(&nm_i->free_nid_list);
	INIT_RADIX_TREE(&nm_i->nat_root, GFP_NOIO);
	INIT_RADIX_TREE(&nm_i->nat_set_root, GFP_NOIO);
	INIT_LIST_HEAD(&nm_i->nat_entries);
	spin_lock_init(&nm_i->nat_list_lock);

	mutex_init(&nm_i->build_lock);
	spin_lock_init(&nm_i->nid_list_lock);
	init_f2fs_rwsem(&nm_i->nat_tree_lock);

	nm_i->next_scan_nid = le32_to_cpu(sbi->ckpt->next_free_nid);
	nm_i->bitmap_size = __bitmap_size(sbi, NAT_BITMAP);
	version_bitmap = __bitmap_ptr(sbi, NAT_BITMAP);
	nm_i->nat_bitmap = kmemdup(version_bitmap, nm_i->bitmap_size,
					GFP_KERNEL);
	if (!nm_i->nat_bitmap)
		return -ENOMEM;

	if (!test_opt(sbi, NAT_BITS))
		disable_nat_bits(sbi, true);

	err = __get_nat_bitmaps(sbi);
	if (err)
		return err;

#ifdef CONFIG_F2FS_CHECK_FS
	nm_i->nat_bitmap_mir = kmemdup(version_bitmap, nm_i->bitmap_size,
					GFP_KERNEL);
	if (!nm_i->nat_bitmap_mir)
		return -ENOMEM;
#endif

	return 0;
}
```

build_free_nids() 主要用于构建 free nid 表，用于给新的 node 分配 nid。 为了节省内存，F2FS 不会将 NAT 中所有的 free nid 读取出来，只会读取一部分，因此使用 `nm_i->fcnt` 表示缓存了多少个 free nid。然后会读取一定的数目的 `f2fs_nat_block` 出来，并遍历其中的每一个 `f2fs_nat_entry`，加入到 free nid 的管理结构中。最后还会搜索一下 log 区域的 free nid 信息，也加入到 free nid 管理结构中。

```c
void build_free_nids(struct f2fs_sb_info *sbi)
{
	struct f2fs_nm_info *nm_i = NM_I(sbi);
	struct curseg_info *curseg = CURSEG_I(sbi, CURSEG_HOT_DATA);
	struct f2fs_journal *journal = curseg->journal;
	int i = 0;
	nid_t nid = nm_i->next_scan_nid; // next_scan_nid的含义上面有介绍，从这里开始搜索free nid

	/* *
	 * 为了节省内存，F2FS不会将NAT中所有的free nid读取出来，只会读取一部分
	 * fcnt表示目前缓存了多少个free nid，如果大于NAT_ENTRY_PER_BLOCK，则不再缓存了
	 */
	if (nm_i->fcnt >= NAT_ENTRY_PER_BLOCK)
		return;

	/* 
	 * 因为准备开始读取NAT的page(block)，因此根据nid(next_scan_nid)的所在的block开始，
	 * 读取FREE_NID_PAGES(=8)个page进入内存 
	 */
	ra_meta_pages(sbi, NAT_BLOCK_OFFSET(nid), FREE_NID_PAGES,
							META_NAT, true);

	down_read(&nm_i->nat_tree_lock);

	while (1) {
		struct page *page = get_current_nat_page(sbi, nid);

        /* 
         * 读取当前nid所在的f2fs_nat_block(page)，
         * 然后将free nid加入到nm_i->free_nid_list/root中 
         */
		scan_nat_page(sbi, page, nid);
        
        /* 释放当前的f2fs_nat_block对应的page */
		f2fs_put_page(page, 1);

        /* 已经读取了一个f2fs_nat_block，自然要跳到下一个f2fs_nat_block的第一个nid */
		nid += (NAT_ENTRY_PER_BLOCK - (nid % NAT_ENTRY_PER_BLOCK));
		if (unlikely(nid >= nm_i->max_nid))
			nid = 0;

        /* 所有block读完之后就跳出循环 */
		if (++i >= FREE_NID_PAGES)
			break;
	}

	/* 更新next_scan_nid，前面的已经扫描过了，下一次从这个nid开始扫描 */
	nm_i->next_scan_nid = nid;

	/* 遍历log的nat_journal记录的nat_entry信息，从中寻找free nid */
	down_read(&curseg->journal_rwsem);
	for (i = 0; i < nats_in_cursum(journal); i++) {
		block_t addr;
		/* 从journal中获取nid信息 */
        nid = le32_to_cpu(nid_in_journal(journal, i));
        /* 从journal中获取该nid对应的物理地址 */
		addr = le32_to_cpu(nat_in_journal(journal, i).block_addr);
        
        /* addr==NULL_ADDR 表示这个nid没有被文件使用，因此加入free nid，否则去除free nid */
		if (addr == NULL_ADDR) 
			add_free_nid(sbi, nid, true);
		else
			remove_free_nid(nm_i, nid);
	}
	up_read(&curseg->journal_rwsem);
	up_read(&nm_i->nat_tree_lock);

	ra_meta_pages(sbi, NAT_BLOCK_OFFSET(nm_i->next_scan_nid),
					nm_i->ra_nid_pages, META_NAT, false);
}
```

scan_nat_page() 函数的作用是扫描当前的 `f2fs_nat_block` 的每一个 entry，并找到其中的 free nid，加入到 `nm_i` 的 free nid 管理结构中。

```c
static int scan_nat_page(struct f2fs_sb_info *sbi,
			struct f2fs_nat_block *nat_blk, nid_t start_nid)
{
	struct f2fs_nm_info *nm_i = NM_I(sbi);
	block_t blk_addr;
	unsigned int nat_ofs = NAT_BLOCK_OFFSET(start_nid);
	int i;

	__set_bit_le(nat_ofs, nm_i->nat_block_bitmap);

	i = start_nid % NAT_ENTRY_PER_BLOCK;

	for (; i < NAT_ENTRY_PER_BLOCK; i++, start_nid++) {
		if (unlikely(start_nid >= nm_i->max_nid))
			break;

		blk_addr = le32_to_cpu(nat_blk->entries[i].block_addr);

		if (blk_addr == NEW_ADDR)
			return -EFSCORRUPTED;

		if (blk_addr == NULL_ADDR) {
			add_free_nid(sbi, start_nid, true, true);
		} else {
			spin_lock(&NM_I(sbi)->nid_list_lock);
			update_free_nid_bitmap(sbi, start_nid, false, true);
			spin_unlock(&NM_I(sbi)->nid_list_lock);
		}
	}

	return 0;
}
```

# Segment Summary Area 区域

Segment Summary Area，简称 SSA，是 F2FS 用于集中管理物理地址到逻辑地址的映射关系的结构，同时它也具有通过 journal 缓存 sit 或者 nat 的操作用于数据恢复的作用。映射关系的主要作用是当给出一个物理地址的时候，可以通过 SSA 索引得到对应的逻辑地址，主要应用在 GC 流程中; SSA 所包含的 journal 可以缓存一些 sit 或者 nat 的操作，用于避免频繁的元数据更新，以及宕机时候的数据恢复。

## 元数据的物理结构

从结构图可以知道，SSA 区域由 N 个 `struct f2fs_summary_block` 组成，每一个 `struct f2fs_summary_block` 包含了 512 个 `struct f2fs_summary_entry`，刚好对应一个 segment。segment 里面的每一个 block(物理地址)对应一个的 `struct f2fs_summary_entry`，它记录了物理地址到逻辑地址的映射信息。它包含了三个变量: nid(该物理地址是属于哪一个 node 的)，version(用于数据恢复)，ofs_in_node(该物理地址属于该 nid 对应的 node 的第 ofs_in_node 个 block)。

`f2fs_journal` 属于 journal 的信息，它的作用是减少频繁地对 NAT 区域以及 SIT 区域的更新。例如，当系统写压力很大的时候，segment bitmap 更新就会很频繁，就会对 `struct f2fs_sit_entry` 结构进行频繁地改动。如果这个时候频繁将新的映射关系写入 SIT，就会加重写压力。此时可以将数据先写入到 journal 中，因此 **journal 的作用就是维护这些经常修改的数据，等待 CP 被触发的时候才写入磁盘，从而减少写压力**。也许这里会有疑问，为什么将 journal 放在 SSA 区域而不是 NAT 区域以及 SIT 区域呢？这是因为这种存放方式可以减少元数据区域空间的占用。

<img src="./F2FS 文件系统.assets/image-20251225154609242.png" alt="image-20251225154609242" style="zoom:67%;" />

SSA 的基本存放单元是 `struct f2fs_summary_block`，定义如下：

```c
/* Block-sized summary block structure */
struct f2fs_summary_block {
	struct f2fs_summary entries[ENTRIES_IN_SUM];
	struct f2fs_journal journal;
	struct summary_footer footer;
} __packed;
```

与 summary 直接相关的是 `struct f2fs_summary` 以及 `struct summary_footer`。`ENTRIES_IN_SUM` 的值 512，因此每一个 entry 对应一个 block，记录了从物理地址到逻辑地址的映射关系，entry 的结构如下：

```c
/* a summary entry for a block in a segment */
struct f2fs_summary {
	__le32 nid;		/* parent node id */
	union {
		__u8 reserved[3];
		struct {
			__u8 version;		/* node version number */
			__le16 ofs_in_node;	/* block index in parent node */
		} __packed;
	};
} __packed;

struct summary_footer {
	unsigned char entry_type;	/* SUM_TYPE_XXX */
	__le32 check_sum;		/* summary checksum */
} __packed;
```

`struct f2fs_summary` 用了一个 union 结构进行表示，但是核心信息是 `nid`、`version` 以及 `ofs_in_node`。数据的索引是通过 node 来进行。文件访问某一个页的数据时，需要首先根据页的索引，找到对应的 nid 以及 offset(两者构成逻辑地址)，从而根据 nid 得到 node page，再根据 offset 得到了该页的物理地址，然后从磁盘中读取出来。`f2fs_summary` 则是**记录物理地址到逻辑地址的映射**，即根据物理地址找到对应的 nid 以及 offset。例如，现在需要根据物理地址为 624 的 block，找到对应的 nid 以及 offset。那么物理地址为 624，可以得到该地址位于第二个 segment，然后属于第二个 segment 的第 113 个 block(block 的编址从 0 开始)。因此根据属于第二个 segment 的信息，找到第二个 `struct f2fs_summary_block`，然后根据偏移量为 113 的信息，找到对应的 `struct f2fs_summary` 结构，从而得到 `nid` 以及 `ofs_in_node`。

`struct summary_footer` 结构记录了校验信息，以及这个 summary 对应的 segment 是属于保存 data 数据的 segment 还是 node 数据的segment。

SSA 在内存没有单独的管理结构，summary 以及 journal 在内存中主要存在于 CURSEG 中。

# 文件数据组织方式

文件数据的组织方式一般时被设计为 inode-data 模式，即每一个文件都具有一个 inode，这个 inode 记录 data 的组织关系，这个关系称为**文件结构**。例如用户需要访问 A 文件的第 1000 个字节，系统就会先根据 A 文件的路径找到的 A 的 inode，然后从 inode 找到第 1000 个字节所在的物理地址，然后从磁盘读取出来。那么 F2FS 的文件结构是怎么样的呢？

<img src="./F2FS 文件系统.assets/image-20251225155522809.png" alt="image-20251225155522809" style="zoom:80%;" />

F2FS 中的一个 inode，包含两个主要部分: metadata 部分，和数据块寻址部分。我们重点观察数据块寻址部分，分析 inode 时如何将数据块索引出来。在图中，数据块寻址部分包含 direct pointers，single-indirect，double-indirect，以及 triple-indirect。它们的含义分别是：

- **direct pointer:** inode 内直接指向数据块(图右上角 Data)的地址数组，即 **inode->data模式**。
- **single-indirect pointer:** inode 记录了两个 single-indirect pointer(图右上角 Direct node)，每一个 single-indirect pointer 存储了多个数据块的地址，即 **inode->direct_node->data 模式**。
- **double-indirect:** inode 记录了两个 double-indirect pointer(图右上角 indirect node)，每一个 double-indirect pointer 记录了许多 single-indirect pointer，每一个 single-indirect pointer 指向了数据块，即 **inode->indirect_node->direct_node->data 模式**。
- **triple-indirect:** inode 记录了一个 triple-indirect pointer(图右上角 indirect node)，每一个 triple-indirect pointer 记录了许多 double-indirect pointer，每一个 double-indirect pointer 记录了许多 single-indirect pointer，最后每一个 single-indirect pointer 指向了数据块。即 **inode->indirect_node->indirect_node->direct_node->data 模式**。

可以发现，F2FS 的 inode 结构采取 indirect_node，首先在 inode 内部寻找物理地址，如果找不到再去 direct_node 找，层层深入。

## f2fs_node 结构及其作用

对于一个较大的文件，它可能包含 inode 以外的 node，去保存一些间接寻址的信息。single-indirect pointer 记录的是数据块的地址，而 double-indirect pointer 记录的是 single-indirect pointer 的地址，triple-indirect pointer 记录的 double-indirect pointer 地址。在F2FS中：

- inode 对应的是 `f2fs_inode` 结构，包含了多个 direct pointer 指向数据块物理地址；
- single-indirect pointer 对应的是 `direct_node` 结构，包含了多个 direct pointer 指向物理地址；
- double-indirect pointer 对应的是 `indirect_node` 结构，包含了多个指向 `direct_node` 的地址；
- triple-indirect pointer 对应的也是 `indirect_node` 结构，包含了多个指向 `indirect_node` 的地址。

### 基本 node 结构

为了方便 F2FS 的对 node 的区分和管理，`f2fs_inode` 和 `direct_node` 以及 `indirect_node` 都使用了同一个数据结构 `f2fs_node` 进行描述，并通过 union 的方式，将 `f2fs_node` 初始化成不同的 node 形式。定义如下：

```c
struct f2fs_node {
	/* can be one of three types: inode, direct, and indirect types */
	union {
		struct f2fs_inode i;
		struct direct_node dn;
		struct indirect_node in;
	};
	struct node_footer footer;
} __packed;

struct node_footer {
	__le32 nid;		/* node id */
	__le32 ino;		/* inode number */
	__le32 flag;		/* include cold/fsync/dentry marks and offset */
	__le64 cp_ver;		/* checkpoint version */
	__le32 next_blkaddr;	/* next node page block address */
} __packed;
```

其中起到区分是哪一种 node 的关键数据结构是 `node_footer`。如果 `node_footer` 的 `nid` 和 `ino` 相等，则表示这是一个 `f2fs_inode` 结构，如果不相等，则表示这是一个 `direct_node` 或者 `indirect_node`。

### f2fs_inode 结构

考虑 `f2fs_inode` 的结构，省略其他元数据的信息，重点关注文件如何索引的，结构如下：

```c
struct f2fs_inode {
	...
	__le32 i_addr[DEF_ADDRS_PER_INODE]; // DEF_ADDRS_PER_INODE=923
	__le32 i_nid[DEF_NIDS_PER_INODE];	// DEF_NIDS_PER_INODE=5
	...
} __packed;
```

`i_addr` 数组就是前面提及的 direct pointer，数组的下标是文件的逻辑位置，数组的值就是 flash 设备的物理地址。例如文件的第一个页就对应 `i_addr[0]`，第二个页就对应 `i_addr[1]`，而 `i_addr[0]` 和 `i_addr[1]` 所记录的物理地址，就是文件第一个页(page)和第二个页的数据的物理地址，系统可以将两个物理地址提交到 flash 设备，将数据读取出来。

我们可以发现 `i_addr` 的数组长度只有 923，即一个 `f2fs_inode` 只能直接索引到 923 个页/块的地址(约 3.6 MB)，对于大于 3.6 MB的文件，就需要使用**间接寻址**。`f2fs_inode` 的 `i_nid` 数组就是为了间接寻址而设计，`i_nid` 数组是一个长度为 5 的数组，可以记录 5 个 node 的地址。其中：

- `i_nid[0]` 和 `i_nid[1]` 记录的是 `direct_node` 的地址，即对应前述的 single-indirect pointer。
- `i_nid[2]` 和 `i_nid[3]` 记录的是 `indirect_node` 的地址，这两个 `indirect_node` 记录的是 `direct_node` 的地址，即对应前述的 double-indirect pointer。
- `i_nid[4]` 记录的是 `indirect_node` 的地址，但是这个 `indirect_node` 记录的是 `indirect_node` 的地址，即前述的 triple-indirect pointer。

### direct_node 和 indirect_node 结构

`direct_node` 记录的是数据块的地址，`indirect_inode` 记录的是 node 的 id，系统可以通过 nid 找到对应的 node 的地址。

```c
struct direct_node {
	__le32 addr[ADDRS_PER_BLOCK]; // ADDRS_PER_BLOCK=1018
} __packed;

struct indirect_node {
	__le32 nid[NIDS_PER_BLOCK]; // NIDS_PER_BLOCK=1018
} __packed;
```

### Wandering Tree 问题

F2FS 的设计是为了解决 wandering tree 的问题，那么现在的设计是如何解决这个问题的呢。假设一个文件发生更改，修改了 `direct_node` 里面的某一个 block 的数据，根据 LFS 的异地更新特性，我们需要给更改后的数据一个新的 block。传统的 LFS 需要将这个新的 block 的地址一层层网上传递，直到 inode 结构。**而 F2FS 的设计是只需要将 `direct_node` 对应位置的 `addr` 的值更新为新 block 的地址，从而没必要往上传递，因此解决了 wandering tree 的问题。**

## 普通文件数据的保存

一个文件由一个 `f2fs_inode` 和多个 `direct_inode` 或者 `indirect_inode` 所组成。当系统创建一个文件的时候，它会首先创建一个 `f2fs_inode` 写入到 flash 设备，然后用户往该文件写入第一个 page 的时候，会将数据写入到 main area 的一个 block 中，然后将该 block 的物理地址赋值到 `f2fs_inode->i_addr[0]` 中，这样就完成了 Node-Data 的管理关系。随着对同一文件写入的数据的增多，会逐渐使用到其他类型的 node 去保存文件的数据。

## 内联数据文件的保存

文件的实际数据是保存在 `f2fs_inode->i_addr` 对应的物理块当中，因此即使一个很小的文件，如 1 个字节的小文件，也需要一个 node 和 data block 才能实现正常的保存和读写，也就是需要 8 KB 的磁盘空间去保存一个尺寸为 1 字节的小文件。而且 `f2fs_inode->i_addr[923]` 里面除了 `f2fs_inode->i_addr[0]` 保存了一个物理地址，其余的 922 个 i_addr 都被闲置，造成了空间的浪费。

F2FS 为了减少空间的使用量，使用内联(inline)文件减少这些空间的浪费。它的核心思想是当文件足够小的时候，使用 `f2fs_inode->i_addr` 数组直接保存数据本身，而不单独写入一个 block 中，再进行寻址。因此，如上面的例子，只有 1 个字节大小的文件，只需要一个 `f2fs_inode` 结构，即 4 KB，就可以同时将 node 信息和 data 信息同时保存，减少了一半的空间使用量。

根据上述定义，可以计算得到多大的文件可以使用内联的方式进行保存，`f2fs_inode` 有尺寸为 923 的用于保存数据物理地址的数组 i_addr，它的数据类型是 __le32，即 4 个字节。保留一个数组成员另做它用，因此内联文件最大尺寸为: 922 * 4 = 3688 字节。

## 文件访问的实际例子

Linux 的文件是通过 page 进行组织起来的，默认 page 的 size 是 4 KB，使用 index 作为编号。

一个小文件访问例子：例如一个 size = 10 KB 的文件，需要 3 个 page 去保存数据，这 3 个 page 的编号是 0，1，2。当用户访问这个文件的第 2~6kb 的数据的时候，系统就会计算出数据保存在 page index = 0 和 1 的 page 中，然后根据文件的路径找到对应的 `f2fs_inode` 结构，page index = 0 和 1 即对应 `f2fs_inode` 的 `i_addr[0]` 和 `i_addr[1]`。系统进而从这两个 `i_addr` 读取物理地址，提交到 flash 设备将数据读取出来。

一个大文件访问例子：假设用户需要读取文件第 4000 个页(page index = 3999)的数据，第一步: 那么首先系统会根据文件路径找到对应的 f2fs_inode 结构。第二步：由于 4000 >（923 + 1018 + 1018），`f2fs_inode->i_addr `和 `f2fs_inode->nid[0]和nid[1]` 都无法满足需求，因此系统根据 `f2fs_inode->nid[2]` 找到对应的 `indirect_node` 的地址。第三步：`indirect_node` 保存的是 `direct_node` 的nid数组，由于 4000 - 923 - 1018 - 1018 = 1041，而一个 `direct_node` 只能保存 1018 个 block，因此可以知道数据位于 `indirect_node->nid[1]` 对应的 `direct_node` 中。第四步：计算剩下的的偏移(4000-923-1018-1018-1018=23)找到数据的物理地址位于该 `direct_node` 的 `direct_node->addr[23]` 中。

# 读流程

F2FS 的读流程包含了以下几个子流程:
1. vfs_read 函数。
2. generic_file_read_iter 函数: 根据访问类型执行不同的处理。
3. generic_file_buffered_read 函数: 根据用户传入的文件偏移，读取尺寸等信息，计算起始位置和页数，然后遍历每一个 page，通过预读或者单个读取的方式从磁盘中读取出来。
4. f2fs_read_data_page&f2fs_read_data_pages 函数: 从磁盘读取 1 个 page 或者多个 page。
5. f2fs_mpage_readpages 函数: f2fs 读取数据的主流程。

第一步的 vfs_read 函数是 VFS 层面的流程，下面仅针对涉及 F2FS 的读流程，且经过简化的主要流程进行分析。

## generic_file_read_iter 函数
这个函数的作用是处理普通方式访问以及 direct 方式访问的读行为，这里仅针对普通方式的读访问进行分析:
```c
ssize_t generic_file_read_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	size_t count = iov_iter_count(iter); // 获取需要读取的字节数
	ssize_t retval = 0;

	if (!count)
		goto out;

	if (iocb->ki_flags & IOCB_DIRECT) { // 处理direct方式的访问，这里不做介绍
		...
	}

	retval = generic_file_buffered_read(iocb, iter, retval); // 进行普通的读访问
out:
	return retval;
}
```

## generic_file_buffered_read 函数
在介绍这两个之前，需要先介绍一种 VFS 提高读取速度的机制: 预读(readahead)机制。它的核心原理是，当用户访问 page 1，系统就会将 page 1 后续的 page 2，page 3，page 4 一起读取到 page cache(减少与磁盘这种速度慢设备的交互次数，提高读性能)。之后用户再连续读取 page 2，page 3，page 4 时，由于已经读取到内存中，因此可以快速地返回给用户。

generic_file_buffered_read() 函数的主要作用是循环地从磁盘或者内存读取用户需要的 page，同时也会在某些情况调用 page_cache_sync_readahead() 函数进行预读，由于函数比较复杂，且很多 goto 语句，简化后的步骤如下：

**情况 1：预读(readahead)机制成功预读到用户需要接下来访问的 page**

1. ind_get_page: 系统无法在 cache 中找到用户需要的 page。
2. page_cache_sync_readahead: 系统执行该函数进行预读，一次性读取多个 page。
3. find_get_page: 再重新在 cache 获取一次 page，获取成功后跳转到 page ok 区域。
4. page_ok: 复制 page 的数据去用户传入的 buffer 中，然后判读是否为最后一个 page，如果是则退出读流程。

**情况 2：预读(readahead)机制错误预读到用户需要接下来访问的 page**

1. find_get_page: 系统无法在 cache 中找到用户需要的 page。
2. page_cache_sync_readahead: 系统执行该函数进行预读，一次性读取多个 page。
3. find_get_page: 再重新在 cache 获取一次 page，获取失败，跳转到 no_cached_page 区域。
4. no_cached_page: 创建一个 page cache 结构，加入到 LRU 后，跳转到 readpage 区域。
5. readpage: 执行 `mapping->a_ops->readpage` 函数从磁盘读取数据，成功后跳转到 page ok 区域。
6. page_ok: 复制 page 的数据去用户传入的 buffer 中，然后判读是否为最后一个 page，如果是则退出读流程。


```c
static ssize_t generic_file_buffered_read(struct kiocb *iocb,
		struct iov_iter *iter, ssize_t written)
{

	index = *ppos >> PAGE_SHIFT; // 文件指针偏移*ppos除以page的大小就是页偏移index
	prev_index = ra->prev_pos >> PAGE_SHIFT;
	prev_offset = ra->prev_pos & (PAGE_SIZE-1);
	last_index = (*ppos + iter->count + PAGE_SIZE-1) >> PAGE_SHIFT;
	offset = *ppos & ~PAGE_MASK;

	for (;;) {
find_page:
		page = find_get_page(mapping, index); // 根据页偏移index从cache获取page
		if (!page) { // 获取失败进行一次预读
			page_cache_sync_readahead(mapping, ra, filp,
					index, last_index - index);
			page = find_get_page(mapping, index); // 预读后再从cache获取page
			if (unlikely(page == NULL)) // 如果仍然失败则跳转到no_cached_page，成功则直接去page ok区域
				goto no_cached_page;
		}
page_ok: 
		// page数据读取成功后都进入这个区域，用于将数据复制到用户传入的buffer中
		isize = i_size_read(inode);
		end_index = (isize - 1) >> PAGE_SHIFT;

		nr = PAGE_SIZE;
		if (index == end_index) { // 如果到了最后一个index就退出循环
			nr = ((isize - 1) & ~PAGE_MASK) + 1;
			if (nr <= offset) {
				put_page(page);
				goto out;
			}
		}
		nr = nr - offset;
		ret = copy_page_to_iter(page, offset, nr, iter); // 复制用户数据到buffer中
		offset += ret;
		index += offset >> PAGE_SHIFT;
		offset &= ~PAGE_MASK;
		prev_offset = offset;

		put_page(page);
		written += ret;
		if (!iov_iter_count(iter))  // 如果将所有数据读取完毕后退出循环
			goto out;
		if (ret < nr) {
			error = -EFAULT;
			goto out;
		}
		continue;
readpage:
		ClearPageError(page);
		error = mapping->a_ops->readpage(filp, page); // 去磁盘进行读取
		goto page_ok;
no_cached_page:
		page = page_cache_alloc(mapping); // 创建page cache
		error = add_to_page_cache_lru(page, mapping, index,
				mapping_gfp_constraint(mapping, GFP_KERNEL)); // 加入lru
		goto readpage;
	}
out:
	ra->prev_pos = prev_index;
	ra->prev_pos <<= PAGE_SHIFT;
	ra->prev_pos |= prev_offset;

	*ppos = ((loff_t)index << PAGE_SHIFT) + offset;
	file_accessed(filp);
	return written ? written : error;
}
```
预读函数 page_cache_sync_readahead() 的分析由于篇幅有限无法全部展示，这里仅分析它的核心调用函数 __do_page_cache_readahead():
```c
unsigned int __do_page_cache_readahead(struct address_space *mapping,
		struct file *filp, pgoff_t offset, unsigned long nr_to_read,
		unsigned long lookahead_size)
{
	end_index = ((isize - 1) >> PAGE_SHIFT); // 得到文件的最后一个页的页偏移index

	for (page_idx = 0; page_idx < nr_to_read; page_idx++) { // nr_to_read是需要预读的page的数目
		pgoff_t page_offset = offset + page_idx; // offset表示从第几个page开始预读

		if (page_offset > end_index) // 预读超过了文件大小就退出
			break;
			
		page = __page_cache_alloc(gfp_mask); // 创建page cache
		page->index = page_offset; // 设置page index
		list_add(&page->lru, &page_pool); // 将所有预读的page加入到一个list中
		nr_pages++;
	}

	if (nr_pages)
		read_pages(mapping, filp, &page_pool, nr_pages, gfp_mask); // 执行预读
	BUG_ON(!list_empty(&page_pool));
out:
	return nr_pages;
}

static int read_pages(struct address_space *mapping, struct file *filp,
		struct list_head *pages, unsigned int nr_pages, gfp_t gfp)
{
	struct blk_plug plug;
	unsigned page_idx;
	int ret;

	blk_start_plug(&plug);

	if (mapping->a_ops->readpages) {
		ret = mapping->a_ops->readpages(filp, mapping, pages, nr_pages); // 执行readpages函数进行预读
		put_pages_list(pages);
		goto out;
	}
	ret = 0;

out:
	blk_finish_plug(&plug);

	return ret;
}
```

## f2fs_read_data_page & f2fs_read_data_pages 函数
当预读机制会调用 `mapping->a_ops->readpages` 函数一次性读取多个 page。而当预读失败时，也会调用 `mapping->a_ops->readpage` 读取单个 page。这两个函数在 f2fs 中对应的就是 `f2fs_read_page` 和 `f2fs_read_pages`，如下所示:
```c
static int f2fs_read_data_page(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	int ret = -EAGAIN;

	trace_f2fs_readpage(page, DATA);

	if (f2fs_has_inline_data(inode)) // inline文件使用特定的读取方法，这里暂不分析
		ret = f2fs_read_inline_data(inode, page);
	ret = f2fs_mpage_readpages(page->mapping, NULL, page, 1); // 读取1个page
	return ret;
}

static int f2fs_read_data_pages(struct file *file,
			struct address_space *mapping,
			struct list_head *pages, unsigned nr_pages)
{
	struct inode *inode = mapping->host;
	struct page *page = list_last_entry(pages, struct page, lru);

	trace_f2fs_readpages(inode, page, nr_pages);

	if (f2fs_has_inline_data(inode)) // inline文件是size小于1个page的文件，因此不需要进行预读，直接return 0
		return 0;

	return f2fs_mpage_readpages(mapping, pages, NULL, nr_pages); // 读取nr_pages个page
}
```

## f2fs_mpage_readpages函数
无论是 `f2fs_read_page` 函数还是 `f2fs_read_pages` 函数，都是调用 `f2fs_mpage_readpages` 函数进行读取，区别仅在于传入参数。`f2fs_mpage_readpages` 的定义为:
```c
static int f2fs_mpage_readpages(struct address_space *mapping,
			struct list_head *pages, struct page *page, unsigned nr_pages);
```
1. 第二个参数表示一个链表头，这个链表保存了多个 page，因此需要写入多个 page 的时候，就要传入一个 List。
2. 第三个参数表示单个 page，在写入单个 page 的时候，通过这个函数写入。
3. 第四个参数表示需要写入 page 的数目。

因此：

1. 在写入多个 page 的时候，需要设定第二个参数，和第四个参数，然后设定第三个参数为 NULL。
2. 在写入单个 page 的时候，需要设定第三个参数，和第四个参数，然后设定第二个参数为 NULL。

然后分析这个函数的执行流程:

1. 遍历传入的 page，得到每一个 page 的 index 以及 inode。
2. 将 page 的 inode 以及 index 传入 f2fs_map_blocks() 函数获取到该 page 的物理地址。
3. 将物理地址通过 submit_bio() 读取该 page 在磁盘中的数据。

```c
static int f2fs_mpage_readpages(struct address_space *mapping,
			struct list_head *pages, struct page *page,
			unsigned nr_pages)
{
	// 主流程第一步 初始化map结构，这个步骤非常重要，用于获取page在磁盘的物理地址
	struct f2fs_map_blocks map;
	map.m_pblk = 0;
	map.m_lblk = 0;
	map.m_len = 0;
	map.m_flags = 0;
	map.m_next_pgofs = NULL;

	// 主流程第二步 开始进行遍历，结束条件为 nr_pages 不为空
	for (page_idx = 0; nr_pages; page_idx++, nr_pages--) {

		// 循环第一步，如果是读取多个page，则pages不为空，从list里面读取每一次的page结构
		if (pages) {
			page = list_entry(pages->prev, struct page, lru);
			list_del(&page->lru);
			if (add_to_page_cache_lru(page, mapping,
						  page->index, GFP_KERNEL))
				goto next_page;
		}

		/**
 		 * map.m_lblk是上一个block_in_file
 		 * map.m_lblk + map.m_len是需要读取长度的最后一个blokaddr
 		 * 因此这里的意思是，如果是在这个 map.m_lblk < block_in_file < map.m_lblk + map.m_len 
 		 * 这个范围里面，不需要map，直接将上次的blkaddr+1就是需要的地址
 		 * 
		 */
		// 循环第二步，如果上一次找到了page，则跳到 got_it 通过bio获取page的具体数据
		if ((map.m_flags & F2FS_MAP_MAPPED) && block_in_file > map.m_lblk &&
			block_in_file < (map.m_lblk + map.m_len))
			goto got_it;
	
		// 循环第三步，使用page offset和length，通过f2fs_map_blocks获得物理地址
		map.m_flags = 0;
		if (block_in_file < last_block) {
			map.m_lblk = block_in_file; // 文件的第几个block
			map.m_len = last_block - block_in_file; // 读取的block的长度

			if (f2fs_map_blocks(inode, &map, 0,
						F2FS_GET_BLOCK_READ))
				goto set_error_page;
		}

got_it:
		// 循环第四步，通过map的结果执行不一样的处理方式
		if ((map.m_flags & F2FS_MAP_MAPPED)) { // 如果找到了地址，则计算block_nr得到磁盘的地址
			block_nr = map.m_pblk + block_in_file - map.m_lblk;
			SetPageMappedToDisk(page);

			if (!PageUptodate(page) && !cleancache_get_page(page)) {
				SetPageUptodate(page);
				goto confused;
			}
		} else { // 获取失败了，则跳过这个page
			zero_user_segment(page, 0, PAGE_SIZE);
			SetPageUptodate(page);
			unlock_page(page);
			goto next_page;
		}

		/**
		 * 这部分开始用于将物理地址通过submit_bio提交到磁盘读取数据
		 * 由于从磁盘读取数据是一个相对耗时的操作，
		 * 因此显然每读取一个页就访问一次磁盘一次的方式是低效的且影响读性能的，
		 * 所以F2FS会尽量一次性提交多个页到磁盘读取数据，以提高性能。
		 * 
		 * 这部分开始就是具体实现:
		 * 1. 创建一个bio(最大一次性提交256个页)
		 * 2. 将需要读取的页添加到这个bio中，
		 *     ------如果bio未满则将page添加到bio中
		 *     ------如果bio满了立即访问磁盘读取
		 *     ------如果循环结束以后，bio还是未满，则通过本函数末尾的操作提交未满的bio。
		 *     
         */

		// 循环第五步，判断bio装的page是否到了设定的最大数量，如果到了最大值则先发送到磁盘
		if (bio && (last_block_in_bio != block_nr - 1)) {
submit_and_realloc:
			submit_bio(READ, bio);
			bio = NULL;
		}

		// 循环第六步，如果bio是空，则创建一个bio，然后指定的f2fs_read_end_io进行读取
		if (bio == NULL) {
			struct fscrypt_ctx *ctx = NULL;

			if (f2fs_encrypted_inode(inode) &&
					S_ISREG(inode->i_mode)) {

				ctx = fscrypt_get_ctx(inode, GFP_NOFS);
				if (IS_ERR(ctx))
					goto set_error_page;

				/* wait the page to be moved by cleaning */
				f2fs_wait_on_encrypted_page_writeback(
						F2FS_I_SB(inode), block_nr);
			}

			bio = bio_alloc(GFP_KERNEL,
				min_t(int, nr_pages, BIO_MAX_PAGES)); // 创建bio
			if (!bio) {
				if (ctx)
					fscrypt_release_ctx(ctx);
				goto set_error_page;
			}
			bio->bi_bdev = bdev;
			bio->bi_iter.bi_sector = SECTOR_FROM_BLOCK(block_nr); // 设定bio的sector地址
			bio->bi_end_io = f2fs_read_end_io;
			bio->bi_private = ctx;
		}

		// 循环第七步，将page加入到bio中，等待第五步满了之后发送到磁盘
		if (bio_add_page(bio, page, blocksize, 0) < blocksize)
			goto submit_and_realloc;

set_error_page:
		SetPageError(page);
		zero_user_segment(page, 0, PAGE_SIZE);
		unlock_page(page);
		goto next_page;
confused: // 特殊情况进行submit bio
		if (bio) {
			submit_bio(READ, bio);
			bio = NULL;
		}
		unlock_page(page);
next_page:
		if (pages)
			put_page(page);
		
	}

	
	BUG_ON(pages && !list_empty(pages));

	// 如果还有bio没有处理，例如读取的页遍历完以后，还没有达到第五步要求的bio的最大保存页数，就会在这里提交bio到磁盘读取
	if (bio)
		submit_bio(READ, bio);
	return 0;
}
```

# 写流程

F2FS 的写流程主要包含了以下几个子流程:
1. 调用 vfs_write 函数。
2. 调用 f2fs_file_write_iter 函数: 初始化 f2fs_node 的信息。
3. 调用 f2fs_write_begin 函数: 创建 page cache，并填充数据。
4. 写入到 page cache: 等待系统触发 writeback 回写到磁盘。
5. 调用 f2fs_write_end 函数: 将 page 设置为最新状态。
6. 调用 f2fs_write_data_pages 函数: 系统 writeback 或者 fsync 触发的时候执行这个函数写入到磁盘。

第一步的 vfs_write 函数是 VFS 层面的流程，下面仅针对涉及 F2FS 的写流程，且经过简化的主要流程进行分析。

## f2fs_file_write_iter函数
这个函数的主要作用是在数据写入文件之前进行预处理，核心流程就是将该文件对应 `f2fs_inode` 或者 `direct_node` 对应写入位置的 `i_addr` 或者 `addr `的值进行初始化。例如用户需要在第 4 个 page 的位置写入数据，那么 `f2fs_file_write_iter` 函数会首先找到该文件对应的 `f2fs_inode`，然后找到第 4 个 page 对应的数据块地址记录，即 `f2fs_inode->i_addr[3]`。如果该位置的值是 `NULL_ADDR` 则表示当前是**添加写(Append Write)**，因此将值初始化为 `NEW_ADDR`。如果是该位置的值是一个具体的 block 号，那么表示为**覆盖写(Overwrite)**，不需要做处理。
```c
static ssize_t f2fs_file_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file_inode(file);
	ssize_t ret;

	...
	err = f2fs_preallocate_blocks(iocb, from); // 进行预处理
	...
	ret = __generic_file_write_iter(iocb, from); // 预处理完成后继续执行下一步写流程
	...

	return ret;
}
```
下面继续分析 f2fs_preallocate_blocks()。
```c
int f2fs_preallocate_blocks(struct kiocb *iocb, struct iov_iter *from)
{
	struct inode *inode = file_inode(iocb->ki_filp); // 获取inode
	struct f2fs_map_blocks map;

	map.m_lblk = F2FS_BLK_ALIGN(iocb->ki_pos); // 根据文件指针偏移计算需要从第几个block开始写入
	map.m_len = F2FS_BYTES_TO_BLK(iocb->ki_pos + iov_iter_count(from)); // 计算要写入block的个数

	// 初始化一些信息
	map.m_next_pgofs = NULL;
	map.m_next_extent = NULL;
	map.m_seg_type = NO_CHECK_TYPE;

	flag = F2FS_GET_BLOCK_PRE_AIO;

map_blocks:
	err = f2fs_map_blocks(inode, &map, 1, flag); // 进行初始化
	return err;
}
```
f2fs_map_blocks() 函数的作用非常广泛，主要作用是通过逻辑地址(文件偏移指针)找到对应的物理地址(block 号)。因此在读写流程中都有作用。在写流程中，该函数的主要作用是初始化地址信息。
```c
int f2fs_map_blocks(struct inode *inode, struct f2fs_map_blocks *map,
						int create, int flag)
{
	unsigned int maxblocks = map->m_len;

	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	int mode = create ? ALLOC_NODE : LOOKUP_NODE;

	map->m_len = 0;
	map->m_flags = 0;

	pgofs =	(pgoff_t)map->m_lblk; // 获得文件访问偏移量
	end = pgofs + maxblocks; // 获得需要读取的block的长度

next_dnode:

	set_new_dnode(&dn, inode, NULL, NULL, 0); // 初始化dnode，dnode的作用是根据逻辑地址找到物理地址
	
	// 根据inode找到对应的f2fs_inode或者direct_node结构，然后通过pgofs(文件页偏移)获得物理地址，记录在dn中
	err = f2fs_get_dnode_of_data(&dn, pgofs, mode); 

	start_pgofs = pgofs;
	prealloc = 0;
	last_ofs_in_node = ofs_in_node = dn.ofs_in_node;
	end_offset = ADDRS_PER_PAGE(dn.node_page, inode);

next_block:
	// 根据dn获得物理地址，ofs_in_node表示这个物理地址位于当前node的第几个数据块
	// 如 f2fs_inode->i_addr[3]，那么dn.ofs_in_node=3
	blkaddr = datablock_addr(dn.inode, dn.node_page, dn.ofs_in_node); 
	...
	if (!is_valid_blkaddr(blkaddr)) { // is_valid_blkaddr函数用于判断是否存在旧数据
		// 如果不存在旧数据
		if (create) {
			if (flag == F2FS_GET_BLOCK_PRE_AIO) {
				if (blkaddr == NULL_ADDR) {
					prealloc++; // 记录有多少个添加写的block
					last_ofs_in_node = dn.ofs_in_node;
				}
			}
			map->m_flags |= F2FS_MAP_NEW; // F2FS_MAP_NEW表示正在处理一个从未使用的数据
			blkaddr = dn.data_blkaddr; // 记录当前的物理地址
		}
	}
	...
	// 记录处理了多少个block
	dn.ofs_in_node++; 
	pgofs++;
	...
	// 这里表示已经处理到最后一个block了
	if (flag == F2FS_GET_BLOCK_PRE_AIO &&
			(pgofs == end || dn.ofs_in_node == end_offset)) {

		dn.ofs_in_node = ofs_in_node; // 回到第一个block
		err = f2fs_reserve_new_blocks(&dn, prealloc); // 通过这个函数将其地址设置为NEW_ADDR
		map->m_len += dn.ofs_in_node - ofs_in_node;
		dn.ofs_in_node = end_offset;
	}
	...
	if (pgofs >= end)
		goto sync_out; // 表示已经全部处理完，可以退出这个函数了
	else if (dn.ofs_in_node < end_offset)
		goto next_block; // 每执行上面的流程就处理一个block，如果没有处理所有用户写入的block，那么回去继续处理
	...
sync_out:
	...
out:
	return err;
}
```
然后分析 f2fs_reserve_new_blocks()。
```c
int f2fs_reserve_new_blocks(struct dnode_of_data *dn, blkcnt_t count)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(dn->inode);
	int err;

	...
	for (; count > 0; dn->ofs_in_node++) {
		block_t blkaddr = datablock_addr(dn->inode,
					dn->node_page, dn->ofs_in_node);
		if (blkaddr == NULL_ADDR) { // 首先判断是不是NULL_ADDR，如果是则初始化为NEW_ADDR
			dn->data_blkaddr = NEW_ADDR;
			__set_data_blkaddr(dn);
			count--;
		}
	}
	...

	return 0;
}
```
## f2fs_write_begin 和 f2fs_write_end 函数
VFS 中 `write_begin` 和 `write_end` 函数分别是数据写入 page cache 前以及写入后的处理。写入 page cache 后，系统会维护一段时间，直到满足一定条件后(如 fsync 和 writeback 会写)，VFS 会调用 writepages 函数，将这些缓存在内存中的 page 一次性写入到磁盘中。`write_begin` 和 `write_end` 函数的调用可以参考 VFS 的 `generic_perform_write` 函数。
```c
ssize_t generic_perform_write(struct file *file,
				struct iov_iter *i, loff_t pos)
{
	struct address_space *mapping = file->f_mapping;
	const struct address_space_operations *a_ops = mapping->a_ops;
	long status = 0;
	ssize_t written = 0;
	unsigned int flags = 0;

	do {
		struct page *page;
		unsigned long offset;
		unsigned long bytes;
		size_t copied;
		void *fsdata;

		offset = (pos & (PAGE_SIZE - 1)); // 计算文件偏移，按page计算
		bytes = min_t(unsigned long, PAGE_SIZE - offset, iov_iter_count(i)); // 计算需要写多少个字节
again:
		status = a_ops->write_begin(file, mapping, pos, bytes, flags, &page, &fsdata); // 调用write_begin，对page进行初始化

		copied = iov_iter_copy_from_user_atomic(page, i, offset, bytes); //  将处理后的数据拷贝到page当中
		flush_dcache_page(page); // 将包含用户数据的page加入到page cache中，等待系统触发writeback的时候回写

		status = a_ops->write_end(file, mapping, pos, bytes, copied, page, fsdata); // 调用write_end函数进行后续处理
		
		copied = status;

		iov_iter_advance(i, copied);

		pos += copied;
		written += copied;

		balance_dirty_pages_ratelimited(mapping);
	} while (iov_iter_count(i)); // 直到处理完所有的数据

	return written ? written : status;
}
```
然后分析 VFS 的 `write_begin` 和 `write_end` 对应的功能，`write_begin` 在 F2FS 中对应的是 `f2fs_write_begin`，它的作用是将根据用户需要写入的数据类型，对 page 进行初始化，如下所示：
```c
static int f2fs_write_begin(struct file *file, struct address_space *mapping,
		loff_t pos, unsigned len, unsigned flags,
		struct page **pagep, void **fsdata)
{
	struct inode *inode = mapping->host;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct page *page = NULL;
	pgoff_t index = ((unsigned long long) pos) >> PAGE_SHIFT;
	bool need_balance = false, drop_atomic = false;
	block_t blkaddr = NULL_ADDR;
	int err = 0;

repeat:
	page = f2fs_pagecache_get_page(mapping, index,
				FGP_LOCK | FGP_WRITE | FGP_CREAT, GFP_NOFS); // 第一步创建或者获取page cache

	*pagep = page;

	err = prepare_write_begin(sbi, page, pos, len,
					&blkaddr, &need_balance); // 第二步根据页偏移信息获取到对应的物理地址blkaddr

	// 第三步，根据写类型对新创建的page进行初始化处理
	if (blkaddr == NEW_ADDR) { //如果是添加写，则将该page直接使用0填充
		zero_user_segment(page, 0, PAGE_SIZE);
		SetPageUptodate(page);
	} else {  //如果是覆盖写，则将该page直接使用0填充
		err = f2fs_submit_page_read(inode, page, blkaddr); // 从磁盘中将旧数据读取出来

		lock_page(page);
		if (unlikely(page->mapping != mapping)) {
			f2fs_put_page(page, 1);
			goto repeat;
		}
		if (unlikely(!PageUptodate(page))) {
			err = -EIO;
			goto fail;
		}
	}
	return 0;
}
```
通过 flush_dcache_page() 函数将用户数据写入到 page cache 之后，进行 `write_end` 处理，在 F2FS 中它对应的是 f2fs_write_end() 函数，它的作用是，如下所述:
```c
static int f2fs_write_end(struct file *file,
			struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	struct inode *inode = page->mapping->host;

	if (!PageUptodate(page)) { // 判断是否已经将page cache在写入是否到达了最新的状态
		if (unlikely(copied != len))
			copied = 0;
		else
			SetPageUptodate(page); // 如果不是就处理后设置为最新
	}
	if (!copied)
		goto unlock_out;

	set_page_dirty(page); // 将page设置为dirty，就会加入到inode->mapping的radix tree中，等待系统回写

	if (pos + copied > i_size_read(inode))
		f2fs_i_size_write(inode, pos + copied); // 更新文件尺寸
unlock_out:
	f2fs_put_page(page, 1);
	f2fs_update_time(F2FS_I_SB(inode), REQ_TIME); // 更新文件修改日期
	return copied;
}
```

## f2fs_write_data_pages 函数
系统会将用户写入的数据先写入到 page cache，然后等待时机回写到磁盘中。page cache 的回写是通过 f2fs_write_data_pages() 函数进行。系统会将 page cache 中 dirty 的 pages 加入到一个 list 当中，然后传入到 f2fs_write_data_pages() 进行处理。它包含如下步骤:
1. f2fs_write_data_pages & __f2fs_write_data_pages 函数: 做一些不那么重要的预处理。
2. f2fs_write_cache_pages 函数: 从 inode->mapping 的 radix tree 中取出 page。
3. __write_data_page 函数: 判断文件类型(内联文件，目录文件，普通文件)进行不同的写入。
4. f2fs_do_write_data_page: 根据 F2FS 的状态选择进行就地回写(在原物理地址更新)还是异地回写(在其他物理地址更新)。
5. f2fs_outplace_write_data: 执行回写，更新 f2fs_inode 的状态。
6. do_write_page: 从 CURSEG 分配物理地址，然后写入到磁盘。

### f2fs_write_data_pages & __f2fs_write_data_pages 函数
这两个函数只是包含了一些不太重要的预处理。
```c
static int f2fs_write_data_pages(struct address_space *mapping,
			    struct writeback_control *wbc)
{
	struct inode *inode = mapping->host;

	return __f2fs_write_data_pages(mapping, wbc,
			F2FS_I(inode)->cp_task == current ?
			FS_CP_DATA_IO : FS_DATA_IO); // 这个函数可以知道当前是普通的写入，还是Checkpoint数据的写入
}

static int __f2fs_write_data_pages(struct address_space *mapping,
						struct writeback_control *wbc,
						enum iostat_type io_type)
{
	struct inode *inode = mapping->host;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct blk_plug plug;
	int ret;


	blk_start_plug(&plug);

	ret = f2fs_write_cache_pages(mapping, wbc, io_type); // 取出需要回写的page，然后写入

	blk_finish_plug(&plug);

	f2fs_remove_dirty_inode(inode); // 写入后将inode从dirty标志清除，即不需要再回写
	return ret;
skip_write:
	wbc->pages_skipped += get_dirty_pages(inode);
	trace_f2fs_writepages(mapping->host, wbc, DATA);
	return 0;
}
```

### f2fs_write_cache_pages 函数
这个函数的主要作用是从 inode 对应的 mapping(radix tree 的 root)中，取出所有需要回写的 page，然后通过一个循环，逐个写入到磁盘。
```c
static int f2fs_write_cache_pages(struct address_space *mapping,
					struct writeback_control *wbc,
					enum iostat_type io_type)
{
	struct pagevec pvec;
	
	pagevec_init(&pvec); // 这是一个用于装载page的数组，数组大小是15个page

	if (wbc->sync_mode == WB_SYNC_ALL || wbc->tagged_writepages)
		tag = PAGECACHE_TAG_TOWRITE; // tag是mapping给每一个pae的标志，用于标志这些page的属性
	else
		tag = PAGECACHE_TAG_DIRTY;
		
retry:
	if (wbc->sync_mode == WB_SYNC_ALL || wbc->tagged_writepages)
		tag_pages_for_writeback(mapping, index, end); // SYNC模式下，将所有的tag=PAGECACHE_TAG_DIRTY的page重新标志为PAGECACHE_TAG_TOWRITE，作用是SYNC模式下必须全部回写到磁盘
	done_index = index;
	
	while (!done && (index <= end)) {
		int i;
		
		// 从mapping中取出tag类型的15个page，装载到pvec中
		nr_pages = pagevec_lookup_range_tag(&pvec, mapping, &index, end, tag); 
		
		// 循环将pvec中的page取出，回写到磁盘
		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];
			bool submitted = false;
			
			ret = __write_data_page(page, &submitted, wbc, io_type); // 写入磁盘的核心函数

			if (--wbc->nr_to_write <= 0 &&
					wbc->sync_mode == WB_SYNC_NONE) {
				done = 1; // 如果本次writeback的所有page写完就退出
				break;
			}
		}
		pagevec_release(&pvec); // 释放掉pvec
		cond_resched();
	}

	if (wbc->range_cyclic || (range_whole && wbc->nr_to_write > 0))
		mapping->writeback_index = done_index;

	if (last_idx != ULONG_MAX)
		// page通过一些函数后，会放入到bio中，然后提交到磁盘。
		// f2fs的机制是不会马上提交bio，需要等到bio包含了一定数目的page之后才会提交
		// 因此这个函数作用是，即使数目不够，但是仍要强制提交bio，需要与磁盘同步
		f2fs_submit_merged_write_cond(F2FS_M_SB(mapping), mapping->host,
						0, last_idx, DATA);

	return ret;
}
```

### __write_data_page 函数
这个函数的作用是判断文件类型(目录文件，内联文件，普通文件)进行不同的写入。F2FS 针对普通文件，有两种保存方式，分别是内联方式(inline)和普通方式。这里主要介绍普通文件的写流程。
```c
static int __write_data_page(struct page *page, bool *submitted,
				struct writeback_control *wbc,
				enum iostat_type io_type)
{
	struct inode *inode = page->mapping->host;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	loff_t i_size = i_size_read(inode);
	const pgoff_t end_index = ((unsigned long long) i_size) >> PAGE_SHIFT;
	// 这个数据结构在整个写流程非常重要，记录了写入的信息
	// 关键变量是 fio->old_blkaddr 以及 fio->new_blkaddr记录旧地址和新地址
	struct f2fs_io_info fio = { 
		.sbi = sbi,
		.ino = inode->i_ino,
		.type = DATA,
		.op = REQ_OP_WRITE,
		.op_flags = wbc_to_write_flags(wbc),
		.old_blkaddr = NULL_ADDR,
		.page = page, // 即将写入的page
		.encrypted_page = NULL,
		.submitted = false,
		.need_lock = LOCK_RETRY,
		.io_type = io_type,
		.io_wbc = wbc,
	};

	if (page->index < end_index)
		goto write;
		
write:
	if (S_ISDIR(inode->i_mode)) { // 如果是目录文件，直接写入不需要修改
		err = f2fs_do_write_data_page(&fio);
		goto done;
	}

	err = -EAGAIN;
	if (f2fs_has_inline_data(inode)) { // 内联文件使用内联的写入方式
		err = f2fs_write_inline_data(inode, page);
		if (!err)
			goto out;
	}

	if (err == -EAGAIN) { // 普通文件则使用普通的方式
		err = f2fs_do_write_data_page(&fio);
	}

done:
	if (err && err != -ENOENT)
		goto redirty_out;

out:
	inode_dec_dirty_pages(inode); // 每写入一个page，就清除了inode一个dirty pages，因此数目减去1
	if (err)
		ClearPageUptodate(page);

	unlock_page(page);

	if (submitted)
		*submitted = fio.submitted;

	return 0;

redirty_out:
	redirty_page_for_writepage(wbc, page);
	if (!err || wbc->for_reclaim)
		return AOP_WRITEPAGE_ACTIVATE;
	unlock_page(page);
	return err;
}
```
### f2fs_do_write_data_page 函数
这个函数的作用是根据系统的状态选择就地更新数据(inplace update)还是异地更新数据(outplace update)。一般情况下，系统只会在磁盘空间比较满的时候选择就地更新策略，避免触发过多的 gc 影响性能。因此，这里主要介绍异地更新的写流程:
```c
int f2fs_do_write_data_page(struct f2fs_io_info *fio) // 前面提到fio是写流程最重要的数据结构
{
	struct page *page = fio->page;
	struct inode *inode = page->mapping->host;
	struct dnode_of_data dn;
	struct extent_info ei = {0,0,0};
	bool ipu_force = false;
	int err = 0;

	set_new_dnode(&dn, inode, NULL, NULL, 0); // 初始化dnode
	err = f2fs_get_dnode_of_data(&dn, page->index, LOOKUP_NODE); // 根据文件偏移page->index获取物理地址

	fio->old_blkaddr = dn.data_blkaddr; // 将旧的物理地址赋值给fio->old_blkaddr

	if (fio->old_blkaddr == NULL_ADDR) { // 前面提及到f2fs_file_write_iter已经将物理地址设置为NEW_ADDR或者具体的block号，因此这里表示在写入磁盘之前，用户又将这部分数据删除了，所以没必要写入了
		ClearPageUptodate(page);
		goto out_writepage;
	}
got_it:
	if (ipu_force || (is_valid_blkaddr(fio->old_blkaddr) &&
					need_inplace_update(fio))) { // 判断是否需要就地更新
		err = encrypt_one_page(fio);
		if (err)
			goto out_writepage;

		set_page_writeback(page);
		ClearPageError(page);
		f2fs_put_dnode(&dn);
		if (fio->need_lock == LOCK_REQ)
			f2fs_unlock_op(fio->sbi);
		err = f2fs_inplace_write_data(fio); // 使用就地更新的方式写入
		trace_f2fs_do_write_data_page(fio->page, IPU);
		set_inode_flag(inode, FI_UPDATE_WRITE);
		return err;
	}

	err = encrypt_one_page(fio); // 如果开启系统加密，会将这个fio->page先加密

	set_page_writeback(page);
	ClearPageError(page);

	f2fs_outplace_write_data(&dn, fio); // 执行异地更新函数

	set_inode_flag(inode, FI_APPEND_WRITE);
	if (page->index == 0)
		set_inode_flag(inode, FI_FIRST_BLOCK_WRITTEN);
out_writepage:
	f2fs_put_dnode(&dn);
out:
	if (fio->need_lock == LOCK_REQ)
		f2fs_unlock_op(fio->sbi);
	return err;
}
```

### f2fs_outplace_write_data 函数
这个函数主要用作异地更新，所谓异地更新即不在原先的物理地址更新数据，因此包含了如下四个步骤:
1. 分配一个新的物理地址。
2. 将数据写入新的物理地址。
3. 将旧的物理地址无效掉，然后等 GC 回收。
4. 更新逻辑地址和物理地址的映射关系。

本函数即完成以上四个步骤:
```c
void f2fs_outplace_write_data(struct dnode_of_data *dn,
					struct f2fs_io_info *fio)
{
	struct f2fs_sb_info *sbi = fio->sbi;
	struct f2fs_summary sum;
	struct node_info ni;

	f2fs_get_node_info(sbi, dn->nid, &ni);
	set_summary(&sum, dn->nid, dn->ofs_in_node, ni.version);
	
	do_write_page(&sum, fio); // 这里完成第1,2,3步骤
	f2fs_update_data_blkaddr(dn, fio->new_blkaddr); // 这里完成第四个步骤，重新建立映射
}
```
`struct dnode_of_data dn` 的作用是根据文件 inode，找到 `f2fs_inode` 或者 `direct_node`，然后再通过文件偏移得到物理地址，因此 f2fs_update_data_blkaddr() 也是通过 `dnode_of_data` 将新的物理地址更新到 `f2fs_inode` 或者 `direct_node` 对应的位置中。

```c
void f2fs_update_data_blkaddr(struct dnode_of_data *dn, block_t blkaddr)
{
	dn->data_blkaddr = blkaddr; // 获得新的物理地址
	f2fs_set_data_blkaddr(dn); // 更新地址到f2fs_inode或者direct_node
	f2fs_update_extent_cache(dn); // 更新cache
}

void f2fs_set_data_blkaddr(struct dnode_of_data *dn)
{
	f2fs_wait_on_page_writeback(dn->node_page, NODE, true); // 因为要更新node，所以要保证当前的node是最新状态
	__set_data_blkaddr(dn);
	if (set_page_dirty(dn->node_page)) // 设置dirty，因为更新后的地址要回写到磁盘记录
		dn->node_changed = true;
}

static void __set_data_blkaddr(struct dnode_of_data *dn)
{
	struct f2fs_node *rn = F2FS_NODE(dn->node_page); // 根据node page转换到对应的f2fs_node
	__le32 *addr_array;
	int base = 0;

	addr_array = blkaddr_in_node(rn); // 这个用于获得f2fs_inode->i_addr地址或者direct_node->addr地址
	addr_array[base + dn->ofs_in_node] = cpu_to_le32(dn->data_blkaddr); // 根据偏移赋值更新
}

static inline __le32 *blkaddr_in_node(struct f2fs_node *node)
{
	// RAW_IS_INODE判断当前node是属于f2fs_inode还是f2fs_node，然后返回物理地址数组指针
	return RAW_IS_INODE(node) ? node->i.i_addr : node->dn.addr;
}
```

### do_write_page 函数
上一节提及到异地更新的 1,2,3 步骤都是在这里完成，分别是 f2fs_allocate_data_block() 函数完成新物理地址的分配，以及旧物理地址的回收; f2fs_submit_page_write() 函数完成最后一步，将数据提交到磁盘。下面进行分析:

```c
static void do_write_page(struct f2fs_summary *sum, struct f2fs_io_info *fio)
{
	int type = __get_segment_type(fio); // 获取数据类型，这个类型指HOT/WARM/COLD X NODE/DATA的六种类型

	f2fs_allocate_data_block(fio->sbi, fio->page, fio->old_blkaddr,
			&fio->new_blkaddr, sum, type, fio, true); // 完成异地更新的1,2步

	f2fs_submit_page_write(fio); //完成异地更新的第3步

}
```
f2fs_allocate_data_block() 函数首先会根据 type 获得 CURSEG。然后在 CURSEG 分配一个新的物理块，然后将旧的物理块无效掉。
```c
void f2fs_allocate_data_block(struct f2fs_sb_info *sbi, struct page *page,
		block_t old_blkaddr, block_t *new_blkaddr,
		struct f2fs_summary *sum, int type,
		struct f2fs_io_info *fio, bool add_list)
{
	struct sit_info *sit_i = SIT_I(sbi);
	struct curseg_info *curseg = CURSEG_I(sbi, type);

	*new_blkaddr = NEXT_FREE_BLKADDR(sbi, curseg); // 获取新的物理地址

	__add_sum_entry(sbi, type, sum); // 将当前summary更新到CURSEG中

	__refresh_next_blkoff(sbi, curseg); // 更新下一次可以用的物理地址

	// 下面更新主要是更新SIT区域的segment信息
	
	// 根据new_blkaddr找到对应的sit_entry，然后更新状态为valid(值为1)，表示被用户使用，不可被其他人所使用
	update_sit_entry(sbi, *new_blkaddr, 1);
	
	// 根据old_blkaddr找到对应的sit_entry，然后更新状态为invalid(值为-1)，表示被覆盖了，等待GC回收后重新投入使用
	if (GET_SEGNO(sbi, old_blkaddr) != NULL_SEGNO)
		update_sit_entry(sbi, old_blkaddr, -1);

	// 如果当前segment没有空间进行下一次分配了，就分配一个新的segment给CURSEG
	if (!__has_curseg_space(sbi, type))
		sit_i->s_ops->allocate_segment(sbi, type, false);

	// 将segment设置为脏，等CP写回磁盘
	locate_dirty_segment(sbi, GET_SEGNO(sbi, old_blkaddr));
	locate_dirty_segment(sbi, GET_SEGNO(sbi, *new_blkaddr));

}
```
f2fs_submit_page_write() 完成最后的提交到磁盘的任务，先创建一个 bio，然后将 page 加入到 bio 中，如果 bio 满了就提交到磁盘。
```c
void f2fs_submit_page_write(struct f2fs_io_info *fio)
{
	struct f2fs_sb_info *sbi = fio->sbi;
	enum page_type btype = PAGE_TYPE_OF_BIO(fio->type);
	struct f2fs_bio_info *io = sbi->write_io[btype] + fio->temp; // 这个是F2FS用于临时存放bio的变量
	struct page *bio_page;

	down_write(&io->io_rwsem);
next:
	// 第一步根据是否有加密，将bio_page设置为对应的page
	if (fio->encrypted_page)
		bio_page = fio->encrypted_page;
	else
		bio_page = fio->page;

	fio->submitted = true;

alloc_new:
	// 如果bio是null，就创建一个新的bio
	if (io->bio == NULL) {
		io->bio = __bio_alloc(sbi, fio->new_blkaddr, fio->io_wbc,
						BIO_MAX_PAGES, false,
						fio->type, fio->temp); // BIO_MAX_PAGES一般等于256
		io->fio = *fio;
	}

	// 将page加入到bio中，如果  < PAGE_SIZE 表示bio已经满了，因此就先将这个bio提交，然后重新分配一个新的bio
	if (bio_add_page(io->bio, bio_page, PAGE_SIZE, 0) < PAGE_SIZE) {
		__submit_merged_bio(io); // 提交bio，最终会执行submit_bio函数
		goto alloc_new;
	}
out:
	up_write(&io->io_rwsem);
}
```
需要注意的是，在这个函数，当 bio 还没有填满 page 的时候是不会被提交到磁盘的，这是因为 F2FS 通过增大 bio 的 size 提高了写性能。因此，在用户 fsync 或者系统 writeback 的时候，为了保证这些 page 都可以刷写到磁盘，会如 f2fs_write_cache_pages() 函数所介绍一样，通过 f2fs_submit_merged_write_cond() 函数或者其他函数强行提交这个 page 未满的 bio。

# 文件创建流程

linux 的文件的创建可以抽象为两个流程。

1. 创建一个 inode，使得包含文件的元数据信息;
2. 将这个新创建的 inode 加入父目录的管理当中，可以理解建立父目录与这个新 inode 的关系。

到具体代码，上述两个抽象流程在 F2FS 中主要包含了以下几个子流程：

1. 调用 vfs_open 函数。
2. 调用 f2fs_create 函数: 创建文件 inode，并链接到父目录。
   - f2fs_new_inode 函数创建 inode。
   - f2fs_add_link 函数链接到父目录。

第一步的 vfs_open 函数是 VFS 层面的流程，下面仅针对涉及 F2FS 的文件创建流程，且经过简化的主要流程进行分析。

## inode 和 f2fs_inode_info

`inode` 结构是 linux 的 vfs 层最核心的结构之一，反应了文件的应该具有的基础信息，但是对于一些文件系统，原生的 `inode` 结构的信息并不够，还需要增加一些额外的变量去支持文件系统的某些功能，同时为了保证 vfs 层对所有文件系统的兼容性，我们直接修改 `inode` 结构不是一个明智的方法。针对这种场景，f2fs 使用了一种叫 `f2fs_inode_info` 的结构去扩展原有的 `inode` 的功能。

### 相互转换

从 `inode` 到 `f2fs_inode_info`:

```c
static inline struct f2fs_inode_info *F2FS_I(struct inode *inode)
{
	return container_of(inode, struct f2fs_inode_info, vfs_inode);
}
```

从 `f2fs_inode_info` 到 `inode`:

```c
// vfs的inode其实是f2fs_inode_info结构体的一个内部变量
struct f2fs_inode_info {
	struct inode vfs_inode;		/* serve a vfs inode */
	...
};

// 因此访问可以直接指向
struct f2fs_inode_info *fi = F2FS_I(inode);
fi->vfs_inode // 这里 fi->vfs_inode == inode
```

从上面代码我们可以看出，f2fs 中的 `inode` 是 `f2fs_inode_info` 当中的一个内部变量，因此可以用 container_of 这个函数直接获得，也可以通过指针获得。

### VFS inode 的创建和销毁

我们一般使用 VFS 提供的 `new_inode` 函数创建一个新 inode。这个 `new_inode` 函数内部会调用 new_inode_pseudo 函数，然后再调用 alloc_inode 函数，最后调用 `f2fs_alloc_inode` 函数，我们从这里开始分析:

如下代码，显然就是通过内存分配函数先创建一个 `f2fs_inode_info` 然后返回给上层：

```c
static struct inode *f2fs_alloc_inode(struct super_block *sb)
{
	struct f2fs_inode_info *fi;

	fi = kmem_cache_alloc(f2fs_inode_cachep, GFP_F2FS_ZERO); //简单直接创建f2fs_inode_info
	if (!fi)
		return NULL;

	init_once((void *) fi); // 这个函数初始化vfs inode部分的原始信息

    // 下面开始初始化f2fs_inode_info部分的原始信息
	atomic_set(&fi->dirty_pages, 0);
	init_rwsem(&fi->i_sem);
	...
	return &fi->vfs_inode; // 返回的vfs_inode给上层
}
```

当 vfs inode 的 link 是 0 的时候，它应当被销毁。由于 vfs inode 是 f2fs_inode_info 的内部变量：

```c
// 用户传入一个inode销毁
static void f2fs_destroy_inode(struct inode *inode)
{
	call_rcu(&inode->i_rcu, f2fs_i_callback);
}

```

同样简单直接，free 掉这块内存就行。

```c
static void f2fs_i_callback(struct rcu_head *head)
{
	struct inode *inode = container_of(head, struct inode, i_rcu);
	kmem_cache_free(f2fs_inode_cachep, F2FS_I(inode));
}
```

## f2fs_create 函数
这个函数的主要作用是创建 vfs_inode，并链接到对应的目录下，核心流程就是先创建该文件的基于 f2fs 的 inode 结构，以及对应的 f2fs 的 inode page，即 `f2fs_inode`。然后设置函数指针，最后将 f2fs 的 inode page 链接到对应的目录下。
```c
static int f2fs_create(struct inode *dir, struct dentry *dentry, umode_t mode,
						bool excl)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(dir);
	struct inode *inode;
	nid_t ino = 0;
	int err;

	inode = f2fs_new_inode(dir, mode); // 创建f2fs特定的inode结构

	inode->i_op = &f2fs_file_inode_operations; // 然后赋值对应的函数指针
	inode->i_fop = &f2fs_file_operations;
	inode->i_mapping->a_ops = &f2fs_dblock_aops;
	ino = inode->i_ino; // 记录该inode的ino

	err = f2fs_add_link(dentry, inode); // 将该inode链接到用户传入的父目录dir中
	if (err)
		goto out;

	f2fs_alloc_nid_done(sbi, ino); // 在f2fs_new_inode函数内分配了ino，在这里完成最后一步

	return 0;
}

```
### f2fs_new_inode 函数

下面继续分析 `f2fs_new_inode` 函数(只显示主干部分)，这个函数创建 inode 结构，**还没**创建对应的 f2fs inode page。

```c
static struct inode *f2fs_new_inode(struct inode *dir, umode_t mode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(dir);
	nid_t ino;
	struct inode *inode;
	bool nid_free = false;
	int xattr_size = 0;
	int err;

	inode = new_inode(dir->i_sb); // 先创建出来一个没有ino的inode结构，参考前面提及的创建流程

	if (!f2fs_alloc_nid(sbi, &ino)) { // 然后给这个inode分配一个nid，即ino
		goto fail;
	}
    
	nid_free = true;

	inode_init_owner(inode, dir, mode); // 初始化从属信息: 访问模式、父目录等

	inode->i_ino = ino; // 初始化一些元数据信息，例如ino
	inode->i_blocks = 0;
	inode->i_mtime = inode->i_atime = inode->i_ctime = current_time(inode);
	F2FS_I(inode)->i_crtime = inode->i_mtime;
	inode->i_generation = sbi->s_next_generation++;

	err = insert_inode_locked(inode); // 将这个inode插入到全局的inode table(VFS行为)

    set_inode_flag(inode, FI_NEW_INODE); // 注意这个标志位后面会用到
    
	......
	// 上面省略代码都在设置法f2fs_inode_info的flag，并在这个函数将部分flag设置到vfs inode中
	f2fs_set_inode_flags(inode); 
	return inode;
}
```
### f2fs_add_link 函数

经过上面的函数，我们已经创建了一个 f2fs 使用的 vfs inode，接下来我们要将这个 inode 链接到父目录的 inode 当中，建立联系，`f2fs_add_link` 函数直接会调用 `f2fs_do_add_link` 函数，因此我们直接分析这个函数。其中 `f2fs_dir_entry` 代表是目录项，可以理解为**父目录包含了多个子文件/目录项，每一个目录项对应一个子文件/子目录的关联信息。我们将新创建的 inode 加入到父目录的管理，也就是在父目录中为这个新 inode 下创建一个目录项。**

```c
static inline int f2fs_add_link(struct dentry *dentry, struct inode *inode)
{
    // 这里的dentry就是新inode的dentry
	return f2fs_do_add_link(d_inode(dentry->d_parent), &dentry->d_name,
				inode, inode->i_ino, inode->i_mode);
}

// dir是父目录
int f2fs_do_add_link(struct inode *dir, const struct qstr *name,
				struct inode *inode, nid_t ino, umode_t mode)
{
	struct f2fs_dir_entry *de = NULL; // 父目录dir的目录项，初始化为NULL
	int err;
    // 如果文件已经加密，则获得解密后的名字fname
	err = fscrypt_setup_filename(dir, name, 0, &fname); 
	if (de) { // 如果找到目录项
		f2fs_put_page(page, 0);
		err = -EEXIST;
	} else if (IS_ERR(page)) {
		err = PTR_ERR(page);
	} else { // 对于一个新inode，它对应的父目录的目录项f2fs_dir_entry应该是不存在的
		err = f2fs_add_dentry(dir, &fname, inode, ino, mode);
	}
	return err;
}
```

`f2fs_add_dentry` 函数提取了文件名字的字符串以及字符串长度：

```c
int f2fs_add_dentry(struct inode *dir, struct fscrypt_name *fname,
				struct inode *inode, nid_t ino, umode_t mode)
{
	struct qstr new_name;
	int err = -EAGAIN;

	new_name.name = fname_name(fname); // 将文件名的字符串格式保存在这里
	new_name.len = fname_len(fname);   // 将文件名的长度保存在这里

    // 在这个函数实现新inode和父inode的链接
	err = f2fs_add_regular_entry(dir, &new_name, fname->usr_fname,
						inode, ino, mode);

	f2fs_update_time(F2FS_I_SB(dir), REQ_TIME); // 更新修改时间
	return err;
}
```

新 inode 的 `f2fs_dir_entry` 应该是不存在的，注意 `FI_NEW_INODE` 的 flag。

```c
int f2fs_add_regular_entry(struct inode *dir, const struct qstr *new_name,
				const struct qstr *orig_name,
				struct inode *inode, nid_t ino, umode_t mode)
{
	...

	// 上面的机制比较复杂，在这里不提，在目录项的作用相关章节再提
    // 上面做了一大堆事情可以理解为，根据[文件名的长度]创建一个新的f2fs_dir_entry，然后加入到父目录当中
    // 需要注意的是这个f2fs_dir_entry还没有包含新inode的信息
       
    //  接下来就是要做的就是
    // 	1. 为新的vfs inode创建inode page，初始化与父目录有关的信息
    // 	2. 基于新inode的信息(名字，ino等)更新f2fs_dir_entry
        
	if (inode) {
        // 这个函数就是创建inode page，初始化与父目录有关的信息
		page = f2fs_init_inode_metadata(inode, dir, new_name,
						orig_name, NULL);
	}


    // 基于新inode的信息(名字，ino等)更新f2fs_dir_entry
	f2fs_update_dentry(ino, mode, &d, new_name, dentry_hash, bit_pos);

	set_page_dirty(dentry_page);
	f2fs_update_parent_metadata(dir, inode, current_depth); // 清除FI_NEW_INODE的flag
	return err;
}
```

由于新 inode 设置了 `FI_NEW_INODE`，因此 `f2fs_init_inode_metadata` 函数就是完成了两个功能:

1. 创建一个新的 inode page，然后初始化 acl、security 等信息。
2. 然后初始化新创建的 inode page 的名字。
3. 再增加 inode 的引入链接。

```c
struct page *f2fs_init_inode_metadata(struct inode *inode, struct inode *dir,
			const struct qstr *new_name, const struct qstr *orig_name,
			struct page *dpage)
{
	struct page *page;
	int err;

    // 由于新inode设置了FI_NEW_INODE
	if (is_inode_flag_set(inode, FI_NEW_INODE)) {
        // 创建一个新的inode page，然后初始化acl、security等信息。
		page = f2fs_new_inode_page(inode);

		err = f2fs_init_acl(inode, dir, page, dpage);
		if (err)
			goto put_error;

		err = f2fs_init_security(inode, dir, orig_name, page);
		if (err)
			goto put_error;
		}
	} else {
		page = f2fs_get_node_page(F2FS_I_SB(dir), inode->i_ino);
		if (IS_ERR(page))
			return page;
	}

	if (new_name) { // 然后初始化新创建的inode page的名字
		init_dent_inode(new_name, page);
		if (f2fs_encrypted_inode(dir))
			file_set_enc_name(inode);
	}
	// 再增加inode的引入链接。
	if (is_inode_flag_set(inode, FI_INC_LINK))
		f2fs_i_links_write(inode, true);
	return page;
}
```
将新的 inode 链接到父目录后，后续用户访问时，可以通过父目录找到新创建的文件的 inode，即完成了整个文件的创建流程。

# 重要数据结构和函数分析

TODO

# 参考文档

1. [RiweiPan/F2FS-NOTES: F2FS的学习笔记以及源码分析](https://github.com/RiweiPan/F2FS-NOTES)

