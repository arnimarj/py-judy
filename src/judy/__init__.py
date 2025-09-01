from collections.abc import Iterator
from typing import Any, Literal, NoReturn, overload, TypeVar

from ._judy_nb import _JudyIntIntMap, _JudyIntSet

_T = TypeVar('_T')


class JudyIntIntMap:
    def __init__(self):
        self._m = _JudyIntIntMap()

    @overload
    def __contains__(self, value: int) -> bool:
        ...

    @overload
    def __contains__(self, value: Any) -> Literal[False]:
        ...

    def __contains__(self, value: Any) -> bool:
        return (value in self._m)

    def __len__(self) -> int:
        return len(self._m)

    def clear(self) -> None:
        return self._m.clear()

    @overload
    def __getitem__(self, index: int) -> int:
        ...

    @overload
    def __getitem__(self, index: Any) -> NoReturn:
        ...

    def __getitem__(self, index: int) -> int:
        return self._m[index]

    def __str__(self) -> str:
        return self._m.__str__()

    def __repr__(self) -> str:
        return self._m.__repr__()

    def __sizeof__(self) -> int:
        return self._m.__sizeof__()

    @overload
    def get(self, key: int) -> int | None:
        ...

    @overload
    def get(self, key: Any) -> None:
        ...

    @overload
    def get(self, key: int, default: _T) -> int | _T:
        ...

    @overload
    def get(self, key: Any, default: _T) -> _T:
        ...

    def get(self, key: int, default: _T | None = None) -> int | _T | None:
        return self._m.get(key, default)

    def __setitem__(self, key: int, value: int) -> None:
        self._m[key] = value

    @overload
    def pop(self, key: int) -> int:
        ...

    @overload
    def pop(self, key: Any) -> NoReturn:
        ...

    @overload
    def pop(self, key: int, default: _T) -> int | _T:
        ...

    @overload
    def pop(self, key: Any, default: _T) -> _T:
        ...

    def pop(self, key: Any, default: _T | None = None) -> int | _T | None:
        return self._m.pop(key, default)


class JudyIntSet:
    def __init__(self):
        self._s = _JudyIntSet()

    @overload
    def __contains__(self, value: int) -> bool:
        ...

    @overload
    def __contains__(self, value: Any) -> Literal[False]:
        ...

    def __contains__(self, value: Any) -> bool:
        return (value in self._s)

    def add(self, value: int) -> None:
        self._s.add(value)

    def remove(self, value: int) -> None:
        self._s.remove(value)

    def __len__(self) -> int:
        return len(self._s)

    def __sizeof__(self) -> int:
        return self._s.__sizeof__()

    def __str__(self) -> str:
        return self._s.__str__()

    def __repr__(self) -> str:
        return self._s.__repr__()

    def __iter__(self) -> Iterator[int]:
        return iter(self._s)

    def __getitem__(self, index: int) -> int:
        return self._s[index]


__all__ = ['JudyIntSet']
