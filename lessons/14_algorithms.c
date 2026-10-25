#include <stdio.h>

/* 使用半开区间 [left, right)，空数组也适用。 */
static size_t lower_bound(const int *values, size_t count, int target)
{
    size_t left = 0;
    size_t right = count;
    while (left < right) {
        size_t middle = left + (right - left) / 2;
        if (values[middle] < target) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }
    return left;
}

static void insertion_sort(int *values, size_t count)
{
    for (size_t i = 1; i < count; ++i) {
        int value = values[i];
        size_t position = i;
        while (position > 0 && values[position - 1] > value) {
            values[position] = values[position - 1];
            --position;
        }
        values[position] = value;
    }
}

int main(void)
{
    int values[] = {4, 1, 3, 2, 3};
    size_t count = sizeof values / sizeof values[0];
    insertion_sort(values, count);
    for (size_t i = 0; i < count; ++i) {
        printf("%d%c", values[i], i + 1 == count ? '\n' : ' ');
    }
    printf("first_3=%zu insertion_9=%zu empty=%zu\n",
           lower_bound(values, count, 3), lower_bound(values, count, 9),
           lower_bound(NULL, 0, 3));
    return 0;
}
