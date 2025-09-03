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


template <typename T, T (*Func)(Word_t, Word_t)>
struct JudyIntIntMapIterator {
    std::shared_ptr<JudyIntIntMap> judy_map_object;
    bool started;
    bool ended;
    Word_t key;

    // for range iteration
    std::optional<Word_t> lower_range;
    std::optional<Word_t> upper_range;

    JudyIntIntMapIterator(
        std::shared_ptr<JudyIntIntMap>& parent,
        std::optional<Word_t> lower_range,
        std::optional<Word_t> upper_range
    )
        : judy_map_object(parent)
        , lower_range(lower_range)
        , upper_range(upper_range)

    {
        started = false;
        ended = false;
        key = 0;
    }

    T Next()
    {
        nb::ft_lock_guard guard(judy_map_object->mutex);

        // already exhausted
        if (ended)
            throw nb::stop_iteration();

        void* v = nullptr;

        // first item
        if (!started) {
            JLF(v, judy_map_object->judy_map, key);
            started = true;

            if (v == nullptr) {
                ended = true;
                throw nb::stop_iteration();
            }

            return Func(key, *((Word_t*)v));
        }

        // next item
        JLN(v, judy_map_object->judy_map, key);

        if (v != nullptr)
            return Func(key, *((Word_t*)v));

        // beyond last item
        ended = true;
        throw nb::stop_iteration();
    }
};
