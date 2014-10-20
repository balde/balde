/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file COPYING-BSD.
 */

#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <balde-utils/string_utils.h>


char*
b_strdup(const char *s)
{
    if (s == NULL)
        return NULL;
    size_t l = strlen(s);
    char *tmp = malloc(l + 1);
    if (tmp == NULL)
        return NULL;
    memcpy(tmp, s, l + 1);
    return tmp;
}


char*
b_strndup(const char *s, size_t n)
{
    if (s == NULL)
        return NULL;
    size_t l = strnlen(s, n);
    char *tmp = malloc(l + 1);
    if (tmp == NULL)
        return NULL;
    memcpy(tmp, s, l);
    tmp[l] = '\0';
    return tmp;
}


char*
b_strdup_vprintf(const char *format, va_list ap)
{
    va_list ap2;
    va_copy(ap2, ap);
    int l = vsnprintf(NULL, 0, format, ap2);
    va_end(ap2);
    if (l < 0)
        return NULL;
    char *tmp = malloc(l + 1);
    if (!tmp)
        return NULL;
    int l2 = vsnprintf(tmp, l + 1, format, ap);
    if (l2 < 0)
        return NULL;
    return tmp;
}


char*
b_strdup_printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    char *tmp = b_strdup_vprintf(format, ap);
    va_end(ap);
    return tmp;
}


bool
b_str_starts_with(const char *str, const char *prefix)
{
    int str_l = strlen(str);
    int str_lp = strlen(prefix);
    if (str_lp > str_l)
        return false;
    return strncmp(str, prefix, str_lp) == 0;
}


bool
b_str_ends_with(const char *str, const char *suffix)
{
    int str_l = strlen(str);
    int str_ls = strlen(suffix);
    if (str_ls > str_l)
        return false;
    return strcmp(str + str_l - str_ls, suffix) == 0;
}


char*
b_str_strip(char *str)
{
    if (str == NULL)
        return str;
    int i;
    size_t str_len = strlen(str);
    for (i = str_len - 1; i >= 0; i--) {
        if (!isspace(str[i])) {
            str[i + 1] = '\0';
            break;
        }
    }
    str_len = strlen(str);
    for (i = 0; i < str_len; i++) {
        if (!isspace(str[i])) {
            str = str + i;
            break;
        }
    }
    return str;
}


char**
b_str_split(const char *str, char c, unsigned int max_pieces)
{
    if (!str)
        return NULL;
    char **rv = malloc(sizeof(char*));
    unsigned int i, start = 0, count = 0;
    for (i = 0; i < strlen(str) + 1; i++) {
        if (str[0] == '\0')
            break;
        if ((str[i] == c && (!max_pieces || count + 1 < max_pieces)) || str[i] == '\0') {
            rv = realloc(rv, (count + 1) * sizeof(char*));
            rv[count] = malloc(i - start + 1);
            memcpy(rv[count], str + start, i - start);
            rv[count++][i - start] = '\0';
            start = i + 1;
        }
    }
    rv = realloc(rv, (count + 1) * sizeof(char*));
    rv[count] = NULL;
    return rv;
}


char*
b_str_replace(const char *str, const char search, const char *replace)
{
    char **pieces = b_str_split(str, search, 0);
    if (pieces == NULL)
        return NULL;
    char* rv = b_strv_join((const char**) pieces, replace);
    b_strv_free(pieces);
    return rv;
}


void
b_strv_free(char **strv)
{
    if (strv == NULL)
        return;
    unsigned int i;
    for (i = 0; strv[i] != NULL; i++)
        free(strv[i]);
    free(strv);
}


char*
b_strv_join(const char **strv, const char *separator)
{
    if (strv == NULL)
        return NULL;
    unsigned int i = 0;
    b_string_t *str = b_string_new();
    for (i = 0; strv[i] != NULL; i++) {
        str = b_string_append(str, strv[i]);
        if (strv[i+1] != NULL)
            str = b_string_append(str, separator);
    }
    return b_string_free(str, false);
}


unsigned int
b_strv_length(char **strv)
{
    if (!strv)
        return 0;
    unsigned int i;
    for (i = 0; strv[i] != NULL; i++);
    return i;
}


b_string_t*
b_string_new(void)
{
    b_string_t* rv = malloc(sizeof(b_string_t));
    rv->str = NULL;
    rv->len = 0;
    rv->allocated_len = 0;

    // initialize with empty string
    rv = b_string_append(rv, "");

    return rv;
}


char*
b_string_free(b_string_t *str, bool free_str)
{
    char *rv = NULL;
    if (free_str)
        free(str->str);
    else
        rv = str->str;
    free(str);
    return rv;
}


b_string_t*
b_string_append_len(b_string_t *str, const char *suffix, size_t len)
{
    if (suffix == NULL)
        return str;
    size_t old_len = str->len;
    str->len += len;
    if (str->len + 1 > str->allocated_len) {
        str->allocated_len = (((str->len + 1) / B_STRING_CHUNK_SIZE) + 1) * B_STRING_CHUNK_SIZE;
        str->str = realloc(str->str, str->allocated_len);
    }
    memcpy(str->str + old_len, suffix, len);
    str->str[str->len] = '\0';
    return str;
}


b_string_t*
b_string_append(b_string_t *str, const char *suffix)
{
    if (suffix == NULL)
        return str;
    return b_string_append_len(str, suffix, strlen(suffix));
}


b_string_t*
b_string_append_c(b_string_t *str, char c)
{
    size_t old_len = str->len;
    str->len += 1;
    if (str->len + 1 > str->allocated_len) {
        str->allocated_len = (((str->len + 1) / B_STRING_CHUNK_SIZE) + 1) * B_STRING_CHUNK_SIZE;
        str->str = realloc(str->str, str->allocated_len);
    }
    str->str[old_len] = c;
    str->str[str->len] = '\0';
    return str;
}


b_string_t*
b_string_append_printf(b_string_t *str, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    char *tmp = b_strdup_vprintf(format, ap);
    va_end(ap);
    str = b_string_append(str, tmp);
    free(tmp);
    return str;
}
