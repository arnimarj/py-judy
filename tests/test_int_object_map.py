import sys

import judy


def test_refleaks() -> None:
    obj = object()

    def ref() -> int:
        return sys.getrefcount(obj) - 1

    expected = ref()

    m = judy.JudyIntObjectMap()
    n = 1000

    for _ in range(n):
        m[_] = obj
        expected += 1
        assert expected == ref()

    for _ in range(n // 2):
        del m[_]
        expected -= 1
        assert expected == ref()

    # pop
    m.pop(700)
    expected -= 1
    assert expected == ref()

    # add
    m[700] = obj
    expected += 1
    assert expected == ref()

    # overwrite
    m[700] = obj
    assert expected == ref()

    # failed-pop with default
    m.pop(20000, None)
    assert expected == ref()

    i = m.by_index(10)
    expected += 1
    assert expected == ref()

    del i
    expected -= 1
    assert expected == ref()

    pre_iter = ref()

    for it in (m.values(), m.items()):
        assert expected == ref()

        for _ in it:
            expected += 1
            assert expected == ref()
            expected -= 1

        del _
        assert pre_iter == ref()
        assert expected == ref()

    expected -= len(m)
    m.clear()
    assert expected == ref()


def test_circular_destruction() -> None:
    m = judy.JudyIntObjectMap()

    def ref() -> int:
        return sys.getrefcount(m) - 1  # noqa: F821

    expected = ref()

    m[1] = m
    expected += 1
    assert expected == ref()

    m.clear()
    expected -= 1

    assert expected == ref()

    m[1] = m
    del m
