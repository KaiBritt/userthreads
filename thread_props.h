#ifndef THREAD_PROPS_H
#define THREAD_PROPS_H
#include "common.h"
#include "function_prop.h"
#include <unistd.h>
#include <ucontext.h>
struct thread_prop {
    tid_t tid;  
    ucontext_t * context;
    int priority;
    int status;
    List * join_tids; 
    struct func_prop * fp;  
    unsigned long current_run_time;
};

typedef struct thread_prop thread_prop;

thread_prop * new_thread(ucontext_t * context, int priority);
thread_prop * new_thread_SJF(ucontext_t * context, struct func_prop * func_prop);


#endif