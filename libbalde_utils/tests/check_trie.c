/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file COPYING-BSD.
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdlib.h>

#include <balde-utils/string_utils.h>
#include <balde-utils/trie.h>


static void
test_new(void **state)
{
    b_trie_t *trie = b_trie_new(free);
    assert_non_null(trie);
    assert_null(trie->root);
    assert_true(trie->free_func == free);
    b_trie_free(trie);
}


static void
test_insert(void **state)
{
    b_trie_t *trie = b_trie_new(free);

    b_trie_insert(trie, "bola", b_strdup("guda"));
    assert_true(trie->root->key == 'b');
    assert_null(trie->root->data);
    assert_true(trie->root->child->key == 'o');
    assert_null(trie->root->child->data);
    assert_true(trie->root->child->child->key == 'l');
    assert_null(trie->root->child->child->data);
    assert_true(trie->root->child->child->child->key == 'a');
    assert_null(trie->root->child->child->child->data);
    assert_true(trie->root->child->child->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->child->child->data, "guda");


    b_trie_insert(trie, "chu", b_strdup("nda"));
    assert_true(trie->root->key == 'b');
    assert_null(trie->root->data);
    assert_true(trie->root->child->key == 'o');
    assert_null(trie->root->child->data);
    assert_true(trie->root->child->child->key == 'l');
    assert_null(trie->root->child->child->data);
    assert_true(trie->root->child->child->child->key == 'a');
    assert_null(trie->root->child->child->child->data);
    assert_true(trie->root->child->child->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->child->child->data, "guda");

    assert_true(trie->root->next->key == 'c');
    assert_null(trie->root->next->data);
    assert_true(trie->root->next->child->key == 'h');
    assert_null(trie->root->next->child->data);
    assert_true(trie->root->next->child->child->key == 'u');
    assert_null(trie->root->next->child->child->data);
    assert_true(trie->root->next->child->child->child->key == '\0');
    assert_string_equal(trie->root->next->child->child->child->data, "nda");


    b_trie_insert(trie, "bote", b_strdup("aba"));
    assert_true(trie->root->key == 'b');
    assert_null(trie->root->data);
    assert_true(trie->root->child->key == 'o');
    assert_null(trie->root->child->data);
    assert_true(trie->root->child->child->key == 'l');
    assert_null(trie->root->child->child->data);
    assert_true(trie->root->child->child->child->key == 'a');
    assert_null(trie->root->child->child->child->data);
    assert_true(trie->root->child->child->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->child->child->data, "guda");

    assert_true(trie->root->next->key == 'c');
    assert_null(trie->root->next->data);
    assert_true(trie->root->next->child->key == 'h');
    assert_null(trie->root->next->child->data);
    assert_true(trie->root->next->child->child->key == 'u');
    assert_null(trie->root->next->child->child->data);
    assert_true(trie->root->next->child->child->child->key == '\0');
    assert_string_equal(trie->root->next->child->child->child->data, "nda");

    assert_true(trie->root->child->child->next->key == 't');
    assert_null(trie->root->child->child->next->data);
    assert_true(trie->root->child->child->next->child->key == 'e');
    assert_null(trie->root->child->child->next->child->data);
    assert_true(trie->root->child->child->next->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->next->child->child->data, "aba");


    b_trie_insert(trie, "bo", b_strdup("haha"));
    assert_true(trie->root->key == 'b');
    assert_null(trie->root->data);
    assert_true(trie->root->child->key == 'o');
    assert_null(trie->root->child->data);
    assert_true(trie->root->child->child->key == 'l');
    assert_null(trie->root->child->child->data);
    assert_true(trie->root->child->child->child->key == 'a');
    assert_null(trie->root->child->child->child->data);
    assert_true(trie->root->child->child->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->child->child->data, "guda");

    assert_true(trie->root->next->key == 'c');
    assert_null(trie->root->next->data);
    assert_true(trie->root->next->child->key == 'h');
    assert_null(trie->root->next->child->data);
    assert_true(trie->root->next->child->child->key == 'u');
    assert_null(trie->root->next->child->child->data);
    assert_true(trie->root->next->child->child->child->key == '\0');
    assert_string_equal(trie->root->next->child->child->child->data, "nda");

    assert_true(trie->root->child->child->next->key == 't');
    assert_null(trie->root->child->child->next->data);
    assert_true(trie->root->child->child->next->child->key == 'e');
    assert_null(trie->root->child->child->next->child->data);
    assert_true(trie->root->child->child->next->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->next->child->child->data, "aba");

    assert_true(trie->root->child->child->next->next->key == '\0');
    assert_string_equal(trie->root->child->child->next->next->data, "haha");

    b_trie_free(trie);


    trie = b_trie_new(free);

    b_trie_insert(trie, "chu", b_strdup("nda"));
    assert_true(trie->root->key == 'c');
    assert_null(trie->root->data);
    assert_true(trie->root->child->key == 'h');
    assert_null(trie->root->child->data);
    assert_true(trie->root->child->child->key == 'u');
    assert_null(trie->root->child->child->data);
    assert_true(trie->root->child->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->child->data, "nda");


    b_trie_insert(trie, "bola", b_strdup("guda"));
    assert_true(trie->root->key == 'c');
    assert_null(trie->root->data);
    assert_true(trie->root->child->key == 'h');
    assert_null(trie->root->child->data);
    assert_true(trie->root->child->child->key == 'u');
    assert_null(trie->root->child->child->data);
    assert_true(trie->root->child->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->child->data, "nda");

    assert_true(trie->root->next->key == 'b');
    assert_null(trie->root->next->data);
    assert_true(trie->root->next->child->key == 'o');
    assert_null(trie->root->next->child->data);
    assert_true(trie->root->next->child->child->key == 'l');
    assert_null(trie->root->next->child->child->data);
    assert_true(trie->root->next->child->child->child->key == 'a');
    assert_null(trie->root->next->child->child->child->data);
    assert_true(trie->root->next->child->child->child->child->key == '\0');
    assert_string_equal(trie->root->next->child->child->child->child->data, "guda");


    b_trie_insert(trie, "bote", b_strdup("aba"));
    assert_true(trie->root->key == 'c');
    assert_null(trie->root->data);
    assert_true(trie->root->child->key == 'h');
    assert_null(trie->root->child->data);
    assert_true(trie->root->child->child->key == 'u');
    assert_null(trie->root->child->child->data);
    assert_true(trie->root->child->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->child->data, "nda");

    assert_true(trie->root->next->key == 'b');
    assert_null(trie->root->next->data);
    assert_true(trie->root->next->child->key == 'o');
    assert_null(trie->root->next->child->data);
    assert_true(trie->root->next->child->child->key == 'l');
    assert_null(trie->root->next->child->child->data);
    assert_true(trie->root->next->child->child->child->key == 'a');
    assert_null(trie->root->next->child->child->child->data);
    assert_true(trie->root->next->child->child->child->child->key == '\0');
    assert_string_equal(trie->root->next->child->child->child->child->data, "guda");

    assert_true(trie->root->next->child->child->next->key == 't');
    assert_null(trie->root->next->child->child->next->data);
    assert_true(trie->root->next->child->child->next->child->key == 'e');
    assert_null(trie->root->next->child->child->next->child->data);
    assert_true(trie->root->next->child->child->next->child->child->key == '\0');
    assert_string_equal(trie->root->next->child->child->next->child->child->data, "aba");


    b_trie_insert(trie, "bo", b_strdup("haha"));
    assert_true(trie->root->key == 'c');
    assert_null(trie->root->data);
    assert_true(trie->root->child->key == 'h');
    assert_null(trie->root->child->data);
    assert_true(trie->root->child->child->key == 'u');
    assert_null(trie->root->child->child->data);
    assert_true(trie->root->child->child->child->key == '\0');
    assert_string_equal(trie->root->child->child->child->data, "nda");

    assert_true(trie->root->next->key == 'b');
    assert_null(trie->root->next->data);
    assert_true(trie->root->next->child->key == 'o');
    assert_null(trie->root->next->child->data);
    assert_true(trie->root->next->child->child->key == 'l');
    assert_null(trie->root->next->child->child->data);
    assert_true(trie->root->next->child->child->child->key == 'a');
    assert_null(trie->root->next->child->child->child->data);
    assert_true(trie->root->next->child->child->child->child->key == '\0');
    assert_string_equal(trie->root->next->child->child->child->child->data, "guda");

    assert_true(trie->root->next->child->child->next->key == 't');
    assert_null(trie->root->next->child->child->next->data);
    assert_true(trie->root->next->child->child->next->child->key == 'e');
    assert_null(trie->root->next->child->child->next->child->data);
    assert_true(trie->root->next->child->child->next->child->child->key == '\0');
    assert_string_equal(trie->root->next->child->child->next->child->child->data, "aba");

    assert_true(trie->root->next->child->child->next->next->key == '\0');
    assert_string_equal(trie->root->next->child->child->next->next->data, "haha");

    b_trie_free(trie);
}


static void
test_keep_data(void **state)
{
    b_trie_t *trie = b_trie_new(NULL);

    char *t1 = "guda";
    char *t2 = "nda";
    char *t3 = "aba";
    char *t4 = "haha";

    b_trie_insert(trie, "bola", t1);
    b_trie_insert(trie, "chu", t2);
    b_trie_insert(trie, "bote", t3);
    b_trie_insert(trie, "bo", t4);

    b_trie_free(trie);

    assert_string_equal(t1, "guda");
    assert_string_equal(t2, "nda");
    assert_string_equal(t3, "aba");
    assert_string_equal(t4, "haha");
}


static void
test_lookup(void **state)
{
    b_trie_t *trie = b_trie_new(free);

    b_trie_insert(trie, "bola", b_strdup("guda"));
    b_trie_insert(trie, "chu", b_strdup("nda"));
    b_trie_insert(trie, "bote", b_strdup("aba"));
    b_trie_insert(trie, "bo", b_strdup("haha"));

    assert_string_equal(b_trie_lookup(trie, "bola"), "guda");
    assert_string_equal(b_trie_lookup(trie, "chu"), "nda");
    assert_string_equal(b_trie_lookup(trie, "bote"), "aba");
    assert_string_equal(b_trie_lookup(trie, "bo"), "haha");

    assert_null(b_trie_lookup(trie, "arcoiro"));

    b_trie_free(trie);

    trie = b_trie_new(free);

    b_trie_insert(trie, "chu", b_strdup("nda"));
    b_trie_insert(trie, "bola", b_strdup("guda"));
    b_trie_insert(trie, "bote", b_strdup("aba"));
    b_trie_insert(trie, "bo", b_strdup("haha"));
    b_trie_insert(trie, "copa", b_strdup("bu"));
    b_trie_insert(trie, "b", b_strdup("c"));
    b_trie_insert(trie, "test", b_strdup("asd"));

    assert_string_equal(b_trie_lookup(trie, "bola"), "guda");
    assert_string_equal(b_trie_lookup(trie, "chu"), "nda");
    assert_string_equal(b_trie_lookup(trie, "bote"), "aba");
    assert_string_equal(b_trie_lookup(trie, "bo"), "haha");

    assert_null(b_trie_lookup(trie, "arcoiro"));

    b_trie_free(trie);
}


static void
test_size(void **state)
{
    b_trie_t *trie = b_trie_new(free);

    b_trie_insert(trie, "bola", b_strdup("guda"));
    b_trie_insert(trie, "chu", b_strdup("nda"));
    b_trie_insert(trie, "bote", b_strdup("aba"));
    b_trie_insert(trie, "bo", b_strdup("haha"));

    assert_int_equal(b_trie_size(trie), 4);
    assert_int_equal(b_trie_size(NULL), 0);

    b_trie_free(trie);

    trie = b_trie_new(free);

    b_trie_insert(trie, "chu", b_strdup("nda"));
    b_trie_insert(trie, "bola", b_strdup("guda"));
    b_trie_insert(trie, "bote", b_strdup("aba"));
    b_trie_insert(trie, "bo", b_strdup("haha"));
    b_trie_insert(trie, "copa", b_strdup("bu"));
    b_trie_insert(trie, "b", b_strdup("c"));
    b_trie_insert(trie, "test", b_strdup("asd"));

    assert_int_equal(b_trie_size(trie), 7);
    assert_int_equal(b_trie_size(NULL), 0);

    b_trie_free(trie);
}


static unsigned int counter;
static char *expected_keys[] = {"chu", "copa", "bola", "bote", "bo", "b", "test"};
static char *expected_datas[] = {"nda", "bu", "guda", "aba", "haha", "c", "asd"};

static void
mock_foreach(const char *key, void *data)
{
    assert_string_equal(key, expected_keys[counter]);
    assert_string_equal((char*) data, expected_datas[counter++]);
}


static void
test_foreach(void **state)
{
    b_trie_t *trie = b_trie_new(free);

    b_trie_insert(trie, "chu", b_strdup("nda"));
    b_trie_insert(trie, "bola", b_strdup("guda"));
    b_trie_insert(trie, "bote", b_strdup("aba"));
    b_trie_insert(trie, "bo", b_strdup("haha"));
    b_trie_insert(trie, "copa", b_strdup("bu"));
    b_trie_insert(trie, "b", b_strdup("c"));
    b_trie_insert(trie, "test", b_strdup("asd"));

    counter = 0;
    b_trie_foreach(trie, mock_foreach);

    b_trie_free(trie);
}


int
main(void)
{
    const UnitTest tests[] = {
        unit_test(test_new),
        unit_test(test_insert),
        unit_test(test_keep_data),
        unit_test(test_lookup),
        unit_test(test_size),
        unit_test(test_foreach),
    };
    return run_tests(tests);
}
