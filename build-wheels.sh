#!/bin/bash

set -xe

mkdir -p ./wheelhouse


pip install -U pip virtualenv
python -m virtualenv venv
./venv/bin/python -m pip install cibuildwheel==4.3.4 setuptools==84.2.1
./venv/bin/cibuildwheel --config-file pyproject.toml --output-dir wheelhouse


pip install pipx
pipx run build --sdist --outdir ./wheelhouse .

# test it
python -m virtualenv venv

./venv/bin/pip install ./wheelhouse/*.gz
