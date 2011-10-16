#include "judy_wrapper.h"

#define JUDY_WORD_MAX (~(Word_t)0)

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

int judy_1_set(PPvoid_t judy, Word_t index)
{
	JError_t error;
	int i = Judy1Set(judy, index, &error);
	return (i != JERR);
}

int judy_w_remove(PPvoid_t judy, Word_t index)
{
	JError_t error;
	int i = JudyLDel(judy, index, &error);
	return (i != JERR);
}

int judy_1_remove(PPvoid_t judy, Word_t index)
{
	JError_t error;
	int i = Judy1Unset(judy, index, &error);
	return (i != JERR);
}

int judy_w_replace(PPvoid_t judy, Word_t index, Word_t* old_value, Word_t value)
{
	// we ignore error conditions, only occur on corrupt data
	JError_t error;
	PWord_t p_value = (PWord_t)JudyLGet(judy, index, &error);

	if (p_value == 0)
		return 0;

	if (old_value)
		*old_value = *p_value;

	*p_value = value;
	return 1;
}

int judy_w_contains(Pvoid_t judy, Word_t index)
{
	// we ignore error conditions, only occur on corrupt data
	JError_t error;
	PWord_t value = (PWord_t)JudyLGet(judy, index, &error);
	return (value != 0);
}

int judy_1_contains(Pvoid_t judy, Word_t index)
{
	JError_t error;
	int i = Judy1Test(judy, index, &error);
	assert(i == 0 || i == 1);
	return i;
}

void judy_w_walk_range(Pvoid_t judy, judy_w_walk_cb cb, Word_t a, Word_t b, void* user)
{
	JError_t error;
	Word_t index;
	PWord_t value;

	if (a == 0)
		value = (PWord_t)JudyLFirst(judy, &index, &error);
	else {
		index = a - 1;
		value = (PWord_t)JudyLNext(judy, &index, &error);
	}

	while (value) {
		cb(index, *value, user);
		value = (PWord_t)JudyLNext(judy, &index, &error);

		if (b != JUDY_WORD_MAX && b > index)
			break;
	}
}

void judy_w_walk(Pvoid_t judy, judy_w_walk_cb cb, void* user)
{
	judy_w_walk_range(judy, cb, 0, JUDY_WORD_MAX, user);
}

void judy_1_walk_range(Pvoid_t judy, judy_1_walk_cb cb, Word_t a, Word_t b, void* user)
{
	JError_t error;
	Word_t index;
	int i;

	if (a == 0) {
		i = Judy1First(judy, &index, &error);
		assert(i == 0 || i == 1);
	} else {
		index = a - 1;
		i = Judy1Next(judy, &index, &error);
		assert(i == 0 || i == 1);
	}

	while (i) {
		cb(index, user);

		i = Judy1Next(judy, &index, &error);
		assert(i == 0 || i == 1);

		if (b != JUDY_WORD_MAX && b > index)
			break;
	}
}

void judy_1_walk(Pvoid_t judy, judy_1_walk_cb cb, void* user)
{
	judy_1_walk_range(judy, cb, 0, JUDY_WORD_MAX, user);
}
