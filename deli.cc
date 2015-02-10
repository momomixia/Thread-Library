#include "dthreads.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;

int CORKBOARD_MAXCAPACITY;
int NUM_CASHIERS;
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
		printf("%s\n", filenames[count]);
		dthreads_start((dthreads_func_t) cashier, (void *) filenames[count]);

	}
}
void cashier(void* args){
	printf("cashier is cashin\n");
	//ifstream stream1(((char*)args)[0]);
	char *filename;
   	filename= (char*)args;
   	printf("%s\n",filename);
	//printf("name of file: %s\n", ((char[] *)args));
	// char a[80];

	// if(!stream1)

	// {

	// 	cout << "While opening a file an error is encountered" << endl;

	// }

	// else

	// {

	// 	cout << "File is successfully opened" << endl;

	// }

	// while(!stream1.eof())
	// {

	// 	stream1 >> a;
		
	// 	cout << a << endl;
	// 	printf(a);

	// }

}