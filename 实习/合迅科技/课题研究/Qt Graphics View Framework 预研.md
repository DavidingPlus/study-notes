---
title: Qt Graphics View Framework 预研
categories:
  - 实习
  - 合迅科技
  - 课题研究
abbrlink: e2c22a62
date: 2024-07-19 15:10:00
updated: 2024-07-19 15:10:00
---

<meta name="referrer" content="no-referrer"/>

该架构涉及到的最主要的三个类是`QGraphicsScene`、`QGraphicsView`和`QGraphicsItem`。

# 整体流程

1. 绘制流程（`QGraphicsItem`->`QGraphicsScene`->`QGraphicsView`）

   `QGraphicsItem`当中保存了自身的“场景坐标”供`QGraphicsScene`进行管理。在绘制时，由`QGraphicsView`对象调用渲染方法，根据自身所设置的可视化相关属性，基于“视图坐标”确定将要绘制的`QGraphicsScene`当中有哪一部分“场景坐标”内的图元需要渲染，随后通过`QGraphicsScene`提供的方法，将属于这部分“场景坐标”内的图元(也就是`QGraphicsItem`)全部找出，并渲染这些图元到可视化`viewport`中。

<!-- more -->

2. 事件流转（`QGraphicsView`->`QGraphicsScene`->`QGraphicsItem`）

   由`QGraphicsView`绘制出的`viewport`是与用户直接交互的对象，用户发起的`UI`事件都由`QGraphicsView`首先接收，它在接收到事件以后对其中的部分参数进行适当的处理(如鼠标事件的坐标进行转换)，随后将事件转发给`QGraphicsScene`对象，由`QGraphicsScene`确定事件发送到哪个具体的图元(如鼠标事件发送到符合坐标位置的图元，键盘事件发送到当前焦点所在的图元)，图元在接收到事件以后作自行处理。

3. `QGraphicsScene`的查询图元的`BSP`树相关算法

   在实际的工程场景中，极有可能会出现一个`QGraphicsScene`管理非常多个`QGraphicsItem`，例如几百万个，当用户与`QGraphicsView`交互的时候，需要经过`QGraphicsScene`将本事件传递到某个或者某些具体的图元，如果挨个遍历每个图元，并且判断是否触发事件，那么耗费的代价就太大了。我们希望做到的效率是几百万条数据在几毫秒以内能够确定目标图元，因此需要上相应的算法。

4. 图元实现优先级以及顺序和事件实现优先级以及顺序

   `QGraphicsItem`本质上是一个抽象父类，为子类规定了各种需要覆写的方法，子类通过重写这些方法能够很好的被`QGraphicsScene`所管理，最终呈现出来。因此不仅支持官方提供的矩形、椭圆、文本框等，还能处理自定义类型的图元。

   在现阶段，先不考虑纯虚的抽象类，先把`QGraphicsItem`当成矩形框来做，先和`QGraphicsScene`把流程跑通，后续在扩展的时候也能非常快就完成工作。

   事件处理是一个大块。经讨论，事件处理的优先级是鼠标事件大于键盘事件大于拖拽事件。因此第一步实现的时候，先实现最基本的`handleMousePressEvent`，当这个流程通了以后，后续的所有操作都是一模一样的了。

5. 整体实现顺序

   先把`QGraphicsView`剔出来，`QGraphicsView`实际上只负责可视化的绘制，真正做处理的操作在`QGraphicsScene`这一层，因此管理类`QGraphicsScene`和图元类`QGraphicsItem`是第一步需要实现的。

   目前的工作计划经商讨以后，决定先调研`QGraphicsScene`和`QGraphicsItem`的源码，明确如何以最小的代价实现我们目前需要的功能，包括事件转发、绘制逻辑、坐标系统等，这样明确需求和设计以后再实现初版，搭好整体架构。

# 重要功能总结

## QGraphicsScene

`QGraphicsScene`类提供了一个管理大量`QGraphicsItem`的容器。

`QGraphicsScene`类只管理所有的图元，若想要可视化场景，需要配合`QGraphicsView`。

`QGraphicsItem`作为`QGraphicsScene`类的图元，自然有很多接口都是围绕着`QGraphicsItem`来的，例如：

1. 添加图元：除了最基本的`addItem()`，还有更多针对特殊类型图元的`add*()`函数

   ![image-20240719141156412](https://img-blog.csdnimg.cn/direct/264b9f96baca489092997c096d29ddcd.png)

2. 查找图元：在大规模数据的情况下保证在极短的时间内查询到对应的图元，`Qt`使用的是索引算法，使用的是`BSP`（二进制空间分区）树。能够在极短的时间内在极大的数据规模中确定某个图元的位置，这也是`QGraphicsScene`的最大优势之一。

3. 移除图元：`removeItem()`

4. 管理图元状态，处理图元选择和焦点处理等

   - `setSelectionArea()`：可以传递一个形状范围来选择图元项目
   - `clearSelection()`：清除当前选择
   - `selectedItems()`：获取被选择的图元的列表
   - `setFocusItem()、setFocus()`：设置图元获取焦点
   - `focusItem()`：获取当前焦点

5. 接受事件：用户通过`QGraphicsView`触发事件，并通过`QGraphicsScene`将事件转发到对应的图元：例如鼠标按下、移动、释放和双击事件，鼠标悬停（`LarkSDK`目前好像没有）、滚轮事件，键盘输入焦点和按键事件，拖拽事件等

   ![image-20240719141220363](https://img-blog.csdnimg.cn/direct/0e995aa6681b407586d96a4469b73fdf.png)

   `QGraphicsScene`另一个重要的功能就是转发`QGraphicsView`的事件，通过一系列操作，例如通过鼠标点击的坐标计算出到底是选中了哪个图元，键盘事件对应的哪些图元具有焦点等，能够将这些事件转发给对应的图元，最后进入真正的事件循环进行处理。

## QGraphicsView

`QGraphicsView`类真正提供可视化`QGraphicsScene`的内容的功能，它在一个可滚动的`viewport`之内将一个`QGraphicsScene`中的内容实现可视化。

`QGraphicsView`主要功能包括但不限于：

1. 设置可视化操作的属性：`QGraphicsView`中提供了大量可设置的属性用以指示在实现可视化操作时的各种具体事项，如`RenderHints`提供参数初始化用于绘制的`QPainter`，`Alignment`提供当前视图中所绘制的场景的对齐方式。

   ![image-20240719141236742](https://img-blog.csdnimg.cn/direct/51c31d08460f4036a69542dce5ea8a31.png)

2. 对场景（`Scene`）进行可视化与视觉效果调整：`QGraphicsView`对象的成员方法render对场景进行可视化的绘制呈现在`viewport`中，并提供了一系列方法对`viewport`整体的视觉效果进行调整，如`centerOn`方法将滚动`viewport`中的内容以确保场景坐标`pos`在视图居中，`fitInView`方法将缩放并滚动`viewport`中的内容使得场景内的矩形区域`rect`铺满当前`viewport`。

   ![image-20240719141250017](https://img-blog.csdnimg.cn/direct/f8ecde7795e64156a313492167cc20f9.png)

3. 管理“场景（`Scene`）坐标”与“视图（`View`）坐标”之间的数学关系，并提供方法对视图内容施行各种坐标变换。`QGraphicsScene`对象当中的各个图元有其在`QGraphicsScene`中的坐标即“场景坐标”，它们代表了各个图元在`QGraphicsScene`中的位置信息；而`QGraphicsView`对各个图元进行绘制以及调整变换时则是通过由自身管理的“视图坐标”，它们代表了各个要绘制的图形在`viewport`中的位置信息。`QGraphicsView`可以由用户设置“场景坐标”向“视图坐标”变换的方式，对`viewport`实现旋转、伸缩等坐标变换，同时由于`QGraphicsView`的绘图使用“视图坐标”，因此这个过程不会干扰图元自身的“场景坐标”。此外还提供`mapToScene`/`mapFromScene`方法供用户调用实现这两种坐标之间的数学换算。

4. 接受鼠标和键盘的事件，并通过处理传递给`QGraphicsScene`对象，进而通过索引算法转发给对应的图元。

## QGraphicsItem

`QGraphicsItem`是所有图元的基类，可以派生出各种典型的形状（例如矩形、椭圆、文本等）和自定义的形状。

`QGraphicsItem`主要功能包括但不限于：

1. 接受`QGraphicsScene`传递的事件：进行事件处理，例如鼠标按下、移动、释放和双击事件，鼠标悬停、滚轮事件，键盘输入焦点和按键事件，拖拽事件等。

   ![image-20240719141308564](https://img-blog.csdnimg.cn/direct/250915a7ad6d4b96b6cfcc3879323d1f.png)

2. 坐标系统

   每个`Item`都有自己的本地坐标系，一般以自身的中心为`(0, 0)`，自身的方向作为基准方向建立，多个`Item`的情况就如图。因此需要通过某些机制将不同`Item`的坐标联系在一起。

   为了统一方便的管理，引入`parent-child`的关系。每个对象的变换都依赖于其父对象的坐标。子对象的`pos()`接口返回的是其在父对象坐标系统中的坐标。子对象的坐标处理是首先通过父对象不断向上传递，最终得到一个真实的坐标。同理，父对象的坐标变换也会同理影响到子对象的真实位置（批量处理），但是注意子对象存储的坐标没有变化，这样就非常好维护了。

   ![image-20240719141330634](https://img-blog.csdnimg.cn/direct/a90c76dd86114480a02b8b24a737a570.png)

3. 坐标变换

   `QGraphicsItem`除了提供基本位置`pos()`以外，还支持坐标变换，例如`setRotation()`旋转，`setScale()`缩放等。

   同理，父对象的变换会影响子对象，例如父对象顺时针转`90`度，子对象会跟着一起转`90`度。

4. 提供分组功能（调研是否需要当前考虑）

   通过前面的坐标系统可以知道，每个`QGraphicsItem`都有一个父对象，也可以有一系列子对象，这是类似于对象树机制的在构造的时候确定的关系。 当然也可以手动分组。`QGraphicsItemGroup`是一个特殊的派生类，该类记录了一系列的图元为一个组，在该组的所有图元通过`Group`调用的时候移动、事件处理等都会进行批量处理。

5. 提供编写自定义图元的接口（后续考虑）

   创建一个`QGraphicsItem`的子类，然后覆写两个纯虚函数`boundingRect()`返回该图元项目所绘制区域的估计值，`paint()`实现实际绘图。

   `boundingRect()`返回的是一个`QRectF`类型，将该图元的外部估计边界定义为矩形，这个方法也可以为不同`Item`的范围做大致的估算，可以被其他的方法所调用，省去一些暴力查找的过程。当然对于真正的矩形`boundingRect()`可以返回精确的范围，对于其他的曲线或者不规则的形状只能做大致的范围。

6. 碰撞检测（后续考虑）

   通过`shape()`函数和`collidesWith()`这两个虚拟函数，可以支持碰撞检测。`shape()`函数返回一个局部的坐标`QPainterPath`。目前没有细节调研`QPainterPath`，只知道`QPainterPath`记录了绘图的路径，比如`2D`图形的形状是由某些直线、曲线等构成的，通过这个能够确定图形的形状。

   `QGraphicsItem`会根据默认的`shape()`函数自动处理碰撞检测，实现合理的效果，比如在碰撞区域应该如何进行绘制。如果用户想要定义自己的碰撞检测，可以通过`collidesWith()`实现。

7. 图元顺序（后续考虑）

   难免会发生两个图元的范围出现重叠的情况。合理处理顺序决定了鼠标点击的时候哪些场景会接受鼠标事件。一个比较合理的想法是子对象位于父对象的顶部，而同级对象之间按照定义的顺序进行堆叠。例如添加对象`A、B`，那么对象`B`位于`A`的顶部。这是比较符合自然逻辑的，`Qt`也是这样做的。

   `Qt`提供了一些可以更改项目的排序方式的接口。例如可以在一个图元项目上调用 `setZValue()`，以将其显式堆叠在其他同级项目之上或之下。项的默认`Z`值为`0`，具有相同`Z`值的项按插入顺序堆叠。还可以设置`ItemStacksBehindParent`标志以将子项堆叠在其父项之后。

# 2D BSP 树在 QGraphicsScene 中的应用

`BSP`树构造一个`n`维空间到凸子空间的分层细分（`a BSP tree is a heirarchical subdivisions of n dimensional space into convex subspaces`）。每个节点都有一个前叶子节点和后叶子节点。从根节点开始，所有后续插入都由当前节点的超平面划分。在二维空间中，超平面是一条线。 在`3`维空间中，超平面是一个平面。`BSP`树的最终目标是让超平面的分布情况满足“每个叶节点都在父节点超平面的前面或后面”（`The end goal of a BSP tree if for the hyperplanes of the leaf nodes to be trivially "behind" or "infront" of the parent hyperplane.`）。

`BSP`树对于与静态图像的显示进行实时交互非常有用。 在渲染静态场景之前，需要计算`BSP`树。 可以非常快地（线性时间）遍历`BSP`树，以去除隐藏的表面和投射阴影。 通过一些工作，可以修改`BSP`树以处理场景中的动态事件。

下面是在对象空间构建`BSP`树的过程：

1. 首先，确定要划分的世界区域以及其中包含的所有多边形。为了方便讨论，我们将使用一个二维世界。

2. 创建一个根节点`L`，该节点本身对应一个分区超平面(在二维世界中，分区超平面就是直线)；同时这个节点维护一个多边形列表，在节点刚刚创建时，多边形列表中保存着它对应的超平面所划分的目标区域(对 L 而言，它对应的直线所划分的目标区域就是整个二维世界)当中的所有多边形。

3. 使用`L`对世界进行分区，假设分为了两个区域`A`和`B`。在根节点上创建两个叶子节点分别对应`A`和`B`，并将世界中的所有多边形移动到`A`或`B`的多边形列表中。遵循以下规则：

   对于世界中的每个多边形：

   - 如果该多边形完全位于`A`区域，请将该多边形移动到`A`区域的节点列表中。
   - 如果该多边形完全位于`B`区域，请将该多边形移动到`B`区域的节点列表中。
   - 如果该多边形与`L`相交，则将其拆分为两个多边形，并将它们移动到`A`和`B`的相应多边形列表中。这种情况下，算法必须找到多边形与分割线`L`的交点，以确定多边形的哪一部分位于哪个区域。
   - 如果该多边形与`L`重合(也就是说，恰好是一条位于`L`上的线段)，请保持其仍位于节点`L`处的多边形列表中。

   在上述步骤完成后，节点`A`和`B`的多边形列表已经生成，但是`A`和`B`尚未完成创建分区超平面并划分的步骤，也就是说，对于当前的节点`A`，它的状态与步骤`1`中的“整个二维世界”一致(区域确定，包含的多边形确定)，只不过区域大小有差别，`B`同理。

4. 在`A`和`B`上继续划分其所对应的二维区域，并将上述算法递归地应用于`A`和`B`的多边形列表。

   ![image-20240719113430085](https://img-blog.csdnimg.cn/direct/ea0ea86b45634374ae95427485db4eff.png)

更多细节和代码实现请参考博客：[一些关于空间数据结构的简单研究与实现 | GilbertNewtonLewis](https://gilbertnewtonlewis.github.io/2024/06/21/SpaticalData/)

# 参考文档

1. [https://doc.qt.io/qt-6/graphicsview.html](https://doc.qt.io/qt-6/graphicsview.html)
2. [https://www.cnblogs.com/q735613050/p/13413866.html](https://www.cnblogs.com/q735613050/p/13413866.html)
3. [https://blog.csdn.net/keilert/category_12595359.html](https://blog.csdn.net/keilert/category_12595359.html)
4. [https://blog.csdn.net/weixin_43925768/article/details/129138298](https://blog.csdn.net/weixin_43925768/article/details/129138298)
5. [https://blog.csdn.net/ZJU_fish1996/article/details/52554620](https://blog.csdn.net/ZJU_fish1996/article/details/52554620)
6. [https://blog.csdn.net/packdge_black/article/details/114681992](https://blog.csdn.net/packdge_black/article/details/114681992)

