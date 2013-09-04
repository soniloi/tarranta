/*
 *	class representing items that can be switched on and off
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
//			cerr << "Constructor: SwitchableItem\tName\t" << Statics::codeToStr(cd) << ", type is:\t" << typeid(*this).name() << "\tallocating: " << sizeof(*this) << endl << endl;
			this->code = cd;
			this->status = stat;
			this->longname = longn;
			this->fullname = fulln;
			this->writing = writ;
			this->size = sz;
			this->on = false; // On initialisation, item is off
		}; // This refused to compile if I put it in a separate implementation class (so don't); presumably something to do with inheritance
		~SwitchableItem();
		bool isOn();
		void setOn(bool b);
		virtual string getLongname();
		virtual string getInventoryname();
		virtual string getFullname();
		
};

#endif