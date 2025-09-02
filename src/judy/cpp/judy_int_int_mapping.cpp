#include "judy_int_int_mapping.h"


JudyIntIntMap::JudyIntIntMap()
{
    judy_map = nullptr;
}

JudyIntIntMap::~JudyIntIntMap()
{
    nb::ft_lock_guard guard(mutex);
    Word_t bytes_freed;
    JLFA(bytes_freed, judy_map);
    judy_map = 0;
}

bool JudyIntIntMap::Contains(Word_t value)
{
    nb::ft_lock_guard guard(mutex);
    void* v = nullptr;
    JLG(v, judy_map, value);
    return (v != nullptr);
}

Word_t JudyIntIntMap::size()
{
    nb::ft_lock_guard guard(mutex);
    Word_t count = 0;
    JLC(count, judy_map, 0, -1);
    return count;
}

Word_t JudyIntIntMap::size_of()
{
    nb::ft_lock_guard guard(mutex);
    Word_t size = 0;
    JLMU(size, judy_map);
    return size + sizeof(*this);
}

void JudyIntIntMap::Clear()
{
    nb::ft_lock_guard guard(mutex);
    Word_t bytes_freed = 0;
    JLFA(bytes_freed, judy_map);
    judy_map = nullptr;
}

Word_t JudyIntIntMap::GetItem(Word_t key)
{
    nb::ft_lock_guard guard(mutex);

    void* v = nullptr;
    JLG(v, judy_map, key);

    if (v == nullptr)
        throw nb::key_error();

    return *((Word_t*)v);
}

std::string JudyIntIntMap::ToString()
{
    std::stringbuf sbuf;
    std::ostream os(&sbuf);
    os << "{";

    {
        nb::ft_lock_guard guard(mutex);

        void* v = nullptr;
        Word_t key = 0;
        JLF(v, judy_map, key);

        if (v != nullptr)
            os << key << ": " << *((Word_t*)v);

        while (true) {
            JLN(v, judy_map, key);

            if (v == nullptr)
                break;

            os << ", " << key << ": " << *((Word_t*)v);
        }
    }

    os << "}";
    return sbuf.str();
}

std::variant<Word_t, std::optional<nb::handle>> JudyIntIntMap::Get(Word_t key, std::optional<nb::handle> failobj)
{
    nb::ft_lock_guard guard(mutex);
    void* v = nullptr;
    JLG(v, judy_map, key);

    if (v == nullptr)
        return failobj;

    return *((Word_t*)v);
}

void JudyIntIntMap::SetItem(Word_t key, Word_t value)
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

Word_t JudyIntIntMap::Pop(Word_t key)
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

std::variant<Word_t, std::optional<nb::handle>> JudyIntIntMap::PopDefault(Word_t key, std::optional<nb::handle> failobj)
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

Word_t JudyIntIntMap::ByIndex(Py_ssize_t index)
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
