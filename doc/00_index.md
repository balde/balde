Documentation {#mainpage}
=============

This is *balde*, a microframework for C based on GLib and bad intentions.

It is designed to be fast, simple, and memory efficient. Most of its architecture is based on other microframeworks, like Flask, and it can run on any web server that supports CGI and/or FastCGI.

As a micro-framework, it just does the basic and tries to do it quite well. It will not stay on your way and will let you do almost anything the way you want, but you may have to do some extra work to get complex stuff done.

*balde* uses GLib, the low-level system libraries developed by GNOME project. Some knowledge of the library and its usage is required. Some knowledge of other micro-frameworks, like [Flask](http://flask.pocoo.org/) may be useful as well.

*balde* is free software, released under the LGPL2 license.

The project is hosted and managed using GitHub: https://github.com/balde/balde


### But what about the name?!

"balde" means "bucket" in Portuguese.

The name should be always typed in lower case. It is "balde", not "Balde".


User's Guide
------------

- @subpage getting-started
- @subpage installation
- @subpage application-development
- @subpage application-structure
- @subpage application-cli
- @subpage application-deployment


API Reference
-------------

- @ref balde.h


Additional Notes
----------------

- @subpage api-changes
