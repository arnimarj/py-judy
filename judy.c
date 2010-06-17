#include "judy.h"

static PyMethodDef judy_methods[] =
{
	{NULL, NULL},
};

#define TRY_MODULE_INIT(module, description) PyObject* module##_module = 0; if ((module##_module = Py_InitModule4(#module, module##_methods, description, 0, PYTHON_API_VERSION)) == 0) { return; }
#define TRY_MODULE_ADD_OBJECT(module, key, value) if (PyModule_AddObject(module, key, value) != 0) return;

extern PyTypeObject PyJudyIntObjectMapType;
// extern PyTypeObject PyJudyIntObjectMapIterType;
// extern PyTypeObject PyJudyIntSetType;
// extern PyTypeObject PyJudyIntSetIterType;

PyMODINIT_FUNC
initjudy(void)
{
	TRY_MODULE_INIT(judy, "Judy Python API");

	if (PyType_Ready(&PyJudyIntObjectMapType) < 0)
		return;

	if (PyType_Ready(&PyJudyIntSetType) < 0)
			return;

	if (PyType_Ready(&PyJudyIntSetIterType) < 0)
			return;

	Py_INCREF(&PyJudyIntObjectMapType);
	Py_INCREF(&PyJudyIntSetType);
	Py_INCREF(&PyJudyIntSetIterType);

	TRY_MODULE_ADD_OBJECT(judy_module, "JudyIntObjectMap", (PyObject*)&PyJudyIntObjectMapType);
	TRY_MODULE_ADD_OBJECT(judy_module, "JudyIntSet",       (PyObject*)&PyJudyIntSetType);
	TRY_MODULE_ADD_OBJECT(judy_module, "JudyIntSetIter",   (PyObject*)&PyJudyIntSetIterType);
}
