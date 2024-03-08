#ifndef LIST_EXTRAS_H
#define LIST_EXTRAS_H
#include "List.h"
#include "common.h"
#include "thread_props.h"
#include "function_prop.h"

// int delete_process(List *l, struct thread_prop * process);

struct thread_prop * get_by_tid(const List *l, tid_t pid);

struct func_prop * get_by_func(const List *l, void(*func)(void*));

void addSJF(List *l, struct thread_prop * tp_insert );
// void print_processes(List *l);

#endif
