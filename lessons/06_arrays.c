#include <stdio.h>

int main(void)
{
    int values[] = {3, 1, 4, 1, 5};
    size_t count = sizeof values / sizeof values[0];
    int sum = 0;
    int minimum = values[0];
    int grid[2][3] = {{1, 2, 3}, {4, 5, 6}};
    for (size_t i = 0; i < count; ++i) {
        sum += values[i];
        if (values[i] < minimum) {
            minimum = values[i];
        }
    }
    printf("count=%zu sum=%d min=%d\n", count, sum, minimum);
    printf("grid[1][2]=%d\n", grid[1][2]);
    /* 最后一个有效下标是 count - 1，不能读取 values[count]。 */
    return 0;
}
