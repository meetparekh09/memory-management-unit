/*
	Module Name: random.cpp
	Description: designed to load random numbers from rfile and generate random numbers as 		per size provided.
*/
#include <fstream>


/**************************** Global Variables ****************************/
int *__numbers; //array that holds the numbers
int __size; //number of random numbers in rfile
int __curr; //current position in array


void load_random_numbers(char *filename) {
	/*
		Function Name: load_random_numbers
		Arguments:
			char *filename: path to rfile
		Returns: void
		Description: loads random numbers from rfile into __numbers array and initializes 		__size and __curr
	*/
	int num;

	//open file in std::ios_base::in mode to read from file.
	std::fstream file(filename, std::ios_base::in); 

	//first number read into __size
	file >> __size; 

	//initialize the data structure to load numbers
	__numbers = new int[__size];
	__curr = 0;


	//read numbers
	while(file >> num) {
		__numbers[__curr++] = num;
	}
	__curr = 0;
	return;
}


int myrandom(int size) {
	/*
		Function Name: myrandom
		Arguments:
			int size: size to be taken as modulus
		Returns: int - random number
		Description: takes random number from array and generates the number using size.
	*/
	int num = __numbers[__curr];
	num = num % size;
	__curr = (__curr + 1) % __size;
	return num;
}