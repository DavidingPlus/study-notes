# 在X11下使用cairo引擎绘制图形

`cairo`是一个方便和高性能的第三方`C`库。它可以作为绘制引擎，帮助我们绘制各种图形，并且提供多种输出方式。本文将在`Linux`下结合`X11`图形显示协议绘制简单的图形。

这是效果图：

<img src="https://img-blog.csdnimg.cn/direct/69a40467c7b144a2a2c1fa9aa0725b43.png" alt="59cb53826fcba1602fd3769171f5f127" style="zoom:67%;" />

# 安装cairo库

`cairo`的官方网站是[https://cairographics.org](https://cairographics.org/)。上面对`cairo`图形库做了一个完整的介绍，包括下载、`API`接口、示例等等。

## 通过apt安装

通过官方文档知道，在`Linux`下可以直接通过`apt`包管理进行下载。

```bash
sudo apt install libcairo2-dev
```

下载好以后头文件和动态库就安装好了。头文件安装在`/usr/include/cairo/`中，静态库和动态库分别位于`/usr/lib/x86_64-linux-gnu/libcairo.a`和`/usr/lib/x86_64-linux-gnu/libcairo.so`。因此能直接被系统识别，直接引入头文件，编译的时候链接到`cairo`库即可。

## 通过conan安装

对于个人而言，`apt`安装自然是非常友好的。但是对于`LarkSDK`这样一个面向用户的基础框架而言，除非最基本的系统库例如`X`窗口系统`libx11-dev`，`Wayland`窗口系统`libwayland-dev`和`Wayland`键盘处理`libxkbcommon-dev`等，其他的第三方库均最好不以`apt`包的方式引入。鉴于`C++`没有自己的包管理器，因此需要借助第三方的包管理器，例如`conan`，`cpm`等。本项目使用`conan`管理第三方包。

`conan`是一个`python`语言编写的`C++`的包管理器，官方网址是[https://conan.io/](https://conan.io/)。`conan`管理了众多第三方的`C++`库，通过命令行操作就能方便的在自己的项目中引入`conan`包。需要通过`conanfile.py`或者`conanfile.txt`进行配置，当然这不是本文的重点，具体见文档[https://docs.conan.io/2/](https://docs.conan.io/2/)。

`conan`官方提供了自己的`conan`仓库，[https://conan.io/center](https://conan.io/center)。在上面能找到很多我们熟知的第三方库，例如`gtest`，`qt`，`boost`，`fmt`等。当然还有一些更基础的工具库，这里不赘述。当然[`cairo`](https://conan.io/center/recipes/cairo)库也在其中。

### 尝试安装cairo

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

### 关于conan包

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

<img src="https://img-blog.csdnimg.cn/direct/57f819b462394828bae2636578ae85b6.png" alt="image-20240715163723130" style="zoom:55%;" />

我们发现`settings`中的内容和我们的`profiles/default`中的内容对应，这就是前面提到的匹配。例如图中是一个`Mac`下的`apple-clang`的`13.0`版本的`静态库`的`Debug`包。每个包的`options`，`settings`和`requires`都会对最前面的`Package_ID`产生影响，这是一个哈希计算值，具体如何影响和生成参考[https://docs.conan.io/2/reference/binary_model/package_id.html](https://docs.conan.io/2/reference/binary_model/package_id.html)。当然这其中也有令人费解的地方，请见下文。

### 解决错误（并吐槽）

前面提到，`conan`官方的包有问题，会导致在安装的时候出现错误。我在安装的出现的错误如下：

<img src="https://img-blog.csdnimg.cn/direct/7a6baa276bc84af4aa9382fe3fb1b09f.png" alt="image-20240715165238889" style="zoom:55%;" />

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

<img src="https://img-blog.csdnimg.cn/direct/70fb6e9ca5394e2c9dfdd937f4a4562c.png" alt="image-20240715172400074" style="zoom:58%;" />

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

不是哥们，`expat`需要的是`2.5.0`，但是你规定依赖的包是`>=2.6.2 and <3`。哈？这不前后矛盾吗？对于`freetype`和`fontconfig`也是相同的问题。同时，我们再浏览一下`cairo`包拉下来以后的整体结构。奥，原来同一个版本的所有包的`conanfile.py`都是同一个文件。好，这没有任何问题，这是`conan`的设计。但是你自己不改依赖版本是什么意思，并且如果硬要不改的话，分不同的版本也是`ok`的啊，这样一套流程下来，导致`conan`拉包就出现了问题。

<img src="https://img-blog.csdnimg.cn/direct/13896d1937574a4795d6fd7e494c1ace.png" alt="image-20240715172916780" style="zoom:67%;" />

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

<img src="https://img-blog.csdnimg.cn/direct/8bccf94aaab943ffa2d25b919f9d17a4.png" alt="image-20240715175839407" style="zoom:60%;" />

# 使用cairo库绘制图形

TODO

