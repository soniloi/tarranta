#include "SwitchableItem.h"

/*
 *	Destructor
 */
SwitchableItem::~SwitchableItem(){
	//cerr << "SwitchableItem destructor called on: " << this->longname << endl;
}

bool SwitchableItem::isOn(){
	return this->on;
}

void SwitchableItem::setOn(bool b){
	this->on = b;
}

/*
 *	Override method to display also whether the switchable item is on or off
 */
string SwitchableItem::getLongname(){

	string result = this->longname;
	result += " (currently ";

	if(this->on)
		result += "on";
	else
		result += "off";

	result += ')';

	return result;

}

/*
 *	Override method to display also whether the switchable item is on or off
 */
string SwitchableItem::getInventoryname(){
	return this->getLongname();
}

/*
 *	Override methodeturn full description of this switchable item; includes information as to whether it is on
 */
string SwitchableItem::getFullname(){
	string result = this->fullname;
	result += ".\nIt is currently ";
	if(this->on)
		result += "on";
	else
		result += "off";
	return result;
}