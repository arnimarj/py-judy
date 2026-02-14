VIRTUAL_ENV := ".venv"

all: local-build local-test

local-build:
	uv --project ${VIRTUAL_ENV} pip install nanobind scikit-build-core[pyproject] pytest
	uv --project ${VIRTUAL_ENV} pip install --no-deps --no-build-isolation --force-reinstall -C build-dir="$(shell pwd)/build" .

local-test:
	uv --project ${VIRTUAL_ENV} run pytest .
