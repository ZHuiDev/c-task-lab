#include <limits.h>
#include <stdio.h>

typedef struct {
    long long sum;
    int minimum;
    int maximum;
} Statistics;

/* TODO: 非空数组计算 sum/minimum/maximum，成功返回 1。
   参数为空、数组为空或总和超出 long long 范围时返回 0，不改动 *result。
   调用者保证 values 指向至少 count 个有效 int。 */
static int array_stats(const int *values, size_t count, Statistics *result)
{
    (void)values;
    (void)count;
    (void)result;
    return 0;
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
    int values[] = {3, -2, 8, 1};
    int single[] = {-7};
    Statistics result = {.sum = 99, .minimum = 99, .maximum = 99};
    int failed = 0;
    int ok = array_stats(values, sizeof values / sizeof values[0], &result);
    failed += check(ok == 1 && result.sum == 10 && result.minimum == -2 &&
                    result.maximum == 8, "mixed array");
    ok = array_stats(single, 1, &result);
    failed += check(ok == 1 && result.sum == -7 && result.minimum == -7 &&
                    result.maximum == -7, "single element");
    result = (Statistics){.sum = 99, .minimum = 99, .maximum = 99};
    failed += check(array_stats(values, 0, &result) == 0 &&
                    result.sum == 99 && result.minimum == 99 && result.maximum == 99,
                    "empty input leaves output unchanged");
    failed += check(array_stats(NULL, 1, &result) == 0, "null input");
    failed += check(array_stats(values, 4, NULL) == 0, "null output");
    if (failed != 0) {
        return 1;
    }
    puts("PASS: array statistics");
    return 0;
}

