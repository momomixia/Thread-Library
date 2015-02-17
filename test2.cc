


#include <stdlib.h>
#include <iostream>
#include "1t.h"
#include <assert.h>

using namespace std;

int g=0;

void loop(void *a) {
  char *id;
  int i;

  id = (char *) a;
  cout <<"loop called with id " << (char *) id << endl;

  if(id=="parent thread"){
   dthreads_semdown(1);
   cout <<"unlocked but bout to yield " << endl;
   dthreads_yield();
 }

 if(id=="child thread"){
  dthreads_semup(1);
     cout <<"about to yield " << endl;
  dthreads_yield();
}

cout <<"finished " << (char *) id << endl;

return;
}

void parent(void *a) {
  if ( dthreads_semup(1)) {
    cout << "dthreads_semup failed\n";
  }
  if ( dthreads_semdown(1)) {
    cout << "dthreads_semdown failed\n";
  }
  dthreads_seminit(1,0);
 
if ( dthreads_seminit(1,5)) {
    cout << "dthreads_seminit failed\n";
}

  int arg;
  arg = (int) a;

  cout << "parent called with arg " << arg << endl;
  if (dthreads_start((dthreads_func_t) loop, (void *) "child thread")) {
    cout << "dthreads_start failed\n";
  exit(1);
}

loop( (void *) "parent thread");
}

int main(int argc, char *argv[]) {

  if (dthreads_yield()){
    cout << "dthreads_yield failed\n";
  }

  if (dthreads_init( (dthreads_func_t) parent, (void *) 100)) {
    cout << "dthreads_init failed\n";
    exit(1);
  }
}