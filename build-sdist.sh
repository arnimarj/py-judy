#!/bin/bash

set -xe

mkdir -p ./wheelhouse

# pip install pipx
pipx run build --sdist --outdir ./wheelhouse .

# test it
pip install -U pip virtualenv
python -m virtualenv venv

cmake -S . -B build
cmake --build build
./venv/bin/pip install ./wheelhouse/*.gz
