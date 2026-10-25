#include <stdio.h>

/* TODO: 交换两个指针指向的 int，成功返回 1。
   任一指针为空返回 0；两个指针相同也必须正确。 */
static int swap_int(int *left, int *right)
{
    (void)left;
    (void)right;
    return 0;
}

static int check(int condition, const char *label)
{
    if (!condition) {
        printf("FAIL: %s\n", label);
        return 1;
    }
    return 0;
}

int main(void)
{
    int left = -3;
    int right = 8;
    int failed = 0;
    int ok = swap_int(&left, &right);
    failed += check(ok == 1 && left == 8 && right == -3, "ordinary swap");
    /* 恢复独立基线，避免首个失败干扰后面的边界测试。 */
    left = 8;
    right = -3;
    ok = swap_int(&left, &left);
    failed += check(ok == 1 && left == 8, "same pointer");
    failed += check(swap_int(NULL, &right) == 0 && right == -3, "null left");
    failed += check(swap_int(&left, NULL) == 0 && left == 8, "null right");
    if (failed != 0) {
        return 1;
    }
    puts("PASS: pointer swap");
    return 0;
}

