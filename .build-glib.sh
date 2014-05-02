#!/bin/bash
#
# This script builds and installs several versions of GLib in parallel.
#
# You should install the dependencies before running this script
#
#     $ sudo apt-get install -y pkg-config gettext zlib1g-dev libffi-dev \
#         autoconf automake1.11 build-essential libtool libxml2-utils
#
# It will install the bundle to /opt/glib

set -xe

GLIB_VERSIONS=( 2.34.3 2.38.2 2.40.0 )

OUTPUT_DIR="/opt/glib"
BUILD_DIR="/tmp/glib-build"

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

for release in ${GLIB_VERSIONS[@]}; do
    version="${release%.*}"
    local_src_dir="${BUILD_DIR}/glib-${release}"
    local_build_dir="${BUILD_DIR}/glib-${release}-build"
    local_output_dir="${OUTPUT_DIR}/${release}"
    wget \
        --directory-prefix="${BUILD_DIR}" \
        "http://ftp.gnome.org/pub/gnome/sources/glib/${version}/glib-${release}.tar.xz"
    tar \
        --extract \
        --verbose \
        --file "${BUILD_DIR}/glib-${release}.tar.xz" \
        --directory "${BUILD_DIR}"
    mkdir -p "${local_output_dir}" "${local_build_dir}"
    pushd "${local_build_dir}" > /dev/null
    "${local_src_dir}/configure" --prefix="${local_output_dir}"
    make -j"$(($(cat /proc/cpuinfo | grep processor | wc -l)+1))"
    make install
    popd > /dev/null
done

echo Done.
