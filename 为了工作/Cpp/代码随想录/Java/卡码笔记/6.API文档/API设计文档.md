# API 设计文档

## API 风格

卡码笔记所有 API 都遵循 Restful API 风格，Restful API 详情请参考：

[REST API 设计规范：最佳实践和示例](https://apifox.com/apiskills/rest-api-design-specification/)

## 基础请求路径

在开发环境下，API 的请求的基础路径为：`http://localhost:8080/api`

## 用户身份鉴别

卡码笔记的前端在向后端发送 HTTP 请求时，会自动在请求头加上 `Authorization` 字段，字段内容是当前登录用户的 token，后端根据该 token 鉴别用户身份。

```http
Authorization: Bearer <token>
```

## 数据统一响应模型

```json
{
  "code": 200,          // 状态码：200=成功，其他=失败
  "message": "成功",     // 响应消息
  "data": {},           // 响应数据
  "token": "xxx"        // JWT token（仅在登录/注册时返回）
}
```


> 更新: 2025-05-29 16:24:13  
> 原文: <https://www.yuque.com/chengxuyuancarl/px6ppg/ophvm9bmcd1449vp>