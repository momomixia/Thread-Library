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

  //for (i=0; i<5; i++, g++) {

    cout << id << ":\t" << "\t" << endl;

     if(id=="parent thread"){
       //cout <<"parent bout to get shackled " << endl;
       dthreads_semdown(1);

     }

     if(id=="child thread"){
    //   cout <<"child thread saves the day for daddy " << endl;
       dthreads_semup(1);
        cout <<"child thread did it " << endl;
       dthreads_yield();
     }

    // if (dthreads_yield()) {
    //   cout << "dthreads_yield failed\n";
    //   exit(1);
    // }
    cout <<"finished " << (char *) id << endl;
  //}
    return;
}

void parent(void *a) {
  dthreads_seminit(1,1);

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
  if (dthreads_init( (dthreads_func_t) parent, (void *) 100)) {
    cout << "dthreads_init failed\n";
    exit(1);
  }
}