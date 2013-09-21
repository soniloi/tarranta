/*
 *	Class representing a game player
 */
#ifndef PLAYER_H
#define PLAYER_H

#include "Location.h"
#include "Inventory.h"

class Player{

	private:
		int score;
		int moves; // Number of moves player has made
		bool alive;
		int deaths; // Number of times player died
		bool strong;
		Location* location;
		Inventory* inventory;
		Location* wakeLocation;
		Location* safeLocation;

	public:
		Player(Location* initial, Location* safeloc);
		~Player();
		int getScore();
		int getMoves();
		bool isAlive();
		int getDeaths();
		bool isStrong();
		bool isInvisible();
		void setStrong(bool str);
		void setWakeLocation(Location* wakeloc);
		void setSafeLocation(Location* safeloc);
		void kill();
		void reincarnate();
		void dropAll(bool permanent);
		void collectTemporary(Location* invent);
		Location* getLocation();
		Location* getNextLocation(uint64_t dircode);
		void incrementScore(int num);
		void incrementMoves();
		void setLocation(Location* loc);
		bool hasInPresent(Item* item);
		bool hasInInventory(Item* item);
		bool hasInInventoryWithAttribute(int attribute);
		void addToInventory(Item* item);
		void extractFromInventory(Item* item);
		Container* getParentContainer(Item* item);
		list<Container*> getSuitableContainers(Item* item);
		list<Container*> getSuitableInventoryContainers(Item* item);
		list<Container*> getFullLiquidInventoryContainers();
		string getLocationStub();
		string getLocationArrival();
		string getLocationAll();
		string inventoryToString();
		bool hasLight();
		bool hasAir();
		bool hasGravity();
		bool hasNosnomp();
		
};

#endif
