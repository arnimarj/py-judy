#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

#include <Judy.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/variant.h>

#include "judy_int_set.h"￼
￼
namespace nb = nanobind;
using namespace nb::literals;


struct JudyIntIntMap {
    Pvoid_t judy_map;
    nb::ft_mutex mutex;

    JudyIntIntMap()
    {
        judy_map = nullptr;
    }

    ~JudyIntIntMap()
    {
        nb::ft_lock_guard guard(mutex);
        Word_t bytes_freed;
        JLFA(bytes_freed, judy_map);
        judy_map = 0;
    }

    bool Contains(Word_t value)
    {
        nb::ft_lock_guard guard(mutex);
        void* v = nullptr;
        JLG(v, judy_map, value);
        return (v != nullptr);
    }

    void Clear()
    {
        nb::ft_lock_guard guard(mutex);
        Word_t bytes_freed = 0;
        JLFA(bytes_freed, judy_map);
        judy_map = nullptr;
    }

    Word_t GetItem(Word_t key)
    {
        nb::ft_lock_guard guard(mutex);

        void* v = nullptr;
        JLG(v, judy_map, key);

        if (v == nullptr)
            throw nb::key_error();

        return *((Word_t*)v);
    }

    std::string ToString()
    {
        std::stringbuf sbuf;
        std::ostream os(&sbuf);
        os << "{";

        {
            nb::ft_lock_guard guard(mutex);

            void* v = nullptr;
            Word_t i = 0;
            JLF(v, judy_map, i);

            if (v != 0)
                os << i << ": " << *((Word_t*)v);

            while (true) {
                JLN(v, judy_map, i);

                if (v == 0)
                    break;

                os << ", " << i << ": " << *((Word_t*)v);
            }
        }

        os << "}";
        return sbuf.str();
    }

    std::variant<Word_t, std::optional<nb::handle>> Get(Word_t key, std::optional<nb::handle> failobj)
    {
        nb::ft_lock_guard guard(mutex);
        void* v = nullptr;
        JLG(v, judy_map, key);

        if (v == nullptr)
            return failobj;

        return *((Word_t*)v);
    }

    void SetItem(Word_t key, Word_t value)
    {
        nb::ft_lock_guard guard(mutex);

        // get pointer to current item, if if exists
        void* v = nullptr;
        JLG(v, judy_map, key);

        if (v == nullptr) {
            // otherwise, create new item and get pointer to that
            // insert otherwise
            JLI(v, judy_map, key);

            if (v == PJERR)
                throw std::bad_alloc();
        }

        *((Word_t*)v) = value;
    }

    Word_t Pop(Word_t key)
    {
        nb::ft_lock_guard guard(mutex);

        void* v = nullptr;
        JLG(v, judy_map, key);

        if (v == nullptr)
            throw nb::key_error();

        Word_t value = *((Word_t*)v);
        int i = 0;
        JLD(i, judy_map, key);

        if (i == JERR)
            throw std::bad_alloc();

        return value;
    }

    std::variant<Word_t, std::optional<nb::handle>> PopDefault(Word_t key, std::optional<nb::handle> failobj)
    {
        try {
            return Pop(key);
        }
        catch (const nb::python_error &e)
        {
            if (e.matches(PyExc_KeyError))
                return failobj;

            throw;
        }
    }

    Word_t ByIndex(Py_ssize_t index)
    {
        if (index < 0)
            throw nb::index_error();

        nb::ft_lock_guard guard(mutex);

        // NOTE: judy index is 1-based
        Word_t k = 0;
        void* v = 0;
        JLBC(v, judy_map, index + 1, k);

        if (v == nullptr)
            throw nb::index_error();

        return *((Word_t*)v);
    }
};


NB_MODULE(_judy_nb, m) {
    nb::class_<JudyIntIntMap>(m, "_JudyIntIntMap")
        .def(nb::init<>())
        .def("__contains__", &JudyIntIntMap::Contains)
        .def("__contains__", [](const JudyIntIntMap&, std::optional<nb::handle>) { return false; }, nb::arg("index").none())
        .def("__len__", [](JudyIntIntMap& s) {
            nb::ft_lock_guard guard(s.mutex);
            Word_t count = 0;
            JLC(count, s.judy_map, 0, -1);
            return count;
        })
        .def("clear", &JudyIntIntMap::Clear)
        .def("__getitem__", &JudyIntIntMap::GetItem)
        .def("__getitem__", [](const JudyIntIntMap&, std::optional<nb::handle>) { throw nb::key_error(); }, nb::arg("index").none())
        .def("__setitem__", &JudyIntIntMap::SetItem)
        .def("__str__", &JudyIntIntMap::ToString)
        .def("__repr__", &JudyIntIntMap::ToString)
        .def("__sizeof__", [](JudyIntIntMap& s) {
            nb::ft_lock_guard guard(s.mutex);
            Word_t size = 0;
            JLMU(size, s.judy_map);
            return size + sizeof(s);
        })
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
        .def("__len__", [](JudyIntSet& s) {
            nb::ft_lock_guard guard(s.mutex);
            Word_t count;
            J1C(count, s.judy_set, 0, -1);
            return count;
        })
        .def("__sizeof__", [](JudyIntSet& s) {
            nb::ft_lock_guard guard(s.mutex);
            Word_t size = 0;
            J1MU(size, s.judy_set);
            return size + sizeof(s);
        })
    ;

    nb::class_<JudyIntSetIterator>(m, "_JudyIntSetIterator")
        .def("__iter__", [](nb::handle h) { return h; })
        .def("__next__", &JudyIntSetIterator::Next)
    ;
}
