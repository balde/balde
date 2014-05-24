#!/bin/bash

export AUTOMAKE="$(which automake-1.11)"
export ACLOCAL="$(which aclocal-1.11)"

autoreconf \
    --warnings=all \
    --install \
    --force
