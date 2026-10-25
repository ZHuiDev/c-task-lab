#include <stdio.h>

int main(void)
{
    /* tmpfile 创建临时二进制更新流；关闭后文件自动移除。 */
    FILE *stream = tmpfile();
    if (stream == NULL) {
        perror("tmpfile");
        return 1;
    }
    if (fputs("alpha\nbeta\n", stream) == EOF || fflush(stream) == EOF ||
        fseek(stream, 0L, SEEK_SET) != 0) {
        perror("write or seek");
        (void)fclose(stream);
        return 1;
    }
    char line[32];
    unsigned int lines = 0U;
    while (fgets(line, sizeof line, stream) != NULL) {
        printf("line: %s", line);
        ++lines;
    }
    if (ferror(stream)) {
        fputs("read failed\n", stderr);
        (void)fclose(stream);
        return 1;
    }
    printf("lines=%u eof=%d\n", lines, feof(stream) != 0);
    if (fclose(stream) == EOF) {
        perror("fclose");
        return 1;
    }
    return 0;
}
