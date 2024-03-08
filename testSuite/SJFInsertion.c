// TEST SJF inserition, making sure SJF insertion is correct after collect time for each function f1,f2,f3
#include "userthread.h"
int f1(){
    printf("hello\n");
}
int f2(){
    sleep(2);
}
int f3(){
    sleep(3);
}

int main(){
    thread_libinit(SJF);
    tid_t one = thread_create(f2,NULL,NULL); // 1
    tid_t two thread_create(f3,NULL,NULL); // 2
    tid_t three thread_create(f1,NULL,NULL);// 3
    thread_join(one);
    thread_join(two);
    thread_join(three);
    tid_t four = thread_create(f3,NULL,NULL); // 4
    tid_t five = thread_create(f2,NULL,NULL); // 5 
    tid_t six = thread_create(f1,NULL,NULL); // 6

    //print(SJFqueue) // check order of queue to make sure it's 6, 5, 4 in thread log
    thread_join(four);
    thread_join(five);
    thread_join(six);

     
     
    // thread_yield();
    printf("hello Wor;d");
    thread_libterminate();
    }