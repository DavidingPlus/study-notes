---
title: 编译缓存工具 CCache 介绍与基本使用
categories:
  - 一些技巧
abbrlink: 2f8a8633
date: 2024-07-30 17:05:00
updated: 2024-08-19 16:35:00
---

<meta name="referrer" content="no-referrer"/>

编译时间过长是影响大型项目研发效率的一大难题。就市面上被普遍使用的构建系统，其确实存在一定的编译缓存机制，使得用户在重复进行编译操作时（例如修改了一个`Bug`），尽可能复用之前的编译成果以节省编译时间。例如`CMake`使用`CMakeCache.txt`来对编译缓存项进行追踪。但类似的机制存在一些限制，如`CMakeCache.txt`只能管理它自己所在项目的构建目录，如果用户因为一些原因清空了整个构建目录，构建缓存也将被一同清理；又如果多人协作同一个代码项目，他们各自都有属于自己的用户目录和工作目录（甚至自己的工作电脑），自然不可能指望他们可以共享编译缓存，即使这些缓存实质上确实是完全相同的，毕竟是同一份源码编译出的同样的东西。

为了解决这个问题，我们可以引入专业的编译缓存管理工具：`CCache`。

<!-- more -->

# CCache简介

> CCache is a compiler cache. It speeds up recompilation by catching the result of previous compilations and detecting when the same compilation is being done again.

你可以把`CCache`理解为编译缓存的共享存储空间。它管理一个目录`cache_dir`，这个目录负责存放和记录你编译产生的所有缓存项。当你每次进行编译时，若`CCache`在缓存中发现（缓存命中）了你将要编译的某个项目（比如你没有改某个`.cpp`文件从而生成的完全相同的`.cpp.o`文件），将会跳过真正的编译流程，而直接从缓存中将命中的项目提取出来“假装”完成了编译。这对于大型项目增量编译而言，意义非凡。这保证了我们每一次对项目代码进行修改并编译的时候，几乎总是能做到严格的增量编译，从而不去浪费时间在一遍又一遍的编译那些重复的东西上。

总体来说，编译缓存是一种“空间换时间”的思路：利用空余的磁盘空间存储尽可能多的编译缓存项目，并用专门的工具和缓存命中算法加以管理的策略。更进一步思考，编译缓存的存储位置不一定在本机，完全可以放在某个网络服务器上（为保证速度，通常用局域网服务器搭建），工作在同一个局域网的机器可以一起利用服务器来获取编译缓存，机器自身在编译产生成果的同时同时还可以反过来向服务器贡献编译缓存。这样的工作流程有时会被成为编译农场（`Compiling Farm`）。

# CCache安装与配置

> 参考：[官网 4.10.2 版本完整使用说明书](https://ccache.dev/manual/4.10.2.html)

以下操作的默认都在`Linux`系统进行。`Windows`系统类似，区别会在后面额外说明。

## 下载

`CCache`的安装非常简单。其最新版`4.10.2`可以[在官网上直接下载](https://ccache.dev/download.html)。推荐直接下载二进制发布版（`binary release`），当然愿意的话也可以下载源码编译。

> 注：不推荐用 apt 安装，上面只有较老旧的 3.x 版本。

下载后解压，目录内只需要关注唯一一个可执行文件`ccache`。为执行方便，可以将其复制到`PATH`所包含的目录中。官网推荐复制到`/usr/local/bin`目录：

```bash
# 先解压并进入 CCache 安装包目录
sudo cp ccache /usr/local/bin/
```

需要的话重新载入终端，测试“安装”结果：

```bash
ccache -V

# ccache version 4.10.1
# Features: avx2 file-storage http-storage redis+unix-storage redis-storage

# Copyright (C) 2002-2007 Andrew Tridgell
# Copyright (C) 2009-2024 Joel Rosdahl and other contributors

# See <https://ccache.dev/credits.html> for a complete list of contributors.

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later version.
```

## 配置

通过`-p`或`--show-config`参数，可以查看`CCache`的配置：

```bash
ccache -p

# (default) absolute_paths_in_stderr = false
# (default) base_dir =
# (default) cache_dir = /home/lzx0626/.cache/ccache
# (default) compiler =
# (default) compiler_check = mtime
# (default) compiler_type = auto
# (default) compression = true
# (default) compression_level = 0
# (default) cpp_extension =
# (default) debug = false
# (default) debug_dir =
# (default) debug_level = 2
# (default) depend_mode = false
# (default) direct_mode = true
# (default) disable = false
# (default) extra_files_to_hash =
# (default) file_clone = false
# (default) hard_link = false
# (default) hash_dir = true
# (default) ignore_headers_in_manifest =
# (default) ignore_options =
# (default) inode_cache = true
# (default) keep_comments_cpp = false
# (default) log_file =
# (default) max_files = 0
# (/home/lzx0626/.config/ccache/ccache.conf) max_size = 10.0 GiB
# (default) msvc_dep_prefix = Note: including file:
# (default) namespace =
# (default) path =
# (default) pch_external_checksum = false
# (default) prefix_command =
# (default) prefix_command_cpp =
# (default) read_only = false
# (default) read_only_direct = false
# (default) recache = false
# (default) remote_only = false
# (default) remote_storage =
# (default) reshare = false
# (default) run_second_cpp = true
# (default) sloppiness =
# (default) stats = true
# (default) stats_log =
# (default) temporary_dir = /run/user/1000/ccache-tmp
# (default) umask =
```

默认配置一般已经可以满足大部分情况下使用。目前需要关注的可能有：

- `max_size`：描述`CCache`将预留多少磁盘空间用于存放编译缓存。默认`5GB`。如果编译较大的项目且磁盘空间富余，可以适当考虑预留多一些空间。
- `cache_dir`：`CCache`存放编译缓存的目录位置。可以看到这个目录位于当前的用户目录下，也就是说，如果不同的用户使用同一台机器，他们将使用不同的缓存目录，需注意。

若需修改配置，可以使用`-M`或`--max-size`参数，例如将`max_size`设置为`10GB`：

```bash
ccache -M 10
# Set cache size limit to GiB
```

检查是否设置成功：

```bash
ccache -p | grep max_size
# (/home/lzx0626/.config/ccache/ccache.conf) max_size = 10.0 GiB
```

注意这里对`max_size`的配置已经被写入了自动建立的配置文件中。

> 关于配置项的完整说明，参见[官网手册](https://ccache.dev/manual/4.10.2.html#_configuration_options)

## 使用

`CCache`通常有两种使用方式：

1. 直接使用：通过`ccache`命令直接调用，编译命令本身统统接在`ccache`后面作为其参数。例如：`ccache gcc -c example.c`
2. 通过“劫持”`gcc、g++`等命令使用。这样当每次调用`gcc`等命令时，实际已经通过了`ccache`的处理。

第一种方式一般仅仅在测试`CCache`功能等场合时使用。实际工作中因为我们通常都是使用`Makefile`等手段完成编译，配置编译工具链来调用`ccache`既麻烦又不现实，所以一般使用第二种方式。官网推荐构建软链接的方式完成“劫持”：

```bash
sudo ln -s ccache /usr/local/bin/gcc
sudo ln -s ccache /usr/local/bin/g++
sudo ln -s ccache /usr/local/bin/cc
sudo ln -s ccache /usr/local/bin/c++
```

这样就把`gcc、g++、cc、c++`四个命令通过软链接指向了`ccache`。验证如下：

```bash
ll /usr/local/bin/gcc
# lrwxrwxrwx 1 root root 6 7月  12 22:51 /usr/local/bin/gcc -> ccache
```

通常安装好`gcc`后，`gcc`等命令本身位于`/usr/bin/gcc`，命令本身是指向实际`gcc`版本的软链接，再通过多重软连接等手段指向实际的`gcc`可执行文件。这个在每台机器上可能都有所不同，例如：

```bash
ll /usr/bin/gcc
# lrwxrwxrwx 1 root root 21 7月  12 22:39 /usr/bin/gcc -> /etc/alternatives/gcc

ll /usr/bin/gcc-9
# lrwxrwxrwx 1 root root 22 10月 24  2022 /usr/bin/gcc-9 -> x86_64-linux-gnu-gcc-9

which x86_64-linux-gnu-gcc-9
# /usr/bin/x86_64-linux-gnu-gcc-9
```

因此我们选择不覆盖原本`/usr/bin`里面的软链接，而把我们指向`ccache`的软链接放在更高执行优先的`/usr/local/bin`之中。这样既可以使`gcc`等命令完美优先指向`ccache`，同时也尽可能不去破坏原有环境。

## 整合到CMake

如果现在我们直接使用`make`编译，可能会发现编译缓存并没有起到作用。原因在于`CMake`默认告诉`Makefile`的编译命令，可能直接是`/usr/bin`下面的软链接，这样便绕开了我们定义在`/usr/local/bin`下面的软链接。可以用`make`命令的`VERBOSE`参数验证如下：

```bash
cmake ..
make VERBOSE=1
```

通过输出可以检查`make`实际调用的命令：

```markdown
...
...
[ 75%] Building CXX object CMakeFiles/larkgui.dir/src/lark-gui/component/lbutton.cpp /usr/bin/g++ ...
...
...
```

如果证实了上述情况，我们可以借助设置`CMake`的`CMAKE_<LANG>_COMPILER`变量来控制`make`具体使用的编译器命令。例如我们分别设置用于`C`和`C++`语言的编译器命令。经测试，注意这里一定要加上引号，否则可能失效。

```bash
set (CMAKE_C_COMPILER "gcc")
set (CMAKE_CXX_COMPILER "g++")
```

# Windows平台适配

`CCache`当然在`Windows`上使用，只是配置稍微有些麻烦。

在`Windows`中安装`C/C++`编译环境有几种选择。可以安装微软提供的`VS`全家桶，也就是`MSVC`编译器。也可以安装跨平台编译器`gcc`，当然大部分情况是服务于`linux`的。或者安装`GNU`项目专门为了在`Windows`上使用以`gcc`为核心组件的`GNU`工具链而研发的`Mingw`工具。这几个的区别和联系可以参考文章：[https://blog.csdn.net/qq_29495615/article/details/133365562](https://blog.csdn.net/qq_29495615/article/details/133365562)

安装`MSVC`推荐去[微软官方](https://visualstudio.microsoft.com/vs/)下载，安装`gcc`或者`Mingw`推荐使用`Scoop`包管理器进行安装，具体请自行研究。

前面在`Linux`上配置`CCache`的时候，我们配置软链接对`gcc`和`g++`命令做了劫持。在`Windows`上不太好做到这一点，我也没有具体尝试过，因此这里将以`MSVC`编译器为例进行配置。在`github`上有相关文章介绍：[https://github.com/ccache/ccache/wiki/MS-Visual-Studio](https://github.com/ccache/ccache/wiki/MS-Visual-Studio)，以下均基于此文章进行总结。

## 配置环境变量

在官网下载下来的`CCache`压缩包，例如`ccache-4.10.2-windows-x86_64.zip`，解压下来以后默认是不具有环境变量的性质的。也就是没有办法直接在命令行使用`CCache`命令，因此需要配置用户变量或者系统变量。具体配置请自行查阅资料。

配置环境变量也是为了能正确被`VS`或者`CMake`找到可执行文件的位置，方便后续的操作。配置好以后，像前面一样，输入`ccache -p`或者其他命令应该会得到预期结果。

> 注意，如果你使用了 Scoop 或者 Chocolatey 等 Windows 上的包管理器，请不要使用它们安装 CCache，后文会提到原因。请从官网下载压缩包解压，并且配置环境变量。

## 选择方案

文章中给出了两个方案，一个是**伪装成编译器**，一个是**编写包装脚本**，如图所示：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20240819160359910.png" alt="image-20240819160359910" style="zoom: 60%;" />

一般我们的`C/C++`项目都是使用`CMake`进行管理的，因此伪装成编译器的可行性更大一些，也和前面的劫持`gcc/g++`命令似乎有着异曲同工之妙。

> 留意第一个方案，文章提到使用包管理安装的 CCache ，没有配置环境变量，但是使用命令行能正常使用，原因就是因为包管理器能找到其可执行文件对应的**符号链接**，这也是原文提到的 shim executable，更深入可以参考文章[https://docs.chocolatey.org/en-us/features/shim/](https://docs.chocolatey.org/en-us/features/shim/)。在这种情况下，拷贝下来的是一个符号链接而不是可执行文件本体，在后续的使用中会出现问题。如果手动配置的话也可能出现环境变量冲突的问题。这也就解释了前文提到的一定要手动去官网下载而不使用包管理器。

## 在CMake中配置

文章中给出了在`VS`和`CMake`的两种配置方法，当然这里采用`CMake`的方式，官网也给出了代码：

<img src="https://image.davidingplus.cn/images/2025/01/31/image-20240819160710904.png" alt="image-20240819160710904" style="zoom:60%;" />

这个配置的含义和方案一是一样的，将`CCache`可执行文件拷贝到`CMake`的构建目录，即`build`，然后将`ccache.exe`重命名为`cl.exe`。`cl.exe`即为`MSVC`编译器的可执行文件的名称，即做到了伪装编译器的功能。

留意到原文中注释的提醒，打开`/Z7`编译参数并做了一些小修改，最终的`CMake`配置应该是这样的，这样无论有无`CCache`，配置都能正常进行。

```cmake
# configure compiler MSVC with Ccache
add_compile_options ("/Z7")

find_program (CCACHE_EXECUTABLE ccache)
if (CCACHE_EXECUTABLE)
    message ("-- Ccache executable found at: ${CCACHE_EXECUTABLE}")
    file (COPY "${CCACHE_EXECUTABLE}" DESTINATION "${PROJECT_BINARY_DIR}")
    file (RENAME "${PROJECT_BINARY_DIR}/ccache.exe" "${PROJECT_BINARY_DIR}/cl.exe")
    set (CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>")
    set (CMAKE_VS_GLOBALS
        "CLToolExe=cl.exe"
        "CLToolPath=${PROJECT_BINARY_DIR}"
        "TrackFileAccess=false"
        "UseMultiToolTask=true"
        "DebugInformationFormat=OldStyle"
    )

else ()
    message ("-- CCache not found.")

endif ()
```

# 体验CCache编译缓存

`CCache`提供了`-s`或`--show-stats`命令参数对缓存使用情况进行统计，我们可以借助其来监控`CCache`的使用情况。例如：

```bash
ccache -s

# Cacheable calls:    3936 / 4719 (83.41%)
#   Hits:             2522 / 3936 (64.08%)
#     Direct:         2505 / 2522 (99.33%)
#     Preprocessed:     17 / 2522 ( 0.67%)
#   Misses:           1414 / 3936 (35.92%)
# Uncacheable calls:   783 / 4719 (16.59%)
# Local storage:
#   Cache size (GiB):  0.1 / 10.0 ( 1.13%)
#   Hits:             2522 / 3936 (64.08%)
#   Misses:           1414 / 3936 (35.92%)
```

在刚配置好`CCache`的机器上进行初次构建，由于缓存尚未建立，编译速度并未有所提升，但此时编译缓存已经慢慢建立。第一次执行`ccache -s`可能看见`Cacheable calls`为零，但随着后续的编译操作进行，将产生越来越多的缓存命中，再执行`ccache -s`就可看见变化，同时可以明显感知到编译速度的提升。

由于编译缓存归于`CCache`管理，因此即使你完全删除了构建目录（在`build`里面`rm -rf *`之类的），下一次`make`的时候同样可以享受编译缓存带来的加速体验。以上便是`CCache`基本的本地使用方式。事实上`CCache`还支持网络使用、跨用户共享等高级功能，这超出了本文的介绍范围。关于`CCache`的高级使用，还请参阅官方文档。

# 参考文档

1. [CCache 官网](https://ccache.dev/)

2. [MS-Visual-Studio](https://github.com/ccache/ccache/wiki/MS-Visual-Studio)

3. [Executable shimming (like symlinks but better)](https://docs.chocolatey.org/en-us/features/shim/)

