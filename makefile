userthread: userthread.c
	make clean
	make node.o
	make List.o
	make List_Extras.o
	make function_prop.o
	make thread_props.o
	make userthread.o
	gcc -shared -g -o libuserthread.so node.o List.o List_Extras.o function_prop.o userthread.o thread_props.o -ggdb

clean: 
	rm -rf *.o 
	rm -rf *.gch
	rm -rf userthread
node.o: node.h
	gcc -Wall -fpic -g -c node.c 
List.o: node.h List.h
	gcc -Wall -fpic -g  -c List.c 
List_Extras.o: node.h List_Extras.h
	gcc -Wall -fpic -g -c List_Extras.c 
thread_props.o: common.h thread_props.h
	gcc -Wall -fpic -g -c thread_props.c 
function_prop.o: common.h
	gcc -Wall -fpic -g -c function_prop.c 
userthread.o: common.h	
	gcc -Wall -fpic -g -c userthread.c 

alltests: 
	gcc -o test ./testSuite/FCFS_insertion.c
