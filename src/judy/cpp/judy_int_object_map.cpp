#include "judy_int_object_map.h"


JudyIntObjectMap::JudyIntObjectMap()
{
    judy_map = nullptr;
}


JudyIntObjectMap::~JudyIntObjectMap()
{
    nb::ft_lock_guard guard(mutex);
    UnlockedClear();
}


bool JudyIntObjectMap::Contains(Word_t value)
{
    nb::ft_lock_guard guard(mutex);
    void* v = nullptr;
    JLG(v, judy_map, value);
    return (v != nullptr);
}


Word_t JudyIntObjectMap::size()
{
    nb::ft_lock_guard guard(mutex);
    Word_t count = 0;
    JLC(count, judy_map, 0, -1);
    return count;
}


void JudyIntObjectMap::Clear()
{
    nb::ft_lock_guard guard(mutex);
    UnlockedClear();
}


void JudyIntObjectMap::UnlockedClear()
{
    void* v = nullptr;
    Word_t key = 0;
    JLF(v, judy_map, key);

    if (v != nullptr) {
    	Py_DECREF(v);

        while (true) {
            JLN(v, judy_map, key);

            if (v == nullptr)
                break;

	    	Py_DECREF(v);
        }
    }

    // free the map
    Word_t bytes_freed;
    JLFA(bytes_freed, judy_map);
    judy_map = nullptr;
}
