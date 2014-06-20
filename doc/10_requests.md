HTTP requests {#requests}
=============

The HTTP requests are handle by the @ref balde_request_t data structure. This structure stores everything request-related, and its lifetime is a single HTTP request. This data structure is created and initialized by balde by default, and passed to all the views.

There are 5 functions available, to get data from the request context:

- @ref balde_request_get_header - Returns an HTTP header sent by the client.
- @ref balde_request_get_arg - Returns a query string argument sent by the client in the URL.
- @ref balde_request_get_form - Returns a value sent by the client as a form.
- @ref balde_request_get_view_arg - Returns a value parsed from the URL path, as defined by the URL rule, when registering the view in the [balde application context](@ref application).
- @ref balde_request_get_cookie - Returns an HTTP cookie value, as sent by the client.


Handling Basic HTTP authentication
----------------------------------

The framework provides some minimal infrastructure to handle Basic HTTP authentication. The [request context data structure](@ref balde_request_t) comes with a field called [authorization](@ref balde_authorization_t). This field stores the username and the password of the user trying to authenticate, if provided. Otherwise, the field will be ``NULL``.

Some web servers, like Apache, won't forward the ``Authorization`` header to the CGI application, and you need to fix it manually. The following code snippet fixes this for Apache.

@code
RewriteEngine on
RewriteCond %{HTTP:Authorization} ^(.*)
RewriteRule .* - [e=HTTP_AUTHORIZATION:%1]
@endcode

Add it to your VirtualHost configuration block.
