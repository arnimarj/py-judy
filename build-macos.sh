#!/bin/bash

set -e

mkdir ./wheelhouse

basename="$(dirname "$0")"
git clone https://github.com/pyenv/pyenv.git "$basename/.pyenv"

mkdir -p "$basename/wheelhouse/"

export PYENV_ROOT="$basename/.pyenv"
export PATH="$PYENV_ROOT/bin:$PATH"

eval "$(pyenv init -)"

declare -a pythons=("3.5.9" "3.6.9" "3.7.7" "3.8.2")

for py in "${pythons[@]}"
do
	echo "installing $py"
	pyenv install -s "$py"
	pyenv shell "$py"
	pip install -U virtualenv > /dev/null
	python -m virtualenv -q "venv-$py"
	echo " ..version installed" "$(./venv-"$py"/bin/python --version)"

	"./venv-$py/bin/pip" install -U pip setuptools wheel > /dev/null
	rm -rf ./dist/ ./build/
	"./venv-$py/bin/python" setup.py clean > /dev/null
	"./venv-$py/bin/python" setup.py bdist_wheel > /dev/null
	cp dist/*.whl ./wheelhouse/
done

ls -l ./wheelhouse
