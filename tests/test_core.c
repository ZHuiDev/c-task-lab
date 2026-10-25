#include "storage.h"
#include "task.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 不使用 assert：即使构建时定义 NDEBUG，测试也必须真的执行。 */
static unsigned failures = 0u;
static unsigned checks = 0u;

#define CHECK(condition) do { \
    ++checks; \
    if (!(condition)) { \
        ++failures; \
        (void)fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    } \
} while (0)

static FILE *stream_from_bytes(const char *bytes, size_t length)
{
    FILE *stream = tmpfile();

    CHECK(stream != NULL);
    if (stream != NULL) {
        if (fwrite(bytes, 1u, length, stream) != length ||
            fseek(stream, 0L, SEEK_SET) != 0) {
            CHECK(false);
            (void)fclose(stream);
            return NULL;
        }
    }
    return stream;
}

static TaskStatus read_text(const char *text, TaskList *list)
{
    FILE *stream = stream_from_bytes(text, strlen(text));
    TaskStatus status;

    if (stream == NULL) {
        return TASK_IO;
    }
    status = task_read(stream, list);
    CHECK(fclose(stream) == 0);
    return status;
}

static void test_add_find_remove(void)
{
    TaskList list;
    unsigned long first = 0ul;
    unsigned long second = 0ul;

    task_init(&list);
    CHECK(list.items == NULL && list.len == 0u && list.cap == 0u);
    CHECK(list.next_id == 1ul);
    CHECK(task_find(&list, 1ul) == NULL);
    CHECK(task_add(&list, "learn C", PRIORITY_NORMAL, &first) == TASK_OK);
    CHECK(task_add(&list, "write a test", PRIORITY_HIGH, &second) == TASK_OK);
    CHECK(first == 1ul && second == 2ul && list.len == 2u);
    CHECK(task_find(&list, first) != NULL);
    if (task_find(&list, first) != NULL) {
        CHECK(strcmp(task_find(&list, first)->title, "learn C") == 0);
        CHECK(task_find(&list, first)->flags == 0u);
    }
    CHECK(task_remove(&list, first) == TASK_OK);
    CHECK(list.len == 1u && list.items[0].id == second);
    CHECK(task_remove(&list, first) == TASK_NOT_FOUND);
    CHECK(task_add(&list, "new task", PRIORITY_LOW, &first) == TASK_OK);
    CHECK(first == 3ul);
    CHECK(task_remove(&list, second) == TASK_OK);
    CHECK(task_remove(&list, first) == TASK_OK);
    CHECK(list.len == 0u && list.next_id == 4ul);
    task_free(&list);
    CHECK(list.items == NULL && list.len == 0u && list.next_id == 1ul);
    task_free(&list);
}

static void test_title_and_arguments(void)
{
    TaskList list;
    char longest[TASK_TITLE_CAP];
    char too_long[TASK_TITLE_CAP + 1];
    unsigned long output = 777ul;
    const char utf8[] = "\xe5\xad\xa6\xe4\xb9\xa0 C";

    task_init(&list);
    (void)memset(longest, 'a', sizeof longest - 1u);
    longest[sizeof longest - 1u] = '\0';
    (void)memset(too_long, 'b', sizeof too_long - 1u);
    too_long[sizeof too_long - 1u] = '\0';
    CHECK(task_add(&list, longest, PRIORITY_LOW, NULL) == TASK_OK);
    CHECK(strlen(list.items[0].title) == TASK_TITLE_CAP - 1u);
    CHECK(task_add(&list, utf8, PRIORITY_NORMAL, NULL) == TASK_OK);
    CHECK(strcmp(list.items[1].title, utf8) == 0);
    CHECK(task_add(&list, too_long, PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(task_add(&list, "", PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(task_add(&list, NULL, PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(task_add(&list, "bad\ttitle", PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(task_add(&list, "bad\ntitle", PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(task_add(&list, "bad\rtitle", PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(task_add(&list, "bad\x7f", PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(task_add(&list, "bad\x01", PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(task_add(&list, "ok", (TaskPriority)0, &output) == TASK_INVALID);
    CHECK(task_add(&list, "ok", (TaskPriority)4, &output) == TASK_INVALID);
    CHECK(task_add(NULL, "ok", PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(list.len == 2u && list.next_id == 3ul && output == 777ul);
    CHECK(task_find(NULL, 1ul) == NULL);
    CHECK(task_find(&list, 0ul) == NULL);
    CHECK(task_remove(NULL, 1ul) == TASK_INVALID);
    CHECK(task_remove(&list, 0ul) == TASK_INVALID);
    CHECK(task_set_done(NULL, 1ul, true) == TASK_INVALID);
    CHECK(task_set_urgent(&list, 0ul, true) == TASK_INVALID);
    list.next_id = ULONG_MAX;
    CHECK(task_add(&list, "limit", PRIORITY_LOW, &output) == TASK_INVALID);
    CHECK(output == 777ul && list.len == 2u);
    list.next_id = ULONG_MAX - 1ul;
    CHECK(task_add(&list, "last ID", PRIORITY_LOW, &output) == TASK_OK);
    CHECK(output == ULONG_MAX - 1ul && list.next_id == ULONG_MAX);
    CHECK(task_add(&list, "overflow", PRIORITY_LOW, NULL) == TASK_INVALID);
    task_free(&list);
}

static void test_growth_and_alias(void)
{
    TaskList list;
    size_t index;
    char title[40];

    task_init(&list);
    for (index = 0u; index < 1000u; ++index) {
        int length = snprintf(title, sizeof title, "task %lu", (unsigned long)index);
        CHECK(length > 0 && (size_t)length < sizeof title);
        CHECK(task_add(&list, title, PRIORITY_NORMAL, NULL) == TASK_OK);
    }
    CHECK(list.len == 1000u && list.cap >= list.len && list.next_id == 1001ul);
    CHECK(task_find(&list, 1ul) != NULL && task_find(&list, 1000ul) != NULL);
    task_free(&list);

    task_init(&list);
    CHECK(task_add(&list, "borrowed title", PRIORITY_LOW, NULL) == TASK_OK);
    while (list.len < list.cap) {
        CHECK(task_add(&list, "padding", PRIORITY_LOW, NULL) == TASK_OK);
    }
    CHECK(task_add(&list, list.items[0].title, PRIORITY_NORMAL, NULL) == TASK_OK);
    CHECK(strcmp(list.items[list.len - 1u].title, "borrowed title") == 0);
    task_free(&list);
}

static void count_visitor(const Task *task, void *context)
{
    unsigned long *sum = context;
    *sum += task->id;
}

static void test_flags_sort_and_callback(void)
{
    TaskList list;
    unsigned long sum = 0ul;

    task_init(&list);
    CHECK(task_add(&list, "low", PRIORITY_LOW, NULL) == TASK_OK);
    CHECK(task_add(&list, "high first", PRIORITY_HIGH, NULL) == TASK_OK);
    CHECK(task_add(&list, "normal", PRIORITY_NORMAL, NULL) == TASK_OK);
    CHECK(task_add(&list, "high second", PRIORITY_HIGH, NULL) == TASK_OK);
    CHECK(task_set_done(&list, 2ul, true) == TASK_OK);
    CHECK(task_set_urgent(&list, 2ul, true) == TASK_OK);
    CHECK(task_find(&list, 2ul)->flags == 3u);
    CHECK(task_set_done(&list, 2ul, false) == TASK_OK);
    CHECK(task_find(&list, 2ul)->flags == TASK_FLAG_URGENT);
    CHECK(task_set_urgent(&list, 2ul, false) == TASK_OK);
    CHECK(task_find(&list, 2ul)->flags == 0u);
    CHECK(task_set_done(&list, 77ul, true) == TASK_NOT_FOUND);
    CHECK(task_set_urgent(&list, 77ul, true) == TASK_NOT_FOUND);
    task_sort_by_priority(&list);
    CHECK(list.items[0].id == 2ul && list.items[1].id == 4ul);
    CHECK(list.items[2].id == 3ul && list.items[3].id == 1ul);
    task_visit(&list, count_visitor, &sum);
    CHECK(sum == 10ul);
    task_visit(&list, NULL, NULL);
    task_visit(NULL, count_visitor, &sum);
    task_sort_by_priority(NULL);
    task_free(&list);

    /* 文件中相同优先级的编号倒序，排序后必须按编号递增。 */
    task_init(&list);
    CHECK(read_text("CTASK\t1\t100\n99\t3\t0\thigh\n1\t3\t0\tlow\n", &list) == TASK_OK);
    task_sort_by_priority(&list);
    CHECK(list.items[0].id == 1ul && list.items[1].id == 99ul);
    task_free(&list);
}

static void test_roundtrip_and_id_history(void)
{
    TaskList source;
    TaskList target;
    FILE *stream;
    unsigned long next;

    task_init(&source);
    task_init(&target);
    CHECK(task_add(&source, "keep", PRIORITY_HIGH, NULL) == TASK_OK);
    CHECK(task_add(&source, "deleted", PRIORITY_NORMAL, NULL) == TASK_OK);
    CHECK(task_add(&source, "also deleted", PRIORITY_LOW, NULL) == TASK_OK);
    CHECK(task_set_done(&source, 1ul, true) == TASK_OK);
    CHECK(task_set_urgent(&source, 1ul, true) == TASK_OK);
    CHECK(task_remove(&source, 3ul) == TASK_OK);
    CHECK(task_remove(&source, 2ul) == TASK_OK);
    CHECK(task_add(&target, "replaced", PRIORITY_LOW, NULL) == TASK_OK);
    stream = tmpfile();
    CHECK(stream != NULL);
    if (stream != NULL) {
        CHECK(task_write(stream, &source) == TASK_OK);
        CHECK(fflush(stream) == 0);
        CHECK(fseek(stream, 0L, SEEK_SET) == 0);
        CHECK(task_read(stream, &target) == TASK_OK);
        CHECK(fclose(stream) == 0);
        CHECK(target.len == 1u && target.next_id == 4ul);
        CHECK(target.items[0].id == 1ul && target.items[0].flags == 3u);
        CHECK(target.items[0].priority == PRIORITY_HIGH);
        CHECK(strcmp(target.items[0].title, "keep") == 0);
        CHECK(task_add(&target, "after reload", PRIORITY_LOW, &next) == TASK_OK);
        CHECK(next == 4ul);
    }
    task_free(&source);
    task_free(&target);
}

static void test_title_persistence(void)
{
    TaskList source;
    TaskList target;
    char longest[TASK_TITLE_CAP];
    const char utf8[] = "\xe5\xad\xa6\xe4\xb9\xa0 C";
    FILE *stream;

    task_init(&source);
    task_init(&target);
    (void)memset(longest, 'z', sizeof longest - 1u);
    longest[sizeof longest - 1u] = '\0';
    CHECK(task_add(&source, longest, PRIORITY_LOW, NULL) == TASK_OK);
    CHECK(task_add(&source, utf8, PRIORITY_NORMAL, NULL) == TASK_OK);
    stream = tmpfile();
    CHECK(stream != NULL);
    if (stream != NULL) {
        CHECK(task_write(stream, &source) == TASK_OK);
        CHECK(fseek(stream, 0L, SEEK_SET) == 0);
        CHECK(task_read(stream, &target) == TASK_OK);
        CHECK(target.len == 2u);
        if (target.len == 2u) {
            CHECK(strcmp(target.items[0].title, longest) == 0);
            CHECK(strcmp(target.items[1].title, utf8) == 0);
        }
        CHECK(fclose(stream) == 0);
    }
    task_free(&source);
    task_free(&target);
}

/* 对每种坏输入都检查指针和内容未变，验证读取的事务语义。 */
static void expect_format_failure(const char *text)
{
    TaskList list;
    Task *original_items;
    size_t original_capacity;

    task_init(&list);
    CHECK(task_add(&list, "preserved", PRIORITY_HIGH, NULL) == TASK_OK);
    original_items = list.items;
    original_capacity = list.cap;
    CHECK(read_text(text, &list) == TASK_FORMAT);
    CHECK(list.items == original_items && list.len == 1u);
    CHECK(list.cap == original_capacity && list.next_id == 2ul);
    CHECK(list.items[0].id == 1ul && strcmp(list.items[0].title, "preserved") == 0);
    task_free(&list);
}

static void test_malformed_inputs(void)
{
    static const char *const invalid[] = {
        "", "CTASK\t1\n", "CTASK\t2\t2\n", "CTASK\t01\t2\n",
        "CTASK\t1\t2", "CTASK\t1\t0\n", "CTASK\t1\t-1\n",
        "CTASK\t1\t+2\n", "CTASK\t1\t 2\n", "CTASK\t1\t2\textra\n",
        "CTASK\t1\t2\n\n", "CTASK\t1\t2\n1\t2\t0\ttitle",
        "CTASK\t1\t2\n1\t2\t0\t\n", "CTASK\t1\t2\n0\t2\t0\ttitle\n",
        "CTASK\t1\t2\n-1\t2\t0\ttitle\n", "CTASK\t1\t2\n+1\t2\t0\ttitle\n",
        "CTASK\t1\t2\n 1\t2\t0\ttitle\n", "CTASK\t1\t2\n1x\t2\t0\ttitle\n",
        "CTASK\t1\t2\n1\t0\t0\ttitle\n", "CTASK\t1\t2\n1\t4\t0\ttitle\n",
        "CTASK\t1\t2\n1\t2\t4\ttitle\n", "CTASK\t1\t2\n1\t2\t-1\ttitle\n",
        "CTASK\t1\t2\n1\t2\t0\ttitle\textra\n", "CTASK\t1\t2\n1\t2\t0\tbad\x7f\n",
        "CTASK\t1\t2\n1\t2\t0\tbad\rtitle\n", "CTASK\t1\t2\n1\t2\ttitle\n",
        "CTASK\t1\t2\n1\t2\t0\ttitle\n1\t1\t0\tduplicate\n",
        "CTASK\t1\t1\n1\t2\t0\ttitle\n", "CTASK\t1\t2\n2\t2\t0\ttitle\n"
    };
    size_t index;
    char text[1024];
    int length;

    for (index = 0u; index < sizeof invalid / sizeof invalid[0]; ++index) {
        expect_format_failure(invalid[index]);
    }
    /* 在 ULONG_MAX 的十进制表示后再加 0，保证跨平台溢出。 */
    length = snprintf(text, sizeof text, "CTASK\t1\t%lu0\n", ULONG_MAX);
    CHECK(length > 0 && (size_t)length < sizeof text);
    expect_format_failure(text);
    length = snprintf(text, sizeof text, "CTASK\t1\t%lu\n%lu\t2\t0\ttitle\n",
                      ULONG_MAX, ULONG_MAX);
    CHECK(length > 0 && (size_t)length < sizeof text);
    expect_format_failure(text);
    length = snprintf(text, sizeof text, "CTASK\t1\t2\n%lu0\t2\t0\ttitle\n", ULONG_MAX);
    CHECK(length > 0 && (size_t)length < sizeof text);
    expect_format_failure(text);
    (void)strcpy(text, "CTASK\t1\t2\n1\t2\t0\t");
    index = strlen(text);
    (void)memset(text + index, 'x', 96u);
    text[index + 96u] = '\n';
    text[index + 97u] = '\0';
    expect_format_failure(text);
    (void)memset(text + index, 'x', 900u);
    text[index + 900u] = '\n';
    text[index + 901u] = '\0';
    expect_format_failure(text);
}

static void test_empty_crlf_nul_and_limits(void)
{
    TaskList list;
    char text[200];
    FILE *stream;
    int length;
    const char embedded_nul[] = "CTASK\t1\t2\n1\t2\t0\tbad\0hidden\n";

    task_init(&list);
    CHECK(read_text("CTASK\t1\t17\n", &list) == TASK_OK);
    CHECK(list.len == 0u && list.next_id == 17ul);
    CHECK(read_text("CTASK\t1\t3\r\n2\t3\t1\tCRLF\r\n", &list) == TASK_OK);
    CHECK(list.len == 1u && list.items[0].id == 2ul && list.items[0].flags == 1u);
    stream = stream_from_bytes(embedded_nul, sizeof embedded_nul - 1u);
    if (stream != NULL) {
        CHECK(task_read(stream, &list) == TASK_FORMAT);
        CHECK(list.len == 1u && list.items[0].id == 2ul);
        CHECK(fclose(stream) == 0);
    }
    length = snprintf(text, sizeof text, "CTASK\t1\t%lu\n%lu\t3\t3\tlast\n",
                      ULONG_MAX, ULONG_MAX - 1ul);
    CHECK(length > 0 && (size_t)length < sizeof text);
    CHECK(read_text(text, &list) == TASK_OK);
    CHECK(list.next_id == ULONG_MAX && list.items[0].id == ULONG_MAX - 1ul);
    CHECK(task_add(&list, "overflow", PRIORITY_LOW, NULL) == TASK_INVALID);
    task_free(&list);
}

static void test_file_errors_and_validation(void)
{
    TaskList list;
    FILE *stream;

    task_init(&list);
    CHECK(task_add(&list, "unchanged", PRIORITY_LOW, NULL) == TASK_OK);
    CHECK(task_read(NULL, &list) == TASK_INVALID);
    CHECK(task_write(NULL, &list) == TASK_INVALID);
    CHECK(task_load("", &list) == TASK_INVALID);
    CHECK(task_save(NULL, &list) == TASK_INVALID);
    CHECK(task_load("ctask_missing_directory_73921/no-file.txt", &list) == TASK_IO);
    CHECK(task_save("ctask_missing_directory_73921/no-file.txt", &list) == TASK_IO);
    CHECK(list.len == 1u && strcmp(list.items[0].title, "unchanged") == 0);
    stream = tmpfile();
    CHECK(stream != NULL);
    if (stream != NULL) {
        list.items[0].flags = 4u;
        CHECK(task_write(stream, &list) == TASK_INVALID);
        list.items[0].flags = 0u;
        list.next_id = 1ul;
        CHECK(task_write(stream, &list) == TASK_INVALID);
        list.next_id = 2ul;
        CHECK(task_write(stream, &list) == TASK_OK);
        CHECK(fclose(stream) == 0);
    }
    CHECK(strcmp(task_status_string(TASK_OK), "ok") == 0);
    CHECK(strlen(task_status_string((TaskStatus)999)) != 0u);
    task_free(&list);
}

int main(void)
{
    test_add_find_remove();
    test_title_and_arguments();
    test_growth_and_alias();
    test_flags_sort_and_callback();
    test_roundtrip_and_id_history();
    test_title_persistence();
    test_malformed_inputs();
    test_empty_crlf_nul_and_limits();
    test_file_errors_and_validation();
    (void)printf("%u checks, %u failures\n", checks, failures);
    return failures == 0u ? EXIT_SUCCESS : EXIT_FAILURE;
}
