# 一、每日作业-JavaScript第02天

## 主观题

### 核心知识点

1. 打印0-20之间的整数, 将每个数输出到控制台

   ~~~javascript
   <script>
     for (let i = 0; i <= 20; i++) {
       console.log(`0~20之间的整数是: ${i}`)
     }
   </script>
   ~~~

2. 计算1-1000之间的累加和，并且打印输出到页面中

   ~~~html
   <script>
     let sum = 0
     for (let i = 1; i <= 1000; i++) {
       sum += i
     }
     console.log(`1~1000之间的累加和是: ${sum}`)
   </script>
   ~~~

3. 页面依次打印 100-200之间,可以被6整除的数字

   ~~~html
   <script>
     for (let i = 100; i <= 200; i++) {
       if (i % 6 === 0) {
         console.log(`100~200之间可以被6整除的是: ${i}`)
       }
     }
   </script>
   ~~~

4. 用户弹窗输入两个数字，页面输出两个数字中的最大值（请使用三元运算符完成）

   ~~~html
   课堂练习，不需要pink老师写了吧
   ~~~

5. **用户输入一个数，  计算 1 到这个数的累加和** 

   - 比如 用户输入的是 5， 则计算 1~5 之间的累加和
   - 比如用户输入的是10， 则计算 1~10 之间的累加和

   ~~~html
   <script>
       let num = prompt('请输入一个数字')
       let sum = 0
       for (let i = 1; i <= num; i++) {
         sum += i
       }
       console.log(`累加和是：${sum}`)
     </script>
   ~~~

6. **输出成绩案例（请使用if多分支来书写程序）**

   题目描述：

   接收用户输入的分数，根据分数输出对应的等级字母 A、B、C、D、E，

   - 90分(含)以上 ，输出：A
     80分(含)~ 90 分(不含)，输出：B	
      70分(含)~ 80 分(不含)，输出：C 	
      60分(含)~ 70 分(不含)，输出：D   	
   - 60分(不含) 以下，输出 E

   ~~~html
   课堂案例类似不需要pink老师再啰嗦了吧~~
   ~~~

7. **输出星期练习（请使用switch分支语句来书写程序）**

   题目描述：

   请用户输入1个星期数. 就将对应的英文的星期打印出来. 

   - 比如用户输入'星期一'， 则 页面可以打印  monday 
   - 英文自己查有道。比如星期一是  monday  星期二是 tuesday

   ~~~html
   <script>
     // 注意获得的是字符串，所以下面的case 数字要加引号
     let day = prompt('请输入1~7之间的一个数字:')
     switch (day) {
       case '1':
         console.log('星期一')
         break
         case '2':
         console.log('星期二')
         break
         case '3':
         console.log('星期三')
         break
         case '4':
         console.log('星期四')
         break
         case '5':
         console.log('星期五')
         break
         case '6':
         console.log('星期六')
         break
         case '7':
         console.log('星期天')
         break
     }
   </script>
   ~~~

   ​

8. **用户登录验证**

   题目描述：

   接收用户输入的用户名和密码，若用户名为 “admin” ,且密码为 “123456” ,则提示用户登录成功!  否则，让用户一直输入。

   ~~~javascript
   while (true) {
       let msg = prompt('名字')
       let pwd = prompt('密码')
       if (msg === 'admin' && pwd === '123456') {
           break
       }
   }
   alert('登录成功')

   ~~~

### 综合案例变形

需求：根据用户选择计算两个数的结果：

**题目描述：**

打开页面出现一个提示框，注意是一直提示的，'请您选择 + - * / ，如果输入q，则是退出结束程序

- 如果输入的是 + - * / 其中任何一个，比如用户输入是 + ，则是计算求和，如果用户输入是 *  则是计算乘积
  - 则提示第一个弹窗，提示用户：'请您输入第一个数字'
  - 输入完毕则继续提示第二个弹窗，提示用户：'请您输入第二个数字'
  - 都输入完毕，则通过警示框 alert 输出结果
- 如果输入是 q，则结束程序

提示：多分支请使用 if 多分支来完成

具体效果如图：

 <img src="assets/111.gif">

~~~html
<script>
  while (true) {
    let re = prompt(`
      1. 输入 + - * / 计算两个数字的结果
      2. 如果是q则是退出
    `)
    if (re === 'q') {
      break
    } else {
      let x = +prompt('请输入第一个数字')
      let y = +prompt('请输入第二个数字')
      if (re === '+') {
        alert(`两个数的和是: ${x + y}`)
      } if (re === '-') {
        alert(`两个数的差是: ${x - y}`)
      } if (re === '*') {
        alert(`两个数的乘积是: ${x * y}`)
      } if (re === '/') {
        alert(`两个数的求商是: ${x / y}`)
      }
    }
  }
</script>
~~~



## 排错题

### 排错题1

~~~javascript
<!-- 请问以下代码会出现什么问题，如何解决？ -->
<script>
  // 需求： 求用户输入2个数字的和
 
  let num1 = prompt('请输入第一个值')  // 答案： prompt获取的是字符串，需要转换为数字型
  let num2 = prompt('请输入第二个值')  // 答案： prompt获取的是字符串，需要转换为数字型
  alert(num1 + num2)
</script>
~~~

### 排错题2

~~~javascript
<!-- 请问下面代码会出现什么问题，怎么去解决？ -->
<script>
  // 判断用户名的案例，用户会输入用户名
  // 1. 如果用户名输入'迪丽热巴'，则页面弹出 '用户名输入正确'
  // 2. 如果用户名输入不是'迪丽热巴'，否则弹出'用户名输入不正确'
	 
  let username = prompt('请输入用户名:')
  if (username = '迪丽热巴') { // 答案： 单等是赋值，这里永远是正确的，需要把判断条件改为  ===
    alert('用户名输入正确')
  } else {
    alert('用户名输入不正确')
  }
</script>
~~~

### 排错题3

~~~html
<body>
  <!-- 请问以下代码会出什么问题？如何解决？ -->
  <script>
    // 需求，用户输入1~10之间的整数，则循环打印用户输入的次数
    // 注意此处有2个错误，找出并且修正
		
    let num = prompt('请输入一个1~10之间的整数')
    let i = 1
    while (i < num) {  // 1. 从1开始要小于等于， 从 0开始 可以写小于
      console.log(`我是第${i}句话`)
      // 2. 落下了 i++
      
    }
  </script>
</body>
~~~

### 排错题4

~~~html
<body>
  <!-- bug:请你找到下面代码代码穿透的原因,并找到匹配不上case里面的值的问题进行修改 -->

  <script>
    // 需求： 用户输入1~4之间整数，对应输出 '春' '夏' '秋' '冬' 
    // 例如用户输入 1 则输出 '春' ，用户输入 2，则输出 '夏' 以此类推
    // 注意： 此处有2个错误，找出并且修正
    let num = +prompt('请你输入一个1-4之间的值')
    switch (num) {
      case '1':  // 第一个错误： prompt 转换为数字型，此时的 1 应该不加引号
        alert('春')  // 第二个错误： 落下了 break ，下面case 都落下了
      case '2':
        alert('夏')
      case '3':
        alert('秋')
      case '4':
        alert('冬')
      default:
        alert('请输入1~4之间整数')
        break
    }

  </script>
~~~



## 客观题

地址：https://ks.wjx.top/vm/tjmrvAg.aspx# 

请扫码做题 

 ![67263889554](assets/1672638895542.png)



## 简答题：关键字汇总( 能够简单的说出来这些关键字干啥用的 )

- let    声明变量
- typeof   检测数据类型
- if   条件语句如果
- else   否则 
- switch    分支语句   
- case   选项
- default      默认语句
- while    
- break    退出循环
- continue    继续下一次循环


## 关注pink老师

关于pink老师抖音（黑马pink讲前端），领取学习路线图、面试宝典以及八大学科的基础视频哦~~

 ![67263896570](assets/1672638965706.png)



