#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    size_t count = 3;
    size_t expanded_count = 5;
    if (count > SIZE_MAX / sizeof(int) || expanded_count > SIZE_MAX / sizeof(int)) {
        return 1;
    }
    int *values = calloc(count, sizeof *values);
    if (values == NULL) {
        fputs("allocation failed\n", stderr);
        return 1;
    }
    for (size_t i = 0; i < count; ++i) {
        values[i] = (int)i + 1;
    }
    /* realloc 失败时原指针仍有效，不能直接覆盖它。 */
    int *replacement = realloc(values, expanded_count * sizeof *values);
    if (replacement == NULL) {
        free(values);
        fputs("resize failed\n", stderr);
        return 1;
    }
    values = replacement;
    for (size_t i = count; i < expanded_count; ++i) {
        values[i] = (int)i + 1;
    }
    int sum = 0;
    for (size_t i = 0; i < expanded_count; ++i) {
        sum += values[i];
    }
    printf("count=%zu sum=%d\n", expanded_count, sum);
    free(values);
    values = NULL;
    free(values); /* free(NULL) 合法；其他重复释放不合法。 */
    return 0;
}
