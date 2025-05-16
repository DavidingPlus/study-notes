---
title: tips
categories:
  - 一些技巧
abbrlink: 642c4108
date: 2024-06-26 16:05:00
updated: 2025-05-16 16:25:00
---

<meta name="referrer" content="no-referrer"/>

本文总结了工作过程中可能遇到的各种问题的解决方法的链接。

- typora破解版下载链接：[https://blog.csdn.net/weixin_43580824/article/details/131371951](https://blog.csdn.net/weixin_43580824/article/details/131371951)

- code的c++的代码规范格式

  - c++插件设置搜索format，C_Cpp: Clang_format_fallback Style中填入，详见clang-format-fallback-style目录的config.json

  - 参考文档：

    - [https://blog.csdn.net/weixin_43717839/article/details/129382657](https://blog.csdn.net/weixin_43717839/article/details/129382657)

    - [https://blog.csdn.net/qq_29856169/article/details/119617736](https://blog.csdn.net/qq_29856169/article/details/119617736)

<!-- more -->

```json
{
    "BasedOnStyle": "LLVM",
    "UseTab": "Never",
    "IndentWidth": 4,
    "TabWidth": 4,
    "BreakBeforeBraces": "Allman",
    "AllowShortIfStatementsOnASingleLine": true,
    "AllowShortLoopsOnASingleLine": true,
    "IndentCaseLabels": true,
    "ColumnLimit": 0,
    "AccessModifierOffset": -4,
    "NamespaceIndentation": "All",
    "FixNamespaceComments": false,
    "MaxEmptyLinesToKeep": 2,
    "EmptyLineBeforeAccessModifier": "Always",
    "EmptyLineAfterAccessModifier": "Always",
    "SortIncludes": "Never"
}
```

- code中列出所有的插件列表：`code --list-extensions`

  - 以下的`VS Code`插件都不适合升到最高版本，可能会出现各种各样的问题，下面是推荐的版本：
    - `C++`：`1.18.5`
    - `CMake Tools`：`1.16.32`

- gcc不同版本的切换：[https://www.jianshu.com/p/f66eed3a3a25](https://www.jianshu.com/p/f66eed3a3a25)

- Ubuntu 20.04 安装高版本的 gcc（如 gcc-11）：[https://blog.csdn.net/tiansyun/article/details/136592258](https://blog.csdn.net/tiansyun/article/details/136592258)

- 彻底卸载计算机中的python：[https://blog.csdn.net/qq_38463737/article/details/107205696](https://blog.csdn.net/qq_38463737/article/details/107205696)

- pip 相关

  - 清除下载缓存：
  
  ```bash
  pip cache purge
  ```

  - pip 换源：[https://blog.csdn.net/JineD/article/details/125090904](https://blog.csdn.net/JineD/article/details/125090904)

  - pip 无法从清华源下载，报错 ERROR HTTP error 403：[https://blog.csdn.net/qq_21386397/article/details/136964624](https://blog.csdn.net/qq_21386397/article/details/136964624)

  - 直接使用阿里云的源即可：
  
  ```bash
  pip config set global.index-url https://mirrors.aliyun.com/pypi/simple/
  ```

- windows下的包管理器scoop

  - 如何安装：
    - [https://blog.csdn.net/Apple_Coco/article/details/113281197](https://blog.csdn.net/Apple_Coco/article/details/113281197)
    - [https://blog.dejavu.moe/posts/windows-scoop](https://blog.dejavu.moe/posts/windows-scoop)
    - 推荐使用第二个，使用管理员打开powershell，注意区分开**用户本身和全局安装的路径**
    - 若最后一步提示管理员权限失败，参考：[https://stackoverflow.com/questions/74763204/installing-scoop-fails-running-the-installer-as-administrator-is-disabled-by-d](https://stackoverflow.com/questions/74763204/installing-scoop-fails-running-the-installer-as-administrator-is-disabled-by-d)
  - 关于`Hash Check Failed`问题：加`-s`参数，详见[https://github.com/lzwme/scoop-proxy-cn/blob/main/README.md#关于-hash-check-failed-的问题](https://github.com/lzwme/scoop-proxy-cn/blob/main/README.md#关于-hash-check-failed-的问题)
  - 换国内的`scoop`更新源
    - `scoop config SCOOP_REPO https://gitee.com/scoop-bucket/scoop.git`
  - 换国内`bucket`的`scoop-cn`源
    - 参照两个`github`仓库链接配置即可，推荐使用第一个，数量更齐全
    - [https://github.com/lzwme/scoop-proxy-cn](https://github.com/lzwme/scoop-proxy-cn)
      - 仓库提供的镜像被墙了，可使用 gitee 镜像：[https://gitee.com/mybasegy/scoopcn](https://gitee.com/mybasegy/scoopcn)。个人建议使用原 github 仓库的 ssh 链接：[git@github.com:lzwme/scoop-proxy-cn.git](git@github.com:lzwme/scoop-proxy-cn.git)，请注意自己为 github 配个 ssh 密钥。
    - [https://github.com/duzyn/scoop-cn](https://github.com/duzyn/scoop-cn)
    - 为了保险，将这个`bucket`的名字设置为`main`，防止`update`的时候报`no bucket main`的错误
  - 用`scoop`安装`oh-my-posh`：[https://sspai.com/post/69911](https://sspai.com/post/69911)
  - `scoop`可以安装`aria2`加快下载速度，但是不建议自定义其配置，用默认就行
    - 关闭警告可以开：`scoop config aria2-warning-enabled False`
    - 删除`config`使用`scoop config rm <...>`
  - 清理命令（其他的请自行搜索）
    - 清理软件的旧版本：`scoop cleanup *`
    - 清理下载缓存：`scoop cache rm *`
    - 更新scoop本身：`scoop update`
    - 更新某些app：`scoop update <appName>`
    - 禁用某些程序更新：`scoop hold <appName>`
    - 允许某些程序更新：`scoop unhold <appName>`

- powshell增强，自动补全、主题美化、Git扩展：[https://cloud.tencent.com/developer/article/2317806](https://cloud.tencent.com/developer/article/2317806)

  - 相关powershell配置文件见`.\oh-my-posh-theme\Microsoft.PowerShell_profile.ps1`，将其拷贝到`~\Documents\WindowsPowerShell\Microsoft.PowerShell_profile.ps1`
  - 在windows中添加alias命令别名：[https://zhuanlan.zhihu.com/p/74881435](https://zhuanlan.zhihu.com/p/74881435)

- bandizip安装

  - 6.25绿色专业版，有右键菜单：[https://www.52pojie.cn/thread-1477693-1-1.html](https://www.52pojie.cn/thread-1477693-1-1.html)
  - 不推荐使用Scoop包管理器安装，因为是便携版，没有右键菜单

- pip安装报错

  - https://www.cnblogs.com/GrnLeaf/p/17201957.html

  - [https://www.yaolong.net/article/pip-externally-managed-environment](https://www.yaolong.net/article/pip-externally-managed-environment)

- Linux应用层编程可以开发的一些项目：https://blog.csdn.net/sinat_16643223/article/details/105761275

- typora实现页内链接：https://blog.csdn.net/Manchester__united/article/details/113257595

- Linux下配置github：https://blog.csdn.net/qq_33975041/article/details/104275499

- Linux下安装python2(2.7.9)(可靠回答)：https://ubuntuforums.org/showthread.php?t=2486174

- 清理wsl2占据空间的问题(虚拟空间，会吃磁盘)：https://zhuanlan.zhihu.com/p/614993276

  ~~~powershell
  diskpart

  # 在新开的终端里面
  select vdisk file="D:\Wsl\Ubuntu\ext4.vhdx"
  compact vdisk
  ~~~

- cmake编译器选择指定的版本：https://www.cnblogs.com/lidabo/p/15400964.html

- 搭建自己的博客：

  - [https://yushuaigee.gitee.io/](https://yushuaigee.gitee.io/)
  - [https://blog.csdn.net/qq_42893625/article/details/100852221](https://blog.csdn.net/qq_42893625/article/details/100852221)
  - [http://yearito.cn/posts/hexo-theme-beautify.html](http://yearito.cn/posts/hexo-theme-beautify.html)
  - [https://blog.csdn.net/qq_41461536/article/details/126127819](https://blog.csdn.net/qq_41461536/article/details/126127819)

- 解决博客网络图片加载不出来：https://blog.csdn.net/mqdxiaoxiao/article/details/96770756
  (在md文章正文前面添加：`<meta name="referrer" content="no-referrer"/>`)

- .gitignore不起作用：https://blog.csdn.net/Saintmm/article/details/120847019

- hexo给主页设置分页：https://blog.csdn.net/qq_39181839/article/details/109477431

- hexo访客数和访问次数统计：https://echowinde.github.io/2022/06/08/hexo-diary-1/

- Git使用分支管理不同的项目：https://blog.csdn.net/Jone_hui/article/details/114068497

- Git删除分支：https://www.freecodecamp.org/chinese/news/how-to-delete-a-git-branch-both-locally-and-remotely/

- Git分支重命名：https://blog.csdn.net/qq_37148270/article/details/107106392

- Linux下离线安装python：https://blog.csdn.net/m0_37711292/article/details/131593577

- 利用SCP协议将本地文件传输到云服务器："scp 本地文件路径 Linux云服务器用户名@云服务器实例公网IP地址:Linux云服务器文件路径"
  如果传文件夹可以打tar包，也可以加上-r参数，一定放在scp和本地文件路径之间

- 腾讯云Centos升级gcc：https://blog.csdn.net/higer2008/article/details/123322867

- json-server，伪造一些json数据挂到服务器上：

  - [https://blog.csdn.net/qq_43780023/article/details/121670691](https://blog.csdn.net/qq_43780023/article/details/121670691)
  - [https://blog.csdn.net/weixin_48299611/article/details/120709995](https://blog.csdn.net/weixin_48299611/article/details/120709995)

- 将静态网页挂载到服务器上：https://blog.csdn.net/weixin_51563198/article/details/119673234

- wsl下载更新网络错误：[https://zhuanlan.zhihu.com/p/661429025](https://zhuanlan.zhihu.com/p/661429025)

- wsl登录`getpwnam(username) failed 0`：[https://github.com/microsoft/WSL/issues/9340](https://github.com/microsoft/WSL/issues/9340)

- wsl永久修改hostname：

  - [https://blog.csdn.net/qq_19922839/article/details/120697210](https://blog.csdn.net/qq_19922839/article/details/120697210)

  - [https://blog.csdn.net/ichenglin/article/details/124963918](https://blog.csdn.net/ichenglin/article/details/124963918)

- wsl 安装出现 Installing, this may take a few minutes... 时间长。且重新打开进入 root 用户的问题的解决：[https://www.cnblogs.com/ziyulab/p/18446092](https://www.cnblogs.com/ziyulab/p/18446092)

- wsl安装微软雅黑字体（解决窗体title乱码问题）

  - [https://blog.csdn.net/qq_30712797/article/details/128885517](https://blog.csdn.net/qq_30712797/article/details/128885517)

  - `8.3`节

  - 显示器类型选择`LCD`

- 离线安装pip：

  - [https://zhuanlan.zhihu.com/p/37473690](https://zhuanlan.zhihu.com/p/37473690)
  - [https://www.cnblogs.com/geoffreyone/p/10332737.html](https://www.cnblogs.com/geoffreyone/p/10332737.html)

- Zenity，在Shell下迈向图形化的GUI工具：[https://blog.csdn.net/DarlingTeng/article/details/106266964](https://blog.csdn.net/DarlingTeng/article/details/106266964)

- Linux配置git：https://blog.csdn.net/qq_26620783/article/details/97265409

- Docker搭建httpbin服务：https://blog.csdn.net/DahlinSky/article/details/104369657
  (配置好了在面板上就可以方便的管理)

- 给自己的博客添加网易云音乐：https://music.163.com/#/outchain/0/8897180109/m/use/html

- Hexo 每天自动提交网站url到搜索引擎、百度、必应、谷歌：https://blog.csdn.net/Lott0419/article/details/113564110
  这个是部署之后也不立刻提交，而是在一个固定的时间点提交，点击star就可以提交一次url，非常好用

- Github博客配置CDN加速(不是域名)：https://zhuanlan.zhihu.com/p/97389072，还有自己去看B站视频

- 使用Cloudflare进行CDN加速之后“网站重定向次数过多”：将SSL/TLS加密模式从 灵活 改为 完全

- 在自己的服务器上配置teamspeak服务器并且绑定域名：https://bambovc.com/index.php/archives/30/

- 使用MCSM非常方便的搭建我的世界服务器：https://mcsmanager.com/

- 给自己的mc服务器绑定域名：[https://tutujanjan.com/如何给minecraft服务器绑定域名](https://tutujanjan.com/如何给minecraft服务器绑定域名)

- 在docker上部署个人云盘nextcloud：https://zhuanlan.zhihu.com/p/108301496/

- github的ssh连接超时：[https://blog.csdn.net/the__future/article/details/130038818](https://blog.csdn.net/the__future/article/details/130038818)

- 解决Qt（从5.15以后只能在线安装）在线安装过慢：[https://zhuanlan.zhihu.com/p/645793252](https://zhuanlan.zhihu.com/p/645793252)

- vscode的CMake-tools插件kits的json文件位置：`~/.local/share/CMakeTools`

- ccache清除缓存文件夹：使用命令`ccache -s -v`查看缓存的位置，删除即可

- windows 下安装 npm 和 cnpm：[https://blog.csdn.net/qq_40323256/article/details/100825982](https://blog.csdn.net/qq_40323256/article/details/100825982)

- npm 配置环境变量：[https://blog.csdn.net/InOneStep/article/details/110235287](https://blog.csdn.net/InOneStep/article/details/110235287)

  - 注意：Scoop 安装的 nodejs 最好也按照文章配置环境变量。不过有一点需要注意，node_global 和 node_cache 目录最好放在 Scoop 的 persist 目录下的对应位置。否则更新 nodejs 以后，对应的目录会被清除，导致使用出现问题。

- 在 npm 官方发布自己的包

  - [https://blog.csdn.net/Angsts/article/details/116307509](https://blog.csdn.net/Angsts/article/details/116307509)
  - [https://blog.csdn.net/to_the_Future/article/details/122949610](https://blog.csdn.net/to_the_Future/article/details/122949610)

- npm 换源

  - 官方源

    ```bash
    npm config set registry https://registry.npmjs.org
    ```

  - 镜像源

    ```bash
    npm config set registry https://registry.npmmirror.com
    ```

- 在windows的git-bash中使用tree命令：[https://blog.csdn.net/qq_33154343/article/details/100064726](https://blog.csdn.net/qq_33154343/article/details/100064726)

- 我的世界通过forge核心进行服务器搭建：[https://www.mintimate.cn/2020/02/08/Minecraft-Mod服务器搭建)](https://www.mintimate.cn/2020/02/08/Minecraft-Mod服务器搭建)

- 安装配置zsh(linux下推荐，windows下建议使用posh)

  - [https://zhuanlan.zhihu.com/p/35283688](https://zhuanlan.zhihu.com/p/35283688)
  - [https://zhuanlan.zhihu.com/p/265525597](https://zhuanlan.zhihu.com/p/265525597)

- oh-my-zsh 安装使用镜像：[https://www.cnblogs.com/chencarl/p/16824387.html](https://www.cnblogs.com/chencarl/p/16824387.html)

- zsh推荐字体：[https://gitee.com/keyboardkiller/MesloLGS_NF](https://gitee.com/keyboardkiller/MesloLGS_NF)

- zsh安装好用插件

  - [https://blog.csdn.net/qq_41996454/article/details/102640653](https://blog.csdn.net/qq_41996454/article/details/102640653)
  - [https://blog.csdn.net/qq_36835255/article/details/128096616](https://blog.csdn.net/qq_36835255/article/details/128096616)

- 修复zsh历史错误：[https://blog.csdn.net/henryhu712/article/details/123836534](https://blog.csdn.net/henryhu712/article/details/123836534)

  ```bash
  cd ~
  mv .zsh_history .zsh_history_bad
  strings -eS .zsh_history_bad > .zsh_history
  fc -R .zsh_history
  ```

- python项目的requirements.txt：[https://blog.csdn.net/pearl8899/article/details/113877334](https://blog.csdn.net/pearl8899/article/details/113877334)

  - `python`一次性删除所有的第三方包：使用`pip freeze > log`将第三方包的内容导入到`log`文件中，然后通过`pip uninstall -r log`即可删除，如果不想输入`y`可以加上`-y`参数

- python执行报错SyntaxError：[https://blog.csdn.net/flyfish1986/article/details/114702706](https://blog.csdn.net/flyfish1986/article/details/114702706)

- 使用gulp压缩hexo博客文件（兼容ES6标准）：[https://blog.inkuang.com/2021/405](https://blog.inkuang.com/2021/405)

- windows下cmd的命令大全（cmd不是powershell，可以在C程序中的system()函数调用）：[https://zhuanlan.zhihu.com/p/468515490(https://zhuanlan.zhihu.com/p/468515490)](https://zhuanlan.zhihu.com/p/468515490(https://zhuanlan.zhihu.com/p/468515490))

  - 创建目录：`md ...`
  - 删除目录：`rd ...`
  - 创建文件（没有直接的命令，利用重定向）：`echo > ...`
  - 删除文件：`del ...`

- vscode通过ssh连接远程服务器：[https://blog.csdn.net/qq_29856169/article/details/115489702](https://blog.csdn.net/qq_29856169/article/details/115489702)

- linux编译并且安装内核：[https://blog.csdn.net/weixin_43567188/article/details/119407019](https://blog.csdn.net/weixin_43567188/article/details/119407019)

- linux删除多余的内核版本：[https://blog.csdn.net/jays_/article/details/90738580](https://blog.csdn.net/jays_/article/details/90738580)

- linux下使用tar压缩或者解压：[https://blog.csdn.net/stf1065716904/article/details/81909320](https://blog.csdn.net/stf1065716904/article/details/81909320)

- linux下使用zip压缩或者解压：[https://blog.csdn.net/soback00/article/details/132853862](https://blog.csdn.net/soback00/article/details/132853862)

- windows下使用tar压缩或者解压：[https://blog.csdn.net/weixin_42672685/article/details/132900030](https://blog.csdn.net/weixin_42672685/article/details/132900030)

- 在linux下如何使用tar和zip命令压缩、解压、分割、合并文件（分割和合并的操作对于`tar`和`zip`是一样的）：[https://blog.csdn.net/qq_41979513/article/details/98763608](https://blog.csdn.net/qq_41979513/article/details/98763608)

- cmake的build和install命令，不使用make

  - build：`cmake --build ./`
  - install：`cmake --build ./ --target install`

- oh-my-zsh更新换源

  - 更新命令：`omz update`
  - 进入`oh-my-zsh`安装目录：`cd ~/.oh-my-zsh`
  - `git config --edit`
  - 将`url`改为`https://gitee.com/mirrors/oh-my-zsh.git`

- ubuntu重启后进入initramfs导致无法开机：[https://blog.csdn.net/weixin_44002151/article/details/134212707](https://blog.csdn.net/weixin_44002151/article/details/134212707)

- windows下powershell中类似find命令的替代：[https://blog.csdn.net/yao_zhuang/article/details/115645435](https://blog.csdn.net/yao_zhuang/article/details/115645435)

- windows下配置临时变量：[https://blog.csdn.net/qq_23212697/article/details/69177654](https://blog.csdn.net/qq_23212697/article/details/69177654)

- 虚拟机网卡掉了，没网：[https://blog.csdn.net/qq_40344790/article/details/120645806](https://blog.csdn.net/qq_40344790/article/details/120645806)

- wps绿色版：[https://www.52pojie.cn/thread-1263670-1-1.html](https://www.52pojie.cn/thread-1263670-1-1.html)

- Office tool plus下载：[https://otp.landian.vip/zh-cn/](https://otp.landian.vip/zh-cn/)

- Office 2021专业（增强版）安装教程：[https://www.mrdadong.com/archives/office-2021](https://www.mrdadong.com/archives/office-2021)

- Office从C盘迁移到其他盘：[https://www.bilibili.com/video/BV1LT411g7Ae/](https://www.bilibili.com/video/BV1LT411g7Ae/)

- 按照上一条视频内容以后报错"**操作系统当前的配置不能运行此应用程序**"

  - [https://blog.csdn.net/m0_57941956/article/details/125575736](https://blog.csdn.net/m0_57941956/article/details/125575736)
  - 看到最后面，发现目录需要匹配，也就是需要创建在`Program Files/`目录下面，而不能直接放在`D`盘根目录下

  ```powershell
  mklink /j "C:\Program Files\Microsoft Office" "D:\Program Files\Microsoft Office"
  ```

- idm绿色版下载：[https://www.bilibili.com/read/cv35705140/](https://www.bilibili.com/read/cv35705140/)

- windows配置命令别名alias：[https://segmentfault.com/a/1190000015928399](https://segmentfault.com/a/1190000015928399)

- vmware workstation pro个人版安装：[https://blogs.vmware.com/workstation/2024/05/vmware-workstation-pro-now-available-free-for-personal-use.html](https://blogs.vmware.com/workstation/2024/05/vmware-workstation-pro-now-available-free-for-personal-use.html)

- Linux 下安装 docker 教程（不建议在 Wsl2 上安装，极可能会出问题）：[https://www.cnblogs.com/Liyuting/p/17022739.html](https://www.cnblogs.com/Liyuting/p/17022739.html)

- ubuntu 完全卸载 docker 及再次安装：[https://blog.csdn.net/qq_45495857/article/details/113743109](https://blog.csdn.net/qq_45495857/article/details/113743109)

- docker 无法删除镜像 hello-world 等的解决办法：[https://blog.csdn.net/yyj108317/article/details/105875836](https://blog.csdn.net/yyj108317/article/details/105875836)

- Linux 系统配置 clash-gui 界面：[https://chh3213.github.io/2023/11/12/linux%E7%B3%BB%E7%BB%9F%E9%85%8D%E7%BD%AEclash-gui%E7%95%8C%E9%9D%A2/](https://chh3213.github.io/2023/11/12/linux%E7%B3%BB%E7%BB%9F%E9%85%8D%E7%BD%AEclash-gui%E7%95%8C%E9%9D%A2/)

- 在低版本 Ubuntu 22.04|20.04|18.04 上安装 Node.js 20：[https://cn.linux-console.net/?p=20486](https://cn.linux-console.net/?p=20486)

- Docker Hub Search：[https://docker.fxxk.dedyn.io/](https://docker.fxxk.dedyn.io/)

- Docker 国内镜像源下架的解决办法（推荐 1Panel 镜像）：[https://mixuying.com/archives/1719753069678](https://mixuying.com/archives/1719753069678)

- conan 更改安装的包的路径（主要用于 Windows）

  - conan 1：修改 `~\.conan\conan.conf` 文件，参考：[https://www.cnblogs.com/bibo/p/15712445.html](https://www.cnblogs.com/bibo/p/15712445.html)
  
  ```ini
  [storage]
  path = <target path>
  ```

  - conan 2：修改 `~\.conan2\global.conf` 文件，在末尾添加下面这句，参考：[https://docs.conan.io/2/reference/commands/config.html](https://docs.conan.io/2/reference/commands/config.html)

  ```ini
  # Absolute path where the packages and database are stored
  core.cache:storage_path = <target path>
  ```

- 安装思维导图软件 MindManager 2022 破解版：[https://www.zwnblog.com/archives/mindmanager2022-an-zhuang-yu-po-jie--bao-mu-jiao-cheng-](https://www.zwnblog.com/archives/mindmanager2022-an-zhuang-yu-po-jie--bao-mu-jiao-cheng-)

  - 注意安装的时候不要自定义路径，选择默认的 C 盘路径，否则程序可能无法正常启动。

  - 推荐使用 mklink 作符号链接，这样既能正常启动也能节省 C 盘空间，建议目录结构保持一致（使用管理员打开 CMD）：

  ```cmd
  mklink /j "C:\Program Files\MindManager 22" "D:\Program Files\MindManager 22"
  ```

- 编译调试 Linux 内核方法：[https://ignotusjee.github.io/2024/10/18/Linux-Debug/](https://ignotusjee.github.io/2024/10/18/Linux-Debug/)

- 安装完 Wsl 以后，解决检测到 localhost 代理配置，但未镜像到 WSL，NAT 模式下的 WSL 不支持 localhost 代理的问题：[https://jasminides.com/posts/wsl_error1/](https://jasminides.com/posts/wsl_error1/)

- 禁用/删除 Windows Defender，推荐使用 [windows-defender-remover](https://github.com/ionuttbara/windows-defender-remover)。推荐选择 Remove Windows Defender Antivirus (A) 而不是 Remove Windows Defender Antivirus + Disable All Security Mitigations (Y)。全删了会删除用户账户控制的功能，就是打开程序的时候有一个确认弹窗，这个我自己比较喜欢，不想删掉。

- 安装 Nvidia 精简显卡驱动（只安装 Display Driver，不安装 GeForce Experience 以及其他组件）：[https://www.techpowerup.com/nvcleanstall/](https://www.techpowerup.com/nvcleanstall/)

- OBS 及时回放的详细设置：[https://www.bilibili.com/video/BV1kXDVYkE2t/](https://www.bilibili.com/video/BV1kXDVYkE2t/)

  - obs-notifier 插件无法向 N 卡一样在全屏的进程中进行提醒的显示，issue 见：[https://github.com/DmitriySalnikov/OBSNotifier/issues/9](https://github.com/DmitriySalnikov/OBSNotifier/issues/9)

  - 上面插件的该功能是 enhancement 的状态，为了目前使用，个人写了一个声音提醒的脚本，仅作暂时使用：[https://github.com/DavidingPlus/obs-audio-notifier](https://github.com/DavidingPlus/obs-audio-notifier)

- Windows 在默认打开方式中去掉已失效或者已删除的应用选项：[https://blog.csdn.net/qq_58995684/article/details/142319248](https://blog.csdn.net/qq_58995684/article/details/142319248)

- Windows 截屏工具：[https://zh.snipaste.com/](https://zh.snipaste.com/)

  - 通过 Scoop 安装：`scoop install snipaste-beta`

- Windows 照片/视频查看工具：[https://github.com/easymodo/qimgv](https://github.com/easymodo/qimgv)

  - 通过 Scoop 安装：`scoop install qimgv-video`

- Windows PDF 阅读器：

  - [PDF24 Creator](https://tools.pdf24.org/en/creator)（推荐），通过 Scoop 安装：`scoop install pdf24-creator-np`
  - [sumatrapdf](https://github.com/sumatrapdfreader/sumatrapdf)，通过 Scoop 安装：`scoop install sumatrapdf`

- 使用 1Panel 自动申请和续签证书：[https://hin.cool/posts/sslfor1panel.html](https://hin.cool/posts/sslfor1panel.html)

  - 截止 2025.04.15，由于我将域名迁移到了 Cloudflare，使用腾讯云的 DNS 账户续签一直卡住，最终失败，可能和 DNS 域名解析服务器有关。解决办法是使用 Cloudflare 的 DNS 账户。

- 腾讯云服务器 root 远程登陆被 ssh 服务器拒绝密码（即使密码正确）：[https://blog.csdn.net/COCO56/article/details/123770645](https://blog.csdn.net/COCO56/article/details/123770645)

- Github Action ssh-deploy 的配置注意事项（如何让 Github 工作流把文件自动推送到我们自己的云服务器）：[https://github.com/easingthemes/ssh-deploy/issues/143#issuecomment-2490116974](https://github.com/easingthemes/ssh-deploy/issues/143#issuecomment-2490116974)

- 解决 Linux /dev/mapper/ubuntu--vg-ubuntu--lv 磁盘空间不足的问题：[https://blog.csdn.net/Fly_1213/article/details/105142427](https://blog.csdn.net/Fly_1213/article/details/105142427)

- Linux 安装 Anaconda：[https://blog.csdn.net/feng8403000/article/details/127814067](https://blog.csdn.net/feng8403000/article/details/127814067)

- 使用 Deno 部署静态网页，可使用 Deno Deploy 作为静态网页的托管平台，无缝衔接：[https://deno.org.cn/blog/hugo-blog-with-deno-deploy](https://deno.org.cn/blog/hugo-blog-with-deno-deploy)

- 使用 GitHub Proxy 加快 scoop bucket 的访问：[https://ghproxy.link/](https://ghproxy.link/)

  - 使用方法：如代理地址是 https://proxy.top，bucket 仓库地址是 https://github.com/abc/bucket，则最后的地址是 https://proxy.top/github.com/abc/bucket。

  - 如果实在无法访问，使用 ssh 地址即可，例如上面则是 git@github.com:abc/bucket

- 自带 Cloudflare R2 对象存储上传的 picgo 升级版工具 PicList：[https://github.com/Kuingsmile/PicList](https://github.com/Kuingsmile/PicList)

- 使用 Cloudflare R2 和 PicList 搭建图床：[https://blog.xingchencloud.top/p/e613c591.html](https://blog.xingchencloud.top/p/e613c591.html)

- 1Panel 搭建 frp 服务端并使用 Openresty 反向代理：[https://www.cnblogs.com/drcode/p/18455476/panel-frps-reverse-proxy-https](https://www.cnblogs.com/drcode/p/18455476/panel-frps-reverse-proxy-https)

- Frp 安全连接远程桌面，简单、安全、可视化（安全穿透）：[https://www.bilibili.com/video/BV1hZtme2Esu/](https://www.bilibili.com/video/BV1hZtme2Esu/)

