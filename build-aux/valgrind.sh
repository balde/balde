#!/bin/bash

set -e

dir="$(dirname "$(realpath "$0")")"

export G_SLICE=always-malloc
export G_DEBUG=gc-friendly
export LC_ALL=C

export TESTS_ENVIRONMENT="
	${VALGRIND:-valgrind} \
		--tool=memcheck \
		--leak-check=full \
		--leak-resolution=high \
		--show-leak-kinds=definite \
		--errors-for-leak-kinds=definite \
		--num-callers=20 \
		--error-exitcode=1 \
		--suppressions=${dir}/glib.supp"

if [[ "${1}" == *.sh ]]; then
	exec "${@}"
else
	exec ${TESTS_ENVIRONMENT} "${@}"
fi
