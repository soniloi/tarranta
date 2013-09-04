/*
 *	Class representing the overall station
 *	We can think of this as a wrapper around a data structure holding all the locations of the game,
 *		together with methods to manipulate them
 */

#ifndef STATION_H
#define STATION_H

#include <vector>
#include <fstream>
#include <cstdlib>
#include "Location.h"
#include "BitFileReader.h"

class Station{

	private:
		vector<Location*> locations;

	public:
		Station(BitFileReader& reader);
		~Station();
		int getLocationCount();
		Location* get(unsigned int index);
		Item* find(uint64_t itemcode);
		void fix(int index, Item* item);
		void leave(int index, Item* item);

};

#endif
