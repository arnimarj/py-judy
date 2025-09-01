#include <Judy.h>

struct JudyIntSet {
    Pvoid_t judy_set;
    nb::ft_mutex mutex;

    JudyIntSet();
    ~JudyIntSet();
};


struct JudyIntSetIterator {
    std::shared_ptr<JudyIntSet> judy_set_object;
    bool started;
    bool ended;
    Word_t cursor;
};
