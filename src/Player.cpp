#include "Player.h"

/*
 *	Constructor
 */
Player::Player(Location* initial, Location* safeloc){
	this->score = 0;
	this->moves = 0;
	this->alive = true;
	this->deaths = 0;
	this->strong = false;
	this->location = initial;
	this->inventory = new Inventory(INVENTORY_CAPACITY);
	this->wakeLocation = initial;
	this->safeLocation = safeloc;
}

/*
 *	Destructor
 */
Player::~Player(){
	delete this->inventory;
}

/*
 *	Assorted public accesors
 */
int Player::getScore(){
	return this->score;
}

int Player::getMoves(){
	return this->moves;
}

bool Player::isAlive(){
	return this->alive;
}

int Player::getDeaths(){
	return this->deaths;
}

bool Player::isStrong(){
	return this->strong;
}

bool Player::isInvisible(){
	return this->hasInInventoryWithAttribute(CTRL_ITEM_GIVES_INVISIBILITY);
}

void Player::setStrong(bool str){
	this->strong = str;
}

void Player::setWakeLocation(Location* wakeloc){
	this->wakeLocation = wakeloc;
}

void Player::setSafeLocation(Location* safeloc){
	this->safeLocation = safeloc;
}

/*
 *	Kill player, leaving all their essential items at a safe location
 */
void Player::kill(){
	this->alive = false; // Set player's status to dead
	this->deaths++; // Increment the number of deaths player has experienced

	map<uint64_t, Item*>::iterator it = this->inventory->items.begin();
	while(it != this->inventory->items.end()){

		if(it->second->hasAttribute(CTRL_ITEM_KEEP)){ // Essential items get left within reach
			it->second->setLocation(this->safeLocation);
			this->safeLocation->deposit(it->second);
		}
		else{ // Non-essential items get left where the player died
			it->second->setLocation(this->location);
			this->location->deposit(it->second);
		}

		map<uint64_t, Item*>::iterator jt = it; // Otherwise the pointer will be broken
		jt++;
		this->inventory->items.erase(it); // Either way, remove it from the inventory
		it = jt;

	}
}

/*
 *	Reincarnate player at a particular return location
 */
void Player::reincarnate(){
	this->alive = true;
	this->location = this->wakeLocation;
}

/*
 *	Drop all items at player's current location
 *	The temporary boolean variable states whether this is to be a temporary drop or a permanent one
 *		Temporary drop means the dropped items' locations are not updated. This has the effect
 *		that they are essentially tagged for later re-collection; if not permanent, this tag is lost
 */
void Player::dropAll(bool permanent){

	map<uint64_t, Item*>::iterator it = this->inventory->items.begin();
	while(it != this->inventory->items.end()){

		if(permanent)
			it->second->setLocation(this->location);
		this->location->deposit(it->second);

		map<uint64_t, Item*>::iterator jt = it; // Otherwise the pointer will be broken
		jt++;
		this->inventory->items.erase(it); // Either way, remove it from the inventory
		it = jt;

	}

}

/*
 *	Bring temporarily-dropped items back into the inventory
 *	(Read description for dropAll)
 */
void Player::collectTemporary(Location* invent){

	map<uint64_t, Item*>::iterator it = this->location->items.begin();
	while(it != this->location->items.end()){
		if(it->second->getLocation() == invent){
			this->inventory->items[it->first] = it->second; // Place item back in inventory
			map<uint64_t, Item*>::iterator jt = it; // Otherwise the pointer will be broken
			jt++;
			this->location->items.erase(it); // Remove item from location
			it = jt;
		}
		else
			it++;

	}	
}

Location* Player::getLocation(){
	return this->location;
}

Location* Player::getNextLocation(uint64_t dircode){
	return this->location->getDirection(dircode);
}

/*
 *	Add num to player's score
 */
void Player::incrementScore(int num){
	this->score += num;
}

void Player::incrementMoves(){
	this->moves++;
}

void Player::setLocation(Location* loc){
	this->location = loc;
}

bool Player::hasInPresent(Item* item){
	if(this->inventory->contains(item))
		return true;
	if(this->location->get(item->getCode()))
		return true;
	return false;
}

/*
 *	Return whether item can fit in inventory
 */
bool Player::canAccept(Item* item){
	return this->inventory->canAccept(item);
}

bool Player::hasInInventory(Item* item){
	return this->inventory->contains(item);
}

/*
 *	Check whether player is carrying any item with a certain attribute
 */
bool Player::hasInInventoryWithAttribute(int attribute){
	return this->inventory->containsWithAttribute(attribute);
}

/*
 *	Deposit an item into inventory
 */
void Player::addToInventory(Item* item){
	this->inventory->deposit(item);
}

/*
 *	Remove an item from inventory
 */
void Player::extractFromInventory(Item* item){
	this->inventory->extract(item->getCode());

	if(item->getLocation()->getID() == LOCATION_CONTAINER){ // Deal with nasty stuff
		Container* parent = this->getParentContainer(item);
		if(parent){ // Just in case it somehow returns NULL, I don't want a big ugly segfault
			parent->extractItemWithin();
		}
	}

	else
		this->inventory->extract(item->getCode());
}

/*
 *	Find the container that contains a certain item
 *	In theory, could return NULL, but ideally this should not be called unless certain that it will not return NULL
 */
Container* Player::getParentContainer(Item* item){

	Container* container;

	container = this->inventory->getParentContainer(item);
	if(container)
		return container;

	container = this->location->getParentContainer(item);
	if(container)
		return container;

	return NULL; // NEVER let the program get into a position where this is returned
}

/*
 *	Create and return a list of containers suitable for containing item
 *	Does not return containers that are too small, containers that are actually item,
 *		liquid containers for solid items, or solid containers for liquid items
 */
list<Container*> Player::getSuitableContainers(Item* item){
	list<Container*> containers = this->location->getSuitableContainers(item); // Get list of all suitable containers at this location
	list<Container*> inventcontainers = this->inventory->getSuitableContainers(item); // Get list of all suitable containers in inventory
	containers.splice(containers.end(), inventcontainers); // Merge both lists
	return containers;
}

/*
 *	Return a list of containers suitable for containing item
 *	Must come from inventory alone
 */
list<Container*> Player::getSuitableInventoryContainers(Item* item){
	return this->inventory->getSuitableContainers(item);
}

/*
 *	Return a list of liquid containers that currently have liquid in them
 */
list<Container*> Player::getFullLiquidInventoryContainers(){
	return this->inventory->getFullLiquidContainers();
}

string Player::getLocationStub(){
	return this->location->getStub();
}

string Player::getLocationArrival(){
	if(this->inventory->hasLight() && this->location->hasAttribute(CTRL_LOC_NEEDSNO_LIGHT))
		return "I cannot see a thing through this haze.";
	else if(!this->hasLight())
		return "I cannot see a thing through the darkness.";
	return this->location->getArrival();
}

string Player::getLocationAll(){
	if(this->inventory->hasLight() && this->location->hasAttribute(CTRL_LOC_NEEDSNO_LIGHT))
		return "I cannot see a thing through this haze.";
	else if(!this->hasLight())
		return "I cannot see a thing through the darkness.";
	return this->location->getAll();
}

string Player::inventoryToString(){
	return this->inventory->toString();
}

bool Player::hasLight(){
	return (this->location->hasLight() || this->inventory->hasLight()); // Do *not* change this to hasInInventoryWithAttribute (need to check if switched on)
}

bool Player::hasAir(){
	return (this->location->hasAir() || this->hasInInventoryWithAttribute(CTRL_ITEM_GIVES_AIR));
}

bool Player::hasGravity(){
	return (this->location->hasGravity() || this->hasInInventoryWithAttribute(CTRL_ITEM_GIVES_GRAVITY));
}

bool Player::hasNosnomp(){
	return (this->location->hasNosnomp() || this->hasInInventoryWithAttribute(CTRL_ITEM_GIVES_NOSNOMP));
}

bool Player::hasLand(){
	return (this->location->hasLand() || this->hasInInventoryWithAttribute(CTRL_ITEM_GIVES_LAND));
}