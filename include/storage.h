#ifndef CTASK_STORAGE_H
#define CTASK_STORAGE_H

#include <stdio.h>
#include "task.h"

/* 格式：CTASK<TAB>1<TAB>next_id<LF>，随后每行：
 * id<TAB>priority<TAB>flags<TAB>title<LF>。
 * 读取接受 LF / CRLF，每行必须有换行；数字只能由 ASCII 数字组成。
 * id 范围是 1..ULONG_MAX-1；next_id > 所有现有 id，且不为 0。
 * 保存 next_id 使删除后再加载也不会重用已发出的编号。
 * FILE* 版本不会关闭调用方的流。读取失败不改变原有列表。
 * write 检查写入错误，但调用方仍负责检查 fflush / fclose 的延迟错误。 */
TaskStatus task_write(FILE *stream, const TaskList *list);
TaskStatus task_read(FILE *stream, TaskList *list);

/* save 直接覆盖文件，不是原子保存；失败时目标文件可能只写了一部分。
 * save/load 自己打开并关闭文件，并检查关闭时的错误。 */
TaskStatus task_save(const char *path, const TaskList *list);
TaskStatus task_load(const char *path, TaskList *list);

#endif
