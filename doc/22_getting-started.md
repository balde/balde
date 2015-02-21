Getting started {#getting-started}
===============

To install balde you must [download it first](https://github.com/balde/balde/releases). After downloading and extracting it, you must install the required dependencies, that are: ``FastCGI development tookit``, ``GLib >= 2.35`` and the ``shared-mime-info`` package. You will also need a C compiler (``GCC`` and ``clang`` are fully supported) and ``GNU make``.

If you are used to write C applications, it will be quite easy for you to start creating simple web applications with balde.

This is the simplest web application that someone can build with balde, a "hello world":

@snippet hello.c Hello world

This example is mostly self-explanatory, but lets go into the details of it, to make things even clearer.

@dontinclude hello.c

First of all, the user must include the balde header:

@skipline include

After that, the user can declare a view, to reply to requests. As this is a simple example, we are not going to add the prototype of the function, just implement it directly:

@skip balde_response_t
@until {

The ``hello`` function is a typical balde view. It gets a [balde application context](@ref balde_app_t) and a [balde request context](@ref balde_request_t), and returns a [balde response context](@ref balde_response_t).

The simple view of the example just returns a response with a hardcoded string:

@skip return
@until }

The ``main`` function is the function that is called when any ``C`` application is started:

@skip in
@until {

At the begin of the function, the user should initialize a [balde application context](@ref balde_app_t):

@skipline balde_app_t

After that, the user should register the previously defined ``hello`` function as a view for this application context, accepting ``GET`` requests at ``/``:

@skipline balde_app_add_url_rule

Now that a view is registered in the application context, the user can finally run the application main loop:

@skipline balde_app_run

When the main loop exits, the user must free the memory used by the application context:

@skipline balde_app_free

The main functions for balde will almost always return ``0`` in the end:

@skip return
@until }

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
