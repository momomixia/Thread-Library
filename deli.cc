#include "dthreads.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int CORKBOARD_MAXCAPACITY;
int NUM_CASHIERS;
int main(int argc, char *argv[]){
	int count; 
	

	if (argc > 1){
  		for (count = 2; count < argc; count++){
	  		printf("Input file #%d : %s\n", count-1, argv[count]);
		}
	}
	NUM_CASHIERS=count-2; //Number of input files aka number of cashiers (one input file per cashier)
	CORKBOARD_MAXCAPACITY = (*argv[1])-'0'; //hackey way to convert character string to integer
	printf("There are %d cashiers\n", NUM_CASHIERS);
	printf("The corkboard capacity is %d\n", CORKBOARD_MAXCAPACITY);

	//dthreads_init((dthreads_func_t) master_maker, 0);
}
void master_maker(){
	printf("master_maker thread started");
	// for(int i=0;i<NUM_CASHIERS;i++){
	// 	dthreads_start(dthreads_func_t func, void *arg);
	// }
}
void cashier(){

}