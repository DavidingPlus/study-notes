---
title: 标准库string的sso优化对LVector插入影响的探究
categories:
  - 实习
  - 合迅科技
  - 课题研究
abbrlink: b67fd3f4
date: 2024-02-28 17:55:00
updated: 2024-02-29 10:15:00
---

<meta name="referrer" content="no-referrer"/>

钟老师让自己的做的一个课题研究，主题是`标准库string的sso优化对LVector插入的影响`。

# 问题背景

`LVector`在插入`std::string`的时候遇到了问题，钟老师研究了一段时间，我接着他的成果继续探讨，将学到的内容总结在这里。

# std::string的优化

标准库的`std::string`其实是做了优化的，不同的编译器实现的细节可能不同，但是基本的大思路框架都是一样的。

注：以下都是理论上的思路分析，具体的底层代码请自行查阅资料。

<!-- more -->

## 基本内存模型

我们熟知的`std::string`的内存模型大致是这样的

- 栈区当中存放容量`capacity`，大小`size`和一根指向堆区数据区域的指针，三个分别占据`8`字节，总共`24`字节
- 堆区当中`data`是实际存放数据的地方，通过分配器分配出来的（默认使用`std::allocator`，实际就是`new`出来的），`std::string`中的`c_str()`方法获取的就是堆区这个数据区首地址

<img src="https://img-blog.csdnimg.cn/direct/45dab91a4ec140b7b19742b06166e180.png" alt="image-20240228155812377" style="zoom:80%;" />

## COW优化（现在不用）

`COW`，即为`Copy-On-Write`，写时拷贝。

提到这里，我首先想到了`Linux`当中父子进程的"**读时共享，写时拷贝**"，父子进程在读的时候共享用户区的数据，例如先`open()`一个文件，在`fork()`，父子进程的文件描述符是同一个，具体可以表现为父进程读`2`个字节，对于子进程的文件偏移指针也向后偏移`2`个字节；这就是因为用户区的文件描述符表是读时共享的；当需要修改用户区的数据，比如一个变量，就会做拷贝操作了，这点毋庸置疑，这也是优化性能的一种策略。

参上，`std::string`的`COW`优化也是一个道理，具体如下图

- 在读取的时候共享`data`数据内存区域，当需要修改（写）的时候，`str2`就做自己的拷贝

<img src="https://img-blog.csdnimg.cn/direct/344d329b170b403697fbde02c2382c46.png" alt="image-20240228161141491" style="zoom:75%;" />

### 存在的问题

看到上面，可能会想，`COW`这么好，为啥标题还是现在一般不用呢？

之所以不用，是因为这种机制在**多线程**当中可能会出现不可预期的乱七八糟的问题，具体自行查阅资料，这里不作阐述。

## SSO优化

所以就有了`SSO`优化，即`Small String Optimization`，翻译过来就是**短字符串优化**。

那么为什么需要短字符串优化呢？看基本的内存模型

<img src="https://img-blog.csdnimg.cn/direct/f808303727ea4007a9f1e576552c0e21.png" alt="image-20240228161643461" style="zoom: 80%;" />

想象一下，如果我的字符串比较短，举个例子，小到`8`个字节就能存下，那么是不是就不用存一个指针了，直接在栈区存储即可，还不用去堆区开辟空间，还不用考虑堆区内存释放的问题，岂不美哉？

我们再考虑一下，`8`个字节的`capacity`和`8`个字节的`size`最大能表示多少的数？`2^63 - 1`，这也太大了吧，完全没必要，因此`capacity`和`size`也可以做进一步优化，注意不同编译器的实现不同，但是思路都是这样，能砍的就砍。当然不管如何，里面存放的`pointer`是不会变的，因为`std::string`中还有`c_str()`接口，不能让功能变了。大概的优化模型如下，可以看到数据在栈区，这个时候指针指向自身内部的`data`，合理，非常合理。

<img src="https://img-blog.csdnimg.cn/direct/47a1685e27bf43b2a3207a5d110c0332.png" alt="image-20240228163129862" style="zoom:75%;" />

当字符串的长度变长，达到长字符串的标准（不同编译器的规定不一样），就会恢复一般的内存模型

关于`SSO`优化的下的字符串的拷贝，由于内存模型中仍然存在指针，显然是一个深拷贝，可以写一个程序测试一下，顺便看一下`std::string`的`SSO`优化的表现

~~~cpp
#include <iostream>
#include <string>

int main()
{
    std::string a{"one"};
    auto b = a;

    std::string c{"twooooooooooooooooooooooooooooooooooooooooooooo"};

    printf("a address: %p, c_str address: %p\n", &a, a.c_str());
    printf("b address: %p, c_str address: %p\n", &b, b.c_str());
    printf("c address: %p, c_str address: %p\n", &c, c.c_str());

    return 0;
}
~~~

执行结果，可以发现完美验证了我们的分析

- `a`到`b`经过了一次深拷贝，他们两个的数据区地址不同
- `a、b`的本类地址和数据区地址非常相近，而`c`离的很远

![image-20240228163645469](https://img-blog.csdnimg.cn/direct/5161d6b19fef44d692239e932ab570f8.png)

当然`SSO`优化也存在一定问题，这就不是这个课题的重点了，请自行查阅资料。

# 在LVector中插入std::string

## 问题背景

测试`LVector`的时候，发现`prepend std::string`的时候程序崩溃，返回的值也不符合预期

![image-20240228164611062](https://img-blog.csdnimg.cn/direct/0f3bf79d27fb484cb78102c33ae4aba4.png)

## 具体分析

我们先不管这个`LVector`是如何实现的，我们知道`prepend`函数，肯定是调用`insert`方法，所以去查`insert`函数，以下只给出关键代码，其余都是针对代码的显而易见的分析（代码不是我写的，是钟老师写的，我只是阅读）

真正做插入的函数叫`insertMultiple()`，里面有一些算法，不用管他，我们考虑`insert`函数的逻辑，在目标处进行插入，然后需要把其他的数据前移或者后移，也就是说，需要做内存的移动或者拷贝，问题就处在这里，也就是代码中的`moveMemory()`中

~~~cpp
template <typename T>
inline void LPaddedVector<T>::moveMemory(T *pTargetAddress, T *pSourceAddress, std::size_t count)
{
    std::memmove(pTargetAddress, pSourceAddress, count * sizeof(T));
}
~~~

可以发现，`moveMemory`是直接调用的`std::memmove`，直接把原内存给移动过去了，乍一看好像没什么毛病，搬运就搬运呗，但是注意，使用了`std::memmove`，对象仅仅是换了一个位置，里面的数据什么都没变，现在把这个同`std::string`的`SSO`优化结合起来

- 从左边移动到右边，`capacity`、`size`、`data`都没有问题，关键在于这个指针，前面说过，里面的数据仅仅是换了一个位置，那指针指向的还是原先的地址啊，而原先的地址现在如何？不知道，可能被覆盖，可能被释放了，因此就会出现上面的问题

<img src="https://img-blog.csdnimg.cn/direct/12e9272dacce47829218f3032314bfbb.png" alt="image-20240228165833325" style="zoom: 67%;" />

## 问题解决

分析了问题的来源，那解决问题就好办了，比如可以为`std::string`做特化，让他在这里使用拷贝的策略，这样能解决问题

- 注意：这里的拷贝和前面移动导致的类似浅拷贝不一样，这里的拷贝是通过分配器构造，实际上调用的是`std::string`的拷贝构造函数，不管`std::string`是哪种优化方式，深拷贝他是必然做的，也就是说那根`pointer`就指向的是自身的`data`而不是之前的了，这样就是对的

~~~cpp
template <>
inline void LPaddedVector<std::string>::copyConstruct(std::string *pTargetAddress, const std::string &itemToCopy)
{
    sm_allocator.construct(pTargetAddress, itemToCopy.data(), itemToCopy.size());
}

template <>
inline void LPaddedVector<std::string>::moveMemory(std::string *pTargetAddress, std::string *pSourceAddress, std::size_t count)
{
    if (pTargetAddress < pSourceAddress)
    {
        for (int i = 0; i < count; i++)
        {
            sm_allocator.construct(pTargetAddress + i, pSourceAddress[i].data(), pSourceAddress[i].size());
            sm_allocator.destroy(pSourceAddress + i);
        }
    }
    else if (pTargetAddress > pSourceAddress)
    {
        for (int i = count - 1; i >= 0; i--)
        {
            sm_allocator.construct(pTargetAddress + i, pSourceAddress[i].data(), pSourceAddress[i].size());
            sm_allocator.destroy(pSourceAddress + i);
        }
    }
}
~~~

## 继续思考

那么问题来了，如果有一个自定义类型，里面含有`std::string`，那又该怎么办呢？

例如下面的测试

~~~cpp
struct DataT
{
    int x;
    std::string desc;
};
~~~

![image-20240228170626962](https://img-blog.csdnimg.cn/direct/4db04a293bfd4b55bfff38c3259b62d8.png)

我第一反应想到的就是能不能用`c++`通过某种手段判断一个类当中是否含有指定类型例如`std::string`的成员变量，但由于水平不够，或者因为本来就不太好使，这条路走不通

所以就只能从刚才提到的`moveMemory()`入手了，既然直接移动不好，那我干脆改成拷贝不行吗？当然不好，白白多了很多次拷贝，这是不可接受的，那有没有办法将二者结合起来呢？你别说，还真有。

参考了`Qt`的部分实现，`Qt`中封装了一个叫`QTypeInfoQuery`的类，里面有一个变量`isRelocatable`，这个东西可以用来判断类能否**平凡可复制**，顾名思义，像`std::string`显然不能平凡可复制，因为`SSO`的优化，平凡复制的话指针指向的地方是原来的，显然不行，说白了就是类似浅拷贝，因此这里做了判断，如果不行就拷贝，可以就移动

<img src="https://img-blog.csdnimg.cn/direct/b98cdfae9ae5411c8c4489f4e41772c6.png" alt="8a91e41e1e61a64d333c6494ae960975" style="zoom:75%;" />

关于`std::string`堆内存那个模型，是满足平凡可复制条件的，画个图如下理解

- `std::memmove`不会触发类的析构函数，因此堆内存还在，不会被释放，因此就做到了完美迁移，同时避免了不必要的栈内存和堆内存的拷贝，提升了效率

<img src="https://img-blog.csdnimg.cn/direct/c1b1f4b05f534b42a7cd6298001c154d.png" alt="image-20240228171845622" style="zoom:70%;" />

在查看了`QTypeInfoQuery`的`isRelocatable`的来源之后，我发现调用的是标准库的一个`type_traits`，叫`is_trivially_copyable`，可以判断是否可以平凡可复制，平凡可复制的含义见上

这里写了一个程序测试

~~~cpp
#include <iostream>
#include <type_traits>

struct DataT1
{
    int x;
    std::string str;
};

struct DataT2
{
    int x;
    DataT1 t;
};

struct DataT3
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
};

class DataT4
{

public:
    DataT4() { p = new int(0); }

    ~DataT4()
    {
        if (p)
        {
            delete p;
            p = nullptr;
        }
    }

    int *p = nullptr;
};

int main()
{
    std::cout << std::is_trivially_copyable<DataT1>::value << '\n';
    std::cout << std::is_trivially_copyable<DataT2>::value << '\n';
    std::cout << std::is_trivially_copyable<DataT3>::value << '\n';
    std::cout << std::is_trivially_copyable<DataT4>::value << '\n';

    return 0;
}
~~~

执行结果

- 前两个类由于具有`std::string`（第二个类套娃，也算），返回`0`，不可平凡可复制
- 第三个类，全是一些`int`，显然可以
- 第四个类，堆内存，这个设计和一般的`std::string`是一样的，返回的是`0`

![image-20240228172702844](https://img-blog.csdnimg.cn/direct/0e5398ca0453476081c63f0945f06573.png)

好，现在问题来了，我们刚才说借助`type_traits`来进行判断，好决定是通过移动还是通过拷贝，代码甚至我都写好了

~~~cpp
template <typename T>
inline void LPaddedVector<T>::moveMemory(T *pTargetAddress, T *pSourceAddress, std::size_t count)
{
    if (std::is_trivially_copyable<T>::value)
    {
        // 如果是平凡可复制类型，可以直接使用 std::memmove
        std::memmove(pTargetAddress, pSourceAddress, count * sizeof(T));
    }
    else
    {
        // 如果不是，逐个元素的拷贝
        // 注意源地址和目标地址的位置不同，拷贝的顺序也不同
        if (pTargetAddress < pSourceAddress)
        {
            for (int i = 0; i < count; ++i)
            {
                sm_allocator.construct(pTargetAddress + i, *(pSourceAddress + i));
                sm_allocator.destroy(pSourceAddress + i);
            }
        }
        else if (pTargetAddress > pSourceAddress)
        {
            for (int i = count - 1; i >= 0; --i)
            {
                sm_allocator.construct(pTargetAddress + i, *(pSourceAddress + i));
                sm_allocator.destroy(pSourceAddress + i);
            }
        }
    }
}
~~~

我们还是考虑刚才的堆内存模型，经过判断之后是走拷贝这一条路，但是没必要啊，`std::memmove`不会激活对象的析构函数，我把所有数据移动到另一个地方，这个指针还是指向堆区的这一块内存，也就是说，实际上这个内存模型是平凡可复制的，这也是钟老师最初的想法（我研究了这么久才到大佬的初步想法，`e`了），这一点优化，就导致了最开始的问题，但是我们必须要有这种思维。

那么就没有解决方案了吗？

其实是有的，可以在这个判断之前再加上一层判断，例如某个类就是这种堆内存模型，经过分析他其实是可以平凡可复制的，那么我通过某种手段，例如宏，在那个类当中提供一种注册的方式，我保证这个类的行为是平凡可复制的，执行到这里之后先执行这个判断，如果`ok`，那直接走移动的道路，目前这样来看比较合理。这也是钟老师目前总结之后的研发需求。

