#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <balde.h>
#include "foo.h"
#include "templates/base.h"
#include "static/resources.h"


balde_response_t*
hello_view(balde_app_t *app, balde_request_t *request)
{
    balde_response_t *response = balde_make_response("");
    balde_response_set_tmpl_var(response, "title", "Some random title");
    gchar *number = g_strdup_printf("%d", foo_get_number());
    balde_response_set_tmpl_var(response, "number", number);
    g_free(number);
    balde_template_base(app, request, response);
    return response;
}


int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init();
    balde_resources_load(app, resources_get_resource());
    balde_app_add_url_rule(app, "hello", "/", BALDE_HTTP_GET, hello_view);
    balde_app_run(app, argc, argv);
    balde_app_free(app);
    return 0;
}
