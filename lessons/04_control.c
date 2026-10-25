#include <stdio.h>

int main(void)
{
    int sum = 0;
    int countdown = 3;
    for (int i = 1; i <= 6; ++i) {
        if (i == 2) {
            continue;
        }
        if (i == 5) {
            break;
        }
        sum += i;
    }
    printf("sum=%d\n", sum);
    while (countdown > 0) {
        printf("%d ", countdown);
        --countdown;
    }
    puts("go");
    /* switch 不会自动终止一个 case，通常需要 break。 */
    switch (sum) {
        case 8: puts("matched eight"); break;
        default: puts("other"); break;
    }
    do {
        ++countdown;
    } while (countdown < 1);
    printf("do_while=%d\n", countdown);
    return 0;
}
