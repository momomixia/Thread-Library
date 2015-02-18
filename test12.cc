/*

GENERATED OUTPUT

semdown on 1
calling semup on 1
calling semup on 2
calling semup on 3
child thread 2 finishing
semdown on 2
semdown on 3
child thread 1 finishing
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
	dthreads_seminit(1,0);
	dthreads_seminit(2,0);
	dthreads_seminit(3,0);
	dthreads_start( (dthreads_func_t) child_thread1, (void *) 100);
	dthreads_start( (dthreads_func_t) child_thread2, (void *) 100);
}
void child_thread1(void * args){
	cout << "semdown on 1\n";
	dthreads_semdown(1);
	cout << "semdown on 2\n";
	dthreads_semdown(2);
	cout << "semdown on 3\n";
	dthreads_semdown(3);
	cout << "child thread 1 finishing\n";
	
}

void child_thread2(void * args){
	cout << "calling semup on 1\n";
	dthreads_semup(1);
	cout << "calling semup on 2\n";
	dthreads_semup(2);
	// cout << "calling semup on 3\n";
	// dthreads_semup(3);
	cout << "child thread 2 finishing\n";
}
