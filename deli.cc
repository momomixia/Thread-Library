#include "dthreads.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;
int CORKBOARD_CURRENT_CAPACITY = 0;
int CORKBOARD_MAXCAPACITY;
int NUM_CASHIERS;
int LIVING_CASHIERS;
unsigned int lock = 1;
void master_maker(void* args);
void cashier(void* args);

int main(int argc, char *argv[]){
	int count; 

	if (argc > 1){
		for (count = 2; count < argc; count++){
			printf("Input file #%d : %s\n", count-1, argv[count]);
		}
	}
	NUM_CASHIERS=count-2; //Number of input files aka number of cashiers (one input file per cashier)
	CORKBOARD_MAXCAPACITY = atoi(argv[1]); //hackey way to convert character string to integer
	printf("There are %d cashiers\n", NUM_CASHIERS);
	printf("The corkboard capacity is %d\n", CORKBOARD_MAXCAPACITY);

	dthreads_init((dthreads_func_t) master_maker, argv);
}
void master_maker(void* args){
	printf("master_maker thread started\n");
	char ** filenames = (char **)(args); //Landon is a omniscient god.
	for(int count = 2; count < NUM_CASHIERS+2; count++){
		dthreads_start((dthreads_func_t) cashier, (void *) filenames[count]);

	}
	dthreads_lock(lock);
	int last_sandwich = -1; //this is default
	while(LIVING_CASHIERS>0){
		if(LIVING_CASHIERS<CORKBOARD_MAXCAPACITY){
			CORKBOARD_MAXCAPACITY=LIVING_CASHIERS;
		}
		while(CORKBOARD_CURRENT_CAPACITY<CORKBOARD_MAXCAPACITY){ //wait for board to fill up to capacity
			dthreads_wait(lock, CORKBOARD_CURRENT_CAPACITY);
			dthreads_lock(lock); // do we need this or is lock automatically handed back to us when signaled?
		}
		//choose closest match in the board
		CORKBOARD_CURRENT_CAPACITY--;

		//make sandwich
	}
	dthreads_unlock(lock);
}
void cashier(void* args){
	dthreads_lock(lock);
	char *filename;
   	filename= (char*)args;
   	ifstream stream1(filename);
	char raw_order[80];
	while(!stream1.eof())
	{
		stream1 >> raw_order; 

		if(!stream1.eof() && !raw_order[0]==0){ //might be problematicly hardcoded for these specific test cases.	
			int order_number = atoi(raw_order);
			printf("%d\n",order_number);
			while(CORKBOARD_CURRENT_CAPACITY == CORKBOARD_MAXCAPACITY){ //wait for spot on board to open up
				dthreads_wait(lock, CORKBOARD_CURRENT_CAPACITY)
				dthreads_lock(lock); // do we need this or is lock automatically handed back to us when signaled?
			}
			//push to the board (maybe we implement the board using a global vector... google this.)
			dthreads_wait(lock, order_number); //I think that we push the order number and we wait for the order number to be signalled 
			//wait for order to be taken (maybe sandwich is the CV and we wait for that to be brodcasted)
			//...

			//be woken up when complete and continue.
			 
		}
	}
	LIVING_CASHIERS--;
	dthreads_unlock(lock);
}