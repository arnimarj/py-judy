#!/bin/bash

set -ex

mkdir -p ./wheels

_clean() {
	rm -rf /io/build/
	rm -rf /io/dist/
	rm -rf /io/judy.egg-info/
	rm -rf /io/judy-1.0.5/src/libJudy.a
}

for PYBIN in /opt/python/*/bin; do
	echo "$PYBIN"

	if [[ $PYBIN == *"pypy3"* ]]; then
		_clean;
		"${PYBIN}/pip" install -U pip
		"${PYBIN}/pip" wheel /io/ -w wheels/
	elif [[ $PYBIN =~ cp36|cp37|cp38|cp39|cp310 ]]; then
		_clean;
		"${PYBIN}/pip" install -U pip
		"${PYBIN}/pip" wheel /io/ -w wheels/
	fi
done

for whl in wheels/*.whl; do
	auditwheel repair "$whl" --plat "$PLAT" -w ./wheelhouse/
done

cp wheelhouse/*.whl /io/wheelhouse/

_clean;
