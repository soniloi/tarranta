#include "Game.h"

/*
 *	Constructor
 */
Game::TestingExecutor::TestingExecutor(Game* game){
	this->game = game;
}

/*
 *	Testing methods (conditionally-compiled only)
 */

#if defined (TESTING_READ)
/*
 *	Execute command to tell player current location of specified item
 */
void Game::TestingExecutor::execAimsigh(uint64_t arg){
	Item* item = game->items->get(arg);
	if(item == game->items->get(ITEM_NULL)){ // Whatever they input as argument isn't anything we know as an item
		Terminal::wrpro("I do not know what " + Statics::codeToStr(arg) + " is.");
		return;
	}
	Location* loc = item->getLocation();
	string nodeid = game->itos(loc->getID());
	Terminal::wrpro("The " + item->getShortname() + " says it is at node: " + nodeid + ", \"" + loc->getLongname() + "\".");
	if(loc->contains(item))
		Terminal::wrpro("Node " + nodeid + " confirms that the " + item->getShortname() + " is present.");
	else
		Terminal::wrpro("However, node " + nodeid + " says that it has no " + item->getShortname() + ".");
}

/*
 *	Execute command to echo player's current location to terminal
 */
void Game::TestingExecutor::execNode(){
	Location* loc = game->player->getLocation();
	Location* prevloc = loc->getDirection(CMD_BACK);
	Terminal::wrpro("You are currently at node: " + game->itos(loc->getID()) + ", \"" + loc->getLongname() + "\".");
	Terminal::wrpro("Your previous location was node: " + game->itos(prevloc->getID()) + ", \"" + prevloc->getLongname() + "\".");
}
#endif

#if defined (TESTING_READ) && defined (TESTING_WRITE)
/*
 *	Execute command to move player to any desired location in game
 */
void Game::TestingExecutor::execFlash(uint64_t arg){
	int locid;
	Statics::strToInt(locid, Statics::codeToStr(arg));

	if((unsigned int) locid >= game->station->getLocationCount()){
		Terminal::wrpro("There are not that many locations on the station.");
		return;
	}

	Terminal::wrpro("Proceeding to specified node.");
	game->player->setLocation(game->station->get(locid));
}

/*
 *	Execute command to move any item in game into player's inventory
 */
void Game::TestingExecutor::execGrab(uint64_t arg){
	Item* item = game->items->get(arg);
	if(item == game->items->get(ITEM_NULL)){ // Whatever they input as argument isn't anything we know as an item
		Terminal::wrpro("I do not know what " + Statics::codeToStr(arg) + " is.");
		return;
	}
	if(game->player->hasInInventory(item)){ // Already in inventory
		Terminal::wrpro("You already have it.");
		return;
	}

	Location* itemloc = item->getLocation();
	if(itemloc != NULL)
		itemloc->extract(item);
	item->setLocation(game->station->get(LOCATION_INVENTORY));
	game->player->addToInventory(item);
	Terminal::wrpro("Taken.");

}
#endif
