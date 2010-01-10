#ifndef __JUDY__MODULE__H__
#define __JUDY__MODULE__H__

#include <Python.h>
#include <Judy.h>

#include "structmember.h"

#include <stdio.h>
#include <limits.h>

typedef struct {
	PyObject_HEAD
	Pvoid_t judy_L;
} PyJudyIntObjectMap;

typedef struct {
	PyObject_HEAD
} PyJudyIntObjectMapIter;

typedef struct {
	PyObject_HEAD
} PyJudyIntSet;

typedef struct {
	PyObject_HEAD
} PyJudyIntSetIter;

extern PyTypeObject PyJudyIntObjectMapType;
extern PyTypeObject PyJudyIntObjectMapIterType;
extern PyTypeObject PyJudyIntSetType;
extern PyTypeObject PyJudyIntSetIterType;

#define PyJudyIntObjectMap_Check(op) PyObject_TypeCheck(op, &PyJudyIntObjectMapType)
#define PyJudyIntObjectIterMap_Check(op) PyObject_TypeCheck(op, &PyJudyIntObjectMapIterType)

#define PyJudyIntSet_Check(op) PyObject_TypeCheck(op, &PyJudyIntSetType)
#define PyJudyIntSetIter_Check(op) PyObject_TypeCheck(op, &PyJudyIntSetIterType)

#endif
