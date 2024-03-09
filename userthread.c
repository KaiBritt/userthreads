#include "common.h"
#include "thread_props.h"
#include "time.h"
#include <valgrind/valgrind.h>
//FIFO globals
static List * FIFOqueue;
static List * AllThreads;

//SJF globals
static List * SJFqueue;
static List * funcList;
unsigned long finished_threads = 0;
unsigned long total_run_time = 0;
static struct timeval pstart,start, pstop, stop;


//PRIORITY globals
static List * highPQueue;
static List * midPQueue;
static List * lowPQueue;
static int cur_queue; 
static sigset_t procmask;
static unsigned long pRuntimes[3] = {0};
static int run_prob[] = {0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,-1,0,-1,-1,-1,-1};
static struct itimerval quantaTimer; 


//shared globals
static int isInitialized = FALSE;
static int valgrind_registers[MAX_THREADS];
static thread_prop * thread_table[MAX_THREADS];
static ucontext_t s_ctx;
static thread_prop * running_thread; 
FILE * log;
static int algo;

static List * getCurList(int cur_queue){
    if(cur_queue == HIGH_PRIORITY){
        return highPQueue;
    }
    if (cur_queue == MID_PRIORITY){
        return midPQueue;
    }
    if(cur_queue == LOW_PRIORITY){
        return lowPQueue;
    }
    return NULL;
}


static int getNextQueue(){
    int randnum = rand() % sizeof(run_prob)/sizeof(run_prob[0]);
    if(highPQueue->head == NULL && lowPQueue->head == NULL && midPQueue->head == NULL){
        return -2;
    }
    while(getCurList(run_prob[randnum])->head == NULL){
    randnum = rand() % sizeof(run_prob)/sizeof(run_prob[0]);
    }
    return run_prob[randnum];
    // struct queue_info{
    //     int queue_num;
    //     int isReal;
    // };
    // struct queue_info activeQueues[3] = {0};
    // int cur_idx = 0;
    // for (int i = -1; i < 2; i++){ //chekcing what is empty
    //     List * temp =  getCurList(i);
    //     if (temp->head != NULL){
    //         struct queue_info t;
    //         t.isReal = TRUE;
    //         t.queue_num = i;
    //         activeQueues[cur_idx++] = t;
    //     }
    // }
    // struct queue_info cur_out;
    // for (int i = 0; i < 2; i++){
    //     if (activeQueues[i].isReal == TRUE && activeQueues[i+1].isReal == TRUE){
    //         if(activeQueues[i].queue_num == HIGH_PRIORITY && activeQueues[i+1].queue_num == LOW_PRIORITY){
    //             if (pRuntimes[activeQueues[i].queue_num+1] < pRuntimes[activeQueues[i+1].queue_num+1] * 2.25){
    //                 cur_out = activeQueues[i];
    //             }
    //             else{
    //                 cur_out = activeQueues[i+1];
    //             }
    //         }
    //         else{
    //             if (pRuntimes[activeQueues[i].queue_num+1] < pRuntimes[activeQueues[i+1].queue_num+1] * 1.5){
    //                 cur_out = activeQueues[i];
    //             }
    //             else{
    //                 cur_out = activeQueues[i+1];
    //             }
    //         }
    //     }
    //     else if(activeQueues[i+1].isReal != TRUE){
    //         break;
    //     }
    // }
    // return cur_out.queue_num;
}
static void writeToLog(thread_prop * cur_thread){
    int status = cur_thread->status;
    int tid = cur_thread->tid;
    int priority = cur_thread->priority;
    gettimeofday(&pstop, NULL);
    unsigned long tic = ((pstop.tv_sec * SEC_TO_MICRO+ pstop.tv_usec)-(pstart.tv_sec * SEC_TO_MICRO + pstart.tv_usec))/MILISEC;
    if (status == TERMINATED){
        fprintf(log, "[%lu]\tSTOPPED\t%d\t%-d\n",tic,tid,priority);
    }
    if (status == ACTIVE){
        fprintf(log,"[%lu]\tSCHEDULED\t%-d\t%d\n",tic,tid,priority);
    }
    if (status == SUSPENDED){
        fprintf(log,"[%lu]\tSTOPPED\t%d\t%d\n",tic,tid,priority);
    }
    if (status == SCHEDULED){
        fprintf(log,"[%lu]\tCREATED\t%d\t%d\n",tic,tid,priority);
    }
}

static void FIFOscheduler(int flag){
    if (isInitialized == FALSE) return;
    thread_prop * cur_thread = FIFOqueue->head->data;
    if (flag == JOIN){
        cur_thread->status = SUSPENDED;
        deleteHead(FIFOqueue);       
    }
    else if(flag == YEILD){
        cur_thread->status = SUSPENDED;
        deleteHead(FIFOqueue);
        add(FIFOqueue, cur_thread);
    }   

    else{ // deleting
        cur_thread->status = TERMINATED;
        deleteHead(FIFOqueue);
        while (cur_thread->join_tids->head != NULL){
            thread_prop * thread_prop_to_add = cur_thread->join_tids->head->data;
            addHead(FIFOqueue,thread_prop_to_add);
            deleteHead(cur_thread->join_tids);
        }
    }
    writeToLog(cur_thread);
    makecontext(&s_ctx,FIFOscheduler,1, DEL); // setting next to be delete when it is called again,
    thread_prop * cur_head = FIFOqueue->head->data;
    cur_head->status = ACTIVE;
    running_thread = cur_head;

    writeToLog(cur_head);
    setcontext(cur_head->context);

}


static void SJFscheduler(int flag){
    if (isInitialized == FALSE) return -1;
    gettimeofday(&stop,0); //stoping the clock
    thread_prop * cur_thread = SJFqueue->head->data;
    if (cur_thread->tid != 0){ // only changing time when it's not main b/c I assume main is always at the front
    cur_thread->current_run_time += (stop.tv_sec * 1000000+ stop.tv_usec)-(start.tv_sec * 1000000 + start.tv_usec); //updating time, whenever scheudler is invoked the head will be moved.
    }

    if (flag == JOIN){
        cur_thread->status = SUSPENDED;
        deleteHead(SJFqueue);       
    }
    else if(flag == YEILD){
        cur_thread->status = SUSPENDED;
        deleteHead(SJFqueue);
        add(SJFqueue, cur_thread);
    }   

    else{ // deleting
        cur_thread->status = TERMINATED;
        // updating func_props
        func_prop * func = get_by_func(funcList, cur_thread->fp->func_pointer);

        fpAddTime(func,cur_thread->current_run_time);
        finished_threads++;
        deleteHead(SJFqueue);
        while (cur_thread->join_tids->head != NULL){
            thread_prop * thread_prop_to_add = cur_thread->join_tids->head->data;
            addHead(SJFqueue,thread_prop_to_add);
            deleteHead(cur_thread->join_tids);
        }
    }
    writeToLog(cur_thread);

    // print(FIFOqueue);
    makecontext(&s_ctx,SJFscheduler,1, DEL); // setting next to be delete when it is called again,
    thread_prop * cur_head = SJFqueue->head->data;
    cur_head->status = ACTIVE;
    running_thread = cur_head;
    writeToLog(cur_head);
    gettimeofday(&start,0); //starting the clock for the next process
    setcontext(cur_head->context);

}


static void priorityScheduler(int flag){
    if (isInitialized == FALSE) return;

    gettimeofday(&stop,0); //stoping the clock
    // printf("scheduler invoked, curQueue: %d\n", cur_queue);

    List * cur_list = getCurList(cur_queue);
    thread_prop * cur_thread = cur_list->head->data;
    pRuntimes[cur_queue + 1] += (stop.tv_sec * 1000000+ stop.tv_usec)-(start.tv_sec * 1000000 + start.tv_usec); //updating time, whenever scheudler is invoked the head will be moved.
    // total_run_time += cur_thread->current_run_time;

    if (flag == JOIN){
        cur_thread->status = SUSPENDED;

        deleteHead(cur_list);       
    }
    else if(flag == YEILD){
        cur_thread->status = SUSPENDED;
        deleteHead(cur_list);
        add(cur_list, cur_thread);
    }   

    else{ // deleting
        cur_thread->status = TERMINATED;
        // updating func_props
       deleteHead(cur_list);
        while (cur_thread->join_tids->head != NULL){
            thread_prop * thread_prop_to_add = cur_thread->join_tids->head->data;
            deleteHead(cur_thread->join_tids);
            cur_list = getCurList(thread_prop_to_add->priority);
            addHead(cur_list,thread_prop_to_add);
        }
    }
    writeToLog(cur_thread);
    // print(FIFOqueue);g
    cur_queue = getNextQueue();
    cur_list = getCurList(cur_queue);
    makecontext(&s_ctx,priorityScheduler,1, DEL); // setting next to be delete when it is called again,
    thread_prop * cur_head = cur_list->head->data;
    cur_head->status= ACTIVE;
    running_thread = cur_head;

    writeToLog(cur_head);
    // printf("%d\n",cur_head->tid);
    gettimeofday(&start,0); //starting the clock for the next process
    quantaTimer.it_value.tv_usec = MILISEC;
    setcontext(cur_head->context);

}




int thread_join(int tid){ // check that id is in 
    if (isInitialized == FALSE) return -1;
    // printf("hello");

    thread_prop * to_point_to_temp = thread_table[tid];
    // thread_prop * to_point_to_temp = get_by_tid(AllThreads, tid);
    if (to_point_to_temp == NULL){
        return -1; // target thread not found.
    }

    if (algo == FCFS){
        thread_prop * temp = FIFOqueue->head->data; 
        if (to_point_to_temp->status != TERMINATED){
            add(to_point_to_temp->join_tids,temp);
            makecontext(&s_ctx,FIFOscheduler,1,JOIN);
            swapcontext(temp->context, &s_ctx);
            return 0;
        }  
    }
    else if (algo == SJF){
        thread_prop * temp = SJFqueue->head->data; 
        if (to_point_to_temp->status != TERMINATED){
            add(to_point_to_temp->join_tids,temp);
            makecontext(&s_ctx,SJFscheduler,1,JOIN);
            swapcontext(temp->context,&s_ctx);
            return 0;
        }  
    }
    else if (algo == PRIORITY){
        sigprocmask(SIG_SETMASK,&procmask,NULL);
        List * cur_list = getCurList(cur_queue);
        sigprocmask(SIG_UNBLOCK,NULL, &procmask);

        thread_prop * temp = cur_list->head->data; 
        if (to_point_to_temp->status != TERMINATED){
            add(to_point_to_temp->join_tids,temp);
            
            makecontext(&s_ctx,priorityScheduler,1,JOIN);
            swapcontext(temp->context,&s_ctx);
            return 0;
        }  
    }
    else{
        return 0; // already joined just continue as normal
    }
    return 0;
}


int thread_yield(){
    if (isInitialized == FALSE) return -1;

    if (algo == FCFS){
    thread_prop * temp = FIFOqueue->head->data; 
    makecontext(&s_ctx,FIFOscheduler,1,YEILD);
    swapcontext(temp->context,&s_ctx);
    return 0;
    }
    else if (algo == SJF){
        thread_prop * temp = SJFqueue->head->data; 
        makecontext(&s_ctx,SJFscheduler,1,YEILD);
        swapcontext(temp->context,&s_ctx);
        return 0;
    }
    else if (algo == PRIORITY){
        sigprocmask(SIG_SETMASK,&procmask,NULL);
        List * cur_list = getCurList(cur_queue);
        sigprocmask(SIG_UNBLOCK,NULL, &procmask);
        thread_prop * temp = cur_list->head->data;
        makecontext(&s_ctx,priorityScheduler,1,YEILD);
 
        swapcontext(temp->context,&s_ctx);


    }

    return 0;
}
static void signalHandler(int sig){
    thread_yield();
}
    // makecontext(s_ctx,FIFOschedule,1,YIELD)

int thread_libinit(int policy){
    if ((policy != SJF && policy != FCFS && policy != PRIORITY) || isInitialized == TRUE) return -1;
    gettimeofday(&pstart,NULL);
    gettimeofday(&start, NULL);
    log = fopen("userthread_log.txt","w");

    isInitialized = TRUE;
    getcontext(&s_ctx);
    // AllThreads = newList();
    void * stack = malloc(STACKSIZE);
    s_ctx.uc_stack.ss_sp = stack;
    s_ctx.uc_stack.ss_size = STACKSIZE;
    s_ctx.uc_stack.ss_flags = 0;
    valgrind_registers[0]  = VALGRIND_STACK_REGISTER(stack,stack+STACKSIZE);
    sigemptyset(&(s_ctx.uc_sigmask));
    s_ctx.uc_link = NULL;
    // makecontext(&s_ctx,s,1,arg);

    if (policy == FCFS){
        makecontext(&s_ctx,FIFOscheduler,1, DEL);
        algo = FCFS;
        FIFOqueue = newList();
        ucontext_t * mc = (ucontext_t *) malloc(sizeof(ucontext_t));
        thread_prop * main_thread =  new_thread(mc,-1);
        running_thread = main_thread;

        add(FIFOqueue, main_thread);
        thread_table[0] =  main_thread;
        // add(AllThreads,main_thread);
        getcontext(mc);
        mc->uc_stack.ss_sp = NULL;
        return 0;
 
    } 
    else if (policy == SJF ){
        algo = SJF;
        SJFqueue = newList();
        funcList = newList();
        ucontext_t * mc = (ucontext_t *) malloc(sizeof(ucontext_t));
        func_prop * main_func_dummy = new_function_time(0,1);
        thread_prop * main_thread =  new_thread_SJF(mc,main_func_dummy);  // t
        running_thread = main_thread;
        add(SJFqueue, main_thread);
        thread_table[0] =  main_thread;
        // add(AllThreads,main_thread);
        add(funcList,main_func_dummy);
        gettimeofday(&start, NULL);
        getcontext(mc);
        mc->uc_stack.ss_sp = NULL;
        return 0;
    } 
    else if (policy == PRIORITY){
        srand(time(NULL));
        algo = PRIORITY;
        highPQueue = newList();
        midPQueue = newList();
        lowPQueue = newList();
        sigaddset(&(s_ctx.uc_sigmask), SIGALRM); //maksing schedulers sig alarm so it can't be interuppted by itimer
        sigaddset(&procmask, SIGALRM); // temporairly maksing sig allarm
        ucontext_t * mc = (ucontext_t *) malloc(sizeof(ucontext_t));
        thread_prop * main_thread =  new_thread(mc,HIGH_PRIORITY);  // for now setting main to high priority
        running_thread = main_thread;
        add(highPQueue, main_thread);
        thread_table[0] =  main_thread;
        // add(AllThreads,main_thread);
        cur_queue = HIGH_PRIORITY;
        gettimeofday(&start, NULL);



        // initializing itemer
        quantaTimer.it_interval.tv_sec = 0;
        quantaTimer.it_value.tv_sec = 0;
        quantaTimer.it_interval.tv_usec = MILISEC;
        quantaTimer.it_value.tv_usec = MILISEC;
        signal(SIGALRM, signalHandler);
        setitimer(ITIMER_REAL, &quantaTimer, NULL);

        getcontext(mc);
        mc->uc_stack.ss_sp = NULL;
        return 0;
    }
    return -1; 

}


int thread_create(void (*func)(void *), void *arg, int priority){
    if (isInitialized == FALSE) return -1;

    ucontext_t * uc = (ucontext_t *) malloc(sizeof(ucontext_t));

    getcontext(uc);
    void * stack = malloc(STACKSIZE);
    uc->uc_stack.ss_sp = stack;
    uc->uc_stack.ss_size = STACKSIZE;
    uc->uc_stack.ss_flags = 0;
    valgrind_registers[global_tid_counter]  = VALGRIND_STACK_REGISTER(stack,stack+STACKSIZE);

    sigemptyset(&(uc->uc_sigmask));
    uc->uc_link = &s_ctx;
    makecontext(uc,func,1,arg);
    if (errno == ENOMEM) {
        free(uc);
        return -1; //if make context fails it set errno to ENOMEM
    }
    thread_prop * thread_to_add; 
    if (algo == FCFS){
        thread_to_add =  new_thread(uc,priority);
        add(FIFOqueue, thread_to_add);
        thread_table[thread_to_add->tid] =  thread_to_add;

        // add(AllThreads,thread_to_add);

    } 
    else if(algo == SJF){
        
        func_prop * passed_func = get_by_func(funcList,func);
        if (passed_func == NULL){ //add function to func list if it's not already there
            // printf("not ran\n");
            passed_func = new_function(func);
            add(funcList,passed_func);
        }
        thread_to_add = new_thread_SJF(uc,passed_func);
        addSJF(SJFqueue,thread_to_add);
        thread_table[thread_to_add->tid] =  thread_to_add;

        // add(AllThreads,thread_to_add);

    }
    else if (algo == PRIORITY){
   
        sigprocmask(SIG_SETMASK,&procmask,NULL);
        if (priority != LOW_PRIORITY && priority != MID_PRIORITY && priority != HIGH_PRIORITY){ // not valid priority
        return -1;
        }
        thread_to_add = new_thread(uc,priority);
        List * cur_list = getCurList(priority);
        add(cur_list, thread_to_add);
        thread_table[thread_to_add->tid] =  thread_to_add;
        // add(AllThreads,thread_to_add);
        sigprocmask(SIG_UNBLOCK,NULL, &procmask);

    };
    writeToLog(thread_to_add);
    return thread_to_add->tid;

}


int thread_libterminate(){
    if (isInitialized == FALSE || (running_thread->tid != 0)) return -1;   // can't call terminate unless it's the main thread and the tread lib has been initialized

    free(s_ctx.uc_stack.ss_sp); // free stack
    for (int i = 0; i < global_tid_counter;i++){
        VALGRIND_STACK_DEREGISTER(valgrind_registers[i]);
    }
    fclose(log);
    isInitialized = FALSE;

    for(int i = 0; i < global_tid_counter;i++){
        void * stack = thread_table[i]->context->uc_stack.ss_sp;
        if (stack != NULL) free(stack);
        free(thread_table[i]->context);
        free(thread_table[i]->join_tids);
        free(thread_table[i]);
    }
    if (algo == FCFS){
        clearNodes(FIFOqueue);
        // clearThreadProps(AllThreads);
        // free(AllThreads);
        free(FIFOqueue);
        return EXIT_SUCCESS;
    }
    else if (algo == SJF){
        // clearThreadProps(AllThreads);
        clear(funcList);
        clearNodes(SJFqueue);
        // free(AllThreads);
        free(SJFqueue );
        free(funcList);
        return EXIT_SUCCESS;

    }
    else if(algo == PRIORITY){
        clearNodes(highPQueue);
        clearNodes(midPQueue);
        clearNodes(lowPQueue);
        // clearThreadProps(AllThreads);
    
        // clearNodes(AllThreads);
        free(highPQueue);
        free(midPQueue);
        free(lowPQueue);
        // free(AllThreads);
        return EXIT_SUCCESS;
    }

    return FAILED_EXIT;
}


