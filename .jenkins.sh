#!/bin/bash
#
# This script assumes that balde dependencies (other than glib and peg) are
# already installed on the system.
#
# They should be installed by hand if you are going to run a Jenkins slave.
#
#     $ sudo apt-get install -y libfcgi-dev libmagic-dev pkg-config gettext \
#         zlib1g-dev libffi-dev autoconf automake1.11 build-essential libtool \
#         libxml2-utils
#
# Also, it depends on some enviromnent variables, that are setup by Jenkins:
#
#     GLIB_VERSION (e.g. 2.34)
#     GLIB_RELEASE (e.g. 2.34.3)
#     PEG_VERSION  (e.g. 0.1.13)
#

BASE_DIR="/tmp/jenkins"


## GLib variables
GLIB_BASE_DIR="${BASE_DIR}/glib"
GLIB_SRC_DIR="${GLIB_BASE_DIR}/glib-${GLIB_RELEASE}"
GLIB_BUILD_DIR="${GLIB_BASE_DIR}/glib-${GLIB_RELEASE}-build"
GLIB_DESTDIR="${GLIB_BASE_DIR}/glib-${GLIB_RELEASE}-prefix"
GLIB_TARBALL="${GLIB_BASE_DIR}/glib-${GLIB_RELEASE}.tar.xz"
GLIB_URL="http://ftp.gnome.org/pub/gnome/sources/glib/${GLIB_VERSION}/glib-${GLIB_RELEASE}.tar.xz"


## peg variables
PEG_BASE_DIR="${BASE_DIR}/peg"
PEG_SRC_DIR="${PEG_BASE_DIR}/peg-${PEG_VERSION}"
PEG_TARBALL="${PEG_BASE_DIR}/peg-${PEG_VERSION}.tar.gz"
PEG_URL="http://piumarta.com/software/peg/peg-${PEG_VERSION}.tar.gz"


## balde variables
BALDE_SRC_DIR="$(pwd)"
BALDE_BUILD_DIR="${BALDE_SRC_DIR}/build"


## Build GLib, if needed.
if [[ ! -x "${GLIB_DESTDIR}/bin/gtester" ]]; then
    rm -rf "${GLIB_DESTDIR}"
    mkdir -p "${GLIB_BUILD_DIR}" "${GLIB_DESTDIR}"
    wget -q -O "${GLIB_TARBALL}" "${GLIB_URL}"
    tar -xf "${GLIB_TARBALL}" -C "${GLIB_BASE_DIR}"
    pushd "${GLIB_BUILD_DIR}" > /dev/null
    "${GLIB_SRC_DIR}/configure" --prefix="${GLIB_DESTDIR}"
    make
    make install
    popd > /dev/null
fi


## Build Peg, if needed.
if [[ ! -x "${PEG_SRC_DIR}/peg" ]]; then
    mkdir -p "${PEG_SRC_DIR}"
    wget -q -O "${PEG_TARBALL}" "${PEG_URL}"
    tar -xf "${PEG_TARBALL}" -C "${PEG_BASE_DIR}"
    pushd "${PEG_SRC_DIR}" > /dev/null
    make
    popd > /dev/null
fi


## balde needs to know where to look for glib stuff
export PKG_CONFIG_LIBDIR="${GLIB_DESTDIR}/lib/pkgconfig"
export PATH="${GLIB_DESTDIR}/bin:${PEG_SRC_DIR}:${PATH}"


rm -rf "${BALDE_BUILD_DIR}"

./autogen.sh

mkdir -p "${BALDE_BUILD_DIR}"
pushd "${BALDE_BUILD_DIR}" > /dev/null
"${BALDE_SRC_DIR}"/configure --enable-examples --with-leg
popd > /dev/null

make -C "${BALDE_BUILD_DIR}" check
