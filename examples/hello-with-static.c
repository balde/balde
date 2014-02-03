#include <balde.h>
#include "resources.h"


balde_response_t*
static_resource(balde_app_t *app, balde_request_t *request)
{
    const gchar* p = balde_request_get_view_arg(request, "p");
    gchar *tmp = g_strdup_printf("/static/%s", p);
    balde_response_t *rv = balde_make_response_from_static_resource(app, tmp);
    g_free(tmp);
    return rv;
}


int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init();
    balde_resources_load(app, resources_get_resource());
    balde_app_add_url_rule(app, "static", "/static/<path:p>", BALDE_HTTP_GET,
        static_resource);
    balde_app_run(app);
    balde_app_free(app);
}
