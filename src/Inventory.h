/*
 *	Class representing the player's inventory
 */

#include <map>
#include <list>
//#include "Item.h"
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
 		Item* get(uint64_t cd); // Return pointer to desired Item without removing from inventory
 		Item* extract(uint64_t cd); // Return pointer to desired Item and remove from inventory
 		bool contains(uint64_t cd);
 		bool hasLight();
		bool hasAir();
		bool hasGravity();
		bool hasNosnomp();
 		string toString();
 		list<Container*> getSuitableContainers(Item* item);

};