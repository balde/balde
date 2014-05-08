HTTP requests {#requests}
=============

The HTTP requests are handle by the @ref balde_request_t data structure. This structure stores everything request-related, and its lifetime is a single HTTP request. This data structure is created and initialized by balde by default, and passed to all the views.

There are 5 functions available, to get data from the request context:

- @ref balde_request_get_header - Returns an HTTP header sent by the client.
- @ref balde_request_get_arg - Returns a query string argument sent by the client in the URL.
- @ref balde_request_get_form - Returns a value sent by the client as a form.
- @ref balde_request_get_view_arg - Returns a value parsed from the URL path, as defined by the URL rule, when registering the view in the [balde application context](@ref application).
- @ref balde_request_get_cookie - Returns an HTTP cookie value, as sent by the client.

