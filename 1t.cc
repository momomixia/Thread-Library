#include "1t.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <ucontext.h>
#include <deque>
#include <iterator>
#include "interrupt.h"
#include <map>

using namespace std;

struct Thread{
    ucontext_t *ucontext;
    char *stack;
    unsigned int semID;
}; 

struct Semaphore{
    int semID;
    int semValue;
};

static ucontext_t *previousContext;

static Thread *currentThread;
static deque<Thread*> activeThreads;
static deque<Thread*> blockedThreads;

static map<unsigned int, Semaphore * > semMap;

static int start (dthreads_func_t func, void *arg);

extern int dthreads_init(dthreads_func_t func, void * arg){
    interrupt_disable();

    //DETECT MULTIPLE CALLS TO dthreads_init()
    if(previousContext!=NULL){
        interrupt_enable();
        return -1;
    }

    Thread* main;
    try{
        main = new Thread;
        main->ucontext = new ucontext_t;
        main->stack = new char [STACK_SIZE];
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }

    

    //DETECT BAD getcontext() CALL
    if(getcontext((main->ucontext))==-1){
        interrupt_enable();
        return -1;
    }

    
    main->ucontext->uc_stack.ss_sp = main->stack;
    main->ucontext->uc_stack.ss_size = STACK_SIZE;
    main->ucontext->uc_stack.ss_flags = 0;
    main->ucontext->uc_link = NULL;

    main->semID = -1;


    makecontext((main->ucontext), (void (*)()) start, 2, func, arg);

    activeThreads.push_front(main);

    //interrupt_enable();

    currentThread = activeThreads.front();
    activeThreads.pop_front();

    try{
        previousContext = new ucontext_t;
        previousContext->uc_stack.ss_sp = new char [STACK_SIZE];
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }

    //DETECT BAD getcontext() CALL
    if(getcontext(previousContext)==-1){
        interrupt_enable();
        return -1;
    }


    previousContext->uc_stack.ss_size = STACK_SIZE;
    previousContext->uc_stack.ss_flags = 0;
    previousContext->uc_link = NULL;
    
    //interrupt_disable();

    //DETECT BAD swapcontext() CALL
    if(swapcontext(previousContext, main->ucontext)==-1){
        interrupt_enable();
        return -1;
    }

    while (activeThreads.size() > 0) {
        currentThread = activeThreads.front();
        activeThreads.pop_front();

        //DETECT BAD swapcontext() CALL
        if(swapcontext(previousContext, (currentThread->ucontext))==-1){
            interrupt_enable();
            return -1;
        }

        // Do we need to free up space from old threads?
    }

    cout << "Thread library exiting.\n";
    exit(0);
}

extern int dthreads_start(dthreads_func_t func, void *arg){
   interrupt_disable();

    //DETECT dthreads_init() NOT YET CALLED
    if(previousContext==NULL){
        interrupt_enable();
        return -1;
    }

    Thread* child;
    try{
        child = new Thread;
        child->ucontext = new ucontext_t;
        child->stack = new char [STACK_SIZE];
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }

   //DETECT BAD getcontext() CALL
   if(getcontext((child->ucontext))==-1){
        interrupt_enable();
        return -1;
   }

   child->ucontext->uc_stack.ss_sp = child->stack;
   child->ucontext->uc_stack.ss_size = STACK_SIZE;
   child->ucontext->uc_stack.ss_flags = 0;
   child->ucontext->uc_link = NULL;

   child->semID = -1;

   makecontext((child->ucontext), (void (*)()) start, 2, func, arg);

   activeThreads.push_back(child);

   interrupt_enable();
   return 0;
}

static int start(dthreads_func_t func, void *arg) {
    //DETECT dthreads_init() NOT YET CALLED
    if(previousContext==NULL){
        interrupt_enable();
        return -1;
    }

    interrupt_enable();
    func(arg);
    interrupt_disable();

    //DETECT BAD swapcontext() CALL
    if(swapcontext((currentThread->ucontext),(previousContext))==-1){
        interrupt_enable();
        return -1;
    }
}

extern int dthreads_yield(void){
    interrupt_disable();

    //DETECT dthreads_init() NOT YET CALLED
    if(previousContext==NULL){
        interrupt_enable();
        return -1;
    }
    activeThreads.push_back(currentThread);

    //DETECT BAD swapcontext() CALL
    if(swapcontext((currentThread->ucontext),(previousContext))==-1){
        interrupt_enable();
        return -1;
    }

    interrupt_enable();
    return 0;
}

/* dthreads_seminit: takes an initial value for a semaphore and returns
 * a unique semaphore identifier or -1 for an error */
extern int dthreads_seminit(unsigned int sem, unsigned int value){
    interrupt_disable();

    //DETECT dthreads_init() NOT YET CALLED
    if(previousContext==NULL){
        interrupt_enable();
        return -1;
    }

    //DETECT SEMAPHORE ALREADY INITIALIZED
    if(semMap.find(sem)!=semMap.end()){
        interrupt_enable();
        return -1;
    }

    Semaphore* currentSemaphore;
    try{
        currentSemaphore = new Semaphore;
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }
    
    currentSemaphore -> semID = sem;
    currentSemaphore -> semValue = value;

    semMap.insert(make_pair(currentSemaphore->semID,currentSemaphore));

    interrupt_enable();
    return 0;
}

/* dthreads_semup: takes the identifier for an initialized semaphore and
 * increments its value */
extern int dthreads_semup(unsigned int sem){
    interrupt_disable();

    //DETECT dthreads_init() NOT YET CALLED
    if(previousContext==NULL){
        interrupt_enable();
        return -1;
    }

    //DETECT THAT SEMAPHORE HAS BEEN INITILIAZED
    if(semMap.find(sem)==semMap.end()){
        interrupt_enable();
        return -1;
    }

    
    if((semMap.at(sem)->semValue) == 0){
        //iterate thorugh blockedThreads, and transfer all threads with semID=sem to activeThreads
        std::deque<Thread*>::iterator it = blockedThreads.begin();
        while (it != blockedThreads.end()){
            if (((*it)->semID) == sem){
                (*it)->semID=-1;
                activeThreads.push_back(*it);
                it = blockedThreads.erase(it);
            }
            else {
                ++it;
            }  
        }
    }    

    (semMap.at(sem)->semValue)++;

    interrupt_enable();
    return 0;
}

/* dthreads_semdown: takes the identifier for an initialized semaphore
 * and returns after the semaphore's value has been decremented by
 * the calling thread; a semaphore's value can only be decremented if
 * its value if it is greater than 0; returns 0 on success or -1 for
 * an error */ 
 extern int dthreads_semdown(unsigned int sem){
    interrupt_disable();

    //DETECT dthreads_init() NOT YET CALLED
    if(previousContext==NULL){
        interrupt_enable();
        return -1;
    }

    //DETECT THAT SEMAPHORE HAS BEEN INITILIAZED
    if(semMap.find(sem)==semMap.end()){
        interrupt_enable();
        return -1;
    }

    

    if ((semMap.at(sem)->semValue) >= 0){
        if(semMap.at(sem)->semValue == 0){
            std::deque<Thread*>::iterator it = activeThreads.begin();

            while (it != activeThreads.end()){
                if ((*it)->semID == sem){
                    (*it)->semID=sem;
                    blockedThreads.push_back(*it);
                    it = activeThreads.erase(it); 
                }
                else {
                    ++it;
                }
            }

            currentThread->semID=sem;
            blockedThreads.push_back(currentThread);


            //DETECT BAD swapcontext() CALL
            if(swapcontext((currentThread->ucontext),(previousContext))==-1){
                interrupt_enable();
                return -1;
            }        
        }
        else {
            (semMap.at(sem)->semValue)--;
        }
        interrupt_enable();
        return 0;
    } else{
     interrupt_enable(); 
     return -1;
 }
}