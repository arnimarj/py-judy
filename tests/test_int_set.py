import itertools
import random

import pytest
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
    judy_set = judy.JudyIntSet()
    judy_set.discard(10)

    with pytest.raises(KeyError):
        judy_set.remove(10)

    judy_set.add(10)
    assert len(judy_set) == 1

    judy_set.discard(10)
    assert len(judy_set) == 0

    judy_set.add(10)
    assert len(judy_set) == 1

    judy_set.remove(10)
    assert len(judy_set) == 0


def test_iterators() -> None:
    # empty iterator
    judy_set = judy.JudyIntSet()

    with pytest.raises(StopIteration):
        next(iter(judy_set))

    # iterator, starting at empty set,
    # then discovering first item
    it = iter(judy_set)
    judy_set.add(10)
    assert next(it) == 10

    with pytest.raises(StopIteration):
        next(it)

    # iterator finding newly added items when
    # added in order
    it = iter(judy_set)

    for key in range(10, 100, 10):
        judy_set.add(key)
        assert next(it) == key

    with pytest.raises(StopIteration):
        next(it)

    # it will continue to raise StopIteration
    # even if larger keys are added
    judy_set.add(max(judy_set) + 1)

    with pytest.raises(StopIteration):
        next(it)

    # this won't work for descending keys
    judy_set.clear()
    it = iter(judy_set)

    judy_set.add(1000)
    assert next(it) == 1000

    judy_set.add(2000)
    assert next(it) == 2000

    judy_set.add(0)

    with pytest.raises(StopIteration):
        next(it)


def test_overwrite() -> None:
    judy_set = judy.JudyIntSet()
    judy_set.add(0)
    judy_set.add(0)
    assert list(judy_set) == [0]


def test_str_repr() -> None:
    judy_set = judy.JudyIntSet()

    for _ in range(1000):
        judy_set.add(random.randint(0, 0))

    assert str(judy_set) == f'JudyIntSet({sorted(judy_set)})'
    assert repr(judy_set) == f'JudyIntSet({sorted(judy_set)})'


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
        assert sorted(key for key in py_set if key >= lower) == list(judy_set.ranged_iterator(lower_inclusive=lower))

    for upper in uppers:
        assert sorted(key for key in py_set if key <= upper) == list(judy_set.ranged_iterator(upper_inclusive=upper))

    for lower, upper in itertools.product(lowers, uppers):
        assert sorted(key for key in py_set if lower <= key <= upper) == list(judy_set.ranged_iterator(lower_inclusive=lower, upper_inclusive=upper))


def test_pop() -> None:
    judy_set = judy.JudyIntSet()
    py_set = {random.randint(0, 10_000) for _ in range(100)}
    popped = set()

    for key in py_set:
        judy_set.add(key)

    for _ in range(len(judy_set)):
        popped.add(judy_set.pop())

    with pytest.raises(KeyError):
        judy_set.pop()

    assert popped == py_set
    assert len(judy_set) == 0


def test_from_array() -> None:
    raise NotImplementedError


def test_concurrent_mutations() -> None:
    raise NotImplementedError
