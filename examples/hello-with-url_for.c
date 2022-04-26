/*
 * balde: A microframework for C based on GLib.
 * Copyright (C) 2013-2017 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the LGPL-2 License.
 * See the file COPYING.
 */

/**
 * \example hello-with-url_for.c
 *
 * An example with the url_for helper. It depends on more files. Take a look at
 * it on the balde source code.
 */

#include <balde.h>
#include "templates/url_for.h"


balde_response_t*
home(balde_app_t *app, balde_request_t *request)
{
    balde_response_t *response = balde_make_response("");
    balde_template_url_for(app, request, response);
    return response;
}


balde_response_t*
profile(balde_app_t *app, balde_request_t *request)
{
    const gchar *name = balde_request_get_view_arg(request, "name");
    gchar *str = g_strdup_printf("Hello, %s\n", name != NULL ? name : "World");
    balde_response_t *response = balde_make_response(str);
    g_free(str);
    return response;
}


int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init();
    balde_app_add_url_rule(app, "home", "/", BALDE_HTTP_GET, home);
    balde_app_add_url_rule(app, "profile", "/profile/<name>/", BALDE_HTTP_GET, profile);
    balde_app_run(app, argc, argv);
    balde_app_free(app);
    return 0;
}
