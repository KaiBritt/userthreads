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
	rm -rf libuserthread.so
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
	gcc -g -o test testSuite/DoubleInit.c -L. -luserthread
	echo DoubleInit
	valgrind ./test
	gcc -g -o test testSuite/earlyTerminate.c -L. -luserthread
	echo "Early Terminate"
	valgrind ./test
	echo FCFS insertion
	gcc -g -o test testSuite/FCFS_insertion.c -L. -luserthread
	valgrind ./test
	echo Join lower priority group
	gcc -g -o test testSuite/JoinLowerPriorityGroup.c -L. -luserthread
	valgrind ./test
	echo JoinThreadOutsideOfReadyQueue
	gcc -g -o test testSuite/JoinThreadOutsideOfReadyQueue.c -L. -luserthread
	valgrind ./test
	echo multiple_joins_on_same_thread
	gcc -g -o test testSuite/multiple_joins_on_same_thread.c -L. -luserthread
	valgrind ./test
	echo SJFInsertion
	gcc -g -o test testSuite/SJFInsertion.c -L. -luserthread
	valgrind ./test
	gcc -g -o test testSuite/PriorityTest.c -L. -luserthread
	echo priority test
	valgrind ./test
	echo SJFMiddleInsertion
	gcc -g -o test testSuite/SJFMiddleInsertion.c -L. -luserthread
	valgrind ./test
	echo SJFyield
	gcc -g -o test testSuite/SJFyield.c -L. -luserthread
	valgrind ./test







