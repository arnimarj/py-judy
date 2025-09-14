TBD

KeysView/ValuesView/ItemsView for iterators
TestSuite
CI+Build Setup
StressTestSuite for free threading
(batched) BisectLeft
IntObjectMap
test IntObjectMap ref-cycles under free-threading (do we need mutex lock under traverse/clear?)

consisten API betwewen int-object/int-int maps, and possibly code reuse

NOTES

cmake -S . -B build
 	cmake --build build
cmake -S . -B build -DPython_EXECUTABLE=`which python3`

cibuildwheel --config-file pyproject.toml --output-dir wheelhouse


DONE
* Auto Stub Generation
* Make classes generic
* Avoid installing CPP files on pip-install
* Ranged Iterators
