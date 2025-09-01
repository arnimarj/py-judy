#include <memory>
#include <optional>
#include <stdexcept>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/shared_ptr.h>

#include "judy_int_set.h"
#include "judy_int_int_mapping.h"

namespace nb = nanobind;
using namespace nb::literals;


NB_MODULE(_judy_nb, m) {
    nb::class_<JudyIntIntMap>(m, "_JudyIntIntMap")
        .def(nb::init<>())
        .def("__contains__", &JudyIntIntMap::Contains)
        .def("__contains__", [](const JudyIntIntMap&, std::optional<nb::handle>) { return false; }, nb::arg("index").none())
        .def("__len__", &JudyIntIntMap::size)
        .def("clear", &JudyIntIntMap::Clear)
        .def("__getitem__", &JudyIntIntMap::GetItem)
        .def("__getitem__", [](const JudyIntIntMap&, std::optional<nb::handle>) { throw nb::key_error(); }, nb::arg("index").none())
        .def("__setitem__", &JudyIntIntMap::SetItem)
        .def("__str__", &JudyIntIntMap::ToString)
        .def("__repr__", &JudyIntIntMap::ToString)
        .def("__sizeof__", &JudyIntIntMap::size_of)
        .def("get", &JudyIntIntMap::Get, nb::arg("index"), nb::arg("default").none() = nb::none())
        .def("pop", &JudyIntIntMap::Pop, nb::arg("index"))
        .def("pop", &JudyIntIntMap::PopDefault, nb::arg("index"), nb::arg("default").none() = nb::none())
        .def("pop", [](const JudyIntIntMap&, std::optional<nb::handle> def) { return def; }, nb::arg("index"))
        .def("by_index", &JudyIntIntMap::ByIndex)
    ;

    nb::class_<JudyIntSet>(m, "_JudyIntSet")
        .def(nb::init<>())
        .def("__contains__", &JudyIntSet::Contains)
        .def("__contains__", [](const JudyIntSet&, std::optional<nb::handle>) { return false; }, nb::arg("index").none())
        .def("add", &JudyIntSet::Add)
        .def("remove", &JudyIntSet::Remove)
        .def("__getitem__", &JudyIntSet::GetItem)
        .def("__getitem__", [](const JudyIntSet&, std::optional<nb::handle>) { throw nb::key_error(); }, nb::arg("index").none())
        .def("__iter__", [](std::shared_ptr<JudyIntSet>& s) {
            return JudyIntSetIterator(s);
        }, nb::rv_policy::reference)
        .def("__str__", &JudyIntSet::ToString)
        .def("__repr__", &JudyIntSet::ToString)
        .def("__len__", &JudyIntSet::size)
        .def("__sizeof__", &JudyIntSet::size_of)
    ;

    nb::class_<JudyIntSetIterator>(m, "_JudyIntSetIterator")
        .def("__iter__", [](nb::handle h) { return h; })
        .def("__next__", &JudyIntSetIterator::Next)
    ;
}
