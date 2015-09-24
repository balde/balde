#!/bin/bash

set -ex

[[ $# -eq 1 ]]

source "$(dirname $0)/settings.sh"

GLIB_VERSION="${1}"
GLIB_BASE_DIR="${GLIB_PREFIX}/${GLIB_VERSION}"

BALDE_SRC_DIR="$(pwd)"
BALDE_BUILD_DIR="${BALDE_SRC_DIR}/build"

export PKG_CONFIG_LIBDIR="${GLIB_BASE_DIR}/lib/pkgconfig"
export PKG_CONFIG_PATH="${GLIB_BASE_DIR}/lib/pkgconfig:/usr/share/pkgconfig"
export PATH="${GLIB_BASE_DIR}/bin:${PATH}"
export LD_LIBRARY_PATH="${GLIB_BASE_DIR}/lib:${LD_LIBRARY_PATH}"

GLIB_VERSION_PKGCONFIG="$(pkg-config --modversion glib-2.0)"
if [[ "${GLIB_VERSION_PKGCONFIG}" != "${GLIB_VERSION}" ]]; then
    exit 1
fi

rm -rf "${BALDE_BUILD_DIR}"

./autogen.sh

mkdir -p "${BALDE_BUILD_DIR}"
pushd "${BALDE_BUILD_DIR}" > /dev/null
"${BALDE_SRC_DIR}"/configure \
    CFLAGS="-O2 -g -Wall" \
    --enable-examples \
    --enable-http \
    --enable-leg \
    --enable-valgrind \
    --disable-doc
popd > /dev/null

make \
    -j"$(($(cat /proc/cpuinfo | grep processor | wc -l)+1))" \
    -C "${BALDE_BUILD_DIR}" valgrind
