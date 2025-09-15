---
title: LDir 和 LFileInfo 的语义和设计
categories:
  - 实习
  - 合迅科技
  - 课题研究
abbrlink: fceabd3a
date: 2024-04-15 21:55:00
updated: 2024-06-05 23:30:00
---

<meta name="referrer" content="no-referrer"/>

`LarkSDK`中`FileSystem`中关于`LDir和LFileInfo的语义和设计`的总结。

# 语义明确

​	`QDir`和`QFileInfo`的语义一直以来都比较令人费解。我们知道文件和目录的关系是：目录是一种特殊的文件。按照`QDir`和`QFileInfo`的命名来讲，应该是`QDir`管理目录，`QFileInfo`管理文件，但是实际上这两个类的功能是非常混乱的，`QDir`可以操作文件，`QFileInfo`也可以操作目录。而初版的`LDir`和`LFileInfo`也是完全按照QT的思维走的，因此导致该部分的语义非常混乱，让我们和用户感到非常费解。

<!-- more -->

​	注意，该任务不包括`LFile`的部分，只关心`LDir`和`LFileInfo`的语义和设计问题。`LFile`是一类，需要通过`IO`进行文件操作，类似于`IODevice`。而`LDir`和`LFileInfo`是一类。

​	现在的设计方式是直接存储一个`LString`类型的路径，但是这样对于`windows`平台非常不友好。`QT`也是这样做的，不过可能是因为历史原因，未能按照更好的方式修改。但是QT的功能经过庞大的迭代是很稳定的，但是这部分初版的功能一言难尽。

​	同时，目前`LDir`和`LFileInfo`的语义非常不明确。为了避免混淆，将二者重新命名为`LFileSystemPath`和`LFileSystemEntry`，后续`LDir`作为`LFileSystemPath`的别名，`LFileInfo`作为`LFileSystemEntry`的别名。经过我的思考和组内集体的讨论，有了如下的设计。

1. `LFileSystemPath`：存储规范化后的路径的结构。

   `LFileSystemPath`讨论以后的设计是能够将用户传入的路径进行合理并且严格的规范化，本类只负责这个功能。至于该路径指向的具体是文件还是目录，该路径指向的文件或目录是否存在，有什么权限，本类不关心。按照此语义，本类应当不会涉及与平台相关的具体接口。

2. `LFileSystemEntry`：内部存储一个路径结构`LFileSystemPath`，真正与系统`API`打交道的类。

   `LFileSystemPath`对路径做了严格规定，因此在`LFileSystemEntry`中会存储这个结构用于处理系统中的路径。在本类当中就会提供平台相关的借口了，比如创建目录`mkdir`，进入目录`cd`，文件权限`permission`等等操作，当然，这些操作都离不开路径结构`LFileSystemPath`。

   与`Qt`不同的是，`LFileSystemEntry`中存储的路径指向的文件或目录都必须是在系统中实际存在的，不允许存储不存在的路径，因为这样没有意义。如果非要存储，请使用单纯的路径结构`LFileSystemPath`。

3. 二者的命名规定。

   对于大多数用户而言，可能并不知道目录是一种特殊的文件（目录和文件都具有`rwx`权限，只是表现方式不同），因此本类的名称有待商榷，目前的商讨结果是将`LDir`作为另一个类（例如`LFileSystemPath`这种）的别名，类似`LVector`和`LPaddedVector`的关系。保留`LDir`是考虑了`Qt`的缘故。`LFileSystemEntry`与`LFileInfo`的关系同理。

# 更多细节

1. `LFileSystemPath`如何存储路径。

   经过对比`Qt`和与钟老师讨论，决定使用`StringList`存储各级目录名和文件名的方式。

   `windows`和`linux`文件系统最大的区别就在于`windows`使用反斜杠`\`，`linux`使用正斜杠`/`，但是这些在存储的时候根本没有必要存储，因此直接使用`StringList`存储文件名即可。下面会涉及到更多的细节。

   第一，如何判断绝对路径还是相对路径。

   在`Linux`下，绝对路径以`/`开始。在`windows`下同理，绝对路径是盘符加上`\`，例如`c:\`，用户传递进来的路径经过我们内部的`split`处理存入到一个一个的`StringList`元素中，很容易联想到，在开头作标识即可。如果是绝对路径，那么`StringList`开头会留出一个空元素，相对路径则不会。

   例如，对路径`/path/to/../to/local`，就是这样的结构：

   <img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240411163331656.png" alt="image-20240411163331656" style="zoom: 80%;" />

   第二，如何判断末尾是文件还是目录。

   文件和目录很有可能出现同名的情况，这是必须要考虑的事情。（这和文件是否具有后缀没有关系，目录也可以写成带后缀的形式）我们需要想办法在`LFileSystemPath`的结构里面表现出来。结合上面的例子，如果我的路径是`/path/to/../to/local/`的话，就一定代表`local/`是一个目录了，具体在系统中到底存不存在本类不关心，可以效仿刚才的做法，在末尾加上一个空元素，即可区分。

   对于文件还是目录，在本类中做了严格的规定，`local`是文件，`local/`是目录。之所以要这样做，请看下面`LFileSystemEntry`提出的联想问题。

   对于路径`/path/to/../to/local/`，就是这样的结构：

   <img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240411164213369.png" alt="image-20240411164213369" style="zoom:80%;" />

   至此，我们就在`LFileSystemPath`的层面对绝对路径和相对路径，文件和目录进行了严格的规定。

2. `LFileSystemEntry`构造时关于文件和目录的修正。

   在构造的时候会涉及到一个问题，对于`LFileSystemPath`而言，我们硬性对目录和文件做了规定，目录结尾必须有`/`，文件没有。但是对用户而言，可能并不知道，因此用户可能想要目录，也可能写入`/path/to/../to/local`，如果系统中这个`local`的确是一个目录，并且没有要给同名的`local`文件存在，这是没有毛病的。这个问题需要进行处理。

   比较合理的解决方式是，首先由于无论是在`linux`还是在`windows`下，文件和目录不能同名，也就是不能同时出现`test`和`test/`。对于`LFileSystemEntry`而言，对于文件类型的路径，既能匹配到文件也能匹配到目录，因此如果匹配成功，需要做二次匹配，匹配对应的目录，如果匹配失败，代表是一个文件路径；如果匹配成功，需要对此时的`LFileSystemPath`存储的内容做了修正，变成了`/path/to/../to/local/`。当然用户如果传入`/path/to/../to/local/`，那一定匹配的是目录，这一点毋庸置疑。

3. 考虑盘符。

   `windows`下存在盘符，例如路径`d:\a\b\c`，其中`d:`就代表盘符，`\a\b\c`就是从`d:`盘符下的根目录开始的依次的`a`，`b`目录和`c`文件，那么对应到`linux`下面呢？例如`d:/a/b/c`，这就是一个正确的相对路径了，分别对应`d:`，`a`，`b`目录和`c`文件，可见如何处理盘符是一个非常重要的问题。

   其次，考虑了盘符以后，我们需要考虑`LFileSystemPath`和`LFileSystemEntry`的对接问题。例如我给出路径`d:a\b\c`，对应盘符`d:`和相对路径`a\b\c`。我们上面专门谈到过`LFileSystemPath`不关心这个路径是否合法，指向的东西是否存在，只是做一个规范化的存储的数据结构。因此在这里而言，理论上来讲，这个路径下的盘符是没有意义的，因为给出的是一个相对路径，而`LFileSystemPath`的结构为`LFileSystemEntry`服务的时候，真正的工作路径的盘符与用户给出的盘符可能并没有联系，因此经过讨论，这里的盘符会被忽略掉。也就是说如果调用`path()`方法导出路径，会得到`a\b\c`，盘符就没有了，这也是比较符合逻辑的。

   当然。上面只是考虑了一种情况，实际的情况可能是有无盘符和绝对相对路径的综合情况，故作下图进行总结：

   <img src="https://cdn.davidingplus.cn/images/2025/02/01/综合总结.png" alt="综合总结" style="zoom:80%;" />

   因此，经过如上考虑，最新的算法流程是一个`path`进来以后，先考虑盘符，如果是`windows`尝试提取盘符，如果是`linux`不管；后面再进行反斜杠`\`转化为正斜杠`/`，然后`split`，再存储的过程，当然其中会有更多需要注意的小细节。

4. 经过讨论，原`LDir`中处理目录的相关接口和`LFileInfo`处理文件的相关接口移动到新`LFileInfoEntry`中，现将所有接口归纳在这里。

   - 路径返回接口

     - `drive()`：返回盘符，效果等同于`LFileSystemPath`的`drive()`
     - `path()`：返回路径，效果等同于`LFileSystemPath`的`path()`

     - `canonicalPath()`：返回规范化路径，效果等同于`LFileSystemPath`的`canonicalPath()`

     - `absolutePath()`：返回绝对路径，如果是绝对路径，计算规范化以后的绝对路径；如果是相对路径，以当前可执行文件的工作目录为基准路径，进行拼接，计算最后规范化以后的绝对路径
     - `relativePath(const LString& path)`：计算给定`path`相对于本类中文件或目录的相对路径（这个算法待写）
     - 关于`path()`，`canonicalPath()`和`absolutePath()`接口三者的区别，通过一个示例就知道了（假设是`linux`，`windows`同理），设当前可执行文件的目录是：`/a/b/c/d/e/`
       - 构造原串：`../..///./..`
       - `path()`：`../.././../`
       - `canonicalPath()`：`../../../`
       - `absolutePath()`：`/a/b/`

   - 判断接口

     - `isValid()`：判断路径是否合法，在构造函数的时候会验证路径是否合法（遵循上面的规则），并且`LFileSystemEntry`规定路径指向的文件或目录必须存在，不存在则警告，并且清空数据。后续的其他函数首要逻辑就是判断是否合法
     - `isAbsolute()`：判断是否为绝对路径
     - `makeAbsolute()`：将当前类存储的路径转化为绝对路径
     - `isRelative()`：判断是否为相对路径
     - `isDir()`：判断是否为目录
     - `isFile()`：判断是否为文件
     - `isRoot()`：判断是否为根目录
     - `isHidden()`：判断是否为隐藏文件或目录
     - `isReadable()`：判断目录或文件是否具有可读权限
     - `isWritable()`：判断目录或文件是否具有可写权限
     - `isExecutable()`：判断目录或文件是否具有可执行权限

   - 名字返回接口

     - `basename()`：返回当前目录或文件的名称。遵循`linux`系统下`basename`命令的规则，经验证，返回完整名字，注意目录需要去掉末尾的斜杠
     - `name()`：`basename()`的别名
     - `singleSuffix()`：获取当前文件的后缀，找到最后一个`.`以后的部分
     - `suffix()`：`singleSuffix()`的别名
     - `completeSuffix()`：获取当前文件的后缀，找到第一个`.`以后的部分

   - 目录相关接口

     - `cd(const LString& path)`：根据传入的路径进入指定的目录。如果是绝对路径，按照新绝对路径；如果是相对路径，以本类中存储的目录路径会基准进行拼接，得到最终的绝对路径
     - `cdUp()`：进入父级目录，效果等同于`cd(LString(".."))`
     - `mkdir(const LString& path)`：根据传入的路径创建新的目录。规则同`cd()`
     - `rmdir(const LString& path)`：根据传入的路径删除指定的目录。规则同`cd()`
     - `entryCount()`：统计当前目录中的目录和文件的总数
     - `count()`：`entryCount()`的别名
     - `entryList()`：获取当前目录中的目录和文件的列表

   - 其他接口（由于调用了其他的类，目前直接放的旧版内容）
     - `rename(const LString& path)`：将当前目录或文件重命名为`path`指向的目录或文件
     - `birthTime()`：返回当前文件或目录的创建日期
     - `lastModified()`：返回当前文件或目录的最后一次修改日期
     - `lastRead()`：返回当前文件或目录的最后一次读取日期
     - `owner()`：返回当前文件或目录的所有者
     - `setPermission(LPermission permisson)`：设置当前文件或目录的权限
     - `permission()`：返回当前文件或目录的权限

5. 后续维护任务归纳。

   - 查看其他接口旧版的代码内容，优化修改或者重写
   - `cd()`接口考虑返回自身引用而不是`bool`值，满足链式调用需求
   - `isReadable()`、`isWritable()`、`isExecutable()`、`entryCount()`、`entryList()`目前使用旧版代码内容，同第一点
   - `relativeFilePath()`和`rename()`逻辑待补
   - `windows`下的`Lark::executableDirPath()`目前采用手动截取的方式，后续看有没有办法改为系统接口

