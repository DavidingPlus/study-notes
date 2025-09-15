---
title: 在 X11 下使用 cairo 引擎绘制图形
categories:
  - 实习
  - 合迅科技
  - 课题研究
abbrlink: '70513923'
date: 2024-07-02 18:30:00
updated: 2024-07-16 16:30:00
---

<meta name="referrer" content="no-referrer"/>

`cairo`是一个方便和高性能的第三方`C`库。它可以作为绘制引擎，帮助我们绘制各种图形，并且提供多种输出方式。本文将在`Linux`下结合`X11`图形显示协议绘制简单的图形。

这是效果图：

<img src="https://cdn.davidingplus.cn/images/2025/02/01/59cb53826fcba1602fd3769171f5f127.png" alt="59cb53826fcba1602fd3769171f5f127" style="zoom:67%;" />

<!-- more -->

# 安装Cairo库

`cairo`的官方网站是[https://cairographics.org](https://cairographics.org/)。上面对`cairo`图形库做了一个完整的介绍，包括如何下载、`API`接口、示例等等。

## 通过包管理器安装

通过官方文档知道，在`Linux`下可以直接通过包管理器进行下载，以`Ubuntu`为例。

```bash
sudo apt install libcairo2-dev
```

下载好以后头文件和动态库就安装好了。头文件安装在`/usr/include/cairo/`中，静态库和动态库分别位于`/usr/lib/x86_64-linux-gnu/libcairo.a`和`/usr/lib/x86_64-linux-gnu/libcairo.so`。因此能直接被系统识别，直接引入头文件，编译的时候链接`cairo`库即可。

## 通过Conan安装

对于个人而言，`apt`安装自然是非常友好的。但是对于`LarkSDK`这样一个面向用户的基础框架而言，除非最基本的系统库例如`X`窗口系统`libx11-dev`，`Wayland`窗口系统`libwayland-dev`和`Wayland`键盘处理`libxkbcommon-dev`等，其他的第三方库均最好不以`apt`包的方式引入。鉴于`C++`没有自己的包管理器，因此需要借助第三方的包管理器，例如`conan`，`cpm`等。本项目使用`conan`管理第三方包。

`conan`是一个`python`语言编写的`C++`的包管理器，官方网址是[https://conan.io/](https://conan.io/)。`conan`管理了众多第三方的`C++`库，通过命令行操作就能方便的在自己的项目中引入`conan`包。需要通过`conanfile.py`或者`conanfile.txt`进行配置，当然这不是本文的重点，具体见文档[https://docs.conan.io/2/](https://docs.conan.io/2/)。

`conan`官方提供了自己的`conan`仓库，[https://conan.io/center](https://conan.io/center)。在上面能找到很多我们熟知的第三方库，例如`gtest`，`qt`，`boost`，`fmt`等。当然还有一些更基础的工具库，这里不赘述。当然[cairo](https://conan.io/center/recipes/cairo)库也在其中。

### 尝试安装Cairo

现在让我们尝试安装`cairo`库并尝试用`CMake`将其引入。在新开的项目中创建`conanfile.txt`，引入项目依赖`cairo`。使用`conanfile.py`主要用于生成和发布自己的`conan`包，`conan`提供了丰富的选项供用户操作。这里只是为了测试，因此使用最简单的`conanfile.txt`即可。

```ini
[requires]
cairo/1.18.0

[generators]
cmake
```

编写自己的`CMakeLists.txt`，配置项目的相关信息，引入`conan`的部分类似如下：

```cmake
...

include (${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup (NO_OUTPUT_DIRS)

...

add_executable (xxx
    ...
)
target_link_libraries (xxx ${CONAN_LIBS})
```

之后执行一般的构建流程即可。

```bash
conan install ..
cmake ..
make # windows 下默认没有 make 命令，使用 cmake --build ./ 代替
```

`conan`官方的包很多，很全，但是`conan`本身还有很多`bug`，单就`cairo`包的使用过程中就有很多问题。最典型的，执行`conan install ..`可能会失败，并且遇到很多错误。因此我们需要了解`conan`包是个什么东西，才能明确问题是如何形成的。

### 关于Conan包

现在对`conan`包做一个简述。对于`C++`库而言，为了让用户方便的使用，把`.h`和`.cpp`代码全部打包出去是不合适的，这些代码不应该在用户的机器上再被编译一次，而应该在需要用的时候被直接使用。因此需要通过库的方式进行发布，也就是使用静态库和动态库。在发布的包中，最重要的文件就是头文件和库文件。当然可能会携带一些其他必要的文件，例如资源文件、版本说明文件等。

我们都知道，编译`C/C++`的代码需要依赖于`C++`和编译器的版本。进一步的，由于`C/C++`是非常接近底层的代码，虽然标准库是跨平台的，但是如果需要写平台相关的程序，还需要注意操作系统的版本。因此，对于同一个版本的`conan`包，不同的系统，不同的编译环境，是静态库还是动态库，是`Debug`包还是`Release`包，甚至依赖包的版本，都可能对最后的编译造成一定影响。在本地的`conan`配置中会保存相关的这些信息，在`conan`拉包的时候会匹配本地的配置拉取合适的包。配置文件默认位于`~/.conan/profile/default`中。

在`Linux`下的配置类似于这样，看其参数很明显就能知道对应的含义。

```ini
[settings]
os=Linux
os_build=Linux
arch=x86_64
arch_build=x86_64
compiler=gcc
compiler.version=9
compiler.libcxx=libstdc++11
build_type=Release
[options]
[build_requires]
[env]
```

在`Windows`下类似于这样：

```ini
[settings]
os=Windows
os_build=Windows
arch=x86_64
arch_build=x86_64
compiler=Visual Studio
compiler.version=16
build_type=Release
[options]
[build_requires]
[env]
```

现在我们以`cairo/1.18.0@`包为例，使用`conan`命令查看其远端的包的列表。

```bash
conan search cairo/1.18.0@ -r conancenter
```

得到的结果大致是这样：

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240715163723130.png" alt="image-20240715163723130" style="zoom:55%;" />

我们发现`settings`中的内容和我们的`profiles/default`中的内容对应，这就是前面提到的匹配。例如图中是一个`Mac`下的`apple-clang`的`13.0`版本的`静态库`的`Debug`包。每个包的`options`，`settings`和`requires`都会对最前面的`Package_ID`产生影响，这是一个哈希计算值，具体如何影响和生成请参考[https://docs.conan.io/2/reference/binary_model/package_id.html](https://docs.conan.io/2/reference/binary_model/package_id.html)。当然这其中也有令人费解的地方，请见下文。

### 解决错误（并吐槽）

前面提到，`conan`官方的包有问题，会导致在安装的时候出现错误。例如，我在安装的出现的错误如下：

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240715165238889.png" alt="image-20240715165238889" style="zoom:55%;" />

错误信息告诉我`fontconfig`的`2.15.0`的版本需要`conan 1.60.4`以上才能安装。由于公司使用的是`conan 1.60.1`，首先我想到的是`conan`版本不正确。深入研究后，我发现的问题出乎我的意料。

由于公司实际开发的`conan`版本不可能从`1.60.1`升到`1.60.4`，要升肯定一步到位到`conan 2.0`了。首先查看`conancenter`中`cairo/1.18.0@`远端的包，检索符合当前系统和编译环境的。我发现了三个长的几乎一样的包：

```ini
; package 1
Package_ID: 703bcc640002869a53960c4449d3825ff8a103e6
    [options]
        fPIC: True
        shared: False
        tee: False
        with_fontconfig: True
        with_freetype: True
        with_glib: True
        with_lzo: True
        with_png: True
        with_symbol_lookup: False
        with_xcb: True
        with_xlib: True
        with_xlib_xrender: True
        with_zlib: True
    [settings]
        arch: x86_64
        build_type: Release
        compiler: gcc
        compiler.version: 9
        os: Linux
    [requires]
        brotli/1.1.0:b21556a366bf52552d3a00ce381b508d0563e081
        bzip2/1.0.8:da606cf731e334010b0bf6e85a2a6f891b9f36b0
        expat/2.6.0:c215f67ac7fc6a34d9d0fb90b0450016be569d86
        fontconfig/2.15.0:b172ac37518ca059ccac0be9c3eb29e5179ecf1e
        freetype/2.13.2:f1014dc4f9380132c471ceb778980949abf136d3
        glib/2.78.3:06c63123a2bb8b6d3ea5dcae501525df32efb7b5
        libelf/0.8.13:6af9cc7cb931c5ad942174fd7838eb655717c709
        libffi/3.4.4:6af9cc7cb931c5ad942174fd7838eb655717c709
        libmount/2.39:6af9cc7cb931c5ad942174fd7838eb655717c709
        libpng/1.6.43:7929d8ecf29c60d74fd3c1f6cb78bbb3cb49c0c7
        libselinux/3.5:6b0384e3aaa343ede5d2bd125e37a0198206de42
        lzo/2.10:6af9cc7cb931c5ad942174fd7838eb655717c709
        pcre2/10.42:647f8233073b10c84d51b1833c74f5a1cb8e8604
        pixman/0.43.4:6af9cc7cb931c5ad942174fd7838eb655717c709
        util-linux-libuuid/2.39.2:6af9cc7cb931c5ad942174fd7838eb655717c709
        xorg/system:5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9
        zlib/1.3.1:6af9cc7cb931c5ad942174fd7838eb655717c709
    Outdated from recipe: True

; package 2
Package_ID: 8098347825649d9fd3e21c49992446a2a2193ad4
    [options]
        fPIC: True
        shared: False
        tee: False
        with_fontconfig: True
        with_freetype: True
        with_glib: True
        with_lzo: True
        with_png: True
        with_symbol_lookup: False
        with_xcb: True
        with_xlib: True
        with_xlib_xrender: True
        with_zlib: True
    [settings]
        arch: x86_64
        build_type: Release
        compiler: gcc
        compiler.version: 9
        os: Linux
    [requires]
        brotli/1.1.0:b21556a366bf52552d3a00ce381b508d0563e081
        bzip2/1.0.8:da606cf731e334010b0bf6e85a2a6f891b9f36b0
        expat/2.5.0:c215f67ac7fc6a34d9d0fb90b0450016be569d86
        fontconfig/2.14.2:b172ac37518ca059ccac0be9c3eb29e5179ecf1e
        freetype/2.13.0:f1014dc4f9380132c471ceb778980949abf136d3
        glib/2.78.0:06c63123a2bb8b6d3ea5dcae501525df32efb7b5
        libelf/0.8.13:6af9cc7cb931c5ad942174fd7838eb655717c709
        libffi/3.4.4:6af9cc7cb931c5ad942174fd7838eb655717c709
        libmount/2.39:6af9cc7cb931c5ad942174fd7838eb655717c709
        libpng/1.6.40:7929d8ecf29c60d74fd3c1f6cb78bbb3cb49c0c7
        libselinux/3.5:6b0384e3aaa343ede5d2bd125e37a0198206de42
        lzo/2.10:6af9cc7cb931c5ad942174fd7838eb655717c709
        pcre2/10.42:647f8233073b10c84d51b1833c74f5a1cb8e8604
        pixman/0.40.0:6af9cc7cb931c5ad942174fd7838eb655717c709
        util-linux-libuuid/2.39:6af9cc7cb931c5ad942174fd7838eb655717c709
        xorg/system:5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9
        zlib/1.3:6af9cc7cb931c5ad942174fd7838eb655717c709
    Outdated from recipe: True
        
; package 3
Package_ID: a336bac291d8ec6a55c6257f3266f9a8760c7403
    [options]
        fPIC: True
        shared: False
        tee: False
        with_fontconfig: True
        with_freetype: True
        with_glib: True
        with_lzo: True
        with_png: True
        with_symbol_lookup: False
        with_xcb: True
        with_xlib: True
        with_xlib_xrender: True
        with_zlib: True
    [settings]
        arch: x86_64
        build_type: Release
        compiler: gcc
        compiler.version: 9
        os: Linux
    [requires]
        brotli/1.1.0:b21556a366bf52552d3a00ce381b508d0563e081
        bzip2/1.0.8:da606cf731e334010b0bf6e85a2a6f891b9f36b0
        expat/2.5.0:c215f67ac7fc6a34d9d0fb90b0450016be569d86
        fontconfig/2.14.2:b172ac37518ca059ccac0be9c3eb29e5179ecf1e
        freetype/2.13.2:f1014dc4f9380132c471ceb778980949abf136d3
        glib/2.78.1:06c63123a2bb8b6d3ea5dcae501525df32efb7b5
        libelf/0.8.13:6af9cc7cb931c5ad942174fd7838eb655717c709
        libffi/3.4.4:6af9cc7cb931c5ad942174fd7838eb655717c709
        libmount/2.39:6af9cc7cb931c5ad942174fd7838eb655717c709
        libpng/1.6.40:7929d8ecf29c60d74fd3c1f6cb78bbb3cb49c0c7
        libselinux/3.5:6b0384e3aaa343ede5d2bd125e37a0198206de42
        lzo/2.10:6af9cc7cb931c5ad942174fd7838eb655717c709
        pcre2/10.42:647f8233073b10c84d51b1833c74f5a1cb8e8604
        pixman/0.42.2:6af9cc7cb931c5ad942174fd7838eb655717c709
        util-linux-libuuid/2.39.2:6af9cc7cb931c5ad942174fd7838eb655717c709
        xorg/system:5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9
        zlib/1.3:6af9cc7cb931c5ad942174fd7838eb655717c709
    Outdated from recipe: True
```

这三个包的唯一区别在于`requires`不同，第一个包需要`expat/2.6.0`，第二个和第三个包需要`expat/2.5.0`。第二个包需要`freetype/2.13.0`，第三个包`freetype/2.13.2`。这就是唯一区别，然后生成了三个不同的哈希值，对应不同的`package`。

那么问题来了，`conan install`默认会读取本地的配置，但是本地配置不可能指定`requires`啊。不同的项目可能用的版本不同，这是很正常的事情，也不应该是由用户承担责任的地方。其次，`conan install`不能指定`package_id`下载，因此下载的是哪一个包，完全就看他怎么想了。非常不幸运的是，我需要第二个包，但是下载只能下载到第一个包。

那么如何解决这个问题呢？幸运的是，`conan`提供了`conan download`的方法，这个东西可以指定`package_id`进行下载。那就简单了，先把`cairo`本包下载到本地，然后再指定这一套流程，由于本地有缓存，会跳过去远端拉取`cairo`包的这一步，这样所有本包和所有依赖不就顺利拉取下来了吗？说干就干。

```bash
conan download cairo/1.18.0@:8098347825649d9fd3e21c49992446a2a2193ad4 -r conancenter
```

成功下载下来以后，再次执行`conan install ..`，又出问题了。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240715172400074.png" alt="image-20240715172400074" style="zoom:58%;" />

不对啊，这个包依赖的`fontconfig`的版本是`2.14.2`啊，为什么这里还是下载的`2.15.0`啊？为了解决这个问题，我打开了对应的`conanfile.py`，阅读到`requirements()`函数的时候，豁然开朗。

```python
def requirements(self):
    self.requires("pixman/0.43.4")
    if self.options.with_zlib and self.options.with_png:
        self.requires("expat/[>=2.6.2 <3]")
    if self.options.with_lzo:
        self.requires("lzo/2.10")
    if self.options.with_zlib:
        self.requires("zlib/[>=1.2.11 <2]")
    if self.options.with_freetype:
        self.requires("freetype/2.13.2", transitive_headers=True, transitive_libs=True)
    if self.options.with_fontconfig:
        self.requires("fontconfig/2.15.0", transitive_headers=True, transitive_libs=True)
    if self.options.with_png:
        self.requires("libpng/[>=1.6 <2]")
    if self.options.with_glib:
        self.requires("glib/2.78.3")
    if self.settings.os in ["Linux", "FreeBSD"]:
        if self.options.with_xlib or self.options.with_xlib_xrender or self.options.with_xcb:
            self.requires("xorg/system", transitive_headers=True, transitive_libs=True)
    if self.options.get_safe("with_opengl") == "desktop":
        self.requires("opengl/system", transitive_headers=True, transitive_libs=True)
        if self.settings.os == "Windows":
            self.requires("glext/cci.20210420")
            self.requires("wglext/cci.20200813")
            self.requires("khrplatform/cci.20200529")
    if self.options.get_safe("with_opengl") and self.settings.os in ["Linux", "FreeBSD"]:
        self.requires("egl/system", transitive_headers=True, transitive_libs=True)
```

不是哥们，`expat`需要的是`2.5.0`，但是你规定依赖的包是`>=2.6.2 and <3`。哈？这不前后矛盾吗？对于`freetype`和`fontconfig`也是相同的问题。同时，我们再浏览一下`cairo`包拉下来以后的整体结构。奥，原来同一个版本的所有包的`conanfile.py`都是同一个文件。好，这没有任何问题，这是`conan`的设计。但是你自己不更新依赖版本的限制是什么意思，如果硬要不改的话，发布不同的版本也是`ok`的啊。这样一套流程下来，导致`conan`拉包就出现了问题。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240715172916780.png" alt="image-20240715172916780" style="zoom:67%;" />

那么如何解决呢？公司这边的解决方式是将该版本的`cairo`包上传到公司的`conan`服务器上，并手动修改`conanfile.py`使其版本匹配，并将所有的依赖以及穿透依赖全部拷贝到公司服务器上。至此，`cairo`终于成功通过`conan`安装。

`Windows`那边也是同样的问题，同样操作即可。

### 补充

在`Linux`下的`Cairo`包中，有一个依赖项值得看一下，即`xorg/system`。这个包的版本是`system`，意思是和系统相关的包，这里是`Linux`下`X`图形协议相关的依赖。众所周知，系统包是通过`apt`或者`yum`进行安装的，意思是`conan`能够调用这些包命令来帮我们自动安装对应的包吗？

答案是可以的，参见文档[https://docs.conan.io/2/reference/tools/system/package_manager.html](https://docs.conan.io/2/reference/tools/system/package_manager.html)。留意这段代码：

```python
def system_requirements(self):
    apt = package_manager.Apt(self)
    apt.install(["libx11-dev", "libx11-xcb-dev", "libfontenc-dev", "libice-dev", "libsm-dev", "libxau-dev", "libxaw7-dev",
                 "libxcomposite-dev", "libxcursor-dev", "libxdamage-dev", "libxdmcp-dev", "libxext-dev", "libxfixes-dev",
                 "libxi-dev", "libxinerama-dev", "libxkbfile-dev", "libxmu-dev", "libxmuu-dev",
                 "libxpm-dev", "libxrandr-dev", "libxrender-dev", "libxres-dev", "libxss-dev", "libxt-dev", "libxtst-dev",
                 "libxv-dev", "libxxf86vm-dev", "libxcb-glx0-dev", "libxcb-render0-dev",
                 "libxcb-render-util0-dev", "libxcb-xkb-dev", "libxcb-icccm4-dev", "libxcb-image0-dev",
                 "libxcb-keysyms1-dev", "libxcb-randr0-dev", "libxcb-shape0-dev", "libxcb-sync-dev", "libxcb-xfixes0-dev",
                 "libxcb-xinerama0-dev", "libxcb-dri3-dev", "uuid-dev", "libxcb-cursor-dev", "libxcb-dri2-0-dev",
                 "libxcb-dri3-dev", "libxcb-present-dev", "libxcb-composite0-dev", "libxcb-ewmh-dev",
                 "libxcb-res0-dev"], update=True, check=True)
    apt.install_substitutes(
        ["libxcb-util-dev"], ["libxcb-util0-dev"], update=True, check=True)

    yum = package_manager.Yum(self)
    yum.install(["libxcb-devel", "libfontenc-devel", "libXaw-devel", "libXcomposite-devel",
                       "libXcursor-devel", "libXdmcp-devel", "libXtst-devel", "libXinerama-devel",
                       "libxkbfile-devel", "libXrandr-devel", "libXres-devel", "libXScrnSaver-devel",
                       "xcb-util-wm-devel", "xcb-util-image-devel", "xcb-util-keysyms-devel",
                       "xcb-util-renderutil-devel", "libXdamage-devel", "libXxf86vm-devel", "libXv-devel",
                       "xcb-util-devel", "libuuid-devel", "xcb-util-cursor-devel"], update=True, check=True)

    dnf = package_manager.Dnf(self)
    dnf.install(["libxcb-devel", "libfontenc-devel", "libXaw-devel", "libXcomposite-devel",
                       "libXcursor-devel", "libXdmcp-devel", "libXtst-devel", "libXinerama-devel",
                       "libxkbfile-devel", "libXrandr-devel", "libXres-devel", "libXScrnSaver-devel",
                       "xcb-util-wm-devel", "xcb-util-image-devel", "xcb-util-keysyms-devel",
                       "xcb-util-renderutil-devel", "libXdamage-devel", "libXxf86vm-devel", "libXv-devel",
                       "xcb-util-devel", "libuuid-devel", "xcb-util-cursor-devel"], update=True, check=True)

    zypper = package_manager.Zypper(self)
    zypper.install(["libxcb-devel", "libfontenc-devel", "libXaw-devel", "libXcomposite-devel",
                          "libXcursor-devel", "libXdmcp-devel", "libXtst-devel", "libXinerama-devel",
                          "libxkbfile-devel", "libXrandr-devel", "libXres-devel", "libXss-devel",
                          "xcb-util-wm-devel", "xcb-util-image-devel", "xcb-util-keysyms-devel",
                          "xcb-util-renderutil-devel", "libXdamage-devel", "libXxf86vm-devel", "libXv-devel",
                          "xcb-util-devel", "libuuid-devel", "xcb-util-cursor-devel"], update=True, check=True)

    pacman = package_manager.PacMan(self)
    pacman.install(["libxcb", "libfontenc", "libice", "libsm", "libxaw", "libxcomposite", "libxcursor",
                          "libxdamage", "libxdmcp", "libxtst", "libxinerama", "libxkbfile", "libxrandr", "libxres",
                          "libxss", "xcb-util-wm", "xcb-util-image", "xcb-util-keysyms", "xcb-util-renderutil",
                          "libxxf86vm", "libxv", "xcb-util", "util-linux-libs", "xcb-util-cursor"], update=True, check=True)

    package_manager.Pkg(self).install(["libX11", "libfontenc", "libice", "libsm", "libxaw", "libxcomposite", "libxcursor",
                       "libxdamage", "libxdmcp", "libxtst", "libxinerama", "libxkbfile", "libxrandr", "libxres",
                       "libXScrnSaver", "xcb-util-wm", "xcb-util-image", "xcb-util-keysyms", "xcb-util-renderutil",
                       "libxxf86vm", "libxv", "xkeyboard-config", "xcb-util", "xcb-util-cursor"], update=True, check=True)
```

对于`Ubuntu`来讲，`conan`能手动帮我们调用`apt`安装所需要的系统依赖。需要注意一点，需要在配置文件中加上两句，来指明开启这个功能和使用`sudo`。所以最终的配置文件类似如下：

```ini
[settings]
os=Linux
os_build=Linux
arch=x86_64
arch_build=x86_64
compiler=gcc
compiler.version=9
compiler.libcxx=libstdc++11
build_type=Release
[options]
[build_requires]
[env]
[conf]
tools.system.package_manager:mode=install
tools.system.package_manager:sudo=True
```

至此，我们成功通过`conan`安装下来了`cairo`，看到成功的结果，我的内心无比兴奋。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240715175839407.png" alt="image-20240715175839407" style="zoom:60%;" />

# 使用Cairo库绘制图形

安装完`cairo`库，是时候使用它绘制一些简单的图形了。

## Linux GUI背景简述

`Linux`本身是不带有图形界面的，真正原生的`Linux`系统只是一个基于命令行的操作系统。但是我们目前所使用的`Linux`发行版，例如`Ubuntu`、`Centos`等，都是有图形界面的啊。这是因为这些图形界面是`Linux`下的一个应用程序而已，是通过程序和协议模拟和实现出来的，或者说图形界面并不属于`Linux`内核的一部分。这一点和`Windows`系统完全不一样，`Windows`的命令行在我看来完全不如`Linux`这么好用，甚至有时候我会爆粗口喷他，但是`Windows`的用户的依然最多。为什么呢？就是因为`GUI`好看。`Windows`的图形界面是操作系统的一部分，并且做的确实好看和丝滑。这样变相的降低了用户的学习和使用成本，对大多数的人而言是件好事。但是对于程序员特别是偏向底层的程序员来讲，却真不一定。

在`Linux`下，需要通过应用程序实现图形界面，那就需要设计一个合适的协议。目前市面上比较流行的有两种协议，`X`协议和`Wayland`协议。这两种协议都是基于网络通信的思想，将图形显示分为了客户端（即你的应用程序）和服务端通信的过程。输入设备和显示设备不是同一个设备，而且他们需要相互配合，进行画面显示，所以需要一个交互协议，建立他们直接的沟通桥梁。当然`X`协议和`Wayland`协议的细节有所区别，粗略的讲是`server`和`compositor`的设计不同，具体可见[https://www.secjuice.com/wayland-vs-xorg/](https://www.secjuice.com/wayland-vs-xorg/)。

本文以及后续以`X`协议为例展开，并以`XClient`和`XServer`分别代指客户端和服务端。例如现在我需要画一个圆，`XClient`需要告诉`XServer`在屏幕的什么地方，使用什么颜色，画多大的一个圆。至于这个圆如何生成，如何使用硬件真正绘制图形等等这些操作，都是由`XServer`完成的。当然更进一步的，`XServer`还可以捕捉鼠标和键盘的动作，会触发相应的事件。`XClient`可以接受相应的事件并且完成相应的逻辑。这就是整个`X`协议以及绘制逻辑的简要概括。

`X`协议有很多实现。目前用的最多的是`XOrg`，对应的`XClient`有`Xlib`和`XCB`的两种实现，提供了和`XServer`对接的`API`。（`At the bottom level of the X client library stack are Xlib and XCB, two helper libraries (really sets of libraries) that provide API for talking to the X server.`）本文的背景是`X11`，也是`Xlib`库的一个特殊版本。

上面只是非常粗略的说明了一下基本思路，更多文档请参考：

1. [X.Org](https://www.x.org/wiki/)
2. [Wayland](https://wayland.freedesktop.org/)
3. [The X New Developer’s Guide: Xlib and XCB](https://www.x.org/wiki/guide/xlib-and-xcb/)

## 安装X11并编写GUI程序

强烈建议在虚拟机下进行，因为`Wsl`需要内核更新到`2.2.4`以后才能使用最新功能的`GUI`，而且体验还不是很好。

以`Ubuntu`为例，系统默认是未安装`X11`库的（`XServer`肯定是有的，不然你怎么看得到图形界面呢？安装`X11`只是提供了窗口系统的开发支持）。因此我们使用如下命令手动安装。值得一提的是，由于`X11`是系统库，因此使用`apt`包安装即可。同理对于`Wayland`也是一样。

```bash
sudo apt install libx11-dev
```

安装完成以后，使用`CMake`就能很方便的引入`X11`支持了。

```cmake
add_executable (xxx
    ...
)
target_link_libraries (xxx X11)
```

现在我们编写一个样例程序，用于在`X11`下绘制一个图形窗口。这里面涉及到很多的`X11`的`API`，本文只做简单介绍，具体请参考文档[https://www.x.org/releases/X11R7.7/doc/libX11/libX11/libX11.html](https://www.x.org/releases/X11R7.7/doc/libX11/libX11/libX11.html)

```cpp
#include <X11/Xlib.h>

#include <iostream>


int main()
{
    // 用于和 XServer 建立连接，dpy 指针全局只有一份
    Display *dpy = XOpenDisplay(nullptr);
    if (!dpy)
    {
        std::cerr << "Unable to open X display!" << std::endl;
        throw;
    }

    int screen = DefaultScreen(dpy);
    // 创建一个顶层窗口
    Window w = XCreateSimpleWindow(
        dpy,
        RootWindow(dpy, DefaultScreen(dpy)),
        100, 100, 400, 300, 0,
        BlackPixel(dpy, screen),
        BlackPixel(dpy, screen)
    );
    // 将需要检测的事件绑定在窗口上
    XSelectInput(dpy, w, ExposureMask);
    // 展示这个窗口
    XMapWindow(dpy, w);

    std::cout << "Entering loop ..." << std::endl;

    // 进入事件循环
    XEvent e;
    while (1)
    {
        XNextEvent(dpy, &e);
        switch (e.type)
        {
            case Expose:
                std::cout << "event: Expose" << std::endl;
                break;
            default:
                std::cout << "event: " << e.type << std::endl;
                break;
        }
    }

    return 0;
}
```

接口的大致功能以在代码注释中体现。注意到整个程序最重要的架构是最后的这个事件循环，当`XClient`窗口成功创建出来以后，`XServer`需要不断监听`XClient`发送的事件并予以处理，这样才能实现`GUI`程序的功能。因此事件循环的存在就自然而然了。有点类似于`IO`多路复用中`epoll`技术的框架。在这里监听的是`Expose`事件，不用具体关心这个语义是什么意思，在窗口创建和窗口大小发生改变的时候会触发`Expose`事件，这里也是用作信息打印测试。

程序的运行结果如下，可以发现窗口创建和改变窗口大小的时候在不断打印`Expose`的信息。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240716111623175.png" alt="image-20240716111623175" style="zoom:67%;" />

对于事件循环这个概念，不管是`Qt`，还是`LarkSDK`，还是对于一个跨平台的`GUI`框架，事件循环显然是必不可少的。问题在于跨平台需要统一不同平台下的窗体系统和事件处理等逻辑，最终抽象出跨平台的接口，这就是这些框架正在做最重要的一件事情。以`LarkSDK`为例，虽然最简单的跨平台的程序是四行就能搞定，但是其中涉及到的知识和背景是非常庞大的。

```cpp
#include <lwindowapplication.h>
#include <lwindow.h>

int main()
{
    LWindowApplication app; // 创建窗体程序主框架实例
    LWindow w; // 创建顶层窗体
    w.show(); // 让窗体可见
    return app.exec(); // 进入主事件循环
}
```

## 尝试引入Cairo

`cairo`将输出和绘制的概念做了严格区分。`cairo surface`是一个抽象出来的概念，与其对接的是多种输出方式，例如`PDF`、`PNG`、`SVG`、`Win32`、`XLib`、`XCB`等，如图所示。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240716141915019.png" alt="image-20240716141915019" style="zoom:75%;" />

接着我们查看`cairo`官方提供的[samples](https://cairographics.org/samples/)，可以发现，官方提供的样例好像完全和`surface`没有关系，换句话说没有反映输出的方式。例如这段代码：

```cpp
double xc = 128.0;
double yc = 128.0;
double radius = 100.0;
double angle1 = 45.0  * (M_PI/180.0);  /* angles are specified */
double angle2 = 180.0 * (M_PI/180.0);  /* in radians           */

cairo_set_line_width (cr, 10.0);
cairo_arc (cr, xc, yc, radius, angle1, angle2);
cairo_stroke (cr);

/* draw helping lines */
cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
cairo_set_line_width (cr, 6.0);

cairo_arc (cr, xc, yc, 10.0, 0, 2*M_PI);
cairo_fill (cr);

cairo_arc (cr, xc, yc, radius, angle1, angle1);
cairo_line_to (cr, xc, yc);
cairo_arc (cr, xc, yc, radius, angle2, angle2);
cairo_line_to (cr, xc, yc);
cairo_stroke (cr);
```

它的绘制结果是这样的：

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240716142350311.png" alt="image-20240716142350311" style="zoom:85%;" />

这个图样可以被输出到前面提到的任意一种`surface`中。同时这也是我想说的，`cairo`将输出和绘制的概念做了完整区分，同样这也是我们容易想到和愿意看到的。所以，如果想要创建一个输出到`PNG`中的实例，代码应该类似如下：

```cpp
// 创建 surface
cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 640, 480);
// 创建绘图上下文 context
cairo_t *cr = cairo_create(surface);

// 绘制逻辑，只和 context 相关，与 surface 无关
...

// 输出到 png 格式中
cairo_surface_write_to_png(surface, "xxx.png");
```

至此，我们用`surface`和`context`来代指输出和绘制的概念，也明白了`cario`是如何区分这两个概念的了。`surface`用作指明绘制的图形输出到哪里，`context`则用于进行绘制。读者可以尝试编写一个完整的程序输出上面的图案到一张图片文件中。

## Cairo和X11相结合

使用`cairo`成功输出到图片文件中以后，试着想想如何与`X11`窗口系统结合了。提前声明，用到的`surface`只有`XLib Surface`和`Image Surface`，其他的`API`请自行查询[文档](https://cairographics.org/manual/)。

### XLib Surface

首先想到的肯定是`XLib Surface`。这代表`cairo`帮我做好了与`X11`平台的对接工作，我只需要按部就班地使用`cairo`的`API`即可。所有的`surface`基本上只有在创建的时候会有区别，在`context`那一层的绘制几乎没有区别。例如下面就是`XLib Surface`的创建`API`，参数具体含义请参考[官方文档](https://cairographics.org/manual/cairo-XLib-Surfaces.html)。

```cpp
cairo_surface_t *
cairo_xlib_surface_create (Display *dpy,
                           Drawable drawable,
                           Visual *visual,
                           int width,
                           int height);
```

查询该方法需要的接口如何获取以后，编写出如下的代码：

```cpp
#include <iostream>
#include <exception>

#include <X11/Xlib.h>

#include "cairo.h"
#include "cairo/cairo-xlib.h"


int main()
{
    Display *dpy = XOpenDisplay(nullptr);
    if (!dpy)
    {
        throw std::runtime_error("Failed to open X display");
    }

    int screen = DefaultScreen(dpy);
    Window w = XCreateSimpleWindow(
        dpy,
        RootWindow(dpy, DefaultScreen(dpy)),
        100, 100, 640, 480, 0,
        BlackPixel(dpy, screen),
        BlackPixel(dpy, screen));
    XSelectInput(dpy, w, ExposureMask);

    XMapWindow(dpy, w);

    std::cout << "Entering loop ..." << std::endl;

    // 根据 window id 获取该窗口的信息
    XWindowAttributes attr;
    XGetWindowAttributes(dpy, w, &attr);

    // 创建 XLib Surface
    cairo_surface_t *surface = cairo_xlib_surface_create(dpy, w, attr.visual, attr.width, attr.height);
    cairo_t *cr = cairo_create(surface);

    XEvent e;
    while (true)
    {
        XNextEvent(dpy, &e);

        switch (e.type)
        {
            case Expose:
            {
                std::cout << "event: Expose" << std::endl;

                // 绘制操作
                cairo_set_source_rgb(cr, 1.0, 1.0, 0.5);
                cairo_paint(cr);

                cairo_set_source_rgb(cr, 1.0, 0.0, 1.0);
                cairo_move_to(cr, 100, 100);
                cairo_line_to(cr, 200, 200);
                cairo_stroke(cr);

                break;
            }
            default:
                std::cout << "event: " << e.type << std::endl;
                break;
        }
    }

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    XDestroyWindow(dpy, w);
    XCloseDisplay(dpy);


    return 0;
}
```

这样能在`Expose`事件触发的时候成功绘制出文章开始时候展示的图样。

### Image Surface

`XLib Surface`的代码结构看着很像自动挡的感觉，创建`surface`，在事件循环中用`context`进行绘制，最终得到想要的图案。

我们深入思考一下，图形是如何被绘制到屏幕上的呢？前面举了个例子，画一个圆，告诉`XServer`在哪里，用什么颜色，画多大、多宽的圆。至于如何用硬件画不是`XClient`关心的事情，但是如何表示这些信息呢？显然需要用合适的`data`进行存储。进一步讲，`context`调用各种方法的实际过程，其实就是往数据缓冲区`data`中写数据的过程。当类似`flush`操作的被调用以后，这些数据才会真正反映在屏幕上，形成我们观看的效果。

在这样的语义下，我们进一步思考`surface`的概念，其实用**可绘制表面**的概念好像更加贴切（本概念借鉴于`LarkSDK`的`LSurface`）。绘图的数据缓冲区记录了图形的数据，类型是`unsigned char *`，这些数据和不同的输出方式对接就能达到不同的输出效果。至于为什么是`unsigned char *`（我猜测大概是字节流）以及如何对接，这不是本文的重点，有兴趣请自己查询资料。

知道这个过程以后，回到`Image Surface`本身，为什么要使用这个东西，是因为它为我们提供了获取数据的接口。也就是当我用`context`绘制以后，调用这个方法就能立刻拿到缓冲区的数据。

```cpp
unsigned char *
cairo_image_surface_get_data (cairo_surface_t *surface);
```

然后让我们思考一下绘制效率。不同引擎的效率的区别根本上就是在于如何快速的把这些数据计算出来，或者换句话讲，如何快速地让缓冲区的内存填充为指定的数据。比如对于最基本的暴力软渲染和`cairo`引擎，他们的效率差距显然是非常大的。这里有一个例子可以参考，是`LarkSDK`原生软渲染和`cairo`引擎同样绘制`10000`条斜线的效率差距，以下是结果，保守估计至少差了几百到一千倍。

<img src="https://cdn.davidingplus.cn/images/2025/02/01/image-20240716155335456.png" alt="image-20240716155335456" style="zoom:80%;" />

回到正题，再结合`X11`的`API`，我们可以给出使用`Image Surface`的代码：

```cpp
#include <iostream>
#include <exception>

#include <X11/Xlib.h>

#include "cairo.h"


int main()
{
    Display *dpy = XOpenDisplay(nullptr);
    if (!dpy)
    {
        throw std::runtime_error("Failed to open X display");
    }

    int screen = DefaultScreen(dpy);
    Window w = XCreateSimpleWindow(
        dpy,
        RootWindow(dpy, DefaultScreen(dpy)),
        100, 100, 640, 480, 0,
        BlackPixel(dpy, screen),
        BlackPixel(dpy, screen));
    XSelectInput(dpy, w, ExposureMask);

    unsigned long mask = 0;
    XGCValues values;
    GC gc = XCreateGC(dpy, w, mask, &values);

    XMapWindow(dpy, w);

    // 创建 Image Surface
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 640, 480);
    cairo_t *cr = cairo_create(surface);

    // 获得 Cairo 管理的绘图数据的指针
    unsigned char *pData = cairo_image_surface_get_data(surface);
    // 创建 X11 下的 Image Buffer ，将其中的数据替换为 Cairo 的数据指针
    XImage *pBackBuffer = XCreateImage(
        dpy,
        DefaultVisual(dpy, screen),
        DefaultDepth(dpy, screen),
        ZPixmap,
        0,
        (char *)pData,
        640, 480,
        8,
        0);

    std::cout << "Entering loop ..." << std::endl;

    XEvent e;
    while (true)
    {
        XNextEvent(dpy, &e);

        switch (e.type)
        {
            case Expose:
            {
                std::cout << "event: Expose" << std::endl;

                cairo_set_source_rgb(cr, 1.0, 1.0, 0.5);
                cairo_paint(cr);

                cairo_set_source_rgb(cr, 1.0, 0.0, 1.0);
                cairo_move_to(cr, 100, 100);
                cairo_line_to(cr, 200, 200);
                cairo_stroke(cr);

                // flush 操作，刷新缓冲区，更新数据
                cairo_surface_flush(surface);
                
                // X11 下真正绘制图形的方法，用到了外面定义的 X11 Image Buffer，而其内部的数据就是 Cairo 管理的缓冲区数据
                XPutImage(
                    dpy,
                    w,
                    gc,
                    pBackBuffer,
                    0, 0,
                    0, 0,
                    640, 480);

                break;
            }
            default:
                std::cout << "event: " << e.type << std::endl;
                break;
        }
    }


    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    XDestroyWindow(dpy, w);
    XCloseDisplay(dpy);


    return 0;
}
```

至此，我们完成了在`X11`下使用`Cairo`引擎绘制图形的全部过程。`Windows`的程序架构和事件循环有所区别，但思路是相同的。当然，这仅仅是阐述了基本过程，还有更多的细节值得研究和探讨。

