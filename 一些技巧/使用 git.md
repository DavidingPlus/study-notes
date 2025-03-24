---
title: 使用 git
categories:
  - 一些技巧
abbrlink: 13ca1b18
date: 2024-04-24 20:15:00
updated: 2025-03-24 22:45:00
---

<meta name="referrer" content="no-referrer"/>

本文是一个使用 git 的简要总结。

- 准备工作，git 配置 ssh：[https://blog.csdn.net/lqlqlq007/article/details/78983879](https://blog.csdn.net/lqlqlq007/article/details/78983879)
- 第一次拉取，以 github 服务器为例，其他服务器将 github.com 替换成为对应的 IP 或者域名：git clone git@github.com:xxx/xxx.git 前面写用户名，后面写仓库名称；拉取之后会把 github 上的目录拉取到本地，在目录当中会有一个 .git/ 隐藏文件夹，不要动它，这是自动生成的配置

<!-- more -->

- 后续的拉取：

  - git pull
- 后续提交按照顺序：
  - git add . 
  - 将所有的文件放到暂存区，这个时候可以用 git status 查看文件状态，已经放到暂存区
- git commit -m "xxx"

  - 使用 git commit 提交文件，此时提交的修改仍然存储在本地，并没有上传到远程服务器。-m 后为此次提交的说明，解释做了哪些修改，方便他人理解
- git push -u origin master（可以加 -u 也可以不加）
  - origin 表示是远程仓库，master 是我们操作的分支，我一般就用 master
  - 我们可以用 git remote -v 查看地址
- git 切换分支
  - git branch -a 查看远端仓库的所有分支
  - git branch 查看现在项目处于哪个分支
  - git checkout master 切换到指定的分支，我这里指定的是 master
- git 新建分支
  - git checkout -b <branch>：保留当前分支的 commit 历史，开一个新分支，这时候 merge 原分支是最新的
  - git checkout --orphan <branch>：开一个没有 commit 历史的独立分支，这两个分支是独立的
  - 在实际工作中，用第一个保留历史的最好，因为独立分支的话，后续应用主分支的修改还要 merge，就会把该分支第一次的 commit 全 merge 过来，效果和第一个一样，但是更费力，不推荐，除非是功能不相干的分支，使用第二个比较好
  - 最终 push 到远端的时候需要显式指定分支，这里最好加上都 -u 参数
- 恢复修改 M 和删除 D 的文件：git chekcout .
- 删除未追踪 U 的文件：git clean -fd
  - 参考链接：[https://blog.csdn.net/zhu_superman/article/details/136326154](https://blog.csdn.net/zhu_superman/article/details/136326154)
- 关于 git 的大小写敏感问题
  - 在 Windows 上默认是不敏感的，在 Linux 和 Mac 上是敏感的
  - 也就是说在 Windows 下，修改一个文件或者目录的字符大小写，不会被检测到，这就可能出现很大的问题了。
  - 解决办法：git config core.ignorecase false，将忽略大小写设置为 false，这样就能正确识别了，当然也可以使用参数 --global 设置全局
  - 但是有个问题：设置了全局以后，每次克隆一个仓库，默认会添加这一条 core.ignorecase，并且为 true，没办法，手动改一下吧，至少解决主要问题了。
- git 版本回退的步骤：[https://blog.csdn.net/qing040513/article/details/109150075](https://blog.csdn.net/qing040513/article/details/109150075)

  - 使用 git log 命令或 code 的 git graph 插件查看需要恢复的 commit 的 ID
  - git reset --hard <commit>，回退到某次的 commit 版本
    - 注意：--hard 的参数的有无是很重要的，有 --hard 参数，回退以后本地当前的修改全部清空，没有才会保留
    - 因此，引申出撤销已经 commit 但未 push 的提交：git reset origin/<branch>，记得不要加上 --hard
  - git commit --allow-empty，当然可以在此版本上修改之后在提交，但这里为了简便，就空提交了
  - git push -f -u origin <branch>，-f 参数表示强制推送，这样会覆盖回退的版本到最新版本之间的所有 commit，直接到达最新提交的 commit；所以 -f 参数还是慎用，否则把别人的工作干掉了得不偿失，尽量还是拉取合并解决冲突吧
- git 在合并的时候全部采用某一端的文件：[https://blog.csdn.net/chaiyu2002/article/details/83791671](https://blog.csdn.net/chaiyu2002/article/details/83791671)

  - merge 的时候加上采用传入端或者本端
    - 采用传入修改：git merge <branch> --strategy-option theirs

    - 采用本地修改：git merge <branch> --strategy-option ours

  - 如果显示 unrelated histories，在后面加上 --allow-unrelated-histories

  - 同理 git pull 在冲突需要合并的时候也可以采取这样的方法
- git 同步远程已删除的分支和删除本地多余的分支：[https://www.cnblogs.com/saysmy/p/9166331.html](https://www.cnblogs.com/saysmy/p/9166331.html)
  - 查看本地分支和追踪情况：git remote show origin
  - 同步远程已删除的分支：git remote prune origin
  - 删除本地分支：git branch -D <branch>
- git 删除远程分支：git push origin --delete <branch>
- git 重命名分支

  - 重命名本地分支：git branch -m <newName>
  - 如何重命名远程分支？
    - 首先将本地对应的分支（一般都是与远程分支相同，如果不同请跳过这一步）重命名为想要的名字
    - 然后将原远程分支删除
    - 最后重新将本地重命名的分支推送上去即可：git push -u origin <newBranch>
- git 将 vim 设置为默认编辑器（Linux 下默认的是 nano，用着不太习惯）：git config --global core.editor vim
- git 指定合并某次的 commit（相当于可以跳过 git tree 中间的一些 commit）：git cherry-pick命令
  - [https://geek-docs.com/git/git-questions/1103_git_git_skipping_specific_commits_when_merging.html](https://geek-docs.com/git/git-questions/1103_git_git_skipping_specific_commits_when_merging.html)
  - [https://geek-docs.com/git/git-questions/71_git_git_cherry_picking_with_ourstheirs_strategy.html](https://geek-docs.com/git/git-questions/71_git_git_cherry_picking_with_ourstheirs_strategy.html)
  - [https://zhuanlan.zhihu.com/p/355413226](https://zhuanlan.zhihu.com/p/355413226)
- git 报错 fatal: bad object refs/remotes/origin/xxx 的解决方法：[https://www.cnblogs.com/along007/p/17335825.html](https://www.cnblogs.com/along007/p/17335825.html)
- git 拉取子模块中的所有内容：[https://blog.csdn.net/toopoo/article/details/104225592](https://blog.csdn.net/toopoo/article/details/104225592)
  - 使用子模块的仓库的例子，boost：[https://github.com/boostorg/boost](https://github.com/boostorg/boost)
- git 报错对象文件为空（object-file-is-empty）的解决方法：[https://blog.csdn.net/syx_1990/article/details/117237631](https://blog.csdn.net/syx_1990/article/details/117237631)
- git 仓库的默认分支修改以后，最好同步将 origin/HEAD 同步修改，和 git 仓库的默认分支保持一致，否则在 fetch 和 pull 的时候可能会出现奇奇怪怪的问题：git remote set-head origin <branch>
- git 中 tag 的使用：[https://blog.csdn.net/qq_39505245/article/details/124705850](https://blog.csdn.net/qq_39505245/article/details/124705850)
- git status 不能显示中文：[https://blog.csdn.net/u012145252/article/details/81775362](https://blog.csdn.net/u012145252/article/details/81775362)
- git lfs 的使用: [https://zhuanlan.zhihu.com/p/106295945](https://zhuanlan.zhihu.com/p/106295945)
- git 项目文件大小优化（去除大文件）：[https://www.cnblogs.com/fuhua/p/15527023.html](https://www.cnblogs.com/fuhua/p/15527023.html)
  - 注意 git rev-list 和 git filter-branch 命令是可以指定分支的。默认是全部分支，也就是 --all。如果需要指定分支将 --all 替换为对应分支即可，例如 master。这样可以过滤非必要分支，例如 gh-pages。
  - 移除某个目录下所有文件的引用，命令 `git filter-branch --force --prune-empty --index-filter 'git rm -rf --cached --ignore-unmatch report/app/public/pdf/*.*' --tag-name-filter cat -- --all` 中 `*.*` 建议替换为 `*` 防止无法匹配无后缀的文件，例如 Makefile。另外例如想要匹配任何目录下名为 images 的目录可使用 `*/images/*`。
- git 遇到类似 error: cannot lock ref 'refs/remotes/origin/master': unable to resolve reference 'refs/remotes/origin/master' 的问题：[https://blog.csdn.net/a54674/article/details/102302288](https://blog.csdn.net/a54674/article/details/102302288)

