#include <Judy.h>

#include <optional>
#include <string>
#include <variant>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/variant.h>

namespace nb = nanobind;
using namespace nb::literals;


struct JudyIntObjectMap {
    Pvoid_t judy_map;
    nb::ft_mutex mutex;

    JudyIntObjectMap();
    ~JudyIntObjectMap();

    JudyIntObjectMap(JudyIntObjectMap&& other) noexcept
        // only move judy pointer, and reset it in the
        // move'd instance
        : judy_map(other.judy_map)
    {
        other.judy_map = nullptr;
    }

    bool Contains(Word_t value);
    Word_t size();
    Word_t size_of();
    void Clear();
    nb::handle GetItem(Word_t key);
    void SetItem(Word_t key, nb::handle value);
    void DeleteItem(Word_t key);
    nb::handle Pop(Word_t key);
    std::optional<nb::handle> PopDefault(Word_t key, std::optional<nb::handle> failobj);

    std::optional<nb::handle> Get(Word_t key);
    std::optional<nb::handle> GetDefault(Word_t key, std::optional<nb::handle> failobj);

    /*
    std::string ToString();
    nb::handle ByIndex(Py_ssize_t index);
    */

    int GCVisit(visitproc visit, void* arg);
};


int int_obj_map_object_traverse(PyObject* self, visitproc visit, void* arg);
int int_obj_map_object_clear(PyObject* self);
