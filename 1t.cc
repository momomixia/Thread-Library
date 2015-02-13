#include "1t.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <ucontext.h>

using namespace std;

extern int dthreads_init(dthreads_func_t func, void * arg){
	printf("initializing new thread\n");
	ucontext_t ucontext_ptr; 
	getcontext(&ucontext_ptr); 

    char *stack = new char [STACK_SIZE];
    ucontext_ptr.uc_stack.ss_sp = stack;
    ucontext_ptr.uc_stack.ss_size = STACK_SIZE;
    ucontext_ptr.uc_stack.ss_flags = 0;
    ucontext_ptr.uc_link = NULL;

    makecontext(&ucontext_ptr, (void (*)()) func, 1, arg);
    setcontext(&ucontext_ptr);
    printf("finishing thread\n");
}
extern int dthreads_start(dthreads_func_t func, void *arg){
	printf("starting new thread\n");
	ucontext_t new_context; 
	ucontext_t current_context;
	getcontext(&new_context); 
	getcontext(&current_context);
	
    char *stack = new char [STACK_SIZE];
    new_context.uc_stack.ss_sp = stack;
    new_context.uc_stack.ss_size = STACK_SIZE;
    new_context.uc_stack.ss_flags = 0;
    new_context.uc_link = NULL;


    // do more set up before calling function, cause we are fucked once its called. it is killing itself and has nowhere to return to
    //check piazza
    makecontext(&new_context, (void (*)()) func, 1, arg);

    
    swapcontext(&current_context, &new_context);
    //setcontext(&new_context);
}
extern int dthreads_yield(void){

}