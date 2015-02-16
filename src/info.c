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
#include "balde.h"
#include "balde-private.h"
#include "info.h"


balde_response_t*
balde_info_view(balde_app_t *app, balde_request_t *request)
{
    balde_response_t* response = balde_make_response(
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"DTD/xhtml1-transitional.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
        "<head>\n"
        "    <title>balde information</title>\n"
        "    <meta name=\"ROBOTS\" content=\"NOINDEX,NOFOLLOW,NOARCHIVE\" />\n"
        "    <style type=\"text/css\">\n"
        "        * {margin:0; padding: 0}\n"
        "        body {font-family: Verdana, Arial}\n"
        "        h1 {text-align: center; font-size: 26px; margin-top: 15px; margin-bottom: 30px}\n"
        "        table {width: 800px; margin: 25px auto; margin-top: 0; border: 1px solid #000; border-spacing: 0}\n"
        "        td, th {padding: 3px 15px;}\n"
        "        th {text-align: center; border-bottom: 1px solid #000}\n"
        "        p {margin: 5px auto}\n"
        "        a {color: #009}\n"
        "        .first-table {margin-top: 20px}\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <table class=\"first-table\">\n"
        "        <tr>\n"
        "            <td>\n"
        "                <h1>" PACKAGE_STRING "</h1>\n"
        "            </td>\n"
        "            <td>\n"
        "                <a href=\"http://balde.io/\">\n"
        "                    <img border=\"0\" src=\"data:image/gif;base64,"
        "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"
        "ASUVORK5CYII=\" alt=\"balde logo\" />\n"
        "                </a>\n"
        "            </td>\n"
        "        </tr>\n"
        "    </table>\n");

    gchar *tmp;

    // request data
    if (request != NULL) {
        tmp = g_strdup_printf(
            "    <table>\n"
            "        <tr><th colspan=\"2\">Request data</th></tr>\n"
            "        <tr><td>Server name:</td><td>%s</td></tr>\n"
            "        <tr><td>Script name:</td><td>%s</td></tr>\n"
            "        <tr><td>Path:</td><td>%s</td></tr>\n"

            "    </table>\n",
            request->server_name, request->script_name, request->path);


        balde_response_append_body(response, tmp);
        g_free(tmp);
    }


    balde_response_append_body(response, "</body>\n</html>\n");
    return response;
}


BALDE_API void
balde_info_register(balde_app_t *app, const gchar *endpoint, const gchar *rule)
{
    balde_app_add_url_rule(app, endpoint, rule, BALDE_HTTP_ANY, balde_info_view);
}
