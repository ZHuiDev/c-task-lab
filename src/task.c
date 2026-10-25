#include "task.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static bool title_is_valid(const char *title)
{
    size_t length;

    if (title == NULL) {
        return false;
    }
    for (length = 0u; length < TASK_TITLE_CAP; ++length) {
        unsigned char byte = (unsigned char)title[length];
        if (byte == 0u) {
            return length != 0u;
        }
        if (byte < 32u || byte == 127u) {
            return false;
        }
    }
    return false;
}

static bool priority_is_valid(TaskPriority priority)
{
    return priority == PRIORITY_LOW || priority == PRIORITY_NORMAL ||
           priority == PRIORITY_HIGH;
}

static TaskStatus reserve_one_more(TaskList *list)
{
    size_t capacity;
    Task *replacement;

    if (list->len < list->cap) {
        return TASK_OK;
    }
    if (list->len == SIZE_MAX / sizeof *list->items) {
        return TASK_NOMEM;
    }
    capacity = list->cap == 0u ? 8u : list->cap;
    if (capacity > SIZE_MAX / 2u) {
        capacity = list->len + 1u;
    } else if (list->cap != 0u) {
        capacity *= 2u;
    }
    if (capacity > SIZE_MAX / sizeof *list->items) {
        capacity = list->len + 1u;
    }
    /* realloc 失败时原指针仍有效，所以必须先接到临时变量。 */
    replacement = realloc(list->items, capacity * sizeof *list->items);
    if (replacement == NULL) {
        return TASK_NOMEM;
    }
    list->items = replacement;
    list->cap = capacity;
    return TASK_OK;
}

void task_init(TaskList *list)
{
    if (list != NULL) {
        list->items = NULL;
        list->len = 0u;
        list->cap = 0u;
        list->next_id = 1ul;
    }
}

void task_free(TaskList *list)
{
    if (list != NULL) {
        free(list->items);
        task_init(list);
    }
}

TaskStatus task_add(TaskList *list, const char *title,
                    TaskPriority priority, unsigned long *new_id)
{
    TaskStatus status;
    Task value = {0};

    if (list == NULL || !title_is_valid(title) ||
        !priority_is_valid(priority) || list->next_id == 0ul ||
        list->next_id == ULONG_MAX || list->len > list->cap ||
        list->cap > SIZE_MAX / sizeof *list->items ||
        (list->cap != 0u && list->items == NULL)) {
        return TASK_INVALID;
    }
    /* 先复制标题，允许 title 是当前列表里某一任务的标题。
     * 扩容后原来的借用指针可能失效。 */
    value.id = list->next_id;
    (void)strcpy(value.title, title);
    value.priority = priority;
    value.flags = 0u;
    status = reserve_one_more(list);
    if (status != TASK_OK) {
        return status;
    }
    list->items[list->len] = value;
    ++list->len;
    ++list->next_id;
    if (new_id != NULL) {
        *new_id = value.id;
    }
    return TASK_OK;
}

const Task *task_find(const TaskList *list, unsigned long id)
{
    size_t index;

    if (list == NULL || id == 0ul) {
        return NULL;
    }
    for (index = 0u; index < list->len; ++index) {
        if (list->items[index].id == id) {
            return &list->items[index];
        }
    }
    return NULL;
}

TaskStatus task_remove(TaskList *list, unsigned long id)
{
    size_t index;

    if (list == NULL || id == 0ul) {
        return TASK_INVALID;
    }
    for (index = 0u; index < list->len; ++index) {
        if (list->items[index].id == id) {
            /* memmove 允许重叠；移动的是元素个数乘以元素大小。 */
            if (index + 1u < list->len) {
                (void)memmove(&list->items[index], &list->items[index + 1u],
                              (list->len - index - 1u) * sizeof *list->items);
            }
            --list->len;
            return TASK_OK;
        }
    }
    return TASK_NOT_FOUND;
}

static TaskStatus set_flag(TaskList *list, unsigned long id,
                           unsigned mask, bool enabled)
{
    size_t index;

    if (list == NULL || id == 0ul) {
        return TASK_INVALID;
    }
    for (index = 0u; index < list->len; ++index) {
        if (list->items[index].id == id) {
            if (enabled) {
                list->items[index].flags |= mask;
            } else {
                list->items[index].flags &= ~mask;
            }
            return TASK_OK;
        }
    }
    return TASK_NOT_FOUND;
}

TaskStatus task_set_done(TaskList *list, unsigned long id, bool done)
{
    return set_flag(list, id, TASK_FLAG_DONE, done);
}

TaskStatus task_set_urgent(TaskList *list, unsigned long id, bool urgent)
{
    return set_flag(list, id, TASK_FLAG_URGENT, urgent);
}

static int compare_priority(const void *left, const void *right)
{
    const Task *first = left;
    const Task *second = right;

    /* 不用减法，避免无符号回绕和窄化成 int。 */
    if (first->priority != second->priority) {
        return first->priority > second->priority ? -1 : 1;
    }
    return (first->id > second->id) - (first->id < second->id);
}

void task_sort_by_priority(TaskList *list)
{
    if (list != NULL && list->len > 1u) {
        qsort(list->items, list->len, sizeof *list->items, compare_priority);
    }
}

void task_visit(const TaskList *list, TaskVisitor visitor, void *context)
{
    size_t index;

    if (list != NULL && visitor != NULL) {
        for (index = 0u; index < list->len; ++index) {
            visitor(&list->items[index], context);
        }
    }
}

const char *task_status_string(TaskStatus status)
{
    switch (status) {
    case TASK_OK: return "ok";
    case TASK_INVALID: return "invalid argument or ID limit reached";
    case TASK_NOT_FOUND: return "task not found";
    case TASK_NOMEM: return "out of memory";
    case TASK_IO: return "file I/O error";
    case TASK_FORMAT: return "invalid task file format";
    default: return "unknown status";
    }
}
