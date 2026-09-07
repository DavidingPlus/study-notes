# Nacos部署

# 三方软件直接部署
![1760862196412-8cf7212c-db26-4930-9cac-e7678c5a260f.png](./img/w0ExaxUtb9qmV7r3/1760862196412-8cf7212c-db26-4930-9cac-e7678c5a260f-801280.png)

## 使用Docker-Compose部署
```xml
version: '3'
services:
  nacos:
    image: nacos/nacos-server:latest
    container_name: nacos
    environment:
      - PREFER_HOST_MODE=hostname
      - MODE=standalone  # 关键点：standalone 模式，不使用数据库
    ports:
      - "8848:8848"
      - "9848:9848"

```

## 检查部署结果
查看nocos状态，输出Up说明容器启动成功

```plain
docker ps -a | findstr nacos-standalone

:: 查看容器日志，确认nacos启动正常
docker logs -f nacos-standalone
```

![1770707136510-a4af9833-4bcc-487c-8da7-f69742cbb624.png](./img/w0ExaxUtb9qmV7r3/1770707136510-a4af9833-4bcc-487c-8da7-f69742cbb624-217364.png)

![1770707162934-3ed2892a-793d-477b-8d6b-c7bde0fe20d0.png](./img/w0ExaxUtb9qmV7r3/1770707162934-3ed2892a-793d-477b-8d6b-c7bde0fe20d0-726528.png)

## 访问nacos控制台：默认账号密码均为nacos，能登录即正常运行
##### [http://localhost:8848/nacos](http://localhost:8848/nacos)


> 更新: 2026-05-21 15:54:06  
> 原文: <https://www.yuque.com/chengxuyuancarl/edybxb/mp0mype4zfuo8s3i>