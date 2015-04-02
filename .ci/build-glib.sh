#!/bin/bash

set -ex

[[ $# -eq 1 ]]

source "$(dirname $0)/settings.sh"

GLIB_RELEASE="${1}"

GLIB_VERSION="${GLIB_RELEASE%.*}"
GLIB_SRC_DIR="${GLIB_BUILD}/glib-${GLIB_RELEASE}"
GLIB_BUILD_DIR="${GLIB_BUILD}/glib-${GLIB_RELEASE}-build"
GLIB_PREFIX_DIR="${GLIB_PREFIX}/${GLIB_RELEASE}"

[[ -d "${GLIB_PREFIX_DIR}" ]] && exit 0

wget \
    --continue \
    --directory-prefix="${GLIB_BUILD}" \
    "http://ftp.gnome.org/pub/gnome/sources/glib/${GLIB_VERSION}/glib-${GLIB_RELEASE}.tar.xz"

tar \
    --extract \
    --verbose \
    --file "${GLIB_BUILD}/glib-${GLIB_RELEASE}.tar.xz" \
    --directory "${GLIB_BUILD}"

rm -rf "${GLIB_BUILD_DIR}"
mkdir -p "${GLIB_BUILD_DIR}"

pushd "${GLIB_BUILD_DIR}" > /dev/null

"${GLIB_SRC_DIR}/configure" \
    --prefix="${GLIB_PREFIX_DIR}" \
    --with-threads=posix \
    --disable-selinux \
    --disable-xattr \
    --disable-fam \
    --disable-static \
    --disable-dtrace \
    --disable-systemtap \
    --disable-compile-warnings \
    --disable-man

make -j"$(($(cat /proc/cpuinfo | grep processor | wc -l)+1))" install

find "${GLIB_PREFIX_DIR}" -name \*.la -delete

popd > /dev/null
