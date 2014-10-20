/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file COPYING-BSD.
 */

#ifndef _BALDE_UTILS__SLIST_H
#define _BALDE_UTILS__SLIST_H

typedef struct _b_slist_t {
    struct _b_slist_t *next;
    void *data;
} b_slist_t;

b_slist_t* b_slist_append(b_slist_t *l, void *data);
void b_slist_free_full(b_slist_t *l, void (*free_func)(void *ptr));
void b_slist_free(b_slist_t *l);
unsigned int b_slist_length(b_slist_t *l);

#endif /* _BALDE_UTILS__SLIST_H */
