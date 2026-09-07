# Rocket MQ部署

可以使用 用 Docker 一键安装并启动 RocketMQ 消息队列  

以win系统为例，需要：

```bash
docker network create rocketmq

docker run -d --name rmqnamesrv --network rocketmq -p 9876:9876 apache/rocketmq:5.1.4 sh mqnamesrv

docker run -d --name rmqbroker --network rocketmq -e "NAMESRV_ADDR=rmqnamesrv:9876" -p 10911:10911 -p 10909:10909 apache/rocketmq:5.1.4 sh mqbroker -n rmqnamesrv:9876
```

在代码中更改为自己的信息，networkdisk-common/networkdisk-mq/src/main/resources/stream.yml

```yaml
spring:
  cloud:
    stream:
      rocketmq:
        binder:
          # rocketmq 地址
          name-server: 127.0.0.1:9876 #rocketmq的地址，如果你要用他，这里需要改成你自己的
```





> 更新: 2026-04-25 14:28:34  
> 原文: <https://www.yuque.com/chengxuyuancarl/edybxb/cxsescch2lhyua1y>