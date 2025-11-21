#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <utility>
#include <stdexcept>

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/typing.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/pair.h>

#include "judy_int_set.h"
#include "judy_int_int_map.h"
#include "judy_int_object_map.h"
#include "judy_map_iter.h"

namespace nb = nanobind;
using namespace nb::literals;


static Word_t MapIterSelectKey(Word_t key, Word_t value)
    { return key; }
static Word_t MapIterSelectValue(Word_t key, Word_t value)
    { return value; }
static std::pair<Word_t, Word_t> MapIterSelectItem(Word_t key, Word_t value)
    { return std::pair(key, value); }

static std::pair<Word_t, nb::handle> MapIterSelectItemAsObject(Word_t key, Word_t value)
{
    return std::pair(key, nb::borrow((PyObject*)(value)));
}

static nb::handle MapIterSelectValueAsObject(Word_t key, Word_t value)
{
    return nb::borrow((PyObject*)(value));
}


PyType_Slot int_object_map_gc_slots[] = {
    { Py_tp_traverse, (void*)int_obj_map_object_traverse },
    { Py_tp_clear, (void*)int_obj_map_object_clear },
    { 0, 0 }
};


NB_MODULE(_judy_nb, m) {
    m.attr("T") = nb::type_var("T");
    m.attr("_T") = nb::type_var("_T");

    nb::class_<JudyIntObjectMap>(
        m,
        "JudyIntObjectMap",
        nb::type_slots(int_object_map_gc_slots),
        nb::is_generic(),
        nb::sig("class JudyIntObjectMap(typing.Generic[T])")
    )
        .def(nb::init<>())
        .def("__contains__", &JudyIntObjectMap::Contains)
        .def(
            "__contains__",
            [](const JudyIntObjectMap&, std::optional<nb::handle>) { return false; },
            nb::arg("index").none(),
            nb::sig("def __contains__(self, arg: typing.Any, /) -> typing.Literal[False]")
        )
        .def("__len__", &JudyIntObjectMap::size)
        .def("clear", &JudyIntObjectMap::Clear)
        .def("__sizeof__", &JudyIntObjectMap::size_of)
        .def(
            "__getitem__",
            &JudyIntObjectMap::GetItem,
            nb::sig("def __getitem__(self, arg: int, /) -> T")
        )
        .def(
            "__getitem__",
            [](const JudyIntObjectMap&, std::optional<nb::handle>) { throw nb::key_error(); },
            nb::arg("index").none(),
            nb::sig("def __getitem__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def(
            "__setitem__", 
            &JudyIntObjectMap::SetItem,
            nb::sig("def __setitem__(self, arg0: int, arg1: T, /) -> None")
        )
        .def("__delitem__", &JudyIntObjectMap::DeleteItem)
        .def("__str__", &JudyIntObjectMap::ToString)
        .def("__repr__", &JudyIntObjectMap::ToString)
        .def(
            "pop",
            &JudyIntObjectMap::Pop,
            nb::arg("index"),
            nb::sig("def pop(self, arg: int, /) -> T")
        )
        .def(
            "pop",
            &JudyIntObjectMap::PopDefault,
            nb::arg("index"),
            nb::arg("default").none() = nb::none(),
            nb::sig("def pop(self, arg: int, default: _T, /) -> T | _T")
        )
        .def(
            "pop",
            [](const JudyIntObjectMap&, nb::handle index, std::optional<nb::handle> def) { return def; },
            nb::arg("index"),
            nb::arg("default"),
            nb::sig("def pop(self, arg: typing.Any, default: _T, /) -> _T")
        )
        .def(
            "get",
            &JudyIntObjectMap::Get,
            nb::arg("index"),
            nb::sig("def get(self, arg: int, /) -> T | None")
        )
        .def(
            "get",
            &JudyIntObjectMap::GetDefault,
            nb::arg("index"),
            nb::arg("default").none() = nb::none(),
            nb::sig("def get(self, arg: int, default: _T, /) -> T | _T")
        )
        .def(
            "by_index",
            &JudyIntObjectMap::ByIndex,
            nb::sig("def by_index(self, arg: int, /) -> T")
        )
        .def(
            "__eq__",
            [](const JudyIntObjectMap&, std::optional<nb::handle>) { throw nb::type_error(); },
            nb::sig("def __eq__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def(
            "__ne__",
            [](const JudyIntObjectMap&, std::optional<nb::handle>) { throw nb::type_error(); },
            nb::sig("def __ne__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def("__iter__", []
            (
                std::shared_ptr<JudyIntObjectMap>& s
            )
            {
                return JudyMapIterator<JudyIntObjectMap, Word_t, MapIterSelectKey>(s, {}, {});
            }
        )
        .def("keys", [](
            std::shared_ptr<JudyIntObjectMap>& s,
            std::optional<Word_t> lower_inclusive,
            std::optional<Word_t> upper_inclusive
        ) {
            return JudyMapIterator<JudyIntObjectMap, Word_t, MapIterSelectKey>(s, lower_inclusive, upper_inclusive);
        },
            nb::kw_only(),
            nb::arg("lower_inclusive").none() = nb::none(),
            nb::arg("upper_inclusive").none() = nb::none(),
            nb::rv_policy::reference
        )
        .def("values", [](
            std::shared_ptr<JudyIntObjectMap>& s,
            std::optional<Word_t> lower_inclusive,
            std::optional<Word_t> upper_inclusive
        ) {
            return JudyMapIterator<JudyIntObjectMap, nb::handle, MapIterSelectValueAsObject>(s, lower_inclusive, upper_inclusive);
        },
            nb::kw_only(),
            nb::arg("lower_inclusive").none() = nb::none(),
            nb::arg("upper_inclusive").none() = nb::none(),
            nb::rv_policy::reference,
            nb::sig("def values(self, *, lower_inclusive: int | None = None, upper_inclusive: int | None = None) -> JudyIntObjectMapValueIterator[T]")
        )
        .def("items", [](
            std::shared_ptr<JudyIntObjectMap>& s,
            std::optional<Word_t> lower_inclusive,
            std::optional<Word_t> upper_inclusive
        ) {
            return JudyMapIterator<JudyIntObjectMap, std::pair<Word_t, nb::handle>, MapIterSelectItemAsObject>(s, lower_inclusive, upper_inclusive);
        },
            nb::kw_only(),
            nb::arg("lower_inclusive").none() = nb::none(),
            nb::arg("upper_inclusive").none() = nb::none(),
            nb::rv_policy::reference,
            nb::sig("def items(self, *, lower_inclusive: int | None = None, upper_inclusive: int | None = None) -> JudyIntObjectMapItemIterator[T]")
        )
   ;

    nb::class_<JudyMapIterator<JudyIntObjectMap, Word_t, MapIterSelectKey>>(
        m,
        "JudyIntObjectMapKeyIterator"
    )
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def("__next__", &JudyMapIterator<JudyIntObjectMap, Word_t, MapIterSelectKey>::Next)
    ;

    nb::class_<JudyMapIterator<JudyIntObjectMap, nb::handle, MapIterSelectValueAsObject>>(
        m,
        "JudyIntObjectMapValueIterator",
        nb::is_generic(),
        nb::sig("class JudyIntObjectMapValueIterator(typing.Generic[T])")
    )
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def(
            "__next__",
            &JudyMapIterator<JudyIntObjectMap, nb::handle, MapIterSelectValueAsObject>::Next,
            nb::sig("def __next__(self) -> T")
        )
    ;

    nb::class_<JudyMapIterator<JudyIntObjectMap, std::pair<Word_t, nb::handle>, MapIterSelectItemAsObject>>(
        m,
        "JudyIntObjectMapItemIterator",
        nb::is_generic(),
        nb::sig("class JudyIntObjectMapItemIterator(typing.Generic[T])")
    )
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def(
            "__next__",
            &JudyMapIterator<JudyIntObjectMap, std::pair<Word_t, nb::handle>, MapIterSelectItemAsObject>::Next,
            nb::sig("def __next__(self) -> tuple[int, T]")
        )
    ;

    nb::class_<JudyIntIntMap>(m, "JudyIntIntMap")
        .def(nb::init<>())
        .def("__contains__", &JudyIntIntMap::Contains)
        .def(
            "__contains__",
            [](const JudyIntIntMap&, std::optional<nb::handle>) { return false; },
            nb::arg("index").none(),
            nb::sig("def __contains__(self, arg: typing.Any, /) -> typing.Literal[False]")
        )
        .def("__len__", &JudyIntIntMap::size)
        .def("clear", &JudyIntIntMap::Clear)
        .def("__getitem__", &JudyIntIntMap::GetItem)
        .def(
            "__getitem__",
            [](const JudyIntIntMap&, std::optional<nb::handle>) { throw nb::key_error(); },
            nb::arg("index").none(),
            nb::sig("def __getitem__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def("__setitem__", &JudyIntIntMap::SetItem)
        .def("__delitem__", &JudyIntIntMap::DeleteItem)
        .def("__str__", &JudyIntIntMap::ToString)
        .def("__repr__", &JudyIntIntMap::ToString)
        .def("__sizeof__", &JudyIntIntMap::size_of)
        .def(
            "__eq__",
            [](const JudyIntIntMap&, std::optional<nb::handle>) { throw nb::type_error(); },
            nb::sig("def __eq__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def(
            "__ne__",
            [](const JudyIntIntMap&, std::optional<nb::handle>) { throw nb::type_error(); },
            nb::sig("def __ne__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def("get", &JudyIntIntMap::Get, nb::arg("index"))
        .def(
            "get",
            &JudyIntIntMap::GetDefault,
            nb::arg("index"),
            nb::arg("default").none() = nb::none(),
            nb::sig("def get(self, arg: int, default: _T, /) -> int | _T")
        )
        .def("pop", &JudyIntIntMap::Pop, nb::arg("index"))
        .def(
            "pop",
            &JudyIntIntMap::PopDefault,
            nb::arg("index"),
            nb::arg("default").none() = nb::none(),
            nb::sig("def pop(self, arg: int, default: _T, /) -> int | _T")
        )
        .def(
            "pop",
            [](const JudyIntIntMap&, nb::handle index, std::optional<nb::handle> def) { return def; },
            nb::arg("index"),
            nb::arg("default"),
            nb::sig("def pop(self, arg: typing.Any, default: _T, /) -> _T")
        )
        .def("by_index", &JudyIntIntMap::ByIndex)
        .def("__iter__", []
            (
                std::shared_ptr<JudyIntIntMap>& s
            )
            {
                return JudyMapIterator<JudyIntIntMap, Word_t, MapIterSelectKey>(s, {}, {});
            }
        )
        .def("keys", [](
            std::shared_ptr<JudyIntIntMap>& s,
            std::optional<Word_t> lower_inclusive,
            std::optional<Word_t> upper_inclusive
        ) {
            return JudyMapIterator<JudyIntIntMap, Word_t, MapIterSelectKey>(s, lower_inclusive, upper_inclusive);
        },
            nb::kw_only(),
            nb::arg("lower_inclusive").none() = nb::none(),
            nb::arg("upper_inclusive").none() = nb::none(),
            nb::rv_policy::reference
        )
        .def("values", [](
            std::shared_ptr<JudyIntIntMap>& s,
            std::optional<Word_t> lower_inclusive,
            std::optional<Word_t> upper_inclusive
        ) {
            return JudyMapIterator<JudyIntIntMap, Word_t, MapIterSelectValue>(s, lower_inclusive, upper_inclusive);
        },
            nb::kw_only(),
            nb::arg("lower_inclusive").none() = nb::none(),
            nb::arg("upper_inclusive").none() = nb::none(),
            nb::rv_policy::reference
        )
        .def("items", [](
            std::shared_ptr<JudyIntIntMap>& s,
            std::optional<Word_t> lower_inclusive,
            std::optional<Word_t> upper_inclusive
        ) {
            return JudyMapIterator<JudyIntIntMap, std::pair<Word_t, Word_t>, MapIterSelectItem>(s, lower_inclusive, upper_inclusive);
        },
            nb::kw_only(),
            nb::arg("lower_inclusive").none() = nb::none(),
            nb::arg("upper_inclusive").none() = nb::none(),
            nb::rv_policy::reference
        )

        // TBD: how to specialize across all combos of u8/16/32/64
        .def_static("FromArray", [](
            nb::ndarray<uint64_t, nb::shape<-1>, nb::device::cpu> keys,
            nb::ndarray<uint64_t, nb::shape<-1>, nb::device::cpu> values
        ) {
            // release gil while building
            nb::gil_scoped_release release_gil;
            return JudyIntIntMap::FromSpan(
                std::span{keys.data(), keys.shape(0)},
                std::span{values.data(), values.shape(0)}
            );
        })
    ;

    nb::class_<JudyMapIterator<JudyIntIntMap, Word_t, MapIterSelectKey>>(m, "JudyIntIntMapKeyIterator")
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def("__next__", &JudyMapIterator<JudyIntIntMap, Word_t, MapIterSelectKey>::Next)
    ;

    nb::class_<JudyMapIterator<JudyIntIntMap, Word_t, MapIterSelectValue>>(m, "JudyIntIntMapValueIterator")
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def("__next__", &JudyMapIterator<JudyIntIntMap, Word_t, MapIterSelectValue>::Next)
    ;

    nb::class_<JudyMapIterator<JudyIntIntMap, std::pair<Word_t, Word_t>, MapIterSelectItem>>(m, "JudyIntIntMapItemIterator")
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def("__next__", &JudyMapIterator<JudyIntIntMap, std::pair<Word_t, Word_t>, MapIterSelectItem>::Next)
    ;

    nb::class_<JudyIntSet>(m, "JudyIntSet")
        .def(nb::init<>())
        .def("__contains__", &JudyIntSet::Contains)
        .def(
            "__contains__",
            [](const JudyIntSet&, std::optional<nb::handle>) { return false; },
            nb::arg("index"),
            nb::sig("def __contains__(self, arg: typing.Any, /) -> typing.Literal[False]")
        )
        .def("add", &JudyIntSet::Add)
        .def("pop", []
            (
                std::shared_ptr<JudyIntSet>& s
            )
            {
                auto v = s->Pop();

                if (!v.has_value())
                    throw nb::key_error("pop from an empy set");

                return v.value();
            }
        )
        .def("discard", [](std::shared_ptr<JudyIntSet>& s, Word_t v) { s->Remove(v, false); })
        .def("remove", [](std::shared_ptr<JudyIntSet>& s, Word_t v) { s->Remove(v, true); })
        .def("__getitem__", &JudyIntSet::GetItem)
        .def(
            "__getitem__",
            [](const JudyIntSet&, std::optional<nb::handle>) { throw nb::key_error(); },
            nb::arg("index").none(),
            nb::sig("def __getitem__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def("__iter__", []
            (
                std::shared_ptr<JudyIntSet>& s
            )
            {
                return JudyIntSetIterator(s, {}, {});
            }
        )
        .def("ranged_iterator", [](
            std::shared_ptr<JudyIntSet>& s,
            std::optional<Word_t> lower_inclusive,
            std::optional<Word_t> upper_inclusive
        ) {
            return JudyIntSetIterator(s, lower_inclusive, upper_inclusive);
        },
            nb::kw_only(),
            nb::arg("lower_inclusive").none() = nb::none(),
            nb::arg("upper_inclusive").none() = nb::none(),
            nb::rv_policy::reference
        )
        .def(
            "__eq__",
            [](const JudyIntSet&, std::optional<nb::handle>) { throw nb::type_error(); },
            nb::sig("def __eq__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def(
            "__ne__",
            [](const JudyIntSet&, std::optional<nb::handle>) { throw nb::type_error(); },
            nb::sig("def __ne__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def("__str__", &JudyIntSet::ToString)
        .def("__repr__", &JudyIntSet::ToString)
        .def("__len__", &JudyIntSet::size)
        .def("clear", &JudyIntSet::Clear)
        .def("__sizeof__", &JudyIntSet::size_of)
        .def("by_index", &JudyIntSet::ByIndex)

        .def_static("FromArray", [](nb::ndarray<uint8_t, nb::shape<-1>, nb::device::cpu> array) {
            // release gil while building
            nb::gil_scoped_release release_gil;
            return JudyIntSet::FromSpan(std::span{array.data(), array.shape(0)});
        })
        .def_static("FromArray", [](nb::ndarray<uint16_t, nb::shape<-1>, nb::device::cpu> array) {
            // release gil while building
            nb::gil_scoped_release release_gil;
            return JudyIntSet::FromSpan(std::span{array.data(), array.shape(0)});
        })
        .def_static("FromArray", [](nb::ndarray<uint32_t, nb::shape<-1>, nb::device::cpu> array) {
            // release gil while building
            nb::gil_scoped_release release_gil;
            return JudyIntSet::FromSpan(std::span{array.data(), array.shape(0)});
        })
        .def_static("FromArray", [](nb::ndarray<uint64_t, nb::shape<-1>, nb::device::cpu> array) {
            // release gil while building
            nb::gil_scoped_release release_gil;
            return JudyIntSet::FromSpan(std::span{array.data(), array.shape(0)});
        })
    ;

    nb::class_<JudyIntSetIterator>(m, "JudyIntSetIterator")
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def("__next__", &JudyIntSetIterator::Next)
    ;
}
