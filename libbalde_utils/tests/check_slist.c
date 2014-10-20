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

#include <balde-utils/slist.h>
#include <balde-utils/string_utils.h>


static void
test_slist_append(void **state)
{
    b_slist_t *l = NULL;
    l = b_slist_append(l, (void*) b_strdup("bola"));
    assert_non_null(l);
    assert_string_equal(l->data, "bola");
    assert_null(l->next);
    l = b_slist_append(l, (void*) b_strdup("guda"));
    assert_non_null(l);
    assert_string_equal(l->data, "bola");
    assert_non_null(l->next);
    assert_string_equal(l->next->data, "guda");
    assert_null(l->next->next);
    b_slist_free_full(l, free);
}


static void
test_slist_free(void **state)
{
    b_slist_t *l = NULL;
    char *t1 = b_strdup("bola");
    char *t2 = b_strdup("guda");
    char *t3 = b_strdup("chunda");
    l = b_slist_append(l, (void*) t1);
    l = b_slist_append(l, (void*) t2);
    l = b_slist_append(l, (void*) t3);
    b_slist_free(l);
    assert_string_equal(t1, "bola");
    assert_string_equal(t2, "guda");
    assert_string_equal(t3, "chunda");
    free(t1);
    free(t2);
    free(t3);
}


static void
test_slist_length(void **state)
{
    b_slist_t *l = NULL;
    l = b_slist_append(l, (void*) b_strdup("bola"));
    l = b_slist_append(l, (void*) b_strdup("guda"));
    l = b_slist_append(l, (void*) b_strdup("chunda"));
    assert_int_equal(b_slist_length(l), 3);
    b_slist_free_full(l, free);
}


int
main(void)
{
    const UnitTest tests[] = {
        unit_test(test_slist_append),
        unit_test(test_slist_free),
        unit_test(test_slist_length),
    };
    return run_tests(tests);
}
