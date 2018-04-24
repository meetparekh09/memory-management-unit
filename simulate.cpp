/*
	Module Name: simulate.cpp
	Description: Runs the simulation of memory management
*/
#include <stdio.h>
#include <vector>
#include "pager.h"
#include "datastructures.h"

/*************************** imported from main.cpp ***************************/
extern int curr_frame_index;
extern int frame_limit;
extern Frame **frame_table;
extern Process **processes;
extern int num_processes;
extern bool O_flag, P_flag, F_flag, S_flag, x_flag, y_flag, f_flag, a_flag;
extern char algo;


/*************************** imported from readinput.cpp ***************************/
extern char* getNextLine();


/*************************** Global Variables ***************************/
Pager *pager;
std::vector<unsigned int> age (frame_limit);


/*************************** Function Definition ***************************/
Frame* allocate_from_free_list();
Frame* get_frame();
void print_frame_table();
void print_age();

void simulate() {
	/*
		Function Name: simulate
		Returns: void
		Description: simulates the memory management
	*/
	char cmd; //command
	int cmd_num; //number provided along with command
	int curr_proc = -1; //current context
	Frame *frame = NULL; //current frame
	PTE **page_table = NULL; //current page table

	//summary stats info variables
	unsigned long int instr_num = 0, ctx_switches = 0, reads = 0, writes = 0;

	//initialize the pager as per per algorithm provided
	if(algo == 'f') {
		pager = new FIFOPager();
	} else if(algo == 's') {
		pager = new SecondChancePager();
	} else if(algo == 'r') {
		pager = new RandomPager();
	} else if(algo == 'n') {
		pager = new NRUPager();
	} else if(algo == 'c') {
		pager = new ClockPager();
	} else if(algo == 'a') {
		pager = new AgingPager();
		for(int i = 0; i < frame_limit; i++) {
			age.push_back(0);
		}
	}

	char *line = NULL; //buffer for holding input line
	line = getNextLine();
	sscanf(line, "%c %d", &cmd, &cmd_num);

	//load the context as first command is always context
	page_table = processes[cmd_num]->page_table; 
	curr_proc = cmd_num;
	ctx_switches++;

	//print info as per flags
	if(O_flag)
		printf("%lu: ==> %c %d\n", instr_num, cmd, cmd_num);
	

	while(line = getNextLine()) {
		instr_num++;

		sscanf(line, "%c %d", &cmd, &cmd_num);

		//print info as per flags
		if(O_flag)
			printf("%lu: ==> %c %d\n", instr_num, cmd, cmd_num);

		//context switch
		if(cmd == 'c') {
			ctx_switches++;
			
			//load context as specified in argument of command
			page_table = processes[cmd_num]->page_table;
			curr_proc = cmd_num;

			//print info as per flags
			if(x_flag)
				processes[curr_proc]->print_page_table();
			if(f_flag) {
				print_frame_table();
				print_age();
			}

			//move onto next command
			continue;
		} 

		//otherwise it is a read or write command
		else {
			//get the pte of specified page
			PTE *pte = page_table[cmd_num];

			//if invalid then raise SEgV
			if(!pte->valid) {
				if(O_flag)
					printf(" SEGV\n");
				processes[curr_proc]->seg_v++;
				reads++;

				//print info as per flags
				if(x_flag)
					processes[curr_proc]->print_page_table();
				if(f_flag) {
					print_frame_table();
					print_age();
				}

				//move onto next command
				continue;
			}

			//if it is not in physical memory then look for a frame to be allocated
			if(!pte->frame_assigned) {
				frame = get_frame();
			} 
			//otherwise directly use the frame
			else {
				frame = frame_table[pte->frame_index];
			}
			
			//if frame is being used for the first time
			if(frame->process_id == -1) {

				//if file mapped then fin
				if(pte->file_mapped) {
					pte->referenced = 0;
					pte->modified = 0;
					if(O_flag) {
						printf(" FIN\n");
					}
					processes[curr_proc]->fin++;
				}
				//if paged out then in
				else if(pte->paged_out) {
					pte->referenced = 0;
					pte->modified = 0;
					if(O_flag)
						printf(" IN\n");
					processes[curr_proc]->pin++;
				} 
				else {
					if(O_flag)
						printf(" ZERO\n");
					processes[curr_proc]->zero++;
				}

				
				//print info as per flags
				if(O_flag)
					printf(" MAP %d\n", frame->frame_number);

				//map the page to memory
				processes[curr_proc]->maps++;

				//update the info in PTE and Frame
				pte->frame_assigned = 1;
				pte->frame_index = frame->frame_number;
				frame->process_id = curr_proc;
				frame->page_number = cmd_num;
			} 

			//if there is some other page on the frame then it has to be replaced
			else if(frame->process_id != curr_proc || frame->page_number != cmd_num) {
				//print info as per flags
				if(O_flag)
					printf(" UNMAP %d:%d\n", frame->process_id, frame->page_number);

				//unmap the frame that is there in the memory
				processes[frame->process_id]->unmaps++;
				PTE *target_pte = processes[frame->process_id]->page_table[frame->page_number];
				target_pte->frame_assigned = 0;
				
				
				//if the page was modified then it needs to be framed out
				if(target_pte->modified) {

					//if file mapped then it is file out otherwise page out
					if(target_pte->file_mapped) {
						//print info as per flags
						if(O_flag)
							printf(" FOUT\n");

						//fout as file mapped
						processes[frame->process_id]->fout++;
					} else {
						//print info as per flags
						if(O_flag)
							printf(" OUT\n");

						//page out as it was modified
						target_pte->paged_out = 1;
						processes[frame->process_id]->pout++;
					}
				}
				
				//if page to be loaded is file mapped then fin
				if(pte->file_mapped) {
					pte->referenced = 0;
					pte->modified = 0;

					//print info as per flags
					if(O_flag) {
						printf(" FIN\n");
					}
					processes[curr_proc]->fin++;
				}

				//if the page was paged out then it is page in
				else if(pte->paged_out) {
					pte->referenced = 0;
					pte->modified = 0;
					if(O_flag)
						printf(" IN\n");
					processes[curr_proc]->pin++;
				} 

				//otherwise zero the memory and map the page to memory
				else {
					if(O_flag)
						printf(" ZERO\n");
					processes[curr_proc]->zero++;
				}

				if(O_flag)
					printf(" MAP %d\n", frame->frame_number);
				processes[curr_proc]->maps++;


				//update PTE and Frame
				pte->frame_assigned = 1;
				pte->frame_index = frame->frame_number;
				frame->process_id = curr_proc;
				frame->page_number = cmd_num;
			}

			//update other pte flags and print the info as per flags
			pte->referenced = 1;
			if(cmd == 'r') {
				reads++;
			} else {
				if(pte->write_protected) {
					if(O_flag)
						printf(" SEGPROT\n");
					processes[curr_proc]->seg_p++;
					reads++;
					if(x_flag)
						processes[curr_proc]->print_page_table();
					if(f_flag) {
						print_frame_table();
						print_age();
					}
					continue;
				}
				pte->modified = 1;
				writes++;
			}
			if(x_flag)
				processes[curr_proc]->print_page_table();
			if(f_flag) {
				print_frame_table();
				print_age();
			}
		}
	}



	//print the info as per flags
	if(P_flag)
		for(int i = 0; i < num_processes; i++) {
			processes[i]->print_page_table();
		}

	if(F_flag)
		print_frame_table();

	if(S_flag) {
		unsigned long long int cost = 0;
		for(int i = 0; i < num_processes; i++) {
			processes[i]->print_stats();
			cost += processes[i]->get_cost();
		}

		cost += ctx_switches*121 + reads + writes;

		printf("TOTALCOST %lu %lu %llu\n", ctx_switches, instr_num+1, cost);
	}
}


Frame* allocate_from_free_list() {
	/*
		Function Name: allocate_from_free_list
		Returns: 
			Frame*: frame to be allocated, NULL otherwises
		Description: simply return the next frame if not exhausted of physical memory
	*/

	//if not exhausted of physical memory then get the frame and increase the current index
	if(curr_frame_index < frame_limit) {
		Frame *frame = frame_table[curr_frame_index];
		curr_frame_index++;
		return frame;
	} else {
		return NULL;
	}
}


Frame* get_frame() {
	/*
		Function Name: get_frame
		Returns:
			Frame*: get the frame, either from free list or from pager
		Description: returns the frame to be used for the page that needs to be accessed
	*/
	Frame *frame = allocate_from_free_list();
	if(frame == NULL) frame = pager->determine_victim_frame();
	return frame;
}

void print_age() {
	/*
		Function Name: print_age
		Returns: void
		Description: prints the age of all frames for aging algorithm
	*/
	if(algo == 'a') {
		for(int i = 0; i < frame_limit; i++) {
			if(frame_table[i]->process_id != -1) {
				printf("%d:%d ", i, age.at(i));
			}
		}
		printf("\n");
	}
}

void print_frame_table() {
	/*
		Function Name: print_frame_table
		Returns: void
		Description: print the frame table in the required format
	*/
	printf("FT: ");
	for(int i = 0; i < frame_limit; i++) {
		if(frame_table[i]->process_id == -1) {
			printf("* ");
		} else {
			printf("%d:%d ", frame_table[i]->process_id, frame_table[i]->page_number);
		}
	}
	printf("\n");
}