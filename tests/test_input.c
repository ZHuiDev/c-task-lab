#include "input.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

static unsigned checks;
static unsigned failures;
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition); ++failures; } } while (0)

int main(void)
{
    FILE *stream;
    char buffer[5];
    char maximum[64];
    unsigned long value = 42;
    CHECK(input_ulong("0", 0, 9, &value) && value == 0);
    CHECK(input_ulong("003", 1, 3, &value) && value == 3);
    CHECK(!input_ulong("-1", 0, ULONG_MAX, &value));
    CHECK(!input_ulong("+1", 0, ULONG_MAX, &value));
    CHECK(!input_ulong(" 1", 0, ULONG_MAX, &value));
    CHECK(!input_ulong("1 ", 0, ULONG_MAX, &value));
    CHECK(!input_ulong("1a", 0, ULONG_MAX, &value));
    CHECK(!input_ulong("", 0, ULONG_MAX, &value));
    CHECK(!input_ulong("10", 0, 9, &value));
    CHECK(!input_ulong("1", 2, 1, &value));
    CHECK(!input_ulong(NULL, 0, 9, &value));
    CHECK(!input_ulong("1", 0, 9, NULL));
    CHECK(!input_ulong("99999999999999999999999999999999999999", 0, ULONG_MAX, &value));
    (void)snprintf(maximum, sizeof maximum, "%lu", ULONG_MAX);
    CHECK(input_ulong(maximum, 0, ULONG_MAX, &value) && value == ULONG_MAX);
    stream = tmpfile();
    CHECK(stream != NULL);
    if (stream == NULL) { return 1; }
    CHECK(fputs("abc\r\nabcd\r\n123456789\nOK\n\nlast", stream) >= 0);
    rewind(stream);
    CHECK(input_line(stream, buffer, sizeof buffer) == INPUT_OK && strcmp(buffer, "abc") == 0);
    CHECK(input_line(stream, buffer, sizeof buffer) == INPUT_OK && strcmp(buffer, "abcd") == 0);
    CHECK(input_line(stream, buffer, sizeof buffer) == INPUT_TOO_LONG);
    CHECK(input_line(stream, buffer, sizeof buffer) == INPUT_OK && strcmp(buffer, "OK") == 0);
    CHECK(input_line(stream, buffer, sizeof buffer) == INPUT_OK && strcmp(buffer, "") == 0);
    CHECK(input_line(stream, buffer, sizeof buffer) == INPUT_OK && strcmp(buffer, "last") == 0);
    CHECK(input_line(stream, buffer, sizeof buffer) == INPUT_EOF);
    CHECK(fclose(stream) == 0);
    stream = tmpfile();
    CHECK(stream != NULL);
    if (stream == NULL) { return 1; }
    CHECK(fputc('\0', stream) != EOF);
    CHECK(fputs("bad\nnext\n", stream) >= 0);
    rewind(stream);
    CHECK(input_line(stream, buffer, sizeof buffer) == INPUT_TOO_LONG);
    CHECK(input_line(stream, buffer, sizeof buffer) == INPUT_OK && strcmp(buffer, "next") == 0);
    CHECK(fclose(stream) == 0);
    CHECK(input_line(NULL, buffer, sizeof buffer) == INPUT_ERROR);
    printf("Input: %u checks, %u failures\n", checks, failures);
    return failures == 0u ? 0 : 1;
}
