oh-my-posh --init --shell pwsh --config D:\scoop\apps\oh-my-posh\current\themes\hunk.omp.json | Invoke-Expression #  加载 oh-my-posh 主题
Set-PSReadLineOption -PredictionSource None # 禁用预测 IntelliSense
Set-PSReadLineKeyHandler -Key "Tab" -Function MenuComplete # Tab 会出现自动补全菜单
Import-Module posh-git # git 命令在 Powershell 中会自动补全
Set-Alias -Name ss -Value scoop-search # 为 scoop-search 命令设置别名 ss
function rf($path) { Remove-Item -Recurse -Force $path } # 为 Windows 配置类似 rm -rf 的简便命令 rf
function ds($path) { dust -d 1 -r $path } # 为 Windows 配置类似 Linux 下 du 的命令 ds，使用软件包 dust
