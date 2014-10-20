/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file COPYING-BSD.
 */

#ifndef _BALDE_UTILS__STRING_UTILS_H
#define _BALDE_UTILS__STRING_UTILS_H

#include <stdbool.h>

#define B_STRING_CHUNK_SIZE 128

typedef struct _b_string_t {
    char *str;
    size_t len;
    size_t allocated_len;
} b_string_t;

char* b_strdup(const char *s);
char* b_strndup(const char *s, size_t n);
char* b_strdup_vprintf(const char *format, va_list ap);
char* b_strdup_printf(const char *format, ...);
bool b_str_starts_with(const char *str, const char *prefix);
bool b_str_ends_with(const char *str, const char *suffix);
char* b_str_strip(char *str);
char** b_str_split(const char *str, char c, unsigned int max_pieces);
char* b_str_replace(const char *str, const char search, const char *replace);
void b_strv_free(char **strv);
char* b_strv_join(const char **strv, const char *separator);
unsigned int b_strv_length(char **strv);

b_string_t* b_string_new(void);
char* b_string_free(b_string_t *str, bool free_str);
b_string_t* b_string_append_len(b_string_t *str, const char *suffix, size_t len);
b_string_t* b_string_append(b_string_t *str, const char *suffix);
b_string_t* b_string_append_c(b_string_t *str, char c);
b_string_t* b_string_append_printf(b_string_t *str, const char *format, ...);

#endif /* _BALDE_UTILS__STRING_UTILS_H */
