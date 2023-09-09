本 C++ 编码规范适用于合迅智灵产品。

本规范中某些章节仅针对合迅智灵特定的产品线的代码项目。对于此类章节，在章节开头作出标记，例如：

> 本章节内容适用于 XXX 产品线项目。
> {.is-info}

| 版本 | 日期 | 说明 |
| ---- | ---- | :--- |
| v0.1 | 2023.3.30 | 评审稿 |
| v0.2 | 2023.4.10 | 评审稿 |
| v1.0 | 2023.4.19 | 初版定稿 |


# 一、常规编码规范

## 1 头文件规范

### 1.1 自我包含原则

“自我包含”的头文件含义是：用户不需为了包含本头文件而包含额外的头文件。换言之，该头文件永远可以作为第一个头文件被引入（即该头文件内容完全不依赖于其他头文件内容）。

### 1.2 #define 保护

为避免多重包含，每个头文件都需要实施 `#define` 宏保护。

> Lark5 写法：`_[全大写的无后缀文件名]_H_`
> {.is-info}

```cpp
// #define 保护应放在文件头注释之后，文件头注释参见注释规范章节
// 假定文件名为：myclass.h

#ifndef _MYCLASS_H_
#define _MYCLASS_H_

// 文件内容部分 ...

#endif
```

> Lark4 写法：`[全大写的无后缀文件名]_H`
> {.is-info}

```cpp
// #define 保护应放在文件头注释之后，文件头注释参见注释规范章节
// 假定文件名为：myclass.h

#ifndef MYCLASS_H
#define MYCLASS_H

// 文件内容部分 ...

#endif
```


> 出于可移植性考虑，禁止使用 `#pragma once` 等依赖于平台的解决方案。

### 1.3 `#include` 规范

#### 1.3.1 使用时机

尽量在源文件（`.cpp`）内包含，而非无脑在头文件（`.h`）内包含。

在不影响编译的前提下（如涉及到复杂的模板类），尽可能以*在头文件中使用前置声明，在源文件中包含*的方式，代替头文件中的 `#include`。

#### 1.3.2 引入路径

包含项目内的头文件（源码头文件和项目内预编译的第三方库头文件）时，用双引号（`"xxxx.h"`）形式；包含系统头文件（系统库和系统内安装的第三方库）时，用尖括号（`<xxxx.h>`）形式。

> 按照目前 LarkSDK 的 CMake 配置，项目所有源码目录和子目录均已加入 `include_directories`，因此本项目内所有的头文件在包含时直接写文件名即可。

#### 1.3.3 包含顺序

包含头文件应遵循如下顺序：

1. 若当前文件为源文件，优先引入与实现文件对应的头文件；
2. C 库头文件；
3. C++ 库头文件；
4. 第三方库头文件（系统安装的）；
5. 第三方库头文件（项目内预编译的）；
6. 本项目其他模块头文件。

> 优先引入对应实现文件（如 `myclass.cpp`）对应的头文件（如 `myclass.h`），目的是为了保证当 `myclass.h` 遗漏某些必要的库时，`myclass.cpp` 的构建会立刻中止。保证首先看到关于当前实现的功能的错误信息，而不是来自其他包的错误信息。

上述顺序同时将 `#include` 语句分为多个“包含组”，每一组之间以空行分隔，以最大化可读性。例如：

```cpp
// 文件：lfont.cpp
// 仅示例，不代表实际代码情况

#include "lfont.h"

#include <wchar.h>

#include <memory>
#include <string>

#include <X11/Xlib.h>

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftbitmap.h"
#include "freetype/fterrors.h"

#include "lstring.h"
#include "lvector.h"
#include "lelements.h"

// 文件内容部分 ...

```

#### 1.3.4 第三方头文件

禁止在对外（面向用户的）头文件中直接包含第三方库头文件。

> *对外头文件*的定义为开放给用户代码包含的头文件。若对外头文件中 `#include` 了第三方头文件，则在编译用户代码的时候，会报找不到第三方头文件的错误。
>
> 一个成熟的代码库是不应该把任何*开发时*用到的第三方文件暴露给用户的。

## 2 作用域规范

### 2.1 命名空间

原则上，命名空间是*产品设计的一部分*（类比 Qt 提供的 `Qt` 命名空间），没有详细设计文档支持的情况下，禁止项目组成员自行定义任何非匿名的命名空间。

允许出于避免污染全局命名空间的目的，在 `.cpp` 文件内定义匿名的命名空间。见下节说明。

### 2.2 全局函数/变量

原则上，**禁止使用裸全局函数/变量**。

原则上，**禁止使用 extern 关键字**声明外部函数/变量。

> 以上两种写法破坏了模块封装性。非必要情况不允许使用。

### 2.3 非成员函数/变量

定义*非成员函数/变量*为仅当前模块内部使用，但并不定义在当前模块的某个类中，一般不提供给外部（其他模块和用户代码）使用的函数或变量。使用内部函数应遵循以下守则：

- 尽可能使用私有的静态成员函数/变量代替非成员函数/变量。私有的静态成员在模块内部可看作全局，在模块外则是不可见的（若准备提供给外部使用，考虑非私有的静态成员）；
- 若确实不方便使用私有静态成员，可在 `.cpp` 源文件（不可在 `.h` 头文件），将内部函数/变量以匿名命名空间包裹。若如此做，须在命名空间结束之处加上注释 `namespace`（注意与最后一个 } 空格隔开）。这样匿名命名空间里的实体，仅在本 `.cpp` 文件内部可用。

```cpp
// 文件：ooxx.cpp
// 再次声明：禁止在头文件里如此做

namespace
{
    int foo();  // 模块内部函数
    int bar;    // 模块内部变量
} // namespace

foo();  // 合法的访问
```

```cpp
// 文件：xxoo.cpp
// 这里将无法访问其他 cpp 文件里定义的匿名命名空间

foo();  // 非法的访问
```

### 2.4 最小作用域原则

对于**基本类型**变量，将变量尽可能置于最小的作用域内，并在变量声明时就进行初始化。禁止出现未初始化的变量。

```cpp
int foo;        // 错误，禁止出现未初始化的变量
int bar = 0;    // 正确用法，即使 bar 马上就要被重新赋值

for (int i = 0; i < 10; i++)
{
    int baz = 0;    // 正确用法，限制在 for 语句的循环体作用域内
    // Do something with baz ...
}
```

对于非基本类型（如复杂类对象），考虑到默认构造函数的调用，以及多次构造和析构的开销，不需严格遵循上述规则。

## 3 类规范

### 3.1 成员声明顺序

编写类定义时，按可访问性分“区段”，以如下顺序排列成员区段：

1. public
2. protected
3. private

在每一个区段内，按如下顺序排列不同类型的成员：

1. 枚举类型
2. 结构体类型（如需定义在类内）
3. 构造函数
4. 析构函数
5. 其他非静态成员函数
6. 静态成员函数
7. 信号对象
8. 成员变量

### 3.2 结构体

由于 C++ 的结构体和类并无本质区别，我们只能主观从*语义*上区别如下：

`struct` 通常用来定义仅包含数据，不包含“行为”的对象, 除了存取数据成员之外不应有别的函数功能（即“只有数据，没有行为”）。并且存取功能是通过直接访问域，而非通过 getter/setter 成员。除了构造、析构、初始化、重置、验证等类似的函数外，不能提供任何其它功能的函数。

若使用结构体，代表你遵循其成员默认为 `public` 的事实。一般不允许在结构体内部定义非 `public` 的成员。

如果一个对象存在“行为”，即使用 `class`。若拿不准该使用什么，优先考虑 `class`。

结构体的设计需严格考虑使用场景，以及结构体是否需要对用户暴露。原则上不允许自行定义对外结构体。若需定义仅模块内部使用的结构体（通常用于算法设计中），需将结构体定义在类的内部，避免对外暴露。

### 3.3 内联函数

只有当函数体少于 5 行时，才允许定义为内联函数。

构造和析构函数一般不定义为内联函数。

鼓励 getter 和 setter 成员设置为内联函数。

### 3.4 重写父类方法

重写父类方法时，必须加 `override` 关键字；

```cpp
// 基类
class BaseClass
{
protected:
    virtual int someMethod();
};

// 派生类
class DerivedClass : public BaseClass
{
protected:
    int someMethod() override;
}
```

### 3.5 构造函数初始化列表

出于令代码更为简洁的目的，在需要初始化的成员变量较多时，尽可能使用构造函数初始化列表，而不是在构造函数体内部一一赋值。

> 参考：http://c.biancheng.net/view/2223.html

继承父类和构造函数初始化列表同时存在时，建议把要继承的父类和类保持在同一行，接下来每一行初始化一个成员变量：

基本类型成员变量，包括指针变量，不要求一定要在初始化列表内初始化，也可以选择在定义的时候就赋予初始值（例如，一般 `int` 类型赋初始值 `0`，指针变量赋初始值 `nullptr`）。

即使在构造函数中马上就要为成员变量赋新值，也必须无条件初始化成员函数。

```cpp
// 文件：myclass.h

class MyClass : public SomeBaseClass
{
public:
    MyClass();
private:
    int m_someInteger = 0;          // 基本类型成员变量初始化
    int* m_pSomePointer = nullptr;  // 指针类型成员变量初始化
    OtherClass m_someObject;        // 复杂类型成员留待构造函数初始化列表初始化
}
```

```cpp
// 文件：myclass.cpp

MyClass:: MyClass() : SomeBaseClass(),
    m_someObject()                  // 复杂类型成员初始化
{
    // ...
}
```

### 3.6 其他注意事项

- 非特殊情况，非方法的数据成员，如成员变量，不允许声明为或 `public`；
  - 信号成员就是一个例外。参见“信号命名”章节
- 为避免隐式类型转换，声明单参数构造函数时，应以 `explicit` 修饰；
  - 特殊情况允许例外，如 `const char *` 隐式转换为 `LString` 允许隐式转换以避免代码繁琐
- 如果类需要被继承，则其析构函数必须以 `virtual` 修饰为虚函数；
- 对于明确不会修改对象自身数据的成员函数，一律用 `const` 修饰，如 getter 函数；

```cpp
// 文件：lfont.h

class LFont
{
public:
    // ...
    int fontSize() const;
    // ...
}
```

- 若成员函数返回对象内部某个成员引用，且需要避免该成员函数的调用者修改对象内部成员的场合，允许返回 `const` 引用（以 `const` 修饰返回值）；

> **注**：这种情况很少，因为一般不允许外部不通过类提供的接口就修改类成员。若确实需要定义返回常引用的成员，须有充分且正式的理由，并明确体现在详设中。

- 若虽然返回的是引用，但无所谓外部修改不修改的场合（如返回一个新建立的 `LString` 引用），则禁止使用 `const` 修饰返回值，以免引起语义上的混淆。

> 关于 `const` 关键字的合理使用经验，请参考：https://www.cprogramming.com/tutorial/const_correctness.html

## 4 函数规范

### 4.1 参数顺序

函数参数列表遵循先输入后输出的排列顺序。排序如下：

1. 输入参数
2. 输出参数
3. 具备默认值的输入参数

> 由于 C++ 语法限制，具备默认值的输入参数必须放在最后面，但这将打破先输入后输出的习惯性规则。因此函数在设计时，尽可能避免参数列表中同时出现输出参数和具备默认值的输入参数。

### 4.2 按引用传递的参数

所有按引用传递的参数，一律用 `const` 修饰。若确实需要修改某参数的值，原则上参数按指针传递。

```cpp
void setText(const LString &text);  // 按引用传递需用 const 修饰
```

## 5 命名规范

### 5.1 通用规范

函数、变量、文件命名需具备*描述性*，少用缩写。尤其是不要用只有项目组成员能理解的缩写。多将自己置于用户代码编写者的角度考虑问题，禁止出现 `aaa`、`x132`、`waaaargh` 等不专业的命名。

对于缩写的使用，遵循经验原则：*若将该缩写展开，是否会引起读者困扰*。例如 IP、DNS 等单词，估计不会有人在符号命名中想看到他们的全称 InternetProtocol、DomainNameSystem。这种时候即可放心的使用缩写。

关于在驼峰式命名中使用多字母缩写的时候的大小写问题，如 `ID` 和 `Id` 之争，目前无法作出具体规范。只能粗略制定以下原则：

- 对于*指代一个具体产品的专有名词*，如 OpenGL、D2D（Direct2D），出于美观考虑不建议写成 `OpenGl`、`D2d`，可以直接写 `OpenGL`、`D2D`，如 `LOpenGLEngine`；
- 对于*非专有名词*，如 HTTP、XML，在不影响美观的前提下，建议写成 `Http`、`Xml`。

对于一些广泛使用的缩写或习惯命名，例如用 `i` 表述循环变量、`it` 表示迭代器变量、`e` 表示事件对象，都是可以接受的。

### 5.2 类型命名

定义以下实体为*类型*：

- 类
- 结构体
- `typedef`（类型定义）
- 枚举

类型命名采用大驼峰，每个单词首字母均大写。非特殊情况不允许出现数字，禁止出现下划线等特殊字符。

#### 5.2.1 类

使用大驼峰命名。

```cpp
class MyAwesomeClass;
class AnotherCoolClass;
```

> **Lark5 类：**
>
> 在类型命名通用规则基础上，添加前缀大写字母 `L`。
>
> ```cpp
> class LObject;
> class LString;
> class LLatin1String;
> class LWindowApplication;
> ```

#### 5.2.2 结构体

在类命名规范上，添加 `Struct` 后缀，表示结构体。

> **Lark5 结构体：**
>
> 在结构体命名通用规则基础上，添加前缀大写字母 `L`。
>
> ```cpp
> struct LPropertyInfoStruct;
> struct LObjectInfoStruct;
> ```

##### 5.2.2.1 内部结构体

若结构体定义在类内部，为类内部结构体。

类内部结构体命名无需添加前缀和后缀，遵循类型命名通用规则。

禁止匿名结构体。

```cpp
class MyClass
{
private:
    struct MyData
    {
        ...
    };
};
```

#### 5.2.3 typedef

原则上 `typedef` 的命名规范与 `class` 类的命名规范相同。

和命名空间同理，typedef 也是*产品设计的一部分*（类比 Qt 的 `QObjectList`）。在没有详设的情况下，不允许项目组成员自行编写 `typedef`；

将 `typedef` 看作类（尤其是通过模板组合得到的复杂类）的*别名*，因此采用和类相同的命名规范。

> 不要使用类似 `typedef unsigned int uint` 的方式简化代码。
>
> 如果需要使用简化类型定义，考虑使用 `#include <cinttypes>` 等手段。

#### 5.2.4 枚举

遵循类型命名通用规则，无需额外前缀和后缀。

禁止匿名枚举类型。

对于枚举值也采用相同的规则。

```cpp
// 文件：lobject.h

enum ConnectionMode
{
    Auto,
    SingleThread,
    MultiThreadSafe,
    EventQueue
};

// 完整调用形如：LObject::ConnectionMode::Auto
// 实际代码中，通常省略枚举名调用，如：LObject::Auto

// 匿名枚举类型，禁止
enum
{
    One,
    Another
};
```

### 5.3 变量与常量命名

变量与常量命名采用小驼峰命名法，除首个单词外，每个单词首字母均大写。非特殊情况不允许出现数字，除用作类的数据成员前缀（`m_xxx`）等特殊情况，禁止额外出现下划线等特殊字符。

#### 5.3.1 普通变量

没有特殊规则。

```cpp
int foo;
int someVar;
int anotherVar;
```

#### 5.3.2 函数参数

没有特殊规则。

```cpp
void foo(int someParam, int anotherParam);
```

#### 5.3.3 类成员变量

在变量命名通用规则基础上，添加 `m_` 为前缀，表示*成员*变量。

对于静态成员，添加 `sm_` 为前缀，表示*静态*的*成员*变量。

> 在类内部使用自身的数据成员时，`m_` 足以描述，原则上无需使用 `this->m_xxx` 的形式访问。

> 一般来说，很少有需要使用静态成员变量的时候。一般使用场景更普遍的是静态常量。参见后文静态常量命名规则。

```cpp
class MyClass
{
private:
    int m_someMember = 0;
    static int sm_staticMember;
};

int MyClass::sm_staticMember = 0;
```

#### 5.3.4 结构体成员变量

考虑到结构体的成员默认为公有，结构体的成员不需要添加 `m_` 前缀，以保持代码简洁。

```cpp
struct MyStruct
{
    int foo;
    int bar;
}

MyStruct s;
s.foo = 12;
s.bar = 34;
```

#### 5.3.5 指针变量

在上述规则基础上，以小写字母 `p` 开头，后续每个单词均大写开头（相当于小驼峰规则中第一个单词变成 `p`）。另外要求 `*` 符号挨着后面的变量名，和前面的类型名以空格隔开。

```cpp
int *pFoo;

void bar(int *pSomeParam);

class MyClass
{
private:
    int *m_pSomePointer = nullptr;
};
```

> 特殊情况下，对于单字母指针参数，例如在事件处理函数中的 `LEvent *e`，可以变通，不严格遵循此规则。

#### 5.3.6 引用

没有特殊规则。要求 `&` 符号挨着后面的变量名，和前面的类型名以空格隔开。

```cpp
int foo;
int &bar;
```

> **Lark5 信号：**
> 
> 信号为特殊的类成员对象，为方便外部进行信号槽连接等操作，一般允许声明为 `public`。若确定信号只能由类内部连接，可以考虑声明为 `private` 或 `protected`。
> 
> 信号的命名遵循普通变量的命名规则，并以 `Signal` 作为后缀。注意，信号命名通常使用动词原型，无需使用过去时形式。
> 
> ```cpp
> // 文件：lbutton.h
> 
> class LButton : public LComponent
> {
> public:
>     // ...
>     LSignal<> clickSignal;
>     // ...
> };
> ```

#### 5.3.8 普通常量

没有特殊规则。

```cpp
const int foo;
```

#### 5.3.9 静态常量

静态常量一般用于依附于某个类的常量值。在类定义的时候就确定不变。使用上类似于枚举类型值，因此采用和枚举类型命名相同的方式采用大驼峰命名。例如：

```cpp
// 定义
class LGlyphIcon
{
public:
    static const int IconWidth;
    static const int IconHeight;
}

// 初始化
const int LGlyphIcon::IconWidth = 24;
const int LGlyphIcon::IconHeight = 24;

// 使用
int foo = LGlyphIcon::IconWidth * LGlyphIcon::IconHeight;
```

### 5.4 函数命名

常规函数命名和变量相同，使用小驼峰命名即可。

```cpp
int someFunc();
```

#### 5.4.1 getter/setter 函数

Getter 函数定义为简单返回对象的某个成员（可能存在一些处理）的公有函数。对于 getter 函数的命名，原则上以被获取的属性使用相同的词汇组合，若无特殊情况通常定义为内联，且以 `const` 修饰（不需要以 `get` 作为前缀）。

和 getter 相对应的，setter 函数用于简单设置某个成员的值。setter 函数需以 `set` 为前缀，通常类型为 `void`，参数名与被设置的属性使用相同的词汇组合，若无特殊情况通常定义为内联。

```cpp
class MyClass
{
public：
    int someProp() const { return m_someProp; }
    int anotherProp() const { return m_anotherProp; }
    void setSomeProp(int someProp) { m_someProp = someProp; }
    void setAnotherProp(int anotherProp) { m_anotherProp = anotherProp; }
private:
    int m_someProp = 0;
    int m_anotherProp = 0;
};
```

> **Lark5 事件处理函数：**
> 
> 事件处理方法一般只允许声明为 `protected`，一般为 `void` 类型并以 `virtual` 修饰，若覆盖了父类同名方法需声明 `override`，方便子类继承，同时阻止外部调用（参见前文“重写父类方法规范”章节）。
> 
> 事件处理方法遵循常规函数的命名规则，并以 `handle` 作为首个单词，`Event` 作为末尾单词。
> 
> ```cpp
> // 文件：lbutton.h
> 
> class LButton : public LComponent
> {
> protected:
>     // ...
>     virtual void handleMouseDownEvent(LMouseEvent *e) override;
>     virtual void handleMouseUpEvent(LMouseEvent *e) override;
>     // ...
> };
> ```

### 5.5 宏命名

和命名空间同理，宏也是产品设计的一部分（类比 Qt 的各种宏）。在没有详设的情况下，不允许项目组成员自行定义任何功能宏，以免污染全局代码。

宏的命名为全大写字母，单词之间以下划线字符 `_` 分隔。

```cpp
#define SOME_MACRO
```

> **Lark5 宏：**
>
> 产品内部设计的宏规定以 `LARK_` 作为前缀：
>
> ```cpp
> #define LARK_FOO_BAR
> ```

## 6 编码风格

### 6.1 制表符 (Tab)

所有 tab 以 4 个空格代替，代码内不允许出现制表符（`\t`）。

> 参考：VS Code 中将 tab 字符替换为空格的方式：
>
> 1. 打开某文件，`Ctrl+F` 搜索；
> 2. 搜索内容填写 `\t`，并激活右侧最后一个图标`使用正则表达式 (Alt+R)`；
> 3. 替换内容填写四个空格；
> 4. 点击全部替换。

### 6.2 行内空格

行内代码需按照美观可读的原则加空格。

二元、三元运算符，在每个符号左右两侧都需要加空格：

```cpp
int three = one + two;

for (int i = 0; i < 10; i++)
{
    // ...
}

doSomeCheck() ? doThis() : orDoThis();
```

一元运算符，以及取成员（点 `.`、箭头 `->`）等特殊运算符，可以不加空格：

```cpp
// 点运算
int prop = obj.prop;

// 箭头运算
int prop = pObj->prop();
```

声明指针或引用变量时，`*` 和 `&` 符号和前面的类型名之间需要空格，和后面变量名之间无需空格：

```cpp
int *pSomePointer;       // 普通指针
int &someRef             // 普通引用
int *arrayOfPointer[]    // 指针数组
```

### 6.3 括号与换行

对于*封装代码段*的花括号，代码段的左花括号 `{` 一律换行。

代码段只有一个语句的场合，可以令左右花括号在同一行且不用换行，但要保证花括号周围都有合适的空格。

```cpp
// for 语句
for (...)
{
    ...
}

// 条件语句
if (...)
{
    ...
}
else
{
    ...
}

// 函数
function foo(...)
{
    ...
}

// 不用换行的场合
if (...) { ... }
```

对于赋值语句或表达式（如 Lambda 表达式）后的花括号，例如给结构体初始化，若不准备全部写在一行的话，左花括号 `{` 一律不换行。

Lambda 表达式的方括号 `[]` 和圆括号 `()` 之间不空格，但 `->` 符号左右两侧均需空格。

简单总结：赋值的时候，左花括号不换行，否则要换行。

```cpp
// 假设结构体如此定义
// 注意结构体的定义里面，花括号需换行
struct MyStruct
{
    int foo,
    double bar,
    LString baz
};

// 顺序赋值写法
struct MyStruct s1 = {
    42,
    3.14,
    "baz"
};

// 乱序赋值写法 1
struct MyStruct s2 = {
    .foo = 42,
    .bar = 3.14,
    .baz = "baz"
};

// 乱序赋值写法 2
struct MyStruct s3 = {
    foo: 42,
    bar: 3.14,
    baz: "baz"
};

// Lambda 表达式不换行写法
int inc = [](int a) -> int { return a + 1; }

// Lambda 表达式换行写法
int dec = [](int a) -> int {
    return a - 1;
}
```

> 结构体初始化时，尽量使用乱序写法以提高代码可读性。

## 7 其他规范

此处为无法归类的杂项规范。

- 源文件后缀名规定为 `.cpp`，不使用 `.cxx`、`.cc` 等后缀名；
- 任何情况下禁止使用 `using namespace std`；
- 逻辑运算符（如 `&&`、`||`、`!`）禁止使用替代关键词（`and`、`or`、`not`）；
- 每一行代码，原则上不超过 120 个字符；
- 代码文件必须使用 UTF-8 编码存储；
- 交付的代码中不允许出现 `assert` 语句。


# 二、单元测试编码规范

## 1 指导性规则

以下规则用于从较高的角度整体指导单元测试的编写。原则上所有的单元测试代码都必须严格遵守。

### 1.1 自动化原则

单元测试需要完全自动执行，无任何用户交互。单个单元测试执行时间不超过 2 秒。

### 1.2 简易性原则

单个测试项代码原则上不超过 30 行。过长的单元测试项需进行适当拆分。

### 1.3 最小引入原则

单元测试应限制外部资源的使用，尽量只引入被测的产品类，和确实需引入产品中其他的类。禁止在单元测试代码中引入不必要出现的其他产品类。

> 如果在编写测试代码中需要使用一些辅助类，可以考虑使用标准库。例如若需使用动态列表，可考虑 `std::list`，而非使用产品类 `LList`。

### 1.4 测试项独立

每一个测试项都应看作一个独立运行的小程序。测试项之间原则上不应存在任何代码和数据上的耦合。

若确实需要进行资源复用以降低测试代码复杂度，必须严格使用测试框架提供的工具（如 GoogleTest 提供的 [Fixture 机制](https://google.github.io/googletest/primer.html#same-data-multiple-tests)）以及产品主管允许使用的封装类（如图形界面自动化测试中所使用的 `LTimedWindow`）。禁止自行在单元测试代码中进行任何抽象与封装。

每个测试项负责清理自己申请的内存，避免内存泄露。

### 1.5 不重复测试

单个产品类的每个功能接口，原则上不进行重复测试以尽量控制测试项总数。

注意本条原则不是指对同一个接口的不同参数输入测试。

### 1.6 确定性结果

单元测试需要有确定的结果，便于通过断言验证。

### 1.7 可读性与注释

单元测试代码同样需要遵循 [C++ 代码编写规范](/研发体系/研发流程/lark5-code-style-cpp)。单元测试代码的编写者有责任保证代码的可读性并添加合适的注释。

## 2 具体规范

> 本章节内容适用于 Lark5 产品线项目。
> {.is-info}

LarkSDK 以 [GoogleTest](https://google.github.io/googletest) 作为单元测试框架。单元测试的编写具体须遵循如下规范。

### 2.1 文件与目录结构

LarkSDK 的单元测试相关代码均存放在代码库 `test` 目录下：

```
test
├── lark5-core
│   ├── CMakeLists.txt
│   ├── lapplication_test.cpp
│   ├── lapplog_test.cpp
│   └── ...
├── lark5-gui
│   ├── CMakeLists.txt
│   ├── lbutton_test.cpp
│   ├── lcomponent_test.cpp
│   └── ...
├── lark5-util
│   ├── CMakeLists.txt
│   ├── lchar_test.cpp
│   ├── lfile_test.cpp
│   └── ...
├── CMakeLists.txt
└── README.md
```

子目录 `lark5-core`、`lark5-gui`、`lark5-util` 和源码目录（`src`）中的子目录对应，分别存放 LarkSDK 的三个子模块的单元测试代码。相应子模块的单元测试代码需严格放在对应目录下。

子模块目录中，一个 cpp 文件对应一个产品类的测试代码。即一个 cpp 文件仅负责*一个*产品类的单元测试。

文件命名：`lsomeclass_test.cpp`。其中 `lsomeclass` 为类名 `LSomeClass` 的全小写形式。

### 2.2 命名规范

#### 2.2.1 测试集

测试集命名为被测产品类名 + `Test` 后缀。一个单元测试源码文件，原则上只允许包含同一个测试集的代码。假设 `LSomeClass` 为待测类，测试集命名为：`LSomeClassTest`。

> 如果使用 Test Fixture 方式，`LSomeClassTest` 恰好作为 Fixture Class 类名使用。

#### 2.2.2 测试项

测试项命名遵循大驼峰命名法简要描述测试项内容，例如可以描述测试了被测产品类的哪个接口。同样要求以单词 `Test` 作为后缀。例如：

```cpp
// 假设待测类为 LChar

TEST(LCharTest, ConstructorTest)
TEST(LCharTest, ToUnicodeTest)
TEST(LCharTest, NullCheckTest)
```

> **注**：因 GoogleTest 不允许在测试项和测试集名称中包含下划线字符，故弃用原先使用的大写字母配合下划线 `XXX_XX_XXX` 的命名方式。关于这一点请参考[官方说明](https://google.github.io/googletest/faq.html#why-should-test-suite-names-and-test-names-not-contain-underscore)。

### 2.3 单元测试注释规范

单元测试代码沿用 Doxygen 的注释风格，对每一个测试项添加注释。至少要包含 `@test` 字段。`@details` 字段可选。注意末尾需要添加句号。例如：

```cpp
/**
 * @test 构造函数测试。
 */
TEST(LCharTest, ConstructorTest)
{
    ...
}

/**
 * @test 测试转换为 Unicode。
 * 测试获得 LChar 对应字符的 Unicode 代码。（根据 Doxygen 规则，此处不需显式写明 @details）
 */
TEST(LCharTest, ToUnicodeTest)
{
    ...
}
```

### 2.4 断言规范

*（待补充）*


# 三、注释规范

## 1 Lark5 注释规范

> 本章节内容适用于 Lark5 产品线项目。
> {.is-info}

> Lark5 产品线所有项目遵循 Doxygen 注释规范。

本代码规范用于指导 Doxygen 风格的注释编写，以保证自动生成文档的可读性。

> 若使用 VS Code，可考虑自行搜索安装 [Doxygen Documentation Generator 插件](https://marketplace.visualstudio.com/items?itemName=cschlosser.doxdocgen)。

注释分为文件注释和接口注释两部分:

- 头部注释：是每个文件头部的注释，以 `@file` 描述一个文件。从第一行开始，前面不允许有任何额外的字符包括空行；
- 接口注释：是针对文件内接口、成员变量等内容的注释，紧接在被注释内容前面，中间除正常换行外不得有任何其他字符。

每一个 `.h` 头文件和 `.cpp` 源文件都必须包括文件注释。头文件必须包含所有的接口注释（即接口注释写在头文件而不写在源文件内）。上述两种注释称为标准注释。按照 Doxygen 要求，标准注释一般使用 `/** ... */`（多行）或 `///<`（行内）的形式。

其他注释（不需要生成文档，不受 Doxygen 处理的注释）可在不影响可阅读性的前提下自由书写在任何地方。其他注释一般使用 `// ...` 的形式。

原则上，注释以**中文**撰写。

### 1.2 头文件

#### 1.2.1 头部注释

对于头文件的文件头部注释，模板如下：

```cpp
/**
 * @noop LarkSDK Source Code
 * @noop Copyright (C) 2014-2022 Sinux
 * @noop http://www.sinux.com.cn
 * 
 * @file <文件名>
 * @brief <文件简介>
 * @ingroup <所属模块，填写 Core/GUI/Util>
 * @author <作者中文名>
 * @author <可以添加更多作者名>
 */
 
 #ifndef _XXXXX_H_
 #define _XXXXX_H_
 
 // ...
 
 #endif
```

> 使用 `@noop` 可让 Doxygen 忽略该行注释。

#### 1.2.2 接口注释

接口注释至少需要正确填写简介 `@brief`、参数列表 `@param`（若有）、返回值 `@return`（若有）。其他 Doxygen 指令可以按需添加。

若需要编写的注释内容较长，可以把内容移入 `@details` 中，紧跟在 `@brief` 后。

多个接口之间要求以**正确的空行**分隔。范例如下：

```cpp
// 文件：lapplication.h

/**
 * @brief 返回可执行文件所在目录路径，不包含最右的 \a / 字符。
 * @return LString 所在的目录路径
 * @note 若位于根目录，则返回单独的 \a / 字符。
 */
static LString execDirPath();

/**
 * @brief 获取配置值。
 * @details 若配置值不存在，则返回空字符串。
 * @param configKey 配置的键
 * @return LString 配置的值
 */
static LString configValue(const LString &configKey);

/**
 * @brief 临时使用，判断文件是否存在。实现 LFile 后丢弃。
 * @param filePath 文件完整路径
 * @return true 文件存在
 * @return false 文件不存在
 */
static bool fileExists(const LString &filePath);
```

注意在编写接口注释时注意一个用语上的原则：接口注释和是“对外”的，和对外接口一样，因此理论上是不可以出现“对内”的内容的，如私有接口/成员的名字，不应该出现在对外的接口注释中。另外尽量也不要出现具体的接口名、成员名、类名等。如果涉及到，尽量用描述性的语言代替或略过。

比如下面的注释句就是不太合适的：

> `用于将 LDrawContext 对象的 m_pCurrentBitmap 平移至窗口的指定位置显示。`

可考虑修改为：

> `用于将绘图缓冲区里的内容绘制到窗口的指定位置。`

### 1.2 源文件

#### 1.2.1 头部注释

源文件的文件头注释模板如下（与头文件相比，可以省略文件简介 `@brief`）：

```cpp
/**
 * @noop LarkSDK Source Code
 * @noop Copyright (C) 2014-2022 Sinux
 * @noop http://www.sinux.com.cn
 *
 * @file <文件名>
 * @ingroup <所属模块，填写 Core/GUI/Util>
 * @author <作者中文名>
 * @author <可以添加更多作者名>
 */
 ```

> 接口注释不需要写在源文件内。

### 1.3 其他规范

此处为无法归类的杂项规范。

- Doxygen 命令关键字，段落使用 `@` 形式，行内使用 `\` 形式。如：

```cpp
/**
 * ...
 * @brief 以字典序比较字符串 \a s1 和 \a s2 的大小。 
 * ...
 */
```

- 通常情况下，简介 `@brief` 和描述 `@details` 是完整的句子，原则上需要以句号（。）结尾。但返回值说明 `@return`、参数说明 `@param` 等可以用简单词组描述，不以句号结尾。

## 2 Lark4 注释规范

> 本章节内容适用于 Lark4 产品线项目。
> {.is-info}

> Lark4 产品线所有项目注释遵循 QDoc 规范。

### 2.1 类添加注释的方法

在需要生成帮助文档的类前输入注释模板

```cpp
/*!
 * \class <类的名称>
 * \brief <类的简要描述>
 * \inmodule <类所属的模块>
 */
```

其中 `<module>` 为类属于的模块，此值与模块中的 `<link modules key>` 一致便可依赖到所属模块的页面。
例如：

```cpp
#include "qdocdemoclass.h"

/*!
* \class QDocDemoClass
* \brief QDocDemoClass 是 QDocDemo 的 ModelOne 模块的一个 Demo 类
* \inmodule ModelOne
*/
QDocDemoClass::QDocDemoClass() {}
```

这样便可以生成一个以 `QDocDemoClass` 小写为文件名的独立 html 页面，即 `qdocdemoclass.html`。

> **注**：请确保其所属的模块即 `ModelOne` 的 `index.qdoc` 文件存在，因为生成的 `qdocdemoclass.html` 需要依赖在其所属模块的index页面。

在 QtCreator 中，在类或函数前输入 `/*!` 后回车，可以自动生成注释模板。生成效果如下:

```cpp
// 文件：qdocdemoclass.h

/*!
* \brief The QDocDemoClass class
*/
class QDocDemoClass
{
public:
    QDocDemoClass();
    int add(int a, int b);
};
```

```cpp
// 文件：qdocdemoclass.cpp

/*!
* \brief 这是一个加法运算的函数
* \param a 加数a
* \param b 加数b
* \return 返回a+b的值
*/
int QDocDemoClass::add(int a, int b)
{
    return a + b;
}
```

 > **提示**：QDoc 不会识别头文件 `.h` 中的的注释命令，所以类注释只能写在源文件 `.cpp` 中。QDoc 生成帮助文档所需的类注释模板与上文提到的使用 “`/*!` + 回车键”自动生成注释模板并不完全匹配，所以需要手动编辑类注释模板。

在不需要生成帮助文档的类或函数前，输入 `/**` 后回车，自动生成注释模板，例如:

  ```cpp
  // 文件： qdocdemoclass.h
  
  /**
   * @brief The QDocDemoClass class 是 QDocDemo 的 ModelOne 模块的一个 Demo 类
   */
  class QDocDemoClass
  {
  public:
      QDocDemoClass();
      int add(int a,int b);
  };
  ```
  
  ```cpp
  // 文件：qdocdemoclass.cpp
  
  /**
   * @brief QDocDemoClass::add
   * @param a
   * @param b
   * @return 
   */
  int QDocDemoClass::add(int a, int b)
  {
      return a + b;
  }
  ```
 > **提示**：QDoc 会识别 `/*!` 开头的注释命令，因此如果用 `/*!` 开头的注释会自动为其生成帮助文档，并且如果未编写类注释他会报一些相关的警告。为了避免这些无关紧要的警告生成，建议使用 `/**` 自动生成的注释来注释自己的类和和函数。

### 2.2 QDoc 能识别的命令

#### 2.2.1 主题（Topic Command）

主题命令确定了文档的元素，例如 C++类（class），函数（function），类型（type），或者和所有元素无关的一段文字。

一个主题命令向 QDoc 指明源代码中哪一个语法元素要被归档，还有一些主题命令允许您创建没有绑定到任何源代码语法元素的文档页面。

当 QDoc 遍历 QDoc 注释时，它试图通过首先寻找到一个主题命令来把源代码元素连接到这个注释上。

如果没有主题命令，QDoc会连接紧跟在该注释之后的源代码元素。如果不满足上述条件,并且注释中没有其它元素来说明这段注释与源代码无关， 那么该段注释就会被丢弃。

相关的语法元素的名字通常是主题命令唯一的参数字段， 这里要使用完整的名称。有时可以有第二个参数。例如 `\page`。

`\fn` 也是一个特例，它需要加上完整的函数信息，因此它可能会有多个字段，但是是一个参数。

如果注释中没有 主题命令，QDoc 会把这段注释与紧跟其后的代码联系起来。

主题命令可以出现在注释中的任何位置，但必须独立一行。把主题命令放在第一行是一种很好的做法。

如果参数字段跨越几行，确保每一行（除了最后一个）使用一个反斜杠结束。

此外，QDoc 计数括号，这意味着如果它遇到一个 `(`，它会把 `)` 之前的所有内容作为其参数。

一些主题命令（详见QDoc文档）

- `\class`：会生成一个以参数名的小写为文件名的独立html页面
- `\externalpage`：给一个 URL 定义别名，这样在其它地方可以使用 `\l` 去引用

Miscellaneous： 

提供一些作用于文档生成和渲染的一些命令。

- `\group`：用于生成一个列举了所有类的独立的页面，这些类有 `\ingroup` 元素。

Relating Things：

- `\headerfile`：用于生成一个包含了一个头文件中的全局函数、类型、宏等的页面
- `\relates`：用于把一个全局的元素包含到一个头文件或者类中。 例如，声明了 `\class QChar`，再声明 `\relates AElem`，就会在 `QChar` 页面中看到 `AElem`
- `\overload`：说明函数是重载函数，参数是重载的函数参数
- `\reimp`：用于说明一个函数覆写了虚函数。

#### 2.2.2. 上下文（Context Command）

上下文命令向 QDoc 说明了 `.qdoc` 文件中某个元素和另外的元素怎么样联系，例如前后页链接，同一主题的页组，或者其它链接库，上下文命令还能够获取元素的那些在源代码文件中无法获取到的信息，例如，该元素是否线程安全，是否是一个重载或者覆写函数，是否已被废除，属于哪个 module 等。

#### 2.2.3. 标记（Makeup command）

标记命令告诉 QDoc 文档中的文字和图片如何渲染，以及文档的大纲结构。


# 参考资料

- https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide
- https://geosoft.no/unittesting.html
- https://www.doxygen.nl/manual/docblocks.html
- http://cedar-renjun.github.io/2014/03/21/learn-doxygen-in-10-minutes/index.html