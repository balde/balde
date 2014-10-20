/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file COPYING-BSD.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <balde-utils/string_utils.h>
#include <balde-utils/trie.h>


b_trie_t*
b_trie_new(void (*free_func)(void *ptr))
{
    b_trie_t *trie = malloc(sizeof(b_trie_t));
    trie->root = NULL;
    trie->free_func = free_func;
    return trie;
}


static void
b_trie_free_node(b_trie_t *trie, b_trie_node_t *node)
{
    if (node == NULL)
        return;
    if (node->data != NULL && trie->free_func != NULL)
        trie->free_func(node->data);
    b_trie_free_node(trie, node->next);
    b_trie_free_node(trie, node->child);
    free(node);
}


void
b_trie_free(b_trie_t *trie)
{
    b_trie_free_node(trie, trie->root);
    free(trie);
}


void
b_trie_insert(b_trie_t *trie, const char *key, void *data)
{
    if (data == NULL || key == NULL)
        return;

    b_trie_node_t *parent = NULL;
    b_trie_node_t *previous;
    b_trie_node_t *current;
    b_trie_node_t *tmp;

    while (1) {

        if (trie->root == NULL || (parent != NULL && parent->child == NULL)) {
            current = malloc(sizeof(b_trie_node_t));
            current->key = *key;
            current->data = NULL;
            current->next = NULL;
            current->child = NULL;
            if (trie->root == NULL)
                trie->root = current;
            else
                parent->child = current;
            parent = current;
            goto clean;
        }

        tmp = parent == NULL ? trie->root : parent->child;
        previous = NULL;

        while (tmp != NULL && tmp->key != *key) {
            previous = tmp;
            tmp = tmp->next;
        }

        parent = tmp;

        if (previous == NULL || parent != NULL)
            goto clean;

        current = malloc(sizeof(b_trie_node_t));
        current->key = *key;
        current->data = NULL;
        current->next = NULL;
        current->child = NULL;
        previous->next = current;
        parent = current;

clean:
        if (*key == '\0') {
            parent->data = data;
            break;
        }
        key++;
    }
}


void*
b_trie_lookup(b_trie_t *trie, const char *key)
{
    if (trie->root == NULL || key == NULL)
        return NULL;

    b_trie_node_t *parent = trie->root;
    b_trie_node_t *tmp;
    while (1) {
        for (tmp = parent; tmp != NULL; tmp = tmp->next) {

            if (tmp->key == *key) {
                if (tmp->key == '\0')
                    return tmp->data;
                parent = tmp->child;
                break;
            }
        }
        if (tmp == NULL)
            return NULL;

        if (*key == '\0')
            break;
        key++;
    }
    return NULL;
}


static void
b_trie_size_node(b_trie_node_t *node, unsigned int *count)
{
    if (node == NULL)
        return;

    if (node->key == '\0')
        (*count)++;

    b_trie_size_node(node->next, count);
    b_trie_size_node(node->child, count);
}


unsigned int
b_trie_size(b_trie_t *trie)
{
    if (trie == NULL)
        return 0;

    unsigned int count = 0;
    b_trie_size_node(trie->root, &count);
    return count;
}


static void
b_trie_foreach_node(b_trie_node_t *node, b_string_t *str, void (*func)(const char *key, void *data))
{
    if (node == NULL)
        return;

    if (node->key == '\0') {
        func(str->str, node->data);
        b_string_free(str, true);
    }

    if (node->child != NULL) {
        b_string_t *child = b_string_new();
        child = b_string_append(child, str->str);
        child = b_string_append_c(child, node->key);
        b_trie_foreach_node(node->child, child, func);
    }

    if (node->next != NULL)
        b_trie_foreach_node(node->next, str, func);

    if (node->child != NULL && node->next == NULL)
        b_string_free(str, true);
}


void
b_trie_foreach(b_trie_t *trie, void (*func)(const char *key, void *data))
{
    if (trie->root == NULL)
        return;

    b_string_t *str = b_string_new();
    b_trie_foreach_node(trie->root, str, func);
}
