#include "ItemCollection.h"
#include <typeinfo>

/*
 *	Constructor
 *	We require a pointer to the associated Station object at this point so we can set initial locations
 */
ItemCollection::ItemCollection(BitFileReader& reader, Station* station){

	string line = reader.getline();

	while(line.compare(FILE_DELIMITER_SECTION)){

		vector<string> tokens = Tokeniser::splitToVector(line, REGEX_FILE);

		int status;
		Statics::strToInt(status, tokens.at(INDEX_ISTATUS), HEX);

		int initloc;
		Statics::strToInt(initloc, tokens.at(INDEX_IINITLOC));

		int sz;
		Statics::strToInt(sz, tokens.at(INDEX_ISIZE));

		string shortn = tokens.at(INDEX_ISHORT);
		uint64_t cd = Statics::strToCode(shortn);

		string longn = tokens.at(INDEX_ILONG);
		string fulln = tokens.at(INDEX_IFULL);
		string writ = tokens.at(INDEX_IWRIT);
		if(!writ.compare("0"))
			writ.clear();

		Item* item;
		if(status & CTRL_ITEM_CONTAINER){
			item = new Container(cd, status, longn, fulln, writ, sz);
		}
		else if(status & CTRL_ITEM_SWITCHABLE){
			item = new SwitchableItem(cd, status, longn, fulln, writ, sz);
		}
		else{
			item = new Item(cd, status, longn, fulln, writ, sz);
		}

		item->setLocation(station->get(initloc)); // Set item's initial location
		station->get(initloc)->deposit(item); // Add item to initial location's item list
	
		this->items[cd] = item;

		line = reader.getline();
	}
/*
	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
		string name = Statics::codeToStr(it->first);
		cerr << name << "\tis\t" << typeid(*(it->second)).name() << "\tbytes allocated: " << sizeof(*(it->second)) << endl;
	}
*/
}

/*
 *	Destructor
 */
ItemCollection::~ItemCollection(){
/*
	for(map<uint64_t, Item*>::iterator it=items.begin() ; it!=items.end() ; it++){
		if(sizeof(it->second) > ZERO)
			delete it->second; // Erase the item
	}
*/

	// Method may seem like overkill, but will allow greater flexibility later if I want to introduce aliases for item names later
	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){

		if(it->second != NULL){
			delete it->second;

			// Having deallocated the memory, we must now search for any other keys pointing to the same value to prevent double frees
			map<uint64_t, Item*>::iterator jt = it;
			jt++;

			while(jt != items.end()){
				if(jt->second == it->second)
					items[jt->first] = NULL; // NULL out any extra keys
				jt++;
			}
		}

	}

}

/*
 *	Return pointer to an item with a certain code from the collection,
 *	Returns NULL if item does not exist
 */
Item* ItemCollection::get(uint64_t cd){

	map<uint64_t, Item*>::iterator it = this->items.find(cd);

	if(it == this->items.end()) // If no item by that code in the collection, return NULL
		return NULL;

	return it->second;

}
