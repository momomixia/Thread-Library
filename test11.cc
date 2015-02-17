/* Test Yields are working correctly
EXPECTED OUTPUT

master thread started
master thread yielding
child thread 1 yielding
child thread 2 yielding
child thread 3 yielding
master thread resumed
child thread 1 resumed
child thread 2 resumed
child thread 3 resumed
Thread library exiting.

GENERATED OUTPUT

master thread started
master thread yielding
child thread 1 yielding
child thread 2 yielding
child thread 3 yielding
master thread resumed.
child thread 1 resumed.
child thread 2 resumed.
child thread 3 resumed.
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



int main(int argc, char *argv[]){
	dthreads_init((dthreads_func_t) master_thread, (void *) 100);
	
}
void master_thread(void * args){
	cout << "master thread started\n";
	dthreads_start((dthreads_func_t) child_thread1, (void *) 100);
	dthreads_start((dthreads_func_t) child_thread2, (void *) 100);
	dthreads_start((dthreads_func_t) child_thread3, (void *) 100);
	cout << "master thread yielding\n";
	dthreads_yield();
	cout << "master thread resumed.\n";
}
void child_thread1(void * args){
	cout << "child thread 1 yielding\n";
	dthreads_yield();
	cout << "child thread 1 resumed.\n";
}
void child_thread2(void * args){
	cout << "child thread 2 yielding\n";
	dthreads_yield();
	cout << "child thread 2 resumed.\n";
}
void child_thread3(void * args){
	cout << "child thread 3 yielding\n";
	dthreads_yield();
	cout << "child thread 3 resumed.\n";
}