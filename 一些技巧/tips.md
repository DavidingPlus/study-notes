- code的c++的google代码规范代码

  c++插件设置搜索format，C_Cpp: Clang_format_fallback Style中填入

  ~~~json
  {BasedOnStyle: Google, UseTab: Never, IndentWidth: 4, TabWidth: 4, BreakBeforeBraces: Attach, AllowShortIfStatementsOnASingleLine: false, IndentCaseLabels: false, ColumnLimit: 0, AccessModifierOffset: -4}
  ~~~

- code中列出所有的插件列表：`code --list-extensions`

- gcc不同版本的切换：https://www.jianshu.com/p/f66eed3a3a25

- 彻底卸载计算机中的python：[https://blog.csdn.net/qq_38463737/article/details/107205696](https://blog.csdn.net/qq_38463737/article/details/107205696)

- pip相关

  - 清除下载缓存：`pip cache purge`

  - pip换源：[https://blog.csdn.net/JineD/article/details/125090904](https://blog.csdn.net/JineD/article/details/125090904)

- powshell增强，自动补全、主题美化、Git扩展：[https://cloud.tencent.com/developer/article/2317806](https://cloud.tencent.com/developer/article/2317806)

  - 相关powershell配置文件见`./oh-my-posh-theme/Microsoft.PowerShell_profile.ps1`
  - 在windows中添加alias命令别名：[https://zhuanlan.zhihu.com/p/74881435](https://zhuanlan.zhihu.com/p/74881435)

- windows下的包管理器scoop

  - 如何安装：
    - [https://blog.csdn.net/Apple_Coco/article/details/113281197](https://blog.csdn.net/Apple_Coco/article/details/113281197)
    - [https://blog.dejavu.moe/posts/windows-scoop](https://blog.dejavu.moe/posts/windows-scoop)
  - 换国内的scoop更新源
    - `scoop config SCOOP_REPO git@gitee.com:scoop-bucket/scoop.git`
  - 换国内bucket的scoop-cn源：[https://github.com/duzyn/scoop-cn](https://github.com/duzyn/scoop-cn)
  - 用scoop安装oh-my-posh：[https://sspai.com/post/69911](https://sspai.com/post/69911)
  - scoop可以安装aria2加快下载速度，但是不建议自定义其配置，用默认就行
    - 关闭警告可以开：`scoop config aria2-warning-enabled False`
    - 删除`config`使用`scoop config rm <...>`
  - 清理命令（其他的请自行搜索）
    - 清理软件的旧版本：`scoop cleanup *`
    - 清理下载缓存：`scoop cache rm *`
    - 更新scoop本身：`scoop update`
    - 更新某些app：`scoop update <appName>`
    - 禁用某些程序更新：`scoop hold <appName>`
    - 允许某些程序更新：`scoop unhold <appName>`

- 老白的博客：https://baizj.gitee.io/

- pip安装报错：https://www.cnblogs.com/GrnLeaf/p/17201957.html

- Linux应用层编程可以开发的一些项目：https://blog.csdn.net/sinat_16643223/article/details/105761275

- typora实现页内链接：https://blog.csdn.net/Manchester__united/article/details/113257595

- Linux下配置github：https://blog.csdn.net/qq_33975041/article/details/104275499

- Linux下安装python2(2.7.9)(可靠回答)：https://ubuntuforums.org/showthread.php?t=2486174

- 清理wsl2占据空间的问题(虚拟空间，会吃磁盘)：https://zhuanlan.zhihu.com/p/614993276

  ~~~powershell
  diskpart
  # 在新开的终端里面
  select vdisk file="D:\Ubuntu\ext4.vhdx"
  compact vdisk
  ~~~

  ```powershell
  diskpart
  # 在新开的终端里面
  select vdisk file="D:\Ubuntu20.04\ext4.vhdx"
  compact vdisk
  ```

- cmake编译器选择指定的版本：https://www.cnblogs.com/lidabo/p/15400964.html

- 搭建自己的博客：

  https://yushuaigee.gitee.io/

  https://blog.csdn.net/qq_42893625/article/details/100852221

  http://yearito.cn/posts/hexo-theme-beautify.html

  https://blog.csdn.net/qq_41461536/article/details/126127819

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

  https://blog.csdn.net/qq_43780023/article/details/121670691
  https://blog.csdn.net/weixin_48299611/article/details/120709995

- 将静态网页挂载到服务器上：https://blog.csdn.net/weixin_51563198/article/details/119673234

- wsl永久修改hostname：

  https://blog.csdn.net/qq_19922839/article/details/120697210

  https://blog.csdn.net/ichenglin/article/details/124963918

- 离线安装pip：

  https://zhuanlan.zhihu.com/p/37473690

  https://www.cnblogs.com/geoffreyone/p/10332737.html

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

- 在docker上部署个人云盘nextcloud：https://zhuanlan.zhihu.com/p/108301496/

- github的ssh连接超时：[https://blog.csdn.net/the__future/article/details/130038818](https://blog.csdn.net/the__future/article/details/130038818)

- 解决Qt（从5.15以后只能在线安装）在线安装过慢：[https://zhuanlan.zhihu.com/p/645793252](https://zhuanlan.zhihu.com/p/645793252)

- vscode的CMake-tools插件kits的json文件位置：`~/.local/share/CMakeTools`

- ccache清除缓存文件夹：使用命令`ccache -s -v`查看缓存的位置，删除即可

- windows下安装npm和cnpm：[https://blog.csdn.net/qq_40323256/article/details/100825982](https://blog.csdn.net/qq_40323256/article/details/100825982)

- npm配置环境变量：[https://blog.csdn.net/InOneStep/article/details/110235287](https://blog.csdn.net/InOneStep/article/details/110235287)

- 在windows的git-bash中使用tree命令：[https://blog.csdn.net/qq_33154343/article/details/100064726](https://blog.csdn.net/qq_33154343/article/details/100064726)

- 我的世界通过forge核心进行服务器搭建：[https://www.mintimate.cn/2020/02/08/Minecraft-Mod服务器搭建)](https://www.mintimate.cn/2020/02/08/Minecraft-Mod服务器搭建)

- 修复zsh历史错误：[https://blog.csdn.net/henryhu712/article/details/123836534](https://blog.csdn.net/henryhu712/article/details/123836534)

- python项目的requirements.txt：[https://blog.csdn.net/pearl8899/article/details/113877334](https://blog.csdn.net/pearl8899/article/details/113877334)

- python执行报错SyntaxError：[https://blog.csdn.net/flyfish1986/article/details/114702706](https://blog.csdn.net/flyfish1986/article/details/114702706)

- 使用gulp压缩hexo博客文件（兼容ES6标准）：[https://blog.inkuang.com/2021/405](https://blog.inkuang.com/2021/405)

