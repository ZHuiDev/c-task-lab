# 16 个独立 C 实验

每个 `.c` 都有自己的 `main`，应当**单独编译**，不要把这 16 个文件一起链接。
建议顺序：先预测输出 → 编译运行 → 对照答案 → 完成观察任务 → 修改一个条件再运行。
注释使用中文，程序输出使用 ASCII。以下输出假设内存和临时文件创建成功；失败路径会返回非零退出码。

从项目根目录运行，例如在 PowerShell 中：

```powershell
New-Item -ItemType Directory -Force build | Out-Null
gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow .\lessons\01_basics.c -o .\build\01_basics.exe
.\build\01_basics.exe
$LASTEXITCODE
```

将文件名换成其他课即可，Clang 也可使用同样的警告选项。在 Linux/macOS 下可将路径改为 `lessons/01_basics.c`，输出设为 `build/01_basics`，使用 `./build/01_basics` 运行。

## 01 — 程序骨架、变量与输出

文件：`01_basics.c`。

观察：`#include` 提供什么声明？为什么 `%s` 对应字符串、`%d` 对应 `int`？将赋值改成 `completed += 2`，输出会怎样？

预测答案：

```text
Welcome to C Task Lab!
Completed: 1
```

修改后第二行变为 `Completed: 2`。`return 0` 向操作系统报告成功，并不是打印数字 0。

## 02 — 类型、常量、显式转换

文件：`02_types.c`。

观察：`sizeof` 的结果用哪个格式符输出？`(int)2.5` 为什么得到 2？`bool` 参与可变参数调用时发生什么转换？

预测答案：

```text
integer=42 count=7 ratio=2.5
grade=A ready=1 char_size=1
converted=2
```

`sizeof` 返回 `size_t`，对应 `%zu`；可表示范围内的浮点转整数舍弃小数部分。`bool` 经默认参数提升成为 `int`。`sizeof(char)` 按定义为 1，但 1 个 C 字节不保证正好是 8 位；不要从此推断 `sizeof(int)` 一定为 4。

## 03 — 运算、位操作、短路求值

文件：`03_operators.c`。

观察：为什么 `7 / 2` 与 `(double)7 / 2` 不同？若只把左侧条件改成 `a > b`，`visits` 变为多少？

预测答案：

```text
division=3 remainder=1 real=3.5
flag2=1 flag0=0
visits=0 max=7
```

整型除法不保留小数。位标记采用无符号类型。左侧条件改为真后执行右侧，`visits` 变为 1。不要把多个 `i++` 塞进求值次序不明确的同一表达式来“观察结果”。

## 04 — 分支与循环

文件：`04_control.c`。

观察：逐次写出 `i`、`sum`。`continue` 跳过哪一次相加？`break` 终止哪层循环？`do...while` 至少执行多少次？

预测答案：

```text
sum=8
3 2 1 go
matched eight
do_while=1
```

相加的是 1、3、4；遇到 5 就退出循环。`do...while` 先执行再判断，至少一次。

## 05 — 函数、参数、递归与结果参数

文件：`05_functions.c`。

观察：为什么需要 `&result`？递归停止条件是什么？当输入为 13 或结果指针为 `NULL` 时，函数如何报告错误？

预测答案：

```text
6! = 720
0! = 1
13 accepted: 0
```

C 按值传参；传入地址后，函数可通过解引用写回结果。递归在 `n <= 1` 停止。入口限定 `n <= 12`，确保最小标准保证范围的 `unsigned long` 也可容纳结果，且递归深度受限。失败时不改动结果参数。

## 06 — 数组、下标与二维数组

文件：`06_arrays.c`。

观察：为什么循环使用 `< count`？`sizeof values / sizeof values[0]` 在把数组传给函数后是否仍能获得长度？

预测答案：

```text
count=5 sum=14 min=1
grid[1][2]=6
```

有效下标为 0 到 4。函数参数中的 `int values[]` 会调整为指针，不能用该表达式恢复原数组长度；必须另传长度。二维数组的最后一维决定每行跨度。

## 07 — 字符数组、终止符与容量

文件：`07_strings.c`。

观察：`strlen` 与 `sizeof` 为什么相差 1？把 `title` 的容量改成 4 会发生什么？

预测答案：

```text
Hello length=5 storage=6
title=C lab equal=1
terminator=1
```

`strlen` 不计算 `\0`，而数组存储包含它。容量改成 4 后 `snprintf` 报告本应写出的长度 5，程序识别截断并返回 1。注意 `snprintf` 的返回值不是“实际写入的长度”；容量足够必须满足返回值非负且小于容量。

## 08 — 指针、解引用、常量与尾后位置

文件：`08_pointers.c`。

观察：为什么交换函数需要指针？`end` 能否形成、比较、相减、解引用？`const int *cursor` 限制的是指针移动还是写入元素？

预测答案：

```text
left=8 right=3
sum=60 distance=3
null accepted: 0
```

尾后指针可形成，并可在同一数组内比较或相减；不能解引用。指针差类型是 `ptrdiff_t`，使用 `%td`。`const int *` 仍可移动，但不能通过它写入元素。不要对无关数组的指针做减法。

## 09 — 动态内存与资源所有权

文件：`09_memory.c`。

观察：谁负责释放 `values`？为什么 `realloc` 结果先放在 `replacement`？新扩出来的区域会自动清零吗？

预测答案：

```text
count=5 sum=15
```

`main` 持有并释放内存。`realloc` 失败不释放原块；使用临时指针防止丢失原地址。成功后原地址可能失效。`calloc` 初始字节清零，`realloc` 新区域未初始化，必须赋值后读取。本例转换下标为 `int` 安全，因为下标只到 4；通用容器需要明确处理数值范围。

## 10 — 结构体、枚举、联合体与类型别名

文件：`10_records.c`。

观察：`typedef` 是否创建一个新的基本类型？`task.status` 和 `value->kind` 的访问形式为何不同？为什么需要给 `union` 增加标签？

预测答案：

```text
task=7 title=learn structs done=1
integer=42
real=3.5
```

`typedef` 提供类型别名。对象用 `.`，对象指针用 `->`。联合体成员共享存储，本例用 `kind` 跟踪有效成员；不要把写入 `real` 后读取 `integer` 当作跨平台数值转换。结构体可能包含填充，不能直接按其内存布局设计持久化格式。

## 11 — 文件流、错误与资源清理

文件：`11_files.c`。

观察：为什么写后读之间调用 `fflush` 和 `fseek`？为什么不能写成 `while (!feof(stream))`？

预测答案：

```text
line: alpha
line: beta
lines=2 eof=1
```

更新流从写切换到读需要满足标准规定的同步要求，这里刷新并重新定位。`feof` 只有在读操作已经遇到文件尾之后才成立；应以读取函数返回值控制循环，再区分 EOF 与错误。该实验使用 `tmpfile()`，关闭后自动移除临时文件，不在项目目录留下数据文件。

## 12 — 函数指针、回调与标准库排序

文件：`12_callbacks.c`。

观察：`Predicate` 的参数与返回类型是什么？将 `is_even` 换成 `is_positive` 需要改动统计函数吗？比较器为何不能简单返回 `a - b`？

预测答案：

```text
even=2
1 2 3 4 5
```

回调把“如何判断”作为参数传入，统计流程保持不变。排序比较器只需返回负、零、正；直接相减可能有符号溢出。`qsort` 不保证相等元素的原顺序，不适合在未额外处理时实现稳定排序。

## 13 — 链表、间接指针与释放顺序

文件：`13_structures.c`。

观察：为什么传 `Node **head`？为什么输出顺序与插入顺序相反？先 `free(old)` 再读取 `old->next` 有什么问题？

预测答案：

```text
3 2 1 sum=6
empty=1
```

二级指针让函数更新调用者的头指针。每次头插导致顺序反转。必须在释放前保存下一个节点；释放后再访问属于未定义行为。中途分配失败也会清理已分配的链表。

## 14 — 排序、二分与边界不变量

文件：`14_algorithms.c`。

观察：插入排序第 `i` 轮开始时哪段已有序？二分为什么选半开区间？`target` 大于全部元素时返回的值能否直接当作下标？

预测答案：

```text
1 2 3 3 4
first_3=2 insertion_9=5 empty=0
```

插入排序保持 `[0, i)` 已有序，最坏时间 O(n²)。`lower_bound` 在有序数组上找到第一个不小于目标的位置，时间 O(log n)，未必找到相等值；返回 `count` 时不能解引用。半开区间可自然表示空数组，并避免对 `size_t` 做 `count - 1` 的下溢运算。

## 15 — 输入验证、错误码与转换范围

文件：`15_errors.c`。

观察：只检查 `strtol` 的返回值能否区分合法的 0 与解析失败？为什么先把 `errno` 清零？为什么还要检查 `INT_MIN` 和 `INT_MAX`？

预测答案：

```text
input[0]=42
input[1]=invalid unchanged=-1
input[2]=invalid unchanged=-1
input[3]=invalid unchanged=-1
```

必须组合检查结束指针、剩余字符、`errno` 和目标类型范围。`long` 的范围可能比 `int` 大。`errno` 不会在每次成功调用后自动清零。本函数允许 `strtol` 接受的前导空白与正负号，但拒绝尾随字符；如果产品要求更严格格式，应在转换前增加词法检查。

## 16 — 预处理、静态断言与泛型选择

文件：`16_preprocessor.c`。

观察：`#if` 与运行时 `if` 分别在什么时候生效？`_Static_assert` 在什么时候失败？为什么数组长度宏不能接收指针？

预测答案（默认选项）：

```text
length=3 type=int real=double
feature=on
square=16
```

额外添加编译选项 `-DLAB_FEATURE=0` 后仅第二行变成 `feature=off`。`_Generic` 按表达式类型选择分支，不是运行时类型反射；其控制表达式不求值。宏作文本替换，必须留意括号、重复求值和作用域；函数通常更适合包含行为的操作。`ARRAY_LENGTH` 只能用于当前作用域中真正的数组对象。
