#include "input.h"

#include <errno.h>
#include <stdlib.h>

InputStatus input_line(FILE *stream, char *buffer, size_t capacity)
{
    size_t length = 0;
    int ch;
    int too_long = 0;
    int extra_cr = 0;
    if (stream == NULL || buffer == NULL || capacity == 0) {
        return INPUT_ERROR;
    }
    /* fgetc 返回 int，才能同时表示全部 unsigned char 值和 EOF。 */
    while ((ch = fgetc(stream)) != '\n' && ch != EOF) {
        /* 容量恰好用满时，CRLF 中的 CR 不算正文。若后面还有正文，才溢出。 */
        if (extra_cr) {
            too_long = 1;
            extra_cr = 0;
        }
        if (ch == '\0') {
            too_long = 1; /* 字符串接口不能表示嵌入的 NUL。 */
        }
        if (length + 1 < capacity) {
            buffer[length++] = (char)ch;
        } else if (ch == '\r' && !too_long) {
            extra_cr = 1;
        } else {
            too_long = 1;
        }
    }
    buffer[length] = '\0';
    if (ferror(stream)) {
        return INPUT_ERROR;
    }
    if (ch == EOF && length == 0 && !too_long) {
        return INPUT_EOF;
    }
    if (!extra_cr && length > 0 && buffer[length - 1] == '\r') {
        buffer[length - 1] = '\0';
    }
    return too_long ? INPUT_TOO_LONG : INPUT_OK;
}

int input_ulong(const char *text, unsigned long minimum,
                unsigned long maximum, unsigned long *value)
{
    const unsigned char *cursor = (const unsigned char *)text;
    char *end;
    unsigned long parsed;
    if (text == NULL || value == NULL || *text == '\0' || minimum > maximum) {
        return 0;
    }
    while (*cursor != '\0') {
        if (*cursor < (unsigned char)'0' || *cursor > (unsigned char)'9') {
            return 0;
        }
        ++cursor;
    }
    errno = 0;
    parsed = strtoul(text, &end, 10);
    if (errno == ERANGE || *end != '\0' || parsed < minimum || parsed > maximum) {
        return 0;
    }
    *value = parsed;
    return 1;
}
