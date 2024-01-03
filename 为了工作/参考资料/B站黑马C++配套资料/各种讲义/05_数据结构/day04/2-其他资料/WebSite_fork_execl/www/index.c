
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
	strcat(htmlText,"				<div class='post'>");
	strcat(htmlText,"					<p class='meta'><span class='date'>Sunday, April 26, 2016</span> 7:27 AM Posted by <a href='#'>管理员</a></p>");
	strcat(htmlText,"					<h2 class='title'><a href='#'>楼市地产去库存：因城施策模式开启 </a></h2>");
	strcat(htmlText,"					<div class='entry'>");
	strcat(htmlText,"						<p>2016年两会期间，江西省九江市副市长卢天锡带来了一份《关于供需两侧同时发力有效化解房地产库存》的提案，他提议既要在需求侧发力，又要在供给侧调节，支持调整房地产土地用途，适当延长房企开发时限，改善房地产用地供应管理。");
	strcat(htmlText,"　　今年2月6日，江西省出台了二十条去库存政策，从购房补贴、棚改货币化安置、降低税费、到释放住房公积金、降低首套房首付比例至20%%等二十种调控手段应对2016年的去库存之战。其中提到，商品房库存严重过剩的市、县，土地管理部门对未动工开发的房地产用地，可依法采取符合当地房地产市场实际的调控措施。</p>");
	strcat(htmlText,"					</div>");
	strcat(htmlText,"				</div>");
	strcat(htmlText,"				<div class='post'>");
	strcat(htmlText,"					<p class='meta'><span class='date'>Sunday, April 26, 2016</span> 7:27 AM Posted by <a href='#'>管理员</a></p>");
	strcat(htmlText,"					<h2 class='title'><a href='#'>男子痛失珍贵黑币,价值200万美元</a></h2>");
	strcat(htmlText,"					<div class='entry'>");
	strcat(htmlText,"						<p> 美国圣迭戈市男子兰德尔·劳伦斯从曾是造币厂员工的父亲那里继承了一枚价值不菲的1974年版铝制硬币。然而，一名联邦检察官17日表示，这枚现在收藏市场价值达200万美元的硬币所有权归造币厂。劳伦斯原打算同硬币商店店主迈克尔·麦康奈尔一起举办这枚硬币的“全国巡展”，然后通过拍卖公司将其出售。联邦检察官劳拉·迪菲说，这枚硬币其实是一枚“黑货币”。1973年，由于铜价大涨，美国国会批准造一批铝制硬币。但是，在制造出大约160万枚硬币后，国会议员们最终却没有批准这些硬币入市流通。造币厂只好回收并销毁了其中大部份，但仍有一些“漏网之鱼”。按规定，美国造币厂制造的货币如果没有合法发行，其所有权归美国政府，造币厂职员等无权占有并继承这种货币。</p>");
	strcat(htmlText,"					</div>");
	strcat(htmlText,"				</div>");
	strcat(htmlText,"				<div class='post'>");
	strcat(htmlText,"					<p class='meta'><span class='date'>Sunday, April 26, 2009</span> 7:27 AM Posted by <a href='#'>Someone</a></p>");
	strcat(htmlText,"					<h2 class='title'><a href='#'>落实难民潮协议,欧盟迈出“关键一步”</a></h2>");
	strcat(htmlText,"					<div class='entry'>");
	strcat(htmlText,"						<p>法国总统弗朗索瓦·奥朗德宣布,欧盟28国领导人17日就难民潮协议达成一致立场,虽然谈判持续至深夜,“但是终于取得了成果”。由于欧盟内部先前的分歧不小,这一最新进展令不少人舒了一口气。德国总理安格拉·默克尔曾预测,欧盟各国就难民潮协议取得共同立场并非易事,但是各国均已认识到,眼下急需落实这份协议才能减少难民涌入。根据欧盟与土耳其8日就应对难民潮达成的原则协议,欧盟将遣返全部由土耳其进入希腊的“新的非法移民”并且承担全部费用;在现有协议框架下,土耳其从希腊每接收一名“叙利亚人”,欧盟就将安置一名来自土耳其的叙利亚难民。</p>");
	strcat(htmlText,"					</div>");
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

	//初始化数据连接
	MysqlConn* conn = Init_Mysql();
	if(conn == NULL){
		strcat(htmlText,"数据库连接失败!\n");
	}
	//从数据库获得数据
	Query_Mysql(conn,"SELECT * FROM news limit 0,20");
	int i=0,j=0;
	for(;i<Size_LinkList(conn->result);i++){
		//获得第一行数据
		MysqlLine* line =  (MysqlLine*)Get_LinkList(conn->result,i);
		char htmlLine[1024] = {0};
		sprintf(htmlLine,"<li><a href='news?id=%s'>%s</a></li>",GetByField_Mysql(line,"news_id"),GetByField_Mysql(line,"news_title"));
		strcat(htmlText,htmlLine);
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
	//close(sock);
	return 0;


}
