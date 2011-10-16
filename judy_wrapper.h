#ifndef __JUDY__WRAPPER_H__
#define __JUDY__WRAPPER_H__

#include <assert.h>

#include <Judy.h>

typedef void (*judy_w_walk_cb)(Word_t i, Word_t v, void* user);
typedef void (*judy_1_walk_cb)(Word_t i,           void* user);

int judy_w_destroy(PPvoid_t judy);
int judy_w_insert(PPvoid_t judy, Word_t index, Word_t value);
int judy_w_remove(PPvoid_t judy, Word_t index);
int judy_w_replace(PPvoid_t judy, Word_t index, Word_t* old_value, Word_t value);
int judy_w_contains(Pvoid_t judy, Word_t index);

void judy_w_walk_range(Pvoid_t judy, judy_w_walk_cb cb, Word_t a, Word_t b, void* user);
void judy_w_walk(Pvoid_t judy, judy_w_walk_cb cb, void* user);

int judy_1_destroy(PPvoid_t judy);
int judy_1_set(PPvoid_t judy, Word_t index);
int judy_1_remove(PPvoid_t judy, Word_t index);
int judy_1_contains(Pvoid_t judy, Word_t index);

void judy_1_walk_range(Pvoid_t judy, judy_1_walk_cb cb, Word_t a, Word_t b, void* user);
void judy_1_walk(Pvoid_t judy, judy_1_walk_cb cb, void* user);

#endif
