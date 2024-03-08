//Trying to initalize userthread lib with two priorities.
#include "userthread.h"


int main(){
    int err;
    err = thread_libinit(SJF);
    if (err != 0) return -1;
    err = thread_libinit(FCFS);
    if (err != -1 ) return -1;
    return 0;
    }