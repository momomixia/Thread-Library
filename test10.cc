/* Test FIFO Ordering of Semaphores

EXPECTED OUTPUT

master thread started
master thread yielding
master thread releasing all semaphores
main master thread ended
child thread 3 released	
child thread 2 released
child thread 1 released
child thread 1.5 released
Thread library exiting.

GENERATED OUTPUT

master thread started
master thread yielding
master thread releasing all semaphores
main master thread ended.
child thread 3 released.
child thread 2 released.
child thread 1 released.
child thread 1.5 released
Thread library exiting.


*/


#include "1t.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
void master_thread(void* args);
void child_thread1(void* args);
void child_thread2(void* args);
void child_thread3(void* args);
void child_thread1point5(void* args);


int main(int argc, char *argv[]){
	dthreads_init((dthreads_func_t) master_thread, (void *) 100);
	
}
void master_thread(void * args){
	cout << "master thread started\n";
	dthreads_seminit(1,0);
	dthreads_seminit(2,0);
	dthreads_seminit(3,0);

	dthreads_start((dthreads_func_t) child_thread1, (void *) 100);
	dthreads_start((dthreads_func_t) child_thread1point5, (void *) 100);
	dthreads_start((dthreads_func_t) child_thread2, (void *) 100);
	dthreads_start((dthreads_func_t) child_thread3, (void *) 100);
	cout << "master thread yielding\n";
	dthreads_yield();
	cout << "master thread releasing all semaphores\n";
	dthreads_semup(3);
	dthreads_semup(2);
	dthreads_semup(1);

	cout << "main master thread ended.\n";
}
void child_thread1(void * args){
	dthreads_semdown(1);
	cout << "child thread 1 released.\n";
}
void child_thread1point5(void * args){
	dthreads_semdown(1);
	cout << "child thread 1.5 released\n";
}
void child_thread2(void * args){
	dthreads_semdown(2);
	cout << "child thread 2 released.\n";
}
void child_thread3(void * args){
	dthreads_semdown(3);
	cout << "child thread 3 released.\n";
}