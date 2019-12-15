#include "judy.h"

#define PYJUDY_IO_MAP_FOREACH(a, f, T) { Word_t i = 0; PWord_t v = 0; JLF(v, a, i); while ((v) != 0) { f((T)(*v)); JLN(v, a, i); } }

static PyObject* judy_io_map_clear(PyObject* op)
{
	PyJudyIntObjectMap* m = (PyJudyIntObjectMap*)op;
	PYJUDY_IO_MAP_FOREACH(m->judy_L, Py_DECREF, PyObject*);
	Word_t bytes_freed;
	JLFA(bytes_freed, m->judy_L);
	m->judy_L = 0;
	Py_INCREF(Py_None);
	return Py_None;
}

#ifdef PYPY_VERSION

#define KEY "Py_Repr"

static int
my_Py_ReprEnter(PyObject *obj)
{
    PyObject *dict;
    PyObject *list;
    Py_ssize_t i;

    dict = PyThreadState_GetDict();
    if (dict == NULL)
        return 0;
    list = PyDict_GetItemString(dict, KEY);
    if (list == NULL) {
        list = PyList_New(0);
        if (list == NULL)
            return -1;
        if (PyDict_SetItemString(dict, KEY, list) < 0)
            return -1;
        Py_DECREF(list);
    }
    i = PyList_GET_SIZE(list);
    while (--i >= 0) {
        if (PyList_GET_ITEM(list, i) == obj)
            return 1;
    }
    PyList_Append(list, obj);
    return 0;
}

static void
my_Py_ReprLeave(PyObject *obj)
{
    PyObject *dict;
    PyObject *list;
    Py_ssize_t i;

    dict = PyThreadState_GetDict();
    if (dict == NULL)
        return;
    list = PyDict_GetItemString(dict, KEY);
    if (list == NULL || !PyList_Check(list))
        return;
    i = PyList_GET_SIZE(list);
    /* Count backwards because we always expect obj to be list[-1] */
    while (--i >= 0) {
        if (PyList_GET_ITEM(list, i) == obj) {
            PyList_SetSlice(list, i, i + 1, NULL);
            break;
        }
    }
}

#else
	#define my_Py_ReprEnter Py_ReprEnter 
	#define my_Py_ReprLeave Py_ReprLeave 
#endif


static void judy_io_map_dealloc(PyJudyIntObjectMap* m)
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

static int judy_io_map_print(PyJudyIntObjectMap* m, FILE* fp, int flags)
{
	if (!m->allow_print)
		return fprintf(fp, "<%s object at %p>", Py_TYPE(m)->tp_name, (void*)m);

	int status = my_Py_ReprEnter((PyObject*)m);

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
			my_Py_ReprLeave((PyObject*)m);
			return -1;
		}

		Py_DECREF((PyObject*)(*v));

		JLN(v, m->judy_L, i);
	}

	Py_BEGIN_ALLOW_THREADS
	fprintf(fp, "}");
	Py_END_ALLOW_THREADS

	my_Py_ReprLeave((PyObject*)m);

	return 0;
}


static int _string_to_list(PyObject* list, const char* string)
{
	PyObject* s = PyUnicode_FromString(string);

	if (s == 0)
		return 0;

	int i = (PyList_Append(list, s) == 0);
	Py_DECREF(s);
	return i;
}

static int _object_to_list(PyObject* list, PyObject* object)
{
	return (PyList_Append(list, object) == 0);
}

static PyObject* judy_io_map_repr(PyJudyIntObjectMap* m)
{
	if (!m->allow_print) {
		return PyUnicode_FromFormat("<%s object at %p>", Py_TYPE(m)->tp_name, (void*)m);
	}

	Py_ssize_t r = my_Py_ReprEnter((PyObject*)m);
	PyObject* retval = 0;
	PyObject* pieces = 0;
	PyObject* s = 0;
	char s_buffer[32];

	if (r != 0) {
		return r > 0 ? PyUnicode_FromString("{...}") : 0;
	}

	if ((pieces = PyList_New(0)) == 0) {
		goto cleanup;
	}

	// for each key/value pair
	Word_t i = 0;
	PWord_t v = 0;

	JLF(v, m->judy_L, i);

	if (v == 0) {
		retval = PyUnicode_FromString("{}");
		goto cleanup;
	}

	if (!_string_to_list(pieces, "{"))
		goto cleanup;

	int first_iteration = 1;

	while (v != 0) {
		if (!first_iteration) {
			if (!_string_to_list(pieces, ","))
				goto cleanup;

			if (!_string_to_list(pieces, " "))
				goto cleanup;
		}

		first_iteration = 0;

		sprintf(s_buffer, "%llu: ", (unsigned long long)i);

		if (!_string_to_list(pieces, s_buffer))
			goto cleanup;

		Py_INCREF((PyObject*)(*v));
		PyObject* repr = PyObject_Repr((PyObject*)(*v));
		Py_DECREF((PyObject*)(*v));

		if (!_object_to_list(pieces, repr)) {
			Py_DECREF(repr);
			goto cleanup;
		}

		Py_DECREF(repr);

		JLN(v, m->judy_L, i);
	}

	if (!_string_to_list(pieces, "}"))
		goto cleanup;

	s = PyUnicode_FromString("");

	if (s == 0)
		goto cleanup;

	retval = PyUnicode_Join(s, pieces);
	Py_DECREF(s);
	s = 0;

cleanup:

	Py_XDECREF(pieces);
	my_Py_ReprLeave((PyObject*)m);

	return retval;
}

static int judy_io_map_traverse(PyObject* op, visitproc visit, void* arg)
{
	if (!PyJudyIntObjectMap_Check(op))
		return 0;

	Pvoid_t a = ((PyJudyIntObjectMap*)op)->judy_L;
	PYJUDY_IO_MAP_FOREACH(a, Py_VISIT, PyObject*);

	return 0;
}

static int judy_io_map_tp_clear(PyObject* op)
{
	PyJudyIntObjectMap* m = (PyJudyIntObjectMap*)op;
	PYJUDY_IO_MAP_FOREACH(m->judy_L, Py_DECREF, PyObject*);
	Word_t bytes_freed;
	JLFA(bytes_freed, m->judy_L);
	m->judy_L = 0;
	return 0;
}

static Py_ssize_t judy_io_map_length(PyJudyIntObjectMap* m)
{
	Word_t count;
	JLC(count, m->judy_L, 0, -1);
	//! 2**31-1 on 32-bit systems, 2**63-1 on 64-bit systems
	return (Py_ssize_t)count;
}

static PyObject* judy_io_map_subscript(PyJudyIntObjectMap* m, PyObject* k)
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

static int judy_io_map_ass_sub(PyJudyIntObjectMap* m, PyObject* k, PyObject* v)
{
	Word_t k_ = 0;
	int i = 0;

	if (!pyobject_as_word_t(k, &k_)) {
		PyErr_SetString(PyExc_KeyError, "non-integer keys not supported");
		return -1;
	}

	// delete
	if (v == 0) {
		// find/delete the object
		i = 0;
		PWord_t v_ = 0;
		Word_t v__ = 0;
		JLG(v_, m->judy_L, k_);

		// if no object: key error
		if (v_ == 0) {
			set_key_error(k);
			return -1;
		}

		v__ = *v_;

		i = 0;
		JLD(i, m->judy_L, k_);

		if (i == 0) {
			//! NOTE: should not fail
			PyErr_BadInternalCall();
			return -1;
		}

		if (i == JERR) {
			PyErr_NoMemory();
			return -1;
		}

		Py_DECREF((PyObject*)(v__));

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

static PyMappingMethods judy_io_map_as_mapping = {
	(lenfunc)judy_io_map_length,        /*mp_length*/
	(binaryfunc)judy_io_map_subscript,  /*mp_subscript*/
	(objobjargproc)judy_io_map_ass_sub, /*mp_ass_subscript*/
};

static int judy_io_map_contains(PyJudyIntObjectMap* m, PyObject* k)
{
	Word_t k_ = 0;

	if (!pyobject_as_word_t(k, &k_))
		return 0;

	PWord_t v = 0;
	JLG(v, m->judy_L, k_);

	return (v != 0);
}

static PyObject* judy_io_map_contains_(PyJudyIntObjectMap* m, PyObject* k)
{
	int v = judy_io_map_contains(m, k);

	if (v == -1)
		return 0;

	return PyBool_FromLong(v);
}

static PySequenceMethods judy_io_map_as_sequence = {
	0,			                        /* sq_length */
	0,			                        /* sq_concat */
	0,			                        /* sq_repeat */
	0,			                        /* sq_item */
	0,			                        /* sq_slice */
	0,			                        /* sq_ass_item */
	0,			                        /* sq_ass_slice */
	(objobjproc)judy_io_map_contains,  /* sq_contains */
	0,			                        /* sq_inplace_concat */
	0 			                        /* sq_inplace_repeat */
};

static PyObject* judy_io_map_sizeof(PyJudyIntObjectMap* m)
{
	Word_t w = 0;
	JLMU(w, m->judy_L);
	return PyLong_FromUnsignedLongLong(sizeof(PyJudyIntObjectMap) + w);
}

static PyObject* judy_io_map_value_sizeof(PyJudyIntObjectMap* m)
{
	Word_t i = 0;
	PWord_t v = 0;
	PyObject* o = 0;
	PyObject* V = 0;
	size_t n_bytes = 0;
	long long L = 0;

	JLF(v, m->judy_L, i);

	while (v != 0) {
		o = (PyObject*)(*v);
		PyObject* func = PyObject_GetAttrString(o, "__sizeof__");

		if (func == 0)
			return 0;

		V = PyObject_CallObject(func, 0);
		Py_DECREF(func);

		if (V == 0)
			return 0;

		if (PyLong_Check(V)) {
			L = PyLong_AsLongLong(V);

			if (PyErr_Occurred()) {
				PyErr_SetString(PyExc_ValueError, "__sizeof__() return an out-of-bounds long");
				Py_DECREF(V);
				return 0;
			}

			n_bytes += L;
		} else {
			PyErr_SetString(PyExc_ValueError, "__sizeof__() did not return an int/long");
			Py_DECREF(V);
			return 0;
		}

		Py_DECREF(V);

		JLN(v, m->judy_L, i);
	}

	return PyLong_FromSize_t(n_bytes);
}

static PyObject* judy_io_map_by_index(PyJudyIntObjectMap* m, PyObject* args)
{
	PyObject* i = 0;
	Word_t i_= 0;

	if (!PyArg_UnpackTuple(args, "by_index", 1, 1, &i) || !pyobject_as_word_t(i, &i_))
		return 0;

	// NOTE: judy index is 1-based
	i_ += 1;
	Word_t k = 0;
	PWord_t v = 0;
	JLBC(v, m->judy_L, i_, k);

	if (v == 0) {
		PyErr_SetString(PyExc_IndexError, "index out of range");
		return 0;
	}

	Py_INCREF((PyObject*)(*v));
	return (PyObject*)(*v);
}

static PyObject* judy_io_map_get(PyJudyIntObjectMap* m, PyObject* args)
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

static PyObject* judy_io_map_pop(PyJudyIntObjectMap* m, PyObject* args)
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

static PyObject* judy_io_map_iterkeys(PyJudyIntObjectMap* set, PyObject* args, PyObject* kwds);
static PyObject* judy_io_map_itervalues(PyJudyIntObjectMap* set, PyObject* args, PyObject* kwds);
static PyObject* judy_io_map_iteritems(PyJudyIntObjectMap* set, PyObject* args, PyObject* kwds);
static PyObject* judy_io_map_iter(PyJudyIntObjectMap* set);

PyDoc_STRVAR(contains__doc__, "");
PyDoc_STRVAR(getitem__doc__,  "");
PyDoc_STRVAR(sizeof__doc__,   "");
PyDoc_STRVAR(by_index__doc__,      "");
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
PyDoc_STRVAR(iterkeys__doc__, "");
PyDoc_STRVAR(itervalues__doc__, "");
PyDoc_STRVAR(iteritems__doc__, "");

static PyMethodDef judy_io_map_methods[] = {
	{"__contains__",     (PyCFunction)judy_io_map_contains_,    METH_O | METH_COEXIST,        contains__doc__},
	{"__getitem__",      (PyCFunction)judy_io_map_subscript,    METH_O | METH_COEXIST,        getitem__doc__},
	{"__sizeof__",       (PyCFunction)judy_io_map_sizeof,       METH_NOARGS,                  sizeof__doc__},
	{"__value_sizeof__", (PyCFunction)judy_io_map_value_sizeof, METH_NOARGS,                  sizeof__doc__},
	{"by_index",         (PyCFunction)judy_io_map_by_index,     METH_VARARGS,                 by_index__doc__},
	{"get",              (PyCFunction)judy_io_map_get,          METH_VARARGS,                 get__doc__},
	{"pop",              (PyCFunction)judy_io_map_pop,          METH_VARARGS,                 pop__doc__},
//	{"popitem",          (PyCFunction)judy_io_map_popitem,      METH_NOARGS,                  popitem__doc__},
//	{"keys",             (PyCFunction)judy_io_map_keys,         METH_NOARGS,                  keys__doc__},
//	{"items",            (PyCFunction)judy_io_map_items,        METH_NOARGS,                  items__doc__},
//	{"values",           (PyCFunction)judy_io_map_values,       METH_NOARGS,                  values__doc__},
//	{"update",           (PyCFunction)judy_io_map_update,       METH_VARARGS | METH_KEYWORDS, update__doc__},
//	{"fromkeys",         (PyCFunction)judy_io_map_fromkeys,     METH_VARARGS | METH_CLASS,    fromkeys__doc__},
	{"clear",            (PyCFunction)judy_io_map_clear,        METH_NOARGS,                  clear__doc__},
//	{"copy",             (PyCFunction)judy_io_map_copy,         METH_NOARGS,                  copy__doc__},
	{"keys",             (PyCFunction)judy_io_map_iterkeys,     METH_VARARGS | METH_KEYWORDS, iterkeys__doc__},
	{"values",           (PyCFunction)judy_io_map_itervalues,   METH_VARARGS | METH_KEYWORDS, itervalues__doc__},
	{"items",            (PyCFunction)judy_io_map_iteritems,    METH_VARARGS | METH_KEYWORDS, iteritems__doc__},
	{NULL, NULL}
};

static int judy_int_object_map_init(PyJudyIntObjectMap* self, PyObject* args, PyObject* kwds)
{
	self->allow_print = 1;
	PyObject* allow_print = Py_True;
	static char* kwargs[] = {"allow_print", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O!", kwargs, &PyBool_Type, &allow_print))
		return -1;

	if (allow_print == Py_False)
		self->allow_print = 0;

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
	PyVarObject_HEAD_INIT(NULL, 0)
	"PyJudyIntObjectMap",                /*tp_name*/
	sizeof(PyJudyIntObjectMap),          /*tp_basicsize*/
	0,                                   /*tp_itemsize*/
	(destructor)judy_io_map_dealloc,    /*tp_dealloc*/
	(printfunc)judy_io_map_print,       /*tp_print*/
	0,                                   /*tp_getattr*/
	0,                                   /*tp_setattr*/
	0,//(cmpfunc)judy_io_map_compare,   /*tp_compare*/
	(reprfunc)judy_io_map_repr,         /*tp_repr*/
	0,                                   /*tp_as_number*/
	&judy_io_map_as_sequence,           /* tp_as_sequence */
	&judy_io_map_as_mapping,            /* tp_as_mapping */
	0,                                   /* tp_hash */
	0,                                   /* tp_call */
	0,                                   /* tp_str */
	PyObject_GenericGetAttr,             /* tp_getattro */
	0,                                   /* tp_setattro */
	0,                                   /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE, /* tp_flags */
	0,//judy_int_object_map_doc,                        /* tp_doc */
	judy_io_map_traverse,               /* tp_traverse */
	judy_io_map_tp_clear,               /* tp_clear */
	0,//judy_int_object_map_richcompare, /* tp_richcompare */
	0,                                   /* tp_weaklistoffset */
	(getiterfunc)judy_io_map_iter,        /* tp_iter */
	0,                                   /* tp_iternext */
	judy_io_map_methods,                 /* tp_methods */
	0,                                   /* tp_members */
	0,                                   /* tp_getset */
	0,                                   /* tp_base */
	0,                                   /* tp_dict */
	0,                                   /* tp_descr_get */
	0,                                   /* tp_descr_set */
	0,                                   /* tp_dictoffset */
	(initproc)judy_int_object_map_init,  /* tp_init */
	PyType_GenericAlloc,                 /* tp_alloc */
	judy_int_object_map_new,             /* tp_new */
	PyObject_GC_Del                      /* tp_free */
};

extern PyTypeObject PyJudyIntObjectMapIterKey_Type;
extern PyTypeObject PyJudyIntObjectMapIterValue_Type;
extern PyTypeObject PyJudyIntObjectMapIterItem_Type;

static PyObject* judy_io_map_iter_new(PyJudyIntObjectMap* set, PyTypeObject* type, PyObject* args, PyObject* kwds);

static PyObject* judy_io_map_iterkeys(PyJudyIntObjectMap* set, PyObject* args, PyObject* kwds)
{
	return judy_io_map_iter_new(set, &PyJudyIntObjectMapIterKey_Type, args, kwds);
}

static PyObject* judy_io_map_itervalues(PyJudyIntObjectMap* set, PyObject* args, PyObject* kwds)
{
	return judy_io_map_iter_new(set, &PyJudyIntObjectMapIterValue_Type, args, kwds);
}

static PyObject* judy_io_map_iteritems(PyJudyIntObjectMap* set, PyObject* args, PyObject* kwds)
{
	return judy_io_map_iter_new(set, &PyJudyIntObjectMapIterItem_Type, args, kwds);
}

static PyObject* judy_io_map_iter(PyJudyIntObjectMap* set)
{
	return judy_io_map_iter_new(set, &PyJudyIntObjectMapIterKey_Type, 0, 0);
}

typedef struct {
	PyObject_HEAD
	PyJudyIntObjectMap* map;
	int is_first;
	Word_t i;
	int is_from;
	int is_to;
	Word_t from;
	Word_t to;
} pyjudy_io_map_iter_object;

static PyObject* judy_io_map_iter_new(PyJudyIntObjectMap* map, PyTypeObject* itertype, PyObject* args, PyObject* kwds)
{
	PyObject* k_from_inc = 0;
	PyObject* k_to_inc = 0;
	Word_t _k_from_inc = 0;
	Word_t _k_to_inc = 0;

	if (args || kwds) {
		static char* kwlist[] = {"k_from_inc", "k_to_inc", 0};

		if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OO", kwlist, &k_from_inc, &k_to_inc))
			return 0;

		if (k_from_inc && !pyobject_as_word_t(k_from_inc, &_k_from_inc)) {
			PyErr_SetString(PyExc_TypeError, "range lower-bound is not an integer or out-of-bounds");
			return 0;
		}

		if (k_to_inc && !pyobject_as_word_t(k_to_inc, &_k_to_inc)) {
			PyErr_SetString(PyExc_TypeError, "range upper-bound is not an integer or out-of-bounds");
			return 0;
		}
	}

	pyjudy_io_map_iter_object* mi;
	mi = PyObject_GC_New(pyjudy_io_map_iter_object, itertype);

	if (mi == 0)
		return 0;

	Py_INCREF(map);
	mi->map = map;
	mi->is_first = 1;
	mi->i = 0;
	mi->is_from = (k_from_inc != 0);
	mi->is_to = (k_to_inc != 0);
	mi->from = _k_from_inc;
	mi->to = _k_to_inc;

	PyObject_GC_Track(mi);
	return (PyObject*)mi;
}

static void judy_io_map_iter_dealloc(pyjudy_io_map_iter_object* mi)
{
	Py_XDECREF(mi->map);
	PyObject_GC_Del(mi);
}

static int judy_io_map_iter_traverse(pyjudy_io_map_iter_object* mi, visitproc visit, void* arg)
{
	Py_VISIT(mi->map);
	return 0;
}

static PyObject* judy_io_map_iter_iternextkey(pyjudy_io_map_iter_object* mi)
{
	JError_t JError;
	PWord_t v = 0;

	if (mi->is_first) {
		if (mi->is_from && mi->from > 0) {
			mi->i = mi->from - 1;
			v = (Pvoid_t)JudyLNext(mi->map->judy_L, &mi->i, &JError);
		} else {
			v = (Pvoid_t)JudyLFirst(mi->map->judy_L, &mi->i, &JError);
		}
	} else {
		v = (Pvoid_t)JudyLNext(mi->map->judy_L, &mi->i, &JError);
	}

	mi->is_first = 0;

	if (v == 0)
		return 0;

	if (mi->is_to && mi->i > mi->to)
		return 0;

	return PyLong_FromUnsignedLongLong((unsigned PY_LONG_LONG)mi->i);
}

PyTypeObject PyJudyIntObjectMapIterKey_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	"judyintmap-keyiterator",                   /* tp_name */
	sizeof(pyjudy_io_map_iter_object),          /* tp_basicsize */
	0,                                          /* tp_itemsize */
	(destructor)judy_io_map_iter_dealloc,       /* tp_dealloc */
	0,                                          /* tp_print */
	0,                                          /* tp_getattr */
	0,                                          /* tp_setattr */
	0,                                          /* tp_compare */
	0,                                          /* tp_repr */
	0,                                          /* tp_as_number */
	0,                                          /* tp_as_sequence */
	0,                                          /* tp_as_mapping */
	0,                                          /* tp_hash */
	0,                                          /* tp_call */
	0,                                          /* tp_str */
	PyObject_GenericGetAttr,                    /* tp_getattro */
	0,                                          /* tp_setattro */
	0,                                          /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,    /* tp_flags */
	0,                                          /* tp_doc */
	(traverseproc)judy_io_map_iter_traverse,    /* tp_traverse */
	0,                                          /* tp_clear */
	0,                                          /* tp_richcompare */
	0,                                          /* tp_weaklistoffset */
	PyObject_SelfIter,                          /* tp_iter */
	(iternextfunc)judy_io_map_iter_iternextkey, /* tp_iternext */
	0,                                          /* tp_methods */
	0,
};

static PyObject* judy_io_map_iter_iternextvalue(pyjudy_io_map_iter_object* mi)
{
	JError_t JError;
	PWord_t v = 0;

	if (mi->is_first) {
		if (mi->is_from && mi->from > 0) {
			mi->i = mi->from - 1;
			v = (Pvoid_t)JudyLNext(mi->map->judy_L, &mi->i, &JError);
		} else {
			v = (Pvoid_t)JudyLFirst(mi->map->judy_L, &mi->i, &JError);
		}
	} else {
		v = (Pvoid_t)JudyLNext(mi->map->judy_L, &mi->i, &JError);
	}

	mi->is_first = 0;

	if (v == 0)
		return 0;

	if (mi->is_to && mi->i > mi->to)
		return 0;

	Py_INCREF((PyObject*)(*v));
	return (PyObject*)(*v);
}


PyTypeObject PyJudyIntObjectMapIterValue_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	"judyintmap-valueiterator",                   /* tp_name */
	sizeof(pyjudy_io_map_iter_object),            /* tp_basicsize */
	0,                                            /* tp_itemsize */
	(destructor)judy_io_map_iter_dealloc,         /* tp_dealloc */
	0,                                            /* tp_print */
	0,                                            /* tp_getattr */
	0,                                            /* tp_setattr */
	0,                                            /* tp_compare */
	0,                                            /* tp_repr */
	0,                                            /* tp_as_number */
	0,                                            /* tp_as_sequence */
	0,                                            /* tp_as_mapping */
	0,                                            /* tp_hash */
	0,                                            /* tp_call */
	0,                                            /* tp_str */
	PyObject_GenericGetAttr,                      /* tp_getattro */
	0,                                            /* tp_setattro */
	0,                                            /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,      /* tp_flags */
	0,                                            /* tp_doc */
	(traverseproc)judy_io_map_iter_traverse,      /* tp_traverse */
	0,                                            /* tp_clear */
	0,                                            /* tp_richcompare */
	0,                                            /* tp_weaklistoffset */
	PyObject_SelfIter,                            /* tp_iter */
	(iternextfunc)judy_io_map_iter_iternextvalue, /* tp_iternext */
	0,                                            /* tp_methods */
	0,
};

static PyObject *judy_io_map_iter_iternextitem(pyjudy_io_map_iter_object* mi)
{
	JError_t JError;
	PWord_t v = 0;

	if (mi->is_first) {
		if (mi->is_from && mi->from > 0) {
			mi->i = mi->from - 1;
			v = (Pvoid_t)JudyLNext(mi->map->judy_L, &mi->i, &JError);
		} else {
			v = (Pvoid_t)JudyLFirst(mi->map->judy_L, &mi->i, &JError);
		}
	} else {
		v = (Pvoid_t)JudyLNext(mi->map->judy_L, &mi->i, &JError);
	}

	mi->is_first = 0;

	if (v == 0)
		return 0;

	if (mi->is_to && mi->i > mi->to)
		return 0;

	PyObject* k = PyLong_FromUnsignedLongLong((unsigned PY_LONG_LONG)mi->i);

	if (k == 0)
		return 0;

	PyObject* result = PyTuple_New(2);

	if (result == 0) {
		Py_DECREF(k);
		return 0;
	}

	Py_INCREF((PyObject*)(*v));

	PyTuple_SET_ITEM(result, 0, k);
	PyTuple_SET_ITEM(result, 1, (PyObject*)(*v));

	return result;
}

PyTypeObject PyJudyIntObjectMapIterItem_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	"judyintmap-itemiterator",                    /* tp_name */
	sizeof(pyjudy_io_map_iter_object),            /* tp_basicsize */
	0,                                            /* tp_itemsize */
	(destructor)judy_io_map_iter_dealloc,         /* tp_dealloc */
	0,                                            /* tp_print */
	0,                                            /* tp_getattr */
	0,                                            /* tp_setattr */
	0,                                            /* tp_compare */
	0,                                            /* tp_repr */
	0,                                            /* tp_as_number */
	0,                                            /* tp_as_sequence */
	0,                                            /* tp_as_mapping */
	0,                                            /* tp_hash */
	0,                                            /* tp_call */
	0,                                            /* tp_str */
	PyObject_GenericGetAttr,                      /* tp_getattro */
	0,                                            /* tp_setattro */
	0,                                            /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,      /* tp_flags */
	0,                                            /* tp_doc */
	(traverseproc)judy_io_map_iter_traverse,      /* tp_traverse */
	0,                                            /* tp_clear */
	0,                                            /* tp_richcompare */
	0,                                            /* tp_weaklistoffset */
	PyObject_SelfIter,                            /* tp_iter */
	(iternextfunc)judy_io_map_iter_iternextitem,  /* tp_iternext */
	0,                                            /* tp_methods */
	0,
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
