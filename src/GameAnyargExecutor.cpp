#include "Game.h"

/*
 *	Constructor
 */
Game::AnyargExecutor::AnyargExecutor(Game* game){
	this->game = game;
}

/*
 *	Execute command to call/summon something
 */
void Game::AnyargExecutor::execCall(uint64_t arg){
	if(game->player->getLocation()->getID() == LOCATION_COMMS && arg == STR_SHIP){

		if(game->items->get(STR_SHIP)->getLocation()->getID() != LOCATION_NOWHERE)
			Terminal::wrpro(game->general->get(STR_SHIPALRE));

		else{
			// Change player's start & safe locations on death; they must now be west of the checkpoint rather than east
			game->player->setWakeLocation(game->station->get(LOCATION_START_1));
			game->player->setSafeLocation(game->station->get(LOCATION_SAFE_1));

			Location* loc;
			Location* loc1;
			Item* item;

			// Insert the bloodthirsty corsair at the checkpoint
			loc = game->station->get(LOCATION_CHECKPOINT);
			item = game->items->get(ITEM_CORSAIR);
			loc->deposit(item);
			item->setLocation(loc);

			// Insert the bloodthirsty buccaneer at the docking control area
			loc = game->station->get(LOCATION_DOCKINGCONTROL);
			item = game->items->get(ITEM_BUCCANEE);
			loc->deposit(item);
			item->setLocation(loc);

			// Insert the pirate ship (Item) into the docking bay
			loc = game->station->get(LOCATION_DOCKING);
			item = game->items->get(ITEM_SHIP);
			loc->deposit(item);
			item->setLocation(loc);

			// Link the pirate ship (Location) to the docking bay
			loc = game->station->get(LOCATION_DOCKING);
			loc1 = game->station->get(LOCATION_SHIP);
			loc->setDirection(CMD_EAST, loc1);
			loc->setDirection(CMD_SOUTHEAS, loc1);

			// Unlink the shuttle from the southeast of the docking bay
			loc = game->station->get(LOCATION_SHUTTLE);
			loc->setDirection(CMD_SOUTH, game->station->get(LOCATION_NOWHERE));

			Terminal::wrpro(game->general->get(STR_DISTRESS));
			game->player->incrementScore(SCORE_PUZZLE);
		}
	}
	else
		Terminal::wrpro(game->general->get(STR_NONOHOW));

}

/*
 *	Execute command to explain something to player
 */
void Game::AnyargExecutor::execExplain(uint64_t arg){
	string answer = game->explanations->get(arg);
	if(answer.empty())
		answer = game->explanations->get(HINT_DEFAULT);
	Terminal::wrpro(answer);
}

/*
 *	Execute command to fly
 */
void Game::AnyargExecutor::execFly(uint64_t arg){
	if(arg == ITEM_SHIP){
		if(game->player->getLocation()->getID() == LOCATION_SHIP){
			Terminal::wrpro("You steal the pirate ship and fly away in it. Congratulations, you have successfully escaped from Asterbase Tarranta.");
			game->player->incrementScore(SCORE_ESCAPE);
			game->on = false;
			game->escaped = true;
			game->noargexec->execScore(true);
		}
		else
			Terminal::wrpro("You are not in any ship that is capable of flying.");
	}
	else
		Terminal::wrpro(game->general->get(STR_NONOHOW));
}

/*
 *	Execute command to display hint
 */
void Game::AnyargExecutor::execHint(uint64_t arg){
	string hint = game->hints->get(arg);
	if(hint.empty())
		Terminal::wrpro(game->hints->get(HINT_DEFAULT));
	else{
		Terminal::wrpro("There is something more I can tell you about this, but you will have to accept a point penalty.");
		vector<uint64_t> confirm = Terminal::readCodes("Is this all right? ");
		if(confirm.size() == ONE && (confirm[0] == STR_Y || confirm[0] == STR_YES)){
			Terminal::wrpro(hint);
			game->hints->clear(arg);
			game->player->incrementScore(PENALTY_HINT);
		}
		else
			Terminal::wrpro(game->general->get(STR_OK));
	}
}

/*
 *	Execute command to say something
 */
void Game::AnyargExecutor::execSay(uint64_t arg){
	string statement = Statics::codeToStr(arg);
	Terminal::wrpro("You say \"" + statement + "\" aloud.");
	if(game->player->hasInPresent(game->items->get(ITEM_CORSAIR))){ // Blind corsair will hear player and kill them
		Terminal::wrpro(game->general->get(STR_PIRATKIL));
		game->player->kill();
	}
}

/*
 *	Execute command to tether something to something else
 */
void Game::AnyargExecutor::execTether(uint64_t arg){
	if(arg == ITEM_SHIP || arg == ITEM_SHUTTLE){

		Item* item = game->items->get(arg);
		Item* itemship = game->items->get(ITEM_SHIP);
		Item* itemshuttle = game->items->get(ITEM_SHUTTLE);
		Item* cable = game->items->get(ITEM_CABLE);
		Location* dock = game->station->get(LOCATION_DOCKING);
		int loccode = game->player->getLocation()->getID();

		if(!game->player->hasInPresent(item) && loccode != LOCATION_SHIP && loccode != LOCATION_SHUTTLE){
			Terminal::wrpro("I see no " + Statics::codeToStr(arg) + " here to tether."); // Player is nowhere near either ship or shuttle
		}
		else if(arg == ITEM_SHUTTLE && (game->player->hasInPresent(itemshuttle) || loccode == LOCATION_SHIP || loccode == LOCATION_SHUTTLE) && !dock->get(ITEM_SHIP)){
			Terminal::wrpro("I see nothing here to tether it to."); // Player wishes to tether shuttle, but ship is not nearby
		}
		else if(arg == ITEM_SHIP && (game->player->hasInPresent(itemship) || loccode == LOCATION_SHIP || loccode == LOCATION_SHUTTLE) && !dock->get(ITEM_SHIP)){
			Terminal::wrpro("I see no ship here to tether."); // Player wishes to tether ship, but ship is not in dock
		}
		else if(!game->player->hasInInventory(cable)){
			Terminal::wrpro("You have nothing you can use to tether the " + Statics::codeToStr(arg) + "."); // Player has no cable to use to tether
		}		
		else{
			Location* ship = game->station->get(LOCATION_SHIP);
			Location* shuttle = game->station->get(LOCATION_SHUTTLE);

			ship->setDirection(CMD_NORTH, shuttle);
			shuttle->setDirection(CMD_SOUTH, ship);
			game->destroyItem(cable);
			game->player->incrementScore(SCORE_PUZZLE);
			Terminal::wrpro(game->general->get(STR_TETHER));
		}
	}
	else
		Terminal::wrpro(game->general->get(STR_NONOHOW));
}
