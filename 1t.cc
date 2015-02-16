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
    int threadID; // Do we need this?
    char *stack;
    ucontext_t *ucontext;
    unsigned int semID;
}; 

struct Semaphore{
    int semID;
    int semValue;
};

static int GLOBAL_THREAD_ID = 1; 

static ucontext_t *previousContext;

static Thread *currentThread;
static deque<Thread*> activeThreads;
static deque<Thread*> blockedThreads;

static map<unsigned int, Semaphore * > semMap;

static void start (dthreads_func_t func, void *arg);

extern int dthreads_init(dthreads_func_t func, void * arg){
    interrupt_disable();

    Thread* main = new Thread;

    main->ucontext = new ucontext_t;
    getcontext((main->ucontext));

    main->threadID = 0;

    main->stack = new char [STACK_SIZE];
    main->ucontext->uc_stack.ss_sp = main->stack;
    main->ucontext->uc_stack.ss_size = STACK_SIZE;
    main->ucontext->uc_stack.ss_flags = 0;
    main->ucontext->uc_link = NULL;

    main->semID = 0;

    makecontext((main->ucontext), (void (*)()) start, 2, func, arg);

    activeThreads.push_front(main);

    interrupt_enable();

    currentThread = activeThreads.front();
    activeThreads.pop_front();
    
    previousContext = new ucontext_t;
    getcontext(previousContext);
    
    interrupt_disable();

    swapcontext(previousContext, main->ucontext);

    while (activeThreads.size() > 0) {
        currentThread = activeThreads.front();
        activeThreads.pop_front();

        swapcontext(previousContext, (currentThread->ucontext));

        // Do we need to free up space from old threads?
    }

    cout << "Thread library exiting. \n";
    exit(0);
}

extern int dthreads_start(dthreads_func_t func, void *arg){
   interrupt_disable();

   Thread* child = new Thread;

   child->ucontext = new ucontext_t;
   getcontext((child->ucontext));

   child->threadID = GLOBAL_THREAD_ID;
   GLOBAL_THREAD_ID ++;

   child->stack = new char [STACK_SIZE];
   child->ucontext->uc_stack.ss_sp = child->stack;
   child->ucontext->uc_stack.ss_size = STACK_SIZE;
   child->ucontext->uc_stack.ss_flags = 0;
   child->ucontext->uc_link = NULL;

   child->semID = 0;

   makecontext((child->ucontext), (void (*)()) start, 2, func, arg);

   activeThreads.push_back(child);

   interrupt_enable();
   return 0;
}

static void start(dthreads_func_t func, void *arg) {
    interrupt_enable();
    func(arg);
    interrupt_disable();

    swapcontext((currentThread->ucontext),(previousContext));
}


extern int dthreads_yield(void){
    interrupt_disable();
    activeThreads.push_back(currentThread);
    swapcontext(currentThread->ucontext, previousContext);
    interrupt_enable();
    return 0;
}


/* dthreads_seminit: takes an initial value for a semaphore and returns
 * a unique semaphore identifier or -1 for an error */
extern int dthreads_seminit(unsigned int sem, unsigned int value){
    interrupt_disable();
    Semaphore* currentSemaphore = new Semaphore;
    currentSemaphore ->  semID = sem;
    currentSemaphore -> semValue = value;
    semMap.insert(make_pair(currentSemaphore->semID,currentSemaphore));
    interrupt_enable();
    return 0;
}

/* dthreads_semup: takes the identifier for an initialized semaphore and
 * increments its value */
extern int dthreads_semup(unsigned int sem){
    interrupt_disable();
    if((semMap.at(sem)->semValue) == 0){
        //iterate thorugh blockedThreads, and erase all threads with same semID
        std::deque<Thread*>::iterator it = blockedThreads.begin();
        while (it != blockedThreads.end()){
            if (((*it)->semID) == sem){
                (*it)->semID=0;

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
    if ((semMap.at(sem)->semValue) > 0){

        if(semMap.at(sem)->semValue == 1){
            (semMap.at(sem)->semValue)--;
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
            currentThread = activeThreads.front(); 
            activeThreads.pop_front();

            swapcontext(previousContext, (currentThread->ucontext));
            interrupt_enable();
            return 0;
        }
    } else{
     interrupt_enable(); 
     return -1;
 }
}