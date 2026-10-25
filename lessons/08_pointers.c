#include <stdio.h>

static int swap(int *left, int *right)
{
    if (left == NULL || right == NULL) {
        return 0;
    }
    int temporary = *left;
    *left = *right;
    *right = temporary;
    return 1;
}

int main(void)
{
    int left = 3;
    int right = 8;
    int values[] = {10, 20, 30};
    const int *cursor = values;
    const int *end = values + 3; /* 可形成尾后指针，不能解引用它。 */
    int sum = 0;
    if (!swap(&left, &right)) {
        return 1;
    }
    while (cursor != end) {
        sum += *cursor;
        ++cursor;
    }
    printf("left=%d right=%d\n", left, right);
    printf("sum=%d distance=%td\n", sum, end - values);
    printf("null accepted: %d\n", swap(NULL, &right));
    return 0;
}
