#include "judy_int_object_map.h"


static void UnlockedClear(Pvoid_t judy_map)
{
    void* v = nullptr;
    Word_t key = 0;
    JLF(v, judy_map, key);

    while (v != nullptr) {
        nb::handle(*((PyObject**)v)).dec_ref();
        JLN(v, judy_map, key);
    }

    // free the map
    Word_t bytes_freed;
    JLFA(bytes_freed, judy_map);
}


JudyIntObjectMap::JudyIntObjectMap()
{
    judy_map = nullptr;
}


JudyIntObjectMap::~JudyIntObjectMap()
{
    nb::ft_lock_guard guard(mutex);
    UnlockedClear(judy_map);
    judy_map = nullptr;
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


Word_t JudyIntObjectMap::size_of()
{
    nb::ft_lock_guard guard(mutex);
    Word_t size = 0;
    JLMU(size, judy_map);
    return size + sizeof(*this);
}


void JudyIntObjectMap::Clear()
{
    nb::ft_lock_guard guard(mutex);
    UnlockedClear(judy_map);
    judy_map = nullptr;
}




int JudyIntObjectMap::GCVisit(visitproc visit, void*arg)
{
    // this may need to be recursive
    nb::ft_lock_guard guard(mutex);

    void* v = nullptr;
    Word_t key = 0;
    JLF(v, judy_map, key);

    while (v != nullptr) {
        Py_VISIT(*((PyObject**)v));
        JLN(v, judy_map, key);
    }

    return 0;
}


nb::object JudyIntObjectMap::GetItem(Word_t key)
{
    nb::ft_lock_guard guard(mutex);

    void* v = nullptr;
    JLG(v, judy_map, key);

    if (v == nullptr)
        throw nb::key_error(std::to_string(key).c_str());

    PyObject* obj = *((PyObject**)v);

    return nb::borrow(obj);
}


void JudyIntObjectMap::SetItem(Word_t key, nb::handle value)
{
    nb::ft_lock_guard guard(mutex);

    // get pointer to current item, if if exists
    void* v = nullptr;
    JLG(v, judy_map, key);

    // if it already exists, decref it
    if (v != nullptr) {
        nb::handle(*((PyObject**)v)).dec_ref();
    }

    if (v == nullptr) {
        // otherwise, create new item and get pointer to that
        // insert otherwise
        JLI(v, judy_map, key);

        if (v == PJERR)
            throw std::bad_alloc();
    }

    PyObject* obj = value.ptr();
    value.inc_ref();
    *((PyObject**)v) = obj;
}


void JudyIntObjectMap::DeleteItem(Word_t key)
{
    auto h = Pop(key);
    h.dec_ref();
}


nb::handle JudyIntObjectMap::Pop(Word_t key)
{
    nb::ft_lock_guard guard(mutex);

    void* v = nullptr;
    JLG(v, judy_map, key);

    if (v == nullptr) {
        throw nb::key_error(std::to_string(key).c_str());
    }

    Word_t value = *((Word_t*)v);
    int i = 0;
    JLD(i, judy_map, key);

    if (i == JERR)
        throw std::bad_alloc();

    auto h = nb::handle(*((PyObject**)v));
    h.dec_ref();
    return h;
}


std::optional<nb::handle> JudyIntObjectMap::PopDefault(Word_t key, std::optional<nb::handle> failobj)
{
    try {
        return Pop(key);
    }
    catch (const nb::builtin_exception &e)
    {
        if (e.type() == nb::exception_type::key_error)
            return failobj;

        throw;
    }
}

// see https://nanobind.readthedocs.io/en/latest/refleaks.html
int int_obj_map_object_traverse(PyObject* self, visitproc visit, void* arg)
{
    #if PY_VERSION_HEX >= 0x03090000
        Py_VISIT(Py_TYPE(self));
    #endif

    if (!nb::inst_ready(self))
       return 0;

    return nb::inst_ptr<JudyIntObjectMap>(self)->GCVisit(visit, arg);
}


int int_obj_map_object_clear(PyObject* self)
{
    JudyIntObjectMap* obj = nb::inst_ptr<JudyIntObjectMap>(self);
    nb::ft_lock_guard guard(obj->mutex);

    // since we may get called recursively when dec-refing, we clear
    // the map first

    auto m = obj->judy_map;
    obj->judy_map = nullptr;
    UnlockedClear(m);
    return 0;
}
