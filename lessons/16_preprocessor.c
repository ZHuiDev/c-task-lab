#include <limits.h>
#include <stdio.h>

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))
#define TYPE_NAME(value) _Generic((value), int: "int", double: "double", default: "other")
#ifndef LAB_FEATURE
#define LAB_FEATURE 1
#endif

_Static_assert(CHAR_BIT >= 8, "C requires at least 8 bits per byte");
_Static_assert(sizeof(char) == 1, "sizeof(char) is always 1");

static inline int square(int value)
{
    /* 对可能有副作用的参数，优先函数，避免 SQUARE(i++) 宏重复求值。 */
    return value * value; /* 本例调用者保证 |value| <= 10。 */
}

int main(void)
{
    int values[] = {1, 2, 3};
    printf("length=%zu type=%s real=%s\n", ARRAY_LENGTH(values), TYPE_NAME(1), TYPE_NAME(1.0));
#if LAB_FEATURE
    puts("feature=on");
#else
    puts("feature=off");
#endif
    printf("square=%d\n", square(4));
    return 0;
}
