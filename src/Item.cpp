#include "Item.h"

/*
 *	Constructor
 */
Item::Item(uint64_t cd, int stat, string longn, string fulln, string writ, int sz){

	this->location = NULL; // Default is that the item does not exist
	this->code = cd;
	this->status = stat;
	this->longname = longn;
	this->fullname = fulln;
	this->writing = writ;
	this->size = size;

}

/*
 *	ostream overloading; may not be the most suitable, but I needed a way to be
 *	able to read the codes quicker than trying to learn off the ASCII table
 */
ostream& operator <<(ostream& output, const Item& it){

	for(int i=CHARS_PER_CODE-ONE ; i >=0 ; i--){
		char current = (char) (it.code >> (BITS_PER_BYTE*i));
		output << current;
	}

	return output;

}

/*
 *	Set the location of this item to a new location
 */
void Item::setLocation(Location* loc){
 	this->location = loc; // Tell item of its new location
}

 /*
  *	Public accessors
  */
uint64_t Item::getCode(){
 	return this->code;
}

Location* Item::getLocation(){
	return this->location;
}

string Item::getLongname(){
	return this->longname;
}

/*
 *	The string that would be describe the item while in the inventory
 */
string Item::getInventoryname(){
	return this->longname;
}

/*
 *	Return whether this item is present at the player's location
 */
bool Item::isPresent(Location* current){
	return(this->location == current);
}

/*
 *	Return short name of this item
 */
string Item::getShortname(){
	return Statics::codeToStr(this->code);
}

/*
 *	Return full description of this item
 */
string Item::getFullname(){
	return this->fullname;
}

/*
 *	Return whatever writing is on this item
 */
string Item::getWriting(){
	return this->writing;	
}

/*
 *	Check whether this item has a certain attribute
 */
bool Item::hasAttribute(int attribute){
	return (this->status & attribute);
}

/*
 *	Set attributes for this item
 */
void Item::setAttribute(int attribute, bool b){
	if(b)
		this->status |= attribute;
	else
		this->status &= (~attribute);
}