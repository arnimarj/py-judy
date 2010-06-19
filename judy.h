#ifndef __JUDY__MODULE__H__
#define __JUDY__MODULE__H__

#include <Python.h>
#include <Judy.h>

#include "structmember.h"

#include <stdio.h>
#include <limits.h>

#include "utils.h"

// #define JUDY_WORD_MAX (sizeof(Word_t) == 4 ? 0xFFFFFFFF : 0xFFFFFFFFFFFFFFFF)

typedef struct {
	PyObject_HEAD
	Pvoid_t judy_L;
} PyJudyIntObjectMap;

typedef struct {
        PyObject_HEAD
		Pvoid_t s;
} PyJudyIntSet;

typedef struct {
		PyObject_HEAD
		int b;
		Word_t i;
		PyJudyIntSet* s;
} PyJudyIntSetIter;

extern PyTypeObject PyJudyIntObjectMapType;
extern PyTypeObject PyJudyIntSetType;
extern PyTypeObject PyJudyIntSetIterType;

#define PyJudyIntObjectMap_Check(op) PyObject_TypeCheck(op, &PyJudyIntObjectMapType)
#define PyJudyIntSet_Check(op)       PyObject_TypeCheck(op, &PyJudyIntSetType)
#define PyJudyIntSetIter_Check(op)   PyObject_TypeCheck(op, &PyJudyIntSetIterType)

#endif
