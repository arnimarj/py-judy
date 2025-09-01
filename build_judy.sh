#!/bin/bash

bash --version

set -xe

basename="$(dirname "$0")"

if [[ ${CC} ]]; then
	JUDYCC="$CC"
else
	JUDYCC=gcc
fi

if [[ ! ${COPT} ]]; then
	COPT="-DJU_64BIT -O0 -fPIC -fno-strict-aliasing"
fi


if $JUDYCC -v 2>&1 >/dev/null | grep -E "clang|gcc version 4.6"; then
	COPT+=""
else
	COPT+=" -fno-aggressive-loop-optimizations"
fi

(cd "$basename"; cd judy-1.0.5/src; CC="$JUDYCC" COPT="$COPT" sh ./sh_build)
