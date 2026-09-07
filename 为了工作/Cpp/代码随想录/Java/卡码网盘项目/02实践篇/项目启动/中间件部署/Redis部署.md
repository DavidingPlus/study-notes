# Redis部署

推荐使用docker进行Redis的部署与管理，具体方法如下：

1. 拉取镜像

```plain
docker pull redis
```

2. 启动容器

```plain
docker run -d \
  --name redis \
  -p 6379:6379 \
  -v redis-data:/data \
  redis redis-server --appendonly yes --requirepass 123456
```

3. 验证，如果列表中出现redis则成功

```plain
docker ps
```

4. 连接测试

```plain
docker exec -it redis容器名 redis-cli -a 123456
```



> 更新: 2026-05-21 15:54:33  
> 原文: <https://www.yuque.com/chengxuyuancarl/edybxb/blstebsiqkk7pnpq>