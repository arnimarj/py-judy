#include "judy.h"

PyTypeObject PyJudyIntSetType;
PyTypeObject PyJudyIntSetIterType;

static void print_word_and_error(const char* s, Word_t w, JError_t* e)
{
	switch (JU_ERRNO(e)) {
		case JU_ERRNO_NONE:          printf(" JU_ERRNO_NONE\n");          break;
		case JU_ERRNO_FULL:          printf(" JU_ERRNO_FULL\n");          break;
		//case JU_ERRNO_NFMAX:         printf(" JU_ERRNO_NFMAX\n");         break;
		case JU_ERRNO_NOMEM:         printf(" JU_ERRNO_NOMEM\n");         break;
		case JU_ERRNO_NULLPPARRAY:   printf(" JU_ERRNO_NULLPPARRAY\n");   break;
		case JU_ERRNO_NONNULLPARRAY: printf(" JU_ERRNO_NONNULLPARRAY\n"); break;
		case JU_ERRNO_NULLPINDEX:    printf(" JU_ERRNO_NULLPINDEX\n");    break;
		case JU_ERRNO_NULLPVALUE:    printf(" JU_ERRNO_NULLPVALUE\n");    break;
		case JU_ERRNO_NOTJUDY1:      printf(" JU_ERRNO_NOTJUDY1\n");      break;
		case JU_ERRNO_NOTJUDYL:      printf(" JU_ERRNO_NOTJUDYL\n");      break;
		case JU_ERRNO_NOTJUDYSL:     printf(" JU_ERRNO_NOTJUDYSL\n");     break;
		case JU_ERRNO_UNSORTED:      printf(" JU_ERRNO_UNSORTED\n");      break;
		case JU_ERRNO_OVERRUN:       printf(" JU_ERRNO_OVERRUN\n");       break;
		case JU_ERRNO_CORRUPT:       printf(" JU_ERRNO_CORRUPT\n");       break;
		default:                     printf(" UNKNOWN\n");                break;
	}
}

static void PyJudyIntSet_dealloc(PyJudyIntSet* self)
{
	if (self->s) {
		JError_t JError;
		Word_t w = Judy1FreeArray(&self->s, &JError);

		if (w == JERR)
			print_word_and_error("PyJudyIntSet_dealloc", w, &JError);

	}
	self->ob_type->tp_free((PyObject*)self);
}

static void PyJudyIntSetIter_dealloc(PyJudyIntSetIter* self)
{
	Py_XDECREF(self->s);
	self->ob_type->tp_free((PyObject*)self);
}

PyObject* PyJudyIntSet_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	PyJudyIntSet* self = (PyJudyIntSet*)type->tp_alloc(type, 0);

	if (self)
		self->s = 0;

	return (PyObject*)self;
}

PyObject* PyJudyIntSetIter_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	PyJudyIntSetIter* self = (PyJudyIntSetIter*)type->tp_alloc(type, 0);

	if (self) {
		self->b = 0;
		self->i = 0;
		self->s = 0;
	}

	return (PyObject*)self;
}

static int PyJudyIntSet_init(PyJudyIntSet* self, PyObject* args)
{
	if (self->s) {
		JError_t JError;
		Word_t w = Judy1FreeArray(&self->s, &JError);

		if (w == JERR)
			print_word_and_error("PyJudyIntSet_init", w, &JError);
	}

	if (!PyArg_ParseTuple(args, ""))
		return -1;

	return 0;
}

static int PyJudyIntSetIter_init(PyJudyIntSetIter* self, PyObject* args)
{
	PyErr_SetString(PyExc_TypeError, "unable to instantiate PyJudyIntSetIter directly");
	return -1;
}

static PyObject* PyJudyIntSet_iter(PyObject* s)
{
	if (!PyJudyIntSet_Check(s)) {
		PyErr_BadInternalCall();
		return 0;
	}

	PyJudyIntSetIter* iter = PyObject_New(PyJudyIntSetIter, &PyJudyIntSetIterType);

	if (iter == 0)
		return 0;

	iter = (PyJudyIntSetIter*)PyObject_Init((PyObject*)iter, &PyJudyIntSetIterType);

	Py_INCREF(s);

	iter->b = 0;
	iter->i = 0;
	iter->s = (PyJudyIntSet*)s;

	return (PyObject*)iter;
}

static PyObject* PyJudyIntSet_add(PyJudyIntSet* self, PyObject* key)
{
	Word_t v;

	if (!pyobject_as_word_t(key, &v)) {
		PyErr_Format(PyExc_ValueError, "we only support integers in the range [0, 2**%i-1]", sizeof(Word_t) == 4 ? 32 : 64);
		return 0;
	}

	JError_t JError;
	Word_t w = Judy1Set(&self->s, v, &JError);

	if (w == JERR) {
		judy_set_error(&JError);
		return 0;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* PyJudyIntSet_remove(PyJudyIntSet* self, PyObject* key)
{
	Word_t v;

	if (self->s == 0 || !pyobject_as_word_t(key, &v)) {
		set_key_error(key);
		return 0;
	}

	JError_t JError;
	int i = Judy1Unset(&self->s, (Word_t)v, &JError);

	if (i == JERR) {
		judy_set_error(&JError);
		return 0;
	}

	if (i == 0) {
		set_key_error(key);
		return 0;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* PyJudyIntSet_sizeof(PyJudyIntSet* self)
{
	Word_t w = Judy1MemUsed(self->s);
	return Py_BuildValue("K", (unsigned PY_LONG_LONG)w + sizeof(PyJudyIntSet));
}

static int PyJudyIntSet_contains(PyJudyIntSet* self, PyObject* key)
{
	Word_t v;

	if (!pyobject_as_word_t(key, &v))
		return 0;

	JError_t JError;
	int i = Judy1Test(self->s, v, &JError);

	if (i == JERR) {
		judy_set_error(&JError);
		return -1;
	}

	return (i ? 1 : 0);
}

static PySequenceMethods PyJudyIntSet_as_sequence = {
	0,                                 /* sq_length */
	0,                                 /* sq_concat */
	0,                                 /* sq_repeat */
	0,                                 /* sq_item */
	0,                                 /* sq_slice */
	0,                                 /* sq_ass_item */
	0,                                 /* sq_ass_slice */
	(objobjproc)PyJudyIntSet_contains, /* sq_contains */
};

static PyMethodDef PyJudyIntSet_methods[] = {
	{"add",        (PyCFunction)PyJudyIntSet_add,    METH_O,       ""},
	{"remove",     (PyCFunction)PyJudyIntSet_remove, METH_O,       ""},
	{"__sizeof__", (PyCFunction)PyJudyIntSet_sizeof, METH_NOARGS,  ""},
	{NULL, NULL}
};

PyTypeObject PyJudyIntSetType = {
	PyObject_HEAD_INIT(NULL)
	0,                                        /*ob_size*/
	"judy.PyJudyIntSet",                      /*tp_name*/
	sizeof(PyJudyIntSet),                     /*tp_basicsize*/
	0,                                        /*tp_itemsize*/
	(destructor)PyJudyIntSet_dealloc,         /*tp_dealloc*/
	0,                                        /*tp_print*/
	0,                                        /*tp_getattr*/
	0,                                        /*tp_setattr*/
	0,                                        /*tp_compare*/
	0,//PyJudyIntSet_repr,                    /*tp_repr*/
	0,                                        /*tp_as_number*/
	&PyJudyIntSet_as_sequence,                /*tp_as_sequence*/
	0,                                        /*tp_as_mapping*/
	0,                                        /*tp_hash */
	0,                                        /*tp_call*/
	0,//PyJudyIntSet_str,                     /*tp_str*/
	0,                                        /*tp_getattro*/
	0,                                        /*tp_setattro*/
	0,                                        /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,                       /*tp_flags*/
	"PyJudyIntSet",                           /*tp_doc */
	0,                                        /*tp_traverse */
	0,                                        /*tp_clear */
	0,                                        /*tp_richcompare */
	0,                                        /*tp_weaklistoffset */
	PyJudyIntSet_iter,                        /*tp_iter */
	0,                                        /*tp_iternext */
	PyJudyIntSet_methods,                     /*tp_methods */
	0,                                        /*tp_members */
	0,                                        /*tp_getset */
	0,                                        /*tp_base */
	0,                                        /*tp_dict */
	0,                                        /*tp_descr_get */
	0,                                        /*tp_descr_set */
	0,                                        /*tp_dictoffset */
	(initproc)PyJudyIntSet_init,              /*tp_init */
	0,                                        /*tp_alloc */
	PyJudyIntSet_new,                         /*tp_new */
};

static PyObject* PyJudyIntSetIter_iternext(PyJudyIntSetIter* iter)
{
	JError_t JError;
	int i;

	if (iter->s->s == 0)
		return 0;

	if (iter->b == 0) {
		i = Judy1First(iter->s->s, &iter->i, &JError);
		iter->b = 1;													
																																																			
		if (i == JERR) {
			judy_set_error(&JError);
			return 0;
		}

		return PyLong_FromUnsignedLongLong((unsigned PY_LONG_LONG)iter->i);
	}

	i = Judy1Next(iter->s->s, &iter->i, &JError);

	if (i == JERR) {
		judy_set_error(&JError);
		return 0;
	}

	if (i == 0)
		return 0;

	return PyLong_FromUnsignedLongLong((unsigned PY_LONG_LONG)iter->i);
}

PyTypeObject PyJudyIntSetIterType = {
	PyObject_HEAD_INIT(NULL)
	0,                                               /*ob_size*/
	"judy.PyJudyIntSetIter",                         /*tp_name*/
	sizeof(PyJudyIntSetIter),                        /*tp_basicsize*/
	0,                                               /*tp_itemsize*/
	(destructor)PyJudyIntSetIter_dealloc,            /*tp_dealloc*/
	0,                                               /*tp_print*/
	0,                                               /*tp_getattr*/
	0,                                               /*tp_setattr*/
	0,                                               /*tp_compare*/
	0,                                               /*tp_repr*/
	0,                                               /*tp_as_number*/
	0,                                               /*tp_as_sequence*/
	0,                                               /*tp_as_mapping*/
	0,                                               /*tp_hash */
	0,                                               /*tp_call*/
	0,                                               /*tp_str*/
	0,                                               /*tp_getattro*/
	0,                                               /*tp_setattro*/
	0,                                               /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,                              /*tp_flags*/
	"PyJudyIntSetIter",                              /*tp_doc */
	0,                                               /*tp_traverse */
	0,                                               /*tp_clear */
	0,                                               /*tp_richcompare */
	0,                                               /*tp_weaklistoffset */
	PyObject_SelfIter,                               /*tp_iter */
	(iternextfunc)PyJudyIntSetIter_iternext,         /*tp_iternext */
	0,                                               /*tp_methods */
	0,                                               /*tp_members */
	0,                                               /*tp_getset */
	0,                                               /*tp_base */
	0,                                               /*tp_dict */
	0,                                               /*tp_descr_get */
	0,                                               /*tp_descr_set */
	0,                                               /*tp_dictoffset */
	(initproc)PyJudyIntSetIter_init,                 /*tp_init */
	0,                                               /*tp_alloc */
	PyJudyIntSetIter_new,                            /*tp_new */
};
