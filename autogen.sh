#!/bin/bash

export WANT_AUTOCONF_2_5="yes"
export WANT_AUTOMAKE_1_11="yes"
export WANT_AUTOCONF="2.5"
export WANT_AUTOMAKE="1.11"

autoreconf -W all -i
