#!/bin/bash

set -xe

mkdir -p ./wheelhouse


pip install -U pip virtualenv
python -m virtualenv venv
./venv/bin/python -m pip install cibuildwheel==3.3.0 setuptools==80.9.0
./venv/bin/cibuildwheel --config-file pyproject.toml --output-dir wheelhouse


pip install pipx
pipx run build --sdist --outdir ./wheelhouse .

# test it
python -m virtualenv venv

./venv/bin/pip install ./wheelhouse/*.gz
