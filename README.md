# C 语言任务工坊

> 公开离线版：下载仓库 ZIP 并解压，双击 **Open-Visual.cmd**，或用浏览器打开 **visual/dist/index.html**。无需登录、无需网页服务器。
>
> [下载完整离线包](downloads/C-Task-Lab-Offline.zip?raw=true) · [可视化版说明](visual/README.md) · [第一课](docs/START_HERE.md)

**从零开始，把一个任务管理器做完整。** 教材和注释使用中文，以 ISO C17 为主线，不依赖第三方 C 库。你会经历“显示一张任务卡片 → 管理多张卡片 → 输入菜单 → 动态内存 → 保存文件 → 模块与测试”。

**可视化版已加入：**双击 **Open-Visual.cmd**，或打开 [可视化学习界面](visual/dist/index.html)。包含章节导航、四个内存互动实验、章节自测、练习草稿和可操作的任务界面；详情见 [可视化版说明](visual/README.md)。

现在先打开 **[第一课：做出第一个改动](docs/START_HERE.md)**。不用一次读完整份课程。

## 你将拿到什么

- **一个可以运行的 C 项目**：添加、查看、删除、切换完成/紧急状态、排序、统计、保存与读取任务。
- **三个递进起点**：`stages/`，从几十行代码开始，不要求初学者直接理解最终程序。
- **16 个独立实验**：`lessons/`，每个只集中观察一组概念，有预测输出与解释。
- **6 道动手练习**：`exercises/`，有 TODO、自检、提示和单独的参考答案。
- **系统中文课程**：[COURSE.md](docs/COURSE.md)、[覆盖路线](docs/ROADMAP.md)、[源码导读](docs/PROJECT_GUIDE.md)、[术语与误区](docs/GLOSSARY.md)。

“所有知识”适合作为长期目标。这套项目覆盖 C 核心语法与常用工程方法；并发、复杂标准库、平台 API、嵌入式和网络等内容有明确的拓展路线，不把概念介绍算成实战掌握。每阶段通过“能解释、能修改、能检查”验收。

## Windows：立即体验

解压整个目录后双击 **`Start.cmd`**。它运行附带的 Windows x64 成品 `bin/c-task-lab.exe`。若下载的压缩包被系统拦截，按系统提示处理来自本次下载的文件。

也可在本文件所在目录打开 PowerShell：

```powershell
.\bin\c-task-lab.exe --demo
.\bin\lesson-01.exe
```

`--demo` 在内存里运行，不写文件。交互程序选 **2** 添加、**1** 查看、**7** 保存、**8** 读取、**0** 退出。默认文件是当前目录的 `tasks.tsv`；程序不自动读取或保存。读取将替换当前任务，有未保存改动时须输入 `YES` 确认。

成品仅用于第一次体验。**改完 `.c` 文件后必须重新编译**；双击 `Start.cmd` 运行的始终是附带成品。修改后的版本用下面的 `-Action run` 运行。

## Windows：编译自己的改动

已有 GCC、Clang 或 Zig 时可以直接运行下面的构建命令；没有编译器时，在项目目录运行：

```powershell
powershell -ExecutionPolicy Bypass -File .\setup-compiler.ps1
```

脚本从 [Zig 官网](https://ziglang.org/download/) 下载固定版本 0.15.2（约 89 MiB），核验官方 SHA-256，解压到项目的 `tools/`；使用其中的 `zig cc` 编译 C。它不改系统 PATH，不需要管理员权限。这是为复现构建固定的版本，不是“最新版”声明。官网无法连接时，可从官网手动下载，或使用已有 GCC/Clang。

```powershell
# 第一个实验；修改后再运行同一条命令
.\build.ps1 -Action lesson -Lesson 01_basics

# 项目的第一小步
.\build.ps1 -Action stage -Stage 01_task_card

# 编译并运行完整应用
.\build.ps1 -Action run

# 核心测试；完整检查还会运行所有实验、阶段与参考答案
.\build.ps1 -Action test
.\build.ps1 -Action check

# 初始 TODO 版本会输出 FAIL、返回非零，这是待完成作业的预期行为
.\build.ps1 -Action exercise -Exercise 01_temperature
.\build.ps1 -Action solution -Exercise 01_temperature
```

若 `.ps1` 被执行策略阻止，使用对这次进程生效的命令，不必更改系统策略：

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Action lesson -Lesson 01_basics
```

可用 `-Compiler 'D:\tools\zig.exe'` 或 `-Compiler gcc` 指定编译器。构建结果在 `build/`。GCC/Clang/Zig 支持本脚本；使用 MSVC 请走下面的 CMake 方式。中文乱码时先在终端运行 `chcp 65001`；`Start.cmd` 已设置 UTF-8。源码保存为 UTF-8。

## Linux / macOS

先备好系统 C 编译器（`cc`、GCC 或 Clang），不需要其他 C 库：

```sh
sh build.sh lesson 01_basics
sh build.sh stage 01_task_card
sh build.sh run
sh build.sh test
sh build.sh check
sh build.sh exercise 01_temperature  # 初始预期失败
sh build.sh solution 01_temperature
```

用 `CC=clang sh build.sh test` 指定编译器；Zig 可用 `CC=zig sh build.sh test`。这些脚本以可移植方式编写；本次交付在 Windows x64 实际验证，其他系统需要在当地编译确认。

若熟悉 CMake，可选：

```sh
cmake -S . -B build/cmake
cmake --build build/cmake --config Debug
ctest --test-dir build/cmake -C Debug --output-on-failure
```

## 推荐学习顺序

1. 先按 [START_HERE.md](docs/START_HERE.md) 跑通、预测、修改第一个程序。
2. 按 [stages/README.md](stages/README.md) 完成一张卡片、多张卡片和菜单。
3. 按 [COURSE.md](docs/COURSE.md) 学习对应概念，运行 [lessons/README.md](lessons/README.md) 中的实验。
4. 每学一阶段做一道 [exercises/README.md](exercises/README.md) 练习；先读失败信息，再改 TODO，最后对照答案。
5. 阅读 [PROJECT_GUIDE.md](docs/PROJECT_GUIDE.md)，解释完整应用的数据如何流动，再完成扩展挑战。

学习速度由你决定。不要用“看完章节”替代“亲手改过并解释”。可以把你的代码、输出或不理解的一行发回聊天，继续逐课学习。

## 文件地图

```text
c-task-lab/
  README.md                 入口与运行方式
  Start.cmd                Windows 成品启动器
  bin/                     已编译 Windows x64 体验版
  docs/                    课程、路线、导读与术语
  stages/                  三个递进的小版本
  lessons/                 16 个独立实验
  exercises/               六份 TODO 作业；solutions/ 是答案
  include/                 数据类型与公开函数声明
  src/                     完整应用：main / input / task / storage
  tests/                   核心行为和输入边界测试
  build.ps1 / build.sh     编译、运行和自检
  setup-compiler.ps1        可选的 Windows 本地编译器准备
  CMakeLists.txt           可选构建方式
```

任务标题按字节限制为 95 字节，UTF-8 中文通常占多个字节；程序不验证 UTF-8 编码、不处理复杂的终端字符宽度。文件读取会严格校验格式，并在完全成功后替换旧数据。保存使用直接覆盖写入，**不保证断电安全或原子替换**；这是 [扩展挑战](docs/PROJECT_GUIDE.md) 中要继续改善的一项。具体测试结果与范围见 [VALIDATION.md](docs/VALIDATION.md)。

查证语言规则时，可参考 [WG14 C11 公开委员会草案 N1570](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf) 和课程中的标准资料链接；草案用于查规则，不适合作为初学者第一本教材。
