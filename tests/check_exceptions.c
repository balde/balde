/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2014 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include <balde/app.h>
#include <balde/exceptions.h>
#include <balde/exceptions-private.h>
#include <balde/wrappers.h>
#include <balde/wrappers-private.h>


void
test_exception_get_name_from_code(void)
{
    const gchar *name = balde_exception_get_name_from_code(404);
    g_assert_cmpstr(name, ==, "Not Found");
}


void
test_exception_get_name_from_code_not_found(void)
{
    const gchar *name = balde_exception_get_name_from_code(1024);
    g_assert(name == NULL);
}

void
test_exception_get_description_from_code(void)
{
    const gchar *description = balde_exception_get_description_from_code(404);
    g_assert_cmpstr(description, ==,
        "The requested URL was not found on the server. If you entered the URL "
        "manually please check your spelling and try again.");
}


void
test_exception_get_description_from_code_not_found(void)
{
    const gchar *description = balde_exception_get_description_from_code(1024);
    g_assert(description == NULL);
}


void
test_abort_set_error(void)
{
    balde_app_t *app = balde_app_init();
    g_assert(app != NULL);
    balde_abort_set_error(app, 404);
    g_assert(app->error->code == 404);
    g_assert_cmpstr(app->error->message, ==,
        "The requested URL was not found on the server. If you "
        "entered the URL manually please check your spelling and try again.");
    balde_app_free(app);
}


void
test_abort_set_error_with_description(void)
{
    balde_app_t *app = balde_app_init();
    g_assert(app != NULL);
    balde_abort_set_error_with_description(app, 404, "bola");
    g_assert(app->error->code == 404);
    g_assert_cmpstr(app->error->message, ==,
        "The requested URL was not found on the server. If you "
        "entered the URL manually please check your spelling and try again."
        "\n\nbola");
    balde_app_free(app);
}


void
test_abort(void)
{
    balde_app_t *app = balde_app_init();
    g_assert(app != NULL);
    balde_response_t *res = balde_abort(app, 404);
    g_assert(res->status_code == 404);
    g_assert_cmpstr(res->body->str, ==,
        "404 Not Found\n\n"
        "The requested URL was not found on the server. If you entered the URL "
        "manually please check your spelling and try again.\n");
    GSList *tmp = g_hash_table_lookup(res->headers, "content-type");
    g_assert_cmpstr(tmp->data, ==, "text/plain; charset=utf-8");
    balde_response_free(res);
    balde_app_free(app);
}


void
test_abort_with_description(void)
{
    balde_app_t *app = balde_app_init();
    g_assert(app != NULL);
    balde_response_t *res = balde_abort_with_description(app, 404, "bola");
    g_assert(res->status_code == 404);
    g_assert_cmpstr(res->body->str, ==,
        "404 Not Found\n\n"
        "The requested URL was not found on the server. If you entered the URL "
        "manually please check your spelling and try again.\n\nbola\n");
    GSList *tmp = g_hash_table_lookup(res->headers, "content-type");
    g_assert_cmpstr(tmp->data, ==, "text/plain; charset=utf-8");
    balde_response_free(res);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/exceptions/get_name_from_code",
        test_exception_get_name_from_code);
    g_test_add_func("/exceptions/get_name_from_code_not_found",
        test_exception_get_name_from_code_not_found);
    g_test_add_func("/exceptions/get_description_from_code",
        test_exception_get_description_from_code);
    g_test_add_func("/exceptions/get_description_from_code_not_found",
        test_exception_get_description_from_code_not_found);
    g_test_add_func("/exceptions/abort_set_error", test_abort_set_error);
    g_test_add_func("/exceptions/abort_set_error_with_description",
        test_abort_set_error_with_description);
    g_test_add_func("/exceptions/abort", test_abort);
    g_test_add_func("/exceptions/abort_with_description",
        test_abort_with_description);
    return g_test_run();
}
