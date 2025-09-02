TBD

KeysView/ValuesView/ItemsView for iterators
Make classes generic
TestSuite
CI+Build Setup
StressTestSuite for free threading
Auto Stub Generation
Avoid installing CPP files on pip-install


NOTES

cmake -S . -B build
cmake --build build
cmake -S . -B build -DPython_EXECUTABLE=`which python3`

cibuildwheel --config-file pyproject.toml --output-dir wheelhouse
