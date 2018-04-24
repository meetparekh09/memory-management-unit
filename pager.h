/*
	Module Name: pager.h
	Description: Defines all the pagers implementing different algorithms
*/
#include "datastructures.h"
#include <vector>
#include <list>

/*************************** imported from main.cpp ***************************/
extern Frame **frame_table;
extern int frame_limit;
extern Process **processes;

/*************************** imported from random.cpp ***************************/
extern int myrandom(int size);


/*************************** imported from simulate.cpp ***************************/
extern std::vector<unsigned int> age;



#ifndef PAGER_H
#define PAGER_H

class Pager {
	/*
		Class Name: Pager
		Description: defines a virtual parent pager class
	*/
public:

	//virtual function to be implemented by all childrens
	virtual Frame* determine_victim_frame() {
		/*
			Function Name: determine_victim_frame
			Returns: 
				Frame*: reference to frame selected by algorithm
			Description: returns the frame to be used for memory requirements
		*/
		return NULL;
	};
};

#endif


#ifndef FIFO_PAGER_H
#define FIFO_PAGER_H

class FIFOPager: public Pager {
	/*
		Class Name: FIFOPager
		Description: defines a class for pager allocating as per FIFO Algorithm
	*/
private:
	int ind;

public:
	FIFOPager() {
		ind = 0;
	}

	Frame* determine_victim_frame() {
		/*
			Function Name: determine_victim_frame
			Returns: 
				Frame*: reference to frame selected by algorithm
			Description: returns the frame to be used for memory requirements
		*/
		Frame *frame = frame_table[ind];
		ind = (ind+1)%frame_limit;
		return frame;
	}
};

#endif

#ifndef SECOND_CHANCE_PAGER_H
#define SECOND_CHANCE_PAGER_H


class SecondChancePager : public Pager {
	/*
		Class Name: SecondChancePager
		Description: defines a class for pager allocating as per Second Chance Algorithm
	*/
private:
	std::list<Frame*> second_chance_list;

public:
	SecondChancePager() {
		for(int i = 0; i < frame_limit; i++) {
			second_chance_list.push_back(frame_table[i]);
		}
	}

	Frame* determine_victim_frame() {
		/*
			Function Name: determine_victim_frame
			Returns: 
				Frame*: reference to frame selected by algorithm
			Description: returns the frame to be used for memory requirements
		*/
		Frame *frame = second_chance_list.front();
		while(processes[frame->process_id]->page_table[frame->page_number]->referenced) {
			processes[frame->process_id]->page_table[frame->page_number]->referenced = 0;
			second_chance_list.push_back(frame);
			second_chance_list.pop_front();
			frame = second_chance_list.front();
		}
		second_chance_list.pop_front();
		second_chance_list.push_back(frame);
		return frame;
	}
};

#endif

#ifndef RANDOM_PAGER_H
#define RANDOM_PAGER_H

class RandomPager : public Pager {
	/*
		Class Name: RandomPager
		Description: defines a class for pager allocating as per random allocation
	*/
public:
	Frame* determine_victim_frame() {
		/*
			Function Name: determine_victim_frame
			Returns: 
				Frame*: reference to frame selected by algorithm
			Description: returns the frame to be used for memory requirements
		*/
		int ind = myrandom(frame_limit); //get random frame number
		return frame_table[ind];
	}
};

#endif


#ifndef NRU_PAGER_H
#define NRU_PAGER_H

class NRUPager : public Pager {
	/*
		Class Name: NRUPager
		Description: defines a class for pager allocating as per NRU Algorithm
	*/
private:
	int clock; //keep track of clock to clear references
public:
	NRUPager() {
		clock = 0;
	}

	Frame* determine_victim_frame() {
		/*
			Function Name: determine_victim_frame
			Returns: 
				Frame*: reference to frame selected by algorithm
			Description: returns the frame to be used for memory requirements
		*/
		std::vector <Frame*> class_1;
		std::vector <Frame*> class_2;
		std::vector <Frame*> class_3;
		std::vector <Frame*> class_4;
		
		clock = (clock+1)%10;

		unsigned int selector;
		Frame *frame;

		
		
		for(int i = 0; i < frame_limit; i++) {
			frame = frame_table[i];
			selector = 2*processes[frame->process_id]->page_table[frame->page_number]->referenced + processes[frame->process_id]->page_table[frame->page_number]->modified;
			switch(selector) {
				case 0:
					class_1.push_back(frame);
					break;
				case 1:
					class_2.push_back(frame);
					break;
				case 2:
					class_3.push_back(frame);
					break;
				case 3:
					class_4.push_back(frame);
					break;
			}
		}

		int ind;
		if(class_1.size() != 0) {
			ind = myrandom(class_1.size());

			frame = class_1.at(ind);
		} else if(class_2.size() != 0) {
			ind = myrandom(class_2.size());
			frame = class_2.at(ind);
		} else if(class_3.size() != 0) {
			ind = myrandom(class_3.size());
			frame = class_3.at(ind);
		} else {
			ind = myrandom(class_4.size());
			frame = class_4.at(ind);
		}

		Frame *temp;
		if(clock == 0) {
			for(int i = 0; i < class_3.size(); i++) {
				temp = class_3.at(i);
				processes[temp->process_id]->page_table[temp->page_number]->referenced = 0;
			}
			for(int i = 0; i < class_4.size(); i++) {
				temp = class_4.at(i);
				processes[temp->process_id]->page_table[temp->page_number]->referenced = 0;
			}
		}

		return frame;
	}
};


#endif


#ifndef CLOCK_PAGER_H
#define CLOCK_PAGER_H

class ClockPager : public Pager {
	/*
		Class Name: ClockPager
		Description: defines a class for pager allocating as per Clock Algorithm
	*/
private:
	int ind;

public:
	ClockPager() {
		ind = 0;
	}

	Frame* determine_victim_frame() {
		/*
			Function Name: determine_victim_frame
			Returns: 
				Frame*: reference to frame selected by algorithm
			Description: returns the frame to be used for memory requirements
		*/
		Frame *frame = frame_table[ind];
		while(processes[frame->process_id]->page_table[frame->page_number]->referenced) {
			processes[frame->process_id]->page_table[frame->page_number]->referenced = 0;
			ind = (ind+1)%frame_limit;
			frame = frame_table[ind];
		}
		ind = (ind+1)%frame_limit;
		return frame;
	}
};

#endif

#ifndef AGING_PAGER_H
#define AGING_PAGER_H

class AgingPager : public Pager {
	/*
		Class Name: AgingPager
		Description: defines a class for pager allocating as per Aging Algorithm
	*/
public:
	Frame* determine_victim_frame() {
		/*
			Function Name: determine_victim_frame
			Returns: 
				Frame*: reference to frame selected by algorithm
			Description: returns the frame to be used for memory requirements
		*/
		for(int i = 0; i < frame_limit; i++) {
			age.at(i) = (age.at(i) >> 1) | (processes[frame_table[i]->process_id]->page_table[frame_table[i]->page_number]->referenced << 31);
			processes[frame_table[i]->process_id]->page_table[frame_table[i]->page_number]->referenced = 0;
		}

		int min_age_ind = 0;
		int min_age = age.at(0);

		for(int i = 1; i < frame_limit; i++) {
			if(age.at(i) < age.at(min_age_ind)) {
				min_age_ind = i;
				min_age = age.at(i);
			}
		}

		return frame_table[min_age_ind];
	}
};


#endif