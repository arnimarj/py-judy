#include <Judy.h>


template <typename M, typename T, T (*Func)(Word_t, Word_t)>
struct JudyMapIterator {
    std::shared_ptr<M> judy_map_object;
    bool started;
    bool ended;
    Word_t key;

    // for range iteration
    std::optional<Word_t> lower_inclusive;
    std::optional<Word_t> upper_inclusive;

    JudyMapIterator(
        std::shared_ptr<M>& parent,
        std::optional<Word_t> lower_inclusive,
        std::optional<Word_t> upper_inclusive
    )
        : judy_map_object(parent)
        , lower_inclusive(lower_inclusive)
        , upper_inclusive(upper_inclusive)

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
            if (lower_inclusive.has_value() && lower_inclusive.value() > 0) {
                key = lower_inclusive.value() - 1;
                JLN(v, judy_map_object->judy_map, key);
            } else {
                JLF(v, judy_map_object->judy_map, key);
            }
            started = true;

        // next item
        } else {
            JLN(v, judy_map_object->judy_map, key);
        }

        if (v != nullptr) {
            CheckPastUpper(key);
            return Func(key, *((Word_t*)v));
        }

        // beyond last item
        ended = true;
        throw nb::stop_iteration();
    }

private:
    void CheckPastUpper(Word_t key)
    {
        // maybe past upper limit, if any
        if (upper_inclusive.has_value() and key > upper_inclusive.value()) {
            ended = true;
            throw nb::stop_iteration();
        }
    }
};
