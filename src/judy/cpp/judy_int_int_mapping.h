#include <Judy.h>

#include <optional>
#include <sstream>
#include <string>
#include <variant>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/variant.h>

namespace nb = nanobind;
using namespace nb::literals;


struct JudyIntIntMap {
    Pvoid_t judy_map;
    nb::ft_mutex mutex;

    JudyIntIntMap();
    ~JudyIntIntMap();

    bool Contains(Word_t value);
    Word_t size();
    Word_t size_of();
    void Clear();
    Word_t GetItem(Word_t key);
    std::string ToString();
    std::variant<Word_t, std::optional<nb::handle>> Get(Word_t key, std::optional<nb::handle> failobj);
    void SetItem(Word_t key, Word_t value);
    Word_t Pop(Word_t key);
    std::variant<Word_t, std::optional<nb::handle>> PopDefault(Word_t key, std::optional<nb::handle> failobj);
    Word_t ByIndex(Py_ssize_t index);
};
