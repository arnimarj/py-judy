import judy as j


def test_judy_int_set() -> None:
    s = j.JudyIntSet()
    assert len(s) == 0
    assert dir(s) == []


def test_judy_int_int_map() -> None:
    s = j.JudyIntIntMap()

    assert [] == list(s)
    assert [] == list(s.keys())
    assert [] == list(s.values())
    assert [] == list(s.items())

    s[1] = 2

    assert [1] == list(s)
    assert [1] == list(s.keys())
    assert [2] == list(s.values())
    assert [(1, 2)] == list(s.items())
