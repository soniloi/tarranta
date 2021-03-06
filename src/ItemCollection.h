/*
 *	Class representing the collection of all items in the game
 *	Like Station is to Location, can be thought of a wrapper around a data structure holding all Items
 */

#include <map>
#include "Container.h"
#include "SwitchableItem.h"
#include "Station.h"
#include "FileReader.h"

class ItemCollection{

	private:
		map<uint64_t, Item*> items;

	public:
		ItemCollection(FileReader& reader, int& linecount, Station* station);
		~ItemCollection();
		Item* get(uint64_t cd);
		int countItemsWithAttribute(int attribute);

};