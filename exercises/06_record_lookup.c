#include <stdio.h>

typedef struct {
    unsigned int id;
    const char *title;
    int done;
} Task;

/* TODO: 返回第一个 id 匹配的记录地址；找不到或 tasks 为空返回 NULL。
   返回的是借用指针，不能 free；count 必须不超过数组实际长度。 */
static const Task *find_task(const Task *tasks, size_t count, unsigned int id)
{
    (void)tasks;
    (void)count;
    (void)id;
    return NULL;
}

static int check(int condition, const char *label)
{
    if (!condition) {
        printf("FAIL: %s\n", label);
        return 1;
    }
    return 0;
}

int main(void)
{
    const Task tasks[] = {
        {.id = 7U, .title = "read", .done = 1},
        {.id = 9U, .title = "write", .done = 0},
        {.id = 7U, .title = "review", .done = 0},
        {.id = 11U, .title = "test", .done = 1}
    };
    size_t count = sizeof tasks / sizeof tasks[0];
    int failed = 0;
    failed += check(find_task(tasks, count, 7U) == &tasks[0], "first duplicate");
    failed += check(find_task(tasks, count, 9U) == &tasks[1], "middle record");
    failed += check(find_task(tasks, count, 11U) == &tasks[3], "last record");
    failed += check(find_task(tasks, count, 99U) == NULL, "not found");
    failed += check(find_task(tasks, 0, 7U) == NULL, "empty array");
    failed += check(find_task(NULL, 0, 7U) == NULL, "null array");
    if (failed != 0) {
        return 1;
    }
    puts("PASS: record lookup");
    return 0;
}

