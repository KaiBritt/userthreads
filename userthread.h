
#ifndef USERTHREAD_H
#define USERTHREAD_H
#include "common.h"
int thread_libinit(int policy);

int thread_libterminate(void);

int thread_create(void (*func)(void *), void *arg, int priority);

int thread_yield(void);

int thread_join(int tid);

#endif