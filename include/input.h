#ifndef CTASK_INPUT_H
#define CTASK_INPUT_H

#include <stddef.h>
#include <stdio.h>

typedef enum { INPUT_OK, INPUT_EOF, INPUT_TOO_LONG, INPUT_ERROR } InputStatus;

/* 读完一整行。过长时丢弃余下字符，使下一次读取从新行开始。 */
InputStatus input_line(FILE *stream, char *buffer, size_t capacity);
/* 严格接受十进制非负整数；拒绝负号、空白、尾随垃圾和溢出。 */
int input_ulong(const char *text, unsigned long minimum,
                unsigned long maximum, unsigned long *value);

#endif
