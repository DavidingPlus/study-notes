---
title: 使用 Woboq CodeBrowser 搭建源代码网站
categories:
  - 实习
  - 合迅科技
  - 课题研究
abbrlink: a98e1d5b
date: 2024-07-18 21:20:00
updated: 2024-07-30 16:00:00
---

<meta name="referrer" content="no-referrer"/>

# 背景

在日常学习工作中，我们不免需要浏览一些库的源码。在本地浏览源代码，例如使用`Source Insight`，当然是可以的，但问题是一是不方便，二是很多库下载下来是以头文件配合静态库或动态库的形式存在的，看不到`cpp`代码，因此阅读会受限。现在`Web`技术高速发展，有没有办法用网页直接查看源代码，并且还有类似于`Code`的代码跳转功能呢？换句话说，如何把`C/C++`代码转化为前端页面，并且最好是静态的前端页面，就是一个难题了。

幸运的是，`github`上有人提前考虑到了这件事情，并且有了具体的[项目](https://github.com/KDAB/codebrowser)，已经有`1k`多的`star`。并且原作者还基于这个框架弄出了一个在线的[源代码网站](https://codebrowser.dev/)，方便开发者查看各个`C/C++`库的源代码，例如`Qt`、`GCC`、`Linux Kernel`、`GNU C Library`等。

<!-- more -->

那么问题来了，都有在线的网站了为什么还要自己搭建一个呢？原因就是这个网站在国内被墙了。有人会说？开梯子啊，不是每个节点都能上，我嫌他烦，因此决定自己搭建一个源代码网站一劳永逸。我的源码网站地址：[https://davidingplus.github.io/code-browser/](https://davidingplus.github.io/code-browser/)

# 工作原理简介

`Woboq CodeBrowser`是基于`LLVM/Clang`实现的一个命令行工具。它通过深度解析`C/C++`源码生成最终我们需要的静态`HTML`文件。`Woboq CodeBrowser`包含`codebrowser_generator`和`codebrowser_indexgenerator`两个子命令。生成`HTML`文件到最终可以通过浏览器阅读代码，整体分三个步骤：

1. 先通过`codebrowser_generator`解析`.h`和`.cpp`生成对应的`.h.html`和`.cpp.html`。
2. 然后通过`codebrowser_indexgenerator`为所有目录生成 `index.html`。
3. 最后我们可以把这些`HTML`文件拷贝到某个`Web`服务器上，就可以在浏览器里愉快地浏览`C/C++`项目的源码了。

# 安装Woboq CodeBrowser工具

我们通过源码编译安装这个工具。

首先将该[项目](https://github.com/KDAB/codebrowser)克隆到本地，由于需要用`LLVM/Clang`工具链，强烈建议在`Linux`下操作。

```bash
git clone https://github.com/KDAB/codebrowser.git
```

克隆下的项目中有三个目录比较重要，一是`data`目录，这是一些前端的资源文件，例如`png`、`css`、`js`等；二是`generator`目录，这是可执行文件`codebrowser_generator`的源码目录；三是`indexgenerator`，这是可执行文件`codebrowser_indexgenerator`的源码目录。

前面提到，编译这个工具需要用到`LLVM/Clang`工具链，结合`github`上的[issue#119](https://github.com/KDAB/codebrowser/issues/119)，使用`llvm-14`和`clang-14`的版本比较合适。以`Ubuntu`为例：

```bash
sudo apt install llvm-14 clang-14 libclang-14-dev
```

但是在`Ubuntu20.04`及以下的版本上会出现**定位不到软件包**的问题，官方提供了一个解决方案，参见博客[https://blog.csdn.net/weixin_50749380/article/details/128319851](https://blog.csdn.net/weixin_50749380/article/details/128319851)

这样我们的编译环境就安装好了，紧接着我们需要构建这个项目并且将编译后的文件安装到本地。项目受`CMake`管理，同时参考[官方文档](https://github.com/KDAB/codebrowser/blob/master/README.md)，构建流程就很简单了。

```bash
mkdir -p build
cd build

cmake -DCMAKE_C_COMPILER=clang-14 -DCMAKE_CXX_COMPILER=clang++-14 -DCMAKE_BUILD_TYPE=Release .. # 除了 clang 还可能安装了 gcc 等其他的编译器，所以指定一下；同时指定编译选项为 Release ，这个在 Linux 下没有影响
make
sudo make install # 必要加上 sudo
```

安装完毕以后`data`目录，两个可执行文件`codebrowser_generator`和`codebrowser_indexgenerator`就成功安装到本地了。

![image-20240718105543595](https://image.davidingplus.cn/images/2025/02/01/image-20240718105543595.png)

# 如何使用

接下来以`googletest-1.12.1`为例，展示如何使用这个工具构建静态的源代码网站。

## 生成compile_commands.json

`compile_commands.json`文件是一种特定格式的[compilation database](https://sarcasm.github.io/notes/dev/compilation-database.html)文件，而所谓`compilation database`其实很简单，它里面记录的是每一个源码文件在编译时详细的信息（包括文件路径，文件名，编译选项等等）。而`compile_commands.json`文件是[LibTooling](https://clang.llvm.org/docs/LibTooling.html)需要的以`json`格式呈现的`compilation database`文件，以下截取的是`compile_commands.json`中的一个`entry`：

```json
[
...
    {
        "arguments": [
            "c++",
            "-c",
            "-g",
            "-O2",
            "-Werror",
            "-std=c++0x",
            "-Wall",
            "-fPIC",
            "-o",
            "attrs.o",
            "attrs.cc"
        ],
        "directory": "/home/astrol/libelfin/dwarf",
        "file": "attrs.cc"
    },
...
]
```

`Woboq Codebrowser`是基于`LLVM/Clang`实现的工具，也是基于`compile_commands.json`来分析源码关系的。

换句话说，要想使用`Woboq Codebrowser`，必须首先生成`compile_commands.json`文件。如果项目是由`cmake`构建的，那么恭喜你，只需加上`-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`即可。如果是传统的`make build system`也不要担心，`Bear`和`compdb`工具可以帮我们生成`compile_commands.json`文件。

现在让我们一起生成`googletest-1.12.1`项目对应的`compile_commands.json`。首先我们拉取[googletest](https://github.com/google/googletest)的源码到本地。源码采用`CMake`工具的管理，这样正好很方便的能帮我们生成`compile_commands.json`。之所以这么推荐`CMake`是因为使用其他的工具可能需要编译整个工程才能生成该文件，费事费力。

因为我是`C++11`标准的环境，因此需要使用的版本是`1.12.1`，切换到对应的`tag`。当然直接下载对应的`release`源码也行。

```bash
git checkout release-1.12.1 # target tag
```

然后我们使用`CMake`工具生成`compile_commands.json`文件，个人建议使用`gcc/g++`编译器。

```bash
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="gcc" -DCMAKE_CXX_COMPILER="g++" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```

在`build`目录下看到`compile_commands.json`文件正确生成即为成功。

## 使用codebrowser_generator生成html

`codebrowser_generator`是一个可执行文件，有各种执行参数，说明如下：

- `-a`：处理`compile_commands.json`中的所有文件。如果没有传递这个参数，那么就需要传递要处理的文件列表
- `-o`：指定文件输出目录
- `-b`：是指包含`compile_commands.json`的构建目录。如果没有传递这个参数，编译参数可以在`--`后通过命令行传递
- `-p`：（一个或多个）用于项目规范。即项目的名称、源代码的绝对路径和用冒号分隔的版本信息。示例：`-p projectname:/path/to/source/code:0.3beta`
- `-d`：指定包含所有`JavaScript`和`CSS`文件的数据`URL`。默认为相对于输出目录的`../data`。示例：`-d https://codebrowser.dev/data/`
- `-e`：是对外部项目的引用。示例： `-e clang/include/clang:/opt/llvm/include/clang/:https://codebrowser.dev/llvm`

例如，对于当前`googletest-1.12.1`项目，一条合适的命令可能是这样的：

```bash
codebrowser_generator -b ./compile_commands.json -a -p googletest-1.12.1:"${PWD}/..":1.12.1 -o ./docs -d ../data
```

执行完的结果是这样的：

![image-20240730153744964](https://image.davidingplus.cn/images/2025/02/01/image-20240730153744964.png)

需要注意的是，上面的操作都是在根目录的`build`构建目录执行的，这也是`CMake`用户的构建习惯。

同时，`-o ./docs`代表文件输出在`build/docs`中，`-d ../data`是指定资源文件的相对路径，代表资源文件位于生成目录`docs`的父级目录。至于为什么是`../data`，后面会详细解释。安装好`Woboq CodeBrowser`工具以后`data`目录会被安装在`/usr/local/share/woboq/data`，将其拷贝为`build/data`即可。

## 使用codebrowser_indexgenerator为每个目录生成index.html

`codebrowser_indexgenerator`同样有很多执行参数，说明如下：

- `-p`：（一个或多个）用于项目规范。即项目的名称、源代码的绝对路径和用冒号分隔的版本信息。示例：`-p projectname:/path/to/source/code:0.3beta`
- `-d`：指定包含所有`JavaScript`和`CSS`文件的数据`URL`。默认为相对于输出目录的 `../data`。示例：`-d https://codebrowser.dev/data/`

同样执行类似命令：

```bash
codebrowser_indexgenerator ./docs -d ../data
```

类似执行结果如下，可以看到每个目录对应都有了`index.html`。

![image-20240730154020528](https://image.davidingplus.cn/images/2025/02/01/image-20240730154020528.png)

## 关于-d参数以及资源文件

上面提到，为什么我使用的是`-d ../data`呢？我们先弄清楚`data`目录是干嘛的。

`data`目录存放着前端代码需要的资源文件，例如`CSS`、`JS`和图片资源等。因此需要指明这个目录的路径，绝对路径或者相对路径。由于我的[code-browser](https://github.com/DavidingPlus/code-browser/)项目需要存放很多库的源代码，因此我将`data`目录统一在根目录中，每个库的源代码对应一个自己的子目录，因此最终的结果是这样的：

![image-20240730154929638](https://image.davidingplus.cn/images/2025/02/01/image-20240730154929638.png)

这样就能正确定位到资源文件的路径了，`codebrowser_generator`生成的`html`文件同理。用户使用自己的习惯和方式即可。

# 参考文档

1. [https://blog.csdn.net/astrotycoon/article/details/104831055](https://blog.csdn.net/astrotycoon/article/details/104831055)
2. [https://blog.csdn.net/weixin_42148156/article/details/121727163](https://blog.csdn.net/weixin_42148156/article/details/121727163)
3. [https://blduan.top/post/工具使用/codebrowser之从浏览器阅读源码](https://blduan.top/post/工具使用/codebrowser之从浏览器阅读源码/)

