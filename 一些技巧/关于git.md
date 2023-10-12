# git的一些使用技巧

持续更新~~~

第一次拉取:`git clone git@github.com:xxx/xxx.git` 前面写用户名，后面写仓库名称

拉取之后会把`github`上的文件夹拉取到本地，在文件夹当中会有一个`.git/`隐藏文件夹，不要动它，这是自动生成的配置

- 后续的拉取(我自己写代码一般用不上)：

  `git pull`

- 后续提交按照顺序：

1. `git add .` 
将所有的文件放到暂存区
这个时候可以用 `git status` 查看文件状态，已经放到暂存区

2. `git commit -m "xxx"`
使用git commit 提交文件，此时提交的修改仍然存储在本地，并没有上传到远程服务器。`-m` 后为此次提交的说明，解释做了哪些修改，方便他人理解。

3. `git push -u origin master`(可以`-u`也可以不加)
`origin`表示是远程仓库，`master`是我们操作的分支，我一般就用`master`
我们可以用 `git remote -v` 查看地址

- `Git` 切换分支

1. `git branch -a` 查看远端仓库的所有分支

2. `git branch` 查看现在项目处于哪个分支

3. `git checkout master` 切换到指定的分支，我这里指定的是`master`

