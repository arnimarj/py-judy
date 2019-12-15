#include "judy.h"

static PyMethodDef judy_module_methods[] =
{
	{NULL, NULL},
};


struct judy_module_state {
};

#define GETSTATE(m) ((struct judy_module_state*)PyModule_GetState(m))

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

extern PyTypeObject PyJudyIntObjectMapType;
extern PyTypeObject PyJudyIntSetType;

#define MODULEINITERROR return NULL
PyMODINIT_FUNC

PyInit__judy(void)
{
	PyObject* module_judy = 0;

	module_judy = PyModule_Create(&moduledef);

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

	return module_judy;
}

#undef MODULEINITERROR
