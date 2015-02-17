/*
CORRECT OUTPUT:
"Thread 1 being initialized"
"Thread 2 being initialized"
"Thread 3 being initialized"
"Initiating loop yielding to CPU"
"Child thread ID: 1 running" 
"Calling semdown and adding self to Q"
"Child thread ID: 2 running" 
"Calling semdown and adding self to Q"
"Child thread ID: 3 running" 
"Calling semdown and adding self to Q"
"Returned to main loop after all child threads started waiting"
"Calling semup to release child threads and yielding"
"Child thread ID: 1 signaled and running"
"Child thread ID: 2 signaled and running"
"Child thread ID: 3 signaled and running"
"Returned to main loop, finishing up.  THIS CALL SHOULD BE FOLLOWED BY: Thread Library Exiting"
"Thread Library Exiting."


GENERATED OUTPUT:
Thread 1 being initialized
Thread 2 being initialized
Thread 3 being initialized
Initiating loop yielding to CPU
Child thread ID: 1 running
Calling semdown and adding self to Q
Child thread ID: 2 running
Calling semdown and adding self to Q
Child thread ID: 3 running
Calling semdown and adding self to Q
Returned to main loop after all child threads started waiting
Calling semup to release child threads and yielding
Child thread ID: 1 signaled and running
Child thread ID: 2 signaled and running
Child thread ID: 3 signaled and running
Returned to main loop, finishing up.  THIS CALL SHOULD BE FOLLOWED BY: Thread Library Exiting
Thread library exiting.
*/


#include <stdlib.h>
#include <iostream>
#include "1t.h"
#include <assert.h>

using namespace std;
void child(void* args);

void loop(void *a) {
  int i;
  dthreads_seminit(1,0);
  for (i=1; i<4; i++) {
    cout << "Thread " << i << " being initialized\n";
    dthreads_start( (dthreads_func_t) child, (void *) i);
  }
  cout << "Initiating loop yielding to CPU\n";
  dthreads_yield();
  cout << "Returned to main loop after all child threads started waiting\n";
  cout << "Calling semup to release child threads and yielding\n";
  dthreads_semup(1);
  dthreads_yield();
  cout << "Returned to main loop, finishing up.  THIS CALL SHOULD BE FOLLOWED BY: Thread Library Exiting\n";
  

}

void child(void *a) {
  int arg;
  arg = (int) a;
  cout << "Child thread ID: " << arg << " running\n"; 
  cout << "Calling semdown and adding self to Q\n";
  dthreads_semdown(1);
  cout << "Child thread ID: " << arg << " signaled and running\n";
}

int main(int argc, char *argv[]) {
  if (dthreads_init( (dthreads_func_t) loop, (void *) 100)) {
    cout << "dthreads_init failed\n";
    exit(1);
  }
}
