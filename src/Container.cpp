#include "Container.h"

/*
 *	Destructor
 */
Container::~Container(){
	//cerr << "Container destructor called on: " << this->longname;
}

/*
 *	Insert item into container
 *	This will not check whether there is already an item in the container
 */
void Container::insertItem(Item* item){
	this->within = item;
}

/*
 *	Return pointer to the item in the Container, without removing it
 *	NULL return means the container is empty
 */
Item* Container::getItemWithin(){
	return this->within;
}

/*
 *	Remove item from container and return pointer to it
 */
Item* Container::extractItemWithin(){
	Item* i = this->within;
	this->within = NULL;
	return i;
}

/*
 *	Check whether this container, or any container within it, contains a certain item
 */
bool Container::contains(Item* item){
	if(this->within == item)
		return true;
	if(this->within && this->within->hasAttribute(CTRL_ITEM_CONTAINER)){
		return ((Container*) this->within)->contains(item);
	}
	return false;
}

/*
 *	Check whether this container, or any container within it, contains any item with a certain attribute
 */
bool Container::containsWithAttribute(int attribute){
	if(this->within == NULL)
		return false; // Nothing in the container
	if(this->within->hasAttribute(attribute))
		return true;
	if(this->within->hasAttribute(CTRL_ITEM_CONTAINER))
		return ((Container*) this->within)->containsWithAttribute(attribute);
	return false;
}

/*
 *	Return whether there is anything in the container or not
 */
bool Container::isEmpty(){
	return (!this->within);
}

/*
 *	Return whether this is a suitable container for a certain item
 */
bool Container::isSuitable(Item* item){
	if(this->within)
		return false; // There is already something in the container
	if(this->size <= item->getSize())
		return false; // Container is not big enough to contain item
	if(this->hasAttribute(CTRL_ITEM_CONTAINER_LIQUID) && !item->hasAttribute(CTRL_ITEM_LIQUID))
		return false; // Cannot place a non-liquid in a liquid container
	if(!this->hasAttribute(CTRL_ITEM_CONTAINER_LIQUID) && item->hasAttribute(CTRL_ITEM_LIQUID))
		return false; // Cannot place a liquid in a non liquid-container
	return true;
}

/*
 *	Return the container that contains a certain item
 *	This could be this container itself, or a container within this one
 *	Will return NULL if not found
 */
Container* Container::getParentOf(Item* item){
	if(this->within == item)
		return this;
	if(this->within && this->within->hasAttribute(CTRL_ITEM_CONTAINER)){
		Container* container = (Container*) this->within;
		return container->getParentOf(item);
	}
	return NULL; // Not found
}

/*
 *	Override method to return weight of this container together with anything that's in it
 */
int Container::getWeight(){
	int result = this->size;
	if(this->within)
		result += this->within->getWeight();
	return result;
}

/*
 *	Override method to display also what is in the container
 */
string Container::getLongname(){

	string result = this->longname;

	result += " (";

	if(this->within)
		result += "containing " + this->within->getLongname();
	else
		result += "empty";

	result += ')';

	return result;

}

/*
 *	Display what is in the container, displaying any nested containers correctly
 *	tabcount parameter is the nesting depth
 */
string Container::getInventoryname(int tabcount){

	string result = this->longname;

	result += "\n\t\t";

	for(int i = 0 ; i < tabcount ; i++)
		result += TAB;

	if(this->within){
		result += " containing: ";
		if(this->within->hasAttribute(CTRL_ITEM_CONTAINER)){
			Container* withincontainer = (Container*) this->within;
			result += withincontainer->getInventoryname(tabcount+ONE);
		}
		else
			result += this->within->getInventoryname();
	}
	else
		result += " (empty)";

	return result;

}

/*
 *	Override method to also display what is in the container
 */
string Container::getInventoryname(){
	return this->getInventoryname(ZERO);
}

/*
 *	Override methodeturn full description of this container; includes information about the things it contains, if any
 */
string Container::getFullname(){
	string result = this->fullname;
	result += ".\n";
	if(!this->within)
		result += "It is currently empty";
	else{
		result += "It contains: " + this->within->getLongname() + ".";
		result += "\n\tIt is ";
		result += this->within->getFullname();
	}
	return result;
}