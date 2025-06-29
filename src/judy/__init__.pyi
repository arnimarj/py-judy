from typing import overload, Generic, Iterator, Mapping, Optional, Sequence, Tuple, TypeVar, Union

VT = TypeVar('VT')
DT = TypeVar('DT')


class JudyIntObjectMap(Mapping[int, VT], Sequence[VT], Generic[VT]):
	def __sizeof__(self) -> int:
		...

	def __value_sizeof__(self) -> int:
		...

	def __len__(self) -> int:
		...

	def __contains__(self, key: int) -> bool:
		...

	def __getitem__(self, key: int) -> VT:
		...

	def __setitem__(self, key: int, value: VT) -> None:
		...

	@overload
	def get(self, key: int) -> Optional[VT]:
		...

	@overload  # noqa: F811
	def get(self, key: int, default: DT = ...) -> Union[DT, VT]:
		...

	@overload
	def pop(self, key: int) -> VT:
		...

	@overload  # noqa: F811
	def pop(self, key: int, default: DT = ...) -> Union[DT, VT]:
		...

	def clear(self) -> None:
		...

	def __iter__(self) -> Iterator[int]:
		...

	def keys(self) -> Iterator[int]:
		...

	def values(self) -> Iterator[VT]:
		...

	def items(self) -> Iterator[Tuple[int, VT]]:
		...


class JudyIntSet(Sequence[int]):
	def __len__(self) -> int:
		...

	def __sizeof__(self) -> int:
		...

	def __contains__(self, key: int) -> bool:
		...

	def __iter__(self) -> Iterator[int]:
		...

	def __getitem__(self, index: int) -> int:
		...

	def add(self, key: int) -> None:
		...

	def remove(self, key: int) -> None:
		...
