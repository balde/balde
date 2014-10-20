/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file COPYING-BSD.
 */

#ifndef _BALDE_UTILS__TRIE_H
#define _BALDE_UTILS__TRIE_H

typedef struct _b_trie_node_t {
    char key;
    void *data;
    struct _b_trie_node_t *next, *child;
} b_trie_node_t;

typedef struct _b_trie_t {
    b_trie_node_t *root;
    void (*free_func)(void *ptr);
} b_trie_t;

b_trie_t* b_trie_new(void (*free_func)(void *ptr));
void b_trie_free(b_trie_t *trie);
void b_trie_insert(b_trie_t *trie, const char *key, void *data);
void* b_trie_lookup(b_trie_t *trie, const char *key);
unsigned int b_trie_size(b_trie_t *trie);
void b_trie_foreach(b_trie_t *trie, void (*func)(const char *key, void *data));

#endif /* _BALDE_UTILS__TRIE_H */
