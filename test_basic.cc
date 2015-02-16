/*
*	Compile with the following command:
*	g++ -o test -m32 1t.cc test_basic.cc libinterrupt.a -ldl
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
void child2_thread(void*args);

int main(int argc, char *argv[]){
	int magic_number = 7777;
	dthreads_init((dthreads_func_t) master_thread, &magic_number);
	
}
void master_thread(void * args){
	printf("master thread started\n");
	// printf("magic number is %d\n", * (int *) args);
	dthreads_start((dthreads_func_t) child_thread, args);
	dthreads_yield();
	dthreads_start((dthreads_func_t) child2_thread, args);
	cout <<"master thread ended.\n";
}
void child_thread(void * args){
	printf("child thread started\n");
	
}


void child2_thread(void * args){
	printf("child 2 thread started\n");
	
}
