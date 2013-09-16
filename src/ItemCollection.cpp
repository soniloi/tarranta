#include "ItemCollection.h"

/*
 *	Constructor
 *	We require a pointer to the associated Station object at this point so we can set initial locations
 */
ItemCollection::ItemCollection(FileReader& reader, int& linecount, Station* station){

	string line = reader.getline();

	while(line.compare(FILE_DELIMITER_SECTION)){

		vector<string> tokens = Tokeniser::splitToVector(line, REGEX_FILE);

		if(tokens.size() < MIN_TOKENS_ITEM){ // Basic range check
			cerr << "Bad or corrupt datafile: insufficient tokens on line " << linecount << endl;
			exit(EXIT_FAILURE);
		}

		int status;
		Statics::strToInt(status, tokens.at(INDEX_ISTATUS), HEX);

		int initloc;
		Statics::strToInt(initloc, tokens.at(INDEX_IINITLOC));

		if(initloc >= station->getLocationCount()){
			cerr << "Bad or corrupt datafile: location out-of-range on line " << linecount << endl;
			exit(EXIT_FAILURE);
		}

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
		station->get(initloc)->deposit(item); // Add to initial location's item list
	
		this->items[cd] = item;

		line = reader.getline();
		linecount++;
	}

}

/*
 *	Destructor
 *	May seem like overkill, but will allow greater flexibility later should name aliases for items be introduced
 */
ItemCollection::~ItemCollection(){

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
 *	Returns a special null item if item does not exist; this will be an error, but will minimise segmentation faults
 */
Item* ItemCollection::get(uint64_t cd){

	map<uint64_t, Item*>::iterator it = this->items.find(cd);

	if(it == this->items.end()) // If no item by that code in the collection, return null item
		return this->items[ITEM_NULL];

	return it->second;

}

/*
 *	Count the number of items with a certain attribute
 */
int ItemCollection::countItemsWithAttribute(int attribute){

	int result = 0;

	for(map<uint64_t, Item*>::iterator it = this->items.begin() ; it != this->items.end() ; it++){
		if(it->second->hasAttribute(attribute))
			result++; // Add the item's self-value, if relevant
	}

	return result;

}