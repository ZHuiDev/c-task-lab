#include <stddef.h>
#include <stdio.h>

typedef struct {
    char title[64];
    int done;
} TaskCard;

static size_t count_done(const TaskCard tasks[], size_t count)
{
    size_t completed = 0;
    for (size_t i = 0; i < count; ++i) {
        if (tasks[i].done != 0) { ++completed; }
    }
    return completed;
}

int main(void)
{
    TaskCard tasks[] = {
        {"Compile lesson 01", 1},
        {"Change the task title", 0},
        {"Explain what a loop does", 0}
    };
    const size_t count = sizeof tasks / sizeof tasks[0];
    for (size_t i = 0; i < count; ++i) {
        printf("%zu. [%c] %s\n", i + 1, tasks[i].done ? 'x' : ' ', tasks[i].title);
    }
    printf("Done: %zu/%zu\n", count_done(tasks, count), count);
    /* 练习：增加一项任务，再写一个函数返回未完成任务数。 */
    return 0;
}
