#include "../userthread.h"
#include <time.h>
void nanosleepwrap(struct timespec one, struct timespec two, int seconds){
        one.tv_sec = seconds;
        one.tv_nsec= 0; 
        int count = 0;
        int er = nanosleep(&one, &two);
        while(er == -1){
        count++;
        if (count % 2  == 1) er = nanosleep(&two,&one);
        else  er = nanosleep(&one, &two);

    }
    return ;
}

int f1(){   
    struct timespec sleepy;
    struct timespec rem;
    nanosleepwrap(sleepy,rem,1);
    printf("f1 finsihed\n");

}
int f2(){
    struct timespec one;
    struct timespec two;
    nanosleepwrap(one,two,1);
    printf("f2 finsihed\n");
}
int f3(){
    sleep(3);
}
int f4(){
    
}

int main(){
        printf("first Sleep\n");
    sleep(2);
    thread_libinit(FCFS);
    // tid_t tidList[2];
    // thread_create(f2,NULL,NULL); // 1
    thread_create(f2,NULL,LOW_PRIORITY); // 2
    // thread_create(f1,NULL,HIGH_PRIORITY);// 3
    // thread_create(f1,NULL,MID_PRIORITY);// 3

    // // thread_join(2);
    thread_join(1);
    // thread_join(1);
    // thread_create(f3,NULL,NULL); // 4
    // thread_create(f2,NULL,NULL); // 5 
    // thread_create(f1,NULL,NULL); // 6
    // thread_create(f4,NULL,NULL);
    // print();
    // thread_join(4);
    // thread_join(5);
    // thread_join(6);
    // thread_join(7);
    // thread_join(1);
    thread_join(2);
    thread_join(3);
    printf("second sleep\n");
 

    int count = 0;
    int er;
    // er = nanosleep(&sleepy, &rem);
    // while(er == -1){
    //     count++;
    //     if (count % 2  == 1) er = nanosleep(&rem,&sleepy);
    //     else  er = nanosleep(&sleepy, &rem);

    // }
     
    // thread_yield();
    printf("hello Wor;d"); 
    thread_libterminate();
    return 0;
}