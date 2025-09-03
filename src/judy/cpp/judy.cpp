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

namespace nb = nanobind;
using namespace nb::literals;


Word_t _SelectKey(Word_t key, Word_t value)
    { return key; }
Word_t _SelectValue(Word_t key, Word_t value)
    { return value; }
std::pair<Word_t, Word_t> _SelectItem(Word_t key, Word_t value)
    { return std::pair(key, value); }


NB_MODULE(_judy_nb, m) {
    m.attr("_T") = nb::type_var("_T");

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
        .def("__str__", &JudyIntIntMap::ToString)
        .def("__repr__", &JudyIntIntMap::ToString)
        .def("__sizeof__", &JudyIntIntMap::size_of)
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
            nb::sig("def pop(self, arg: int, default: _T = ..., /) -> int | _T | None")
        )
        .def(
            "pop",
            [](const JudyIntIntMap&, nb::handle index, std::optional<nb::handle> def) { return def; },
            nb::arg("index"),
            nb::arg("default"),
            nb::sig("def pop(self, arg: typing.Any, default: _T, /) -> _T")
        )
        .def("by_index", &JudyIntIntMap::ByIndex)
        .def("__iter__", [](std::shared_ptr<JudyIntIntMap>& s) {
            std::optional<Word_t> empty;
            return JudyIntIntMapIterator<Word_t, _SelectKey>(s, empty, empty);
        }, nb::rv_policy::reference)
        .def("keys", [](std::shared_ptr<JudyIntIntMap>& s) {
            std::optional<Word_t> empty;
            return JudyIntIntMapIterator<Word_t, _SelectKey>(s, empty, empty);
        }, nb::rv_policy::reference)
        .def("values", [](std::shared_ptr<JudyIntIntMap>& s) {
            std::optional<Word_t> empty;
            return JudyIntIntMapIterator<Word_t, _SelectValue>(s, empty, empty);
        }, nb::rv_policy::reference)
        .def("items", [](std::shared_ptr<JudyIntIntMap>& s) {
            std::optional<Word_t> empty;
            return JudyIntIntMapIterator<std::pair<Word_t, Word_t>, _SelectItem>(s, empty, empty);
        }, nb::rv_policy::reference)

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

    nb::class_<JudyIntIntMapIterator<Word_t, _SelectKey>>(m, "JudyIntIntMapKeyIterator")
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def("__next__", &JudyIntIntMapIterator<Word_t, _SelectKey>::Next)
    ;

    nb::class_<JudyIntIntMapIterator<Word_t, _SelectValue>>(m, "JudyIntIntMapValueIterator")
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def("__next__", &JudyIntIntMapIterator<Word_t, _SelectValue>::Next)
    ;

    nb::class_<JudyIntIntMapIterator<std::pair<Word_t, Word_t>, _SelectItem>>(m, "JudyIntIntMapItemIterator")
        .def("__iter__", [](nb::handle h) { return h; }, nb::sig("def __iter__(self) -> typing.Self"))
        .def("__next__", &JudyIntIntMapIterator<std::pair<Word_t, Word_t>, _SelectItem>::Next)
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
        .def("remove", &JudyIntSet::Remove)
        .def("__getitem__", &JudyIntSet::GetItem)
        .def(
            "__getitem__",
            [](const JudyIntSet&, std::optional<nb::handle>) { throw nb::key_error(); },
            nb::arg("index").none(),
            nb::sig("def __getitem__(self, arg: typing.Any, /) -> typing.NoReturn")
        )
        .def("__iter__", [](std::shared_ptr<JudyIntSet>& s) {
            return JudyIntSetIterator(s);
        }, nb::rv_policy::reference)
        .def("__str__", &JudyIntSet::ToString)
        .def("__repr__", &JudyIntSet::ToString)
        .def("__len__", &JudyIntSet::size)
        .def("__sizeof__", &JudyIntSet::size_of)

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
