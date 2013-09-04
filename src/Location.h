/*
 *	Class representing a location in the game
 */

#ifndef LOCATION_H
#define LOCATION_H

#include <map>
#include <list>
#include <string>
#include "Statics.h"
#include "Definitions.h"
#include "Item.h"
#include "SwitchableItem.h"
#include "Container.h" // Comment this out if possible; it causes circular dependency issues if not handled properly

using namespace std;

class Item; // Forward declaration of Item class
class Container;

class Location{

	private:
		Location* nowhere; // This really should be static but I cannot think of a nice way of doing it
		map<uint64_t, Location*> directions;
		int id;
		int status;
		string shortname;
		string longname;
		string fullname;
		map<uint64_t, Item*> fixtures;
		map<uint64_t, Item*> items;
		void printItemListDescription();
		string getBasic();
		string itemListToString(map<uint64_t, Item*> things, bool obstructiononly);
		Location* getOnlyDirection();
		void determineOut();

	public:
		friend class Station;
		friend class Player;
		Location(int i, int stat, string shortn, string longn, string fulln);
		~Location();
		int getID();
		void crossReference(Location* n, Location* s, Location* e, Location* w, Location* ne, Location* sw, Location* se, Location* nw, Location* u, Location* d, Location* nowh);
		void setDirection(uint64_t dir, Location* loc);
		int countTreasures();
		bool hasAttribute(int attribute);
		void setAttribute(int attribute, bool set);
		bool hasLight();
		bool hasAir();
		bool hasGravity();
		bool hasNosnomp();
		string getLongname();
		string getStub();
		string getArrival();
		string getAll();
		void fix(Item* item);
		void deposit(Item* item);
		Location* getDirection(uint64_t dircode);
		Item* get(uint64_t cd);
		Item* extract(uint64_t cd);
		Item* getObstruction();
		list<Container*> getSuitableContainers(Item* item);

};

#endif