Getting started {#getting-started}
===============

To install balde you must [download it first](http://balde.io/downloads.md). After downloading and extracting it, you must install the required dependencies, that are: FastCGI development tookit, GLib >= 2.34 and the shared-mime-info package. You will also need a C compiler and the autotools.

If you are used to write C applications, it will be quite easy for you to start creating simple web applications with balde.

This is the simplest web application that someone can build with balde, a "hello world":

@include hello.c

This example is mostly self-explanatory.

The ``hello`` function is a typical balde view. It gets a [balde application context](@ref balde_app_t) and a [balde request context](@ref balde_request_t), and returns a [balde response context](@ref balde_response_t).

The ``main`` function, that is the function that is called when the app is initialized, initializes the balde application context, registers the ``hello`` view to accept GET requests at ``/``, runs the application, and free resources after used.

This information should be enough to get started. Please read the other parts of the documentation for details about how to do complex stuff with balde.


Deploying the example
---------------------

To run a balde app, you will need a FastCGI capable webserver, like Apache with mod_fcgid.

The following example can be used to run a balde app using apache with mod_fcgid.

@code
<VirtualHost *:80>
    ServerName localhost
    ScriptAlias / /path/to/balde-app/
    <Directory /path/to>
        Order allow,deny
        Allow from all
        Options ExecCGI
        SetHandler fcgid-script
    </Directory>
</VirtualHost>
@endcode

This example would run the ``/path/to/balde-app`` binary (**mind the trailing slash**), mounted at the root of the localhost domain.
