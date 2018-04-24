/*
	Module Name: main.cpp
	Description: Entry point of program. Defines global data structures for containing all 		processes and frame table. Acts as the controller for the program
*/
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include "datastructures.h"


/*************************** imported from random.cpp ***************************/
extern void load_random_numbers(char *filename);


/*************************** imported from readinput.cpp ***************************/
extern void openInputFile(char *filename);
extern void initProcess();

/*************************** imported from simulate.cpp ***************************/
extern void simulate();



/**************************** Global Variables ****************************/
Process **processes; //holds all the processes
int frame_limit; //memory size
Frame **frame_table; //holds all the frames
int curr_frame_index = 0; //used for initial allocation of frames
int num_processes; //total number of processes using the system
bool O_flag = false, P_flag = false, F_flag = false, S_flag = false, x_flag = false, f_flag = false; //flags provided in option
char algo = 'n'; //algorithm to be implemented



int main(int argc, char *argv[]) {
	/*
		Function Name: main
		Arguments:
			int argc: number of command line arguments
			char *argv[]: string array containing all the command line arguments
		Returns: int: program exit status
		Description: entry point of program and also its controlledr
	*/

	int opt;
	int num = -1; 
	char *option;

	while((opt = getopt(argc, argv, "a:f:o:")) != -1) {
		switch(opt) {
		//get the algorithm to be implemented
		case 'a':
			if(optarg != NULL) algo = optarg[0];
			break;
		//get the physical memory size
		case 'f':
			if(optarg != NULL) num = atoi(optarg);
			break;
		//get all the other optional arguments
		case 'o':
			if(optarg != NULL) {
				option = (char*) memchr(optarg, 'O', strlen(optarg));
				if(option!=NULL) {
					O_flag = true;
				}
				option = (char*) memchr(optarg, 'P', strlen(optarg));
				if(option!=NULL) {
					P_flag = true;
				}
				option = (char*) memchr(optarg, 'F', strlen(optarg));
				if(option!=NULL) {
					F_flag = true;
				}
				option = (char*) memchr(optarg, 'S', strlen(optarg));
				if(option!=NULL) {
					S_flag = true;
				}
				option = (char*) memchr(optarg, 'x', strlen(optarg));
				if(option!=NULL) {
					x_flag = true;
				}
				option = (char*) memchr(optarg, 'f', strlen(optarg));
				if(option!=NULL) {
					f_flag = true;
				}
			}
			break;
		default:
			printf("Invalid Option\n");
		}
	}

	load_random_numbers(argv[optind+1]); //load the random numbers from the rfile
	
	//initialize the global file variable defined in readinput.cpp
	openInputFile(argv[optind]);
	//initialize all the processes along with its page table
	initProcess();

	//initialize the frame table
	frame_limit = num;
	frame_table = (Frame**)malloc(sizeof(Frame*)*frame_limit);
	for(int i = 0; i < frame_limit; i++) {
		frame_table[i] = (Frame*)malloc(sizeof(Frame));
		frame_table[i]->process_id = -1;
		frame_table[i]->page_number = -1;
		frame_table[i]->frame_number = i;
	}
	
	//run the simulation
	simulate();
}