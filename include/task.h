#ifndef CTASK_TASK_H
#define CTASK_TASK_H

#include <stdbool.h>
#include <stddef.h>

/* char 数组的容量按字节计，末尾的 '\0' 也占一个字节。 */
#define TASK_TITLE_CAP 96
#define TASK_FLAG_DONE 1u
#define TASK_FLAG_URGENT 2u

typedef enum {
    TASK_OK = 0,
    TASK_INVALID,
    TASK_NOT_FOUND,
    TASK_NOMEM,
    TASK_IO,
    TASK_FORMAT
} TaskStatus;

typedef enum {
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_HIGH = 3
} TaskPriority;

typedef struct {
    unsigned long id;
    char title[TASK_TITLE_CAP];
    TaskPriority priority;
    unsigned flags;
} Task;

/* items 的内存归 TaskList 所有；使用前 init，结束后 free。
 * 始终满足 len <= cap，next_id > 每个现有 id。不要直接改这些字段。 */
typedef struct {
    Task *items;
    size_t len;
    size_t cap;
    unsigned long next_id;
} TaskList;

void task_init(TaskList *list);
void task_free(TaskList *list);

/* 标题允许 1..95 字节，禁止 ASCII 控制字符和 DEL；不解析 UTF-8。
 * new_id 可以为 NULL。失败不改变列表，也不写入 new_id。
 * next_id == ULONG_MAX 时返回 TASK_INVALID，避免编号回绕。 */
TaskStatus task_add(TaskList *list, const char *title,
                    TaskPriority priority, unsigned long *new_id);

/* 返回借用指针，不能 free。add（可能 realloc）、remove、sort、read/load
 * 或 free 后应重新查找；不要跨这些操作保留指针。未找到返回 NULL。 */
const Task *task_find(const TaskList *list, unsigned long id);
TaskStatus task_remove(TaskList *list, unsigned long id);
TaskStatus task_set_done(TaskList *list, unsigned long id, bool done);
TaskStatus task_set_urgent(TaskList *list, unsigned long id, bool urgent);
void task_sort_by_priority(TaskList *list);

/* 回调不能增删列表、排序、释放列表或以其他方式使当前指针失效。 */
typedef void (*TaskVisitor)(const Task *task, void *context);
void task_visit(const TaskList *list, TaskVisitor visitor, void *context);
const char *task_status_string(TaskStatus status);

#endif
