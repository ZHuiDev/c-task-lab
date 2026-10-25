#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} IntVector;

/* TODO: 追加一个值，必要时扩容。成功返回 1。
   内存不足/容量溢出/空指针返回 0，保留原 vector。
   前提：vector 为 {NULL,0,0}，或符合 size <= capacity 且 data 有效。
   vector 具有 data 的唯一所有权。 */
static int vector_push(IntVector *vector, int value)
{
    (void)vector;
    (void)value;
    return 0;
}

static void vector_destroy(IntVector *vector)
{
    free(vector->data);
    *vector = (IntVector){0};
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
    IntVector vector = {0};
    int failed = 0;
    for (int i = 0; i < 20; ++i) {
        if (!vector_push(&vector, i)) {
            failed += check(0, "push");
            break;
        }
    }
    failed += check(vector.size == 20 && vector.capacity >= vector.size, "size/capacity");
    if (vector.data != NULL && vector.size == 20) {
        for (size_t i = 0; i < vector.size; ++i) {
            failed += check(vector.data[i] == (int)i, "values survive growth");
        }
    } else {
        failed += check(0, "valid backing storage");
    }
    failed += check(vector_push(NULL, 1) == 0, "null vector");
    vector_destroy(&vector);
    failed += check(vector.data == NULL && vector.size == 0 && vector.capacity == 0,
                    "destroy resets ownership");
    vector_destroy(&vector);
    if (failed != 0) {
        return 1;
    }
    puts("PASS: dynamic array");
    return 0;
}

