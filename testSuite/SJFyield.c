#include "../userthread.h"

int count = 0;
int expected[] = {2,3,1};
int arr[3];
void f1(){
    thread_yield();
    if (count < 2) count++;
    else arr[count++-2] = 1;
    printf("hello world\n");
}
void f2(){
    if (count < 2) count++;
    else arr[count++-2] = 2;
    sleep(1);
}
 void f3(){
    if (count < 2) count++;
    else arr[count++-2] = 3;
    sleep(2);
}

int main(){
    thread_libinit(SJF);
    tid_t one = thread_create(f2,NULL,NO_PRIORITY); // 1
    tid_t two = thread_create(f3,NULL,NO_PRIORITY); // 2
    
    thread_join(one);
    thread_join(two);
    tid_t three = thread_create(f3,NULL,NO_PRIORITY); // 3
    tid_t four = thread_create(f2,NULL,NO_PRIORITY); // 4
    tid_t five = thread_create(f1,NULL,NO_PRIORITY); // 5

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