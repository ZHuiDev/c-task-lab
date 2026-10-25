#include <stdio.h>
#include <string.h>

/* 参考答案: 复制完整字符串（包含 '\0'），成功返回 1。
   容量不足或空指针返回 0，不修改 destination。
   调用者保证 source 是有效字符串、两个缓冲区不重叠。 */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    if (destination == NULL || source == NULL || capacity == 0) {
        return 0;
    }
    size_t length = strlen(source);
    if (length >= capacity) {
        return 0;
    }
    memcpy(destination, source, length + 1);
    return 1;
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
    char exact[5] = "xxxx";
    char small[3] = "ok";
    char empty[1] = {'x'};
    int failed = 0;
    int ok = copy_text(exact, sizeof exact, "task");
    failed += check(ok == 1 && memcmp(exact, "task", sizeof exact) == 0, "exact fit");
    ok = copy_text(small, sizeof small, "task");
    failed += check(ok == 0 && memcmp(small, "ok", sizeof small) == 0,
                    "short buffer unchanged");
    ok = copy_text(empty, sizeof empty, "");
    failed += check(ok == 1 && empty[0] == '\0', "empty string");
    failed += check(copy_text(NULL, 0, "task") == 0, "null destination");
    failed += check(copy_text(small, sizeof small, NULL) == 0, "null source");
    small[0] = 'o';
    failed += check(copy_text(small, 0, "") == 0 && small[0] == 'o', "zero capacity");
    if (failed != 0) {
        return 1;
    }
    puts("PASS: string copy");
    return 0;
}

