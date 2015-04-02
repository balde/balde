#!/bin/bash

set -ex

source "$(dirname $0)/settings.sh"

sudo apt-get update
sudo apt-get -o dir::cache::archives="${APT_CACHE}" install -y libffi-dev libfcgi-dev peg valgrind libxml2-utils clang

rm -rf "${GLIB_BUILD}" "${JSON_GLIB_BUILD}"
mkdir -p "${GLIB_BUILD}" "${JSON_GLIB_BUILD}"

for glib_release in ${GLIB_VERSIONS[@]}; do
    /bin/bash "$(dirname $0)/build-glib.sh" "${glib_release}"
    for json_glib_release in ${JSON_GLIB_VERSIONS[@]}; do
        err=0
        for blacklisted in ${BLACKLIST[@]}; do
            if [[ "${glib_release}/${json_glib_release}" = "${blacklisted}" ]]; then
                err=1
                break
            fi
        done
        [[ ${err} = 1 ]] && continue
        /bin/bash "$(dirname $0)/build-json-glib.sh" "${glib_release}" "${json_glib_release}"
    done
done

echo Done.
