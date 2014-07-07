#!/bin/bash
#
# Simple script to install balde deps for jenkins slaves
#

## Install dependencies

CI_DEP_BUNDLE_VERSION=0.2
CI_DEP_BUNDLE_DEB="balde-ci-dep-bundle_${CI_DEP_BUNDLE_VERSION}_amd64.deb"

export DEBIAN_FRONTEND=noninteractive

sudo apt-get -y -qq install \
    wget \
    build-essential \
    autoconf \
    automake1.11 \
    libtool \
    pkg-config \
    valgrind \
    gcc \
    clang \
    libfcgi-dev \
    shared-mime-info \
    peg \
    libxml2-utils

wget \
    --directory-prefix="${HOME}" \
    --continue \
    --quiet \
    "https://github.com/balde/ci-dep-bundle/releases/download/v${CI_DEP_BUNDLE_VERSION}/${CI_DEP_BUNDLE_DEB}"

sudo dpkg -i "${HOME}/${CI_DEP_BUNDLE_DEB}"
