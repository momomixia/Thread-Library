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


using namespace std;

struct Thread{
    int id;
    char* stack;
    ucontext_t *ucontext;
    bool finished;
    unsigned int semID = 0;
}; 

static Thread* currentThread;
static ucontext_t *previousContext;
static int id = 1; 

static deque<Thread*> q;
static void delete_current_thread();
static void start (dthreads_func_t func, void *arg);


static void delete_current_thread(){
    delete currentThread->stack;

    currentThread->ucontext->uc_stack.ss_sp = NULL;
    currentThread->ucontext->uc_stack.ss_size = 0;
    currentThread->ucontext->uc_stack.ss_flags = 0;
    currentThread->ucontext->uc_link = NULL;
    delete currentThread->ucontext;
    delete currentThread;
    currentThread = NULL;


}

extern int dthreads_init(dthreads_func_t func, void * arg){
    printf("initializing main thread\n");
    interrupt_disable();

    Thread* main = new Thread;
    main->ucontext = new ucontext_t;
    getcontext((main->ucontext));

    main->id = 0;

    main->stack = new char [STACK_SIZE];
    main->ucontext->uc_stack.ss_sp = main->stack;
    main->ucontext->uc_stack.ss_size = STACK_SIZE;
    main->ucontext->uc_stack.ss_flags = 0;
    main->ucontext->uc_link = NULL;
    main->finished = false;
    makecontext((main->ucontext), (void (*)()) start, 2, func, arg);
    q.push_front(main);
    interrupt_enable();

    currentThread = q.front();
    q.pop_front();
    

    
    previousContext = new ucontext_t;
    getcontext(previousContext);
    
    interrupt_disable();
    swapcontext(previousContext, main->ucontext);

    while (q.size() > 0) { //will we ever enter this?
        // printf("inside while loop\n");
        if(currentThread->finished == true){
            // printf("deleting current thread\n");
            // q.pop_front();
        }

      
        currentThread = q.front();
        q.pop_front();
        // printf("go to next thread\n");
        swapcontext(previousContext, (currentThread->ucontext));

    }

    cout << "Thread library exiting. \n";
    exit(0);
}

extern int dthreads_start(dthreads_func_t func, void *arg){
    // printf("starting new thread\n");
    

     interrupt_disable();

    Thread* child = new Thread;
    child->ucontext = new ucontext_t;
    getcontext((child->ucontext));

    child->id = id;
    id ++;

    child->stack = new char [STACK_SIZE];
    child->ucontext->uc_stack.ss_sp = child->stack;
    child->ucontext->uc_stack.ss_size = STACK_SIZE;
    child->ucontext->uc_stack.ss_flags = 0;
    child->ucontext->uc_link = NULL;
    child->finished = false;

    makecontext((child->ucontext), (void (*)()) start, 2, func, arg);

    q.push_back(child);

    //swapcontext(previousContext, (child->ucontext));

   
    // printf("new thread created\n");
     interrupt_enable();
    return 0;
    
}

static void start(dthreads_func_t func, void *arg) {
    interrupt_enable();
    // printf("execute thread function\n");
    func(arg);
    interrupt_disable();

    currentThread->finished = true;
    swapcontext((currentThread->ucontext),(previousContext));
}



extern int dthreads_yield(void){
    interrupt_disable();
    q.push_back(currentThread);
    swapcontext(currentThread->ucontext, previousContext);
    interrupt_enable();

}

struct Sem{
    int semID;
    int semValue;
};

static deque<Thread*> blockedThreads;
static deque<Thread*> deleteThreads;
static map<unsigned int, Sem*> semMap;





/* dthreads_seminit: takes an initial value for a semaphore and returns
 * a unique semaphore identifier or -1 for an error */
extern int dthreads_seminit(unsigned int sem, unsigned int value){
 Sem currentSem = new Sem;
 currentSem ->  semID = sem;
 currentSem -> semValue = value;
 semMap.put(currentSem->semID,currentSem);
}

/* dthreads_semup: takes the identifier for an initialized semaphore and
 * increments its value */
    extern int dthreads_semup(unsigned int sem){
     if(semMap.get(sem)-> semValue == 0){
        //iterate thorugh blockedThreads, and remove all threads with same semID
        std::deque<Thread*>::iterator it = blockedThreads.begin();
        while (it != blockedThreads.end()){
            if (it->semID == sem){
                q.push_back(it);
                deleteThreads.push_back(it);
            }

          }

           std::deque<Thread*>::iterator it2 = deleteThreads.begin();
         while (it2 != deleteThreads.end()){
                blockedThreads.remove(it2);
          } 
          deleteThreads.clear();

     }   
    (semMap.get(sem)->semValue)++;
    

//semMap.put(semMap.get(sem)->semID,semMap.get(sem)->semValue++);

}


/* dthreads_semdown: takes the identifier for an initialized semaphore
 * and returns after the semaphore's value has been decremented by
 * the calling thread; a semaphore's value can only be decremented if
 * its value if it is greater than 0; returns 0 on success or -1 for
 * an error */ 
extern int dthreads_semdown(unsigned int sem){

 if ((semMap.get(sem)->semValue) > 0{
    if(semMap.get(sem)->semValue == 1){
        blockedThreads.push_back(currentThread);
        q.pop_front();
        swapcontext(previousContext, (currentThread->ucontext));
    }
    (semMap.get(sem)->semValue)--;
    return 0;
}
else{
    return -1;
}
}



