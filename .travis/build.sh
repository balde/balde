#!/bin/bash

set -ex

if [[ -n "${TARGET}" ]] && [[ -e ".travis/targets/${TARGET}.sh" ]]; then
    source ".travis/targets/${TARGET}.sh"
else
    echo "Target not defined or invalid!"
    exit 1
fi

rm -rf build
mkdir -p build

build
