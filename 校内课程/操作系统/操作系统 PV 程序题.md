---
title: 操作系统 PV 程序题
categories:
  - 校内课程
  - 操作系统
abbrlink: cc15ddd9
date: 2023-09-18 04:00:00
updated: 2023-09-18 04:00:00
---

<meta name="referrer" content="no-referrer"/>

注意：所有的互斥信号量的使用都是配对的!!!有wait操作就必然有signal操作!!!

类型定义 用 **semaphore** 就行

# 经典的消费者生产者问题

```c++
int full=0;
int empty=n;
int mutex=1;
Cobegin
Producer();Consumer();
Coend
Producer(){
    while(true){
        生产者生产物品
        wait(empty)
        wait(mutex)
        放入物品
        signal(mutex)
       	signal(full)
    } 
}
Consumer(){
    while(true){
       	wait(full)
        wait(mutex)
        消费物品
        signal(mutex)
        signal(empty)
    }
}
```

<!-- more -->

# 读者写者问题

允许多个进程同时读取一个共享对象；但是不允许一个写进程和其他读进程或者写进程同时访问一个共享对象

即保证 write 进程与其他进程互斥访问共享对象

```c++
semaphore rmutex=1,wmutex=1;
//这个rmutex是给 readcount 临界资源设置的互斥信号量，同时只能有一个读进程去操作
static int readcount=0;
Cobegin
reader();writer();
Coend
reader(){
    while(true){
        //可能有写进程
        if(readcount==0)
            wait(rmutex);
        
        //准备开始读，修改readcount
        wait(rmutex);
        readcount++;
        signal(rmutex);
        
       	读操作;
        
       	//读完了，修改readcount
        wait(rmutex);
        readcount--;
        signal(rmutex);
        
        //当读进程为0，写进程可以进入
        if(readcount==0)
           signal(wmutex);
    }
}
writer(){
    while(true){
        wait(wmutex);
        写;
        signal(rmutex);
    }
}
```

# 哲学家进餐问题

```c++
semaphore chopsticks[5]={1,1,1,1,1};//定义每根筷子的互斥信号量
Cobegin
Eat();
Coend
Eat(){
    while(true){
        wait(chopsticks[i]);
        wait(chopstick[(i+1)%5]);
        吃;
      	signal(chopstick[(i+1)%5]);
       	signal(chopsticks[i]);
    }
}
```

# 1、

n个并发进程共用一个公共变量Q，写出用信号灯的PV操作实现n个进程互斥的程序描述。

```c++
int mutex=1;//互斥
Cobegin
P1();P2();...;Pn();
Coend
Pi(){
    while(true){
        wait(mutex)
        访问Q
        signal(mutex)
    }
}
```

# 2、

如图1所示的进程流程图中，有8个进程合作完成某一任务，试说明这八个进程之间的同步关系，用PV操作实现之，并要求写出程序描述。

<img src="https://image.davidingplus.cn/images/2025/02/01/e96de0527e103c0fe045c6317cca4990.png" alt="e96de0527e103c0fe045c6317cca4990" style="zoom:67%;" />

```cpp
int s2=s3=s4=s35=s36=s37=s45=s46=s47=s28=s58=s68=0;
Cobegin
P1();P2()……P8();
Coend
P1(){
	while(1){
		执行P1;
		V(s2);
		V(s3);
		V(s4);
	}
}
P2(){
	while(1){
		P(s2);
		执行P2;
		V(s28);
	}
}
P3(){
	while(1){
		P(s3);
		执行P3;
		V(s35);
		V(s36);
		V(s37);
	}
}
P4(){
	while(1){
		P(s4);
		执行P4;
		V(s45);
		V(s46);
		V(s47);
	}
}
P5(){
	while(1){
		P(s35);
		P(s45);
		执行P5;
		V(s58);
	}
}
P6(){
	while(1){
		P(s36);
		P(s46);
		执行P6;
		V(s68);
	}
}
P7(){
	while(1){
		P(s37);
		P(s47);
		执行P7;
	}
}
P8(){
	while(1){
		P(s28);
		P(s58);
		P(s68);
		执行P8;
	}
}
```

# 3、

如图2所示，get/copy/put三进程共用两个缓冲区s、t（其大小为每次存放一个记录）。get进程负责不断把输入记录送入缓冲区s中，copy进程负责从缓冲区s中取出记录复制到缓冲区t中，而put进程负责把记录从缓冲区t中取出打印。试用PV操作实现这3个进程之间的同步。

![ef4647bd2c5eb8c55fe42945237c0a3c](https://image.davidingplus.cn/images/2025/02/01/ef4647bd2c5eb8c55fe42945237c0a3c.png)

```c++
int fullS=0;
int emptyS=1;
int fullT=0;
int emptyT=1;
semaphore mutexS=1;
semaphore mutexT=1;
Cobegin
get();copy();put();
Coend
get(){
    while(true){
        wait(emptyS);
        wait(mutexS);
        放入缓冲区s;
        signal(mutexS);
        signal(fullS);
    }
}
copy(){
    while(true){
        wait(fullS);
        wait(mutexS);
        从s中取出记录;
        signal(mutexS);
        signal(emptyS);
        wait(emptyT);
        wait(mutexT);
        放入缓冲区t;
        signal(mutexT);
        signal(fullT);
    }
}
put(){
    while(true){
        wait(fullT);
        wait(mutexT);
        拿出数据打印;
        signal(mutexT);
        signal(emptyT);
    }
}
```

# 4、

有一个阅览室，读者进入阅览室必须先在一张登记表TB上登记，该表为每一个座位设一个表目，读者离开时要消掉其登记信息，阅览室共有100个座位。请用PV操作写出进程间的同步算法。

```c++
int count=100;//座位数量
semaphore mutex=1;//登记表使用互斥
Cobegin
read();
Coend
read(){
    while(true){
        进入阅览室;
        wait(count);
        wait(mutex);
        登记;
        signal(mutex);
        读书;
        wait(mutex);
        撤销登记;
        signal(mutex);
        signal(count);
        离开;
    }
}
```

# 5、

设公共汽车上，司机和售票员的活动分别是：

司机的活动：启动车辆、正常行车、到站停车
售票员的活动：开车门、售票、关车门
在汽车不断到站、停站、行驶过程中，这两个活动有什么同步关系？请用信号量的PV操作实现。

```c++
int s1=0;//是否到站
int s2=0;//是否可以离站
Cobegin
driver();seller();
Coend
driver(){
    while(true){
        wait(s2);
        启动车辆;
        正常行车;
        到站停车;
        signal(s1);
    }
}
seller(){
    while(true){
        wait(s1);
        开车门;
        售票;
        关车门;
        signal(s2);
    }
}
```

# 6、

医生为某病员诊病，认为需要做些化验，于是，就为病员开出化验单，病员取样送到化验室，等待化验完毕交回化验结果，然后继续诊病。医生为病员诊病和化验分别是两个协作的进程。试用信号灯的PV操作描述这两个进程之间的同步。

```cpp
int s1=0;//是否有化验单
int s2=0;//是否有化验结果
医生诊病(){
	while(1){
	诊病;
	V(S1);
	P(S2);
	诊病;
	}
}
化验(){
	while(1){
	P(S1);
	化验;
	V(S2);
	}
}
```

# 7、

有个仓库，可以放A和B两种产品，仓库的存储空间足够大，但要求：

（1）一次只能放入一种产品（A或B）
（2）-N<A产品数量-B产品数量<M
其中，M和N是正整数，试用PV操作描述产品A和产品B的入库过程。

```cpp
int Sa=M-1;
int Sb=N-1;
int mutex=1;
Cobegin
存放A();存放B();
Coend
存放A(){
	while(1){
	P(Sa);
	P(mutex);
	放入A;
	V(mutex);
	V(Sb);
	}
}
存放B(){
	while(1){
	P(Sb);
	P(mutex);
	放入B;
	V(Sa);
	V(mutex);
	}
}
```

# 8、

![image-20230418190835692](https://image.davidingplus.cn/images/2025/02/01/image-20230418190835692.png)

```c++
int capacity=500;//能容纳的最大人数
int mutex=1;//出入口互斥锁
Cobegin
Visit();
Coend
Visit(){
    while(true){
        wait(capacity);
        wait(mutex);
        进门
        signal(mutex);
        参观;
        wait(mutex);
        出门;
        signal(mutex);
        signal(capacity);
    }
}
```

# 9、

![image-20230418191212388](https://image.davidingplus.cn/images/2025/02/01/image-20230418191212388.png)

```c++
int A1=x;//A信箱装的邮件数量
int A2=M-x;//A信箱还可以装的邮件数量
int B1=y;//B邮箱装的邮件数量
int B2=N-y;//B邮箱还以装的邮件数量
int mutexA=1;
int mutexB=1;//访问互斥锁
Cobegin
A();B();
Coend
A(){
    while(true){
        wait(A1);
        wait(mutexA);
        从A的信箱中取出一个信件;
        signal(mutexA);
        signal(A2);
        回答问题并提出一个新问题;
        wait(B2);
        wait(mutexB);
        将新邮件装入B邮箱;
        signal(mutexB);
        signal(B1);
    }
}
B(){
	while(true){
        wait(B1);
        wait(mutexB);
        从B的信箱中取出一个信件;
        signal(mutexB);
        signal(B2);
        回答问题并提出一个新问题;
        wait(A2);
        wait(mutexA);
        将新邮件装入A邮箱;
        signal(mutexA);
        signal(A1);
    }
}
```

# 10、

三个进程 P1、P2、P3 互斥使用一个包含 N（N>0）个单元的缓冲区。P1 每次用 produce() 生成一个正整数并用put()送入缓冲区某一个空单元中；P2 每次用 getodd()从该缓冲区中取出一个奇数并用 countodd()统计奇数个数；P3 每次用 geteven()从该缓冲区中取出一个偶数并用 counteven()统计偶数个数。请用信号量机制实现这三个进程的同步与互斥活动， 并说明所定义的信号量的含义。

```c++
semaphore mutex=1;//缓冲区使用互斥锁
int empty=N;//缓冲区空闲的个数
int odd=0;//奇数区个数
int even=0;//偶数区个数
Cobegin
P1();P2();P3()
Coend
P1(){
    while(true){
        wait(empty);
        wait(mutex);
        produce();
        put();
        signal(mutex);
        if(num&1)//奇数
            signal(odd);
       	else
            signal(even);
    }
}
P2(){
    while(true){
        wait(odd);
        wait(mutex);
        getodd();
        countodd()++;
        signal(mutex);
        signal(empty);
    }
}
P3(){
    while(true){
	    wait(even);
       	wait(mutex);
        geteven();
        counteven()++;
        signal(mutex);
        signal(empty);
    }
}
```

# 11、

一个野人部落从一个大锅中一起吃炖肉，这个大锅一次可以存放 M 人份的炖肉。当野人们想吃的时候，如果锅中不空，他们就自助着从大锅中吃肉。如果大锅空了，他们就叫 醒厨师，等待厨师再做一锅肉。 野人线程未同步的代码如下： while (true){ getServingFromPot() } 厨师线程未同步的代码如下： while (true) { putServingsInPot(M) } 同步的要求是： 当大锅空的时候，野人不能够调用 getServingFromPot() 仅当大锅为空的时候，大厨才能够调用 putServingsInPot() 问题：请写出使用 PV 满足同步要求的完整程序。

野人互斥着吃，厨师互斥着做

```c++
semaphore mutex=1;//使用锅的互斥信号量
semaphore full=0;//锅满了
semaphore empty=0;//锅空了
static int servings=0;//肉的数量
Cobegin
cook();eat();
Coend
cook(){
    while(true){
        wait(empty);
        wait(mutex);
        putServingsInPot(M);
        servings += M;
        signal(mutex);
        signal(full);  
    }
}
//这是一个野人的线程
eat(){
    while(true){
        wait(full);
        wait(mutex);
        getServingFromPot();
        servings -= 1;
        signal(mutex);
        if(servings == 0)
            signal(empty);
    }
}
```

# 12、

机场中有些道路，航班和摆渡车都可以通行。当航班通行时，摆渡车必须等待，直到所有航班通过后才能通行。当摆渡车通行时，航班必须等待，直到所有摆渡车通过后才能通行。请用信号量描述摆渡车和航班通行的控制过程。

```c++
semaphore mutex=1;//道路行驶的互斥锁
semaphore plane=1,car=1;//航班占据还是摆渡车占据
int plane_count=0;,car_count=0;//某时刻在道上的航班或者摆渡车的个数
Cobegin 
    Car();Plane();
Coend
Car(){
    while(true){
        if(car_count==0)
            wait(car);//如果没有车则表明可能有飞机，那么等待车的控制权
        car_count++;
        wait(mutex);
        开车！
        signal(mutex);
        car_count--;
        if(car_count==0)
            signal(plane);//如果车清空了，那么通知飞机可以接管了
    }
} 	
Plane(){
    ...//同理
}
  
```

