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
#include <balde/resources.h>
#include <balde/resources-private.h>
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
balde_assert_resource(GSList *l, gchar *name, gchar *content, gchar *type)
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
}


void
test_resources_load(void)
{
    balde_app_t *app = balde_app_init();
    balde_resources_load(app, resources_get_resource());
    g_assert(app->static_resources != NULL);
    g_assert(g_slist_length(app->static_resources) == 3);
    balde_assert_resource(app->static_resources, "/static/lol.css",
        "body {\n    background-color: #CCC;\n}\n",
        "text/css");
    balde_assert_resource(app->static_resources->next, "/static/lol.js",
        "function a() {\n    alert('lol');\n}\n",
        "application/javascript");
    balde_assert_resource(app->static_resources->next->next, "/static/zz.sh",
        "#!/bin/bash\n\nzz() {\n    :\n}\n",
        "application/x-shellscript");
    g_assert(app->static_resources->next->next->next == NULL);
    balde_app_free(app);
}


int
main(int argc, char** argv)
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/resources/list_files", test_resources_list_files);
    g_test_add_func("/resources/load", test_resources_load);
    return g_test_run();
}
