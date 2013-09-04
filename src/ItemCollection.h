/*
 *	Class representing the collection of all items in the game
 *	Like Station is to Location, can be thought of a wrapper around a data structure holding all Items
 */

#include <map>
//#include "Item.h"
#include "Container.h"
#include "SwitchableItem.h"
#include "Station.h"
#include "BitFileReader.h"

class ItemCollection{

	private:
		map<uint64_t, Item*> items;

	public:
		ItemCollection(BitFileReader& reader, Station* station);
		~ItemCollection();
		Item* get(uint64_t cd);

};