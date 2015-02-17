/*
EXPECTED OUTPUT

master thread started
child 2 saved the day!!!
method ended with master
master thread ended
method ended with child1
Thread Library exiting.


GENERATED OUTPUT

master thread started
child2 saved the day!!!
method ended with master
master thread ended.
method ended with child1
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
	dthreads_seminit(2,0);
	dthreads_seminit(4,0);
	dthreads_start((dthreads_func_t) child_thread, (void *) "child1");
	dthreads_start((dthreads_func_t) child_thread, (void *) "child2");
	child_thread( (void *) "master");

	cout << "master thread ended.\n";
}
void child_thread(void * args){
	char * id = (char *) args;

	if(id=="child1"){
		dthreads_semdown(2);
	}
	if(id=="master"){
		dthreads_semdown(2);
	}

	if(id=="child2"){
		cout << "child2 saved the day!!!" << endl;
		dthreads_semup(2);
		dthreads_semdown(4);
	}
	cout << "method ended with " << id << endl;
}