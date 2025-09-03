TBD

KeysView/ValuesView/ItemsView for iterators
TestSuite
CI+Build Setup
StressTestSuite for free threading
Ranged Iterators
(batched) BisectLeft


NOTES

cmake -S . -B build
 	cmake --build build
cmake -S . -B build -DPython_EXECUTABLE=`which python3`

cibuildwheel --config-file pyproject.toml --output-dir wheelhouse


DONE
* Auto Stub Generation
* Make classes generic
* Avoid installing CPP files on pip-install
