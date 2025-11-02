import itertools
import random
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


@pytest.mark.parametrize('klass', JudyMaps)
def test_ranged_iterators(klass: type[JudyMap]) -> None:
    judy_map = klass()
    UPPER = 10_000
    COUNT = 10_000
    RANGE_INC = 1_000

    keys = {random.randint(0, UPPER) for _ in range(COUNT)}

    for key in keys:
        judy_map[key] = key * 2

    s_keys = sorted(keys)
    s_values = [key * 2 for key in s_keys]

    assert len(judy_map) == len(keys)
    assert list(judy_map) == sorted(keys)

    lowers = list(range(0, UPPER, RANGE_INC))
    uppers = list(range(0, UPPER, RANGE_INC))

    def _in_range(*, lower_inclusive: int | None = None, upper_inclusive: int | None = None) -> list[tuple[int, int]]:
        return [
            (key, value)
            for key, value in zip(s_keys, s_values)
            if (lower_inclusive is None or key >= lower_inclusive)
            and (upper_inclusive is None or key <= upper_inclusive)
        ]

    for lower in lowers:
        assert [kv[0] for kv in _in_range(lower_inclusive=lower)] == list(judy_map.keys(lower_inclusive=lower))
        assert [kv[1] for kv in _in_range(lower_inclusive=lower)] == list(judy_map.values(lower_inclusive=lower))
        assert _in_range(lower_inclusive=lower) == list(judy_map.items(lower_inclusive=lower))

    for upper in uppers:
        assert [kv[0] for kv in _in_range(upper_inclusive=upper)] == list(judy_map.keys(upper_inclusive=upper))
        assert [kv[1] for kv in _in_range(upper_inclusive=upper)] == list(judy_map.values(upper_inclusive=upper))
        assert _in_range(upper_inclusive=upper) == list(judy_map.items(upper_inclusive=upper))

    for lower, upper in itertools.product(lowers, uppers):
        assert [kv[0] for kv in _in_range(lower_inclusive=lower, upper_inclusive=upper)] == list(
            judy_map.keys(lower_inclusive=lower, upper_inclusive=upper)
        )
        assert [kv[1] for kv in _in_range(lower_inclusive=lower, upper_inclusive=upper)] == list(
            judy_map.values(lower_inclusive=lower, upper_inclusive=upper)
        )
        assert _in_range(lower_inclusive=lower, upper_inclusive=upper) == list(
            judy_map.items(lower_inclusive=lower, upper_inclusive=upper)
        )


@pytest.mark.parametrize('klass', JudyMaps)
def test_set_del(klass: type[JudyMap]) -> None:
    m = klass()
    assert len(m) == 0

    m[10] = 20
    assert len(m) == 1

    m[10] = 30
    assert len(m) == 1
