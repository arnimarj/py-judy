#include "judy.h"

static PyMethodDef judy_module_methods[] =
{
	{NULL, NULL},
};


struct judy_module_state {
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct judy_module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct judy_module_state _state;
#endif

#if PY_MAJOR_VERSION >= 3

static int judy_module_traverse(PyObject* m, visitproc visit, void* arg)
{
	return 0;
}

static int judy_module_clear(PyObject* m)
{
	return 0;
}

static struct PyModuleDef moduledef = {
	PyModuleDef_HEAD_INIT,
	"judy",
	NULL,
	sizeof(struct judy_module_state),
	judy_module_methods,
	NULL,
	judy_module_traverse,
	judy_module_clear,
	NULL
};
#endif

extern PyTypeObject PyJudyIntObjectMapType;
extern PyTypeObject PyJudyIntSetType;

#if PY_MAJOR_VERSION >= 3
#define MODULEINITERROR return NULL
PyMODINIT_FUNC
#else
#define MODULEINITERROR return
void
#endif

#if PY_MAJOR_VERSION >= 3
PyInit_judy(void)
#else
initjudy(void)
#endif

{
	PyObject* module_judy = 0;

#if PY_MAJOR_VERSION >= 3
	module_judy = PyModule_Create(&moduledef);
#else
	module_judy = Py_InitModule4("judy", judy_module_methods, "Judy Python API", 0, PYTHON_API_VERSION);
#endif

	if (module_judy == 0) {
		MODULEINITERROR;
	}

	struct {
		PyTypeObject* type;
		const char* name;
	} types[13] = {
		{&PyJudyIntObjectMapType, "JudyIntObjectMap"},
		{&PyJudyIntSetType,       "JudyIntSet"      },
	};

	int i;

	for (i = 0; i < 2; i++) {
		if (PyType_Ready(types[i].type) < 0) {
			Py_DECREF(module_judy);
			MODULEINITERROR;
		}

		Py_INCREF((PyObject*)types[i].type);

		if (PyModule_AddObject(module_judy, types[i].name, (PyObject*)types[i].type) != 0) {
			Py_DECREF(module_judy);
			MODULEINITERROR;
		}
	}

#if PY_MAJOR_VERSION >= 3
	return module_judy;
#endif
}

#undef MODULEINITERROR
