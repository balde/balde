/*
 * balde: A microframework for C based on GLib and bad intentions.
 * Copyright (C) 2013-2015 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>
#include "../src/balde.h"
#include "../src/app.h"
#include "../src/resources.h"
#include "../src/requests.h"
#include "../src/responses.h"
#include "resources.h"


void
test_resources_list_files(void)
{
    GError *error = NULL;
    gchar **rv = balde_resources_list_files(resources_get_resource(), &error);
    g_assert(rv != NULL);
    g_assert(error == NULL);
    g_assert_cmpstr(rv[0], ==, "/static/lol.css");
    g_assert_cmpstr(rv[1], ==, "/static/lol.js");
    g_assert_cmpstr(rv[2], ==, "/static/zz.sh");
    g_assert(rv[3] == NULL);
    g_strfreev(rv);
}


void
balde_assert_resource(GSList *l, gchar *name, gchar *content, gchar *type,
    gchar *hash_name, gchar *hash_content)
{
    balde_resource_t *resource = (balde_resource_t*) l->data;
    if (name != NULL)
        g_assert_cmpstr(resource->name, ==, name);
    else
        g_assert(resource->name == NULL);
    if (content != NULL)
        g_assert_cmpstr(resource->content->str, ==, content);
    else
        g_assert(resource->content == NULL);
    if (type != NULL)
        g_assert_cmpstr(resource->type, ==, type);
    else
        g_assert(resource->type == NULL);
    if (hash_name != NULL)
        g_assert_cmpstr(resource->hash_name, ==, hash_name);
    else
        g_assert(resource->hash_name == NULL);
    if (hash_content != NULL)
        g_assert_cmpstr(resource->hash_content, ==, hash_content);
    else
        g_assert(resource->hash_content == NULL);
}


void
test_resources_load(void)
{
    balde_app_t *app = balde_app_init();
    balde_resources_load(app, resources_get_resource());
    g_assert(app->priv->static_resources != NULL);
    g_assert(g_slist_length(app->priv->static_resources) == 3);
    balde_assert_resource(app->priv->static_resources, "/static/lol.css",
        "body {\n    background-color: #CCC;\n}\n",
        "text/css", "daab60b9178fd56656840a7fb9fc491c",
        "48536785a0d37e65c9ebc6d7ee25119a");
    balde_assert_resource(app->priv->static_resources->next, "/static/lol.js",
        "function a() {\n    alert('lol');\n}\n",
        "application/javascript", "5338df6146fde6cc4034e3c47972d268",
        "d14c4623de381fa7a3a3f9b509cecbc3");
    balde_assert_resource(app->priv->static_resources->next->next, "/static/zz.sh",
        "#!/bin/bash\n\nzz() {\n    :\n}\n",
        "application/x-shellscript", "09284640fe6904d369629d7b04dc1387",
        "e3f8e345860a9caf1eb8d57e04308ccb");
    g_assert(app->priv->static_resources->next->next->next == NULL);
    balde_app_free(app);
}


void
test_make_response_from_static_resource(void)
{
    g_unsetenv("HTTP_IF_NONE_MATCH");
    balde_app_t *app = balde_app_init();
    balde_resources_load(app, resources_get_resource());
    balde_request_t *request = balde_make_request(app, NULL);
    balde_response_t *response = balde_make_response_from_static_resource(app,
        request, "/static/lol.css");
    g_assert(response != NULL);
    g_assert_cmpint(response->status_code, ==, 200);
    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 4);
    GSList *tmp = g_hash_table_lookup(response->priv->headers, "cache-control");
    g_assert_cmpstr(tmp->data, ==, "public, max-age=43200");
    tmp = g_hash_table_lookup(response->priv->headers, "expires");
    g_assert(g_str_has_suffix(tmp->data, " GMT"));
    tmp = g_hash_table_lookup(response->priv->headers, "etag");
    g_assert_cmpstr(tmp->data, ==,
        "\"balde-daab60b9178fd56656840a7fb9fc491c-48536785a0d37e65c9ebc6d7ee25119a\"");
    tmp = g_hash_table_lookup(response->priv->headers, "content-type");
    g_assert_cmpstr(tmp->data, ==, "text/css");
    g_assert_cmpstr(response->priv->body->str, ==,
        "body {\n"
        "    background-color: #CCC;\n"
        "}\n");
    balde_response_free(response);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_make_response_from_static_resource_304(void)
{
    g_setenv("HTTP_IF_NONE_MATCH",
        "\"balde-daab60b9178fd56656840a7fb9fc491c-48536785a0d37e65c9ebc6d7ee25119a\"",
        TRUE);
    balde_app_t *app = balde_app_init();
    balde_resources_load(app, resources_get_resource());
    balde_request_t *request = balde_make_request(app, NULL);
    balde_response_t *response = balde_make_response_from_static_resource(app,
        request, "/static/lol.css");
    g_assert(response != NULL);
    g_assert_cmpint(response->status_code, ==, 304);
    g_assert_cmpint(g_hash_table_size(response->priv->headers), ==, 4);
    GSList *tmp = g_hash_table_lookup(response->priv->headers, "cache-control");
    g_assert_cmpstr(tmp->data, ==, "public, max-age=43200");
    tmp = g_hash_table_lookup(response->priv->headers, "expires");
    g_assert(g_str_has_suffix(tmp->data, " GMT"));
    tmp = g_hash_table_lookup(response->priv->headers, "etag");
    g_assert_cmpstr(tmp->data, ==,
        "\"balde-daab60b9178fd56656840a7fb9fc491c-48536785a0d37e65c9ebc6d7ee25119a\"");
    tmp = g_hash_table_lookup(response->priv->headers, "content-type");
    g_assert_cmpstr(tmp->data, ==, "text/css");
    g_assert_cmpstr(response->priv->body->str, ==, "");
    balde_response_free(response);
    balde_request_free(request);
    balde_app_free(app);
}


void
test_make_response_from_static_resource_404(void)
{
    g_unsetenv("HTTP_IF_NONE_MATCH");
    balde_app_t *app = balde_app_init();
    balde_request_t *request = balde_make_request(app, NULL);
    balde_response_t *response = balde_make_response_from_static_resource(app,
        request, "/static/lol.css");
    g_assert(response != NULL);
    g_assert_cmpint(response->status_code, ==, 404);
    balde_response_free(response);
    balde_request_free(request);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/resources/list_files", test_resources_list_files);
    g_test_add_func("/resources/load", test_resources_load);
    g_test_add_func("/resources/make_response_from_static_resource",
        test_make_response_from_static_resource);
    g_test_add_func("/resources/make_response_from_static_resource_304",
        test_make_response_from_static_resource_304);
    g_test_add_func("/resources/make_response_from_static_resource_404",
        test_make_response_from_static_resource_404);
    return g_test_run();
}
