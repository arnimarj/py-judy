import judy

from .test_utils import judy_set_contains_items


def test_empty() -> None:
    s = judy.JudyIntIntMap()
    assert len(s) == 0

    for _ in range(2):
        assert [] == list(s)
        assert [] == list(s.keys())
        assert [] == list(s.values())
        assert [] == list(s.items())
        s.clear()

    judy_set_contains_items(s, [])
