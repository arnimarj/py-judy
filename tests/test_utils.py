import random
from collections.abc import Sequence

import judy


def key_value_gen(keys: range, values: range, *, shuffle: bool) -> list[tuple[int, int]]:
    assert len(keys) == len(values)
    keys_and_values = list(zip(iter(keys), iter(values)))

    if shuffle:
        random.shuffle(keys_and_values)

    return keys_and_values


def judy_dict_contains_items(
    m: judy.JudyIntIntMap | judy.JudyIntObjectMap[int], sorted_dedup_items: Sequence[tuple[int, int]]
) -> None:
    assert len(m) == len(sorted_dedup_items)
    assert list(m) == [k for k, _ in sorted_dedup_items]
    assert list(m.keys()) == [k for k, _ in sorted_dedup_items]
    assert list(m.items()) == list(sorted_dedup_items)
    assert list(m.values()) == [v for _, v in sorted_dedup_items]


def judy_set_contains_items(m: judy.JudyIntSet, sorted_dedup_items: Sequence[tuple[int, int]]) -> None:
    assert len(m) == len(sorted_dedup_items)
    assert list(m) == [k for k, _ in sorted_dedup_items]
    assert list(m.keys()) == [k for k, _ in sorted_dedup_items]
    assert list(m.items()) == list(sorted_dedup_items)
    assert list(m.values()) == [v for _, v in sorted_dedup_items]
