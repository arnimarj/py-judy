#include <Judy.h>

#include <sstream>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/shared_ptr.h>

namespace nb = nanobind;
using namespace nb::literals;


struct JudyIntSet {
    Pvoid_t judy_set;
    nb::ft_mutex mutex;

    JudyIntSet();
    ~JudyIntSet();

    std::string ToString();
    bool Contains(Word_t value);
    Word_t size();
    Word_t size_of();
    void Add(Word_t value);
    Word_t GetItem(Py_ssize_t index);
    void Remove(Word_t value);
};


struct JudyIntSetIterator {
    std::shared_ptr<JudyIntSet> judy_set_object;
    bool started;
    bool ended;
    Word_t cursor;

    JudyIntSetIterator(std::shared_ptr<JudyIntSet> parent);
    Word_t Next();
};
