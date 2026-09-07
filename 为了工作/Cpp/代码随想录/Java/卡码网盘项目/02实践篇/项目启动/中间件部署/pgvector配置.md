# pgvector配置

可以使用docker直接配置：

```bash
docker run -d \
  --name new-project-pgvector \  # 可以自定义容器名
  -p 5432:5432 \
  -e POSTGRES_PASSWORD=123456 \  # 自定义密码
  -e POSTGRES_USER=postgres \
  -e POSTGRES_DB=new_project_db \
  -v ./pgdata:/var/lib/postgresql/data \
  pgvector/pgvector:pg16
```

执行SQL文件初始化数据库：

```bash
docker exec -i 你的容器名 psql -U postgres -d networkdisk_ai < （注意此处是你的DDL_network_disk_ai_pgvector.sql文件的绝对路径，如）F:\zhizhib\disk\NetworkDisk\networkdisk-config\DDL_network_disk_ai_pgvector.sql
```

执行成功后显示：

```plain
CREATE EXTENSION
CREATE TABLE
CREATE TABLE
CREATE INDEX
```





> 更新: 2026-04-25 15:30:18  
> 原文: <https://www.yuque.com/chengxuyuancarl/edybxb/ui4ac289gxhmqq8f>