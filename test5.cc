/*
EXPECTED OUTPUT

master thread started
master thread about to wait on semaphore 2

child thread calling semup on sempahore 2, adding master thread to Q
child thread acquiring lock on sempahore 4
child thread acquiring lock on sempahore 2
child thread waiting on sempahore 4

master thread resumed
master thread calling semup on semaphore 4, adding child thread to Active Q
master thread ended
main master thread ended.

child thread resumed
child thread ended
Thread Library Exiting.


GENERATED OUTPUT

master thread started
master thread about to wait on sempahore 2
child thread calling semup on sempahore 2, adding master thread to Active Q
child thread acquiring lock on sempahore 4
child thread acquiring lock on sempahore 2
child thread waiting on sempahore 4
master thread resumed
master thread calling semup on semaphore 4, adding child thread to Active Q
master thread ended
main master thread ended.
child thread resumed
child thread ended
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
void child_thread(void* args);


int main(int argc, char *argv[]){
	dthreads_init((dthreads_func_t) master_thread, (void *) 100);
	
}
void master_thread(void * args){
	cout << "master thread started\n";
	dthreads_seminit(1,1);
	dthreads_seminit(2,0);
	dthreads_seminit(3,3);

	dthreads_start((dthreads_func_t) child_thread, (void *) "child");
	child_thread( (void *) "master");

	cout << "main master thread ended.\n";
}
void child_thread(void * args){
	char * id = (char *) args;
	if(dthreads_seminit(4,2)){
		cout << "failed to initialize semaphore 4";
		exit(1);
	}

	if(id=="master"){
		dthreads_semdown(3);
		dthreads_semdown(4);
		cout << "master thread about to wait on sempahore 2\n";
		dthreads_semdown(2);
		cout << "master thread resumed\n";
	}

	if(id=="child"){
		cout << "child thread calling semup on sempahore 2, adding master thread to Active Q\n";
		dthreads_semup(2);
		dthreads_semdown(3);
		cout << "child thread acquiring lock on sempahore 4\n";
		dthreads_semdown(4);
		cout << "child thread acquiring lock on sempahore 2\n";
		dthreads_semdown(2);
		cout << "child thread waiting on sempahore 4\n";
		dthreads_semdown(4);
		cout << "child thread resumed\n";
	}

	if(id=="master"){
		cout << "master thread calling semup on semaphore 4, adding child thread to Active Q\n";
		dthreads_semup(4);
	}
	cout << id << " thread ended\n";
}