#!/bin/bash
#
# This script builds and installs several versions of balde in parallel.
#
# You should install the dependencies before running this script
#
#     $ sudo apt-get install -y pkg-config gettext zlib1g-dev libffi-dev \
#         autoconf automake1.11 build-essential libtool libxml2-utils \
#         libfcgi-dev shared-mime-info libglib2.0-dev
#
# To run it, run the following commands:
#
#     $ sudo mkdir /opt/balde
#     $ sudo chown jenkins.jenkins /opt/balde
#     $ sudo -u jenkins bash .build-balde.sh
#
# It will install the bundle to /opt/balde
#

set -xe

BALDE_VERSIONS=( 0.1.1 )

OUTPUT_DIR="/opt/balde"
BUILD_DIR="/tmp/balde-build"

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

for release in ${BALDE_VERSIONS[@]}; do
    local_src_dir="${BUILD_DIR}/balde-${release}"
    local_build_dir="${BUILD_DIR}/balde-${release}-build"
    local_output_dir="${OUTPUT_DIR}/${release}"
    wget \
        --directory-prefix="${BUILD_DIR}" \
        "https://github.com/balde/balde/releases/download/v${release}/balde-${release}.tar.xz"
    tar \
        --extract \
        --verbose \
        --file "${BUILD_DIR}/balde-${release}.tar.xz" \
        --directory "${BUILD_DIR}"
    mkdir -p "${local_output_dir}" "${local_build_dir}"
    pushd "${local_build_dir}" > /dev/null
    "${local_src_dir}/configure" --prefix="${local_output_dir}"
    make -j"$(($(cat /proc/cpuinfo | grep processor | wc -l)+1))"
    make install
    popd > /dev/null
done

echo Done.
