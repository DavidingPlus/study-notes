# LDir和LFileInfo的语义明确和重新设计

本迭代走查不包括`LFile`的部分，只关心`LDir`和`LFileInfo`的语义和设计问题。因为`LFile`是一类，`LDir`和`LFileInfo`是一类。

现在的设计方式是直接存储一个`LString`类型的路径，但是这样对于`windows`平台非常不友好。

# 语义明确

目前`LDir`和`LFileInfo`的语义非常不明确，为了避免混淆，将二者重新命名为`LFileSystemPath`和`LFileSystemEntry`，后续`LDir`作为`LFileSystemPath`的别名，`LFileInfo`作为`LFileSystemEntry`的别名。同时这里只给出讨论后的结果。

1. `LFileSystemPath`：存储规范化后的路径的结构。

   `LFileSystemPath`讨论以后的设计是能够将用户传入的路径进行合理并且严格的规范化，本类只负责这个功能。至于该路径指向的具体是文件还是目录，该路径指向的文件或目录是否存在，有什么权限，本类不关心。按照此语义，本类应当不会涉及与平台相关的具体接口。

   现在需要考虑如何存储路径了。

   经过对比`Qt`和与钟老师讨论，决定使用`StringList`存储各级目录名和文件名的方式。

   `windows`和`linux`文件系统最大的区别就在于`windows`使用反斜杠`\`，`linux`使用正斜杠`/`，但是这些在存储的时候根本没有必要存储，因此直接使用`StringList`存储文件名即可。下面会涉及到更多的细节。

   第一，如何判断绝对路径还是相对路径。

   在`Linux`下，绝对路径以`/`开始。在`windows`下同理，绝对路径是盘符加上`\`，例如`c:\`，用户传递进来的路径经过我们内部的`split`处理存入到一个一个的`StringList`元素中，很容易联想到，在开头作标识即可。如果是绝对路径，那么`StringList`开头会留出一个空元素，相对路径则不会。

   例如，对路径`/path/to/../to/local`，就是这样的结构：

   <img src="https://img-blog.csdnimg.cn/direct/cf95a44c1cf04e75941f8a6141d03e16.png" alt="image-20240411163331656" style="zoom: 80%;" />

   第二，如何判断末尾是文件还是目录。

   文件和目录很有可能出现同名的情况，这是必须要考虑的事情。（这和文件是否具有后缀没有关系，目录也可以写成带后缀的形式）我们需要想办法在`LFileSystemPath`的结构里面表现出来。结合上面的例子，如果我的路径是`/path/to/../to/local/`的话，就一定代表`local/`是一个目录了，具体在系统中到底存不存在本类不关心，可以效仿刚才的做法，在末尾加上一个空元素，即可区分。

   对于文件还是目录，在本类中做了严格的规定，`local`是文件，`local/`是目录。之所以要这样做，请看下面`LFileSystemEntry`提出的联想问题。

   对于路径`/path/to/../to/local/`，就是这样的结构：

   <img src="https://img-blog.csdnimg.cn/direct/dba84a78265e43639c1543f530ffdf85.png" alt="image-20240411164213369" style="zoom:80%;" />

   至此，我们就在`LFileSystemPath`的层面对绝对路径和相对路径，文件和目录进行了严格的规定。

   第三，类名问题。

   对于大多数用户而言，可能并不知道目录是一种特殊的文件（目录和文件都具有`rwx`权限，只是表现方式不同），因此本类的名称有待商榷，目前的商讨结果是将`LFileSystemPath`作为另一个类（例如`LFileSystemPath`这种）的别名，类似`LVector`和`LPaddedVector`的关系。保留`LFileSystemPath`是考虑了`Qt`的缘故。

2. `LFileSystemEntry`：内部存储一个路径结构`LFileSystemPath`，真正与系统`API`打交道的类。

   `LFileSystemPath`对路径做了严格规定，因此在`LFileSystemEntry`中会存储这个结构用于处理系统中的路径。在本类当中就会提供平台相关的借口了，比如创建目录`mkdir`，进入目录`cd`，文件权限`permission`等等操作，当然，这些操作都离不开路径结构`LFileSystemPath`。

   现在会涉及到一个问题，对于`LFileSystemPath`而言，我们硬性对目录和文件做了规定，目录结尾必须有`/`，文件没有。但是对用户而言，可能并不知道，因此用户可能想要目录，也可能写入`/path/to/../to/local`，如果系统中这个`local`的确是一个目录，并且没有要给同名的`local`文件存在，这是没有毛病的。这个问题需要进行处理。

   比较合理的解决方式是，对于`LFileSystemEntry`而言，首先去判断该路径是否是一个文件，因为如果`local`目录和`local`文件同时存在，这个东西一定是文件；如果匹配失败，去匹配是否是一个目录，如果匹配成功，那么存入`LFileSystemPath`，注意此时的`LFileSystemPath`存储的内容做了修正，变成了`/path/to/../to/local/`。当然用户如果传入`/path/to/../to/local/`，那一定是目录，这一点毋庸置疑。

# 更多细节

1. 考虑盘符的问题。

   `windows`下存在盘符，例如路径`d:\a\b\c`，其中`d:`就代表盘符，`\a\b\c`就是从`d:`盘符下的根目录开始的依次的`a`，`b`目录和`c`文件，那么对应到`linux`下面呢？例如`d:/a/b/c`，这就是一个正确的相对路径了，分别对应`d:`，`a`，`b`目录和`c`文件，可见如何处理盘符是一个非常重要的问题。

   其次，考虑了盘符以后，我们需要考虑`LFileSystemPath`和`LFileSystemEntry`的对接问题。例如我给出路径`d:a\b\c`，对应盘符`d:`和相对路径`a\b\c`。我们上面专门谈到过`LFileSystemPath`不关心这个路径是否合法，指向的东西是否存在，只是做一个规范化的存储的数据结构。因此在这里而言，理论上来讲，这个路径下的盘符是没有意义的，因为给出的是一个相对路径，而`LFileSystemPath`的结构为`LFileSystemEntry`服务的时候，真正的工作路径的盘符与用户给出的盘符可能并没有联系，因此经过讨论，这里的盘符会被忽略掉。也就是说如果调用`path()`方法导出路径，会得到`a\b\c`，盘符就没有了，这也是比较符合逻辑的。

   当然。上面只是考虑了一种情况，实际的情况可能是有无盘符和绝对相对路径的综合情况，故作下图进行总结：

   <img src="https://img-blog.csdnimg.cn/direct/2a60ae10f08742ef8b35f207c79245db.png" alt="综合总结" style="zoom:80%;" />

   因此，经过如上考虑，最新的算法流程是一个`path`进来以后，先考虑盘符，如果是`windows`尝试提取盘符，如果是`linux`不管；后面再进行反斜杠`\`转化为正斜杠`/`，然后`split`，再存储的过程，当然其中会有更多需要注意的小细节。

2. 经过讨论，原`LDir`中处理目录的相关接口和`LFileInfo`处理文件的相关接口移动到新`LFileInfoEntry`中，现将所有接口归纳在这里。

   - 文件接口
     - `isExecutable()`：判断是否为可执行文件
     - `isFile()`：判断是否为非目录文件
     - `isHidden()`：判断是否为隐藏文件
     - `completeSuffix()`：获取文件后缀
   - 目录接口
     - `isDir()`：判断是否为目录
     - `isRoot()`：判断目录是否为根目录
     - `cd()`：进入指定目录
     - `count()`：统计目录中的目录和文件总数
     - `mkdir()`：在当前目录下，创建一个子目录
     - `entryInfoList()`：获取目录下的文件列表
   - 文件和目录通用接口
     - `setFile()`：做了语义明确以后，这个接口应该改为`setPath()`接口，并且针对`LFileSystemPath`构造成的结构作合法性的检测
     - `isEmpty()`：判断目录或文件是否为空
     - `isAbsolute()`：判断是否为绝对路径
     - `isWritable()`：判断目录或文件是否可写
     - `exists()`：判断目录或文件是否存在
     - `name()`：返回目录或文件名称，应与原`LFileInfo`类的`completeBaseName()`统一
     - `rename()`：重命名文件或目录。经检查，重命名文件在`LFile`当中，个人认为在明确语义以后，应将`rename`的操作交予`LFileInfoEntry`处理，`LFile`是一个`IODevice`，专门处理与`IO`相关的内容。如果`LFile`非要提供这个接口也不是不可以，但是内部真正起作用的是`LFileInfoEntry`
     - `absolutePath()`：返回绝对路径，目前语义已明确，目录和文件是一个东西，返回的都是指向目录和文件本身的
       - 应与原`LFileInfo`中的`absoluteFilePath()`和`absoluteDir()`做统一
       - 如果传入的是相对路径，那么需要找一个基准路径来计算得出最终的绝对路径。这个路径可以是当前工作目录的路径，可以是当前可执行文件的路径。经讨论，目前使用可执行文件的路径。后续`lcoremisc`中会提供相对工作目录的路径，后续再讨论如何更好的处理。
     - `makeAbsolute()`：将目录或文件路径转化为绝对路径
     - `relativeFilePath()`：获取给定文件或目录的结构相对于类内文件或目录的相对路径
     - `birthTime()`：返回目录或文件创建日期，如果是符号链接，应当在函数参数中给出参数，分别对符号链接本身和符号链接指向的文件做处理
     - `lastModified()`：获取目录或文件最后一次修改日期
     - `lastRead()`：获取目录或文件最后依次读取日期
     - `owner()`：获取目录或文件所有者
     - `permission()`：返回目录或文件的权限
     - `setPermission()`：设置目录或文件的权限

