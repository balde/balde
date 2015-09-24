Installation {#installation}
============

This document will guide you through all the steps required to get the *balde* libraries installed in your computer.


Dependencies
------------

*balde* depends on a few external libraries and tools:

- ``GNU Make`` - http://www.gnu.org/software/make/
- ``GCC`` or ``clang`` - https://gcc.gnu.org/ - http://clang.llvm.org/
- ``GLib >= 2.35`` - https://developer.gnome.org/glib/
- ``shared-mime-info`` - http://freedesktop.org/wiki/Software/shared-mime-info/


Download
--------

All the releases are listed in the [GitHub releases page](https://github.com/balde/balde/releases). Please pick the latest one and download to your computer. We provide the sources as ``.tar.gz``, ``.tar.bz2``, and ``.tar.xz`` tarballs.

@warning Please DO NOT download the tarballs generated automatically by GitHub. They are broken, because they are just snapshots of the Git repository, lacking most of the build system files. If you really want to use these tarballs you will need recent enough version of GNU Autotools  and some other development tools to create the ``configure`` script.

Extract the sources in your current directory and enter it, using your terminal.


Configuring Build System
------------------------

*balde* releases comes with a ``configure`` script, that accepts several command-line options (or arguments), to allow you to customize your *balde* installation:

- ``--enable-examples``: This option enables the ``Makefile`` rules that builds the balde examples. Examples are *NEVER* installed to the system, but building them is useful to play with them in the local direcoty.
- ``--disable-http``: This option disables the embedded HTTP server, that can be used during the development phase of your application. This is enabled by default, use the option to disable it if not needed, e.g. in production environment. See @ref application-cli for usage details.
- ``--disable-doc``: This option disables the ``Makefile`` rules that are used to build this documentation. Documentation building rules are enabled by default, but depends on [Doxygen](http://www.doxygen.org) being installed on your system. Normal users should not need to change this.

Some other options are available, but they are only useful for people developing balde framework itself. If you want to see additional options, please run ``./configure --help``.

To configure the build system and create the required ``Makefile``s, run:

@verbatim
$ ./configure [options]
@endverbatim

Where ``[options]`` is any of the options described previously.


Building
--------

With everything configured, the user just needs to run:

@verbatim
$ make
@endverbatim

This will build the library, binaries and the examples (if requested during the configuration phase).

Examples are built in the ``examples/`` directory.


Installing
----------

With the binaries built, the user just needs to run, as root:

@verbatim
# make install
@endverbatim

This will install the library, binaries and headers to the system. Examples are never installed, as stated before.

The default installation prefix is ``/usr/local`` for linux users. This can be changed during the configuration phase, passing use the ``--prefix`` option to the ``configure`` script.


Bonus: Building documentation
-----------------------------

To build a local copy of this documentation, if you do not disabled it during configuration phase, and have [Doxygen](http://www.doxygen.org) installed, just run:

@verbatim
$ make doc
@endverbatim


Bonus: Running tests
--------------------

If you want to run balde test suite, just run:

@verbatim
$ make check
@endverbatim
