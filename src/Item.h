/*
 *	Class representing items that may be manipulated
 */
#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>
#include <inttypes.h>
//#include "Location.h" // Avoid having to #include this if possible; leads to circular dependency issues
#include "Definitions.h"
#include "Statics.h"

using namespace std;

class Location; // Forward declaration of Location class

class Item{

	friend ostream& operator <<(ostream&, const Item&);

	protected:
		Location* location;
		uint64_t code;
		int status;
		string longname;
		string fullname;
		string writing;
		int size;

	public:
		friend class ItemCollection;
		friend class Inventory;
		Item(uint64_t cd, int stat, string longn, string fulln, string writ, int sz);
		virtual ~Item() {}; // Default destructor, nothing to see here
		uint64_t getCode();
		Location* getLocation();
		bool isPresent(Location* current);
		bool hasAttribute(int attribute);
		void setLocation(Location* loc);
		string getShortname();
		int getSize();
		virtual int getWeight() {return this->size;};
		virtual string getLongname();
		virtual string getInventoryname();
		virtual string getFullname();
		string getWriting();
		void setAttribute(int attribute, bool b);
		
};

#endif
