/* TEST 2
*  Create two threads, a parent thread and a child.  Using semaphores, parent thread calls semaphore down and waits, 
*  child thread increments sempahore and finishes, then parent thread finishes. (optional call to yield)
*/



#include <stdlib.h>
#include <iostream>
#include "1t.h"
#include <assert.h>

using namespace std;

int g=0;

void loop(void *a) {
  char *id;
  int i;
  dthreads_init( (dthreads_func_t) loop, (void *) 100);

  id = (char *) a;
  cout <<"loop called with id " << (char *) id << endl;

  if(id=="parent thread"){
   dthreads_semdown(1);
   cout <<"unlocked but bout to yield " << endl;
   dthreads_yield();
 }

 if(id=="child thread"){
  dthreads_semup(1);
  dthreads_yield();
}

cout <<"finished " << (char *) id << endl;

return;
}

void parent(void *a) {
  dthreads_seminit(1,0);
  dthreads_seminit(1,0);
  int arg;
  arg = (int) a;

  cout << "parent called with arg " << arg << endl;
  if (dthreads_start((dthreads_func_t) loop, 
   (void *) "child thread")) {
    cout << "dthreads_start failed\n";
  exit(1);
}


loop( (void *) "parent thread");
}

int main(int argc, char *argv[]) {

  dthreads_start((dthreads_func_t) loop, (void *) "child thread");
  if (dthreads_init( (dthreads_func_t) parent, (void *) 100)) {
    cout << "dthreads_init failed\n";
    exit(1);
  }
}