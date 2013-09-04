#include "Player.h"

/*
 *	Constructor
 */
Player::Player(Location* initial, Location* safeloc){
	this->score = 0;
	this->moves = 0;
	this->alive = true;
	this->deaths = 0;
	this->strength = 0;
	this->invisibility = 0;
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

int Player::getStrength(){
	return this->strength;
}

int Player::getInvisibility(){
	return this->invisibility;
}

void Player::setStrength(int stren){
	this->strength = stren;
}

void Player::setInvisibilty(int invis){
	this->invisibility = invis;
}

void Player::decrementStrength(){
	this->strength--;
}

void Player::decrementInvisibility(){
	this->invisibility--;
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

bool Player::hasInInventory(uint64_t itemcode){
	return this->inventory->contains(itemcode);
}

/*
 *	Check whether player is carrying any item with a certain attribute
 */
bool Player::hasInInventoryWithAttribute(int attribute){
	for(map<uint64_t, Item*>::iterator it = this->inventory->items.begin() ; it != this->inventory->items.end() ; it++){
		if(it->second->hasAttribute(attribute))
			return true;
	}
	return false;
}

void Player::addToInventory(Item* item){
	this->inventory->deposit(item);
}

void Player::extractFromInventory(uint64_t itemcode){
	this->inventory->extract(itemcode);
}

list<Container*> Player::getInventoryContainers(){
	return this->inventory->getContainers();
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
	return (this->location->hasLight() || this->inventory->hasLight());
}

bool Player::hasAir(){
	return (this->location->hasAir() || this->inventory->hasAir());
}

bool Player::hasGravity(){
	return (this->location->hasGravity() || this->inventory->hasGravity());
}

bool Player::hasNosnomp(){
	return (this->location->hasNosnomp() || this->inventory->hasNosnomp());
}
