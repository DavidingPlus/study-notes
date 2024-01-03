/*
 * serverdef.h
 *
 *  Created on: 2016年3月24日
 *      Author: mengbaoliang
 */

#ifndef SEVDEF_H_
#define SEVDEF_H_

//服务器IP和端口
#define SERVER_IP "123.56.88.172"
#define SERVER_PORT 9999
//配置文件路径
#define CONFIG_PATH "/home/applications/server/conf/server.conf"
//404错误页面地址
#define ERROR_PAGE_PATH "/home/applications/server/pages"
//网站根目录路径
#define ROOT_PATH "/home/applications/server/www"
//线程池初始化线程数量
#define THREAD_POOL_MAX_NUM 10

//PHP路径
#define PHP "/alidata/server/php/bin/php"

//环境变量


//数据库
#define MYSQL_HOST "127.0.0.1"
#define MYSQL_DPORT 0
#define MYSQL_DATABASE "mysite"
#define MYSQL_USER "root"
#define MYSQL_PASSWARD "jay332572"


//c代码标示
#define C_LEFT_FLAG "<?ccode>"
#define C_RIGHT_FLAG "</ccode>"
#define C_REPLACE_FLAG "{$CREPACE$}"
#define TEMPLATE_REPALCE_FALG "{#ccode#}"


//shell脚本位置
#define SHELL_EXCUTE_COMMAND "sh /home/applications/server/parsec.sh"
#define C_TEMPLATE_PATH "/home/applications/server/c.template.txt"

//扩展名


//相应报文格式
#define RESPONSE_CONTENTTYPE_FORMAT "Content-Type: %s;charset=UTF-8\r\n"  //相应报文内容格式
#define RESPONSE_STATUS_FORMAT "HTTP/1.1 %d OK\r\n" //状态报文格式
#define RESPONSE_CONTENTLENGTH_FORMAT "Content-Length: %ld\r\n\r\n" //内容长度报文

//内容类型
#define HTML_CONTENT_TYPE "text/html"  //HTML格式类型
#define CSS_CONTENT_TYPE "text/css"  //css格式类型
#define JPEG_CONTENT_TYPE "image/jpeg"  //jpeg格式类型
#define JPG_CONTENT_TYPE "application/x-jpg"  //jpg格式类型
#define PNG_CONTENT_TYPE "application/x-png"  //jpg格式类型

//http://tool.oschina.net/commons/





#endif /* SEVDEF_H_ */
