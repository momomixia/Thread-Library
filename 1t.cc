#include "1t.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <ucontext.h>

using namespace std;

extern int dthreads_init(dthreads_func_t func, void * arg){
	printf("builing new thread\n");
	ucontext_t * ucontext_ptr; 
	getcontext(ucontext_ptr); // ucontext_ptr has type (ucontext_t *)
    /*
     * Direct the new thread to use a different stack. Your thread
     * library should allocate STACK_SIZE bytes for each thread's
     * stack.
     */
    char *stack = new char [STACK_SIZE];
    ucontext_ptr->uc_stack.ss_sp = stack;
    ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
    ucontext_ptr->uc_stack.ss_flags = 0;
    ucontext_ptr->uc_link = NULL;
    /*
     * Direct the new thread to start by calling start(arg1, arg2).
     */
    makecontext(ucontext_ptr, (void (*)()) func, 1, arg);
}
// int dthreads_start(dthreads_func_t func, void *arg);
// int dthreads_yield(void);