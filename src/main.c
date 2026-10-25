#include "input.h"
#include "storage.h"
#include "task.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void print_task(const Task *task, void *context)
{
    FILE *output = context;
    fprintf(output, "#%-4lu [%s] P%d %s%s\n", task->id,
            (task->flags & TASK_FLAG_DONE) != 0u ? "x" : " ",
            (int)task->priority,
            (task->flags & TASK_FLAG_URGENT) != 0u ? "! " : "", task->title);
}

static void show_tasks(const TaskList *list)
{
    size_t done = 0;
    for (size_t i = 0; i < list->len; ++i) {
        if ((list->items[i].flags & TASK_FLAG_DONE) != 0u) {
            ++done;
        }
    }
    printf("\n任务 %zu 项，完成 %zu 项，待办 %zu 项。\n", list->len, done, list->len - done);
    task_visit(list, print_task, stdout);
}

/* 返回 1=有效输入，0=无效输入，-1=结束输入。 */
static int prompt_text(const char *prompt, char *buffer, size_t capacity)
{
    InputStatus status;
    fputs(prompt, stdout);
    fflush(stdout);
    status = input_line(stdin, buffer, capacity);
    if (status == INPUT_EOF || status == INPUT_ERROR) {
        return -1;
    }
    if (status == INPUT_TOO_LONG) {
        puts("输入太长或含有 NUL，本次输入已丢弃。");
        return 0;
    }
    return 1;
}

static int prompt_number(const char *prompt, unsigned long minimum,
                         unsigned long maximum, unsigned long *value)
{
    char buffer[64];
    int status = prompt_text(prompt, buffer, sizeof buffer);
    if (status != 1) {
        return status;
    }
    if (!input_ulong(buffer, minimum, maximum, value)) {
        printf("请输入 %lu 到 %lu 的十进制整数。\n", minimum, maximum);
        return 0;
    }
    return 1;
}

static void report(TaskStatus status)
{
    if (status == TASK_OK) {
        puts("操作完成。");
    } else {
        printf("操作失败：%s\n", task_status_string(status));
    }
}

static int demo(void)
{
    TaskList list;
    unsigned long first = 0;
    TaskStatus status;
    task_init(&list);
    status = task_add(&list, "Run lesson 01 and change its output", PRIORITY_HIGH, &first);
    if (status == TASK_OK) {
        status = task_add(&list, "Draw the TaskList memory layout", PRIORITY_NORMAL, NULL);
    }
    if (status == TASK_OK) {
        status = task_add(&list, "Complete exercise 01 without the solution", PRIORITY_HIGH, NULL);
    }
    if (status != TASK_OK) {
        report(status);
        task_free(&list);
        return 1;
    }
    (void)task_set_done(&list, first, true);
    (void)task_set_urgent(&list, first, true);
    task_sort_by_priority(&list);
    puts("C 语言任务工坊 · 演示（仅在内存运行）");
    show_tasks(&list);
    puts("\n学习入口：README.md → docs/START_HERE.md → docs/COURSE.md");
    task_free(&list);
    return 0;
}

static void help(const char *program)
{
    printf("Usage: %s [--demo | --help | --file PATH]\n", program);
    puts("无参数进入交互菜单；默认数据文件 tasks.tsv。");
    puts("启动时不自动读取，退出时不自动保存。菜单 7 保存，菜单 8 读取。");
    puts("--demo 仅展示功能，不写入文件。--file 指定保存/读取路径。");
}

int main(int argc, char **argv)
{
    const char *path = "tasks.tsv";
    TaskList list;
    bool changed = false;
    bool running = true;
    if (argc == 2 && strcmp(argv[1], "--demo") == 0) {
        return demo();
    }
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        help(argv[0]);
        return 0;
    }
    if (argc == 3 && strcmp(argv[1], "--file") == 0 && argv[2][0] != '\0') {
        path = argv[2];
    } else if (argc != 1) {
        help(argv[0]);
        return 2;
    }
    task_init(&list);
    puts("C 语言任务工坊 | C17 | 输入 0 退出");
    printf("数据文件：%s（手动保存/读取）\n", path);
    while (running) {
        unsigned long choice = 0;
        unsigned long id = 0;
        int input;
        TaskStatus status = TASK_OK;
        puts("\n1 列出  2 添加  3 切换完成  4 删除  5 切换紧急  6 排序");
        puts("7 保存  8 读取  9 学习路线  0 退出");
        input = prompt_number("> ", 0, 9, &choice);
        if (input < 0) {
            break;
        }
        if (input == 0) {
            continue;
        }
        switch (choice) {
        case 0:
            running = false;
            break;
        case 1:
            show_tasks(&list);
            break;
        case 2: {
            char title[TASK_TITLE_CAP];
            unsigned long priority = 0;
            input = prompt_text("标题（1–95 字节）：", title, sizeof title);
            if (input < 0) {
                running = false;
                break;
            }
            if (input == 0) {
                break;
            }
            input = prompt_number("优先级 1低/2中/3高：", 1, 3, &priority);
            if (input < 0) {
                running = false;
                break;
            }
            if (input == 0) {
                break;
            }
            status = task_add(&list, title, (TaskPriority)priority, &id);
            if (status == TASK_OK) {
                printf("新增任务 #%lu。\n", id);
                changed = true;
            } else {
                report(status);
            }
            break;
        }
        case 3:
        case 4:
        case 5: {
            const Task *found;
            input = prompt_number("任务 ID：", 1, ULONG_MAX, &id);
            if (input < 0) {
                running = false;
                break;
            }
            if (input == 0) {
                break;
            }
            found = task_find(&list, id);
            if (found == NULL) {
                report(TASK_NOT_FOUND);
                break;
            }
            if (choice == 3) {
                status = task_set_done(&list, id, (found->flags & TASK_FLAG_DONE) == 0u);
            } else if (choice == 4) {
                status = task_remove(&list, id);
            } else {
                status = task_set_urgent(&list, id, (found->flags & TASK_FLAG_URGENT) == 0u);
            }
            changed = changed || status == TASK_OK;
            report(status);
            break;
        }
        case 6:
            task_sort_by_priority(&list);
            changed = true;
            show_tasks(&list);
            break;
        case 7:
            status = task_save(path, &list);
            if (status == TASK_OK) {
                changed = false;
            }
            report(status);
            break;
        case 8:
            if (changed) {
                char answer[8];
                input = prompt_text("读取会替换尚未保存的任务。输入 YES 继续：", answer, sizeof answer);
                if (input < 0) {
                    running = false;
                    break;
                }
                if (input != 1 || strcmp(answer, "YES") != 0) {
                    puts("已取消读取。");
                    break;
                }
            }
            status = task_load(path, &list);
            if (status == TASK_OK) {
                changed = false;
            }
            report(status);
            break;
        case 9:
            puts("先读 docs/START_HERE.md，完成第一个改动。");
            puts("stages/ 从一张卡片到菜单；lessons/ 是 16 个独立实验。");
            puts("docs/COURSE.md 是课程；exercises/ 是带自检的作业。");
            puts("docs/ROADMAP.md 标出必学与进阶，docs/PROJECT_GUIDE.md 解读最终源码。");
            break;
        default:
            break;
        }
    }
    if (changed) {
        puts("本次修改尚未保存；程序退出后将丢弃。下次请先选 7 保存。");
    }
    task_free(&list);
    if (ferror(stdin)) {
        fputs("读取标准输入失败。\n", stderr);
        return 1;
    }
    return 0;
}
