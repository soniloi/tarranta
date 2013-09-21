/*
 *	Class representing items that can be switched on and off
 */
#ifndef SWITCHABLE_H
#define SWITCHABLE_H

#include "Item.h"
#include <typeinfo>

class SwitchableItem: public Item{

	private:
		bool on;

	public:
		SwitchableItem(uint64_t cd, int stat, string longn, string fulln, string writ, int sz): Item(cd, stat, longn, fulln, writ, sz){
			this->code = cd;
			this->status = stat;
			this->longname = longn;
			this->fullname = fulln;
			this->writing = writ;
			this->size = sz;
			this->on = false; // On initialisation, item is off
		}; // This will not compile if in a separate implementation file (needs to be able to see constructor)
		~SwitchableItem();
		bool isOn();
		void setOn(bool b);
		virtual string getLongname();
		virtual string getInventoryname();
		virtual string getFullname();
		
};

#endif