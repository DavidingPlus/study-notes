# 基于Canal的数据库与ES同步的部署与搭建

* 项目基于 Canal + MySQL 实现对 user\_file 等表的增量数据订阅，将变更实时同步至 Elasticsearch，供搜索与分析使用。能够支持全量初始化 + 增量同步，保证字段映射一致与回收站软删策略一致（deleted=0 可搜索）。

涉及组件：

* MySQL
* Canal Server
* Canal Adapter
* Elasticsearch

## es搭建

### es搭建

[附件: elasticsearch-analysis-ik-7.17.6.zip](./attachments/vOm1Wv6Fgs6sfR8R/elasticsearch-analysis-ik-7.17.6.zip)

> es使用 docker-compose 部署

```sql
version: '3.8'

services:
  elasticsearch:
    image: elasticsearch:7.17.6
    container_name: es
    restart: always
    ports:
      - "9200:9200"
    environment:
      - node.name=es
      - cluster.name=es-cluster
      - discovery.type=single-node
      - bootstrap.memory_lock=true
      - "ES_JAVA_OPTS=-Xms512m -Xmx512m"
    ulimits:
      memlock:
        soft: -1
        hard: -1
    volumes:
      - ./docker_data/es/data:/usr/share/elasticsearch/data
      - ./docker_data/es/plugins:/usr/share/elasticsearch/plugins

  kibana:
    image: kibana:7.17.6
    container_name: kibana
    restart: always
    ports:
      - "5601:5601"
    environment:
      - ELASTICSEARCH_HOSTS=http://elasticsearch:9200
      - I18N_LOCALE=zh-CN
    depends_on:
      - elasticsearch
```

启动命令：

**<font style="color:rgb(0, 0, 0);">docker-compose up -d</font>**

<font style="color:rgb(0, 0, 0);">测试访问：</font>

<font style="color:rgb(0, 0, 0);">ES：</font><http://localhost:9200>

<font style="color:rgb(0, 0, 0);">Kibana：</font>[<font style="color:rgb(0, 87, 255);background-color:rgba(0, 0, 0, 0);">http://localhost:5601</font>](http://localhost:5601)

<font style="color:rgb(0, 0, 0);">在容器内安装分词器：</font>

```sql
docker exec es bash -c "bin/elasticsearch-plugin install https://github.com/medcl/elasticsearch-analysis-ik/releases/download/v7.17.6/elasticsearch-analysis-ik-7.17.6.zip"
```

<font style="color:rgb(0, 0, 0);">重启es：</font>

<font style="color:rgb(0, 0, 0);">docker-compose restart es</font>

## Canal搭建与配置

注意：如果你还没有搭建canal，运行时需要先把networkdisk-business/networkdisk-files/src/main/resources/canal.yml中的com.disk.canal.enable 改成 false，不然 files 服务会一直尝试连 Canal。

### canal-deployer搭建

下载地址：<https://github.com/alibaba/canal/releases>

#### 修改conf/canal.properties

![1759759790410-ba162625-4da4-4fb6-b5df-a8c0cb43e3b7.png](./img/vOm1Wv6Fgs6sfR8R/1759759790410-ba162625-4da4-4fb6-b5df-a8c0cb43e3b7-774814.png)

#### 修改conf/example/instance.properties

```sql
canal.instance.master.address=127.0.0.1:3306
canal.instance.dbUsername=canal
canal.instance.dbPassword=canal
canal.instance.connectionCharset = UTF-8
canal.instance.filter.regex=.*\\..*
```

之后进入到bin目录下，执行./startup.sh即可（启动时使用的是java8，如果使用其他版本需要将startup.sh中的不支持配置去除），看到`the canal server is running now ......`表示启动成功。

### canal-adaper搭建

下载地址：<https://github.com/alibaba/canal/releases>

#### 修改application.properties：

```sql
srcDataSources:
    defaultDS:
      url: jdbc:mysql://127.0.0.1:3306/network_disk?useUnicode=true
      username: canal
      password: canal
  canalAdapters:
  - instance: example # canal instance Name or mq topic name
    groups:
    - groupId: g1
      outerAdapters:
      - name: logger
      - name: es7
        hosts: http://127.0.0.1:9200 # 127.0.0.1:9200 for rest mode
        properties:
          mode: rest # or rest
#          # security.auth: test:123456 #  only used for rest mode
          cluster.name: es01
```

#### 修改es7/mytest\_user.yml

因为使用的是es7，所以在es7文件夹修改mytest\_user配置

```sql
dataSourceKey: defaultDS
destination: example
groupId: g1
esMapping:
  _index: user_file_index  # 自己在es上创建的索引
  _id: id                  # 对应的id
#  upsert: true
  pk: id
  sql: "select t.id as id,
       t.user_id as user_id,
       t.parent_id as parent_id,
       t.real_file_id as real_file_id,
       t.filename as filename,
       t.folder_flag as folder_flag,
       t.file_size_desc as file_size_desc,
       t.file_type as file_type,
       t.create_user as create_user,
       t.gmt_create as gmt_create,
       t.gmt_modified as gmt_modified,
       t.update_user as update_user,
       t.deleted as deleted,
       t.lock_version as lock_version 
from user_file as t "
#  objFields:
#    _labels: array:;
#  etlCondition: "where a.c_time>={}"
  commitBatch: 3000

```

#### 新增配置

> 在es7目录下新增同步的文件配置，用于导入存量数据

[附件: user\_file.yml](./attachments/vOm1Wv6Fgs6sfR8R/user_file.yml)

```java
curl http://127.0.0.1:8081/etl/es7/user_file.yml -X POST
```

![1759847472037-cf1e66ad-3a7e-4d89-96b2-ed8eb213fa65.png](./img/vOm1Wv6Fgs6sfR8R/1759847472037-cf1e66ad-3a7e-4d89-96b2-ed8eb213fa65-537863.png)

## 同步范围与索引设计

<font style="color:rgb(0, 0, 0);background-color:rgba(0, 0, 0, 0);">设计了</font><code><font style="color:rgb(0, 0, 0);background-color:rgba(0, 0, 0, 0);">user_file_index</font></code>索引，采用单分片零副本适配本地开发环境，集成 IK 分词器，定义了<code><font style="color:rgb(0, 0, 0);background-color:rgba(0, 0, 0, 0);">ik_max</font></code>、<code><font style="color:rgb(0, 0, 0);background-color:rgba(0, 0, 0, 0);">ik_smart</font></code>两种分词规则，这样可以满足文件名中文模糊检索需求；

字段层面结合业务场景合理划分数据类型，文件名称设为 text 类型支持分词检索，同时附加 keyword 子字段用于精准匹配、排序与聚合，日期、数值、布尔等字段按实际业务属性规范定义，兼顾数据存储规范性与检索性能，适配网盘文件列表查询、全局文件搜索、分类筛选等核心业务场景。

在 Kibana 里执行下列代码以创建索引，看到输出<font style="color:rgb(0, 0, 0);">"acknowledged" : true则代表创建成功</font>

```xml
PUT user_file_index
{
  "settings": {
    "number_of_shards": 1,
    "number_of_replicas": 0,
    "analysis": {
      "analyzer": {
        "ik_smart": {
          "tokenizer": "ik_smart"
        },
        "ik_max": {
          "tokenizer": "ik_max_word"
        }
      }
    }
  },
  "mappings": {
    "properties": {
      "id": { "type": "long" },
      "user_id": { "type": "long" },
      "parent_id": { "type": "long" },
      "real_file_id": { "type": "long" },
      "filename": {
        "type": "text",
        "analyzer": "ik_max",
        "fields": {
          "keyword": { "type": "keyword", "ignore_above": 256 }
        }
      },
      "folder_flag": { "type": "boolean" },
      "file_size_desc": { "type": "keyword" },
      "file_type": { "type": "integer" },
      "create_user": { "type": "long" },
      "gmt_create": { "type": "date", "format": "yyyy-MM-dd HH:mm:ss||strict_date_optional_time" },
      "gmt_modified": { "type": "date", "format": "yyyy-MM-dd HH:mm:ss||strict_date_optional_time" },
      "update_user": { "type": "long" },
      "deleted": { "type": "integer" },
      "lock_version": { "type": "integer" }
    }
  }
}

```

## 验证

写入数据：

```sql
INSERT INTO user_file (
  user_id, parent_id, real_file_id, filename, folder_flag, file_size_desc, file_type,
  create_user, gmt_create, gmt_modified, update_user, deleted, lock_version
)
VALUES
-- 顶层文件夹
(1001, 0, 0, '工作文档', 1, '-', 0, 1001, NOW(), NOW(), 1001, 0, 1),(1001, 0, 0, '图片', 1, '-', 0, 1001, NOW(), NOW(), 1001, 0, 1);
```

![1759758588398-849855b2-b060-468f-8327-4cb95b5de056.png](./img/vOm1Wv6Fgs6sfR8R/1759758588398-849855b2-b060-468f-8327-4cb95b5de056-924626.png)


> 更新: 2026-04-25 14:47:55  
> 原文: <https://www.yuque.com/chengxuyuancarl/edybxb/rg5uoso5y6r7xtxb>