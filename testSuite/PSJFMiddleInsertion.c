//Testing the averaging of SJF, we expect function 3 to be inserted inbetween the threads calling function 1 and 2, and it's average i > 1 but < 2

#include "../userthread.h"
#include <time.h>
int count = 0;
int expected[] = {2,1,3};
int arr[3];


void nanosleepwrap(struct timespec one, struct timespec two, int seconds){ // finish nano sleep kind of accurately.
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

void f1(){
    if (count < 2) count++;
    else arr[count++-2] = 1;
    printf("hello world\n");
}
void f2(){
    struct timespec sleepy;
    struct timespec rem;
    if (count < 2) count++;
    else arr[count++-2] = 2;
    nanosleepwrap(sleepy,rem,1);
}
 void f3(){
    struct timespec sleepy;
    struct timespec rem;
    if (count < 2) count++;
    else arr[count++-2] = 3;
    nanosleepwrap(sleepy,rem,2);
}

int main(){
    thread_libinit(PSJF);
    tid_t one = thread_create(f2,NULL,NO_PRIORITY); // 1
    tid_t two = thread_create(f3,NULL,NO_PRIORITY); // 2
    thread_join(one);
    thread_join(two);
    tid_t three = thread_create(f3,NULL,NO_PRIORITY); // 3
    tid_t four = thread_create(f2,NULL,NO_PRIORITY); // 4
    tid_t five = thread_create(f1,NULL,NO_PRIORITY); // 5

    //print(SJFqueue) // check order of queue to make sure it's 4, 5, 3 in thread log if this joined in the middle
    thread_join(four);
    thread_join(five);
    thread_join(three);
    thread_libterminate();
    for (int i = 2; i < 2+3; i++){
    if (arr[i-2] != expected[i-2]){
        printf("Unexpected order\n");
        return EXIT_FAILURE;
        }
    }
    printf("SUCCESS\n");
    return EXIT_SUCCESS;
    }