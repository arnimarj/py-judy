#!/bin/bash

test -n "$(shopt -s nullglob; echo /opt/python/*pypy3*)"
is_mypy=$?

set -ex

mkdir -p ./wheels

for PYBIN in /opt/python/*/bin; do
	# pypy, but only pypy3
	if [ $is_mypy -eq 0 ]; then
		if [[ $PYBIN == *"pypy3"* ]]; then

			"${PYBIN}/pip" wheel /io/ -w wheels/
		fi
	# cpython
	else
		if [[ $PYBIN =~ cp35|cp36|cp37|cp38 ]]; then

			"${PYBIN}/pip" wheel /io/ -w wheels/
		fi
	fi
done

for whl in wheels/*.whl; do
	auditwheel repair "$whl" --plat "$PLAT" -w ./wheelhouse/
done

cp wheelhouse/*.whl /io/wheelhouse/
