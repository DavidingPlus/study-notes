/*
 * news.c
 *
 *  Created on: 2016年3月29日
 *      Author: mengbaoliang
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "../libs/chttp.h"
#include "../libs/cmysql.h"
#include "../libs/csocket.h"

int main(int agv,char* args[]){

	int sock = atoi(args[0]);
	char* params = args[1];

	//--------------------------------------------------------
	if(agv <= 1){
		printf("参数为空!\n");
	}


	char param[128] = {0};
	if (GetParamValueByKey(params,"id",param) == -1){
		printf("请求参数不存在!\n");
	}else{
		printf("请求地址参数param:%s\n",param);
	}

	//初始化数据连接
	MysqlConn* conn = Init_Mysql();
	if(conn == NULL){
		printf("数据库连接失败!\n");
	}
	char sql[1024]={0};
	strcat(sql,"SELECT * FROM news WHERE news_id=");
	strcat(sql,param);

	Query_Mysql(conn,sql);

	//--------------------------------------------------------

	char htmlText[40960] = {0};
	strcat(htmlText,"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>");
	strcat(htmlText,"<html xmlns='http://www.w3.org/1999/xhtml'>");
	strcat(htmlText,"<head>");
	strcat(htmlText,"<meta http-equiv='content-type' content='text/html; charset=utf-8' />");
	strcat(htmlText,"<title>我的网站</title>");
	strcat(htmlText,"<meta name='keywords' content='' />");
	strcat(htmlText,"<meta name='description' content='' />");
	strcat(htmlText,"<link href='style.css' rel='stylesheet' type='text/css' media='screen' />");
	strcat(htmlText,"</head>");
	strcat(htmlText,"<body>");
	strcat(htmlText,"<div id='header'>");
	strcat(htmlText,"	<div id='menu'>");
	strcat(htmlText,"		<ul>");
	strcat(htmlText,"			<li class='current_page_item'><a href='index' class='first'>首页</a></li>");
	strcat(htmlText,"			<li><a href='/news'>新闻</a></li>");
	strcat(htmlText,"			<li><a href='/message'>留言</a></li>");
	strcat(htmlText,"		</ul>");
	strcat(htmlText,"	</div>");
	strcat(htmlText,"	<!-- end #menu -->");
	strcat(htmlText,"	<div id='search'>");
	strcat(htmlText,"		<form method='get' action=''>");
	strcat(htmlText,"			<fieldset>");
	strcat(htmlText,"				<input type='text' name='s' id='search-text' size='15' />");
	strcat(htmlText,"			</fieldset>");
	strcat(htmlText,"		</form>");
	strcat(htmlText,"	</div>");
	strcat(htmlText,"	<!-- end #search -->");
	strcat(htmlText,"</div>");
	strcat(htmlText,"<!-- end #header -->");
	strcat(htmlText,"<!-- end #header-wrapper -->");
	strcat(htmlText,"<div id='logo'>");
	strcat(htmlText,"	<h1><a href='#'>我的网站 </a></h1>");
	strcat(htmlText,"	<p><em> 做你爱看的 <a href='#'> 专业的新闻网站</a></em></p>");
	strcat(htmlText,"</div>");
	strcat(htmlText,"<hr />");
	strcat(htmlText,"<!-- end #logo -->");
	strcat(htmlText,"<div id='page'>");
	strcat(htmlText,"	<div id='content'>");
	strcat(htmlText,"		<div id='content-bgtop'>");
	strcat(htmlText,"			<div id='content-bgbtm'>");
	strcat(htmlText,"				<div class='post'>");

	//--------------------------------------------------------

	char htmlLine[40960] = {0};
	MysqlLine* dataline = (MysqlLine*)Get_LinkList(conn->result,0);
	strcat(htmlText,"					<p class='meta'><span class='date'>Sunday, April 26, 2016</span> 7:27 AM Posted by <a href='#'>管理员</a></p>");
	sprintf(htmlLine,"						<h2 class='title'><a href='#'>%s </a></h2>",GetByField_Mysql(dataline,"news_title"));
	strcat(htmlText,htmlLine);
	memset(htmlLine,0,40960);
	strcat(htmlText,"					<div class='entry'>");
	sprintf(htmlLine,"						<p>&nbsp;&nbsp;&nbsp;&nbsp;%s</p>",GetByField_Mysql(dataline,"news_content"));
	strcat(htmlText,htmlLine);
	strcat(htmlText,"					</div>");

	//--------------------------------------------------------

	strcat(htmlText,"				</div>");
	strcat(htmlText,"			</div>");
	strcat(htmlText,"		</div>");
	strcat(htmlText,"	</div>");
	strcat(htmlText,"	<!-- end #content -->");
	strcat(htmlText,"	<div id='sidebar'>");
	strcat(htmlText,"		<ul>");
	strcat(htmlText,"			<li>");
	strcat(htmlText,"				<h2>网站简介</h2>");
	strcat(htmlText,"				<p>&nbsp;&nbsp;&nbsp;&nbsp;我的网站始建于2016年。网站立足于本地热点新闻，为本地用户群提供咨询服务。同时，我的网站网也是一个信息咨询分享平台，利用网络媒体和互联网平台的互补优势为客户提供综合性、连续性的网络营销宣传服务。</p>");
	strcat(htmlText,"			</li>");
	strcat(htmlText,"			<li>");
	strcat(htmlText,"				<h2>热门新闻 </h2>");
	strcat(htmlText,"				<ul>");

	//--------------------------------------------------------

	//从数据库获得数据
	Query_Mysql(conn,"SELECT * FROM news limit 0,20");
	int i=0,j=0;
	for(;i<Size_LinkList(conn->result);i++){
		//获得第一行数据
		MysqlLine* line =  (MysqlLine*)Get_LinkList(conn->result,i);
		memset(htmlLine,0,1024);
		sprintf(htmlLine,"<li><a href='news?id=%s'>%s</a></li>",GetByField_Mysql(line,"news_id"),GetByField_Mysql(line,"news_title"));
		strcat(htmlText,htmlLine);
		memset(htmlLine,0,1024);
	}

	//--------------------------------------------------------
	strcat(htmlText,"				</ul>");
	strcat(htmlText,"			</li>");
	strcat(htmlText,"		</ul>");
	strcat(htmlText,"	</div>");
	strcat(htmlText,"	<!-- end #sidebar -->");
	strcat(htmlText,"	<div style='clear: both;'>&nbsp;</div>");
	strcat(htmlText,"	<!-- end #page -->");
	strcat(htmlText,"	<div id='footer'>");
	strcat(htmlText,"		<p>Copyright (c) 2016 wodewangzhan.com All rights reserved. Design by <a href='#'>我的网站</a>.</p>");
	strcat(htmlText,"	</div>");
	strcat(htmlText,"	<!-- end #footer -->");
	strcat(htmlText,"</div>");
	strcat(htmlText,"</body>");
	strcat(htmlText,"</html>");
	strcat(htmlText,"<span style='display:none;'><a href='#' title=''></a></span><span style='display:none;'><a href='#' title=''></a></span>");


	//文本长度
	long len = strlen(htmlText) + 1;
	char buf[512] = {0};
	sprintf(buf,"Content-Length: %ld\r\n\r\n",len);
	char responseText[40960] = {0};
	strcat(responseText,"HTTP/1.1 200 OK\r\n");
	strcat(responseText,"Content-Type: text/html;charset=UTF-8\r\n");
	strcat(responseText,buf);
	strcat(responseText,htmlText);

	int responseLen = strlen(responseText) + 1;
	send(sock,responseText,responseLen,0);

	FreeResult_Mysql(conn);
	Destroy_Mysql(conn);


	return 0;
}
