import random
from collections.abc import Sequence

import pytest

import judy


def _contains_items(m: judy.JudyIntIntMap, sorted_dedup_items: Sequence[tuple[int, int]]) -> None:
    assert len(m) == len(sorted_dedup_items)
    assert list(m) == [k for k, _ in sorted_dedup_items]
    assert list(m.keys()) == [k for k, _ in sorted_dedup_items]
    assert list(m.items()) == list(sorted_dedup_items)
    assert list(m.values()) == [v for _, v in sorted_dedup_items]


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


# FromArray
# delitem
# repr/str
# pop
# ranged-iterators

# copy
# fromkeys
# get
# pop
# popitem
# setdefault
# update
