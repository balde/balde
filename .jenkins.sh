#!/bin/bash
#
# This script assumes that balde dependencies are already installed on the
# system.
#
# They should be installed by hand if you are going to run a Jenkins slave.
#
#     $ sudo apt-get install -y peg libfcgi-dev shared-mime-info pkg-config gettext \
#         zlib1g-dev libffi-dev autoconf automake1.11 build-essential libtool \
#         libxml2-utils valgrind clang
#
# To install GLib, run the .build-glib.sh script (see its code for help).
#
# Also, it depends on some enviromnent variables, that are setup by Jenkins:
#
#     GLIB_VERSION (e.g. 2.34.3)
#

## GLib variables
GLIB_BASE_DIR="/opt/glib/${GLIB_VERSION}"


## balde variables
BALDE_SRC_DIR="$(pwd)"
BALDE_BUILD_DIR="${BALDE_SRC_DIR}/build"


## balde needs to know where to look for glib stuff
export PKG_CONFIG_LIBDIR="${GLIB_BASE_DIR}/lib/pkgconfig"
export PKG_CONFIG_PATH="${GLIB_BASE_DIR}/lib/pkgconfig:/usr/share/pkgconfig"
export PATH="${GLIB_BASE_DIR}/bin:${PATH}"

GLIB_VERSION_PKGCONFIG="$(pkg-config --modversion glib-2.0)"
if [[ "${GLIB_VERSION_PKGCONFIG}" != "${GLIB_VERSION}" ]]; then
    exit 1
fi

rm -rf "${BALDE_BUILD_DIR}"

./autogen.sh

mkdir -p "${BALDE_BUILD_DIR}"
pushd "${BALDE_BUILD_DIR}" > /dev/null
"${BALDE_SRC_DIR}"/configure \
    --enable-examples \
    --with-leg \
    --with-valgrind
popd > /dev/null

make \
    -j"$(($(cat /proc/cpuinfo | grep processor | wc -l)+1))" \
    -C "${BALDE_BUILD_DIR}" valgrind-ci
