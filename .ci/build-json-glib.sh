#!/bin/bash

set -ex

[[ $# -eq 2 ]]

source "$(dirname $0)/settings.sh"

GLIB_RELEASE="${1}"
JSON_GLIB_RELEASE="${2}"

GLIB_PREFIX="${GLIB_PREFIX}/${GLIB_RELEASE}"

[[ -d "${GLIB_PREFIX}" ]]

# Environment variables to found uninstalled glib and build against it.
export PKG_CONFIG_PATH="${GLIB_PREFIX}/lib/pkgconfig"
export PKG_CONFIG="pkg-config --define-variable=prefix=${GLIB_PREFIX}"
export LD_LIBRARY_PATH="${GLIB_PREFIX}/lib:${LD_LIBRARY_PATH}"
export PATH="${GLIB_PREFIX}/bin:${PATH}"

JSON_GLIB_VERSION="${JSON_GLIB_RELEASE%.*}"
JSON_GLIB_BASE_SRC_DIR="${JSON_GLIB_BUILD}/${GLIB_RELEASE}"
JSON_GLIB_SRC_DIR="${JSON_GLIB_BASE_SRC_DIR}/json-glib-${JSON_GLIB_RELEASE}"
JSON_GLIB_BUILD_DIR="${JSON_GLIB_BUILD}/json-glib-${JSON_GLIB_RELEASE}-build/${GLIB_RELEASE}"
JSON_GLIB_PREFIX_DIR="${JSON_GLIB_PREFIX}/${JSON_GLIB_RELEASE}/${GLIB_RELEASE}"

[[ -d "${JSON_GLIB_PREFIX_DIR}" ]] && exit 0

wget \
    --continue \
    --directory-prefix="${JSON_GLIB_BUILD}" \
    "http://ftp.gnome.org/pub/gnome/sources/json-glib/${JSON_GLIB_VERSION}/json-glib-${JSON_GLIB_RELEASE}.tar.xz"

mkdir -p "${JSON_GLIB_BASE_SRC_DIR}"

tar \
    --extract \
    --verbose \
    --file "${JSON_GLIB_BUILD}/json-glib-${JSON_GLIB_RELEASE}.tar.xz" \
    --directory "${JSON_GLIB_BASE_SRC_DIR}"

mkdir -p "${JSON_GLIB_BUILD_DIR}"

pushd "${JSON_GLIB_BUILD_DIR}" > /dev/null

"${JSON_GLIB_SRC_DIR}/configure" \
    --prefix="${JSON_GLIB_PREFIX_DIR}" \
    --disable-gcov \
    --disable-introspection

make -j"$(($(cat /proc/cpuinfo | grep processor | wc -l)+1))" install

find "${JSON_GLIB_PREFIX_DIR}" -name \*.la -delete

popd > /dev/null
