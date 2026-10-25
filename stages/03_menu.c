#include <stdio.h>
#include <string.h>

int main(void)
{
    char command[32];
    int done = 0;
    /* 第三步：读一整行，再验证内容。菜单从固定的一项任务开始。 */
    for (;;) {
        puts("1 Show task | 2 Toggle done | 0 Quit");
        if (fgets(command, (int)sizeof command, stdin) == NULL) {
            if (ferror(stdin)) { return 1; }
            break;
        }
        if (strchr(command, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            puts("Input too long.");
            continue;
        }
        command[strcspn(command, "\r\n")] = '\0';
        if (strcmp(command, "0") == 0) { break; }
        if (strcmp(command, "1") == 0) {
            printf("[%c] Finish the menu stage\n", done ? 'x' : ' ');
        } else if (strcmp(command, "2") == 0) {
            done = !done;
        } else {
            puts("Choose 0, 1 or 2.");
        }
    }
    return 0;
}
