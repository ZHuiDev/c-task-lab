# 同一个项目，逐步长大

先完成小版本，再阅读最终的 `src/`。每份文件都可以独立编译，不需要先理解整个项目。

| 阶段 | 运行命令（Windows PowerShell） | 你要增加的功能 | 过关证据 |
|---|---|---|---|
| 1 一张卡片 | `./build.ps1 -Action stage -Stage 01_task_card` | 改标题，增加完成状态判断 | 能解释变量、格式符、分号、返回值；改值后输出相应变化 |
| 2 多张卡片 | `./build.ps1 -Action stage -Stage 02_fixed_list` | 增加第四项，写 `count_pending` 函数 | 数量不写死；结果由数组数据计算 |
| 3 接受输入 | `./build.ps1 -Action stage -Stage 03_menu` | 加菜单 3，显示当前完成状态 | 无效输入会重试；EOF 可以退出 |
| 4 动态任务表 | 阅读 `include/task.h` 和 `src/task.c` | 先自己实现 `task_find`、再对照成品 | 能说明 `len`、`cap`、借用指针、`realloc` 临时指针 |
| 5 磁盘保存 | 阅读 `src/storage.c` | 先写只有一条记录的保存，再加严格读取 | 保存后重开读取，ID 不回退；损坏文件不改变旧任务 |
| 6 完整应用 | `./build.ps1 -Action run` | 完成 `docs/PROJECT_GUIDE.md` 的扩展挑战 | 正常操作和非法输入都能解释；`-Action test` 通过 |

Linux/macOS 用 `sh build.sh stage 01_task_card`（后两个阶段替换名字）。学习时可复制阶段文件到自建目录再修改；不要一上来抄完整应用。

这三份是递进的可运行起点；阶段 4–6 要动手改最终源码。运行完成不等于掌握：每阶段至少做一个改动，再用自己的话说明它为什么有效。
