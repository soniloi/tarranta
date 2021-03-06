/*
 *	Class representing the player's inventory
 */

#include <map>
#include <list>
#include "Container.h"
#include "SwitchableItem.h"
#include "Terminal.h"
#include "Statics.h"

class Inventory{

 	private:
 		int capacity;
 		map<uint64_t, Item*> items;

 	public:
 		friend class Player;
 		Inventory(int maxcapacity);
 		~Inventory();
 		void deposit(Item* item);
 		int getWeight(); // Return combined weight of all items in inventory
 		bool canAccept(Item* item); // Return true if taking item will not put weight over maxcapacity
 		Item* get(Item* item); // Return pointer to desired Item without removing from inventory
 		Item* extract(uint64_t cd); // Return pointer to desired Item and remove from inventory
 		bool contains(Item* item); // Return whether an item is contained in inventory
 		bool containsWithAttribute(int attribute); // Return whether any item with a certain attribute is contained in inventory
 		bool hasLight();
		bool hasAir();
		bool hasGravity();
		bool hasNosnomp();
 		string toString();
 		list<Container*> getSuitableContainers(Item* item);
 		list<Container*> getFullLiquidContainers();
 		Container* getParentContainer(Item* item); // Return the container that contains a certain item

};