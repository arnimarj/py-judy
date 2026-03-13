#!/bin/bash

set -xe

mkdir -p ./wheelhouse


pip install -U pip virtualenv
python -m virtualenv venv
./venv/bin/python -m pip install cibuildwheel==3.4.0 setuptools==82.0.1
./venv/bin/cibuildwheel --config-file pyproject.toml --output-dir wheelhouse


pip install pipx
pipx run build --sdist --outdir ./wheelhouse .

# test it
python -m virtualenv venv

./venv/bin/pip install ./wheelhouse/*.gz
