# 关于内部实现的说明

## I. 声明

1 **本程序没有经过gcc调试**。因此，关于任何使用gcc编译而出现的问题，恕本人无法解答（~~*不过gcc应该有和cl一样的一些编译选项吧*~~）。
2 **程序代码行数：279（除去了加入命令行参数的部分）**。基本没有任何功能上的问题。~~*（为什么要强调代码行数呢？展示谁封装得更好？那对不起，我宁愿当调（tiao）包侠；或者，我调（diao）了包（指那个几千行的curses），也比大多数人的代码量少（手动狗头））*~~

## II. 关于 Visual Studio IDE（版本2019） 的调试选项

  1) 为加快 .hpp 文件中 lambda 表达式的编译，建议 `多处理器编译`**`:是(/MP)`**。
  2) 更进一步地，建议 `优选大小或速度：`**`代码速度优先(/Ot)`**。
  3) 再进一步地，建议`启用内部函数：`**`是(/Oi)`**。
  4) 为保证程序代在编译期间的基本不变性，建议`优化：`**`已禁用(/Od)`**。
  5) .hpp文件需要“链接时生成”的操作；**必须设置**`链接时间代码生成：`**`使用链接时间代码生成(/LTCG)`**。
  6) 对于上述打开`/LTCG`的操作，**建议手动设置**`启用增量链接：`**`否(/INCREMENTAL:NO)`**。

### 以上均可在**调试->{$项目名称}调试属性->C/C++（->链接器）->所有选项**中修改

  1) 关闭`启用地址级调试`、`源代码不可用时显示反汇编`，以优化分步调试（F11）体验。
  
### 以上可在**工具->选项->调试->常规**中修改

## III. 关于程序中超出课程内容部分的简要介绍

1 模板与枚举（这个或许后面要讲，无所谓，放到这来吧）
  使用*非类型模板形参*（即非`class`或`typename`）时，需确保该形参为**编译期常量**，且为**整型**、**指针**或者**引用**。
  在本程序中，使用了模板为四个方向的操作做了分类。传入的`<num_dir dir>`要么是`iter_l`（即 -1），要么是`iter_r`（即 1）。由于要用到循环中，`iter`即表示“该参数有类似于迭代器一般的功效”。具体请参考程序中的两个函数（应为`compress`和`combine`）。
2 `std::function`
  可以简单地理解成是*函数指针*的替代品。其重载的（拷贝赋值）等号和小括号，可以**接受一个函数的引用**。另外，类型模板形参传入的应是**函数的类型和其形参类型列表**。不妨见下例：
  现有一函数：
  `int f(int a, int b) { return a + b; }`；
  而考虑其function之存储：
  `std::function<int(int, int)>fp = &f;`；
  抑或：
  `std::function<int(int, int)>fp(&f);`。
  而对于其调用，则十分简单地列举如下：
  `std::cout << fp(2, 3) << '\n';`
  另外，特别地，该`function`类，接受`std::bind`和lambda表达式所生成的函数。
3 rvalue、右值引用
  需要注意的是，所谓右值*rvalue*，并非随意的*right value*之缩写，而是*reference value*之缩写。reference，表示该量只可被被动地存储和访问，即**没有实际地址**。
  右值引用（C++中，其运算符为`&&`），相对于一般的引用（`&`），只接受右值的传入。对此做具象解释，不妨见下例：
  `int a = 1;`
  `int& b = a; // 引用`
  `int&& c = a; // 编译不通过，右值引用不接受左值绑定`
  既然不接受左值传入，其意义又何在？简单来说，其意义在于对语义的“窃取”和“移动”，相当于对变量做了*浅拷贝*。在本程序中，较为底层的操作，均使用了右值引用，这样在来回的传参时能节省大量内存空间和赋值所需的时间。
  然而，右值引用远不止于此。在C++11引入的多线程中，右值引用在**移动构造函数**和**完美转发**上大放异彩。值得参考。如果觉得多线程太远，那`algorithm`中的`swap`如何？
  而与右值引用相匹配的一重要功能，便是`std::move`。对于其最简单的理解，便是“**将一个左值强制转换成右值**”。有趣的是，该转换被应用在函数右值引用形参时，**改变外部参数的能力和左值引用相似**。
  不妨考虑下例：
  `void f(int&& a) { ++a; }`
  而在`main`中：
  `int a = 2;`
  `f(std::move(a));`
  `std::cout << a << '\n';`
  其结果应为3。
4 `std::bind`
 该功能较为简单的理解是，**暂存一个函数，留作日后使用**。其使用方法十分简单，首个形参传入为函数的引用，后面则是函数的参数列表。其生成的，可以简单地理解成是一个“已绑定了一些参数的函数”（实际上是一个特殊的_Binder类，然而**在被赋值时，可以隐式地转换为一个函数，或者一个function类**）。
 需要十分注意的，一是：**直接向`std::bind`中传入的参数，不论原函数要求的是否是该参数的引用，都会传入拷贝**。若想**直接传入引用，则用`std::ref`包裹参数***（程序中，为了判断是否能移动，故意用了std::bind的拷贝功能，复制了一份地图，省去了写重载函数的麻烦。~~尽管还是得一个一个列bind的模板函数就是了，还得包一个lambda来随时调map~~）*。
 二是：有些参数暂时不想传入时，可使用`std::placeholders`来替代。替代后，在之后**调用时传入对应placeholders位置顺序的参数**。**而这时，传入的便是引用**。
 对于这两个~~*蛋疼*~~的特性，C++11中多线程的核心：std::thread类也有。
 不妨见下二例：
<1> 直接使用`std::bind`和`std::placeholders`
首先考虑一函数：
`double f(int& a, double& b) { ++a,++b; return a + b; }`
对于`main`：
`int a = 2;`
`double b = 2.5;`
然后考虑将其`bind`：

1) 对于拷贝参数的尝试：
`auto fp = std::bind(&f, a, b);`
`fp();`
`std::cout << a << b << '\n';`
打印结果应为2和2.5。

2) 对于引用参数（`std::ref`）的尝试：
`auto fp = std::bind(&f, std::ref(a), std::ref(b));`
`fp();`
`std::cout << a << b << '\n';`
打印结果应为3和3.5。

3) 对于引用参数（`std::placeholders::_x`）的尝试：
`auto fp = std::bind(&f, std::placeholders::_1, std::placeholders::_2);`
`fp(a, b);`
`std::cout << a << b << '\n';`
打印结果应为3和3.5。

<2> 在类中，简单地使用`std::bind`：
现有一类`X`：
`class X {`
`int a = 2, b = 3;`
`int f(int a, int b) {return a + b;}`
`}`
现将其**在类中**绑定如下：
`...`
`public:`
`auto fp = std::bind(&X::f, this, a, b);`
注意需要多传一个this指针进去。
5 lambda 表达式
  初见lambda表达式，可将其简单地分为三部分理解：

  1) 首当其冲的中括号：
   这是一个**捕获外部变量**的中括号。若里面为`&`,则以**引用**形式捕获；若为`=`，则以**拷贝**形式捕获。另，紧随两个符号之后，可以指定需要捕获哪个变量。*如果以拷贝形式捕获，则该变量只能作为右值使用。*
  2) 紧随其后的小括号：
   **传入的形参列表**。
  3) 最后的大括号：
   **函数体**。
   ~~*其优秀的融入外部环境的能力大大节省了写长引用参数列表的痛。*~~
  
