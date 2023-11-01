# B站前端学习

# 第一章 HTML认知

## Web标准

![image-20230713190450087](https://img-blog.csdnimg.cn/c333b561b3fd42dcb30afa58d9cfe6dc.png)

 可以这么理解：

![image-20230713190721251](https://img-blog.csdnimg.cn/b5c5359f08eb4bf0a6a8cb7af77fcc5c.png)

## HTML界面固定结构

<img src="https://img-blog.csdnimg.cn/0464c45a5e884f0fb5f2894190b51452.png" alt="image-20230713192235645" style="zoom: 80%;" />

html骨架

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>

</body>

</html>
```

## 标签

### 标签结构

分为单标签和双标签

![image-20230713195549054](https://img-blog.csdnimg.cn/897af8da703d4eeb9f4fafe18843abe3.png)

### 排版标签

#### 标题标签( hi )

**1到6级标签，重要程度一次递减!!!**

**独占一行；文字加粗；并且变大，并且大小逐渐减小**

![image-20230713205524191](https://img-blog.csdnimg.cn/fe6f99628034420bb93c9efce60caf38.png)

#### 段落标签( pi )

**段落结束了提到下一行；并且段落之间存在间隙**

![image-20230713205545253](https://img-blog.csdnimg.cn/eceab8176fac4f56a03d7fe48f327fb6.png)

#### 换行标签(单标签 br)

**单标签，让文字强制换行!!!**

![image-20230716145657156](https://img-blog.csdnimg.cn/1622e4db356c43b3b6a1583155c4700e.png)

#### 水平分割线标签(单标签 hr)

**单标签，在页面中显示一条水平线!!!**

![image-20230716150000641](https://img-blog.csdnimg.cn/c3108598d27b45599b602992c0c0e2c3.png)

示例代码：

![image-20230716150107760](https://img-blog.csdnimg.cn/24c4987ae967494b9e56979080c8d30a.png)

### 文本格式化标签

![image-20230716150217806](https://img-blog.csdnimg.cn/d27df48f95c44f4f8f6c3413a1561363.png)

#### 标签语义化

<img src="https://img-blog.csdnimg.cn/31525605abfd47149cba93dc14645c42.png" alt="image-20230716151012659" style="zoom:67%;" />

### 媒体标签

#### 图片标签(img 单标签)

![image-20230716151239502](https://img-blog.csdnimg.cn/a9475a20e5e8459ab0d7e7ba79ece5d9.png)

**一个标签可以有多个属性；标签的多个属性之间用空格隔开；标签名和属性之间用空格隔开；属性之间没有顺序之分**

![image-20230716151935794](https://img-blog.csdnimg.cn/66a45d8849c844a594554150d20c9487.png)

##### src属性

![image-20230716152106584](https://img-blog.csdnimg.cn/a327debe806d4d59a5f485ec537f3bef.png)

##### alt属性

**当图片加载失败的时候显示的替换文本**

![image-20230716152119942](https://img-blog.csdnimg.cn/78f38ed45d8b47f3ac30d8f2a067302c.png)

##### title属性

**当鼠标悬停的时候显示的文本**

![image-20230716152506966](https://img-blog.csdnimg.cn/2d61994efb0747a6af6747dd5be73b65.png)

##### width和height属性

**如果只设置了一个，另一个没设置会自动按照原图片的比例进行缩放；**

**如果强行设置，图片可能会发生变形**

![image-20230716152535142](https://img-blog.csdnimg.cn/7c4f404fed6040a6b0bf2265627378e0.png)

#### 路径

![image-20230716155446557](https://img-blog.csdnimg.cn/1e6da5148fd7448380d47d51bd8e704f.png)

##### 绝对路径

![image-20230716155512817](https://img-blog.csdnimg.cn/eafcb7e7b3e044d68d033020a21f423f.png)

##### 相对路径

<img src="https://img-blog.csdnimg.cn/e6e2dc4eebd24a19bc59d344caee16ae.png" alt="image-20230716155537620" style="zoom: 67%;" />

同级

![image-20230716155726732](https://img-blog.csdnimg.cn/e99c390f403e4420a2e9628bffb871ec.png)

下级

![image-20230716155813139](https://img-blog.csdnimg.cn/51eb2e315a1942ea95f1ad6355505d83.png)

上级

![image-20230716155843340](https://img-blog.csdnimg.cn/1aa8005a013b4261bba2ee984e66d74d.png)

#### 音频标签(audio 双标签)

![image-20230716161310793](https://img-blog.csdnimg.cn/7f5600b95ff747799c7d9f48162346ee.png)

~~~html
<audio src="music.mp3" controls autoplay loop></audio>
~~~

#### 视频标签(video 双标签)

![image-20230716161249035](https://img-blog.csdnimg.cn/b7bfc0afe99047ffbcaa43ced8e743ed.png)

~~~html
<!-- 谷歌浏览器可以让视频自动播放，但是必须是静音状态 muted -->
<video src="video.mp4" controls autoplay muted loop></video>
~~~

#### 链接标签(a 双标签)

**href属性里面写的是需要跳转的html文件，然后文本内容就是需要的超链接文本**

![image-20230716161446284](https://img-blog.csdnimg.cn/3baed2719f984b6b8307df9dee4460af.png)

##### target属性

**可以决定是覆盖原网页还是保留原网页**

![image-20230716162300004](https://img-blog.csdnimg.cn/ba5cca48042847a0a95765c0800a2260.png)

~~~html
<a href="https://www.baidu.com" target="_blank">跳转到百度</a>
<br>
<a href="./01_标题标签.html" target="_self">跳转到01_标题标签</a>

<!-- 网站开发初期，还不知道跳转地址的时候，href的书写为空链接 # ，就是写一个#号 -->
<br>
<a href="#">空链接，不知道跳转到哪里</a>
~~~

## 第二章 HTML基础

### 列表

**展示网页当中具有关联性的内容，按照行的方式展示整齐的内容!!!**

**三种列表标签：无序列表，有序列表，自定义列表**

#### 使用场景

![image-20230716170638077](https://img-blog.csdnimg.cn/7689ef3e766647d891b61b2eba5d8668.png)

#### 无序列表(ul li)

**ul只能包含li标签；li中可以包含任意内容**

**列表的每一项默认都是圆点标识**

![image-20230716170919224](https://img-blog.csdnimg.cn/15a7a355ddc442da8db15d39ab354858.png)

~~~html
<ul>
    <li><strong>榴莲</strong></li>
    <li>香蕉</li>
    <li>苹果</li>
    <li>哈密瓜</li>
    <li>火龙果</li>
</ul>
~~~

#### 有序列表(ol li)

**ol只能包含li标签；li中可以包含任意内容**

**列表的每一项默认都是显示的序号(排序)标识，这个顺序是自己指定的!!!!没有自动排序机制**

![image-20230716171425678](https://img-blog.csdnimg.cn/50faeac419814008ab3f81c50eee8ab4.png)

~~~html
<ol>
	<!-- 这个顺序是我们自己指定的，只是看起来具有顺序 -->
	<li><strong>小明：100分</strong></li>
	<li>小张：110分</li>
	<li>小李：60分</li>
</ol>
~~~

#### 自定义列表(dl dt dd)

![image-20230716171953399](https://img-blog.csdnimg.cn/a9afc744dfa94ed09cedcc70177840b3.png)

~~~html
<dl>
	<dt><strong>帮助中心</strong></dt>
	<dd>账户管理</dd>
	<dd>购物指南</dd>
	<dd>订单操作</dd>
</dl>
~~~

### 表格

#### 标签(table tr td)

**table 嵌套 tr 嵌套 td**

**table表示表格整体；tr表示表格每行；td表示表格每个单元格**

![image-20230716172355571](https://img-blog.csdnimg.cn/cccb7f6dd88d472bb7cd5a1dc982e7ac.png)

~~~html
<table>
    <tr>
        <td>姓名</td>
        <td>成绩</td>
        <td>评语</td>
    </tr>
    <tr>
        <td>小哥哥</td>
        <td>100分</td>
        <td>小哥哥真帅气</td>
    </tr>
    <tr>
        <td>小姐姐</td>
        <td>100分</td>
        <td>小姐姐真漂亮</td>
    </tr>
    <tr>
        <td>总结</td>
        <td>郎才女貌</td>
        <td>郎才女貌</td>
    </tr>
</table>
~~~

这行代码执行结果是：

![image-20230716172826076](https://img-blog.csdnimg.cn/ff4da4f4a8b94793ad000953a5b4f80c.png)

**发现没有格子，那么需要加入table的其他属性才能实现**

#### 属性(border width height)

**width 和 height 属性一般不是在这里设置的，实际开发的时候一般用css进行设置**

![image-20230716172957442](https://img-blog.csdnimg.cn/a172a5f7925f43f1844afefa57a7a28c.png)

#### 表格标题和表头单元格标签

![image-20230716173322631](https://img-blog.csdnimg.cn/88a95950ad5f47d79a0e512fc7983693.png)

**表头单元格th放在 tr 段的第一个，代表第一行，然后替换掉里面的td标签就可以了，就代表是表头单元格标签；也就是 th 和 td 的地位是相同的**

#### 表格的结构标签(了解)

**表格结构标签内部用来包裹tr标签(代表将这些 tr 进行分类，哪些是头，身体和底部)；并且结构标签可以忽略**

![image-20230716173941983](https://img-blog.csdnimg.cn/14afb3f98e0d4945bf8094ed80865364.png)

~~~html
    <table border="1" width="500" height="300">
        <caption><strong>学生成绩单</strong></caption>
        <thead>
            <tr>
                <th>姓名</th>
                <th>成绩</th>
                <th>评语</th>
            </tr>
        </thead>

        <tbody>
            <tr>
                <td>小哥哥</td>
                <td>100分</td>
                <td>小哥哥真帅气</td>
            </tr>
            <tr>
                <td>小姐姐</td>
                <td>100分</td>
                <td>小姐姐真漂亮</td>
            </tr>
        </tbody>

        <tfoot>
            <tr>
                <td>总结</td>
                <td>郎才女貌</td>
                <td>郎才女貌</td>
            </tr>
        </tfoot>
    </table>
~~~

执行结果：

<img src="https://img-blog.csdnimg.cn/f19cdf0d3c6340a988e6af6a209666dc.png" alt="image-20230716174513211" style="zoom:150%;" />

#### 合并单元格(rowspan colspan 单元格的属性设置)

**水平或者垂直合并单元格**

![image-20230716174639702](https://img-blog.csdnimg.cn/afedfc4fcd054fe981fdce64669269a7.png)

**注意：只有同一个结构标签当中的单元格才能合并!!!**

**上下合并：保留最上面的，删除其他的 rowspan**

**左右合并：保留最左边的，删除其他的 colspan**

![image-20230716174756446](https://img-blog.csdnimg.cn/0418ab74e3a940b291b79a967cc347ff.png)

**步骤：确定保留谁；在代码中删除对应的 td 单元格；然后修改保留单元格的属性**

~~~html
<table border="1" width="500" height="300">
    <caption><strong>学生成绩单</strong></caption>
    <thead>
        <tr>
            <th>姓名</th>
            <th>成绩</th>
            <th>评语</th>
        </tr>
    </thead>

    <tbody>
        <tr>
            <td>小哥哥</td>
            <td rowspan="2">
                100分
                <!-- 这一格进行单元格的合并操作 -->
                <!-- 和下面一个进行合并，那么先要把下面的一个单元格删除掉 -->
                <!-- 然后修改单元格的属性操作 2代表合并的单元格数量 -->
            </td>
            <td>小哥哥真帅气</td>
        </tr>
        <tr>
            <td>小姐姐</td>

            <td>小姐姐真漂亮</td>
        </tr>
    </tbody>

    <tfoot>
        <tr>
            <td>总结</td>
            <td colspan="2">郎才女貌</td>
        </tr>
    </tfoot>
</table>
~~~

执行结果：

![image-20230716175636296](https://img-blog.csdnimg.cn/42d6463636e942ba93e14c3b3c561f90.png)