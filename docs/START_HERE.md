# 第一课：让程序听你的话

这一课只做三件事：运行一个 C 程序，修改一句话，增加一次判断。不要先读完整的 `src/main.c`。

## 1. 看见项目的目标

Windows 下双击根目录的 `Start.cmd`。输入 `2` 添加一项任务，输入 `1` 查看，最后输入 `0` 退出。暂时不需要保存。你以后要用 C 代码完成的，就是这个任务管理器。

如果你还没有编译器，按根目录 [README.md](../README.md) 的“编译自己的改动”运行 `setup-compiler.ps1`。

## 2. 打开第一个小程序

用文本编辑器打开 `stages/01_task_card.c`。完整的核心如下：

```c
#include <stdio.h>

int main(void)
{
    const char title[] = "Learn C by building a task manager";
    int priority = 3;
    int done = 0;
    printf("Title: %s\nPriority: %d\nDone: %d\n", title, priority, done);
    return 0;
}
```

此时只记住：

- `#include <stdio.h>` 让编译器看到标准输入输出函数的声明，包括 `printf`。
- `main` 是这个普通命令行程序的入口；`void` 表示这里不接收参数；`int` 表示返回一个整数状态。
- `title` 存放一段文字；`priority` 和 `done` 存放整数。`int done = 0;` 是声明并初始化，`done = 1;` 是给已存在变量赋值。
- `printf` 输出文字。`%s` 对应字符串，`%d` 对应 `int`，`\n` 表示换行；占位符的次序和类型必须匹配参数。
- `return 0;` 向运行环境报告成功。大括号围出函数体；这些声明、调用和返回语句以分号结尾。

先用纸写下预测输出，再在项目目录运行：

```powershell
.\build.ps1 -Action stage -Stage 01_task_card
```

Linux/macOS：`sh build.sh stage 01_task_card`。

输出应为：

```text
Title: Learn C by building a task manager
Priority: 3
Done: 0
```

脚本先把 `.c` 源码编译成可执行文件，再运行它。只保存源码不会自动更新已经编译好的程序。

## 3. 第一个改动

把标题改成 `Finish my first C lesson`，把 `priority` 改成 `2`。保存文件，重新运行同一条构建命令。

你应看到标题和优先级都改变。如果没有改变，检查：编辑的是不是这份文件、是否保存、是否运行了构建命令而不是旧的 `bin/` 成品。

## 4. 第一次判断

在 `return 0;` 前加入：

```c
if (done == 0) {
    puts("Todo");
} else {
    puts("Done");
}
```

`==` 比较两个值是否相等；`=` 是赋值，二者不能混用。`if` 的条件为真执行第一块，否则执行 `else` 的代码。`puts` 输出字符串并自动换行。

先让 `done` 为 `0`，预测最后一行是什么；再改成 `1`，重新编译运行。还可以尝试 `2`：为什么它也输出 `Done`？因为这里问的是“是否等于零”，没有问“是否等于一”。

## 5. 验收与小作业

不用看答案，完成以下任务：

1. 新增变量 `estimated_minutes`，值为 `25`，把它输出为 `Estimate: 25 minutes`。
2. 在原有输出后加入判断：优先级是 `3` 时输出 `High priority`，其他情况输出 `Normal priority`。
3. 分别用优先级 `3` 和 `1` 运行，解释输出为什么不同。
4. 用自己的话说明 `.c` 文件、编译器、可执行文件分别是什么。

验收标准：两种情况结果都正确，能指出自己改了哪一行，以及重新编译的必要性。

<details>
<summary>做完后查看参考片段</summary>

```c
int estimated_minutes = 25;
printf("Estimate: %d minutes\n", estimated_minutes);
if (priority == 3) {
    puts("High priority");
} else {
    puts("Normal priority");
}
```

这些语句放在 `main` 的大括号内，`return 0;` 之前。变量需要在使用前声明。

</details>

## 下一步

继续 [第二阶段：多张任务卡片](../stages/README.md)，配合 [课程](COURSE.md) 中的类型、控制流、函数与数组章节。遇到报错，保留完整报错文字，从第一条开始读；可以把报错和你改过的代码发回聊天。
