#include "Location.h"

/*
 *	Constructor
 */
Location::Location(int i, int stat, string shortn, string longn, string fulln){

	this->id = i;
	this->status = stat;
	this->shortname = shortn;
	this->longname = longn;
	this->fullname = fulln;

}

/*
 *	Destructor
 */
Location::~Location(){
}

/*
 *	Return the ID of this Location
 */
int Location::getID(){
	return this->id;
}

/*
 *	Create all the internal links cross-referencing this location correctly with all others
 */
void Location::crossReference(Location* n, Location* s, Location* e, Location* w, Location* ne, Location* sw, Location* se, Location* nw, Location* u, Location* d, Location* nowh){
	
	this->directions[CMD_NORTH] = n;
	this->directions[CMD_SOUTH] = s;
	this->directions[CMD_EAST] = e;
	this->directions[CMD_WEST] = w;
	this->directions[CMD_NORTHEAS] = ne;
	this->directions[CMD_SOUTHWES] = sw;
	this->directions[CMD_SOUTHEAS] = se;
	this->directions[CMD_NORTHWES] = nw;
	this->directions[CMD_UP] = u;
	this->directions[CMD_DOWN] = d;

	this->nowhere = nowh;
	this->determineOut();

}

/*
 *	Set or reset the out direction from this location
 */
void Location::determineOut(){
	Location* out = this->getOnlyDirection();
	if(out)
		this->directions[CMD_OUT] = out;
	else
		this->directions[CMD_OUT] = this->nowhere;
}

/*
 *	Return pointer to the only Location pointed to by this Location
 *	If this Location is connected to multiple locations (or none, somehow), return NULL
 *	Intention is to be used for 'out' direction
 */
Location* Location::getOnlyDirection(){

	Location* result = NULL;

	for(map<uint64_t, Location*>::iterator it = this->directions.begin() ; it != this->directions.end() ; it++){
		if(it->second->id != LOCATION_NOWHERE){
			if(result && it->first != CMD_BACK) // If this is not the only direction ("back" will double with one of the others so we ignore)
				return NULL;
			else // Otherwise, set this provisionally as the only direction
				result = it->second;
		}
	}

	return result;

}

/*
 *	Link a direction
 */
void Location::setDirection(uint64_t dir, Location* loc){
	this->directions[dir] = loc;
	if(dir != CMD_BACK)
		this->determineOut(); // Re-assess and set the 'out' direction for this location ("back" will double with one of the others so we ignore)
}

/*
 *	Count the number of treasures at this location
 */
int Location::countTreasures(){

	int result = 0;

	for(map<uint64_t, Item*>::iterator it = this->items.begin() ; it != this->items.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_TREASURE))
			result++;
	}

	for(map<uint64_t, Item*>::iterator it = this->fixtures.begin() ; it != this->fixtures.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_TREASURE))
			result++;
	}

	return result;
}

/*
 *	Check whether this location has an attribute as an ambient condition
 */
bool Location::hasAttribute(int attribute){
	return (this->status & attribute);
}

/*
 *	Set a current attribute on or off at this location
 */
void Location::setAttribute(int attribute, bool set){
	if(set)
		this->status |= attribute;
	else
		this->status &= (~attribute);
}

/*
 *	Check whether there is any source of light at this location
 */
bool Location::hasLight(){
	if(this->hasAttribute(CTRL_LOC_HAS_LIGHT)){ // First check whether the location itself has ambient lighting
		return true;
	}
	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){ // Then check whether any items are currently emitting light
		if(it->second->hasAttribute(CTRL_ITEM_SWITCHABLE) && it->second->hasAttribute(CTRL_ITEM_GIVES_LIGHT)){
			SwitchableItem* si = (SwitchableItem*) it->second;
			if(si->isOn())
				return true;
		}
	}
	return false;
}

/*
 *	Check whether there is any source of air at this location
 */
bool Location::hasAir(){
	if(this->hasAttribute(CTRL_LOC_HAS_AIR)) // First check whether the location itself has ambient air
		return true;
	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){ // Then check whether any items are currently emitting air
		if(it->second->hasAttribute(CTRL_ITEM_GIVES_AIR) && !it->second->hasAttribute(CTRL_ITEM_WORN)){ // Ignore items that need to be worn to be effective
			return true;
		}
	}
	return false;
}

/*
 *	Check whether there is any source of gravity at this location
 */
bool Location::hasGravity(){
	return (this->hasAttribute(CTRL_LOC_HAS_GRAVITY)); // Check whether the location itself has ambient gravity; anything else would have to be worn (i.e. in inventory)
}

/*
 *	Check whether there is any protection from snomps at this location
 */
bool Location::hasNosnomp(){
	if(this->hasAttribute(CTRL_LOC_HAS_NOSNOMP)) // First check whether the location itself has ambient protection from snomps
		return true;
	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){ // Then check whether any items are currently protecting from snomps	
		if(it->second->hasAttribute(CTRL_ITEM_GIVES_NOSNOMP)){
			return true;
		}
	}
	return false;
}

/*
 *	Fix an item at this location
 */
void Location::fix(Item* item){
	this->fixtures[item->getCode()] = item;
}

/*
 *	Deposit an item at this location
 */
void Location::deposit(Item* item){
	this->items[item->getCode()] = item;
}

/*
 *	Return a pointer to the location requested by a certain movement command
 *	May return NULL (no such thing as that direction)
 *		or the Nowhere Location (no way to go in that direction)
 */
Location* Location::getDirection(uint64_t dircode){

	map<uint64_t, Location*>::iterator it = this->directions.find(dircode);
	if(it == this->directions.end())
		return NULL; // I'm just including this so that any unrecognised direction codes are caught (this shouldn't happen)
	return
		it->second;

}

/*
 *	Return pointer to an item with a certain code from this location,
 *		but do not remove it from the location
 *	Returns NULL if item at this location
 */
Item* Location::get(uint64_t cd){

	map<uint64_t, Item*>::iterator it = this->items.find(cd);

	if(it == this->items.end()) // If no item by that code at this location, return NULL
		return NULL;

	return it->second;

}

/*
 *	Return pointer to an item with a certain code from this location,
 *		and remove it from the location
 */
Item* Location::extract(uint64_t cd){

	map<uint64_t, Item*>::iterator it = this->items.find(cd);

	if(it == this->items.end()) // If no item by that code at this location, return NULL (no need to remove)
		return NULL;

	Item* item = it->second;
	this->items.erase(it);

	return item;

}

/*
 *	Return basic longname description of this location
 */
string Location::getLongname(){
	return this->longname;
}

/*
 *	Return string form of the stub name of this location;
 *		the form that would appear in the prompt, i.e. just the shortname
 */
string Location::getStub(){
	return this->shortname;
}

/*
 *	Return one-line description of this location
 */
string Location::getBasic(){
	return "You are " + this->longname;
}

/*
 *	Return string representing a list of items in a map
 *	If obstructiononly is true, only concatenate information about items that are obstructions
 */
string Location::itemListToString(map<uint64_t, Item*> things, bool obstructiononly){

	string str;
	for(map<uint64_t, Item*>::iterator it = things.begin() ; it != things.end() ; it++){
		if(!obstructiononly || it->second->hasAttribute(CTRL_ITEM_OBSTRUCTION)){
			str += "\nThere is " + it->second->getLongname() + " here";
			if(it->second->hasAttribute(CTRL_ITEM_OBSTRUCTION) || it->second->hasAttribute(CTRL_ITEM_TREASURE))
				str += EXCLAMATION;
			else
				str += DOT;
		}
	}
	return str;
}

/*
 *	Return string consisting of information a player would receive on arriving at this location
 *		i.e. a basic, one-line description, followed by a list of all portable objects,
 *		followed by a list of all fixtures that are also obstructions
 */
string Location::getArrival(){
	string str = this->getBasic();
	str += DOT;
	str += this->itemListToString(this->items, false);
	str += this->itemListToString(this->fixtures, true);
	return str;
}

/*
 *	Return string consisting of the maximal amount of information about this location,
 *		i.e., any further information about the location, together with a list of all fixtures,
 *		together with a list of all portable objects
 */
string Location::getAll(){
	string str = this->getBasic();
	str += this->fullname;
	str += DOT;
	str += this->itemListToString(this->items, false);
	str += this->itemListToString(this->fixtures, false);
	return str;
}

/*
 *	Return the first obstruction found at this location
 *	Return NULL if none present
 */
Item* Location::getObstruction(){

	// Search all non-mobile items
	for(map<uint64_t, Item*>::iterator it = this->fixtures.begin() ; it != this->fixtures.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_OBSTRUCTION))
			return it->second;
	}

	// Search all mobile items
	for(map<uint64_t, Item*>::iterator it = this->items.begin() ; it != this->items.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_OBSTRUCTION))
			return it->second;
	}

	return NULL;

}

/*
 *	Return a list of pointers to all container items at this location
 */
list<Container*> Location::getContainers(){

	list<Container*> result;

	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_CONTAINER))
			result.push_back( (Container*) it->second);
	}

	for(map<uint64_t, Item*>::iterator it = fixtures.begin() ; it != fixtures.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_CONTAINER))
			result.push_back( (Container*) it->second);
	}

	return result;

}