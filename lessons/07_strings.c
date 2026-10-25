#include <stdio.h>
#include <string.h>

int main(void)
{
    char text[] = "hello"; /* 可修改数组包含结尾的 '\0'。 */
    const char *language = "C"; /* 字符串字面量不能修改。 */
    char title[16];
    int written = snprintf(title, sizeof title, "%s lab", language);
    if (written < 0 || (size_t)written >= sizeof title) {
        fputs("format failed or truncated\n", stderr);
        return 1;
    }
    text[0] = 'H';
    printf("%s length=%zu storage=%zu\n", text, strlen(text), sizeof text);
    printf("title=%s equal=%d\n", title, strcmp(title, "C lab") == 0);
    printf("terminator=%d\n", text[strlen(text)] == '\0');
    return 0;
}
