#include "Game.h"

/*
 *	Constructor
 */
Game::InventoryargExecutor::InventoryargExecutor(Game* game){
	this->game = game;
}

/*
 *	Execute command to cook an item
 */
void Game::InventoryargExecutor::execCook(Item* item){
	if(!game->player->hasInPresent(game->items->get(ITEM_CAULDRON))){
		Terminal::wrpro(game->general->get(STR_NOCOOKER));
		return;
	}
	uint64_t code = item->getCode();
	switch(code){
		case ITEM_RADISHES:{
			game->destroyItem(item);
			Terminal::wrpro(game->general->get(STR_RADICOOK));
			game->player->getLocation()->deposit(game->items->get(ITEM_ELIXIR)); // Remember that we do not set liquid to point to its location
			game->player->incrementScore(SCORE_PUZZLE);
			break;
		}
		default:{
			Terminal::wrpro(game->general->get(STR_NONOHOW));
		}
	}
}

/*
 *	Execute command to drink a (liquid) item
 */
void Game::InventoryargExecutor::execDrink(Item* item){
	Terminal::wrpro(game->general->get(STR_DRINK));
	game->destroyItem(item);
	uint64_t code = item->getCode();
	switch(code){
		case ITEM_AQUA: {
			Terminal::wrpro(game->general->get(STR_DRINKAQU));
			break;
		}
		case ITEM_ELIXIR: {
			Terminal::wrpro(game->general->get(STR_DRINKELI));
			game->player->setStrong(true);
			break;
		}
		case ITEM_POTION: {
			Terminal::wrpro(game->general->get(STR_DRINKPOT));
			game->player->kill();
			break;
		}
		case ITEM_WATER: {
			Terminal::wrpro(game->general->get(STR_DRINKWAT));
			break;
		}
		default: {
			Terminal::wrpro(game->general->get(STR_NOHAPPEN));
		}
	}
}

/*
 *	Execute command to drop item into current location
 */
void Game::InventoryargExecutor::execDrop(Item* item){
	
	Location* current = game->player->getLocation();
	if(item->hasAttribute(CTRL_ITEM_LIQUID)){ // Liquids do not survive being dropped, regardless of gravity
		Container* container = game->player->getParentContainer(item);
		container->extractItemWithin(); // Empty the container that was holding it
		Terminal::wrpro("You pour out the " + item->getShortname() + ". It falls to the ground and quickly seeps away.");
		item->setLocation(game->station->get(LOCATION_NOWHERE));
	}
	else if(!current->hasAttribute(CTRL_LOC_HAS_FLOOR) && game->player->hasGravity()){ // Gravity pulls item down to location beneath current
		Location* below = current->getDirection(CMD_DOWN);
		if(below == game->station->get(LOCATION_NOWHERE)) // Error state; only way this could happen is error in datafile
			Terminal::wrpro(game->general->get(STR_ERROR));
		else{
			Terminal::wrpro(game->general->get(STR_NOFLOOR));
			if(item->hasAttribute(CTRL_ITEM_FRAGILE)){
				Terminal::wrpro(game->general->get(STR_SHATFALL));
				game->destroyItem(item);
			}
			else{
				below->deposit(item);
				game->player->extractFromInventory(item); // Remove item from inventory
				item->setLocation(below);
			}
		}
	}
	else if(item->hasAttribute(CTRL_ITEM_FRAGILE) && game->player->getLocation()->hasAttribute(CTRL_LOC_HAS_GRAVITY)){ // Fragile item breaks due to being dropped carelessly
		Terminal::wrpro(game->general->get(STR_SHATHERE));
		game->destroyItem(item);
	}
	else{
		Terminal::wrpro(game->general->get(STR_DROPGOOD));
		current->deposit(item); // Add item to current location's item list
		game->player->extractFromInventory(item); // Remove item from inventory
		item->setLocation(current); // Set item's location to current location
	}
}

/*
 *	Execute command to give Item item, and request that Item request be dispensed in return
 *	request is an item that is not the null item
 */
void Game::InventoryargExecutor::execExchange(Item* item, Item* request){

	if(item->getCode() == ITEM_CARTRIDG){ // Player is at machine
		int itemlocid = request->getLocation()->getID();

		if(!request->hasAttribute(CTRL_ITEM_FACTORY)) // Requested item is not a factory-made item
			Terminal::wrpro("The machine does not know how to create that.");
		else if(itemlocid != LOCATION_NOWHERE) // Requested item has already been made (is already in play)
			Terminal::wrpro("The machine has already dispensed that item and will not do so again for the time being.");
		else{ // Requested item will be created, and cartridge sent to some new location

			Location* newloc;
			uint64_t reqcode = request->getCode();

			Location* factory = game->station->get(LOCATION_FACTORY);

			if(reqcode == ITEM_LENS) // Player requests lens
				newloc = game->station->get(LOCATION_OBSERVATORY);
			else if(reqcode == ITEM_WIRE)
				newloc = game->station->get(LOCATION_SENSOR);
			else
				newloc = game->station->get(LOCATION_NOWHERE); // Default to prevent segmentation faults; do *not* allow this to get executed

			game->player->extractFromInventory(item); // Player drops cartridge; do *not* destroy item as we simply wish to reset its location
			newloc->deposit(item);
			item->setLocation(newloc);

			factory->deposit(request);
			request->setLocation(factory); // Place requested item in factory

			Terminal::wrpro("The machine accepts your cartridge and dispenses the " + Statics::codeToStr(reqcode) + ".");
			
		}
	}

	else if(request->getCode() == ITEM_BUILDING){ // Player is at farm
		Terminal::wrpro("You give the " + item->getShortname() + " in exchange.");
		Terminal::wrpro("Congratulations, you have bought the farm.");
		game->noargexec->execScore(true);
		game->on = false; // Game over
	}

}

/*
 *	Execute command to release an item from inventory
 */
void Game::InventoryargExecutor::execFree(Item* item){
	uint64_t code = item->getCode();
	execDrop(item);

	Location* current = game->player->getLocation();
	Item* wolf = current->get(ITEM_WOLF);

	if((code == ITEM_LION) && (wolf->getCode() != ITEM_NULL)){ // Lion will attack and drive off wolf if released in same location
		current->extract(wolf); // Remove wolf from location
		Terminal::wrpro(game->general->get(STR_LIONWOLF));
		game->player->incrementScore(SCORE_PUZZLE);
	}
}

/*
 *	Execute command to give this item to someone at this location
 */
void Game::InventoryargExecutor::execGive(Item* item, Item* receiver){
	uint64_t itemcode = item->getCode();
	uint64_t reccode = receiver->getCode();

	if(reccode == ITEM_ALIEN){ // Player wishes to give something to alien
		if(itemcode == ITEM_CHART){
			game->retireItem(item);
			game->player->incrementScore(SCORE_PUZZLE);
			Terminal::wrpro(game->general->get(STR_ALICHART));
		}
		else if(itemcode == ITEM_TRANSMIT){
			if(game->items->get(ITEM_CHART)->getLocation()->getID() != LOCATION_GRAVEYARD) // Player has not yet given chart
				Terminal::wrpro(game->general->get(STR_ALINOUSE));
			else{
				SwitchableItem* beacon = (SwitchableItem*) item;
				if(!beacon->isOn()) // Alien has no hands, so needs beacon switched on for it
					Terminal::wrpro(game->general->get(STR_ALINOUSE));
				else{
					game->retireItem(item);
					game->player->incrementScore(SCORE_PUZZLE);
					Terminal::wrpro(game->general->get(STR_ALIBEACO));
				}
			}
		}
		else if(itemcode == ITEM_LENS){
			if(game->items->get(ITEM_TRANSMIT)->getLocation()->getID() != LOCATION_GRAVEYARD) // Player has not yet given beacon (or chart)
				Terminal::wrpro(game->general->get(STR_ALINOUSE));
			else{
				game->retireItem(item);
				game->retireItem(receiver);
				Location* loc = game->player->getLocation();
				Item* pendant = game->items->get(ITEM_PENDANT);
				pendant->setLocation(loc);
				loc->deposit(pendant);
				game->player->incrementScore(SCORE_PUZZLE);
				Terminal::wrpro(game->general->get(STR_ALILENS));
			}
		}
		else
			Terminal::wrpro("The alien is uninterested in what you are offering.");
	}

	else if(reccode == ITEM_GUNSLING){ // Player wishes to give something to gunslinger
		if(itemcode == ITEM_MAGAZINE){ // Gunslinger is delighted with magazine
			Location* loc = game->player->getLocation();
			Item* cartridge = game->items->get(ITEM_CARTRIDG);
			loc->deposit(cartridge);
			cartridge->setLocation(loc);
			game->destroyItem(item);
			game->destroyItem(receiver);
			game->player->incrementScore(SCORE_PUZZLE);
			Terminal::wrpro(game->general->get(STR_GUNSLING));
		}
		else
			Terminal::wrpro("The gunslinger is uninterested in what you are offering.");
	}

	else if(reccode == ITEM_LION){ // Player wishes to feed lion
		if(itemcode == ITEM_KOHLRABI){
			Terminal::wrpro(game->general->get(STR_LIONKILL)); // Lion does not like cabbage and kills player
			game->player->extractFromInventory(item);
			item->setLocation(game->station->get(LOCATION_NOWHERE));
			game->player->kill();
		}
		else if(item->hasAttribute(CTRL_ITEM_EDIBLE)){
			Terminal::wrpro(game->general->get(STR_LIONWHET)); // Lion't appetite has been whetted by feeding
			game->player->extractFromInventory(item);
			item->setLocation(game->station->get(LOCATION_NOWHERE));
		}
		else
			Terminal::wrpro(game->general->get(STR_LIONYAWN)); // Lion is not interested
	}

	else if(reccode == ITEM_SKELETON){ // Player wishes to give something to skeleton
		if(itemcode == ITEM_MILK){
			Terminal::wrpro(game->general->get(STR_SKELMILK));
			game->destroyItem(item);
			game->destroyItem(receiver);
			Location* loc = game->player->getLocation();
			Item* brooch = game->items->get(ITEM_BROOCH);
			brooch->setLocation(loc);
			loc->deposit(brooch);
			game->player->incrementScore(SCORE_PUZZLE);
		}
		else
			Terminal::wrpro("The skeleton is uninterested in what you are offering.");
	}

	else if(reccode == ITEM_TROLL){ // Player wishes to feed troll
		if(item->hasAttribute(CTRL_ITEM_EDIBLE)){
			Terminal::wrpro(game->general->get(STR_TROLLED));
			game->player->extractFromInventory(item);
			item->setLocation(game->station->get(LOCATION_NOWHERE));
			game->player->kill();
		}
		else
			Terminal::wrpro(game->general->get(STR_TROLYAWN));
	}

	else
		Terminal::wrpro("The " + Statics::codeToStr(reccode) + " is not interested in your " + Statics::codeToStr(itemcode) + ".");
}

/*
 *	Execute command to insert item into a container
 */
void Game::InventoryargExecutor::execInsert(Item* item, Container* container){
	game->player->extractFromInventory(item); // Remove item from inventory
	item->setLocation(game->station->get(LOCATION_CONTAINER)); // Set item's location to "container"
	container->insertItem(item); // Insert item into desired container
	Terminal::wrpro(item->getShortname() + " inserted into " + container->getShortname() + ".");
}

/*
 *	Execute command to throw item
 */
void Game::InventoryargExecutor::execThrow(Item* item){
	Terminal::wrpro("You throw as hard as you can.");
	if(item->hasAttribute(CTRL_ITEM_FRAGILE)){
		Terminal::wrpro(game->general->get(STR_SHATTHRO));
		game->destroyItem(item);
	}
	else
		this->execDrop(item);
}