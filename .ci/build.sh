#!/bin/bash

set -ex

source "$(dirname $0)/settings.sh"

for glib_release in ${GLIB_VERSIONS[@]}; do
    echo -e "\n\n\n\n\n#################### GLib-${glib_release} ####################\n\n\n\n\n"
    if ! /bin/bash "$(dirname $0)/build-balde.sh" "${glib_release}"; then
        echo "Failed: GLib-${glib_release}"
        exit 1
    fi
done

echo Done.
