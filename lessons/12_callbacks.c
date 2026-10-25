#include <stdio.h>
#include <stdlib.h>

typedef int (*Predicate)(int);

static int is_even(int value)
{
    return value % 2 == 0;
}

static size_t count_matching(const int *values, size_t count, Predicate predicate)
{
    size_t matches = 0;
    for (size_t i = 0; i < count; ++i) {
        if (predicate(values[i])) {
            ++matches;
        }
    }
    return matches;
}

static int compare_int(const void *left, const void *right)
{
    int a = *(const int *)left;
    int b = *(const int *)right;
    /* 不用 a - b，避免两个极值相减导致有符号溢出。 */
    return (a > b) - (a < b);
}

int main(void)
{
    int values[] = {5, 2, 4, 1, 3};
    size_t count = sizeof values / sizeof values[0];
    printf("even=%zu\n", count_matching(values, count, is_even));
    qsort(values, count, sizeof values[0], compare_int);
    for (size_t i = 0; i < count; ++i) {
        printf("%d%c", values[i], i + 1 == count ? '\n' : ' ');
    }
    return 0;
}
