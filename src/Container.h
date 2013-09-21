/*
 *	class representing items that can contain other items
 */
#ifndef CONTAINER_H
#define CONTAINER_H

#include "Item.h"
#include <typeinfo>

class Container: public Item{

	private:
		Item* within;
		string getInventoryname(int tabcount);

	public:
		Container(uint64_t cd, int stat, string longn, string fulln, string writ, int sz): Item(cd, stat, longn, fulln, writ, sz){
			this->code = cd;
			this->status = stat;
			this->longname = longn;
			this->fullname = fulln;
			this->writing = writ;
			this->size = sz;
			this->within = NULL; // On initialisation, there is nothing in the container
		}; // This will not compile if in a separate implementation file (needs to be able to see constructor)
		~Container();
		void insertItem(Item* item);
		Item* getItemWithin();
		Item* extractItemWithin();
		bool contains(Item* item); // Check whether this container, or any container within it, contains a certain item
		bool containsWithAttribute(int attribute); // Check whether this container, or any container within it, contains any item with a certain attribute
		bool isEmpty(); // Whether there is anything in the container or not
		bool isSuitable(Item* item); // Whether this container is a suitable container for a certain item
		Container* getParentOf(Item* item); // Return the container, or container within this container, that contains a certain item
		virtual int getWeight(); // The weight of this container together with the weight of whatever is in it
		virtual string getLongname();
		virtual string getInventoryname();
		virtual string getFullname();

};

#endif