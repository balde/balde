#!/bin/bash
#
# balde test runner for Jenkins CI.
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
    --enable-webserver \
    --with-leg \
    --with-valgrind
popd > /dev/null

make \
    -j"$(($(cat /proc/cpuinfo | grep processor | wc -l)+1))" \
    -C "${BALDE_BUILD_DIR}" valgrind-ci
