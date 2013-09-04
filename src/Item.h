/*
 *	Class representing items that may be manipulated
 */
#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>
#include <inttypes.h>
//#include "Location.h" // For the love of god avoid having to #include this, it's far more trouble than it's worth; stick to forward declarations
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
		uint64_t getCode(); // I think I'll need this as I'm reluctant to have this class give friendship to Location
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
