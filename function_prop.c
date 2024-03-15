
#include "function_prop.h"

#define DEFAULTQUANTA 50
func_prop * new_function(void(*func_pointer)(void*)) {
    func_prop * new_func_prop = (func_prop *) malloc(sizeof(func_prop));
    new_func_prop->func_pointer = func_pointer;
    for (int i = 0 ; i < PREVIOUS_RUNTIMES_LIST_SIZE; i++){
        new_func_prop->previous_runtimes[i] = 0; // initializing previous _ritimes
    }
    return new_func_prop;
}


func_prop * new_function_time(void(*func_pointer)(void*),unsigned long t) {
    func_prop * new_func_prop = (func_prop *) malloc(sizeof(func_prop));
    new_func_prop->func_pointer = func_pointer;
    for (int i = 0 ; i < PREVIOUS_RUNTIMES_LIST_SIZE; i++){
        new_func_prop->previous_runtimes[i] = t; // initializing previous _ritimes
    }
    return new_func_prop;
}
unsigned long fpGetAverageTime(func_prop * fp){
    unsigned long count = 0;
    unsigned long totalTime = 0;
    for (int i = 0 ; i < PREVIOUS_RUNTIMES_LIST_SIZE; i++){
        if (fp->previous_runtimes[i] != 0){
            count++;
            totalTime += fp->previous_runtimes[i];
        }
    }
    if (count == 0){
        if (total_run_time == 0) return DEFAULTQUANTA; // 50 unit defat quant
        // printf("\navg time: %lu\n",total_run_time/(unsigned long)(global_tid_counter-1));
        // printf("out:%lu\n",out);

        return total_run_time/(unsigned long)(finished_threads); // not including the most recent process
    }
    return (unsigned long) (totalTime/count);
}

void fpAddTime(func_prop * fp, unsigned long rt){
    for (int i = 0 ; i < PREVIOUS_RUNTIMES_LIST_SIZE-1;i++){
        fp->previous_runtimes[i+1] = fp->previous_runtimes[i]; 
    }
    fp->previous_runtimes[0] = rt;
    total_run_time+= fp->previous_runtimes[0];
}

void fpAddTimePredictive(func_prop * fp, unsigned long rt){};