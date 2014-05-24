Template engine {#template-engine}
===============

balde comes with a simple logic-less template engine. It does not includes several fancy features that are common in other engines, but still makes it simple to create templates and handle web application output.

The template engine does all of its stuff in compilation time, with very small processing overhead. It generates C source code, that is built and linked with the balde application binary.


Variables
---------

All the template variables are strings.

Template variables are set from views (or any other function with access to the [balde response context](@ref balde_response_t)), using the @ref balde_response_set_tmpl_var function.

The template engine can print variables using the following syntax:

@code
{{ variable }}
@endcode


Function calls
--------------

The template engine can print the value returned by a function call.

The value returned by the function must be a newly allocated string, because it will be free'd with ``g_free()`` by the template engine after usage.

The functions must be called only with variables (strings only), strings (double or single quoted), integers, floats, booleans and ``NULL`` as arguments.

The template engine does some mangling on the function before calling it:

- If the ``foo`` function is called on a template, the actual function that will be called is ``balde_tmpl_foo``.
- If a function is called with a ``gchar*`` argument, it is actually called with a @ref balde_app_t, a @ref balde_request_t and the ``gchar*`` argument. This is useful because most of the functions will want to handle the application and request contexts.

That means that a function call like:

@code
{{ foo("asd", TRUE) }}
@endcode

or

@code
{{ foo('asd', TRUE) }}
@endcode

would be translated to:

@code
balde_tmpl_foo(app, request, "asd", TRUE);
@endcode

where ``app`` is a @ref balde_app_t variable, and ``request`` is a @ref balde_request_t variable.

The function calls accepts variables, but does not accept another function call as argument.

There is a template function provided by balde itself, that is a wrapper to the @ref balde_app_url_for function, and is called @ref balde_tmpl_url_for (``url_for`` when writing templates).

Here is a simple example:

@include templates/url_for.html


Include headers
---------------

To be able to run arbitrary functions, the users need to include the header to the template, using the following syntax:

@code
{% include "foo.h" %}
@endcode

or

@code
{% include 'foo.h' %}
@endcode

that will be translated to:

@code
#include "foo.h"
@endcode

This will include all the declarations available on the header to the template.


How to build templates
----------------------

Templates should be converted to C source, that should be built and linked to the application binary afterwards. There's a template generation tool bundled with ``balde``, that is called ``balde-template-gen``.

If your project is autotools-based, you can include the following code snippet to the Makefile.am file of the directory that stores your templates:

@code
%.c: %.html
	$(AM_V_GEN)balde-template-gen $< $@

%.h: %.html
	$(AM_V_GEN)balde-template-gen $< $@
@endcode

These rules will generate headers and sources for you, if you register the generated files as dependencies of the application binary.
