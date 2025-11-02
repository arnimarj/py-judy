#include <Judy.h>

#include <optional>
#include <ranges>
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

    JudyIntIntMap(JudyIntIntMap&& other) noexcept
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
    Word_t GetItem(Word_t key);
    std::string ToString();
    std::optional<Word_t> Get(Word_t key);
    std::variant<Word_t, std::optional<nb::handle>> GetDefault(Word_t key, std::optional<nb::handle> failobj);
    void SetItem(Word_t key, Word_t value);
    void DeleteItem(Word_t key);
    Word_t Pop(Word_t key);
    std::variant<Word_t, std::optional<nb::handle>> PopDefault(Word_t key, std::optional<nb::handle> failobj);
    Word_t ByIndex(Py_ssize_t index);

    template <typename T, typename S>
    static JudyIntIntMap FromSpan(const std::span<T> keys, const std::span<S> values)
    {
        if (keys.size() != values.size())
            throw std::runtime_error("spans must be of the same size");

        auto s = JudyIntIntMap();

        for (auto [key, value] : std::views::zip(keys, values)) {
            s.SetItem(
                static_cast<Word_t>(key),
                static_cast<Word_t>(value)
            );
        }

        return std::move(s);
    }

private:
    void UnlockedClear();
};
