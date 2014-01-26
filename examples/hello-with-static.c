#include <balde.h>
#include "static/static.h"


balde_response_t*
hello(balde_app_t *app, balde_request_t *request)
{
    GBytes *b = g_resource_lookup_data(app->static_resources, "/static/foo.js", 0, &(app->error));
    gsize size;
    gconstpointer data = g_bytes_get_data (b, &size);
    return balde_make_response((const gchar*) data);
}


int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init(static_get_resource());
    balde_app_add_url_rule(app, "hello", "/", BALDE_HTTP_GET, hello);
    balde_app_run(app);
    balde_app_free(app);
}
