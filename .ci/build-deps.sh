#!/bin/bash

set -ex

source "$(dirname $0)/settings.sh"

sudo apt-get update
sudo apt-get -o dir::cache::archives="${APT_CACHE}" install -y libffi-dev peg valgrind libxml2-utils clang

rm -rf "${GLIB_BUILD}"
mkdir -p "${GLIB_BUILD}"

for glib_release in ${GLIB_VERSIONS[@]}; do
    /bin/bash "$(dirname $0)/build-glib.sh" "${glib_release}"
done

echo Done.
