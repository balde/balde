FROM debian:jessie
MAINTAINER Sven Dowideit <SvenDowideit@home.org.au> @SvenDowideit

# build using:
#   docker build -t balde:dev .

# then run:
#   docker run -it balde:dev make check

# or to run interactivly, editing your code outside the container:
#   docker run -it -v $(pwd):/usr/src/balde balde:dev bash

RUN apt-get update && apt-get install -yq \
    pkg-config gettext zlib1g-dev libffi-dev \
    autoconf automake build-essential libtool libxml2-utils \
    libfcgi-dev shared-mime-info libglib2.0-dev wget peg ca-certificates \
    valgrind libglib2.0-0-dbg \
    --no-install-recommends && apt-get clean

WORKDIR /usr/src/balde
COPY . /usr/src/balde

RUN ./autogen.sh && \
    ./configure --prefix=/usr && \
    make && \
    make install
