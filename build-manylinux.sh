#!/bin/bash

set -e

mkdir -p ./wheelhouse

docker run --rm -e PLAT=manylinux2010_x86_64 -v "$(pwd):/io" quay.io/pypa/manylinux2010_x86_64:latest    /io/travis/build-wheels.sh
docker run --rm -e PLAT=manylinux2014_x86_64 -v "$(pwd):/io" quay.io/pypa/manylinux2014_x86_64:latest    /io/travis/build-wheels.sh

docker run --rm -e PLAT=manylinux2010_x86_64 -v "$(pwd):/io" pypywheels/manylinux2010-pypy_x86_64:latest /io/travis/build-wheels.sh

ls -l ./wheelhouse

# make sure we clean up ./build since it gets root permision
sudo rm -rf ./build/
