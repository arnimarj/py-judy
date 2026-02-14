all: local-build local-test

local-build:
	uv --project .venv pip install nanobind scikit-build-core[pyproject] pytest
	uv --project .venv pip install --no-deps --no-build-isolation --force-reinstall -C build-dir="$(shell pwd)/build" .

local-test:
	@VIRTUAL_ENV=.venv uv --project .venv run pytest .
