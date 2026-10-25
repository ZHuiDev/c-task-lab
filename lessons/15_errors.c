#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int parse_int(const char *text, int *result)
{
    char *end = NULL;
    if (text == NULL || result == NULL || *text == '\0') {
        return 0;
    }
    errno = 0; /* 只有结合返回值和 errno，才能识别范围错误。 */
    long value = strtol(text, &end, 10);
    if (end == text || *end != '\0' || errno == ERANGE ||
        value < INT_MIN || value > INT_MAX) {
        return 0;
    }
    *result = (int)value;
    return 1;
}

int main(void)
{
    const char *samples[] = {"42", "12x", "", "999999999999999999999999999999"};
    size_t count = sizeof samples / sizeof samples[0];
    for (size_t i = 0; i < count; ++i) {
        int value = -1;
        if (parse_int(samples[i], &value)) {
            printf("input[%zu]=%d\n", i, value);
        } else {
            printf("input[%zu]=invalid unchanged=%d\n", i, value);
        }
    }
    return 0;
}
