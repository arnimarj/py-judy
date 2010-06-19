#ifndef __PYJUDY__UTILS__H__
#define __PYJUDY__UTILS__H__

#include <Python.h>
#include <Judy.h>

int pyobject_as_word_t(PyObject* p, Word_t* v);
void set_key_error(PyObject* arg);
void judy_set_error(JError_t* error);

#endif
