# 【DOM】

> 原创内容，转载请注明出处！

# 一、DOM基本概念

DOM（Document Object Model，文档对象模型）是 JavaScript 操作 HTML 文档的接口，使文档操作变得非常优雅、简便。

DOM 最大的特点就是将 HTML 文档表示为 “节点树”。

DOM 元素/节点：就是渲染到页面上的，一个个的 HTML 标签体（标签 + 属性 + 内容）。

# 二、DOM节点树

```html
<!DOCTYPE html>
<html>

<head>
    <meta charset="UTF-8">
    <title>DOM</title>
</head>

<body>
    <h1>IMOOC</h1>
    <div>
        <h2>Coder Dream</h2>
        <img src="logo.png">
        <div class="box">
            文本内容
        </div>
    </div>
</body>

</html>
```

【DOM】

<img src="https://img-blog.csdnimg.cn/direct/36ff498975964540955615bce82bdce0.png" style="width: 50%;" />

> 整个 html 文档就对应一个 document 对象，可以操作 html 文档里面所有的标签及其属性和文本。

# 三、nodeType

节点的 nodeType 属性可以显示这个节点具体的类型。

`document.nodeType;	  // 9`

| nodeType值 | 节点类型                        |
| ---------- | ------------------------------- |
| 1          | 元素节点，例如 `<p>` 和 `<div>` |
| 3          | 文字节点                        |
| 8          | 注释节点                        |
| 9          | document 节点                   |
| 10         | DTD 节点（文档类型声明）        |

# 四、document

## 4.1 访问元素节点

所谓 “访问” 元素节点，就是指 “得到”、“获取” 页面上的元素节点。

对节点进行操作，第一步就是要得到它。

访问元素节点主要依靠 document 对象。

## 4.2 认识 document 对象

document 对象是 DOM 中最重要的东西，几乎所有 DOM 的功能都封装在了 document 对象中。

document 对象也表示整个 HTML 文档，它是 DOM 节点树的根。

document 对象的 nodeType 属性值是 9。

```javascript
typeof document;	// object
document.nodeType;	// 9
```

## 4.3 访问元素节点的常用方法

注意：以下方法的参数都是字符串类型 `''`。

| 方法                                | 功能                       | 兼容性                       |
| ----------------------------------- | -------------------------- | ---------------------------- |
| `document.getElementById()`         | 通过 id 得到**元素**       | IE 6                         |
| `document.getElementsByTagName()`   | 通过标签名得到**元素数组** | IE 6                         |
| `document.getElementsByClassName()` | 通过类名得到**元素数组**   | IE 9                         |
| `document.querySelector()`          | 通过选择器得到**元素**     | IE 8 部分兼容、IE 9 完全兼容 |
| `document.querySelectorAll()`       | 通过选择器得到**元素数组** | IE 8 部分兼容、IE 9 完全兼容 |

> Element：元素。
>
> query：查询。
>
> Selector：选择器。

## 4.4 getElementById()

`document.getElementById()` 功能是通过 id 得到元素节点。

- HTML

```html
<div id="box">我是一个盒子</div>
<p id="para">我是一个段落</p>
```

- JS

```javascript
var box = document.getElementById('box');
var para = document.getElementById('para');
```

【注意事项】

> 如果页面上有相同 id 的元素，则只能得到第一个。
>
> 原则上，html 中同一名称的 id 也只能出现一次。

## 4.5 延迟运行

通常 JS 代码要写到 HTML 结构的最后，否则 JS 无法找到相应的 DOM 节点。

可以使用 `window.onload = function(){}` 来延迟 JS 的执行，直到 HTML 文档加载完毕后（触发 window.onload 事件），再执行函数里的代码。

> 一般 script 标签会被放在头部或尾部。
>
> 头部就是 `<head></head>` 里面，尾部一般指 `</body>` 前，但也有放在 `</body>` 之后的（最好不要这样）！

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
    <div id="box">我是一个盒子</div>
    <p id="para">我是一个段落</p>
    <script>
        // 获取 DOM 节点
        var box = document.getElementById('box');
        var para = document.getElementById('para');
        // 输出获取到的 DOM 节点
        console.log(box);	// <div id="box">我是一个盒子</div>
        console.log(para);	// <p id="para">我是一个段落</p>
    </script>
</body>

</html>
```

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <script>
        // 获取 DOM 节点
        var box = document.getElementById('box');
        var para = document.getElementById('para');
        // 输出获取到的 DOM 节点
        // 由于 HTML 代码是顺序执行的，当执行到此处的 JS 代码时，后面 body 内的 DOM 节点还没来得及执行
        console.log(box);	// null
        console.log(para);	// null
    </script>
</head>

<body>
    <div id="box">我是一个盒子</div>
    <p id="para">我是一个段落</p>
</body>

</html>
```

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <script>
        // window.onload 事件：当 HTML 文档加载完毕后触发
        window.onload = function () {
            // 获取 DOM 节点
            var box = document.getElementById('box');
            var para = document.getElementById('para');
            // 输出获取到的 DOM 节点
            console.log(box);	// <div id="box">我是一个盒子</div>
            console.log(para);	// <p id="para">我是一个段落</p>
        }
    </script>
</head>

<body>
    <div id="box">我是一个盒子</div>
    <p id="para">我是一个段落</p>
</body>

</html>
```

## 4.6 getElementsByTagName()

`getElementsByTagName()` 方法的功能是通过标签名得到节点数组。

> 注意：得到的是一个数组！

```html
<p>我是段落</p>
<p>我是段落</p>
<p>我是段落</p>
<p>我是段落</p>
```

```javascript
var ps = document.getElementsByTagName('p');
```

【注意事项】

数组方便遍历，从而可以批量操控元素节点。

即使页面上只有一个指定标签名的节点，也将得到长度为 1 的数组。

任何一个节点元素也可以调用 getElementsByTagName() 方法，从而得到其内部的某种标签名的元素节点。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <script>
        window.onload = function () {
            var ps = document.getElementsByTagName('p');
            console.log(ps);	// HTMLCollection(6) [p, p, p, p, p, p]
        }
    </script>
</head>

<body>
    <div id="box1">
        <p>我是段落</p>
        <p>我是段落</p>
        <p>我是段落</p>
    </div>
    <div id="box2">
        <p>我是段落</p>
        <p>我是段落</p>
        <p>我是段落</p>
    </div>
</body>

</html>
```

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <script>
        window.onload = function () {
            // 先得到 box1
            var box1 = document.getElementById('box1');
            // 得到 box1 中的 p 标签的数组
            var ps_inbox1 = box1.getElementsByTagName('p');
            console.log(ps_inbox1);	// HTMLCollection(3) [p, p, p]
        }
    </script>
</head>

<body>
    <div id="box1">
        <p>我是段落</p>
        <p>我是段落</p>
        <p>我是段落</p>
    </div>
    <div id="box2">
        <p>我是段落</p>
        <p>我是段落</p>
        <p>我是段落</p>
    </div>
</body>

</html>
```

## 4.7 getElementsByClassName()

`getElementsByClassName()` 方法的功能是通过类名得到节点数组。

- HTML

```html
<div class="spec">我是盒子</div>
<div>我是盒子</div>
<div class="spec">我是盒子</div>
<div class="spec">我是盒子</div>
```

- JS

```javascript
var spec_divs = document.getElementsByClassName('spec');
```

【注意事项】

`getElementsByClassName()` 方法从 IE9 开始兼容。

某个节点元素也可以调用 getElementsByClassName() 方法，从而得到其内部的某类名的元素节点。

## 4.8 querySelector()

`querySelector()` 方法的功能是通过选择器得到元素。

- HTML

```html
<div id="box1">
	<p>我是段落</p>
    <p class="spec">我是段落</p>
    <p>我是段落</p>
</div>
```

- JS

```javascript
var the_p = document.querySelector('#box1 .spec');
```

【注意事项】

querySelector() 方法只能得到页面上一个元素，如果有多个元素符合条件，则只能得到第一个元素。

querySelector() 方法从 IE8 开始兼容，但从 IE9 开始支持 CSS3 的选择器，如：`nth-child()`、`:[src^='dog']` 等 CSS3 选择器形式都支持良好。

> 注意：不能选择伪类！

## 4.9 querySelectorAll()

`querySelectorAll()` 方法的功能是通过选择器得到元素数组。

即使页面上只有一个符合选择器的节点，也将得到长度为 1 的数组。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <ul id="list1">
        <li>我是li</li>
        <li>我是li</li>
        <li>我是li</li>
        <li>我是li</li>
        <li>我是li</li>
    </ul>
    <ul id="list2">
        <li>我是li</li>
        <li>我是li</li>
        <li>我是li</li>
        <li>我是li</li>
        <li>我是li</li>
    </ul>
    <script>
        var lis_inlist1 = document.querySelectorAll('#list1 li');

        console.log(lis_inlist1);	// NodeList(5) [li, li, li, li, li]
    </script>
</body>

</html>
```

# 五、节点的关系

<img src="https://img-blog.csdnimg.cn/direct/176878fb20684eacbd3656e0eb971a62.png" style="width: 50%;" />

| 关系           | 考虑所有结点      |
| -------------- | ----------------- |
| 子节点         | `childNodes`      |
| 父节点         | `parentNode`      |
| 第一个子节点   | `firstChild`      |
| 最后一个子节点 | `lastChild`       |
| 前一个兄弟节点 | `previousSibling` |
| 后一个兄弟节点 | `nextSibling`     |

【注意：文本节点也属于节点】

DOM 中，文本节点也属于节点，在使用节点的关系时一定要注意。

在标准的 W3C 规范中，空白文本节点也应该算作节点，但是在 IE8 及以前的浏览器中会有一定的兼容问题，它们不把空白文本节点当作节点。

【排除文本节点的干扰】

从 IE9 开始支持一些 “只考虑元素节点” 的属性。

> 如果考虑兼容性，可以通过后面的函数封装来实现。

| 关系           | 考虑所有结点      | 只考虑元素节点           |
| -------------- | ----------------- | ------------------------ |
| 子节点         | `childNodes`      | `children`               |
| 父节点         | `parentNode`      | 同                       |
| 第一个子节点   | `firstChild`      | `firstElementChild`      |
| 最后一个子节点 | `lastChild`       | `lastElementChild`       |
| 前一个兄弟节点 | `previousSibling` | `previousElementSibling` |
| 后一个兄弟节点 | `nextSibling`     | `nextElementSibling`     |

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <div id="box">
        <p>我是段落A</p>
        <p id="para">我是段落B</p>
        <p>我是段落C</p>
    </div>

    <script>
        var box = document.getElementById('box');
        var para = document.getElementById('para');

        // 所有子节点
        console.log(box.childNodes);
        // 所有的元素子节点（IE9开始兼容）
        console.log(box.children);
        console.log(box.children.para);

        // 第一个子节点
        console.log(box.firstChild);
        console.log(box.firstChild.nodeType);
        // 第一个元素子节点（IE9开始兼容）
        console.log(box.firstElementChild);

        // 最后一个子节点
        console.log(box.lastChild);
        console.log(box.lastChild.nodeType);
        // 最后一个元素子节点（IE9开始兼容）
        console.log(box.lastElementChild);

        // 父节点
        console.log(para.parentNode);

        // 前一个兄弟节点
        console.log(para.previousSibling);
        // 前一个元素兄弟节点（IE9开始兼容）
        console.log(para.previousElementSibling);

        // 后一个兄弟节点
        console.log(para.nextSibling);
        // 后一个元素兄弟节点（IE9开始兼容）
        console.log(para.nextElementSibling);
    </script>
</body>

</html>
```

- 结果

<img src="https://img-blog.csdnimg.cn/direct/a018ea0f7eac4440b89d98c0f5fad957.png" style="width: 50%;" />

注意：文本也算作节点（如图选中空白部分）

<img src="https://img-blog.csdnimg.cn/direct/56fd161bf9e34bcfbcdcde052f26ee28.png" style="width:50%;" />

# 六、书写常见的节点关系函数

书写 IE6 也能兼容的 “寻找所有元素子节点” 函数。

书写 IE6 也能兼容的 “寻找前一个元素兄弟节点” 函数。

如何编写函数，获得某元素的所有的兄弟节点。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <div id="box">
        <p>我是段落</p>
        <p>我是段落</p>
        <p>我是段落</p>
        <p id="fpara">我是段落fpara</p>
        我是文本
        <!-- 我是注释 -->
        <p id="para">
            我是段落para
            <span>1</span>
            <span>2</span>
            <span>3</span>
        </p>
        <p>我是段落</p>
        <p>我是段落</p>
        <p>我是段落</p>
    </div>

    <script>
        var box = document.getElementById('box');
        var para = document.getElementById('para');
        var fpara = document.getElementById('fpara');

        // 封装一个函数，这个函数可以返回元素的所有子元素节点（兼容到 IE6），类似 children 的功能
        function getChildren(node) {
            // 结果数组
            var children = [];
            // 遍历 node 这个节点的所有子节点，判断每一个子节点的 nodeType 属性是不是 1
            // 如果是 1，就推入结果数组
            for (var i = 0; i < node.childNodes.length; i++) {
                if (node.childNodes[i].nodeType == 1) {
                    children.push(node.childNodes[i]);
                }
            }
            return children;
        }

        console.log(getChildren(box));
        console.log(getChildren(para));

        // 封装一个函数，这个函数可以返回元素的前一个元素兄弟节点（兼容到 IE6），类似 previousElementSibling 的功能
        function getElementPrevSibling(node) {
            var o = node;
            // 使用 while 语句
            while (o.previousSibling != null) {
                if (o.previousSibling.nodeType == 1) {
                    // 结束循环，找到了
                    return o.previousSibling;
                }
                // 让 o 成为它的前一个节点
                o = o.previousSibling;
            }
            return null;
        }

        console.log(getElementPrevSibling(para));
        console.log(getElementPrevSibling(fpara));

        // 封装第三个函数，这个函数可以返回元素的所有元素兄弟节点
        function getAllElementSibling(node) {
            // 前面的元素兄弟节点
            var prevs = [];
            // 后面的元素兄弟节点
            var nexts = [];

            var o = node;
            // 遍历 node 的前面的节点
            while (o.previousSibling != null) {
                if (o.previousSibling.nodeType == 1) {
                    prevs.unshift(o.previousSibling);
                }
                o = o.previousSibling;
            }

            o = node;

            // 遍历 node 的后面的节点
            while (o.nextSibling != null) {
                if (o.nextSibling.nodeType == 1) {
                    nexts.push(o.nextSibling);
                }
                o = o.nextSibling;
            }

            // 将两个数组进行合并，然后返回
            return prevs.concat(nexts);
        }

        console.log(getAllElementSibling(para));
    </script>
</body>

</html>
```

# 七、节点操作

## 7.1 如何改变元素节点中的内容

改变元素节点中的内容可以使用两个相关属性。

- `innerHTML`
- `innerText`

`innerHTML` 属性能以 HTML 语法设置节点中的内容。

`innerText` 属性只能以纯文本的形式设置节点中的内容。

- innerHTML

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
    <div id="box"></div>
    <script>
        var oBox = document.getElementById('box');
        oBox.innerHTML = '周吉瑞';
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/b341e9ee28004c9eba57718505936d6f.png" style="width: 33%;" />

- innerHTML

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
    <div id="box"></div>
    <script>
        var oBox = document.getElementById('box');
        // 注意：此处的 HTML 字符串不允许换行！
        oBox.innerHTML = '<ul><li>牛奶</li><li>咖啡</li></ul>';
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/840443a3d8cb44b4902008bf12dc05c2.png" style="width: 25%;" />

- innerText

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
    <div id="box"></div>
    <script>
        var oBox = document.getElementById('box');
        oBox.innerText = '周吉瑞';
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/5cd6e03a42f8411d99596ce53578982f.png" style="width: 33%;" />

- innerText

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
    <div id="box"></div>
    <script>
        var oBox = document.getElementById('box');
        oBox.innerText = '<ul><li>牛奶</li><li>咖啡</li></ul>';
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/7072fc15585b414da6dfad3d45339b16.png" style="width: 33%;" />

## 7.2 如何改变元素节点的CSS样式

改变元素节点的 CSS 样式需要使用这样的语句：

- `oBox.style.backgroundColor = 'red';`
- `oBox.style.backgroundImage = 'url(images/1.jpg)';`
- `oBox.style.fontSize = '32px';`

【注意事项】

- CSS 属性要写成 “驼峰” 形式
- CSS 属性值要设置成完整形式
- 注意写单位

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        .box {
            width: 200px;
            height: 200px;
            border: 1px solid #000;
        }
    </style>
</head>

<body>
    <div class="box" id="box">
        你好
    </div>

    <script>
        var oBox = document.getElementById('box');
        oBox.style.backgroundColor = 'rgb(100, 200, 123)';
        oBox.style.backgroundColor = '#f80';
        oBox.style.backgroundImage = 'url(https://www.imooc.com/static/img/index/logo-recommended.png)';
        oBox.style.backgroundSize = 'contain';
        oBox.style.fontSize = '50px';
    </script>
</body>

</html>
```

> JS 修改的 CSS 样式，属于行内式，优先级最高！所以可以覆盖原有的样式。

## 7.3 如何改变元素节点的HTML属性

标准 W3C 属性，如 `src`、`href`、`title`、`alt` 等等，只需要直接打点进行更改即可。

`oImg.src = 'images/2.jpg';`

【案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <img src="images/1.jpg" id="pic">
    <a href="http://www.baidu.com" id="link">
        去百度
    </a>

    <script>
        var oPic = document.getElementById('pic');
        var oLink = document.getElementById('link');
        oPic.src = 'images/2.jpg';
        oLink.href = 'http://www.imooc.com';
        oLink.innerText = '去慕课网';
    </script>
</body>

</html>
```

对于不符合 W3C 标准的属性，要使用 `setAttribute()` 和 `getAttribute()` 来设置、读取。

```javascript
oBox.setAttribute('data-n', 10);
var n = oBox.getAttribute('data-n');
alert(n);
```

> HTML 的自定义属性，主要用途就是与 JS 配合方便实现一些效果。

【案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <div id="box"></div>

    <script>
        var box = document.getElementById('box');
        box.setAttribute('data-n', 10);
        var n = box.getAttribute('data-n');
        alert(n);
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/38239828cb3b44f88b7c9963bd6183ce.png" style="width:35%;" />

# 八、节点的创建、移动、删除和克隆

## 8.1 节点的创建

`document.createElement()` 方法用于创建一个指定 tag name 的 HTML 元素。

```javascript
var oDiv = document.createElement('div');
```

## 8.2 "孤儿节点"

新创建出的节点是 “孤儿节点”，这意味着它并没有被挂载到 DOM 树上，我们无法看见它。

必须继续使用 `appendChild()` 或 `insertBefore()` 方法将孤儿节点插入到 DOM 树上。

### 8.2.1 appendChild()

任何已经在 DOM 树上的节点，都可以调用 appendChild() 方法，它可以将孤儿节点挂载到它的内部，成为它的最后一个子节点。

```javascript
父节点.appendChild(孤儿节点);
```

【小案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <div id="box">
        <p>我是原本的段落0</p>
        <p>我是原本的段落1</p>
        <p>我是原本的段落2</p>
    </div>
    <script>
        var oBox = document.getElementById('box');
        // 创建孤儿节点
        var oP = document.createElement('p');
        // 设置内部文字
        oP.innerText = '我是新来的';
        // 上树
        oBox.appendChild(oP);
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/fe8581810510411e808d29aa664a7434.png" style="width:24%;" />

### 8.2.2 insertBefore()

任何已经在 DOM 树上的节点，都可以调用 insertBefore() 方法，它可以将孤儿节点挂载到它的内部，成为它的 “标杆子节点” 之前的节点。

```javascript
父节点.insertBefore(孤儿节点, 标杆节点);
```

【小案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <div id="box">
        <p>我是原本的段落0</p>
        <p>我是原本的段落1</p>
        <p>我是原本的段落2</p>
    </div>
    <script>
        var oBox = document.getElementById('box');
        var oPs = oBox.getElementsByTagName('p');
        // 创建孤儿节点
        var oP = document.createElement('p');
        // 设置内部文字
        oP.innerText = '我是新来的';
        // 上树
        oBox.insertBefore(oP, oPs[1]);
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/d285976903fc4b12b8176b86876d7efb.png" style="width:25%;" />

## 8.3 节点创建小案例

【动态创建一个20行12列的表格】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        td {
            width: 20px;
            height: 20px;
            border: 1px solid #000;
        }
    </style>
</head>

<body>
    <table id="mytable"></table>

    <script>
        // 请动态创建出一个 20 行 12 列的表格
        var mytable = document.getElementById('mytable');

        for (var i = 0; i < 20; i++) {
            // 创建了新的 tr 标签
            var tr = document.createElement('tr');
            for (var j = 0; j < 12; j++) {
                // 创建了新的 td 标签
                var td = document.createElement('td');
                // 让 tr 追加 td 标签
                tr.appendChild(td);
            }
            // 让 mytable 追加 tr 标签
            mytable.appendChild(tr);
        }
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/3a8e34c3e2a2416f974c90d7f30c212e.png" style="width:25%;" />

【九九乘法表】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        td {
            width: 80px;
            height: 30px;
            padding-left: 10px;
            border: 1px solid #000;
        }
    </style>
</head>

<body>
    <table id="mytable"></table>

    <script>
        // 请创建九九乘法表
        var mytable = document.getElementById('mytable');

        for (var i = 1; i <= 9; i++) {
            // 创建了新的 tr 标签
            var tr = document.createElement('tr');
            for (var j = 1; j <= i; j++) {
                // 创建了新的 td 标签
                var td = document.createElement('td');
                // 设置 td 内部的文字
                td.innerText = i + '×' + j + '=' + (i * j);
                // 让tr追加 td 标签
                tr.appendChild(td);
            }
            // 让 mytable 追加 tr 标签
            mytable.appendChild(tr);
        }
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/0fd9c03ed9a64032b8daabf446724b6c.png" style="width: 50%;" />

## 8.4 移动节点

如果将已经挂载到 DOM 树上的节点成为 `appendChild()` 或者 `insertBefore()` 的参数，这个节点将会被移动。

```javascript
新父节点.appendChild(已经有父亲的节点);
```

```javascript
新父节点.insertBefore(已经有父亲的节点, 标杆子节点);
```

这意味着一个节点不能同时位于 DOM 树的两个位置。

【小案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <div id="box1">
        <p id="para">我是段落</p>
    </div>

    <div id="box2">
        <p>我是box2的原有p标签</p>
        <p>我是box2的原有p标签</p>
        <p>我是box2的原有p标签</p>
        <p>我是box2的原有p标签</p>
    </div>

    <script>
        var box2 = document.getElementById('box2');
        var para = document.getElementById('para');
        var ps_inbox2 = box2.getElementsByTagName('p');

        box2.insertBefore(para, ps_inbox2[2]);
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/0e211f9be8bf424fb02349c51464783c.png" style="width:25%;" />

## 8.5 删除节点

`removeChild()` 方法从 DOM 中删除一个子节点。

```javascript
父节点.removeChild(要删除子节点);
```

节点不能主动删除自己，必须由父节点删除它。

【小案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <div id="box">
        <p>我是p节点0</p>
        <p>我是p节点1</p>
        <p>我是p节点2</p>
    </div>

    <script>
        var box = document.getElementById('box');
        var the_first_p = box.getElementsByTagName('p')[0];

        box.removeChild(the_first_p);
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/e8fb3dc57d374764bc0db367eb11d3f5.png" style="width:25%;" />

## 8.6 克隆节点

`cloneNode()` 方法可以克隆节点，克隆出的节点是 “孤儿节点”。

必须继续使用 `appendChild()` 或 `insertBefore()` 方法将孤儿节点插入到 DOM 树上。

```javascript
var 孤儿节点 = 老节点.cloneNode();
var 孤儿节点 = 老节点.cloneNode(true);
```

参数是一个布尔值，表示是否采用深度克隆：如果为 true，则该节点的所有后代节点也会被克隆，如果为 false，则只克隆该节点本身。

【小案例】

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <div id="box1">
        <ul>
            <li>牛奶</li>
            <li>咖啡</li>
            <li>可乐</li>
        </ul>
    </div>

    <div id="box2"></div>

    <script>
        var box1 = document.getElementById('box1');
        var box2 = document.getElementById('box2');
        var theul = box1.getElementsByTagName('ul')[0];

        // 克隆节点
        var new_ul = theul.cloneNode(true);
        box2.appendChild(new_ul);
    </script>
</body>
</html>
```

<img src="https://img-blog.csdnimg.cn/direct/dcce25f17ec54a2f9c0f4d73e7673dd6.png" style="width:25%;" />

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <div id="box1">
        <ul>
            <li>牛奶</li>
            <li>咖啡</li>
            <li>可乐</li>
        </ul>
    </div>

    <div id="box2"></div>

    <script>
        var box1 = document.getElementById('box1');
        var box2 = document.getElementById('box2');
        var theul = box1.getElementsByTagName('ul')[0];

        // 克隆节点
        var new_ul = theul.cloneNode(false);
        box2.appendChild(new_ul);
    </script>
</body>
</html>
```

<img src="https://img-blog.csdnimg.cn/direct/61cbb529ab4c48f2a6091a2a0a8238a4.png" style="width:25%;" />

# 九、事件监听

DOM 允许我们书写 JavaScript 代码以让 HTML 元素作出反应。

什么是 “事件”：用户与网页的交互动作。

## 9.1 什么是事件监听

“监听” 顾名思义，就是让计算机随时能够发现这个事件发生了，从而执行程序员预先编写好的一些程序。

设置事件监听的方法主要有 `onxxx` 和 `addEventListener()` 两种，二者的区别将在 “事件传播” 一课中介绍。

> 原始的事件处理方法：“直接通过事件绑定函数”
>
> 比如：
>
> HTML：`<button onclick="add();">点击</button>`
>
> JS：`function add() { alert("相加"); }`
>
> 以上方式不推荐使用！！！

## 9.2 最简单的设置事件监听的方法

最简单的给元素设置事件监听的方法就是设置它们的 `onxxx` 属性，像这样：

```javascript
oBox.onclick = function() {
    // 点击盒子时，将执行这里的语句
}
```

````javascript
function fun() {
    ...
}

oBox.onclick = fun;
````

【小案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        div {
            width: 200px;
            height: 200px;
            background-color: #ccc;
        }
    </style>
</head>

<body>
    <div id="box"></div>
    <script>
        var oBox = document.getElementById('box');

        // 给box这个盒子添加点击事件监听
        oBox.onclick = function () {
            alert('你好，我是点击事件函数');
        };
    </script>
</body>

</html>
```

## 9.3 常见的鼠标事件监听

| 事件名         | 事件描述                                     |
| -------------- | -------------------------------------------- |
| `onclick`      | 当鼠标单击某个对象                           |
| `ondblclick`   | 当鼠标双击某个对象                           |
| `onmousedown`  | 当某个鼠标按键在某个对象上被按下             |
| `onmouseup`    | 当某个鼠标按键在某个对象上被松开             |
| `onmousemove`  | 当某个鼠标按键在某个对象上被移动             |
| `onmouseenter` | 当鼠标进入某个对象（相似事件 `onmouseover`） |
| `onmouseleave` | 当鼠标离开某个对象（相似事件 `onmouseout`）  |

## 9.4 常见的键盘事件监听

| 事件名       | 事件描述                                                     |
| ------------ | ------------------------------------------------------------ |
| `onkeypress` | 当某个键盘的键被按下（系统按钮如箭头键和功能键无法得到识别） |
| `onkeydown`  | 当某个键盘的键被按下（系统按钮可以识别，并且会先于 `onkeypress` 发生） |
| `onkeyup`    | 当某个键盘的键被松开                                         |

## 9.5 常见的表单事件监听

| 事件名     | 事件描述                                  |
| ---------- | ----------------------------------------- |
| `onchange` | 当用户改变完成域的内容                    |
| `onfocus`  | 当某元素获得焦点（比如 tab 键或鼠标点击） |
| `onblur`   | 当某元素失去焦点                          |
| `onsubmit` | 当表单被提交                              |
| `onreset`  | 当表单被重置                              |

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <form id="myform">
        <p>
            姓名：
            <input type="text" name="nameField">
        </p>
        <p>
            年龄：
            <input type="text" name="ageField">
        </p>
        <p>
            <input type="submit">
        </p>
    </form>

    <script>
        var myform = document.getElementById('myform');
        // 表单对象可以通过 “打点” name 属性，得到里面的子元素。
        var nameField = myform.nameField;
        var ageField = myform.ageField;

        nameField.onchange = function () {
            console.log('你已经修改完了姓名');
        };

        nameField.oninput = function () {
            console.log('你正在修改姓名');
        };

        nameField.onfocus = function () {
            console.log('姓名框已经得到了焦点');
        }

        nameField.onblur = function () {
            console.log('姓名框已经失去了焦点');
        }

        myform.onsubmit = function () {
            alert('你正在尝试提交表单');
         /* return true;  可以省略，默认执行表单提交 */
         /* return false; 不执行表单提交，比如用户信息填写不全就不应该提交表单 */
        }
    </script>
</body>

</html>
```

> 表单对象可以通过 “打点” name 属性，得到里面的子元素。

## 9.6 常见的页面事件监听

| 事件名     | 事件描述               |
| ---------- | ---------------------- |
| `onload`   | 当页面或图像被完成加载 |
| `onunload` | 当用户退出页面         |

更多有关窗口或页面的事件在 BOM 课程中介绍。

# 十、事件传播

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        #box1 {
            width: 202px;
            height: 202px;
            border: 1px solid #000;
            padding: 50px;
        }

        #box2 {
            width: 100px;
            height: 100px;
            border: 1px solid #000;
            padding: 50px;
        }

        #box3 {
            width: 100px;
            height: 100px;
            border: 1px solid #000;
        }
    </style>
</head>

<body>
    <div id="box1">
        <div id="box2">
            <div id="box3"></div>
        </div>
    </div>
    <script>
        var oBox1 = document.getElementById('box1');
        var oBox2 = document.getElementById('box2');
        var oBox3 = document.getElementById('box3');

        oBox2.onclick = function () {
            console.log('我是 box2 的 onclick');
        };

        oBox3.onclick = function () {
            console.log('我是 box3 的 onclick');
        };

        oBox1.onclick = function () {
            console.log('我是 box1 的 onclick');
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/4d09cdc879af4b9bb8d42d2707220d36.png" style="width:25%;" />

<img src="https://img-blog.csdnimg.cn/direct/5e5d73a5bf8f406989ec987c9cc011d8.png" style="width: 50%;" />

<img src="https://img-blog.csdnimg.cn/direct/98918589d4a943b9a1efcc308307fb1a.png" style="width:50%;" />

## 10.1 捕获阶段和冒泡阶段

<img src="https://img-blog.csdnimg.cn/direct/916aa9d57e2a451e97b3d1bfc425977f.png" style="width:50%;" />

## 10.2 onxxx写法只能监听冒泡阶段

<img src="https://img-blog.csdnimg.cn/direct/375773982ffc4baaa81f20958f2b6f3d.png" style="width:50%;" />

## 10.3 addEventListener()方法

```javascript
oBox.addEventListener('click', function(){}, true);
```

> Event：事件

<img src="https://img-blog.csdnimg.cn/direct/58f83ec32e4c4c6a89e9a1a6769a53a2.png" style="width:50%;" />

【小案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        #box1 {
            width: 202px;
            height: 202px;
            border: 1px solid #000;
            padding: 50px;
        }

        #box2 {
            width: 100px;
            height: 100px;
            border: 1px solid #000;
            padding: 50px;
        }

        #box3 {
            width: 100px;
            height: 100px;
            border: 1px solid #000;
        }
    </style>
</head>

<body>
    <div id="box1">
        <div id="box2">
            <div id="box3"></div>
        </div>
    </div>
    <script>
        var oBox1 = document.getElementById('box1');
        var oBox2 = document.getElementById('box2');
        var oBox3 = document.getElementById('box3');

        oBox1.onclick = function () {
            console.log('我是box1的onclick');
        };

        oBox2.onclick = function () {
            console.log('我是box2的onclick');
        };

        oBox3.onclick = function () {
            console.log('我是box3的onclick');
        };

        oBox3.addEventListener('click', function () {
            console.log('我是box3的捕获阶段');
        }, true);

        oBox2.addEventListener('click', function () {
            console.log('我是box2的捕获阶段');
        }, true);

        oBox1.addEventListener('click', function () {
            console.log('我是box1的捕获阶段');
        }, true);


        oBox1.addEventListener('click', function () {
            console.log('我是box1的冒泡阶段');
        }, false);

        oBox2.addEventListener('click', function () {
            console.log('我是box2的冒泡阶段');
        }, false);

        oBox3.addEventListener('click', function () {
            console.log('我是box3的冒泡阶段');
        }, false);
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/e086c054acc145a0ad99043b532c9423.png" style="width: 25%;" />

【注意事项】

- 最内部元素不再区分捕获和冒泡阶段，会先执行写在前面的监听，然后执行后写的监听

> 原因：先捕获进入了最内层，然后从最内层开始冒泡，此时对于最内层元素来说，捕获和冒泡都是同一时间段，所以谁写在前就先执行谁。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        #box1 {
            width: 202px;
            height: 202px;
            border: 1px solid #000;
            padding: 50px;
        }

        #box2 {
            width: 100px;
            height: 100px;
            border: 1px solid #000;
            padding: 50px;
        }

        #box3 {
            width: 100px;
            height: 100px;
            border: 1px solid #000;
        }
    </style>
</head>

<body>
    <div id="box1">
        <div id="box2">
            <div id="box3"></div>
        </div>
    </div>
    <script>
        var oBox1 = document.getElementById('box1');
        var oBox2 = document.getElementById('box2');
        var oBox3 = document.getElementById('box3');

        oBox3.onclick = function () {
            console.log('我是box3的onclick');
        };

        oBox3.addEventListener('click', function () {
            console.log('我是box3的冒泡阶段');
        }, false);

        oBox3.addEventListener('click', function () {
            console.log('我是box3的捕获阶段');
        }, true);
    </script>
</body>

</html>
```

```text
我是box3的onclick
我是box3的冒泡阶段
我是box3的捕获阶段
```

> 注意：在新版的 Chrome 中都默认先执行最内层元素的捕获再执行冒泡！

- 如果给元素设置相同的两个或多个同名事件，则 onclick 写法后面写的会覆盖先写的；而 addEventListener 会按顺序执行（原因是 onclick 函数赋值多次会发生覆盖）

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        #box1 {
            width: 202px;
            height: 202px;
            border: 1px solid #000;
            padding: 50px;
        }

        #box2 {
            width: 100px;
            height: 100px;
            border: 1px solid #000;
            padding: 50px;
        }

        #box3 {
            width: 100px;
            height: 100px;
            border: 1px solid #000;
        }
    </style>
</head>

<body>
    <div id="box1">
        <div id="box2">
            <div id="box3"></div>
        </div>
    </div>
    <script>
        var oBox1 = document.getElementById('box1');
        var oBox2 = document.getElementById('box2');
        var oBox3 = document.getElementById('box3');

        oBox2.onclick = function () {
            alert('A');
        };

        oBox2.onclick = function () {
            alert('B');
        };

        oBox2.addEventListener('click', function () {
            alert('C');
        }, false);

        oBox2.addEventListener('click', function () {
            alert('D');
        }, false);
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/d3746c4db96e44418e5879c89d6ce528.png" style="width: 25%;" />

## 10.4 removeEventListener()方法

当我们 addEventListener() 后，该监听事件就会一直生效，直到关闭页面或是移除该对应的监听！

removeEventListener() 方法用来移除监听事件（只能移除具名函数的监听，且方法名称后面不能带）

```javascript
var body = document.querySelector('body'),
var clickTarget = document.getElementById('click-target'),
var mouseOverTarget = document.getElementById('mouse-over-target'),
var toggle = false;

// 具名函数
function makeBackgroundYellow() {
    'use strict';

    if (toggle) {
        body.style.backgroundColor = 'white';
    } else {
        body.style.backgroundColor = 'yellow';
    }

    toggle = !toggle;
}

// 注册监听
clickTarget.addEventListener('click',
    makeBackgroundYellow,
    false
);

// 注册监听
mouseOverTarget.addEventListener('mouseover', function () {
    'use strict';

    // 移除监听
    clickTarget.removeEventListener('click',
        makeBackgroundYellow,
        // 可以指定是移除冒泡类型的，还是捕获类型的！
        false
    );
});
```

## 10.5 特别注意

addEventListener() 一但注册某个事件，那么这个事件是会一直生效的，就算是该注册事件写在某个函数中，那个函数调用已经结束了，但是该事件还是会存在！因为事件的注册是直接绑定到相应的元素上的，并且是异步的，除非页面被关闭，或者是移除该监听！

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
    <button id="btn">点击</button>
    <script>
        var btn = document.getElementById('btn');

        function demo() {
            console.log('btn');
        }

        function test() {
            btn.addEventListener('click', demo, false);
            return 'over';
        }
        
        console.log(test());
    </script>
</body>

</html>

执行结果：
over
（点击按钮）
btn
```

> 特别注意：事件传播过程中如果没有控制，那么事件捕获最先从 document 对象开始，事件冒泡最晚到达 document 对象！

# 十一、事件对象

## 11.1 什么是事件对象

事件处理函数提供一个形式参数，它是一个对象，封装了本次事件的细节。

这个参数通常用单词 `event` 或字母 `e` 来表示。

```javascript
oBox.onmousemove = function(e) {
    // 对象 e 就是这次事件的“事件对象”
}
```

> 这个对象 e 接受的值由浏览器或操作系统传递。

## 11.2 鼠标位置

| 属性      | 属性描述                           |
| --------- | ---------------------------------- |
| `clientX` | 鼠标指针相对于浏览器的水平坐标     |
| `clientY` | 鼠标指针相对于浏览器的垂直坐标     |
| `pageX`   | 鼠标指针相对于整张网页的水平坐标   |
| `pageY`   | 鼠标指针相对于整张网页的垂直坐标   |
| `offsetX` | 鼠标指针相对于事件源元素的水平坐标 |
| `offsetY` | 鼠标指针相对于事件源元素的垂直坐标 |

- 浏览器

<img src="https://img-blog.csdnimg.cn/direct/3ce419a6de8047dbb9def652377dde4f.png" style="width:33%;" />

- 整张网页

<img src="https://img-blog.csdnimg.cn/direct/2b1e385504a840658d3f6557876bc8c4.png" style="width:33%;" />

- 事件源

<img src="https://img-blog.csdnimg.cn/direct/a6f0e515e0c042648fad27a1e1f118d7.png" style="width: 33%;" />

【小案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        * {
            margin: 0;
            padding: 0;
        }

        #box {
            width: 200px;
            height: 200px;
            background-color: #333;
            margin: 100px;
            margin-top: 150px;
        }

        body {
            height: 2000px;
        }

        #info {
            font-size: 30px;
            margin: 60px;
        }
    </style>
</head>

<body>
    <div id="box"></div>
    <div id="info"></div>

    <script>
        var oBox = document.getElementById('box');
        var oInfo = document.getElementById('info');

        oBox.onmousemove = function (e) {
            oInfo.innerHTML = 'offsetX/Y：' + e.offsetX + ',' + e.offsetY + '<br>'
                + 'clientX/Y：' + e.clientX + ',' + e.clientY + '<br>'
                + 'pageX/Y：' + e.pageX + ',' + e.pageY;
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/97d7933602254310bbb2f6f122eac478.png" style="width: 25%;" />

【注意事项】

对于 offsetX 和 offsetY 而言，总是以最内层元素为事件源。

> 所以应避免事件源盒子内部有小盒子的存在。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        * {
            margin: 0;
            padding: 0;
        }

        #box {
            overflow: hidden;
            width: 200px;
            height: 200px;
            background-color: #333;
            margin: 100px;
            margin-top: 150px;
        }

        body {
            height: 2000px;
        }

        #info {
            font-size: 30px;
            margin: 60px;
        }

        p {
            width: 100px;
            height: 100px;
            background-color: pink;
            margin: 50px;
        }
    </style>
</head>

<body>
    <div id="box">
        <p></p>
    </div>
    <div id="info"></div>

    <script>
        var oBox = document.getElementById('box');
        var oInfo = document.getElementById('info');

        oBox.onmousemove = function (e) {
            oInfo.innerHTML = 'offsetX/Y：' + e.offsetX + ',' + e.offsetY + '<br>';
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/fa31f5c8425c40d79bc938417758a86d.png" style="width:25%;" />

## 11.3 e.charCode和e.keyCode属性

`e.charCode` 属性通常用于 `onkeypress` 事件中，表示用户输入的字符的 “字符码”。

`e.keyCode` 属性通常用于 `onkeydown` 事件和 `onkeyup` 中，表示用户按下的按键的 “键码”。

### 11.3.1 charCode字符码

| 字符    | 字符码   |
| ------- | -------- |
| `0 ~ 9` | 48 ~ 57  |
| `A ~ Z` | 65 ~ 90  |
| `a ~ z` | 97 ~ 122 |

### 11.3.2 keyCode键码

| 按键               | 键码                                |
| ------------------ | ----------------------------------- |
| `0 ~ 9`            | 48 ~ 57（同 charCode 键码完全相同） |
| `A ~ Z` `a ~ z`    | 65 ~ 90（字母不分大小写）           |
| 方向键 `← ↑ → ↓`   | 37、38、39、40                      |
| 退格键 `Backspace` | 8                                   |
| `Tab`              | 9                                   |
| 回车键 `enter`     | 13                                  |
| `Shift`            | 16                                  |
| `Ctrl`             | 17                                  |
| `Alt`              | 18                                  |
| 大小键 `CapsLK`    | 20                                  |
| `Esc`              | 27                                  |
| 空格键 `space`     | 32                                  |
| 删除键 `Delete`    | 46                                  |
| 开始键 `Start`     | 91                                  |
| `F1 ~ F12`         | 112 ~ 123                           |

> 以上只是部分！

### 11.3.3 案例

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <input type="text" id="field1">
    <h1 id="info1"></h1>
    <input type="text" id="field2">
    <h1 id="info2"></h1>

    <script>
        var oField1 = document.getElementById('field1');
        var oInfo1 = document.getElementById('info1');
        var oField2 = document.getElementById('field2');
        var oInfo2 = document.getElementById('info2');

        oField1.onkeypress = function (e) {
            oInfo1.innerText = '你输入的字符的字符码是' + e.charCode;
        };

        oField2.onkeydown = function (e) {
            oInfo2.innerText = '你按下的按键的键码是' + e.keyCode;
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/5fcdccd4f30f471d8a8543b4d84054b5.png" style="width:50%;" />

【小案例 - 盒子移动】

制作一个特效：按方向键可以控制页面上的盒子移动。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        #box {
            position: absolute;
            top: 200px;
            left: 200px;
            width: 100px;
            height: 100px;
            background-color: orange;
        }

        #info {
            position: fixed;
        }
    </style>
</head>

<body>
    <div id="box"></div>
    <h1 id="info"></h1>

    <script>
        var oBox = document.getElementById('box');
        var oInfo = document.getElementById('info');

        // 全局变量 t、l，分别表示盒子的 top 属性值和 left 属性值
        var t = 200;
        var l = 200;

        // 监听 document 对象的键盘按下事件监听，表示当用户在整个网页上按下按键的时候
        document.onkeydown = function (e) {
            oInfo.innerText = '你按下的按键的键码是' + e.keyCode;

            switch (e.keyCode) {
                case 37:
                    l -= 3;
                    break;
                case 38:
                    t -= 3;
                    break;
                case 39:
                    l += 3;
                    break;
                case 40:
                    t += 3;
                    break;
            }

            // 更改样式
            oBox.style.left = l + 'px';
            oBox.style.top = t + 'px';
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/84c7bfe2e9ec47eab6c9dc7656e9cec2.png" style="width: 33%;" />

## 11.4 e.preventDefault()方法

`e.preventDefault()` 方法用来阻止事件产生的 “默认动作”。

一些特殊的业务需求，需要阻止事件的 “默认动作”。

【小案例1】

制作一个文本框，只能让用户在其中输入小写字母和数字，其他字符输入没有效果。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <p>
        只能输入小写字母和数字：
        <input type="text" id="field">
    </p>
    <script>
        var oField = document.getElementById('field');

        oField.onkeypress = function (e) {
            console.log(e.charCode);

            // 根据用户输入的字符的字符码（e.charCode)
            // 数字 0~9，字符码 48~57
            // 小写字母 a~z，字符码 97~122
            if (!(e.charCode >= 48 && e.charCode <= 57 || e.charCode >= 97 && e.charCode <= 122)) {
                // 阻止浏览器的默认行为
                e.preventDefault();
            }
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/b163fb53b89449cda2ca2b51da68588f.png" style="width: 33%;" />

【小案例2】

制作鼠标滚轮事件：当鼠标在盒子中向下滚动时，数字加 1；反之，数字减 1。

鼠标滚轮事件是 `onmousewheel`，它的事件对象 e 提供 `deltaY` 属性表示鼠标滚动方向，向下滚动是返回正值，向上滚动时返回负值。

- 没有阻止事件的 “默认动作” 时

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        #box {
            width: 200px;
            height: 200px;
            background-color: #333;
        }

        body {
            height: 2000px;
        }
    </style>
</head>

<body>
    <div id="box"></div>
    <h1 id="info">0</h1>

    <script>
        var oBox = document.getElementById('box');
        var oInfo = document.getElementById('info');

        // 全局变量就是 info 中显示的数字
        var a = 0;

        // 给 box 盒子添加鼠标滚轮事件监听
        oBox.onmousewheel = function (e) {
            if (e.deltaY > 0) {
                a++;
            } else {
                a--;
            }
            oInfo.innerText = a;
        }
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/9fd418f6cf4d481b9c42648fb1894157.png" style="width: 25%;" />

- 阻止事件的 “默认动作” 后

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        #box {
            width: 200px;
            height: 200px;
            background-color: #333;
        }

        body {
            height: 2000px;
        }
    </style>
</head>

<body>
    <div id="box"></div>
    <h1 id="info">0</h1>

    <script>
        var oBox = document.getElementById('box');
        var oInfo = document.getElementById('info');

        // 全局变量就是 info 中显示的数字
        var a = 0;

        // 给 box 盒子添加鼠标滚轮事件监听
        oBox.onmousewheel = function (e) {
            // 阻止默认事件：就是说当用户在盒子里面滚动鼠标滚轮的时候，此时不会引发页面的滚动条的滚动
            e.preventDefault();

            if (e.deltaY > 0) {
                a++;
            } else {
                a--;
            }
            oInfo.innerText = a;
        }
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/95e317d737bf48959420404decb09605.png" style="width:25%;" />

【小案例3】

制作鼠标右键点击事件：当鼠标右键被点击时，改变背景颜色。

`oncontextmenu`：在用户点击鼠标右键打开上下文菜单时触发。

- 没有阻止事件的 “默认动作” 时

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <script>
        var oBody = document.getElementsByTagName('body')[0];
        // 给 document 添加鼠标右键事件监听
        var flag = 0;
        document.oncontextmenu = function (e) {
            if (flag == 0) {
                oBody.style.backgroundColor = 'black';
                flag = 1;;
            } else {
                oBody.style.backgroundColor = 'white';
                flag = 0;
            }
        }
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/6f6f09e335ec4738adefd68ce3ddc834.png" style="width: 33%;" />

- 阻止事件的 “默认动作” 后

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <script>
        var oBody = document.getElementsByTagName('body')[0];
        // 给 document 添加鼠标右键事件监听
        var flag = 0;
        document.oncontextmenu = function (e) {
            // 阻止默认事件：就是说当用户右键的时候，此时不会弹出页面的右键菜单
            e.preventDefault();

            if (flag == 0) {
                oBody.style.backgroundColor = 'black';
                flag = 1;;
            } else {
                oBody.style.backgroundColor = 'white';
                flag = 0;
            }
        }
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/f80731afa8c64f608e27b98c4eacfe1c.png" style="width:33%;" />

## 11.5 e.stopPropagation()方法

`e.stopPropagation()` 方法用来阻止事件继续传播。

在一些场合，非常有必要切断事件继续传播，否则会造成页面特效显示出 bug。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        div {
            width: 200px;
            height: 200px;
            background-color: #333;
        }
    </style>
</head>

<body>
    <div id="box">
        <button id="btn">按我</button>
    </div>
    <script>
        var oBox = document.getElementById('box');
        var oBtn = document.getElementById('btn');

        oBox.onclick = function () {
            console.log('我是盒子');
        };

        oBtn.onclick = function () {
            console.log('我是按钮');
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/ae8aa4312e844934a76f6dba436ec5ae.png" style="width:25%;" />

【阻止冒泡】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        div{
            width: 200px;
            height: 200px;
            background-color: #333;
        }
    </style>
</head>

<body>
    <div id="box">
        <button id="btn">按我</button>
    </div>
    <script>
        var oBox = document.getElementById('box');
        var oBtn = document.getElementById('btn');

        oBox.onclick = function () {
            console.log('我是盒子');
        };

        oBtn.onclick = function (e) {
            // 阻止事件继续传播
            e.stopPropagation();
            console.log('我是按钮');
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/82853efe0b2b4eb19f0fd4d97b93521a.png" style="width:25%;" />

【阻止传播】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        div {
            width: 200px;
            height: 200px;
            background-color: #333;
        }
    </style>
</head>

<body>
    <div id="box">
        <button id="btn">按我</button>
    </div>
    <script>
        var oBox = document.getElementById('box');
        var oBtn = document.getElementById('btn');

        oBox.addEventListener('click', function (e) {
            // 阻止事件继续传播
            e.stopPropagation();
            console.log('我是盒子');
        }, true);

        oBtn.addEventListener('click', function () {
            console.log('我是按钮');
        }, true);
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/e40278b0102d48ad9f02bd876d4cbe9d.png" style="width:25%;" />

【小案例】

制作一个弹出层：点击按钮显示弹出层，点击网页任意地方，弹出层关闭。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        .modal {
            width: 400px;
            height: 140px;
            background-color: #333;
            position: absolute;
            top: 50%;
            left: 50%;
            margin-top: -70px;
            margin-left: -200px;
            display: none;
        }
    </style>
</head>

<body>
    <button id="btn">按我弹出弹出层</button>
    <div class="modal" id="modal"></div>

    <script>
        var oBtn = document.getElementById('btn');
        var oModal = document.getElementById('modal');

        // 点击按钮的时候，弹出层显示
        oBtn.onclick = function (e) {
            // 阻止事件继续传播到document身上
            e.stopPropagation();
            oModal.style.display = 'block';
        };

        // 点击页面任何部分的时候，弹出层关闭
        document.onclick = function () {
            oModal.style.display = 'none';
        };

        // 点击弹出层内部的时候，不能关闭弹出层的，所以应该阻止事件继续传播
        oModal.onclick = function (e) {
            // 阻止事件继续传播到document身上
            e.stopPropagation();
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/3a420b7e96c24a67af7a6cdfb554c382.png" style="width:50%;" />

# 十二、事件委托

## 12.1 批量添加事件监听

题目：页面上有一个无序列表 `<ul>`，它内部共有 20 个 `<li>` 元素，请批量给它们添加事件监听，实现效果：点击哪个 `<li>` 元素，哪个 `<li>` 元素就变红。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <ul id="list">
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
    </ul>

    <script>
        var oList = document.getElementById('list');
        var lis = oList.getElementsByTagName('li');

        // 书写循环语句，批量给元素添加监听
        for (var i = 0; i < lis.length; i++) {
            lis[i].onclick = function () {
                // 在这个函数中，this 表示点击的这个元素，this 涉及函数上下文的相关知识，我们在“面向对象”课程中介绍
                this.style.color = 'red';
            };
        }
    </script>
</body>

</html>
```

## 12.2 批量添加事件监听的性能问题

每一个事件监听注册都会消耗一定的系统内存，而批量添加事件会导致监听数量太多，内存消耗会非常大。

实际上，每个 `<li>` 的事件处理函数都是不同的函数，这些函数本身也会占用内存。

## 12.3 新增元素动态绑定事件

题目：页面上有一个无序列表 `<ul>`，它内部没有 `<li>` 元素，请制作一个按钮，点击这个按钮就能增加一个 `<li>` 元素。并且要求每个增加的 `<li>` 元素也要有点击事件监听，实现效果：点击哪个 `<li>` 元素，哪个 `<li>` 元素就变红。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <button id="btn">按我添加新的li列表项</button>
    <ul id="list"></ul>

    <script>
        var oBtn = document.getElementById('btn');
        var oList = document.getElementById('list');
        var lis = oList.getElementsByTagName('li');

        // 按钮的点击事件
        oBtn.onclick = function () {
            // 创建一个新的 li 列表项，孤儿节点
            var oLi = document.createElement('li');
            oLi.innerHTML = '我是列表项';
            // 上树
            oList.appendChild(oLi);
            // 给新创建的这个 li 节点添加 onclick 事件监听
            oLi.onclick = function () {
                this.style.color = 'red';
            };
        };
    </script>
</body>

</html>
```

## 12.4 动态绑定事件的问题

新增元素必须分别添加事件监听，不能自动获得事件监听。

大量事件监听、大量事件处理函数都会产生大量的内存消耗。

## 12.5 事件委托

利用事件冒泡机制，将后代元素事件委托给祖先元素。

<img src="https://img-blog.csdnimg.cn/direct/338bb1f71192403cb10ad2492a2cb9a0.png" style="width:50%;" />

## 12.6 e.target和e.currentTarget属性

事件委托通常需要结合使用 e.target 属性。

| 属性            | 属性描述                              |
| --------------- | ------------------------------------- |
| `target`        | 触发此事件的最早元素，即 “事件源元素” |
| `currentTarget` | 事件处理程序附加到的元素              |

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <button id="btn">按我创建一个新列表项</button>
    <ul id="list">
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
    </ul>
    <script>
        var oList = document.getElementById('list');
        var oBtn = document.getElementById('btn');

        oList.onclick = function (e) {
            // e.target 表示用户真正点击的那个元素，即 “事件源元素”
            e.target.style.color = 'red';
        };

        oBtn.onclick = function () {
            // 创建新的li元素
            var oLi = document.createElement('li');
            // 写内容
            oLi.innerText = '我是新来的';
            // 上树
            oList.appendChild(oLi);
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/016419b585804be491f3c25b6afb0aba.png" style="width: 25%;" />

## 12.7 事件委托的使用场景

当有大量类似元素需要批量添加事件监听时，使用事件委托可以减少内存开销。

当有动态元素节点上树时，使用事件委托可以让新上树的元素具有事件监听。

## 12.8 使用事件委托时需要注意的事项

（1）`onmouseenter` 和 `onmouseover` 都表示 “鼠标进入”，它们有什么区别呢？

答：`onmouseenter` 不冒泡，`onmouseover` 冒泡。

- 使用事件委托时要注意：不能委托不冒泡的事件给祖先元素。

【onmouseenter】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <button id="btn">按我创建一个新列表项</button>
    <ul id="list">
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
    </ul>
    <script>
        var oList = document.getElementById('list');
        var oBtn = document.getElementById('btn');

        // onmouseenter这个属性天生就是“不冒泡”的，相当于你事件处理函数附加给了那个DOM节点
        // 就是哪个DOM节点自己触发的事件,没有冒泡过程
        // 再因为继承性，所以所有 li 会一起变色
        oList.onmouseenter = function (e) {
            // e.target表示用户真正点击的那个元素
            e.target.style.color = 'red';
        };

        // oList.onmouseover = function (e) {
        //     // e.target表示用户真正点击的那个元素，即：那个 li
        //     e.target.style.color = 'red';
        // };

        oBtn.onclick = function () {
            // 创建新的li元素
            var oLi = document.createElement('li');
            // 写内容
            oLi.innerText = '我是新来的';
            // 上树
            oList.appendChild(oLi);
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/3111c2429c4745b1a0cfd383b2ee6cb0.png" style="width:25%;" />

【onmouseover】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <button id="btn">按我创建一个新列表项</button>
    <ul id="list">
        <li>列表项</li>
        <li>列表项</li>
        <li>列表项</li>
    </ul>
    <script>
        var oList = document.getElementById('list');
        var oBtn = document.getElementById('btn');

        // onmouseenter这个属性天生就是“不冒泡”的，相当于你事件处理函数附加给了那个DOM节点
        // 就是哪个DOM节点自己触发的事件,没有冒泡过程
        // 再因为继承性，所以所有 li 会一起变色
        // oList.onmouseenter = function (e) {
        //     // e.target表示用户真正点击的那个元素
        //     e.target.style.color = 'red';
        // };

        oList.onmouseover = function (e) {
            // e.target表示用户真正点击的那个元素，即：那个 li
            e.target.style.color = 'red';
        };

        oBtn.onclick = function () {
            // 创建新的li元素
            var oLi = document.createElement('li');
            // 写内容
            oLi.innerText = '我是新来的';
            // 上树
            oList.appendChild(oLi);
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/6dedbbfe70984cb99210dabaafe736f2.png" style="width:25%;" />

（2）最内层元素不能再有额外的内层元素了，比如：

<img src="https://img-blog.csdnimg.cn/direct/f7e2abf61be843b28f922fbdee691821.png" style="width:50%;" />

这会导致点击 li 时效果正常，但是点击 span 时，只有 span 会变色。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <button id="btn">按我创建一个新列表项</button>
    <ul id="list">
        <li><span>我是span</span>列表项</li>
        <li><span>我是span</span>列表项</li>
        <li><span>我是span</span>列表项</li>
        <li><span>我是span</span>列表项</li>
        <li><span>我是span</span>列表项</li>
    </ul>
    <script>
        var oList = document.getElementById('list');
        var oBtn = document.getElementById('btn');

        oList.onmouseover = function (e) {
            // e.target表示用户真正点击的那个元素，即：那个 li
            e.target.style.color = 'red';
        };

        oBtn.onclick = function () {
            // 创建新的li元素
            var oLi = document.createElement('li');
            // 写内容
            oLi.innerText = '我是新来的';
            // 上树
            oList.appendChild(oLi);
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/b13c4d9299544e5a8bd7d92fd2d5433b.png" style="width:25%;" />

# 十三、定时器和延时器

## 13.1 定时器

`setInterval()` 函数可以重复调用一个函数，在每次调用之间有固定的时间间隔。

> Interval：间隔

<img src="https://img-blog.csdnimg.cn/direct/239528f2333f420d86fe0a9e28baecd5.png" style="width:50%;" />

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <script>
        var a = 0;

        setInterval(function () {
            console.log(++a);
        }, 1000);
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/0b362dd01dcf4b0baf236da7ab099579.png" style="width:24%;" />

## 13.2 函数的参数

`setInterval()` 函数可以接收第 3、4、…… 个参数，它们将按顺序传入函数。

<img src="https://img-blog.csdnimg.cn/direct/e36ec2ff6cd342b8abe4297f2e858f3b.png" style="width:50%;" />

## 13.3 具名函数也可以传入setInterval

具名函数也可以传入 `setInterval`。

> 具名函数：有名称的函数。
>
> 匿名函数：无名称的函数。

<img src="https://img-blog.csdnimg.cn/direct/f565322cc06c4b71bfa02074e00dd4b9.png" style="width:50%;" />

## 13.4 清除定时器

`clearInterval()` 函数可以清除一个定时器。

<img src="https://img-blog.csdnimg.cn/direct/94ab4c36a398486ebac2913648fdaab0.png" style="width:50%;" />

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <h1 id="info">0</h1>
    <button id="btn1">开始</button>
    <button id="btn2">暂停</button>

    <script>
        var oInfo = document.getElementById('info');
        var oBtn1 = document.getElementById('btn1');
        var oBtn2 = document.getElementById('btn2');

        var a = 0;

        // 全局变量
        var timer;

        oBtn1.onclick = function () {
            // 更改全局变量 timer 的值为一个定时器实体
            timer = setInterval(function () {
                oInfo.innerText = ++a;
            }, 1000);
        };

        oBtn2.onclick = function () {
            clearInterval(timer);
        };
    </script>
</body>

</html>
```

![](https://img-blog.csdnimg.cn/direct/a0d03ba00d2f47959a413045a694b22b.png)

但是，此时有一个 BUG，那就是当重复点击时，会发生定时器叠加。

> 定时器叠加：同一时间有多个定时器在同时工作。

![](https://img-blog.csdnimg.cn/direct/22333d5aab974404baa4ec7dfb44be3c.png)

改进：

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <h1 id="info">0</h1>
    <button id="btn1">开始</button>
    <button id="btn2">暂停</button>

    <script>
        var oInfo = document.getElementById('info');
        var oBtn1 = document.getElementById('btn1');
        var oBtn2 = document.getElementById('btn2');

        var a = 0;

        // 全局变量
        var timer;

        oBtn1.onclick = function () {
            // 为了防止定时器叠加，我们应该在设置定时器之前先清除定时器
            clearInterval(timer);
            // 更改全局变量timer的值为一个定时器实体
            timer = setInterval(function () {
                oInfo.innerText = ++a;
            }, 1000);
        };

        oBtn2.onclick = function () {
            clearInterval(timer);
        };
    </script>
</body>

</html>
```

![](https://img-blog.csdnimg.cn/direct/2300cf23e0704bf595b02aed3b0e4826.png)

## 13.5 延时器

`setTimeout()` 函数可以设置一个延时器，当指定时间到了之后，会执行函数一次，不再重复执行。

<img src="https://img-blog.csdnimg.cn/direct/94d80a883ef548b6bab4036362ce8c2d.png" style="width:50%;" />

## 13.6 清除延时器

`clearTimeout()` 函数可以清除延时器，和 `clearInterval()` 非常类似。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    <button id="btn1">2秒后弹出你好</button>
    <button id="btn2">取消弹出</button>

    <script>
        var btn1 = document.getElementById('btn1');
        var btn2 = document.getElementById('btn2');
        var timer;

        btn1.onclick = function () {
            timer = setTimeout(function () {
                alert('你好');
            }, 2000);
        }

        btn2.onclick = function () {
            clearTimeout(timer);
        }
    </script>
</body>

</html>
```

## 13.7 初步认识异步语句

`setInterval()` 和 `setTimeout()` 是两个异步语句。

异步（asynchronous）：不会阻塞 CPU 继续执行其他语句，当异步完成时，会执行 “回调函数”（callback）。

<img src="https://img-blog.csdnimg.cn/direct/23caf51bcfe4430d848bd196d894e398.png" style="width:50%;" />

## 13.8 使用定时器实现动画

动画是网页上非常常见的业务需求。

使用定时器实现动画就是利用 “视觉暂留” 原理。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        #box {
            position: absolute;
            top: 100px;
            left: 100px;
            width: 100px;
            height: 100px;
            background-color: orange;
        }
    </style>
</head>

<body>
    <button id="btn">开始运动</button>
    <div id="box"></div>

    <script>
        // 得到元素
        var btn = document.getElementById('btn');
        var box = document.getElementById('box');

        // 全局变量盒子的left值
        var left = 100;

        // 按钮监听
        btn.onclick = function () {
            var timer = setInterval(function () {
                // 改变全局变量
                left += 10;
                if (left >= 1000) {
                    clearInterval(timer);
                }
                // 设置left属性
                box.style.left = left + 'px';
            }, 20);
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/86287d32a4b1469a8d0aa2a989708bbc.png" style="width:50%;" />

使用定时器实现动画较为不便：

1. 不方便根据动画总时间计算步长
2. 运动方向要设置正负
3. 多种运动进行叠加较为困难

> 所以实际开发中是利用：
>
> JQuery 等 JS 库（目前利用得越来越少）
>
> JS + CSS3 的模式制作动画

# 十四、JS和CSS3结合实现动画

## 14.1 JS和CSS3结合实现动画

- 我们知道，CSS3 的 `transition` 过渡属性可以实现动画。
- JS 可以利用 CSS3 的 transition 属性轻松实现元素动画。
- JS 和 CSS3 结合实现动画规避了定时器制作动画的缺点。

## 14.2 函数节流

函数节流：一个函数执行一次后，只有大于设定的执行周期后才允许执行第二次。

函数节流的意义：在许多 JS + CSS3 实现的动画中，如果没有函数节流，那么当一个动画还没有执行完时，如果用户再次要求执行动画，则动画会直接中断还未执行完的动画，然后执行新的动画。我们要通过函数节流来避免这种情况，比如：轮播图中就要避免用户高频地点击轮播图切换按钮时轮播图飞快切换的问题，而应该做到无论用户点击多块，轮播图的切换速度始终是平稳的。

函数节流非常容易实现，只需借助 `setTimeout` 延时器。

【函数节流模板】

```javascript
// 声明节流锁
var lock = true;

function 需要节流的函数() {
    // 如果锁是关闭状态，则不执行
    if (!lock) {
        return;
    }
    
    // 函数核心语句
    // ……
    // 函数核心语句
    
    // 关锁
    lock = false;
    
    // 指定毫秒数后将锁打开
    setTimeout(function() {
        lock = true;
    }, 2000);
}
```

【案例】

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        #box {
            width: 100px;
            height: 100px;
            background-color: orange;
            position: absolute;
            top: 100px;
            left: 100px;
        }
    </style>
</head>

<body>
    <button id="btn">按我运动</button>
    <div id="box"></div>

    <script>
        // 得到元素
        var btn = document.getElementById('btn');
        var box = document.getElementById('box');

        // 标识量，指示当前盒子在左边还是右边
        var pos = 1; // 1左边，2右边

        // 函数节流锁
        var lock = true;

        // 事件监听
        btn.onclick = function () {
            // 首先检查锁是否是关闭
            if (!lock) return;

            // 把过渡加上
            box.style.transition = 'all 2s linear 0s';
            if (pos == 1) {
                // 瞬间移动，但是由于有过渡，所以是动画
                box.style.left = '1100px';
                pos = 2;
            } else if (pos == 2) {
                // 瞬间移动，但是由于有过渡，所以是动画
                box.style.left = '100px';
                pos = 1;
            }

            // 关锁
            lock = false;
            // 指定时间后，将锁打开
            setTimeout(function () {
                lock = true;
            }, 2000);
        };
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/700c80db7d1a45f28896a1fb7e3c2eb7.png" style="width:50%;" />

# 十五、常见动画制作

## 15.1 动画效果开发1-无缝连续滚动特效

<img src="https://img-blog.csdnimg.cn/direct/f0031ce619ce43a4a621263885b10e0e.png" style="width:50%;" />

原理:

<img src="https://img-blog.csdnimg.cn/direct/da1151967ca34eab94bea7fb3541b944.png" style="width:50%;" />

代码：

> 此动画利用定时器反而比 JS + CSS3 更方便。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        * {
            margin: 0;
            padding: 0;
        }

        .box {
            width: 1000px;
            height: 130px;
            border: 1px solid #000;
            margin: 50px auto;
            overflow: hidden;
        }

        .box ul {
            list-style: none;
            /* 设置大一点，这样li才能浮动 */
            width: 5000px;
            position: relative;
        }

        .box ul li {
            float: left;
            margin-right: 10px;
        }
    </style>
</head>

<body>
    <div id="box" class="box">
        <ul id="list">
            <li><img src="images/number/0.png" alt=""></li>
            <li><img src="images/number/1.png" alt=""></li>
            <li><img src="images/number/2.png" alt=""></li>
            <li><img src="images/number/3.png" alt=""></li>
            <li><img src="images/number/4.png" alt=""></li>
            <li><img src="images/number/5.png" alt=""></li>
        </ul>
    </div>
    <script>
        var box = document.getElementById('box');
        var list = document.getElementById('list');

        // 复制多一遍所有的li
        list.innerHTML += list.innerHTML;

        // 全局变量，表示当前list的left值
        var left = 0;

        // 定时器，全局变量
        var timer;

        move();

        // 动画封装成函数
        function move() {
            // 设表先关，防止动画积累
            clearInterval(timer);

            timer = setInterval(function () {
                left -= 4;
                // 验收
                if (left <= -1260) {
                    left = 0;
                }
                list.style.left = left + 'px';
            }, 20);
        }

        // 鼠标进入停止定时器
        box.onmouseenter = function () {
            clearInterval(timer);
        };

        // 鼠标离开继续定时器
        box.onmouseleave = function () {
            move();
        };
    </script>
</body>

</html>
```

## 15.2 动画效果开发2-跑马灯轮播图特效

> 跑马灯轮播图，又叫：滑动轮播图。此种轮播图需要将图片排成一行，并在最后一张图片后克隆一份第一张图片（当到达最后一张时，实现瞬间轮回效果）。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        * {
            margin: 0;
            padding: 0;
        }

        .carousel {
            width: 650px;
            height: 360px;
            border: 1px solid #000;
            margin: 50px auto;
            position: relative;
            overflow: hidden;
        }

        .carousel ul {
            list-style: none;
            width: 6000px;
            position: relative;
            left: 0px;
            transition: left .5s ease 0s;
        }

        .carousel ul li {
            float: left;
        }

        .carousel .leftbtn {
            position: absolute;
            left: 20px;
            top: 50%;
            margin-top: -25px;
            width: 50px;
            height: 50px;
            background-color: rgb(28, 180, 226);
            border-radius: 50%;
        }

        .carousel .rightbtn {
            position: absolute;
            right: 20px;
            top: 50%;
            margin-top: -25px;
            width: 50px;
            height: 50px;
            background-color: rgb(28, 180, 226);
            border-radius: 50%;
        }
    </style>
</head>

<body>
    <div class="carousel">
        <ul id="list">
            <li><img src="images/beijing/0.jpg" alt=""></li>
            <li><img src="images/beijing/1.jpg" alt=""></li>
            <li><img src="images/beijing/2.jpg" alt=""></li>
            <li><img src="images/beijing/3.jpg" alt=""></li>
            <li><img src="images/beijing/4.jpg" alt=""></li>
        </ul>
        <a href="javascript:;" class="leftbtn" id="leftbtn"></a>
        <a href="javascript:;" class="rightbtn" id="rightbtn"></a>
    </div>
    <script>
        // 得到按钮和ul，ul整体进行运动
        var leftbtn = document.getElementById('leftbtn');
        var rightbtn = document.getElementById('rightbtn');
        var list = document.getElementById('list');

        // 克隆第一张图片
        var cloneli = list.firstElementChild.cloneNode(true); // 记得要写true，不然就只会克隆li而不会克隆img
        list.appendChild(cloneli);

        // 当前ul显示到第几张了，从0开始数
        var idx = 0;

        // 节流锁
        var lock = true;

        // 右边按钮监听
        rightbtn.onclick = function () {
            // 判断锁的状态
            if (!lock) return;

            lock = false;

            // 给list加过渡，为什么要加？？css中不是已经加了么？？这是因为最后一张图片会把过渡去掉
            list.style.transition = 'left .5s ease 0s';
            idx++;
            if (idx > 4) {
                // 设置一个延时器，延时器的功能就是将ul瞬间拉回0的位置，延时器的目的就是让过渡动画结束之后
                setTimeout(function () {
                    // 取消掉过渡，因为要的是瞬间移动，不是“咕噜”回去
                    list.style.transition = 'none';
                    list.style.left = 0;
                    idx = 0;
                }, 500);
            }
            list.style.left = -idx * 650 + 'px';

            // 函数节流
            setTimeout(function () {
                lock = true;
            }, 500);
        }

        // 左边按钮监听
        leftbtn.onclick = function () {
            if (!lock) return;

            lock = false;

            // 判断是不是第0张，如果是，就要瞬间用假的替换真的
            if (idx == 0) {
                // 取消掉过渡，因为要的是瞬间移动，不是“咕噜”过去
                list.style.transition = 'none';
                // 直接瞬间移动到最后的假图片上
                list.style.left = -5 * 650 + 'px';
                // 设置一个延时器，这个延时器的延时时间可以是0毫秒，虽然是0毫秒，但是可以让我们过渡先是瞬间取消，然后再加上
                setTimeout(function () {
                    // 加过渡
                    list.style.transition = 'left .5s ease 0s';
                    // idx改为真正的最后一张
                    idx = 4;
                    list.style.left = -idx * 650 + 'px';
                }, 0);
            } else {
                idx--;
                list.style.left = -idx * 650 + 'px';
            }

            // 函数节流
            setTimeout(function () {
                lock = true;
            }, 500);
        }
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/3fbe877e70ed4012ae31d72706f96ec1.png" style="width:70%;" />

原理：

<img src="https://img-blog.csdnimg.cn/direct/fdc832689ba24c0ebd3f8fee82a9499d.png" style="width:70%;" />

<img src="https://img-blog.csdnimg.cn/direct/526cc088b7fd431aa93ceef4391f586f.png" style="width:70%;" />

## 15.3 动画效果开发3-呼吸灯轮播图特效

> 呼吸灯轮播图，又叫：淡入淡出轮播图。此种轮播图需要将图片叠到一起。

```html
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        * {
            margin: 0;
            padding: 0;
        }

        .carousel {
            width: 650px;
            height: 360px;
            border: 1px solid #000;
            margin: 50px auto;
            position: relative;

        }

        .carousel ul {
            list-style: none;
        }

        .carousel ul li {
            position: absolute;
            top: 0;
            left: 0;
            /* 透明度都是0 */
            opacity: 0;
            transition: opacity 1s ease 0s;
        }

        /* 只有第一张透明度是1 */
        .carousel ul li:first-child {
            opacity: 1;
        }

        .carousel .leftbtn {
            position: absolute;
            left: 20px;
            top: 50%;
            margin-top: -25px;
            width: 50px;
            height: 50px;
            background-color: rgb(28, 180, 226);
            border-radius: 50%;
        }

        .carousel .rightbtn {
            position: absolute;
            right: 20px;
            top: 50%;
            margin-top: -25px;
            width: 50px;
            height: 50px;
            background-color: rgb(28, 180, 226);
            border-radius: 50%;
        }
    </style>
</head>

<body>
    <div class="carousel">
        <ul id="list">
            <li><img src="images/beijing/0.jpg" alt=""></li>
            <li><img src="images/beijing/1.jpg" alt=""></li>
            <li><img src="images/beijing/2.jpg" alt=""></li>
            <li><img src="images/beijing/3.jpg" alt=""></li>
            <li><img src="images/beijing/4.jpg" alt=""></li>
        </ul>
        <a href="javascript:;" class="leftbtn" id="leftbtn"></a>
        <a href="javascript:;" class="rightbtn" id="rightbtn"></a>
    </div>
    <script>
        // 得到按钮和ul，ul整体进行运动
        var leftbtn = document.getElementById('leftbtn');
        var rightbtn = document.getElementById('rightbtn');
        var list = document.getElementById('list');
        var lis = list.getElementsByTagName('li');

        // 当前是第几张图显示
        var idx = 0;

        // 节流
        var lock = true;

        // 右按钮
        rightbtn.onclick = function () {
            // 判断节流
            if (!lock) return;

            lock = false;

            // 还没有改idx，此时的idx这个图片就是老图，老图淡出
            lis[idx].style.opacity = 0;
            idx++;
            if (idx > 4) idx = 0;
            // 改了idx，此时的idx这个图片就是新图，新图淡入
            lis[idx].style.opacity = 1;

            // 动画结束之后，开锁
            setTimeout(function () {
                lock = true;
            }, 1000);
        }

        // 左按钮
        leftbtn.onclick = function () {
            // 判断节流
            if (!lock) return;

            lock = false;

            // 还没有改idx，此时的idx这个图片就是老图，老图淡出
            lis[idx].style.opacity = 0;
            idx--;
            if (idx < 0) idx = 4;
            // 改了idx，此时的idx这个图片就是新图，新图淡入
            lis[idx].style.opacity = 1;

            // 动画结束之后，开锁
            setTimeout(function () {
                lock = true;
            }, 1000);
        }
    </script>
</body>

</html>
```

<img src="https://img-blog.csdnimg.cn/direct/ee34060c8fdc4b699d7e458292f27c47.png" style="width:70%;" />

# 十六、JQuery

- [jQuery 教程 (w3school.com.cn)](https://www.w3school.com.cn/jquery/index.asp)

- [jQuery 教程 | 菜鸟教程 (runoob.com)](https://www.runoob.com/jquery/jquery-tutorial.html)
