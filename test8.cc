/*
*	Test what happens when yield is called when it is this only thread.  Yield
*	should be yielding to CPU and then, if its the only thread, it should keep going.
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
	cout << "starting yield to CPU\n";
	dthreads_yield();
	cout << "continuing after yield\n";
	cout << "master thread ended.\n";
}
