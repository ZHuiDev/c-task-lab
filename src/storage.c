#include "storage.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* 一个十进制数字占的位数至少为 1，因此位数是十进制位数的安全上界。 */
#define LINE_CAP (TASK_TITLE_CAP + sizeof(unsigned long) * CHAR_BIT + 32u)

static bool title_is_valid(const char *title)
{
    size_t index;

    for (index = 0u; index < TASK_TITLE_CAP; ++index) {
        unsigned char byte = (unsigned char)title[index];
        if (byte == 0u) {
            return index != 0u;
        }
        if (byte < 32u || byte == 127u) {
            return false;
        }
    }
    return false;
}

static TaskStatus validate_list(const TaskList *list)
{
    size_t index;

    if (list == NULL || list->next_id == 0ul || list->len > list->cap ||
        (list->len != 0u && list->items == NULL)) {
        return TASK_INVALID;
    }
    for (index = 0u; index < list->len; ++index) {
        size_t earlier;
        const Task *task = &list->items[index];
        if (task->id == 0ul || task->id >= list->next_id ||
            !title_is_valid(task->title) ||
            (task->priority != PRIORITY_LOW &&
             task->priority != PRIORITY_NORMAL &&
             task->priority != PRIORITY_HIGH) ||
            (task->flags & ~(TASK_FLAG_DONE | TASK_FLAG_URGENT)) != 0u) {
            return TASK_INVALID;
        }
        for (earlier = 0u; earlier < index; ++earlier) {
            if (list->items[earlier].id == task->id) {
                return TASK_INVALID;
            }
        }
    }
    return TASK_OK;
}

TaskStatus task_write(FILE *stream, const TaskList *list)
{
    TaskStatus status;
    size_t index;

    if (stream == NULL) {
        return TASK_INVALID;
    }
    status = validate_list(list);
    if (status != TASK_OK) {
        return status;
    }
    if (fprintf(stream, "CTASK\t1\t%lu\n", list->next_id) < 0) {
        return TASK_IO;
    }
    for (index = 0u; index < list->len; ++index) {
        const Task *task = &list->items[index];
        if (fprintf(stream, "%lu\t%d\t%u\t%s\n", task->id,
                    (int)task->priority, task->flags, task->title) < 0) {
            return TASK_IO;
        }
    }
    return ferror(stream) != 0 ? TASK_IO : TASK_OK;
}

/* 不用 fgets 后再 strlen：输入中的 NUL 不能被误当成正常字符串终点。 */
static TaskStatus read_line(FILE *stream, char *line, size_t capacity, bool *at_eof)
{
    size_t length = 0u;
    int byte;

    *at_eof = false;
    while ((byte = fgetc(stream)) != EOF) {
        if (byte == '\n') {
            if (length > 0u && line[length - 1u] == '\r') {
                --length;
            }
            line[length] = '\0';
            return TASK_OK;
        }
        if (byte == 0 || length + 1u >= capacity) {
            return TASK_FORMAT;
        }
        line[length] = (char)byte;
        ++length;
    }
    if (ferror(stream) != 0) {
        return TASK_IO;
    }
    if (length != 0u) {
        return TASK_FORMAT;
    }
    *at_eof = true;
    return TASK_OK;
}

static bool parse_unsigned(const char *text, unsigned long *value)
{
    const char *cursor;
    char *end;
    unsigned long parsed;

    if (text[0] == '\0') {
        return false;
    }
    /* strtoul 本身接受符号和前导空白；文件格式明确禁止它们。 */
    for (cursor = text; *cursor != '\0'; ++cursor) {
        if (*cursor < '0' || *cursor > '9') {
            return false;
        }
    }
    errno = 0;
    parsed = strtoul(text, &end, 10);
    if (errno == ERANGE || *end != '\0') {
        return false;
    }
    *value = parsed;
    return true;
}

static TaskStatus read_record(char *line, TaskList *candidate,
                              unsigned long persisted_next_id)
{
    char *priority_text;
    char *flags_text;
    char *title;
    unsigned long id;
    unsigned long priority;
    unsigned long flags;
    TaskStatus status;

    priority_text = strchr(line, '\t');
    if (priority_text == NULL) {
        return TASK_FORMAT;
    }
    *priority_text++ = '\0';
    flags_text = strchr(priority_text, '\t');
    if (flags_text == NULL) {
        return TASK_FORMAT;
    }
    *flags_text++ = '\0';
    title = strchr(flags_text, '\t');
    if (title == NULL) {
        return TASK_FORMAT;
    }
    *title++ = '\0';
    if (strchr(title, '\t') != NULL || !parse_unsigned(line, &id) ||
        !parse_unsigned(priority_text, &priority) ||
        !parse_unsigned(flags_text, &flags) || id == 0ul ||
        id == ULONG_MAX || id >= persisted_next_id ||
        priority < 1ul || priority > 3ul || flags > 3ul ||
        task_find(candidate, id) != NULL) {
        return TASK_FORMAT;
    }
    status = task_add(candidate, title, (TaskPriority)priority, NULL);
    if (status != TASK_OK) {
        return status == TASK_INVALID ? TASK_FORMAT : status;
    }
    candidate->items[candidate->len - 1u].id = id;
    candidate->items[candidate->len - 1u].flags = (unsigned)flags;
    return TASK_OK;
}

TaskStatus task_read(FILE *stream, TaskList *list)
{
    char line[LINE_CAP];
    const char prefix[] = "CTASK\t1\t";
    bool at_eof;
    unsigned long persisted_next_id;
    TaskStatus status;
    TaskList candidate;

    if (stream == NULL || list == NULL) {
        return TASK_INVALID;
    }
    task_init(&candidate);
    status = read_line(stream, line, sizeof line, &at_eof);
    if (status != TASK_OK) {
        return status;
    }
    if (at_eof || strncmp(line, prefix, sizeof prefix - 1u) != 0 ||
        !parse_unsigned(line + sizeof prefix - 1u, &persisted_next_id) ||
        persisted_next_id == 0ul) {
        return TASK_FORMAT;
    }
    for (;;) {
        status = read_line(stream, line, sizeof line, &at_eof);
        if (status != TASK_OK || at_eof) {
            break;
        }
        status = read_record(line, &candidate, persisted_next_id);
        if (status != TASK_OK) {
            break;
        }
    }
    if (status != TASK_OK) {
        task_free(&candidate);
        return status;
    }
    /* 提交点：所有解析和分配成功后，才释放旧值并转交内存所有权。 */
    candidate.next_id = persisted_next_id;
    task_free(list);
    *list = candidate;
    return TASK_OK;
}

TaskStatus task_save(const char *path, const TaskList *list)
{
    FILE *stream;
    TaskStatus status;

    if (path == NULL || path[0] == '\0') {
        return TASK_INVALID;
    }
    /* 先验证，避免参数错误也把旧文件截断。 */
    status = validate_list(list);
    if (status != TASK_OK) {
        return status;
    }
    stream = fopen(path, "wb");
    if (stream == NULL) {
        return TASK_IO;
    }
    status = task_write(stream, list);
    if (fclose(stream) != 0) {
        return TASK_IO;
    }
    return status;
}

TaskStatus task_load(const char *path, TaskList *list)
{
    FILE *stream;
    TaskStatus status;
    TaskList candidate;

    if (path == NULL || path[0] == '\0' || list == NULL) {
        return TASK_INVALID;
    }
    stream = fopen(path, "rb");
    if (stream == NULL) {
        return TASK_IO;
    }
    task_init(&candidate);
    status = task_read(stream, &candidate);
    if (fclose(stream) != 0) {
        status = TASK_IO;
    }
    if (status == TASK_OK) {
        task_free(list);
        *list = candidate;
    } else {
        task_free(&candidate);
    }
    return status;
}
