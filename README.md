 Feature TBD
-----------

 * set.update
 * Keys/Items/ValueView for dict iterators
 * ranged-iterators which don't conflict with `keys()/items()/values()`
 * batched bi-section
 * implement `Next(key, default=...)` which returns smallest key LE to `key`
 * `judy.JudyIntSet()` to numpy array
 * `judy.JudyIntIntMap()` to numpy arrays
 * `set/dict.by_index()`

Quality TBD
-----------

 * stress test under ASAN/TSAN in free-threading Python
 * add concurrency tests under  free-threading Python
 * consolidate exposed interfaces for `judy.JudyIntObjectMap/JudyIntIntMap`
 * test ref-cycles in `judy.JudyIntObjectMap`
 * test ref-leaks in `judy.JudyIntObjectMap` iterators
 * implement cycle-detector for `judy.JudyIntObjectMap.__str__()/__repr__()`

Potential Future Features
-------------------------

 * `set/dict.copy()`
 * `set/dict.update()`
 * `set.discard()`

 * `set.difference/intersection/symmetric_difference`
 * `..._update()` variants of these 3
 * `set.isdisjoint/issubset/issuperset`
 * `set.pop/remove()`
 * `dict.pop/popitem()`
 * `dict.setdefault()`
 * `dict.fromkeys`


Notes:
------
 * `PIP_INDEX_URL=https://pypi.org/simple pip install nanobind scikit-build-core`
 * `cmake -S . -B build`
 * `cmake --build build`
 * `cmake -S . -B build -DPython_EXECUTABLE=$(which python3)`
 * `cibuildwheel --config-file pyproject.toml --output-dir wheelhouse`
