all: local-build local-test

local-build:
	@uv pip install nanobind scikit-build-core[pyproject] pytest
	@uv pip install --no-deps --no-build-isolation --force-reinstall -C build-dir="$(shell pwd)/build" .

local-test:
	@pytest .
