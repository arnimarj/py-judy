import itertools
import random

import judy


def _shuffled(s: list[int]) -> list[int]:
    s = s[:]
    random.shuffle(s)
    return s


def test_insert_query_clear() -> None:
    # basic insert, query, clear
    s = judy.JudyIntSet()

    assert len(s) == 0
    assert list(s) == []

    for key in _shuffled(list(range(0, 10_000, 16))):
        s.add(key)

    assert len(s) == 625
    assert list(s) == list(range(0, 10_000, 16))

    for key in range(0, 10_000, 16):
        assert key in s

    for key in range(1, 10_000, 16):
        assert key not in s

    s.clear()
    assert len(s) == 0

    for key in range(0, 10_000, 16):
        assert key not in s


def test_remove() -> None:
    raise NotImplementedError


def test_iterators() -> None:
    raise NotImplementedError


def test_ranged_iterators() -> None:
    judy_set = judy.JudyIntSet()
    py_set = {random.randint(0, 10_000) for _ in range(1000)}

    for key in py_set:
        judy_set.add(key)

    assert len(judy_set) == len(py_set)
    assert list(judy_set) == sorted(py_set)

    lowers = list(range(0, 10_000, 1000))
    uppers = list(range(0, 10_000, 1000))

    for lower in lowers:
        assert sorted(key for key in py_set if key >= lower) == list(judy_set.__iter__(lower_inclusive=lower))

    for upper in uppers:
        assert sorted(key for key in py_set if key <= upper) == list(judy_set.__iter__(upper_inclusive=upper))

    for lower, upper in itertools.product(lowers, uppers):
        assert sorted(key for key in py_set if lower <= key <= upper) == list(judy_set.__iter__(lower_inclusive=lower, upper_inclusive=upper))

def test_from_array() -> None:
    raise NotImplementedError


def test_concurrent_mutations() -> None:
    raise NotImplementedError
