#include <stdio.h>

int main(void)
{
    int a = 7;
    int b = 2;
    unsigned int flags = 0U;
    int visits = 0;
    printf("division=%d remainder=%d real=%.1f\n", a / b, a % b, (double)a / b);
    flags |= 1U << 2U;
    printf("flag2=%u flag0=%u\n", (flags >> 2U) & 1U, flags & 1U);
    /* && 短路求值：左边为假时不执行右边。 */
    if (a < b && ++visits > 0) {
        puts("unreachable");
    }
    printf("visits=%d max=%d\n", visits, a > b ? a : b);
    return 0;
}
