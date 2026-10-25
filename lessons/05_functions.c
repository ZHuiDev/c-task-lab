#include <stdio.h>

/* 只由检查过参数的 factorial 调用，n <= 12，不会乘法溢出。 */
static unsigned long factorial_recursive(unsigned int n)
{
    return n <= 1U ? 1UL : (unsigned long)n * factorial_recursive(n - 1U);
}

static int factorial(unsigned int n, unsigned long *result)
{
    if (result == NULL || n > 12U) {
        return 0;
    }
    *result = factorial_recursive(n);
    return 1;
}

int main(void)
{
    unsigned long result = 0UL;
    if (!factorial(6U, &result)) {
        return 1;
    }
    printf("6! = %lu\n", result);
    if (!factorial(0U, &result)) {
        return 1;
    }
    printf("0! = %lu\n", result);
    printf("13 accepted: %d\n", factorial(13U, &result));
    return 0;
}
