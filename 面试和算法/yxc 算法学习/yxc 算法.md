# yxc 算法

- 太难了，已停更。。。 `-- 2023.10.08`

## 第一章 基础算法

### 1.快速排序(Problem)

**取数组当中的一个值，一般取第一个值，先做处理再进行递归**

![image-20230711194047843](https://image.davidingplus.cn/images/2025/02/02/image-20230711194047843.png)

**分治的思想**：取一个分界点，进行一个轮次的查找使得分界点左边的数均小于x，右边的数均大于x；然后对左边和右边的区间分别进行递归即可

具体而言：

![image-20230711204623490](https://image.davidingplus.cn/images/2025/02/02/image-20230711204623490.png)

![image-20230711204636198](https://image.davidingplus.cn/images/2025/02/02/image-20230711204636198.png)

![image-20230711204656505](https://image.davidingplus.cn/images/2025/02/02/image-20230711204656505.png)

**注意：取左侧作为标兵那么一定是右侧先移动，然后最后将右侧的位置和标兵进行交换!!!!**

#### 模板：

```c++
void swap(int& val1, int& val2) {
    int val = val1;
    val1 = val2;
    val2 = val;
}

// 升序排序
// 这个代码遇到大量的数据的时候还是会出问题，需要进一步修改
void quick_sort(int arr[], int left, int right) {
    // 递归出口
    if (left >= right)
        return;

    int base_value = arr[left];
    int i = left;  // 选择从left开始，可以避免 4 5 中不进入循环然后就交换的问题
    int j = right;

    // 左边找第一个比基准值大的，右边找第一个比基准值小的
    while (i != j) {
        // 顺序很重要，需要先移动右边的!!!!
        while (arr[j] >= base_value && i < j)
            --j;
        while (arr[i] <= base_value && i < j)
            ++i;
        // 要保证j大于i才能交换，否则是无效的
        if (j > i)
            swap(arr[i], arr[j]);
    }
    // 将基准值归位，上面循环弹出的条件是相同，所以两个值相同
    swap(arr[left], arr[j]);

    quick_sort(arr, left, j - 1);
    quick_sort(arr, j + 1, right);
}
```

### 2.归并排序

**取下标中间的位置为分界点，先做左右递归然后再合起来处理(归并)**

采用双指针算法进行合并

![image-20230713160124405](https://image.davidingplus.cn/images/2025/02/02/image-20230713160124405.png)

**两根指针分别指向两个数组起始和结束的位置，然后依次比较两个指针，如果升序，哪个小就把哪个放入后面的数组然后后移，然后再次进行比较，直到一方到达末尾，这个时候把另一个的数组填入即可**

**注意，由于归并排序是先进行递归，所以归并的两个数组都是有序的!!!**

#### 模板：

```c++
// 升序排序
void merge_sort(int array[], int left, int right) {
    // 递归出口
    if (left == right)
        return;

    int newarr[right - left + 1];
    int mid = (left + right) / 2;

    // 递归左右
    merge_sort(array, left, mid);
    merge_sort(array, mid + 1, right);

    // 合并 left 到 mid 下标的左半部分 和 mid+1 到 right 的右半部分
    int left_move = left;
    int right_move = mid + 1;
    int i = 0;
    while (left_move <= mid && right_move <= right) {
        if (array[left_move] <= array[right_move])
            newarr[i++] = array[left_move++];
        else
            newarr[i++] = array[right_move++];
    }
    // 然后处理剩余的部分
    if (left_move == mid + 1)  // 左边跑完了
        for (; right_move <= right; ++i, ++right_move)
            newarr[i] = array[right_move];
    else if (right_move == right + 1)
        for (; left_move <= mid; ++i, ++left_move)
            newarr[i] = array[left_move];

    // 然后copy数组即可
    for (i = 0; i < right - left + 1; ++i)
        array[left + i] = newarr[i];
}
```

例题

第k个数

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230715161200778.png" alt="image-20230715161200778" style="zoom:67%;" />

~~~cpp

~~~

### 3.整数二分

**有单调性一定可以二分，但是没有单调性有的时候有可能也可以用二分!!!**

**注意哪里求mid的时候需要 +1 ，哪里不要**

#### 两个模板：

![image-20230713165547136](https://image.davidingplus.cn/images/2025/02/02/image-20230713165547136.png)

#### 例题

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230713175850386.png" alt="image-20230713175850386" style="zoom:80%;" />

```c++
#include <iostream>
using namespace std;

// 升序序列
// 这个用这个数最小的下标
int _binary_search1(int array[], int left, int right, int target) {
    while (left != right) {
        int mid = (left + right) / 2;
        // 最下下标，不包括这个数，左边所有的都小于这个数，右边的全部大于等于他
        // 思路就是：target左边的所有数小于他，那么如果mid对应的数小于他，那么target必然落在右边并且取不到mid!!!
        if (array[mid] < target)
            left = mid + 1;
        else
            right = mid;
    }

    if (array[left] != target)
        return -1;
    return left;
}

// 这个用这个数最大的下标
int _binary_search2(int array[], int left, int right, int target) {
    while (left != right) {
        int mid = (left + right + 1) / 2;
        // 最下下标，不包括这个数，左边所有的都小于这个数，右边的全部大于等于他
        // 思路就是：target左边的所有数小于他，那么如果mid对应的数小于他，那么target必然落在右边并且取不到mid!!!
        if (array[mid] > target)
            right = mid - 1;
        else
            left = mid;
    }

    if (array[left] != target)
        return -1;
    return left;
}

int main() {
    int n, q;
    scanf("%d%d", &n, &q);

    // 先输入数组
    int array[n];
    for (int i = 0; i < n; ++i)
        cin >> array[i];

    // 再依次输入询问的数
    int target[q];
    for (int i = 0; i < q; ++i)
        cin >> target[i];

    // 循环输出
    for (int i = 0; i < q; ++i) {
        int Left = _binary_search1(array, 0, n - 1, target[i]);
        int Right;
        if (Left == -1)
            Right = -1;
        else
            Right = _binary_search2(array, 0, n - 1, target[i]);
        cout << Left << ' ' << Right << endl;
    }

    return 0;
}
```

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230713183605769.png" alt="image-20230713183605769" style="zoom: 67%;" />

```c++
#include <iostream>
using namespace std;

double triple(double num) {
    return num * num * num;
}

double get_cubic_root(double num) {
    // 分两种情况，如果num>=1，那么三次方根的范围是在 [ 1,num )
    // 如果 0<=num < 1，那么三次方根的范围是在 ( num,1 ]
    double absnum = num;
    if (num < 0)
        absnum = -num;

    // 这个东西只能求正数的值!!!遇到负数求相反数的
    double left = absnum <= 1 ? absnum : 1;
    double right = absnum > 1 ? absnum : 1;

    // 逻辑一样，只是初值不同
    while (right - left > 1e-8) {
        double mid = (left + right) / 2;
        if (absnum > triple(mid))
            left = mid;
        else
            right = mid;
    }

    return num >= 0 ? left : -left;
}

int main() {
    double num;
    cin >> num;

    printf("%6lf\n", get_cubic_root(num));

    return 0;
}
```

### 4.高精度

**A的位数是10的6次方(可以看出超级大了)**

![image-20230714155419310](https://image.davidingplus.cn/images/2025/02/02/image-20230714155419310.png)

##### 存储方法

![image-20230714155943611](https://image.davidingplus.cn/images/2025/02/02/image-20230714155943611.png)

比如有一个数123456789，我们把每一位放到数组当中，从低位开始往后放

**这样放的好处是：当出现进位的时候在数组后面加上新的位数更容易一些!!!**

##### 加法

思路：**存到数组中，从低位开始依次往上加，然后进位**，注意代码细节

模板

~~~cpp
vector<int> add(const vector<int>& A, const vector<int>& B) {
    vector<int> C;
    int num = 0;

    for (int i = 0; i < A.size() || i < B.size(); ++i) {
        // 这样就巧妙的把数都进去了，还不用分段考虑
        if (i < A.size())
            num += A[i];
        if (i < B.size())
            num += B[i];

        // 存入C
        C.push_back(num % 10);
        // 计算进位留给下一次
        num /= 10;
    }

    // 最后出来的进位不要忘了
    if (num)
        C.push_back(1);

    return C;
}
~~~

示例

~~~cpp
#include <iostream>
using namespace std;
#include <string>
#include <vector>

// C=A+B
vector<int> add(const vector<int>& A, const vector<int>& B) {
    vector<int> C;
    bool is_carry = false;
    int i = 0, num = 0;

    for (; i < A.size() || i < B.size(); ++i) {
        if (i < A.size() && i < B.size())  // 先处理公共位数
            num = A[i] + B[i];
        else  // 这是公共位数完了之后处理单个的
            num = A.size() > B.size() ? A[i] : B[i];

        // 先判断上一次的is_carry看是否有进位
        if (is_carry)
            ++num;

        // 判断完之后将num重置
        is_carry = false;
        if (num >= 10) {
            num -= 10;
            is_carry = true;
        }
        C.push_back(num);
    }

    // 最后出来的进位不要忘了
    if (is_carry)
        C.push_back(1);

    return C;
}

int main() {
    string a, b;
    cin >> a >> b;

    vector<int> A, B;  // a="123456"

    // 大整数存储 从低位开始存储
    for (int i = a.size() - 1; i >= 0; --i)
        A.push_back(a[i] - '0');  // A =[6,5,4,3,2,1]
    for (int i = b.size() - 1; i >= 0; --i)
        B.push_back(b[i] - '0');

    auto C = add(A, B);

    for (int i = C.size() - 1; i >= 0; --i)
        cout << C[i];
    cout << endl;

    return 0;
}
~~~

##### 减法

思路：**还是从低位开始往前走，注意借位的处理**

模板

~~~cpp
// 判断A是否大于B 1大于 -1小于 0等于
int cmp(const vector<int>& A, const vector<int>& B) {
    if (A.size() != B.size())
        return A.size() > B.size() ? 1 : -1;

    for (int i = A.size() - 1; i >= 0; --i)
        if (A[i] != B[i])
            return A[i] > B[i] ? 1 : -1;
    // 相同
    return 0;
}

// C=A-B
// 这个函数一定保证了A不小于B
vector<int> sub(const vector<int>& A, const vector<int>& B) {
    vector<int> C;
    int num = 0;
    for (int i = 0; i < A.size(); ++i) {
        num += A[i];
        if (i < B.size())
            num -= B[i];

        C.push_back((num + 10) % 10);  // 这把正负数两种情况综合在一起
        if (num < 0)
            num = -1;
        else
            num = 0;
    }

    // 消去前置0
    while (C.back() == 0)
        C.pop_back();

    return C;
}
~~~

~~~cpp
#include <iostream>
using namespace std;
#include <string>
#include <vector>

int judge_Bigger(const vector<int>& A, const vector<int>& B) {
    if (A.size() != B.size())
        return A.size() > B.size() ? 1 : -1;
    // size相同，从末尾开始往前走，第一个不同就可以判断了

    auto iterA = A.rbegin();
    auto iterB = B.rbegin();
    while ((iterA != A.rend()) && (*iterA == *iterB)) {
        ++iterA;  // 注意这里是rbegin!!!重载了++运算符!!!
        ++iterB;
    }

    if (iterA == A.rend())
        return 0;  // 返回-1表示相同

    return *iterA > *iterB ? 1 : -1;
}

// C=A-B
// 这里先保证A不小于B
vector<int> sub(const vector<int>& A, const vector<int>& B, int is_bigger) {
    // 先判断是否相同
    if (is_bigger == 0)
        return vector<int>{0};

    if (is_bigger == -1)
        return sub(B, A, 1);

    vector<int> C;
    int num = 0;

    for (int i = 0; i < A.size(); ++i) {
        // 先把数拿进来，注意num是包含了上一次的借位了的，看后面
        num += A[i];
        if (i < B.size())
            num -= B[i];

        // 存入数值
        if (num < 0) {  // 借位
            num += 10;
            C.push_back(num);
            num = -1;
        } else {
            C.push_back(num);
            num = 0;
        }
    }

    // 如果高位是0则pop掉
    while (C.back() == 0)
        C.pop_back();

    return C;
}

int main() {
    string a, b;
    cin >> a >> b;

    vector<int> A, B;  // a="123456"

    // 大整数存储 从低位开始存储
    for (int i = a.size() - 1; i >= 0; --i)
        A.push_back(a[i] - '0');  // A =[6,5,4,3,2,1]
    for (int i = b.size() - 1; i >= 0; --i)
        B.push_back(b[i] - '0');

    int is_bigger = judge_Bigger(A, B);

    // 小的情况下先输出一个-号
    if (is_bigger == -1)
        cout << '-';

    auto C = sub(A, B, is_bigger);

    for (int i = C.size() - 1; i >= 0; --i)
        cout << C[i];
    cout << endl;

    return 0;
}
~~~

##### 乘法

**思路：让b去乘以A的每一位，然后记录下每次的进位，然后把进位与上一级相加，该位的值就是这个值mod10**

**与一般的思路不同，这个是把位数小的数放在前面!!!(因为我们也只能让小的数和大的数的每一位相乘!!!)**

<img src="https://image.davidingplus.cn/images/2025/02/02/6bd3078974047cfc651144c0787f894.jpg" alt="6bd3078974047cfc651144c0787f894" style="zoom: 50%;" />

模板

~~~cpp
vector<int> mul(const vector<int>& A, const int& b) {
    // 如果这里 b == 0会出现前置0问题
    if (b == 0)
        return vector<int>{0};

    vector<int> C;

    // 思路：让b去乘以A的每一位，然后记录下每次的进位，然后把进位与上一级相加，该位的值就是这个值mod10
    // 与一般的思路不同，这个是把位数小的数放在前面!!!
    int num = 0;
    for (int i = 0; i < A.size(); ++i) {
        num += A[i] * b;
        C.push_back(num % 10);
        num /= 10;
    }

    // 保存首位
    C.push_back(num);

    // 消除前置0 例子 2*3 不加上面输出 06
    while (C.back() == 0)
        C.pop_back();

    return C;
}
~~~

我自己写的示例和模板实现差不多，略

##### 除法

思路：就按照一般的除法走就完了

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230714203250061.png" alt="image-20230714203250061" style="zoom:67%;" />

模板

```cpp
// 模板是为了适配加减法和乘法，从低位开始存储
vector<int> div(const vector<int>& A, const int& b, int& remainder) {
    vector<int> C;
    int num = 0;

    for (int i = A.size() - 1; i >= 0; --i) {
        remainder = remainder * 10 + A[i];
        C.push_back(remainder / b);
        remainder %= b;
    }
    reverse(C.begin(), C.end());

    // 去除前置0
    while (C.back() == 0 && C.size() > 1)
        C.pop_back();

    return C;
}
```

示例

~~~cpp
#include <deque>
#include <iostream>
#include <string>
using namespace std;

pair<deque<int>, int> div(const deque<int>& A, const int& b) {
    deque<int> C;
    int num = 0;

    // 思路：就按照一般除法的思路走就行了
    for (int i = 0; i < A.size(); ++i) {
        num += A[i];
        C.push_back(num / b);
        num = 10 * (num % b);
    }

    // 去除前置0 比如 1234除以11
    // 关于商为0，就两种情况，一是被除数是0，二是被除数小于除数，保证不把0处理完就好了
    // 这时的num就是除下的余数，经过最后一次还乘以了10，不管是0还是小于是一样的
    while (C.front() == 0 && C.size() > 1)
        C.pop_front();

    return make_pair(C, num / 10);
}

// a很长 b很短
int main() {
    string a;
    int b;

    cin >> a >> b;

    deque<int> A;
    // 出发是从高位开始除，所以顺着存储
    for (int i = 0; i < a.size(); ++i)
        A.push_back(a[i] - '0');

    auto Pair = div(A, b);
    auto C = Pair.first;          // 商的deque表示
    int remainder = Pair.second;  // 余数

    for (int i = 0; i < C.size(); ++i)
        cout << C[i];
    cout << endl;

    cout << remainder << endl;

    return 0;
}
~~~

### 5.前缀和和拆分

#### 前缀和

**前缀和定义：数组前多少个数的和**

**这里数组最好下标从1开始，并且定义S0=0；**

**比如计算 l 到 r 的和，公式就是 Sl - Sr-1**

**计算 1 到 n 的和，公式就是 Sn-S0，这样就做到了统一!!!(处理边界)**

![image-20230714211457962](https://image.davidingplus.cn/images/2025/02/02/image-20230714211457962.png)

前缀和例题

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230714213457166.png" alt="image-20230714213457166" style="zoom:67%;" />

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <vector>

vector<int> Prefix_Sum(const vector<int>& vec) {
    vector<int> ans;
    ans.push_back(0);  // 边界条件 S0=0

    for (int i = 1; i < vec.size(); ++i)
        ans.push_back(ans[i - 1] + vec[i]);

    return ans;
}

int main() {
    // 这段代码
    // 好处：提高cin读取速度
    // 坏处：不能再使用scanf了
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    vector<int> array;
    array.push_back(0);  // 将第一个数存为0

    // 输入数组
    for (int i = 0; i < n; ++i) {
        int num;
        cin >> num;
        array.push_back(num);
    }

    // 得到前缀和数组
    vector<int> Prefix_Sum_Arr = Prefix_Sum(array);

    // 开始询问
    for (int i = 0; i < m; ++i) {
        int start, end;
        cin >> start >> end;

        cout << Prefix_Sum_Arr[end] - Prefix_Sum_Arr[start - 1] << endl;
    }

    return 0;
}
~~~

##### 提高cin速度

~~~cpp
// 这段代码
// 好处：提高cin读取速度
// 坏处：不能再使用scanf了
ios::sync_with_stdio(false);
~~~

例题2

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230714214355261.png" alt="image-20230714214355261" style="zoom:67%;" />

动态递归的思路：(自己画个图就明白了)

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230714214717118.png" alt="image-20230714214717118" style="zoom: 50%;" />

~~~cpp
#include <iostream>
using namespace std;

const int N = 1010;
int n, m, q;
int matrix[N][N] = {0}, prefix_sum[N][N] = {0};
int x1, y1, x2, y2;

int main() {
    scanf("%d%d%d", &n, &m, &q);
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            scanf("%d", &matrix[i][j]);

    // 求前缀和(画个图就理解了)
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            prefix_sum[i][j] = matrix[i][j] + prefix_sum[i - 1][j] + prefix_sum[i][j - 1] - prefix_sum[i - 1][j - 1];

    // 求部分和
    for (int i = 0; i < q; ++i) {
        scanf("%d%d%d%d", &x1, &y1, &x2, &y2);
        cout << prefix_sum[x2][y2] - prefix_sum[x1 - 1][y2] - prefix_sum[x2][y1 - 1] + prefix_sum[x1 - 1][y1 - 1] << endl;
    }

    return 0;
}
~~~

#### 差分

b数组称为a数组的差分，a数组称为b数组的前缀和

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230714221348016.png" alt="image-20230714221348016" style="zoom:67%;" />

**差分的作用：可以用O(1)的时间复杂度将数组当中某个区间的所有数加上某个值**

如图：

**B是A的差分，现在把 bl 加c，因此 al 之后的所有的都加c**

**再把 br+1 减c，该行为会导致 ar 以后的所有都减去c**

**因此，l到r区间当中的前缀和全部加了c**

![image-20230714221736276](https://image.davidingplus.cn/images/2025/02/02/image-20230714221736276.png)

例题

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230714222044152.png" alt="image-20230714222044152" style="zoom:67%;" />

代码

```cpp
#include <iostream>
using namespace std;

const int N = 100010;

int n, m, l, r, c;
int arr[N] = {0}, differ_arr[N] = {0};  // 第二个是差分数组

// 在l到r闭区间当中每个数加上c的操作
void __operate(int l, int r, int c) {
    differ_arr[l] += c;
    differ_arr[r + 1] -= c;
}

int main() {
    scanf("%d%d", &n, &m);
    for (int i = 1; i <= n; ++i) {
        scanf("%d", &arr[i]);
        differ_arr[i] = arr[i] - arr[i - 1];
    }

    // 操作
    for (int i = 0; i < m; ++i) {
        scanf("%d%d%d", &l, &r, &c);
        __operate(l, r, c);
    }

    // 得到最终的结果再求一次前缀和即可
    for (int i = 1; i <= n; ++i) {
        arr[i] = arr[i - 1] + differ_arr[i];
        cout << arr[i] << ' ';
    }
    cout << endl;

    return 0;
}
```

二维差分

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230715153441758.png" alt="image-20230715153441758" style="zoom:67%;" />

代码

~~~cpp
#include <iostream>
using namespace std;

const int N = 1010;
int n, m, q;
int matrix[N][N] = {0}, differ_matrix[N][N] = {0};

// 自己画个图理解一下就明白了
void __operator(int x1, int y1, int x2, int y2, int c) {
    differ_matrix[x1][y1] += c;
    differ_matrix[x2 + 1][y1] -= c;
    differ_matrix[x1][y2 + 1] -= c;
    differ_matrix[x2 + 1][y2 + 1] += c;
}

int main() {
    scanf("%d%d%d", &n, &m, &q);
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            scanf("%d", &matrix[i][j]);

    // 求出最初的差分矩阵，把初始的矩阵一个一个插进去，然后更新差分矩阵
    // 修改原矩阵的某个子矩阵，可以用函数表示，然后现在把这个子矩阵修改为这个点的矩阵，也是一样的
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            __operator(i, j, i, j, matrix[i][j]);

    for (int i = 0; i < q; ++i) {
        int x1, y1, x2, y2, c;
        scanf("%d%d%d%d%d", &x1, &y1, &x2, &y2, &c);
        __operator(x1, y1, x2, y2, c);
    }

    // 最后求一次前缀和就是最终的结果
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            matrix[i][j] = matrix[i - 1][j] + matrix[i][j - 1] - matrix[i - 1][j - 1] + differ_matrix[i][j];

    // output
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j) {
            cout << matrix[i][j];
            if (j != m)
                cout << ' ';
            else if (j == m && i != n)
                cout << '\n';
        }

    return 0;
}
~~~

### 6.双指针算法

可以把O(n2)算法优化为O(n)

绝大部分的模板都是长这样的

![image-20230715171200318](https://image.davidingplus.cn/images/2025/02/02/image-20230715171200318.png)

一个简单的例子

给你一个字符串，里面有很多个单词，单词之间用一个空格隔开(只有一个)，打印出所有的单词

~~~cpp
// getline在c++中可以读取含空格的字符串，分界线为回车
// 第三个参数可以有，意思是分界符，默认为回车
// 第二个参数是输入的最大字节数
cin.getline(buf, sizeof(buf), '\n');
~~~

~~~cpp
// 给你一个字符串，里面有很多个单词，单词之间用一个空格隔开(只有一个)，打印出所有的单词
#include <iostream>
using namespace std;
#include <cstring>
#include <string>

int main() {
    char Words[1010];

    // getline在c++中可以读取含空格的字符串，分界线为回车
    // 第三个参数可以有，意思是分界符，默认为回车
    cin.getline(Words, sizeof(Words), '\n');
    int n = strlen(Words);

    // 每次从i开始，然后定义一个指针往后扫描，直到扫描出空格，他们之间就是一个单词，然后修改i指针就可以了
    for (int i = 0; i < n; ++i) {
        int j = i;
        while (j < n && Words[j] != ' ') ++j;

        // 本题的具体逻辑
        for (int k = i; k < j; ++k) cout << Words[k];
        cout << endl;

        i = j;
    }

    return 0;
}
~~~

#### 例题

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230715194453180.png" alt="image-20230715194453180" />

双指针的题一般都可以从暴力入手，然后再去着手优化

这个题我的思路和他的就形成了对比，他的时间复杂度更低

~~~cpp
//我的
#include <iostream>
using namespace std;

const int N = 100010;
int n;
int arr[N] = {0};

int __max(const int &val1, const int &val2) {
    return val1 >= val2 ? val1 : val2;
}

int main() {
    scanf("%d", &n);
    for (int i = 0; i < n; ++i)
        scanf("%d", &arr[i]);

    int res = 1;  // 0的位置必然为1

    // 用res数组来表示以位置为右端点的最长不包含重复数序列的长度
    // 每次的左端点的值看是不是需要往右移动，因为他已经是最左边的值了，再往左走就矛盾了
    for (int i = 1, j = 0; i < n; ++i) {
        int pre_j = j;
        bool flag = false;
        // 考虑 6 9 5 10 1 2 3 加入9
        // 就算重复也只可能重复一次，就将j放到他的后面就可以了
        while (j < i) {
            if (arr[j] == arr[i]) {
                flag = true;
                ++j;  // 新的序列的j位置
                break;
            }
            ++j;
        }

        // 出来之后判断j是否和i重合，可以用flag标记
        if (!flag)
            j = pre_j;

        res = __max(res, i - j + 1);
    }

    cout << res << endl;

    return 0;
}
~~~

他的

~~~cpp
#include <iostream>
using namespace std;

const int N = 100010;
int n;
int arr[N] = {0}, s[N] = {0};

int __max(const int &val1, const int &val2) {
    return val1 >= val2 ? val1 : val2;
}

int main() {
    cin >> n;
    for (int i = 0; i < n; i++)
        cin >> arr[i];

    int res = 1;  // 0的位置必然为1

    // 用res数组来表示以位置为右端点的最长不包含重复数序列的长度
    // 每次的左端点的值看是不是需要往右移动，因为他已经是最左边的值了，再往左走就矛盾了

    // 从1开始，需要计入arr[0]在s数组中的次数
    ++s[arr[0]];

    for (int i = 1, j = 0; i < n; ++i) {
        // 答案的思路
        // 用一个s[]数组来存储j到i之间每一个数出现的次数，出现多了就把j向后移动直到为1次
        ++s[arr[i]];  // 现在只有可能这个东西不为1
        while (s[arr[i]] > 1) {
            --s[arr[j]];
            ++j;
        }

        res = __max(res, i - j + 1);
    }

    cout << res << endl;

    return 0;
}
~~~

### 7.位运算

#### 最常用的两种操作

1. n的二进制表示中第k位是几？(个位是第0位)

思路：先把第k位移到最后一位 n >> k ;

​		   看下个位是几 x & 1

~~~cpp
n >> k && 1
~~~

2. lowbit(x)：返回x的最后一位1

**比如 x = 1010，返回 lowbit(x) = 10**

**计算方法：lowbit(x) = x & -x**

**在计算机当中，对数负数的表示用的是补码，补码就是取反加1，即 -x = ~x + 1**

**然后我们举个例子：1010 ... 1000...000**

**现在我先给他取反，0101 ... 0111...111，我们发现加1之后最后一位1回来了，然后后面的所有0也回来了，然后前面是取反的，所以想与刚好就是最后一位1的位置**

#### 例题

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230715205834408.png" alt="image-20230715205834408" style="zoom:67%;" />

这个题就是典型的lowbit操作的用法

~~~cpp
#include <iostream>
using namespace std;

const int N = 100010;
int n, num;

// lowbit操作的应用
int lowbit(int x) {
    return x & -x;
}

int main() {
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &num);

        // 每次减去这个数的最后一位1，直到变为0
        int res = 0;
        while (num) {
            num -= lowbit(num);
            ++res;
        }
        cout << res << ' ';
    }

    return 0;
}
~~~

### 8.离散化

#### 思想

值域非常大，我们把这些数映射到从0开始的有序自然数，这个过程就叫离散化

![image-20230715211118777](https://image.davidingplus.cn/images/2025/02/02/image-20230715211118777.png)

两个前提：**第一是整数；第二是保序，也就是数组是有序的**

问题：

- **a数组中可能有重复元素 -> 去重**

~~~cpp
alls.erase(unique(alls.begin(),alls.end()),alls.end());
~~~

- **如何算出a[i]离散化之后的值是多少，也就是找a[i]在元素中的下标是多少？ -> 二分**

#### 例题(!!!)

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230715212020778.png" alt="image-20230715212020778" style="zoom:67%;" />

如果数的范围很小，那么可以用前缀和算法就可以很好的解决了

**但是现在虽然整个值域跨度很大，但是题目添加的次数不多，也就是绝大多数的坐标都是浪费的，所以可以用离散化进行映射，这里用离散化的方法**

思路就是把用到的坐标保序映射到自然数当中，要求前缀和，从1开始

这个题非常有难度，这里我自己的思路和老师的思路大致相同，但是我这里有一些特殊情况的判断。后面看能否做到统一

~~~cpp
#include <iostream>
using namespace std;
#include <algorithm>
#include <vector>

const int N = 300010;
int n, m;

vector<pair<int, int>> _data;  // 用来记录最开始的x和c的情况
vector<int> _map;              // 映射数组
int _map2[N];                  // 映射数组，存储映射后的下标和实际的数值的关系
int Presum[N];                 // 前缀和数组

// 找到第一个大于等于他的数
int __binary_search(const vector<int>& arr, int left, int right, int target, bool& ok) {
    if (left == right)
        return arr[left];

    while (left != right) {
        // 找从左到右第一个数，这个数左边均小于他，右边均不小于他
        int mid = left + right >> 1;
        if (arr[mid] < target)
            left = mid + 1;
        else
            right = mid;
    }

    // 如果找不到修改ok的值
    if (arr[left] != target)
        ok = false;

    return left;
}

int main() {
    scanf("%d%d", &n, &m);

    for (int i = 0; i < n; ++i) {
        int x, c;
        scanf("%d%d", &x, &c);
        _data.push_back(make_pair(x, c));
        _map.push_back(x);
    }

    // 将_map数组去重，排序，然后为了使用前缀和将第一位设置为0
    sort(_map.begin(), _map.end(), less<int>());               // 排序
    _map.erase(unique(_map.begin(), _map.end()), _map.end());  // 去重

    // 更改数组_map2，将映射坐标和实际值做连接
    for (auto val : _data) {
        // 由于_map数组没有加上前置0，所以这里需要加上一位，_map2里面是有前置0的
        bool ok = true;
        int index = 1 + __binary_search(_map, 0, _map.size() - 1, val.first, ok);
        _map2[index] += val.second;
    }

    // 求出前缀和
    for (int i = 1; i <= _map.size(); ++i)
        Presum[i] = Presum[i - 1] + _map2[i];

    // 询问
    for (int i = 0; i < m; ++i) {
        int l, r;
        scanf("%d%d", &l, &r);

        // 如果l比_map数组最大值还大，那么不用找了，为0
        // 如果r比_map数组最大值还大，那么修改r为最大值的点
        if (l > _map.back()) {
            cout << 0 << endl;
            continue;
        }
        if (r > _map.back())
            r = _map.back();

        // 这个坐标得到的是映射出来不小于这个点的下标
        // 但是我们区间应该这么看，如果找不到，左边的应该自动匹配到存在的右边的第一个，右边的应该自动移到左边的第一个

        // 比如     1 3 7
        // 映射 (0) 1 2 3
        // 现在求4和6之间的，都找不到，把4移动到7的位置，6移动到3的位置，然后由于左边的大于右边的，则区间长度为0

        //_l找到的是第一个比他大的._r找到的是第一个比他小的
        bool ok = true, flag_r = true;
        int _l = 1 + __binary_search(_map, 0, _map.size() - 1, l, ok);
        int _r = 1 + __binary_search(_map, 0, _map.size() - 1, r, flag_r);

        // 只有右边的映射可能需要左移一位，因为找不到
        if (!flag_r)
            --_r;

        if (_l > _r)
            cout << 0 << endl;
        else
            cout << Presum[_r] - Presum[_l - 1] << endl;
    }

    return 0;
}
~~~

### 9.区间合并

#### 思想

先把所有区间按照左端点的大小排序，然后从第一个区间开始，由于排了序，那么后面的区间的左端点只可能大于等于本区间，然后就可能出现三种情况如下：

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230719163223755.png" alt="image-20230719163223755" style="zoom:67%;" />

前面两个区间可以合并，然后更新为新的区间；第三种情况合并不了，由于经过了排序，那么第一个区间必然是最后合并区间的一个，那么将其记录下来，然后更新区间为后面的区间，直到所有的区间都判断完毕

![image-20230719163115693](https://image.davidingplus.cn/images/2025/02/02/image-20230719163115693.png)

例题：

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230719165718848.png" alt="image-20230719165718848" style="zoom:67%;" />

代码：

~~~cpp
#include <iostream>
using namespace std;
#include <algorithm>
#include <vector>

const int N = 100010;
int n;
vector<pair<int, int>> arr;  // 区间数组
vector<pair<int, int>> res;  // 结果数组

int __max_(const int& val1, const int& val2) {
    return val1 >= val2 ? val1 : val2;
}

int main() {
    cin >> n;
    for (int i = 0; i < n; ++i) {
        int l, r;
        scanf("%d%d", &l, &r);
        arr.push_back(make_pair(l, r));
    }

    // 先讲区间按照左端点进行排序
    sort(arr.begin(), arr.end(), [=](pair<int, int> val1, pair<int, int> val2) {
        return val1.first <= val2.first;
    });

    int start = arr[0].first, end = arr[0].second;
    // 排序之后进行区间合并
    for (int i = 1; i < arr.size(); ++i) {
        if (arr[i].first <= end)  // 如果是前两种情况 表示可以合并
            end = __max_(end, arr[i].second);
        else {  // 不可合并
            res.push_back(make_pair(start, end));
            start = arr[i].first;
            end = arr[i].second;
        }
    }

    // 将最后一个区间存进去
    // 我们的代码逻辑倒数第二个区间和最后一个区间不管是哪种情况最后一次都是走上面，最后一个区间没输出
    res.push_back(make_pair(start, end));

    cout << res.size() << endl;

    return 0;
}
~~~

## 第二章 数据结构

### 1.链表

**这里都是用数组来模拟链表**

数组模拟链表，也称为静态链表

为什么呢？

如果用指针来实现链表，会用到new操作，new操作在算法题当中是非常慢的!!!

所以可以用数组来模拟链表

#### 单链表

用e[N]和ne[N]来表示结点的值和结点的后继结点的下标，空结点下标定义为-1

![image-20230719170957832](https://image.davidingplus.cn/images/2025/02/02/image-20230719170957832.png)

##### 例题(!!!)

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230719171157485.png" alt="image-20230719171157485" style="zoom:67%;" />

直接看代码吧，都在代码里了(真的很巧妙)

**需要提醒的就是：数组存储的顺序是我们插入的顺序，但是链表实际上的结构并不一定是按照顺序来的，因为并不一定每一次都是在尾部插入**

~~~cpp
#include <iostream>
using namespace std;

const int N = 100010;
// 用数组模拟单链表
int head;   // 单链表的头指针，指向链表的头结点
int e[N];   // 下标为i的结点的值
int ne[N];  // 下标为i的结点的后继结点的序号
int idx;    // 下一个可用的结点的序号

// 注意，这个单链表在数组当中的序号不一定是按照序号来进行排序的，但是我们在插入的过程中早已经把结点的关系记录好了
// idx存储的是下一个可以用的结点的序号，是按照插入的顺序来的，链表实际的顺序大概率不是按照序号顺序来的，因为插入的位置不一定都是末尾

// 初始化
void __init__() {
    head = -1;
    idx = 0;
}

// 将x插头结点后面
void add_to_head(int x) {
    e[idx] = x;
    ne[idx] = head;
    head = idx;
    ++idx;
}

// 将x插到下标是k的点后面
void add_to_k(int k, int x) {
    e[idx] = x;
    ne[idx] = ne[k];
    ne[k] = idx;
    ++idx;
}

// 将下标是k的点的后面一个点删掉
// 对于删除操作，没有将结点回收，因为我们是在算法题里面模拟链表，为了保证效率，不用考虑其他的问题，哪个结点实际上还在数组当中，但是我们的ne[]关系将其删除了，看起来链表里就没有了这个东西
void remove(int k) {
    ne[k] = ne[ne[k]];
}

int M;

int main() {
    cin >> M;
    // 初始化链表
    __init__();

    for (int i = 0; i < M; ++i) {
        // 输入操作
        char op;
        int k, x;
        cin >> op;

        if (op == 'H') {
            cin >> x;
            add_to_head(x);
        } else if (op == 'D') {
            cin >> k;
            // 特殊判断，删除头结点
            if (k == 0)
                head = ne[head];

            // 删除第k个插入的数，下标是k-1，在现在的链表关系中顺序不一定是k-1
            remove(k - 1);
        } else if (op == 'I') {
            cin >> k >> x;
            add_to_k(k - 1, x);
        }
    }

    for (int i = head; i != -1; i = ne[i])
        cout << e[i] << ' ';
    cout << endl;

    return 0;
}
~~~

#### 双链表

##### 例题

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230719211659529.png" alt="image-20230719211659529"  />

**思路差不多，但是这里借用数组的0和1序号来代表head和tail，idx从2开始!!!**

~~~cpp
#include <iostream>
using namespace std;
#include <string>

const int N = 100010;
int e[N];  // 下标为i的结点的值
int l[N];  // 下标为i的结点的前驱节点
int r[N];  // 下标为i的结点的后继节点
int idx;   // 下一个可以用到的结点的序号

// 初始化
// 这里用数组的0号和1号当作左结点和右结点，当作边界，不算实际内容
void __init__() {
    r[0] = 1;
    l[1] = 0;
    idx = 2;
}

// 在下标为k位置的后面，插入值为x
void add_to_k_back(int k, int x) {
    e[idx] = x;

    r[idx] = r[k];
    l[idx] = k;
    r[k] = idx;
    l[r[idx]] = idx;

    ++idx;
}

// 在下标为k位置的前面，插入值为x、
void add_to_k_front(int k, int x) {
    add_to_k_back(l[k], x);
}

// 在头部插入一个值
void add_to_head(int x) {
    add_to_k_back(0, x);
}

// 在尾部插入一个值
void add_to_tail(int x) {
    add_to_k_front(1, x);
}

// 将下标为k的数删除，注意是删除下标为k，不是下一个
void remove_k(int k) {
    l[r[k]] = l[k];
    r[l[k]] = r[k];
}

int M;

int main() {
    cin >> M;

    // 初始化
    __init__();

    for (int i = 0; i < M; ++i) {
        string op;
        int k, x;

        cin >> op;
        if (op == "L") {
            cin >> x;
            add_to_head(x);
        } else if (op == "R") {
            cin >> x;
            add_to_tail(x);
        } else if (op == "D") {
            cin >> k;
            remove_k(k + 1);
        } else if (op == "IL") {
            cin >> k >> x;
            add_to_k_front(k + 1, x);
        } else if (op == "IR") {
            cin >> k >> x;
            add_to_k_back(k + 1, x);
        }
    }

    for (int i = r[0]; i != 1; i = r[i])
        cout << e[i] << ' ';
    cout << endl;

    return 0;
}
~~~

### 2.栈和队列

#### 例题

stack

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230719212205557.png" alt="image-20230719212205557" style="zoom: 80%;" />

~~~cpp
#include <iostream>
using namespace std;
#include <string>

const int N = 100010;

class _Stack {
public:
    _Stack() {
        __init__();
    }

    // 初始化
    void __init__() {
        top = -1;  // 数组下标从0开始
    }

    // 判断是否为空
    bool _is_empty() {
        return top == -1;
    }

    // 入栈
    void push(int x) {
        _stack[++top] = x;
    }

    // 出栈
    void pop() {
        --top;
    }

    // 得到栈顶元素的值
    int gettop() {
        return _stack[top];
    }

private:
    // 用数组模拟栈
    int _stack[N], top;
};

int main() {
    int M;
    cin >> M;

    _Stack s;

    while (M--) {
        string op;
        int x;

        cin >> op;
        if (op == "push") {
            cin >> x;
            s.push(x);
        } else if (op == "pop") {
            s.pop();
        } else if (op == "query") {
            cout << s.gettop() << endl;
        } else if (op == "empty") {
            cout << (s._is_empty() ? "YES" : "NO") << endl;
        }
    }

    return 0;
}
~~~

queue

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230719213226149.png" alt="image-20230719213226149" style="zoom:80%;" />

~~~cpp
#include <iostream>
using namespace std;

const int N = 100010;

class _Queue {
public:
    _Queue() {
        __init__();
    }

    void __init__() {
        // head,tail占据数组位置但是不是队列的值，意思是队列是从1号下标开始的
        head = 0;
        tail = 1;
        // 这样设置初始化可以保证在队尾插入，队头弹出的前提下head和tail指针都在往后移动
    }

    // 插入，队尾插入
    void push(int x) {
        _queue[tail++] = x;
    }

    // 弹出，队头演出
    void pop() {
        ++head;
    }

    // 得到队尾元素的值
    int gettail() {
        return _queue[head + 1];
    }

    // 判断是否为空
    bool _is_empty() {
        return tail - head == 1;
    }

private:
    int _queue[N], head, tail;  // 规定队尾插入，队头弹出
};

int main() {
    int M;
    cin >> M;

    _Queue q;

    while (M--) {
        string op;
        int x;

        cin >> op;
        if (op == "push") {
            cin >> x;
            q.push(x);
        } else if (op == "pop") {
            q.pop();
        } else if (op == "query") {
            cout << q.gettail() << endl;
        } else if (op == "empty") {
            cout << (q._is_empty() ? "YES" : "NO") << endl;
        }
    }

    return 0;
}
~~~

### 3.单调栈

**典型题型：给定一个数列，求每一个数左边离他最近且比他小或者大的数的下标!!!**

这是单调栈用的比较多的一种题型，基本就是这么考的

#### 例题

<img src="https://image.davidingplus.cn/images/2025/02/02/image-20230719220816068.png" alt="image-20230719220816068" style="zoom:80%;" />

代码：

思路在注释中

~~~cpp
#include <iostream>
using namespace std;

const int N = 100010;
int n;

class _Stack {
public:
    _Stack() {
        __init__();
    }

    // 初始化
    void __init__() {
        top = -1;  // 数组下标从0开始
    }

    // 判断是否为空
    bool _is_empty() {
        return top == -1;
    }

    // 入栈
    void push(int x) {
        _stack[++top] = x;
    }

    // 出栈
    void pop() {
        --top;
    }

    // 得到栈顶元素的值
    int gettop() {
        return _stack[top];
    }

private:
    // 用数组模拟栈
    int _stack[N], top;
};

int main() {
    cin >> n;
    _Stack s;

    for (int i = 0; i < n; ++i) {
        int x;
        cin >> x;
        // 思路：
        // 现在需要找到的的是第i个数往前数最近的一个比他小的数
        // 我们现在考虑i前面的两个数 ax和ay，并且x < y
        // 如果ax>=ay，那么符合后面要求的数永远不可能是ax，因为如果ax符合要求，ay小于等于ax并且距离更近，这个值可以扔掉
        // 显然ax也不符合ay的要求
        // 我们现在用栈来模拟这个过程
        // 现在我插入新的数，我判断类似刚才的数是否符合条件，如果不符合条件，现在从栈中删除
        // 那么怎么找到类似刚才的数呢？
        // 我们可以判断栈顶，如果栈顶不符合要求，则弹出，继续判断，直到符合要求，这个操作不仅帮自己避了雷，也帮后面避了雷
        // 这样就把栈变成了单调的，就是单调栈
        // 如果栈空了说明没有值满足，然后将本值插入栈进行下一回合的循环即可
        
        //用 3 4 2 7 5 的测试样例自己验证一下即可

        while (s.gettop() >= x && !s._is_empty())  // 不满足条件则一直弹出
            s.pop();

        if (s._is_empty())
            cout << -1 << ' ';
        else
            cout << s.gettop() << ' ';

        s.push(x);
    }
    cout << endl;

    return 0;
}
~~~

### 4.单调队列

**典型题型：求一下滑动窗口里面的最大值或者最小值**

这是单调队列用的比较多的一种题型，基本就是这么考的

#### 例题

![image-20230719221813851](https://image.davidingplus.cn/images/2025/02/02/image-20230719221813851.png)

代码还有一些问题，后续修改

### 5.KMP算法

TODO

