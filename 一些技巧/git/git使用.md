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

- `git` 切换分支

1. `git branch -a` 查看远端仓库的所有分支

2. `git branch` 查看现在项目处于哪个分支

3. `git checkout master` 切换到指定的分支，我这里指定的是`master`

- 恢复`修改 M`和`删除 D`的文件：`git chekcout .`
- 删除`未追踪 U`的文件：`git clean -fd`
  - 参考链接：[https://blog.csdn.net/zhu_superman/article/details/136326154](https://blog.csdn.net/zhu_superman/article/details/136326154)
- 关于`git`的大小写敏感问题
  - 在`windows`上默认是不敏感的，在`linux`和`mac`上是敏感的
  - 也就是说在`windows`下，修改一个文件或者目录的字符大小写，不会被检测到，这就可能出现很大的问题了。。。
  - 解决办法：`git config core.ignorecase false`，将忽略大小写设置为`false`，这样就能正确识别了，当然也可以使用参数`--global`设置全局
  - 但是有个问题：设置了全局以后，每次克隆一个仓库，默认会添加这一条`core.ignorecase`，并且为`true`，没办法，手动改一下吧，至少解决主要问题了。。。
- 由于我的仓库加入了`git guardian`，所以关于`token`没办法直接写入文件传到远端，直接传会报错，当然它也是为我着想。。。
  - 好吧，图片能传上去，笑死我了。。。
  - 虽然但是，为了隐私，我还是放在本地吧，不传上去了

