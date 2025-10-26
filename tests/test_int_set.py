import itertools
import random

import numpy
import pytest

import judy


@pytest.mark.parametrize('shuffled_insertion', [True, False])
def test_insert_query_clear(*, shuffled_insertion: bool) -> None:
    s = judy.JudyIntSet()

    assert len(s) == 0
    assert list(s) == []

    keys = list(range(0, 10_000, 16))

    if shuffled_insertion:
        random.shuffle(keys)

    for key in keys:
        s.add(key)

    for i, k in enumerate(sorted(keys)):
        assert s.by_index(i) == k
        assert s[i] == k

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
        judy_set.add(random.randint(0, 2**64 - 1))

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
        assert sorted(key for key in py_set if lower <= key <= upper) == list(
            judy_set.ranged_iterator(lower_inclusive=lower, upper_inclusive=upper)
        )


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
    n = 1000

    a8 = numpy.random.randint(low=0, high=2**8 - 1, size=n, dtype=numpy.uint8)
    assert sorted(set(a8.tolist())) == sorted(judy.JudyIntSet.FromArray(a8))

    a16 = numpy.random.randint(low=0, high=2**16 - 1, size=n, dtype=numpy.uint16)
    assert sorted(set(a16.tolist())) == sorted(judy.JudyIntSet.FromArray(a16))

    a32 = numpy.random.randint(low=0, high=2**32, size=n, dtype=numpy.uint32)
    assert sorted(set(a32.tolist())) == sorted(judy.JudyIntSet.FromArray(a32))

    a64 = numpy.random.randint(low=0, high=2**64 - 1, size=n, dtype=numpy.uint64)
    assert sorted(set(a64.tolist())) == sorted(judy.JudyIntSet.FromArray(a64))


@pytest.mark.skip(reason='This test is currently under development')
def test_concurrent_mutations() -> None:
    raise NotImplementedError


def test_sizeof() -> None:
    s = judy.JudyIntSet()
    empty = s.__sizeof__()

    for _ in range(1000):
        s.add(_)

    bigger = s.__sizeof__()
    assert empty < bigger
