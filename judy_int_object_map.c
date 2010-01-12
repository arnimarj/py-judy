#include "judy.h"

#define PYJUDY_IO_MAP_FOREACH(a, f, T) { Word_t i = 0; PWord_t v = 0; JLF(v, a, i); while ((v) != 0) { f((T)(*v)); JLN(v, a, i); } }

static void set_key_error(PyObject* arg)
{
	PyObject* tup = PyTuple_Pack(1, arg);

	if (tup == 0)
		return;

	PyErr_SetObject(PyExc_KeyError, tup);
	Py_DECREF(tup);
}

static int pyobject_as_word_t(PyObject* p, Word_t* v)
{
	unsigned PY_LONG_LONG pv = 0;

	if (PyInt_Check(p)) {
		long pv_ = PyInt_AS_LONG(p);

		if (pv_ < 0)
			 return 0;

		pv = (unsigned PY_LONG_LONG)pv_;
	} else if (PyLong_Check(p)) {
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

static PyObject* judy_i_o_map_clear(PyObject* op)
{
	PyJudyIntObjectMap* m = (PyJudyIntObjectMap*)op;
	PYJUDY_IO_MAP_FOREACH(m->judy_L, Py_DECREF, PyObject*);
	Word_t bytes_freed;
	JLFA(bytes_freed, m->judy_L);
	m->judy_L = 0;
	Py_INCREF(Py_None);
	return Py_None;
}

static void judy_i_o_map_dealloc(PyJudyIntObjectMap* m)
{
 	PyObject_GC_UnTrack(m);
	Py_TRASHCAN_SAFE_BEGIN(m)
	PYJUDY_IO_MAP_FOREACH(m->judy_L, Py_DECREF, PyObject*);
	Word_t bytes_freed;
	JLFA(bytes_freed, m->judy_L);
	m->judy_L = 0;
	Py_TYPE(m)->tp_free((PyObject*)m);
	Py_TRASHCAN_SAFE_END(m)
}

static int judy_i_o_map_print(PyJudyIntObjectMap* m, FILE* fp, int flags)
{
	int status = Py_ReprEnter((PyObject*)m);

	if (status != 0) {
		if (status < 0)
			return status;

		Py_BEGIN_ALLOW_THREADS
		fprintf(fp, "{...}");
		Py_END_ALLOW_THREADS
		return 0;
	}

	Py_BEGIN_ALLOW_THREADS
	fprintf(fp, "{");
	Py_END_ALLOW_THREADS

	Word_t n = 0;
	Word_t i = 0;
	PWord_t v = 0;

	JLF(v, m->judy_L, i);

	while (v != 0) {
		Py_INCREF((PyObject*)(*v));

		if (n++ > 0) {
			Py_BEGIN_ALLOW_THREADS
			fprintf(fp, ", ");
			Py_END_ALLOW_THREADS
		}

		Py_BEGIN_ALLOW_THREADS
		fprintf(fp, "%llu: ", (unsigned long long)i);
		Py_END_ALLOW_THREADS

		if (PyObject_Print((PyObject*)(*v), fp, 0) != 0) {
			Py_DECREF((PyObject*)(*v));
			Py_ReprLeave((PyObject*)m);
			return -1;
		}

		Py_DECREF((PyObject*)(*v));

		JLN(v, m->judy_L, i);
	}

	Py_BEGIN_ALLOW_THREADS
	fprintf(fp, "}");
	Py_END_ALLOW_THREADS

	Py_ReprLeave((PyObject*)m);

	return 0;
}

static PyObject* judy_i_o_map_repr(PyJudyIntObjectMap* m)
{
	// TBD
	return PyString_FromString("{...}");
}


static int judy_i_o_map_traverse(PyObject* op, visitproc visit, void* arg)
{
	if (!PyJudyIntObjectMap_Check(op))
		return 0;

	Pvoid_t a = ((PyJudyIntObjectMap*)op)->judy_L;
	PYJUDY_IO_MAP_FOREACH(a, Py_VISIT, PyObject*);

	return 0;
}

static int judy_i_o_map_tp_clear(PyObject* op)
{
	PyJudyIntObjectMap* m = (PyJudyIntObjectMap*)op;
	PYJUDY_IO_MAP_FOREACH(m->judy_L, Py_DECREF, PyObject*);
	Word_t bytes_freed;
	JLFA(bytes_freed, m->judy_L);
	m->judy_L = 0;
	return 0;
}

static Py_ssize_t judy_i_o_map_length(PyJudyIntObjectMap* m)
{
	Word_t count;
	JLC(count, m->judy_L, 0, -1);
	//! 2**31-1 on 32-bit systems, 2**63-1 on 64-bit systems
	return (Py_ssize_t)count;
}

static PyObject* judy_i_o_map_subscript(PyJudyIntObjectMap* m, PyObject* k)
{
	Word_t k_ = 0;

	if (!pyobject_as_word_t(k, &k_)) {
		set_key_error(k);
		return 0;
	}

	Word_t* v = 0;
	JLG(v, m->judy_L, k_);

	if (v == 0) {
		set_key_error(k);
		return 0;
	}

	Py_INCREF((PyObject*)(*v));
	return (PyObject*)(*v);
}

static int judy_i_o_map_ass_sub(PyJudyIntObjectMap* m, PyObject* k, PyObject* v)
{
	Word_t k_ = 0;
	int i = 0;

	if (!pyobject_as_word_t(k, &k_)) {
		PyErr_SetString(PyExc_KeyError, "non-integer keys not supported");
		return -1;
	}

	// delete
	if (v == 0) {
		i = 0;
		JLD(i, m->judy_L, k_);

		if (i == 0) {
			set_key_error(k);
			return -1;
		}

		if (i == JERR) {
			PyErr_NoMemory();
			return -1;
		}

		if (i != 1) {
			PyErr_BadInternalCall();
			return -1;
		}

		return 0;
	}

	// insert...
	PWord_t v_ = 0;
	JLG(v_, m->judy_L, k_);

	// replace
	if (v_ != 0) {
		Py_DECREF((PyObject*)(*v_));
		Py_INCREF(v);
		*v_ = (Word_t)v;
		return 0;
	}

	// proper insert
	i = 0;

	JLI(v_, m->judy_L, k_);

	if (v_ == PJERR) {
		PyErr_NoMemory();
		return -1;
	}

	Py_INCREF(v);
	*v_ = (Word_t)v;

	return 0;
}

static PyMappingMethods judy_i_o_map_as_mapping = {
	(lenfunc)judy_i_o_map_length,        /*mp_length*/
	(binaryfunc)judy_i_o_map_subscript,  /*mp_subscript*/
	(objobjargproc)judy_i_o_map_ass_sub, /*mp_ass_subscript*/
};

static int judy_i_o_map_contains(PyJudyIntObjectMap* m, PyObject* k)
{
	Word_t k_ = 0;

	if (!pyobject_as_word_t(k, &k_))
		return 0;

	PWord_t v = 0;
	JLG(v, m->judy_L, k_);

	return (v != 0);
}

static PyObject* judy_i_o_map_contains_(PyJudyIntObjectMap* m, PyObject* k)
{
	int v = judy_i_o_map_contains(m, k);

	if (v == -1)
		return 0;

	return PyBool_FromLong(v);
}

static PySequenceMethods judy_i_o_map_as_sequence = {
	0,			                        /* sq_length */
	0,			                        /* sq_concat */
	0,			                        /* sq_repeat */
	0,			                        /* sq_item */
	0,			                        /* sq_slice */
	0,			                        /* sq_ass_item */
	0,			                        /* sq_ass_slice */
	(objobjproc)judy_i_o_map_contains,  /* sq_contains */
	0,			                        /* sq_inplace_concat */
	0 			                        /* sq_inplace_repeat */
};

static PyObject* judy_i_o_map_sizeof(PyJudyIntObjectMap* m)
{
	Word_t w = 0;
	JLMU(w, m->judy_L);
	return PyLong_FromUnsignedLongLong(w);
}

static PyObject* judy_i_o_map_get(PyJudyIntObjectMap* m, PyObject* args)
{
	PyObject* key = 0;
	PyObject* failobj = Py_None;

	if (!PyArg_UnpackTuple(args, "get", 1, 2, &key, &failobj))
		return 0;

	Word_t k_ = 0;

	if (!pyobject_as_word_t(key, &k_)) {
		Py_INCREF(failobj);
		return failobj;
	}

	PWord_t v = 0;
	JLG(v, m->judy_L, k_);

	if (v == 0) {
		Py_INCREF(failobj);
		return failobj;
	}

	Py_INCREF((PyObject*)(*v));
	return (PyObject*)(*v);
}

static PyObject* judy_i_o_map_pop(PyJudyIntObjectMap* m, PyObject* args)
{
	PyObject* key = 0;
	PyObject* deflt = 0;
	Word_t k_ = 0;

	if (!PyArg_UnpackTuple(args, "pop", 1, 2, &key, &deflt))
		return NULL;

	if (!pyobject_as_word_t(key, &k_)) {
		if (deflt) {
			Py_INCREF(deflt);
			return deflt;
		}

		set_key_error(key);
		return 0;
	}

	PWord_t value = 0;
	JLG(value, m->judy_L, k_);

	if (value == 0) {
		if (deflt != 0) {
			Py_INCREF(deflt);
			return deflt;
		}

		set_key_error(key);
		return 0;
	}

	Word_t v = *value;

	int i = 0;
	JLD(i, m->judy_L, k_);

	if (i == JERR)
		return PyErr_NoMemory();

	if (i != 1) {
		PyErr_BadInternalCall();
		return 0;
	}

	return (PyObject*)v;
}


PyDoc_STRVAR(contains__doc__, "");
PyDoc_STRVAR(getitem__doc__,  "");
PyDoc_STRVAR(sizeof__doc__,   "");
PyDoc_STRVAR(get__doc__,      "");
PyDoc_STRVAR(pop__doc__,      "");
//PyDoc_STRVAR(popitem__doc__,  "");
//PyDoc_STRVAR(keys__doc__,     "");
//PyDoc_STRVAR(items__doc__,    "");
//PyDoc_STRVAR(values__doc__,   "");
//PyDoc_STRVAR(update__doc__,   "");
//PyDoc_STRVAR(fromkeys__doc__, "");
PyDoc_STRVAR(clear__doc__,    "");
//PyDoc_STRVAR(copy__doc__,     "");
//PyDoc_STRVAR(iterkeys__doc__, "");
//PyDoc_STRVAR(iervalues__doc__, "");
//PyDoc_STRVAR(iteritems__doc__, "");

static PyMethodDef judy_io_map_methods[] = {
	{"__contains__", (PyCFunction)judy_i_o_map_contains_,    METH_O | METH_COEXIST,        contains__doc__},
	{"__getitem__",  (PyCFunction)judy_i_o_map_subscript,    METH_O | METH_COEXIST,        getitem__doc__},
	{"__sizeof__",   (PyCFunction)judy_i_o_map_sizeof,       METH_NOARGS,                  sizeof__doc__},
	{"get",          (PyCFunction)judy_i_o_map_get,          METH_VARARGS,                 get__doc__},
	{"pop",          (PyCFunction)judy_i_o_map_pop,          METH_VARARGS,                 pop__doc__},
//	{"popitem",      (PyCFunction)judy_i_o_map_popitem,      METH_NOARGS,                  popitem__doc__},
//	{"keys",         (PyCFunction)judy_i_o_map_keys,         METH_NOARGS,                  keys__doc__},
//	{"items",        (PyCFunction)judy_i_o_map_items,        METH_NOARGS,                  items__doc__},
//	{"values",       (PyCFunction)judy_i_o_map_values,       METH_NOARGS,                  values__doc__},
//	{"update",       (PyCFunction)judy_i_o_map_update,       METH_VARARGS | METH_KEYWORDS, update__doc__},
//	{"fromkeys",     (PyCFunction)dict_fromkeys,             METH_VARARGS | METH_CLASS,    fromkeys__doc__},
	{"clear",        (PyCFunction)judy_i_o_map_clear,        METH_NOARGS,                  clear__doc__},
//	{"copy",         (PyCFunction)judy_i_o_map_copy,         METH_NOARGS,                  copy__doc__},
//	{"iterkeys",     (PyCFunction)judy_i_o_map_iterkeys,     METH_NOARGS,                  iterkeys__doc__},
//	{"itervalues",   (PyCFunction)judy_i_o_map_itervalues,   METH_NOARGS,                  itervalues__doc__},
//	{"iteritems",    (PyCFunction)judy_i_o_map_iteritems,    METH_NOARGS,                  iteritems__doc__},
	{NULL,      NULL}
};

static int judy_int_object_map_init(PyObject* self, PyObject* args, PyObject* kwds)
{
	//! TBD: use parameters for something
	return 0;
}


static PyObject* judy_int_object_map_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	PyObject* self = type->tp_alloc(type, 0);

	if (self != 0) {
		PyJudyIntObjectMap* m = (PyJudyIntObjectMap*)self;
		m->judy_L = 0;
	}

	return self;
}

PyTypeObject PyJudyIntObjectMapType = {
	PyObject_HEAD_INIT(NULL)
	0,                                   /*ob_size*/
	"PyJudyIntObjectMap",                /*tp_name*/
	sizeof(PyJudyIntObjectMap),          /*tp_basicsize*/
	0,                                   /*tp_itemsize*/
	(destructor)judy_i_o_map_dealloc,    /*tp_dealloc*/
	(printfunc)judy_i_o_map_print,       /*tp_print*/
	0,                                   /*tp_getattr*/
	0,                                   /*tp_setattr*/
	0,//(cmpfunc)judy_i_o_map_compare,   /*tp_compare*/
	(reprfunc)judy_i_o_map_repr,         /*tp_repr*/
	0,                                   /*tp_as_number*/
	&judy_i_o_map_as_sequence,           /* tp_as_sequence */
	&judy_i_o_map_as_mapping,            /* tp_as_mapping */
	0,                                   /* tp_hash */
	0,                                   /* tp_call */
	0,                                   /* tp_str */
	PyObject_GenericGetAttr,             /* tp_getattro */
	0,                                   /* tp_setattro */
	0,                                   /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
	Py_TPFLAGS_BASETYPE | Py_TPFLAGS_DICT_SUBCLASS, /* tp_flags */
	0,//judy_int_object_map_doc,                        /* tp_doc */
	judy_i_o_map_traverse,               /* tp_traverse */
	judy_i_o_map_tp_clear,               /* tp_clear */
	0,//judy_int_object_map_richcompare, /* tp_richcompare */
	0,                                   /* tp_weaklistoffset */
	0,//(getiterfunc)judy_int_object_map_iter,          /* tp_iter */
	0,                                   /* tp_iternext */
	judy_io_map_methods,                 /* tp_methods */
	0,                                   /* tp_members */
	0,                                   /* tp_getset */
	0,                                   /* tp_base */
	0,                                   /* tp_dict */
	0,                                   /* tp_descr_get */
	0,                                   /* tp_descr_set */
	0,                                   /* tp_dictoffset */
	judy_int_object_map_init,            /* tp_init */
	PyType_GenericAlloc,                 /* tp_alloc */
	judy_int_object_map_new,             /* tp_new */
	PyObject_GC_Del                      /* tp_free */
};

PyObject* PyJudyIntObjectMap_New(void)
{
	PyJudyIntObjectMap* m = PyObject_GC_New(PyJudyIntObjectMap, &PyJudyIntObjectMapType);

	if (m == 0)
		return 0;

	m->judy_L = (Pvoid_t)0;
	PyObject_GC_Track(m);

	return (PyObject*)m;
}
