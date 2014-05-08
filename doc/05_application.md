Application context {#application}
===================

The application context stores everything that is used by the balde application during all of its lifetime, like settings, the registry of views and the static resources. The application context also encapsulates the GLib error reporting infrastructure, making it compatible with the HTTP status codes, that are more suitable for a web application.

The function @ref balde_app_init creates a brand new application context.

The functions @ref balde_app_set_config and @ref balde_app_get_config are used to handle configuration settings.

balde provides two functions to handle views. @ref balde_app_add_url_rule is used to register a view, and @ref balde_app_url_for is used to retrieve the url generated for a given endpoint, that was defined when registering the view with ``balde_app_add_url_rule``. This function accepts a boolean argument called ``external``, that would be used to build complete URLs, suitable for be used on external resources, but this isn't implemented yet.

The function @ref balde_app_run is used to run the balde application, and the @ref balde_app_free function is used to free the allocated resources after usage.

When creating a function that gets the context app as argument, it is important to check if the ``error`` property is ``NULL``, because if this property is set, it means that an error ocurred previously.

There is also a ``user_data`` pointer, that can be used to store arbitraty application-related data.
