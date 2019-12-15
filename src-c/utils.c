#include "utils.h"

int pyobject_as_word_t(PyObject* p, Word_t* v)
{
	unsigned PY_LONG_LONG pv = 0;

	if (PyLong_Check(p)) {
		pv = PyLong_AsUnsignedLongLong(p);

		// not in range of [0, 2**64-1]
		if (PyErr_Occurred()) {
			PyErr_Clear();
			return 0;
		}
	} else {
		return 0;
	}

	if (pv > ULONG_MAX)
		return 0;

	*v = (Word_t)pv;
	return 1;
}

void set_key_error(PyObject* arg)
{
	PyObject* tup = PyTuple_Pack(1, arg);

	if (tup == 0)
		return;

	PyErr_SetObject(PyExc_KeyError, tup);
	Py_DECREF(tup);
}

void judy_set_error(JError_t* error)
{
	switch (JU_ERRNO(error)) {
		case JU_ERRNO_NONE:          PyErr_SetString(PyExc_TypeError, "internal error 1"); break;
		case JU_ERRNO_FULL:          PyErr_SetString(PyExc_TypeError, "internal error 2"); break;
		case JU_ERRNO_NOMEM:         PyErr_NoMemory();                                     break;
		case JU_ERRNO_NULLPPARRAY:   PyErr_SetString(PyExc_TypeError, "internal error 3"); break;
		case JU_ERRNO_NONNULLPARRAY: PyErr_SetString(PyExc_TypeError, "internal error 4"); break;
		case JU_ERRNO_NULLPINDEX:    PyErr_SetString(PyExc_TypeError, "internal error 5"); break;
		case JU_ERRNO_NULLPVALUE:    PyErr_SetString(PyExc_TypeError, "internal error 6"); break;
		case JU_ERRNO_NOTJUDY1:      PyErr_SetString(PyExc_TypeError, "internal error 7"); break;
		case JU_ERRNO_NOTJUDYL:      PyErr_SetString(PyExc_TypeError, "internal error 8"); break;
		case JU_ERRNO_NOTJUDYSL:     PyErr_SetString(PyExc_TypeError, "internal error 9"); break;
		case JU_ERRNO_UNSORTED:      PyErr_SetString(PyExc_TypeError, "internal error 10"); break;
		case JU_ERRNO_OVERRUN:       PyErr_SetString(PyExc_TypeError, "internal error 11"); break;
		case JU_ERRNO_CORRUPT:       PyErr_SetString(PyExc_TypeError, "internal error 12"); break;
		default:                     PyErr_SetString(PyExc_TypeError, "internal error 13"); break;
	}
}
