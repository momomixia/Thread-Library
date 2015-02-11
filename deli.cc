#include "dthreads.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath> 
using namespace std;
unsigned int makerCV=2000;
unsigned int CORKBOARD_CURRENT_CAPACITY = 0;
unsigned int CORKBOARD_MAXCAPACITY;
std::vector<int> corkboard; 
int NUM_CASHIERS;
int LIVING_CASHIERS;
unsigned int lock_maker;
unsigned int lock_cashier;
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
	LIVING_CASHIERS=NUM_CASHIERS;
	CORKBOARD_MAXCAPACITY = atoi(argv[1]); //hackey way to convert character string to integer
	printf("There are %d cashiers\n", NUM_CASHIERS);
	printf("The corkboard maximum capacity is %d\n", CORKBOARD_MAXCAPACITY);

	dthreads_init((dthreads_func_t) master_maker, argv);
}
void master_maker(void* args){
	dthreads_lock(lock_maker);
	printf("master_maker thread started\n");
	char ** filenames = (char **)(args); //Landon is a omniscient god.
	for(int count = 2; count < NUM_CASHIERS+2; count++){
		dthreads_start((dthreads_func_t) cashier, (void *) filenames[count]);
	}
	
	int last_sandwich = -1; //this is default
	int next_sandwich;
	while(LIVING_CASHIERS>0){

		while(CORKBOARD_CURRENT_CAPACITY<CORKBOARD_MAXCAPACITY){ //wait for board to fill up to capacity
			//printf("waiting for corkboard to fill up, current capacity: %d\n", CORKBOARD_CURRENT_CAPACITY);
			dthreads_wait(lock_maker, makerCV);
			if(LIVING_CASHIERS<CORKBOARD_MAXCAPACITY){
				CORKBOARD_MAXCAPACITY=LIVING_CASHIERS;
				printf("decrementing corkboard max capacity to %d\n", CORKBOARD_MAXCAPACITY);
			}
			if(LIVING_CASHIERS==0){
				dthreads_unlock(lock_maker);
				return;
			}
			
			
		}
		printf("Corkboard is full, time to make a sandwich, max capacity: %d\n", CORKBOARD_CURRENT_CAPACITY);
		int sandwich_diff = 10000;
		int indexof_next_sandwich;
		for(int i = 0; i<corkboard.size(); i++){ //choose closest match in the board
			if(std::abs (corkboard.at(i) - last_sandwich) < sandwich_diff) {
				indexof_next_sandwich = i;
				sandwich_diff = std::abs (corkboard.at(i) - last_sandwich);
			}
		}
		next_sandwich = corkboard.at(indexof_next_sandwich);
		printf("Making Sandwich %d\n", next_sandwich);
		CORKBOARD_CURRENT_CAPACITY--;
		corkboard.erase(corkboard.begin() + indexof_next_sandwich); //erase from corkboard
		dthreads_signal(lock_cashier, next_sandwich); //not sure if this signalling will work.
	}
	dthreads_unlock(lock_maker);
}
void cashier(void* args){
	dthreads_lock(lock_cashier);

	
	char *filename;
   	filename= (char*)args;
   	ifstream stream1(filename);
	char raw_order[80];
	while(!stream1.eof())
	{
		stream1 >> raw_order; 

		if(!stream1.eof() && !raw_order[0]==0){ //might be problematicly hardcoded for these specific test cases.	
			int order_number = atoi(raw_order);
			while(CORKBOARD_CURRENT_CAPACITY >= CORKBOARD_MAXCAPACITY){ //wait for spot on board to open up
				printf("waiting for corkboard space to open up\n");
				dthreads_wait(lock_cashier, CORKBOARD_CURRENT_CAPACITY);
			}
			printf("Corkboard space open, current cap = %d    max cap = %d\n", CORKBOARD_CURRENT_CAPACITY, CORKBOARD_MAXCAPACITY);
			corkboard.push_back(order_number); //push to the board (maybe we implement the board using a global vector... google this.)
			CORKBOARD_CURRENT_CAPACITY++;

			
			printf("pushing order #%d\n", order_number);
			//dthreads_wait(lock_maker, order_number); //I think that we push the order number and we wait for the order number to be signalled 
			dthreads_signal(lock_maker, makerCV);
			dthreads_wait(lock_cashier, order_number);
			printf("order #%d finished\n", order_number);
			//wait for order to be taken (maybe sandwich is the CV and we wait for that to be brodcasted)
			//...

			//be woken up when complete and continue.
			 
		}
	}
	LIVING_CASHIERS--;
	dthreads_unlock(lock_cashier);
	dthreads_signal(lock_maker, makerCV);
}