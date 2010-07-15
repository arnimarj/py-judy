#ifndef __JUDY__MODULE__H__
#define __JUDY__MODULE__H__

#include <Python.h>
#include <Judy.h>

#include "structmember.h"

#include <stdio.h>
#include <limits.h>

#include "utils.h"

typedef struct {
	PyObject_HEAD
	Pvoid_t judy_L;
} PyJudyIntObjectMap;

typedef struct {
	PyObject_HEAD
	Pvoid_t s;
} PyJudyIntSet;

extern PyTypeObject PyJudyIntObjectMapType;
extern PyTypeObject PyJudyIntSetType;

#define PyJudyIntObjectMap_Check(op) PyObject_TypeCheck(op, &PyJudyIntObjectMapType)
#define PyJudyIntSet_Check(op)       PyObject_TypeCheck(op, &PyJudyIntSetType)

#endif
