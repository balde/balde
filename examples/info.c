#include <balde.h>


int
main(int argc, char **argv)
{
    balde_app_t *app = balde_app_init();
    balde_info_register(app, "info", "/");
    balde_app_run(app, argc, argv);
    balde_app_free(app);
    return 0;
}
