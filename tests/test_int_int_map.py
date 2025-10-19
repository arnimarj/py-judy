import pytest
import random
from collections.abc import Sequence

import judy


def _contains_items(m: judy.JudyIntIntMap, sorted_dedup_items: Sequence[tuple[int, int]]) -> None:
    assert len(m) == len(sorted_dedup_items)
    assert list(m) == [k for k, _ in sorted_dedup_items]
    assert list(m.keys()) == [k for k, _ in sorted_dedup_items]
    assert list(m.items()) == list(sorted_dedup_items)
    assert list(m.values()) == list(sorted_dedup_items)


def test_empty() -> None:
    s = judy.JudyIntIntMap()
    assert len(s) == 0

    for _ in range(2):
        assert [] == list(s)
        assert [] == list(s.keys())
        assert [] == list(s.values())
        assert [] == list(s.items())
        s.clear()

    _contains_items(s, [])


def _shuffled_key_value_gen(keys: range, values: range) -> list[tuple[int, int]]:
    assert len(keys) == len(values)
    keys_and_values = list(zip(iter(keys), iter(values)))
    random.shuffle(keys_and_values)
    return keys_and_values


def test_insert_query_clear() -> None:
    # basic insert, query, clear
    m = judy.JudyIntIntMap()

    key_range = range(0, 16_000, 16)
    value_range = range(16_000, 32_000, 16)

    for key, value in _shuffled_key_value_gen(keys=key_range, values=value_range):
        m[key] = value

    assert len(m) == len(key_range)
    assert list(m.keys()) == list(key_range)
    assert list(m.keys()) == list(key_range)

    for key, value in zip(iter(key_range), iter(value_range)):
        assert key in m
        assert m[key] == value
        assert m.get(key) == value
        assert m.get(key, 1337) == value

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


# FromArray
# contains
# delitem
# eq
# getiem
# iter
# repr/str
# by_index
# clear/get/pop

# clear
# copy
# fromkeys
# get
# pop
# popitem
# setdefault
# update 

def test_compare() -> None:
    s = judy.JudyIntIntMap()
    t = judy.JudyIntIntMap()

    with pytest.raises(TypeError):
        s < t

    with pytest.raises(TypeError):
        s > t

    with pytest.raises(TypeError):
        s <= t

    with pytest.raises(TypeError):
        s >= t

    with pytest.raises(TypeError):
        s == t

    with pytest.raises(TypeError):
        s != t
