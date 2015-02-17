//DEADLOCK TEST


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

	cout << "master thread ended.\n";
}
void child_thread(void * args){
	char * id = (char *) args;
	dthreads_seminit(4,2);
	dthreads_semdown(1);
	cout << "thread " << id << "acquired lock" << endl;
	if(id=="master"){
		dthreads_semdown(3);
		dthreads_semdown(4);
		dthreads_semdown(2);
	}

	if(id=="child"){
		dthreads_semup(2);
	}
	dthreads_semup(1);
	cout << "child method ended with " << id << endl;
}