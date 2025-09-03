#include "judy_int_set.h"

JudyIntSet::JudyIntSet()
{
    judy_set = nullptr;
}

JudyIntSet::~JudyIntSet()
{
    nb::ft_lock_guard guard(mutex);
    Word_t bytes = 0;

    if (judy_set != nullptr)
        J1FA(bytes, judy_set);

    judy_set = nullptr;
}

std::string JudyIntSet::ToString()
{
    std::stringbuf sbuf;
    std::ostream os(&sbuf);
    os << "JudyIntSet([";

    {
        nb::ft_lock_guard guard(mutex);

        Word_t v = 0;
        int i = 0;
        J1F(i, judy_set, v);

        if (i != 0) {
            os << v;

            while (true) {
                J1N(i, judy_set, v);

                if (i == 0)
                    break;

                os << ", " << v;
            }
        }
    }

    os << "])";
    return sbuf.str();
}

bool JudyIntSet::Contains(Word_t value)
{
    nb::ft_lock_guard guard(mutex);
    int i = 0;
    J1T(i, judy_set, value);
    return i ? true : false;
}

Word_t JudyIntSet::size()
{
    nb::ft_lock_guard guard(mutex);
    Word_t count;
    J1C(count, judy_set, 0, -1);
    return count;
}

Word_t JudyIntSet::size_of()
{
    nb::ft_lock_guard guard(mutex);
    Word_t size = 0;
    J1MU(size, judy_set);
    return size + sizeof(*this);
}

void JudyIntSet::Add(Word_t value)
{
    nb::ft_lock_guard guard(mutex);
    int i = 0;

    J1S(i, judy_set, value);

    if (i == 0)
        throw std::bad_alloc();
}

Word_t JudyIntSet::GetItem(Py_ssize_t index)
{
    nb::ft_lock_guard guard(mutex);

    if (index < 0)
        throw nb::index_error();

    Word_t w;
    // +1 cause Judy indexes are 1-based
    Word_t n = (Word_t)(index + 1);
    int i;

    J1BC(i, judy_set, n, w);

    if (i == 0)
        throw nb::index_error();

    return w;
}

void JudyIntSet::Remove(Word_t value)
{
    bool key_error = false;
    int i = 0;

    {
        nb::ft_lock_guard guard(mutex);

        if (judy_set == nullptr) {
            key_error = true;
        } else {
            J1U(i, judy_set, value);

            if (i == 0)
                key_error = true;
        }
    }

    if (key_error)
        throw nb::key_error(std::to_string(value).c_str());
}


JudyIntSetIterator::JudyIntSetIterator(
    std::shared_ptr<JudyIntSet> parent,
    std::optional<Word_t> lower_inclusive,
    std::optional<Word_t> upper_inclusive
)
    : judy_set_object(parent)
    , lower_inclusive(lower_inclusive)
    , upper_inclusive(upper_inclusive)
{
    started = false;
    ended = false;
    key = 0;
}


void JudyIntSetIterator::CheckPastUpper(Word_t key)
{
    // maybe past upper limit, if any
    if (upper_inclusive.has_value() and key > upper_inclusive.value()) {
        ended = true;
        throw nb::stop_iteration();
    }
}


Word_t JudyIntSetIterator::Next()
{
    nb::ft_lock_guard guard(judy_set_object->mutex);

    // already exhausted
    if (ended)
        throw nb::stop_iteration();

    int i = 0;

    // first item
    if (!started) {
        if (lower_inclusive.has_value() && lower_inclusive.value() > 0) {
            key = lower_inclusive.value() - 1;
            J1N(i, judy_set_object->judy_set, key);
        } else {
            J1F(i, judy_set_object->judy_set, key);
        }

        started = true;

        if (i == 0) {
            ended = true;
            throw nb::stop_iteration();
        }

        CheckPastUpper(key);
        return key;
    }

    // next item
    J1N(i, judy_set_object->judy_set, key);

    if (i != 0) {
        CheckPastUpper(key);
        return key;
    }

    // ...beyond last item
    ended = true;
    throw nb::stop_iteration();
}
