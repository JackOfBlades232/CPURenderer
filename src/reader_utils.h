/* CPURenderer/src/reader_utils.h */
#ifndef READER_UTILS_SENTRY
#define READER_UTILS_SENTRY

#include <stdio.h>

typedef struct tag_line_word {
    char *content;
    struct tag_line_word *next;
} line_word;

typedef struct tag_line {
    char first_char;
    line_word *words;
} line;

FILE *fopen_r(const char *path);
line *read_line(FILE *f);
void free_line(line *l);

#endif
