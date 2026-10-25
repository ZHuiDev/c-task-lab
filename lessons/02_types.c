#include <stdbool.h>
#include <stdio.h>

int main(void)
{
    int integer = 42;
    unsigned int count = 7U;
    double ratio = 2.5;
    char grade = 'A';
    bool ready = true;
    /* sizeof 的结果类型是 size_t，使用 %zu。sizeof(char) 按定义为 1。 */
    printf("integer=%d count=%u ratio=%.1f\n", integer, count, ratio);
    printf("grade=%c ready=%d char_size=%zu\n", grade, ready, sizeof(char));
    /* 转为 int 时丢掉小数部分；不假设 int 的具体字节数。 */
    printf("converted=%d\n", (int)ratio);
    return 0;
}
