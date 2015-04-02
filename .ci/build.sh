#!/bin/bash

set -ex

source "$(dirname $0)/settings.sh"

for glib_release in ${GLIB_VERSIONS[@]}; do
    for json_glib_release in ${JSON_GLIB_VERSIONS[@]}; do
        err=0
        for blacklisted in ${BLACKLIST[@]}; do
            if [[ "${glib_release}/${json_glib_release}" = "${blacklisted}" ]]; then
                err=1
                break
            fi
        done
        [[ ${err} = 1 ]] && continue
        echo -e "\n\n\n\n\n#################### GLib-${glib_release} - Json-GLib-${json_glib_release} ####################\n\n\n\n\n"
        if ! /bin/bash "$(dirname $0)/build-balde.sh" "${glib_release}" "${json_glib_release}"; then
            echo "Failed: GLib-${glib_release} - Json-GLib-${json_glib_release}"
            exit 1
        fi
    done
done

echo Done.
