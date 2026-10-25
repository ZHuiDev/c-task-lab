#include <stdio.h>

typedef enum { TASK_TODO, TASK_DONE } TaskStatus;
typedef struct {
    unsigned int id;
    const char *title;
    TaskStatus status;
} Task;

typedef enum { VALUE_INTEGER, VALUE_REAL } ValueKind;
typedef union {
    int integer;
    double real;
} ValueData;
typedef struct {
    ValueKind kind;
    ValueData data;
} Value;

static void print_value(const Value *value)
{
    /* 根据标签读取最近写入的成员，避免混淆 union 的有效值。 */
    switch (value->kind) {
        case VALUE_INTEGER: printf("integer=%d\n", value->data.integer); break;
        case VALUE_REAL: printf("real=%.1f\n", value->data.real); break;
    }
}

int main(void)
{
    Task task = {.id = 7U, .title = "learn structs", .status = TASK_TODO};
    Value value = {.kind = VALUE_INTEGER, .data = {.integer = 42}};
    task.status = TASK_DONE;
    printf("task=%u title=%s done=%d\n", task.id, task.title, task.status == TASK_DONE);
    print_value(&value);
    value.kind = VALUE_REAL;
    value.data.real = 3.5;
    print_value(&value);
    return 0;
}
