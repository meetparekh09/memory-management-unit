/*
	Module Name: readinput.cpp
	Description: designed to read input file and initialize process page tables and its VMAs. Also it reads input commands one by one and gives it to simulator.
*/

#include <stdio.h>
#include <fstream>
#include <string.h>
#include "datastructures.h"
#include <stdlib.h>

/**************************** Global Variables ****************************/
std::fstream file; //reference to input file


/*************************** imported from main.cpp ***************************/
extern Process **processes; //contains objects of all processes
extern int num_processes; //total number of processes

void openInputFile(char *filename) {
	/*
		Function Name: openInputFile
		Arguments:
			char *filename: path to input file
		Returns: void
		Description: initializes reference to input file in input mode
	*/

	//open file in std::ios_base::in mode to read from file.
	file.open(filename, std::ios_base::in);
}



void initProcess() {
	/*
		Function Name: initProcess
		Returns: void
		Description: initializes process page tables and its VMAs as specified in input file
	*/

	int num_vmas, s_vma, e_vma, w_prot, f_wrt; //used to read from file
	char line[100]; //buffer

	//skip until first input
	file.getline(line, 100);
	while(strlen(line) == 0 || line[0] == '#') {
		if(file.getline(line, 100)) continue;
	}

	//read first line as number of processes
	sscanf(line, "%d", &num_processes);
	
	//initialize list of all processes maintained globally in main.cpp
	processes = (Process**)malloc(sizeof(Process*)*num_processes);


	//initialize page table of all the processes as specified in input file
	for(int i = 0; i < num_processes; i++) {
		processes[i] = new Process(i);

		//skip until next input line
		file.getline(line, 100);
		while(strlen(line) == 0 || line[0] == '#') {
			if(file.getline(line, 100)) continue;
		}
		
		//read the number of VMA for process i
		sscanf(line, "%d", &num_vmas);
		
		int j = 0; //references to 1 position after the last initialized PTE


		//read each VMA and initialize appropriately
		for(int k = 0; k < num_vmas; k++) {
			//skip until next input line
			file.getline(line, 100);
			while(strlen(line) == 0 || line[0] == '#') {
				if(file.getline(line, 100)) continue;
			}

			//read VMA specification
			sscanf(line, "%d %d %d %d", &s_vma, &e_vma, &w_prot, &f_wrt);	

			//set all PTE as invalid from j to s_vma. 
			//This is to initialize the PTE of holes as invalid
			while(j < s_vma) {
				processes[i]->page_table[j]->valid = 0;
				processes[i]->page_table[j]->write_protected = 0;
				processes[i]->page_table[j]->modified = 0;
				processes[i]->page_table[j]->referenced = 0;
				processes[i]->page_table[j]->paged_out = 0;
				processes[i]->page_table[j]->file_mapped = 0;
				processes[i]->page_table[j]->frame_assigned = 0;
				processes[i]->page_table[j]->frame_index = 0;
				j++;
			}

			//next initialize the current VMA PTE entries as specified
			while(j <= e_vma) {
				processes[i]->page_table[j]->valid = 1;
				processes[i]->page_table[j]->write_protected = w_prot;
				processes[i]->page_table[j]->modified = 0;
				processes[i]->page_table[j]->referenced = 0;
				processes[i]->page_table[j]->paged_out = 0;
				processes[i]->page_table[j]->file_mapped = f_wrt;
				processes[i]->page_table[j]->frame_assigned = 0;
				processes[i]->page_table[j]->frame_index = 0;
				j++;
			}
		}

		//This is to take care of hole at the end of VMA if any.
		while(j < 64) {
			processes[i]->page_table[j]->valid = 0;
				processes[i]->page_table[j]->write_protected = w_prot;
				processes[i]->page_table[j]->modified = 0;
				processes[i]->page_table[j]->referenced = 0;
				processes[i]->page_table[j]->paged_out = 0;
				processes[i]->page_table[j]->file_mapped = f_wrt;
				processes[i]->page_table[j]->frame_assigned = 0;
				processes[i]->page_table[j]->frame_index = 0;
				j++;
		}
	}
}

char* getNextLine() {
	/*
		Function Name: getNextLine
		Returns: char* - input command string
		Description: get the next command line from the file and return the same.
	*/
	char *line = new char[100]; //buffer to hold line

	//get the next valid line
	while(file.getline(line, 100)) {
		if(strlen(line) == 0 || line[0] == '#') {
			continue;
		}
		return line;
	}
	return NULL;
}