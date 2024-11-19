# Kai Britt
### Collaborated with Ben Jiang

### Descripton
Simulating a task scheduler using user level threads

### How To Complie: 
#### make


### How To Run:
##### Link the userthread to a program when compiling using -L -luserthread

### Known Bugs and Limitations: 
- small note wasn't sure how to eliminate errors when trying to pass an argument to a function with makecontext the function was type void(*)(void *) but it expected void(*)(void). I ended up looking at stack overflow and came across this https://stackoverflow.com/questions/60591497/how-can-i-mask-the-warning-from-makecontext-if-the-function-passed-in-has-parame
- No known bugs


### Summary of Features:
- FCFS protocol for threads
- SJF protocol for threads
- Premtive SJF protocol for threads
- priority protocol for threads
- UPriority protocol for threads
- Also made it so the same thread can be joined by multiple threads
### Extra Credit:

- Implemented premtive SJF called PSJF, have one test case to demostrate it, called PSJF Middle inseriton, on the second run the shortest job is queued first but is interrupted before it can completely finish, look at userthread_log.txt
- Implemented UNIX like priority queue test case UPriorityQueue demonstrates this all three process move down the priority ladder as they fail to complete in the given quanta. A know limitation hear is that yield always lowers your priority.
- Implemented different quanta's for UPriority using 100, 200, 300 milliseconds. Look at UPriorityQueue test log to see the different run times for each priority group. noticed when there are different quanta, there are some cases when a mid priority





