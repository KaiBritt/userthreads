#define MILISEC 100000
#define SEC_TO_MICRO 1000000
#define MAX_THREADS 512

#define TRUE 1
#define FALSE 0

#define FAILED_EXIT -1

#define DEL 0
#define YEILD 1
#define JOIN 2

#define NO_PRIORITY -1
#define FCFS 0
#define SJF 1 
#define PRIORITY 2

#define TERMINATED -1
#define ACTIVE 0
#define SCHEDULED 1
#define SUSPENDED 2

#define PREVIOUS_RUNTIMES_LIST_SIZE 3

#define HIGH_PRIORITY -1
#define MID_PRIORITY 0
#define LOW_PRIORITY 1

#define STACKSIZE 4096 * 2
#include "function_prop.h"


#include <ucontext.h> 
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <errno.h>
#include "List_Extras.h"
#include "List.h"
#include "node.h"
#include "thread_props.h"



typedef unsigned long  tid_t;

static List * FIFOqueue; 

static List * SJFqueue;
static List * funcList;

static List * highPQueue;
static List * midPQueue;
static List * lowPQueue;

extern unsigned long finished_threads;
extern int global_tid_counter;
extern unsigned long total_run_time; // using thread ID for the count.
extern unsigned long finished_processess;