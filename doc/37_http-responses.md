HTTP responses {#http-responses}
==============

The HTTP responses are handle by the @ref balde_response_t data structure. It must be built and initialized by the application view and returned. balde will use it to render the HTTP response, and free its resources afterwards.

The @ref balde_response_t data structure will store the body of the HTTP response. balde provides two functions to initialize the structure with initial HTTP response body and 2 othre functions to append stuff to the body after initialization:

- @ref balde_make_response - Initializes a response context with a null-terminated string as HTTP response body.
- @ref balde_make_response_len - Initializes a response context with an arbitrary string as HTTP response body, in this case you must provide its length.
- @ref balde_response_append_body - Appends a null-terminated string to HTTP response body.
- @ref balde_response_append_body_len - Appends an arbitrary string to HTTP response body, in this case you must provide its length.

There are 2 functions to handle cookies:

- @ref balde_response_set_cookie - Creates a cookie in the client.
- @ref balde_response_delete_cookie - Deletes a cookie in the client.

And 2 functions to handle template variables (template variables are always strings).

- @ref balde_response_set_tmpl_var - Sets the value of a template variable.
- @ref balde_response_get_tmpl_var - Gets the value of a template variable.
