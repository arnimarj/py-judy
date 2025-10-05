#include <cstdint>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <utility>

#include <Judy.h>

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/shared_ptr.h>

namespace nb = nanobind;
using namespace nb::literals;


struct JudyIntSet {
    Pvoid_t judy_set;
    nb::ft_mutex mutex;

    JudyIntSet();
    ~JudyIntSet();

    JudyIntSet(JudyIntSet&& other) noexcept
        // only move judy pointer, and reset it in the
        // move'd instance
        : judy_set(other.judy_set)
    {
        other.judy_set = nullptr;
    }

    std::string ToString();
    bool Contains(Word_t value);
    Word_t size();
    Word_t size_of();
    void Add(Word_t value);
    void Clear();
    Word_t GetItem(Py_ssize_t index);
    void Remove(Word_t value, bool raise_key_error);
    std::optional<Word_t> Pop();

    template <typename T>
    static JudyIntSet FromSpan(const std::span<T> view)
    {
        auto s = JudyIntSet();

        for (auto value : view)
            s.Add(static_cast<Word_t>(value));

        return std::move(s);
    }
};


struct JudyIntSetIterator {
    std::shared_ptr<JudyIntSet> judy_set_object;
    bool started;
    bool ended;
    std::optional<Word_t> lower_inclusive;
    std::optional<Word_t> upper_inclusive;
    Word_t key;

    JudyIntSetIterator(
        std::shared_ptr<JudyIntSet> parent,
        std::optional<Word_t> lower_inclusive,
        std::optional<Word_t> upper_inclusive
    );
    Word_t Next();

private:
    void CheckPastUpper(Word_t key);
};
