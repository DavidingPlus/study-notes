# Lab11：设备驱动

实验难度出自官网，实验简述和实验耗时是主观体验

* network  `简述`: 为网络接口卡（NIC）编写一个xv6设备驱动程序
* `难度`: 1 hard  `耗时`: 3h
* `建议`: 面试很少被问到，时间不够可以不做

<font style="color:rgb(35, 38, 59);">课程地址：</font><https://pdos.csail.mit.edu/6.S081/2020/schedule.html>\ <font style="color:rgb(35, 38, 59);">Lab 地址：</font><https://pdos.csail.mit.edu/6.S081/2020/labs/net.html>\ <font style="color:rgb(35, 38, 59);">相关翻译：</font><https://xv6.dgs.zone/labs/requirements/lab11.html>\ <font style="color:rgb(35, 38, 59);">参考博客：</font><https://blog.miigon.net/posts/s081-lab11-network/>

开始实验前，**<font style="background-color:#FBDE28;">需要切换到本次实验的分支：net</font>\*\*\*\*。**

## 实验

<font style="color:rgb(35, 38, 59);">给网络接口卡写驱动程序，使驱动程序可以发送和接收数据包。</font>

### <font style="color:rgb(0, 0, 0);">Your </font>Job (hard)

<font style="color:rgb(35, 38, 59);">实现</font>`e1000_transmit`<font style="color:rgb(35, 38, 59);">函数的功能，用于将以太网帧传输到网络接口卡（NIC）的发送描述符环（TX descriptor ring）中，通过E1000网卡发送数据。编写的代码需要实现将以太网帧填入发送描述符环中，并更新发送描述符环的指针。实现细节在注释中：</font>

```c
// kernel/e1000.c
int
e1000_transmit(struct mbuf *m)
{
  //
  // Your code here.
  //
  // the mbuf contains an ethernet frame; program it into
  // the TX descriptor ring so that the e1000 sends it. Stash
  // a pointer so that it can be freed after sending.
  //

    acquire(&e1000_lock);                               // 获取E1000锁，线程安全

    uint32 idx = regs[E1000_TDT];                       // 获取传输buf环的下一个可用索引
    struct tx_desc* desc = &tx_ring[idx];               // 获取当前传输buf的描述符指针

    if ((desc->status & E1000_TXD_STAT_DD) == 0) {      // E1000没有完成之前的传输请求，返回错误（此时已经用完了环形buf列表，缓冲区无空闲）
        release(&e1000_lock);
        return -1;
    }

    if (tx_mbufs[idx]) {                                // 该idx是否关联了一个mbuf
        mbuffree(tx_mbufs[idx]);                        // 表示之前该idx释放完毕了但未释放mbuf
        tx_mbufs[idx] = 0;
    }

    desc->addr = (uint64)m->head;                       // mbuf的地址填入发送描述符中
    desc->length = m->len;                              // mbuf的长度填入发送描述符中
    desc->cmd = E1000_TXD_CMD_EOP | E1000_TXD_CMD_RS;   // EOP表示以太网帧的结束，RS表示需要报告发送状态
    tx_mbufs[idx] = m;                                  // 将mbuf关联到发送描述符

    // 更新发送描述符环的尾指针
    regs[E1000_TDT] = (regs[E1000_TDT] + 1) % TX_RING_SIZE;

    release(&e1000_lock);
    return 0;

    return 0;
}
```

<font style="color:rgb(35, 38, 59);">实现</font>`e1000_recv`<font style="color:rgb(35, 38, 59);">函数的功能，处理从E1000网卡接收到的数据包。检查接收描述符环（RX descriptor ring）中的数据包，为每个数据包创建一个缓冲区（mbuf），将其传递给网络层进行处理。实现细节在注释中：</font>

```c
// kernel/e1000.c
static void
e1000_recv(void)
{
  //
  // Your code here.
  //
  // Check for packets that have arrived from the e1000
  // Create and deliver an mbuf for each packet (using net_rx()).
  //

    // 循环检查接收描述符环中的数据包
    while (1) {
        uint32 idx = (regs[E1000_RDT] + 1) % RX_RING_SIZE;      // 下一个接收描述符的索引

        struct rx_desc* desc = &rx_ring[idx];                   // 当前接收描述符的指针

        if ((desc->status & E1000_RXD_STAT_DD) == 0)            // 接收描述符环中已经没有数据包需要结束，退出函数
            return;

        rx_mbufs[idx]->len = desc->length;                      // 接受描述符中数据包的长度设置到mbuf的长度字段中

        net_rx(rx_mbufs[idx]);                                  // 将mbuf传递给网络层进行处理，网络层负责释放mbuf

        // 分配一个新的mbuf,将新的mbuf的地址设置为接收描述符的地址，状态清空，以便下一次使用该下标使用
        rx_mbufs[idx] = mbufalloc(0);
        desc->addr = (uint64)rx_mbufs[idx]->head;
        desc->status = 0;

        // 将接收描述符环的尾指针设置为当前索引
        regs[E1000_RDT] = idx;
    }
}
```

<font style="color:rgb(35, 38, 59);">此时可以验证实验是否通过。</font>

## 可扩展的实验

* 在这个实验中，网络栈使用中断处理接收的网络包，但不处理发送的网络包。更高级的策略是使用软件队列管理发送包，每次只提供有限数量的包给网络接口控制器（NIC）。然后依赖发送（TX）中断重新填充发送环（transmit ring）。通过这种技术，可以实现对不同类型的发送流量进行优先级管理。
* 提供的网络代码仅部分支持 ARP。实现一个完整的 ARP 缓存，并将其集成到 `net_tx_eth()` 中。
* E1000 网卡支持多个 RX 和 TX 环。配置 E1000 为每个CPU提供一对环（环对），并修改网络栈以支持多个环。这样做可以增加网络栈的吞吐量，并减少锁争用。
* `sockrecvudp()` 使用单链表来查找目标套接字，这种方式效率低下。尝试使用哈希表和 RCU（Read-Copy-Update）来提高性能。
* ICMP 可以提供网络流失败的通知。检测这些通知，并通过套接字系统调用接口将它们作为错误传播。
* E1000 支持几种无状态的硬件卸载功能，包括校验和计算、RSC 和 GRO。使用一个或多个这些卸载功能来提高网络栈的吞吐量。
* 实验中的网络栈容易受到 receive livelock 的影响。使用课程讲座和阅读材料中的内容，设计并实现解决方案以修复此问题。
* 为 xv6 实现一个 UDP 服务器。
* 实现一个最小化的 TCP 栈并下载一个网页。


> 更新: 2024-12-08 21:08:31  
> 原文: <https://www.yuque.com/chengxuyuancarl/gxfm6r/xqcd70111zzbct0s>