#include "Inventory.h"

/*
 *	Constructor
 */
Inventory::Inventory(int maxcapacity){
	this->capacity = maxcapacity;
}

/*
 *	Destructor
 */
Inventory::~Inventory(){

}

/*
 *	Deposit an item into the inventory
 */
void Inventory::deposit(Item* item){
	this->items.insert(pair<uint64_t, Item*>(item->code, item));
}

/*
 *	Return pointer to an item with a certain code from the inventory,
 *		but do not remove it from the inventory
 *	Returns NULL if item not in inventory
 */
Item* Inventory::get(uint64_t cd){

	map<uint64_t, Item*>::iterator it = this->items.find(cd);

	if(it == this->items.end()) // If no item by that code in the inventory, return NULL
		return NULL;

	return it->second;

}

/*
 *	Return pointer to an item with a certain code from the inventory,
 *		and remove it from the inventory
 */
Item* Inventory::extract(uint64_t cd){

	map<uint64_t, Item*>::iterator it = this->items.find(cd);

	if(it == this->items.end()) // If no item by that code in the inventory, return NULL (no need to remove)
		return NULL;

	Item* item = it->second;
	this->items.erase(it);

	return item;

}

/*
 *	Return whether an item with a certain code is in the inventory
 */
bool Inventory::contains(uint64_t cd){

	map<uint64_t, Item*>::iterator it = this->items.find(cd);

	return it != this->items.end();

}

/*
 *	Check the inventory for something that emits light
 *	Note that in the case of light, it must also be switched on	
 */
bool Inventory::hasLight(){
	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_SWITCHABLE) && it->second->hasAttribute(CTRL_ITEM_GIVES_LIGHT)){
			SwitchableItem* si = (SwitchableItem*) it->second;
			if(si->isOn())
				return true;
		}
	}
	return false;
}

/*
 *	Check the inventory for something that produces air
 */
bool Inventory::hasAir(){
	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_GIVES_AIR))
			return true;
	}
	return false;
}

/*
 *	Check the inventory for something that exerts (significant, for all you pedants) gravity
 */
bool Inventory::hasGravity(){
	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_GIVES_GRAVITY))
			return true;
	}
	return false;
}

/*
 *	Check the inventory for something that wards off snomps
 */
bool Inventory::hasNosnomp(){
	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_GIVES_NOSNOMP))
			return true;
	}
	return false;
}

/*
 *	Return a string representation of the inventory
 */
string Inventory::toString(){

	string result;

	if(this->items.empty())
		result += "You are not carrying anything.";
	else{
		result += "You currently have the following:";
		for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
			result += "\n\t";
			if(it->second->hasAttribute(CTRL_ITEM_WORN))
				result += "(wearing) ";
			result += it->second->getInventoryname();
		}
	}

	return result;

}

/*
 *	Return a list of pointers to all container items in the inventory
 */
list<Container*> Inventory::getContainers(){

	list<Container*> result;

	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
		if(it->second->hasAttribute(CTRL_ITEM_CONTAINER))
			result.push_back( (Container*) it->second);
	}

	return result;

}	