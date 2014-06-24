#!/bin/bash

set -xe

[[ $# -eq 2 ]]

BUNDLE_RELEASE="${1}"
PEG_RELEASE="${2}"

BUNDLE="balde-ci-dep-bundle_${BUNDLE_RELEASE}_amd64.deb"

wget \
    --continue \
    "https://github.com/balde/ci-dep-bundle/releases/download/v${BUNDLE_RELEASE}/${BUNDLE}"

sudo dpkg -i "${BUNDLE}"

wget \
    --continue \
    "http://piumarta.com/software/peg/peg-${PEG_RELEASE}.tar.gz"

tar -xvf "peg-${PEG_RELEASE}.tar.gz" -C /tmp
make -C "/tmp/peg-${PEG_RELEASE}/"
sudo make -C "/tmp/peg-${PEG_RELEASE}/" install
