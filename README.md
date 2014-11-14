balde
=====

[![Build Status](https://ci.rgm.io/buildStatus/icon?job=balde)](https://ci.rgm.io/job/balde/)

This is *balde*, a microframework for C based on GLib and bad intentions.

It is designed to be fast, simple, and memory efficient. Most of its architecture is based on other microframeworks, like Flask, and it can run on any web server that supports CGI and/or FastCGI.

*balde* is free software, released under the LGPL2 license.


But what about the name?!
-------------------------

"balde" means "bucket" in Portuguese.


Compile
-------

```
./autogen.sh
./configure --enable-examples
make
```

Running without install
-----------------------

Export `libbalde`.

```
export LD_LIBRARY_PATH=`pwd`/libbalde/.libs/:$LD_LIBRARY_PATH
export LIBRARY_PATH=`pwd`/libbalde/.libs/:$LIBRARY_PATH
```

Enter `examples` folder and run `lighttpd`.

```
cd examples/
lighttpd -D -f lighttpd.conf
```

Go to `http://127.0.0.1:3000/hello`.
