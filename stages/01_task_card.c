#include <stdio.h>

int main(void)
{
    /* 第一步：把程序里的一个值当成现实中的一项任务。 */
    const char title[] = "Learn C by building a task manager";
    int priority = 3;
    int done = 0;
    printf("Title: %s\nPriority: %d\nDone: %d\n", title, priority, done);
    /* 练习：将 done 改为 1，并在下面添加 if，输出 Done 或 Todo。 */
    return 0;
}
