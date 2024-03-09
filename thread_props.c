#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "thread_props.h"


int global_tid_counter = 0;

thread_prop * new_thread(ucontext_t * context, int priority) {
    thread_prop * new_thread_props = (thread_prop *)  malloc(sizeof(thread_prop));
    new_thread_props->context = context;
    new_thread_props->tid = global_tid_counter++;
    new_thread_props->priority = priority;
    new_thread_props->status = SCHEDULED;
    new_thread_props->join_tids = newList();
    new_thread_props->fp = NULL;
    new_thread_props->current_run_time = 0 ;

    return new_thread_props;
}

thread_prop * new_thread_SJF(ucontext_t * context, func_prop * func_prop) {
    thread_prop * new_thread_props = (thread_prop *)  malloc(sizeof(thread_prop));
    new_thread_props->context = context;
    new_thread_props->tid = global_tid_counter++;
    new_thread_props->priority = NO_PRIORITY;
    new_thread_props->fp = func_prop;
    new_thread_props->current_run_time = 0 ;
    new_thread_props->status = SCHEDULED;
    new_thread_props->join_tids = newList();


    return new_thread_props;
}

// void toString(thread_prop * input, char * output, int output_size) {
//     char in_foreground_str[BOOLSTRING_SIZE];
//     char is_suspended_str[BOOLSTRING_SIZE];
//     if (input->in_foreground) {
//         strcpy(in_foreground_str, "TRUE");
//     }
//     else {
//         strcpy(in_foreground_str, "FALSE");
//     }
//     if (input->is_suspended) {
//         strcpy(is_suspended_str, "TRUE");
//     }
//     else {
//         strcpy(is_suspended_str, "FALSE");
//     }
//     // -1 to leave space for null character
//     snprintf(output, output_size-1, "Job ID: %d, Process ID: %d, Command: %s, In Foreground: %s, Suspended: %s", input->job_id, input->pid, input->starting_command, in_foreground_str, is_suspended_str);
// }