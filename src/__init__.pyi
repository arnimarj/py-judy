from typing import Generic, Iterator, Mapping, NoReturn, overload, Sequence, TypeVar


_VT = TypeVar('_VT')
_DT = TypeVar('_DT')


class JudyIntObjectMap(Mapping[int, _VT], Generic[_VT]):
	def __sizeof__(self) -> int:
		...

	def __value_sizeof__(self) -> int:
		...

	def __len__(self) -> int:
		...

	def __contains__(self, key: object) -> bool:
		...

	@overload
	def __getitem__(self, key: int) -> _VT:
		...

	@overload
	def __getitem__(self, key: slice) -> NoReturn:
		...

	def __setitem__(self, key: int, value: _VT) -> None:
		...

	@overload
	def get(self, key: int) -> _VT | None:
		...

	@overload  # noqa: F811
	def get(self, key: int, default: _DT = ...) -> _DT | _VT:
		...

	@overload
	def pop(self, key: int) -> _VT:
		...

	@overload  # noqa: F811
	def pop(self, key: int, default: _DT = ...) -> _DT | _VT:
		...

	def clear(self) -> None:
		...

	def __iter__(self) -> Iterator[int]:
		...

	def keys(self) -> Iterator[int]:
		...

	def values(self) -> Iterator[_VT]:
		...

	def items(self) -> Iterator[tuple[int, _VT]]:
		...


class JudyIntSet(Sequence[int]):
	def __len__(self) -> int:
		...

	def __sizeof__(self) -> int:
		...

	def __contains__(self, key: object) -> bool:
		...

	def __iter__(self) -> Iterator[int]:
		...

	@overload
	def __getitem__(self, index: int) -> int:
		...

	@overload
	def __getitem__(self, index: slice) -> NoReturn:
		...

	def add(self, key: int) -> None:
		...

	def remove(self, key: int) -> None:
		...
