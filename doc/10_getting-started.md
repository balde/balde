Getting started {#getting-started}
===============

This is a quick guide to get users started with balde. At the end of this guide you should be able to get a simple balde example running.

If you are used to write C applications, it will be quite easy for you to start creating simple web applications with balde.

At the end of each section, you will find links to detailed documentation about each topic.

Enjoy!

TODO: add asciinema.org video here


Installing balde
----------------

To install balde you must [download it first](https://github.com/balde/balde/releases). Pick the latest release, and download it to your computer. After downloading, extract it with some tool like ``tar``.

To run this example, the only required dependencies are ``GLib >= 2.35``, ``shared-mime-info`` (that is usually installed with ``GLib``), ``GNU Make`` and ``GCC``. Other dependencies and features are listed in the @ref installation section of the documentation.

To install it, from the extracted directory, run:

@verbatim
$ ./configure
$ make  # or gmake, if you are running some flavour of BSD
# make install  # or sudo make install, if applicable
@endverbatim

See the @ref installation section for more information about how to install balde.


Creating the example
--------------------

This is the simplest web application that someone can build with balde, a "hello world":

@snippet hello.c Hello world

Save its content to a file called ``hello.c`` in your current directory. This file can be found in the ``examples/`` directory of the balde tarball and/or Git repository.

This example is mostly self-explanatory, but lets go into the details of it, to make things even clearer.

@dontinclude hello.c

First of all, you must include the balde header:

@skipline include

After that, you can declare a view, to reply to requests. As this is a simple example, we are not going to add the prototype of the function, just implement it directly:

@skip balde_response_t
@until {

The ``hello`` function is a typical balde view. It gets a [balde application context](@ref balde_app_t) and a [balde request context](@ref balde_request_t), and returns a [balde response context](@ref balde_response_t).

The simple view of the example just returns a response with a hardcoded string:

@skip return
@until }

The ``main`` function is the function that is called when any ``C`` application is started:

@skip in
@until {

At the begin of the function, you should initialize a [balde application context](@ref balde_app_t):

@skipline balde_app_t

After that, you should register the previously defined ``hello`` function as a view for this application context, accepting ``GET`` requests at ``/``:

@skipline balde_app_add_url_rule

Now that a view is registered in the application context, you can finally run the application main loop:

@skipline balde_app_run

When the main loop exits, you must free the memory used by the application context:

@skipline balde_app_free

The ``main`` functions for balde will almost always return ``0`` in the end:

@skip return
@until }

This information should be enough to get started.

See the @ref application-development section for more information about how to write balde applications.


Building the example
--------------------

To build the example, you will want to use [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) to find the balde library and headers, that were previously installed, and pass that info to the compiler.

@verbatim
$ gcc -o hello $(pkg-config --libs --cflags balde) hello.c
@endverbatim

Or, if you use ``clang``:

@verbatim
$ clang -o hello $(pkg-config --libs --cflags balde) hello.c
@endverbatim

See the @ref application-structure section for more information about how to distribute/build your balde applications.


Running the example
-------------------

balde comes with an embedded HTTP server, that can be used when you are developing your applications.

@warning DO NOT use the embedded HTTP server in production! It cannot handle a reasonable number of requests in parallel, and was created just to help developers.

@verbatim
$ ./hello --runserver
 * Running on http://127.0.0.1:8080/ (threads: 10)
@endverbatim

Visit the given URL with your web browser, and you should see the expected message!

See the @ref application-cli section for more information about the usage of the command line interface provided by the balde library.


Deploying the example
---------------------

After having the example running in your local machine, you may want to deploy it to a public server. See the @ref application-deployment section for deployment guides.
