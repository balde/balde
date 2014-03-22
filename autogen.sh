#!/bin/bash

export AUTOMAKE="$(which automake-1.11)"
export ACLOCAL="$(which aclocal-1.11)"

autoreconf -W all -i
