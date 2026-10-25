#include <stdio.h>

/* 参考答案: 把摄氏温度转换为华氏温度，使用浮点数运算。 */
static double celsius_to_fahrenheit(double celsius)
{
    return celsius * 9.0 / 5.0 + 32.0;
}

static int check(double input, double expected)
{
    double actual = celsius_to_fahrenheit(input);
    double difference = actual - expected;
    if (!(difference >= -0.000001 && difference <= 0.000001)) {
        printf("FAIL: C=%.1f expected=%.1f actual=%.1f\n", input, expected, actual);
        return 1;
    }
    return 0;
}

int main(void)
{
    int failed = 0;
    failed += check(0.0, 32.0);
    failed += check(100.0, 212.0);
    failed += check(-40.0, -40.0);
    if (failed != 0) {
        printf("%d check(s) failed. Complete the TODO.\n", failed);
        return 1;
    }
    puts("PASS: temperature");
    return 0;
}

