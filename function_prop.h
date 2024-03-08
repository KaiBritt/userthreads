
#ifndef FUNCTION_PROPS_H
#define FUNCTION_PROPS_H
#include "common.h"

struct func_prop {
    void(*func_pointer)(void *);
    unsigned long previous_runtimes[PREVIOUS_RUNTIMES_LIST_SIZE];
};

typedef struct func_prop func_prop;
func_prop * new_function(void(*func_pointer)(void*));
func_prop * new_function_time(void(*func_pointer)(void*), unsigned long t);

unsigned long fpGetAverageTime(func_prop * fp);

void fpAddTime(func_prop * fp, unsigned long rt);

#endif