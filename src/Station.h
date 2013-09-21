/*
 *	Class representing the overall station
 *	We can think of this as a wrapper around a data structure holding all the locations of the game,
 *		together with methods to manipulate them
 */

#ifndef STATION_H
#define STATION_H

#include <vector>
#include <fstream>
#include "Location.h"
#include "FileReader.h"

class Station{

	private:
		vector<Location*> locations;

	public:
		Station(FileReader& reader, int& linecount);
		~Station();
		int getLocationCount();
		Location* get(unsigned int index);
		Item* find(uint64_t itemcode);
		void leave(int index, Item* item);

};

#endif
