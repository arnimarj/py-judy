import numpy
import pytest

import judy

from .test_utils import judy_set_contains_items


def test_empty() -> None:
    s = judy.JudyIntIntMap()
    assert len(s) == 0

    for _ in range(2):
        assert [] == list(s)
        assert [] == list(s.keys())
        assert [] == list(s.values())
        assert [] == list(s.items())
        s.clear()

    judy_set_contains_items(s, [])


def test_from_array() -> None:
    n = 1000

    a64_empty = numpy.random.randint(low=0, high=2**64 - 1, size=0, dtype=numpy.uint64)

    # we only support 64-bit variants
    keys = numpy.random.randint(low=0, high=2**64 - 1, size=n, dtype=numpy.uint64)
    values = numpy.random.randint(low=0, high=2**64 - 1, size=n, dtype=numpy.uint64)

    py_m = {k.item(): v.item() for k, v in zip(keys, values)}
    py_k = sorted(py_m)
    py_v = [v for k, v in sorted(py_m.items())]
    py_kv = list(zip(py_k, py_v))

    m = judy.JudyIntIntMap.FromArray(keys, values)

    assert py_k == list(m)
    assert py_k == list(m.keys())
    assert py_v == list(m.values())
    assert py_kv == list(m.items())

    with pytest.raises(RuntimeError):
        judy.JudyIntIntMap.FromArray(a64_empty, values)


def test_str_repr() -> None:
    m = judy.JudyIntIntMap()

    for _ in range(10):
        m[_] = _ * 2

    payload = '{' + ', '.join(f'{k}: {v}' for k, v in m.items()) + '}'

    assert str(m) == f'JudyIntIntMap({payload})'
    assert repr(m) == f'JudyIntIntMap({payload})'
