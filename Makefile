all: local-build local-test

ensure-venv:
	@uv venv --no-project --allow-existing .venv 

local-build: ensure-venv
	uv --project .venv pip install nanobind scikit-build-core[pyproject] pytest
	uv --project .venv pip install --no-deps --no-build-isolation --force-reinstall -C build-dir="$(shell pwd)/build" .

local-test: ensure-venv
	@VIRTUAL_ENV=.venv uv --project .venv run pytest .
