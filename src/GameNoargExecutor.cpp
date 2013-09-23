#include "Game.h"

/*
 *	Constructor
 */
Game::NoargExecutor::NoargExecutor(Game* game){
	this->game = game;
}

/*
 *	Execute ABRACADA magic word command
 */
void Game::NoargExecutor::execAbracada(){
	Terminal::wrpro(game->general->get(STR_NOHAPPEN));
}

/*
 *	Execute AVNARAND magic word command
 */
void Game::NoargExecutor::execAvnarand(){
	Location* current = game->player->getLocation();
	Item* obst = current->get(ITEM_ROBOT);
	if(current->getID() == LOCATION_CHECKPOINT && obst != NULL){
		current->extract(obst); // Remove obstruction from location
		obst->setLocation(game->station->get(LOCATION_NOWHERE)); // Place item nowhere
		Terminal::wrpro(game->general->get(STR_ROBOT));
		game->player->incrementScore(SCORE_PUZZLE);
	}
	else
		Terminal::wrpro(game->general->get(STR_NOHAPPEN));
}

/*
 *	Execute CHIMBU magic word command
 */
void Game::NoargExecutor::execChimbu(){
	Location* current = game->player->getLocation();
	Item* fairy = current->get(ITEM_FAIRY);
	if(fairy != NULL){
		Container* envelope = (Container*) game->items->get(ITEM_ENVELOPE);
		Item* within = envelope->getItemWithin();

		if(within != NULL && within->getCode() == ITEM_TOOTH){ // If the player left a tooth in the envelope

			current->extract(fairy);
			fairy->setLocation(game->station->get(LOCATION_NOWHERE)); // Fairy vanishes

			within->setLocation(LOCATION_NOWHERE);
			Item* coin = game->items->get(ITEM_COIN);
			envelope->insertItem(coin);
			coin->setLocation(game->station->get(LOCATION_CONTAINER)); // Replace dragon's tooth with asterium coin

			Terminal::wrpro(game->general->get(STR_FAIRY));

			game->player->incrementScore(SCORE_PUZZLE);
		}
		else
			Terminal::wrpro(game->general->get(STR_NOHAPPEN));
	}
	else
		Terminal::wrpro(game->general->get(STR_NOHAPPEN));

}

/*
 *	Execute command to list commands
 */
void Game::NoargExecutor::execCommands(){
	Terminal::wrpro(game->commands->listNonSecret());
}

/*
 *	Cool story, bro
 */
void Game::NoargExecutor::execCsb(){
	Terminal::wrpro(game->general->get(STR_IKR));
}

/*
 *	Execute command to fish for something
 */
void Game::NoargExecutor::execFish(){

	if(!game->player->hasInInventory(game->items->get(ITEM_NET))){ // Player does not have a fishing net
		Terminal::wrpro(game->general->get(STR_NOEQUIP));
		return;
	}

	Item* glint = game->items->get(ITEM_GLINT);
	if(glint->getLocation() != game->player->getLocation()){ // No "glint in the water" at this location (this is the only thing one would be fishing for)
		Terminal::wrpro(game->general->get(STR_NOFISH));
		return;
	}

	Item* nugget = game->items->get(ITEM_NUGGET);
	if(!game->player->canAccept(nugget)){ // No room in inventory
		Terminal::wrpro("The item glinting down there is larger than you can carry right now. You will have to drop something.");
		return;
	}
	glint->getLocation()->extract(glint);
	glint->setLocation(game->station->get(LOCATION_NOWHERE)); // Remove the glint the player is seeing

	game->player->addToInventory(nugget);
	nugget->setLocation(game->station->get(LOCATION_INVENTORY)); // Give nugget to player

	Terminal::wrpro(game->general->get(STR_NUGGGOOD));
	game->player->incrementScore(SCORE_PUZZLE);	

}

/*
 *	Execute command to give basic help to user
 */
void Game::NoargExecutor::execHelp(){
	Terminal::wrpro(game->general->get(STR_HELP));
}

/*
 *	Execute command to list contents of inventory
 */
void Game::NoargExecutor::execInventor(){
	Terminal::wrpro(game->player->inventoryToString());
}

/*
 *	Execute command to knit
 */
void Game::NoargExecutor::execKnit(){
	Item* needles = game->items->get(ITEM_NEEDLES);
	Item* yarn = game->items->get(ITEM_YARN);
	if(!game->player->hasInInventory(needles) || !game->player->hasInInventory(yarn)){
		Terminal::wrpro(game->general->get(STR_NOEQUIP));
		return;
	}

	game->destroyItem(yarn); // All yarn is consumed
	Location* loc = game->player->getLocation();
	Item* jumper = game->items->get(ITEM_JUMPER);
	loc->deposit(jumper); // Jumper now appears instead
	jumper->setLocation(loc);
	game->player->incrementScore(SCORE_PUZZLE);
	Terminal::wrpro(game->general->get(STR_KNITJUMP));
}

/*
 *	Execute command to describe surroundings to user
 */
void Game::NoargExecutor::execLook(){
	Terminal::wrpro(game->player->getLocationAll());
}

/*
 *	Execute PLUGH magic word command
 */
void Game::NoargExecutor::execPlugh(){
	Terminal::wrpro(game->general->get(STR_PLUGH));
}

/*
 *	Execute command to end game
 */
void Game::NoargExecutor::execQuit(){
	if(game->confirm(game->general->get(STR_ASKSURE))){
		execScore(true);
		game->on = false;
	}
}

/*
 *	Execute command to display current score to player
 */
void Game::NoargExecutor::execScore(bool final){
	stringstream ss;
	if(final){
		ss << "Your final score was " << game->calculateScore() << " points of a possible " << game->maxpoints << ". ";
		ss << "You died " << game->player->getDeaths() << " times, and gave " << game->player->getMoves() << " instructions.";
	}
	else{
		ss << "Your current score is " << game->calculateScore() << " points.";
	}
	Terminal::wrpro(ss.str());
}

/*
 *	Execute command for player to go into/out of 'sleep' sequence
 */
void Game::NoargExecutor::execSleep(){

	int loccode = game->player->getLocation()->getID();

	if(loccode != LOCATION_SLEEP_0 && loccode != LOCATION_SLEEP_2){
		Terminal::wrpro(game->general->get(STR_SLEEPNO));
		return;
	}

	Terminal::wrpro(game->general->get(STR_SLEEP));
	
	if(loccode == LOCATION_SLEEP_0){
		game->player->dropAll(false); // Drop items at this location, temporarily
		game->player->setLocation(game->station->get(LOCATION_SLEEP_1));
	}

	else if(loccode == LOCATION_SLEEP_2){
		game->player->dropAll(true); // Drop items at this location, permanently; we wouldn't be returning to this point
		game->player->setLocation(game->station->get(LOCATION_SLEEP_0));
		game->player->collectTemporary(game->station->get(LOCATION_INVENTORY));
	}

}

/*
 *	Execute TEZAZZLE magic word command (teleport)
 */
void Game::NoargExecutor::execTezazzle(){
	int loccode = game->player->getLocation()->getID();

	if(loccode != LOCATION_TELEPORT_0 && loccode != LOCATION_TELEPORT_1){
		Terminal::wrpro(game->general->get(STR_NOHAPPEN));
		return;
	}

	Terminal::wrpro(game->general->get(STR_TELEPORT));
	game->player->dropAll(true); // Drop items at this location, permanently

	if(loccode == LOCATION_TELEPORT_0)
		game->player->setLocation(game->station->get(LOCATION_TELEPORT_1));

	else if(loccode == LOCATION_TELEPORT_1)
		game->player->setLocation(game->station->get(LOCATION_TELEPORT_0));

	game->forgetReturnLocation();

	if(game->player->hasLight())
		Terminal::wrpro(game->player->getLocationArrival());
	else
		Terminal::wrpro(game->general->get(STR_CANTSEE));

}

/*
 *	Execute command to display current game version to player
 */
void Game::NoargExecutor::execVersion(){
	stringstream ss;
	ss << game->general->get(STR_VERSION) << " (" << game->maxpoints << " points).";
	Terminal::wrpro(ss.str());
}

/*
 *	Execute XYRO magic word command
 */
void Game::NoargExecutor::execXyro(){
	Item* wizard = game->player->getLocation()->get(ITEM_WIZARD);
	if(wizard){
		if(game->player->isInvisible()){ // Wizard cannot kill you if you are invisible, but you still cannot get past him
			Terminal::wrpro(game->general->get(STR_WIZINVIS));
		}
		else if(game->player->hasInInventory(game->items->get(ITEM_MIRROR))){ // Wizard attacks you but player reflects attack
			Terminal::wrpro(game->general->get(STR_WIZMIRRO));
			game->player->getLocation()->extract(wizard);
			wizard->setLocation(LOCATION_NOWHERE);
			game->player->incrementScore(SCORE_PUZZLE);
		}
		else{ // Wizard can see you
			Terminal::wrpro(game->general->get(STR_WIZARDED));
			game->player->kill();
		}
	}
	else
		Terminal::wrpro(game->general->get(STR_SHMAGIC));
}

/*
 *	Execute XYZZY magic word command
 */
void Game::NoargExecutor::execXyzzy(){
	Terminal::wrpro(game->general->get(STR_OK));
}

/*
 *	Execute ZIQUA magic word command
 */
void Game::NoargExecutor::execZiqua(){
	Location* loc = game->player->getLocation();
	if(loc->getID() == LOCATION_TREETOP){
		Item* acorn = game->items->get(ITEM_ACORN);
		if(acorn->getLocation() == game->station->get(LOCATION_NOWHERE)){ // Do not perform this again if you have already done it
			Location* garden = game->station->get(LOCATION_GARDEN);
			garden->deposit(acorn);
			acorn->setLocation(garden);
			game->player->incrementScore(SCORE_PUZZLE);
			Terminal::wrpro(game->general->get(STR_ZIQUA));
		}
		else
			Terminal::wrpro(game->general->get(STR_NOHAPPEN));		
	}
	else
		Terminal::wrpro(game->general->get(STR_NOHAPPEN));
}
