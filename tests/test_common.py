from typing import TypeAlias

import pytest

import judy


JudyContainer: TypeAlias = judy.JudyIntSet | judy.JudyIntIntMap | judy.JudyIntObjectMap
JudyContainers: tuple[type[JudyContainer], ...] = (judy.JudyIntSet, judy.JudyIntIntMap, judy.JudyIntObjectMap)


@pytest.mark.parametrize('klass', JudyContainers)
def test_empty_by_index(klass: type[JudyContainer]) -> None:
    j = klass()

    for index in (-100, 100):
        with pytest.raises(IndexError):
            j.by_index(index)


@pytest.mark.parametrize('klass', JudyContainers)
def test_empty_clear(klass: type[JudyContainer]) -> None:
    j = klass()
    assert len(j) == 0
    assert list(j) == []
    j.clear()
    assert len(j) == 0


@pytest.mark.parametrize('klass', JudyContainers)
def test_compare(klass: type[JudyContainer]) -> None:
    s = klass()
    t = klass()

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
