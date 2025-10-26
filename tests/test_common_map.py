from typing import Any, TypeAlias

import pytest

import judy

from .test_utils import judy_dict_contains_items, key_value_gen


JudyMap: TypeAlias = judy.JudyIntIntMap | judy.JudyIntObjectMap[Any]
JudyMaps: tuple[type[JudyMap], ...] = (judy.JudyIntIntMap, judy.JudyIntObjectMap)


@pytest.mark.parametrize('klass', JudyMaps)
def test_insert_query_clear(klass: type[JudyMap]) -> None:
    # basic insert, query, clear
    m = klass()

    key_range = range(0, 16_000, 16)
    value_range = range(16_000, 32_000, 16)

    for key, value in key_value_gen(keys=key_range, values=value_range, shuffle=True):
        with pytest.raises(IndexError):
            m.by_index(len(m))

        m[key] = value

    assert len(m) == len(key_range)
    assert list(m.keys()) == list(key_range)
    assert list(m.keys()) == list(key_range)

    for i, (key, value) in enumerate(zip(iter(key_range), iter(value_range))):
        assert key in m
        assert m[key] == value
        assert m.get(key) == value
        assert m.get(key, 1337) == value
        assert m.by_index(i) == value

    judy_dict_contains_items(m, list(zip(iter(key_range), iter(value_range))))

    for key in range(1, 16_000, 16):
        assert key not in m

        with pytest.raises(KeyError):
            assert m[key]

        with pytest.raises(KeyError):
            assert m.pop(key)

        assert m.pop(key, None) is None

    m.clear()
    assert len(m) == 0

    for key in iter(key_range):
        assert key not in m

        with pytest.raises(KeyError):
            assert m[key] == value

        assert m.get(key) is None
        assert m.get(key, 1337) == 1337
