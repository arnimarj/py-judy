#!/bin/bash

set -e

mkdir -p ./wheelhouse

python3 setup.py sdist --dist-dir ./wheelhouse

# test it
pip3 install -U pip virtualenv
python -m virtualenv venv
./venv/bin/pip install ./wheelhouse/*.gz


ls -l ./wheelhouse
