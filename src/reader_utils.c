/* CPURenderer/src/reader_utils.c */
#include "reader_utils.h"
#include <stdlib.h>

enum { word_cap = 32 };

FILE *fopen_r(const char *path)
{
    return fopen(path, "r");
}

static int symbol_is_inline_space(int s)
{
    return s == ' ' || s == '\t';
}

static int symbol_is_linebreak(int s)
{
    return s == EOF || s == '\n' || s == '\r';
}

static int symbol_is_space_or_eof(int s)
{
    return symbol_is_inline_space(s) || symbol_is_linebreak(s);
}

static int read_word(FILE *f, char **out, int cap, int *break_c)
{
    int word_is_empty = 1;
    int c;
    char *wp;

    wp = *out;

    while (symbol_is_inline_space(c = getc(f)))
        {}

    while (!symbol_is_space_or_eof(c) && wp - *out < cap) {
        c = getc(f);

        if (word_is_empty)
            word_is_empty = 0;

        *wp = c;
        wp++;
    }

    *break_c = c;
    return !word_is_empty;
}

line *read_line(FILE *f)
{
    line *l;
    line_word *last;
    char word_content;
    int break_c;
    
    l = malloc(sizeof(line));
    l->words = NULL;
    last = NULL;

    while (read_word(f, &word_content, word_cap, &break_c)) {
        /* Add word cap exceeded treatment */

        if (!l->words) {
            l->words = malloc(sizeof(line_word));
            last = l->words;
        } else {
            line_word *tmp;
            tmp = malloc(sizeof(line_word));
            last->next = tmp;
            last = tmp;
        }

        last->content = word_content;

        if (symbol_is_linebreak(break_c))
            break;
    }

    l->first_char = l->words[0].content[0];

    return l;
}

void free_line(line *l)
{
    while (l->words) {
        line_word *tmp;
        tmp = l->words;
        l->words = l->words->next;
        free(tmp->content);
