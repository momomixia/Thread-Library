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

  for (i=0; i<10; i++, g++) {
    cout << id << ":\t" << i << "\t" <<
    g << endl;
    if (dthreads_yield()) {
      cout << "dthreads_yield failed\n";
      exit(1);
    }
  }

  if (dthreads_init( (dthreads_func_t) loop, (void *) 100)) {
    cout << "dthreads_init dummy failed\n";
  }

}

void parent(void *a) {
  int arg;
  arg = (int) a;

  cout << "parent called with arg " << arg << endl;
  if (dthreads_start((dthreads_func_t) loop, 
                     (void *) "child 1 thread")) {
    cout << "dthreads_start failed\n";
    exit(1);
  }

  loop( (void *) "parent thread");

  if (dthreads_start((dthreads_func_t) loop, 
                     (void *) "child 2 thread")) {
    cout << "dthreads_start failed\n";
    exit(1);
  }

}

int main(int argc, char *argv[]) {

  if (dthreads_start((dthreads_func_t) loop, 
                     (void *) "dummy thread")) {
    cout << "dthreads_start dummy thread failed\n";
  }

  if (dthreads_init( (dthreads_func_t) parent, (void *) 100)) {
    cout << "dthreads_init failed\n";
    exit(1);
  }
}