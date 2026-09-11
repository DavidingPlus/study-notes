# part1 负载均衡

<font style="color:rgb(55, 65, 81);">在之前的版本中，当客户端请求服务时，取 从注册中心返回的服务地址列表中的第一个 作为访问的地址</font>

<font style="color:rgb(55, 65, 81);">这样做是明显存在问题的，当请求量过大时，单节点会无法承载庞大的流量从而崩溃。</font>

<font style="color:rgb(55, 65, 81);">所以在一些流量较大的服务上，我们会设置多个节点服务器处理请求，并使用负载均衡的思想 将请求分摊到每个节点上</font>

![1732594016222-7b2ac45c-fd6b-4888-a973-07604b3d1788.png](https://cdn.davidingplus.cn/images/2026/09/11/1732594016222-7b2ac45c-fd6b-4888-a973-07604b3d1788-968014.png)

# 常见的负载均衡算法

<font style="color:rgb(55, 65, 81);">1.轮询法（Round Robin）</font>

* **原理**<font style="color:rgb(55, 65, 81);">：轮询法将所有请求按顺序轮流分配给后端服务器，依次循环。</font>
* **优点**
  * <font style="color:rgb(55, 65, 81);">简单易实现。</font>
  * <font style="color:rgb(55, 65, 81);">无状态，不保存任何信息，因此实现成本低。</font>
* **缺点**
  * <font style="color:rgb(55, 65, 81);">当后端服务器性能差异大时，无法根据服务器的负载情况进行动态调整，可能导致某些服务器负载过大或过小。</font>
  * <font style="color:rgb(55, 65, 81);">如果服务器配置不一样，不适合使用轮询法。</font>

<font style="color:rgb(55, 65, 81);">2.随机法（Random）</font>

* **原理**<font style="color:rgb(55, 65, 81);">：随机法将请求随机分配到各个服务器。</font>
* **优点**
  * <font style="color:rgb(55, 65, 81);">分配较为均匀，避免了轮询法可能出现的连续请求分配给同一台服务器的问题。</font>
  * <font style="color:rgb(55, 65, 81);">使用简单，不需要复杂的配置。</font>
* **缺点**
  * <font style="color:rgb(55, 65, 81);">随机性可能导致某些服务器被频繁访问，而另一些服务器则相对较少，这取决于随机数的生成情况。</font>
  * <font style="color:rgb(55, 65, 81);">如果服务器配置不同，随机法可能导致负载不均衡，影响整体性能。</font>

<font style="color:rgb(55, 65, 81);">3.一致性哈希法（Consistent Hashing）</font>

* **原理**<font style="color:rgb(55, 65, 81);">：一致性哈希法将输入（如客户端IP地址）通过哈希函数映射到一个固定大小的环形空间（哈希环）上，每个服务器也映射到这个哈希环上。客户端的请求会根据哈希值在哈希环上顺时针查找，遇到的第一个服务器就是该请求的目标服务器。</font>
* **优点**
  * <font style="color:rgb(55, 65, 81);">当服务器数量发生变化时，只有少数键需要被重新映射到新的服务器上，这大大减少了缓存失效的数量，提高了系统的可用性。</font>
  * <font style="color:rgb(55, 65, 81);">具有良好的可扩展性，可以动态地添加或删除服务器。</font>
* **缺点**
  * <font style="color:rgb(55, 65, 81);">在哈希环偏斜的情况下，大部分的缓存对象很有可能会缓存到一台服务器上，导致缓存分布极度不均匀。</font>
  * <font style="color:rgb(55, 65, 81);">实现较为复杂，需要引入虚拟节点等技术来解决哈希偏斜问题。</font>

# 整体顺序

在 serviceCenter 下 创建 balance 包->在 balance 下创建 LoadBalance 接口->创建 RoundLoadBalance 类->创建 RandomLoadBalance 类->创建 ConsistencyHashBalance 类->更改<font style="color:rgb(55, 65, 81);">ServiceCenter</font>

# <font style="color:rgb(55, 65, 81);">LoadBalance</font>

实现负载均衡算法的接口

```java
public interface LoadBalance {
    //负责实现具体算法，返回分配的地址
    String balance(List<String> addressList);
    //添加节点
    void addNode(String node);
    //删除节点
    void delNode(String node);
}
```

# RoundLoadBalance

轮询 负载均衡

* `choose` 用于记录当前选择的服务节点的索引。初始化为 -1，表示还没有选择过任何节点。
* `choose++`: 每次调用时，`choose` 自增，表示选择下一个服务节点。
* `choose = choose % addressList.size()`: 对 `choose` 进行取余操作，确保索引在 `addressList` 的有效范围内。如果 `choose` 达到 `addressList.size()`，则会回到列表的第一个地址，形成循环。
* `System.out.println("负载均衡选择了" + "服务器");`：当前代码只输出了一个固定的文本 `服务器`，而没有输出实际选择的服务节点。应该改为 `addressList.get(choose)`，这样可以显示实际选择的服务器。
* `return addressList.get(choose);`：返回当前选择的服务地址。

```java
public class RoundLoadBalance implements LoadBalance {
    private int choose = -1;

    @Override
    public String balance(List<String> addressList) {
        choose++;
        choose = choose % addressList.size();
        System.out.println("负载均衡选择了" + "服务器");
        return addressList.get(choose);
    }

    @Override
    public void addNode(String node) {

    }

    @Override
    public void delNode(String node) {

    }
}
```

# RandomLoadBalance

随机 负载均衡

* `Random random = new Random();`：创建一个 `Random` 对象，用于生成随机数。
* `int choose = random.nextInt(addressList.size());`：生成一个随机整数，范围是 `0` 到 `addressList.size() - 1`，用于选择列表中的一个服务地址。
* `System.out.println("负载均衡选择了" + choose + "服务器");`：输出一个调试信息，打印出所选服务器的索引 `choose`。**问题**：目前输出的是索引 `choose`，而不是实际选择的服务器地址。应该输出 `addressList.get(choose)`，这样能看到所选的服务器地址。
* `return null;`：方法的返回值应该是选中的服务地址 `addressList.get(choose)`，但当前代码返回的是 `null`，这是一个逻辑错误。

```java
public class RandomLoadBalance implements LoadBalance {
    @Override
    public String balance(List<String> addressList) {
        Random random=new Random();
        int choose = random.nextInt(addressList.size());
        System.out.println("负载均衡选择了"+choose+"服务器");
        return null;
    }
    public void addNode(String node){} ;
    public void delNode(String node){};
}
```

# ConsistencyHashBalance

```java
public class ConsistencyHashBalance implements LoadBalance {
    // 虚拟节点个数
    private static final int VIRTUAL_NUM = 5;
    // 保存虚拟节点的hash值和对应的虚拟节点,key为hash值，value为虚拟节点的名称
    private SortedMap<Integer, String> shards = new TreeMap<Integer, String>();
    // 真实节点列表
    private List<String> realNodes = new LinkedList<String>();
    //模拟初始服务器
    private String[] servers = null;
    //该方法初始化负载均衡器，将真实的服务节点和对应的虚拟节点添加到哈希环中。
    private void init(List<String> serviceList) {
        for (String server : serviceList) {
            realNodes.add(server);
            System.out.println("真实节点[" + server + "] 被添加");
            //遍历 serviceList（真实节点列表），每个真实节点都会生成 VIRTUAL_NUM 个虚拟节点，并计算它们的哈希值
            for (int i = 0; i < VIRTUAL_NUM; i++) {
                //虚拟节点的命名规则是 server&&VN<i>，其中 <i> 是虚拟节点的编号。
                String virtualNode = server + "&&VN" + i;
                //使用 getHash 方法计算每个虚拟节点的哈希值，并将其加入 shards 中。
                int hash = getHash(virtualNode);
                //shards 是一个 SortedMap，会根据哈希值对虚拟节点进行排序。
                shards.put(hash, virtualNode);
                System.out.println("虚拟节点[" + virtualNode + "] hash:" + hash + "，被添加");
            }
        }
    }
    //根据请求的 node（比如某个请求的标识符），选择一个服务器节点。
    public String getServer(String node, List<String> serviceList) {
        //首先调用 init(serviceList) 初始化哈希环（即真实节点和虚拟节点）
        init(serviceList);
        //通过 getHash(node) 计算请求的哈希值。
        int hash = getHash(node);
        Integer key = null;
        //使用 shards.tailMap(hash) 获取 hash 值大于等于请求哈希值的所有虚拟节点。
        SortedMap<Integer, String> subMap = shards.tailMap(hash);
        //如果没有找到，意味着请求的哈希值大于所有虚拟节点的哈希值，选择哈希值最大的虚拟节点。
        //否则，选择 tailMap 中第一个虚拟节点。
        if (subMap.isEmpty()) {
            key = shards.lastKey();
        } else {
            key = subMap.firstKey();
        }
        //返回真实节点：从选中的虚拟节点 virtualNode 中提取出真实节点的名称（即虚拟节点名称去掉 &&VN<i> 部分）。
        String virtualNode = shards.get(key);
        return virtualNode.substring(0, virtualNode.indexOf("&&"));
    }
    //添加一个新的真实节点及其虚拟节点到哈希环中。
    public void addNode(String node) {
        if (!realNodes.contains(node)) {
            realNodes.add(node);
            System.out.println("真实节点[" + node + "] 上线添加");
            for (int i = 0; i < VIRTUAL_NUM; i++) {
                String virtualNode = node + "&&VN" + i;
                int hash = getHash(virtualNode);
                shards.put(hash, virtualNode);
                System.out.println("虚拟节点[" + virtualNode + "] hash:" + hash + "，被添加");
            }
        }
    }
    //删除一个真实节点及其对应的虚拟节点。
    public void delNode(String node) {
        if (realNodes.contains(node)) {
            realNodes.remove(node);
            System.out.println("真实节点[" + node + "] 下线移除");
            for (int i = 0; i < VIRTUAL_NUM; i++) {
                String virtualNode = node + "&&VN" + i;
                int hash = getHash(virtualNode);
                shards.remove(hash);
                System.out.println("虚拟节点[" + virtualNode + "] hash:" + hash + "，被移除");
            }
        }
    }
    // FNV1_32_HASH算法
    //该方法用于计算字符串的哈希值，哈希算法基于一个常见的算法，使用了 FNV-1a 哈希和一些额外的位运算，确保哈希值均匀分布
    private static int getHash(String str) {
        final int p = 16777619;
        int hash = (int) 2166136261L;
        for (int i = 0; i < str.length(); i++)
            hash = (hash ^ str.charAt(i)) * p;
        hash += hash << 13;
        hash ^= hash >> 7;
        hash += hash << 3;
        hash ^= hash >> 17;
        hash += hash << 5;
        // 如果算出来的值为负数则取其绝对值
        if (hash < 0)
            hash = Math.abs(hash);
        return hash;
    }
    //模拟负载均衡，通过生成一个随机字符串（random）来模拟请求，最终通过一致性哈希选择一个服务器
    @Override
    public String balance(List<String> addressList) {
        //生成一个随机字符串（UUID），然后将其作为 node 参数传递给 getServer 方法，来获取相应的服务器地址。
        String random = UUID.randomUUID().toString();
        return getServer(random, addressList);
    }

}
```

## 1. 添加虚拟节点的目的是什么？

在一致性哈希（Consistent Hashing）中，**虚拟节点**的引入是为了 **提高负载均衡的均匀性**，尤其是在节点（服务器）变动时，尽量减少请求的重新分配和数据的不均匀分布。

* **避免负载不均衡**：
  * 在没有虚拟节点的情况下，每个物理节点（真实节点）都会映射到哈希环上的一个单独的点。假设哈希值是均匀分布的，那么节点数量少时，可能有一些节点的哈希值落在哈希环的“稀疏”区域，导致某些节点需要处理大量请求，而其他节点的负载非常轻。这样就会造成负载不均衡。
  * 引入虚拟节点后，每个物理节点会映射到多个虚拟节点，从而增加哈希环上的点数，使负载分布更加均匀。每个虚拟节点都有自己的哈希值，物理节点通过这些虚拟节点在环上“分布”，负载更均匀。
* **提高节点的扩展性和容错性**：
  * 在分布式系统中，节点可能会增加或减少。没有虚拟节点时，节点的增加或删除会导致哈希环上的重新分配，可能导致大量的请求重定向到其他节点。引入虚拟节点后，节点的增加或删除只会影响与该节点相关的少部分虚拟节点，从而减少了请求的迁移。
  * 举个例子，假设你有 3 个物理节点，并且它们都只有一个虚拟节点（哈希环上的一个点），如果增加一个节点，这个新增节点可能会影响到哈希环上大部分区域的重新分配，导致很多请求需要迁移到新的节点。而如果每个物理节点有多个虚拟节点，新增一个节点只会影响哈希环上少量的区域，从而减少了请求的迁移量。
* **减少单节点故障的影响**：
  * 如果一个物理节点故障且没有虚拟节点，那么所有映射到该节点的请求都会丢失。引入虚拟节点后，某个物理节点故障时，其对应的虚拟节点也会失效，但其他虚拟节点可能会分摊其负载，减少系统的影响。

## 2.getHash 这个方法是做什么的？

这段代码实现了一个 **哈希函数**，用于将给定的字符串 `str` 转换成一个整数哈希值。哈希函数的作用是把输入的任意长度的字符串映射到一个固定大小的哈希值，以便进行后续的哈希操作，如一致性哈希、数据分片等。

```java
final int p = 16777619;
int hash = (int) 2166136261L;
```

* `p = 16777619` 是一个常数，用于哈希计算。这个常数是 `FNV-1a` 哈希算法中的质数系数。`FNV` 是一种常见的哈希算法，广泛应用于哈希表和一致性哈希中。
* `hash = 2166136261L` 是一个初始的哈希值，通常 `2166136261L` 作为 `FNV-1a` 哈希算法的种子值。它是 `FNV` 哈希算法中的初始哈希值，也常用于哈希计算中的初始化步骤。

```java
for (int i = 0; i < str.length(); i++)
hash = (hash ^ str.charAt(i)) * p;
```

* 这段代码遍历字符串中的每个字符，按照 `FNV-1a` 哈希算法计算哈希值。具体操作是：
  * 通过 `str.charAt(i)` 获取字符串中的第 `i` 个字符。
  * 将当前哈希值 `hash` 与该字符的 Unicode 值（`str.charAt(i)`）进行异或（`^`）操作。
  * 然后将结果乘以常数 `p`，这是 `FNV` 算法中的乘法步骤，用来扩展哈希值的范围。

```java
hash += hash << 13;
hash ^= hash >> 7;
hash += hash << 3;
hash ^= hash >> 17;
hash += hash << 5;
```

* 这部分代码对哈希值进行多轮位操作（包括位移和异或），其目的是进一步扩展哈希值的随机性，减少哈希冲突的可能性。这些操作本质上是混合哈希值的不同部分，使得最终的哈希值更均匀、更难预测。
* 这些位操作类似于一些哈希算法（例如 `MurmurHash`）中使用的混合策略，旨在增加计算的复杂度并降低哈希冲突。

```java
if (hash < 0)
    hash = Math.abs(hash);
```

* 如果最终的哈希值为负数，则取其绝对值。这是因为某些哈希应用（如一致性哈希、分布式系统中的分片等）中需要将哈希值映射到一个正整数区间。负数值在这些应用中可能导致不必要的问题，因此通过 `Math.abs(hash)` 转化为正数。

## 3.private SortedMap\<Integer, String> shards = new TreeMap<>();为什么用 treeMap？

* **自动排序**： `TreeMap` 会根据键的顺序自动进行排序，因此当我们使用 `TreeMap` 存储哈希值与虚拟节点之间的映射时，哈希值将会始终保持有序。这一点非常重要，因为一致性哈希算法依赖于将虚拟节点映射到哈希环上的不同位置，并且需要按哈希值的大小顺序来查找最近的节点。

具体到一致性哈希算法，哈希环上各个节点的哈希值必须是 **有序的**，而 `TreeMap` 正是通过底层的红黑树实现了这种有序性，确保了虚拟节点在哈希环上的顺序正确。

* **高效的范围查找**： `TreeMap` 提供了非常高效的范围查找操作。它允许我们使用 `tailMap()` 方法查找从某个特定键开始的所有键值对，这对于一致性哈希中的 `查询下一个节点` 操作至关重要。

例如，当我们根据某个数据的哈希值（如请求的哈希值）来查找该数据应该映射到哪个虚拟节点时，如果我们在 `shards` 中使用 `TreeMap`，可以通过 `tailMap()` 方法找到所有大于等于该哈希值的虚拟节点，并且快速获取到最近的一个虚拟节点。

# 更改<font style="color:rgb(55, 65, 81);">ServiceCenter</font>

```java
//根据服务名（接口名）返回地址
@Override
public InetSocketAddress serviceDiscovery(String serviceName) {
try {
    //先从本地缓存中找
    List<String> serviceList=cache.getServcieFromCache(serviceName);
    //如果找不到，再去zookeeper中找
    //这种i情况基本不会发生，或者说只会出现在初始化阶段
    if(serviceList==null) {
        serviceList=client.getChildren().forPath("/" + serviceName);
    }
    // 负载均衡得到地址
    String address = new ConsistencyHashBalance().balance(serviceList);
    return parseAddress(address);
} catch (Exception e) {
    e.printStackTrace();
}
return null;
```


> 更新: 2024-11-29 19:35:16  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/fclel2eldqricm7n>