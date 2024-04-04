oh-my-posh --init --shell pwsh --config D:\scoop\apps\oh-my-posh\current\themes\hunk.omp.json | Invoke-Expression #  加载 oh-my-posh 主题
Set-PSReadLineOption -PredictionSource None # 禁用预测 IntelliSense
Set-PSReadLineKeyHandler -Key "Tab" -Function MenuComplete # Tab键会出现自动补全菜单
Import-Module posh-git # git的自动补全
