#include "dthreads.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath> 
using namespace std;

std::vector<int> corkboard; 
unsigned int CORKBOARD_CURRENT_CAPACITY = 0;
unsigned int CORKBOARD_MAXCAPACITY;

int NUM_CASHIERS;
int LIVING_CASHIERS;
int CASHIER_IDS;

unsigned int lock_maker;
unsigned int lock_cashier;
unsigned int makerCV=2000; // ID larger than 1000 which is largest possible sandwich ID 
unsigned int posterCV=2001;

void master_maker(void* args);
void cashier(void* args);


int main(int argc, char *argv[]){

	// Initializing constants from command line inputs
	NUM_CASHIERS=argc-2; // Number of input files aka number of cashiers (one input file per cashier)
	LIVING_CASHIERS=NUM_CASHIERS;
	CORKBOARD_MAXCAPACITY = atoi(argv[1]);

	// Create master thread
	dthreads_init((dthreads_func_t) master_maker, argv);
}

void master_maker(void* args){
	dthreads_lock(lock_maker);

	// Create cahsier threads
	char ** filenames = (char **)(args); // Landon is a omniscient god.
	for(int count = 2; count < NUM_CASHIERS+2; count++){
		dthreads_start((dthreads_func_t) cashier, (void *) filenames[count]);
	}
	
	int last_sandwich = -1; // Default sandwich value
	int next_sandwich;

	while(LIVING_CASHIERS>0){

		// If free space on corkboard
		while(CORKBOARD_CURRENT_CAPACITY<CORKBOARD_MAXCAPACITY){ 

			// Signal for cashiers to post order, then wait for order to be posted
			dthreads_signal(lock_cashier, posterCV);
			dthreads_wait(lock_maker, makerCV);
			
			// Adjust max capacity of corkboard
			if(LIVING_CASHIERS<CORKBOARD_MAXCAPACITY){
				CORKBOARD_MAXCAPACITY=LIVING_CASHIERS;
			}

			// Finish if no orders left
			if(LIVING_CASHIERS==0){
				dthreads_unlock(lock_maker);
				return;
			}		
		}

		// Pick the next sandwich based on closest sandwich ID
		int sandwich_diff = 10000;
		int indexof_next_sandwich;
		for(int i = 0; i<corkboard.size(); i++){
			if(std::abs (corkboard.at(i) - last_sandwich) < sandwich_diff) {
				indexof_next_sandwich = i;
				sandwich_diff = std::abs (corkboard.at(i) - last_sandwich);
			}
		}
		next_sandwich = corkboard.at(indexof_next_sandwich);
		last_sandwich = next_sandwich;

		// Make the sandwich and signal to cashier that it is ready
		CORKBOARD_CURRENT_CAPACITY--;
		corkboard.erase(corkboard.begin() + indexof_next_sandwich); //erase from corkboard
		dthreads_signal(lock_cashier, next_sandwich);
		dthreads_wait(lock_maker, makerCV); // Wait for print confimation form cashier
	}
	dthreads_unlock(lock_maker);
}

void cashier(void* args){
	dthreads_lock(lock_cashier);

	// Set cashier IDs when created
	int myCashierID = CASHIER_IDS;
	CASHIER_IDS++;

	// Preparing filename buffer from command arguments
	char *filename = (char*)args;
	ifstream stream1(filename);
	char raw_order[80];

	while(!stream1.eof())
	{
		stream1 >> raw_order; // Read 1 line  sandwich order

		if(!stream1.eof() && !raw_order[0]==0){ 	
			int order_number = atoi(raw_order); 

			// If corkboard is full, wait for spot on board to open up
			while(CORKBOARD_CURRENT_CAPACITY >= CORKBOARD_MAXCAPACITY){
				dthreads_wait(lock_cashier, posterCV); 
			}

			// Post new order to corkboard 
			corkboard.push_back(order_number); 
			CORKBOARD_CURRENT_CAPACITY++;
			cout << "POSTED: cashier " << myCashierID << " sandwich " << order_number << endl; 

			// Signal master maker that there is a new order on corkboard
			dthreads_signal(lock_maker, makerCV);

			// Wait for my order to be made, then print and signal confirmation
			dthreads_wait(lock_cashier, order_number);
			cout << "READY: cashier " << myCashierID << " sandwich " << order_number << endl; 
			dthreads_signal(lock_maker, makerCV);

			// Waiting for signal to post my next order
			dthreads_wait(lock_cashier, posterCV);
		}
	}

	// If no more sandwich orders in file, finish thread and signal maker that cashier is out of orders
	LIVING_CASHIERS--;
	dthreads_signal(lock_maker, makerCV);
	dthreads_unlock(lock_cashier);
}