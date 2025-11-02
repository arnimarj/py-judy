#!/bin/bash

set -e

(
	cd src/judy && ln -fs ../../build/_judy_nb.cpython-313-x86_64-linux-gnu.so _judy_nb.so
)

cmake --build build
# cmake -S . -B build -DPython_EXECUTABLE=$(which python3)
PYTHONPATH=./src/ pytest tests/
