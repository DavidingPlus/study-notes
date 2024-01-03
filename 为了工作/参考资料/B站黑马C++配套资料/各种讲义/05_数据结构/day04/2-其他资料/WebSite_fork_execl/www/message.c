
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "../libs/chttp.h"
#include "../libs/cmysql.h"
#include "../libs/csocket.h"


int main(int agv,char* args[]){

	int sock = atoi(args[0]);
#if 0
	strcat(htmlText,"AAAAAAAA:%d\n",sock);
	strcat(htmlText,"参数个数：%d\n",agv);
	int i=0;
	for(;i<agv;i++){
		strcat(htmlText,"参数值：%s\n",args[i]);
	}

#endif

	//响应报文
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

	int i=0;
	for(;i<10;i++){

		strcat(htmlText,"				<div class='post'>");
		strcat(htmlText,"					<p class='meta'><span class='date'>Sunday, April 26, 2016</span> 7:27 AM Posted by <a href='#'>悟空</a></p>");
		strcat(htmlText,"					<h2 class='title'><a href='#'>啥时候开始啊？ </a></h2>");
		strcat(htmlText,"					<div class='entry'>");
		strcat(htmlText,"						<p>&nbsp;&nbsp;&nbsp;&nbsp;有山无水难成景，有酒无朋难聚欢；曾经沧海成桑田，情意交心亘不变；红梅飘香话思念，惹落雪花两三片；大好时光多缠绵，愿你快乐每一天！</p>");
		strcat(htmlText,"					</div>");
		strcat(htmlText,"				</div>");

	}


	strcat(htmlText,"			</div>");
	strcat(htmlText,"		</div>");
	strcat(htmlText,"	</div>");
	strcat(htmlText,"	<!-- end #content -->");
	strcat(htmlText,"	<div id='sidebar'>");
	strcat(htmlText,"		<ul>");
	strcat(htmlText,"			<li>");
	strcat(htmlText,"				<h2>留言注意事项</h2>");
	strcat(htmlText,"				<p>真诚地欢迎您为长丰的发展献计献策，对我们的工作提出意见和建议。为提高办事效率，明确责任，在您咨询、投诉或建议之前，请您认真阅读以下须知:");
	strcat(htmlText,"1.写信应遵守宪法、法律、法规、规章及社会公德，反映的对象要明确，内容要真实、客观，不得捏造、歪曲事实和诽谤、诬告、陷害他人。");
	strcat(htmlText,"2.为了提高办事效率，明确责任，请按问题的归属选择分类反映，同一问题不要多处发表。</p>");
	strcat(htmlText,"			</li>");
	strcat(htmlText,"			<li>");
	strcat(htmlText,"				<h2>在此留言</h2>");
	strcat(htmlText,"				<ul>");
	strcat(htmlText,"				<form action='/message' method='post'>");
	strcat(htmlText,"					<li>网名:&nbsp;<input type='text' name='iname' style='width:180px;height:15px;' value='' /></li>");
	strcat(htmlText,"					<li>问题:&nbsp;<input type='text' name='title' style='width:180px;height:15px;' /></li>");
	strcat(htmlText,"					<li><textarea name='content' style='width:210px;height:200px;'></textarea></li>");
	strcat(htmlText,"					<li><input type='submit' name='' value='提交'  />&nbsp;&nbsp;&nbsp;&nbsp;<input type='reset' name='' value='重置'  /></li>");
	strcat(htmlText,"				</form>");
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

	//FreeResult_Mysql(conn);
	//Destroy_Mysql(conn);
	//close(sock);
	return 0;


}
