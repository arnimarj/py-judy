#include "judy_wrapper.h"

int judy_w_destroy(PPvoid_t judy)
{
	JError_t error;
	Word_t bytes_freed = JudyLFreeArray(judy, &error);
	return (bytes_freed != JERR);
}

int judy_1_destroy(PPvoid_t judy)
{
	JError_t error;
	Word_t bytes_freed = Judy1FreeArray(judy, &error);
	return (bytes_freed != JERR);
}

int judy_w_insert(PPvoid_t judy, Word_t index, Word_t value)
{
	JError_t error;
	PWord_t p_value = (PWord_t)JudyLIns(judy, index, &error);

	if (p_value == PJERR)
		return 0;

	*p_value = value;
	return 1;
}

#include <assert.h>
#include <stdio.h>

int judy_1_set(PPvoid_t judy, Word_t index)
{
}

int judy_w_contains(Pvoid_t judy, Word_t index)
{
	JError_t error;
	PWord_t value = (PWord_t)JudyLGet(judy, index, &error);

	//! fix
	assert(value != PJERR);

	if (value == 0)
		return 0;

	printf("value: %u\n", (unsigned int)(*value));
	return 1;
}

int judy_1_contains()
{
}

int judy_w_remove()
{
}

int judy_1_remove()
{
}

//JLI(PValue, PLArray, Index);
//if (PValue == PJERR) goto out_of_memory_handling;

//init
//insert
//delete
//iterate
//range iterate
//walk
//destroy

// PPvoid_t JudyLIns(       PPvoid_t PPJLArray, Word_t    Index, PJError_t PJError);
// int      JudyLDel(       PPvoid_t PPJLArray, Word_t    Index, PJError_t PJError);
// PPvoid_t JudyLGet(       Pcvoid_t  PJLArray, Word_t    Index, PJError_t PJError);
// Word_t   JudyLCount(     Pcvoid_t  PJLArray, Word_t    Index1, Word_t    Index2, PJError_t PJError);
// PPvoid_t JudyLByCount(   Pcvoid_t  PJLArray, Word_t    Nth,  Word_t * PIndex,  PJError_t PJError);
// Word_t   JudyLFreeArray( PPvoid_t PPJLArray, PJError_t PJError);
// Word_t   JudyLMemUsed(   Pcvoid_t  PJLArray);
// PPvoid_t JudyLFirst(     Pcvoid_t  PJLArray, Word_t * PIndex, PJError_t PJError);
// PPvoid_t JudyLNext(      Pcvoid_t  PJLArray, Word_t * PIndex, PJError_t PJError);
// PPvoid_t JudyLLast(      Pcvoid_t  PJLArray, Word_t * PIndex, PJError_t PJError);
// PPvoid_t JudyLPrev(      Pcvoid_t  PJLArray, Word_t * PIndex, PJError_t PJError);
// int      JudyLFirstEmpty(Pcvoid_t  PJLArray, Word_t * PIndex, PJError_t PJError);
// int      JudyLNextEmpty( Pcvoid_t  PJLArray, Word_t * PIndex, PJError_t PJError);
// int      JudyLLastEmpty( Pcvoid_t  PJLArray, Word_t * PIndex, PJError_t PJError);
// int      JudyLPrevEmpty( Pcvoid_t  PJLArray, Word_t * PIndex, PJError_t PJError);

#include <stdio.h>

int main()
{
	Pvoid_t judy = 0;

	if (!judy_w_insert(&judy, 1, 100)) {
		printf("error A\n");
		return 0;
	}

	if (!judy_w_contains(judy, 1)) {
		printf("error B\n");
		return 0;
	}

}
