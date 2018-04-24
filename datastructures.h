/*
	Module Name: datastructures.h
	Description: Contains definitions of PTE, Process and Frame.
*/

#include <stdlib.h>
#include <stdio.h>

#ifndef PTE_H
#define PTE_H

struct PTE {
	/*
		Struct Name: PTE
		Description: defines a page table entry in with 14 bit address
	*/
	unsigned int valid : 1; //whether it is a part of VMA
	unsigned int write_protected: 1; //whether it is write protected
	unsigned int modified: 1; //whether it is modified since last loaded in page table
	unsigned int referenced: 1; //whether it was referered since last loaded in page table
	unsigned int paged_out: 1; //whether it is paged out
	unsigned int file_mapped: 1; //whether there is a file mapped to this page
	unsigned int frame_assigned: 1; //whether this page is loaded into physical frame
	unsigned int frame_index: 7; //address in frame table, 7 bits to address 128 frames.
};

#endif

#ifndef PROCESS_H
#define PROCESS_H

class Process {
	/*
		Class Name: Process
		Description: defines a process with its page table containing PTE of all pages
	*/
public:
	int process_id; 
	PTE **page_table; //pointers to PTE of all VMA Pages

	//records all the stats for summarizing the performance
	long unsigned int seg_p, seg_v, pin, pout, fin, fout, zero, unmaps, maps;

	Process(int process_id) {
		/*
			Function Name: constructor
			Arguments:
				int process_id: process id of the process. It is same as its index in 			global processes array
			Description: Initializes all the members of class
		*/
		this->process_id = process_id;
		page_table = (PTE**)malloc(sizeof(PTE*)*64);
		for(int i = 0; i < 64; i++) {
			page_table[i] = (PTE*)malloc(sizeof(PTE));
		}
		seg_p = seg_v = pin = pout = fin = fout = zero = unmaps = maps = 0;
	}

	void print_stats() {
		/*
			Function Name: print_stats
			Description: prints the summary statistics in the format specified
			Returns: void
		*/
		printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n", process_id, unmaps, maps, pin, pout, fin, fout, zero, seg_v,seg_p);
	}

	void print_page_table() {
		/*
			Function Name: print_page_table
			Description: print the page table of the process in the format specified
			Returns: void
		*/
		printf("PT[%d]: ", process_id);
		PTE *pte;
		for(int i = 0; i < 64; i++) {
			pte = page_table[i];

			//print the pte summary if it is in the physical memory
			if(pte->valid && pte->frame_assigned)
			{
				printf("%d:", i);
				if(pte->referenced) {
					printf("R");
				} else {
					printf("-");
				}
				if(pte->modified) {
					printf("M");
				} else {
					printf("-");
				}
				if(pte->paged_out) {
					printf("S");
				} else {
					printf("-");
				}
				printf(" ");
			} else {
				//if not in physical memory and paged out then print #
				if(pte->valid && pte->paged_out) {
					printf("# ");
				} else {
					printf("* ");
				}
			}
		}
		printf("\n");
	}

	unsigned long int get_cost() {
		/*
			Function Name: get_cost
			Description: returns the cost incurred by this process, calculated as per the 		guidelines provided
			Returns: 
				unsigned long int: cost incurred by this process
		*/
		return (maps+unmaps)*400+(pin+pout)*3000+(fin+fout)*2500+zero*150+seg_v*240+seg_p*300;
	}
};


#endif

#ifndef FRAME_H
#define FRAME_H

struct Frame {
	/*
		Struct Name: Frame
		Description: defines a single Frame
	*/
	int frame_number;
	int process_id; //to reverse map the process
	int page_number; //to get the PTE from the process page table
};


#endif