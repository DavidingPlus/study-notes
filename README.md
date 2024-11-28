# study-notes

学习过程中写的 markdown 文档集合，还有搜集的各种资料。啥都有，东西很杂，但是目录的结构很清晰。

关联 Repo：

1. 存储学习过程中需要的一些资源文件：[https://github.com/DavidingPlus/study-note-resources](https://github.com/DavidingPlus/study-note-resources)

2. 存储学习过程中的 markdown 文档的图片：[https://github.com/DavidingPlus/study-note-images](https://github.com/DavidingPlus/study-note-images)

注意，本仓库中的部分文章通过 github action 向 [blog 仓库](https://github.com/DavidingPlus/blog/) 进行同步，这些文章通过 file-mapping.json 进行管理。注意 json 文件中如果文件名中间存在空格的话记得加上引号括起来，使用英文单引号 `'` 即可。

大致步骤是将本仓库中这些文章的架构通过 Linux 命令处理为 blog 需要的架构，构建 _posts/ 目录。然后签出 blog 仓库，通过 git 命令提交即可。之后 blog 仓库触发 push CI 流程即可。

注意两点：

1. github robot 本身是没有权限的，提交权限需要在签出仓库的时候加上 [Token](https://github.com/settings/tokens)，可以代替 push 时候输入密码的操作，就可以使用 CI 执行 git 操作并且成功 push。同时项目的 Action 设置需要同时允许 Read 和 Write，默认是 Read Only，自己配置下即可。

2. 本来是通过 blog 的 _posts 分支进行中转的，但是 github 不允许连续两次触发 push 的 action，为了防止死循环，因此需要走手动 Trigger 的流程。现在直接一步到位了，因此不需要这个流程了。

