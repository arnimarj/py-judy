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

    /*
    nb::object GetItem(Word_t key);
    std::string ToString();
    std::optional<nb::object> Get(Word_t key);
    std::variant<Word_t, std::optional<nb::handle>> GetDefault(Word_t key, std::optional<nb::handle> failobj);
    void SetItem(Word_t key, nb::object value);
    nb::object Pop(Word_t key);
    std::variant<nb::object, std::optional<nb::handle>> PopDefault(Word_t key, std::optional<nb::handle> failobj);
    nb::object ByIndex(Py_ssize_t index);
    */

private:
    void UnlockedClear();
};
