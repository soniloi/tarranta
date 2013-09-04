#include "Game.h"

/*
 *	Dispatch movement commands
 */
void Game::Dispatcher::dispatchMovement(Game* game, Command* command){

	Location* current = game->player->getLocation();
	Location* next = game->player->getNextLocation(command->getCode()); // Get location in desired direction

	if(next == NULL) // Technically an error state (direction code not recognised)
		Terminal::wrpro(game->general->get(STR_ERROR));

	else if(next == game->station->get(LOCATION_NOWHERE)){ // No location in desired direction
		if(command->is(CMD_BACK)) // game->player requested 'back', but cannot go back from here for whatever reason
			Terminal::wrpro("I do not remember how I got here. Please give a direction.");
		else if(command->is(CMD_OUT)) // game->player requested 'out', but out is ambiguous here
			Terminal::wrpro("I cannot tell in from out here. Please give a direction.");
		else
			Terminal::wrpro("You cannot go that way.");
	}

	else{ // Location exists in desired direction
		Item* ob = game->player->getLocation()->getObstruction(); // Search for obstructions at current location
		if(ob != NULL && next != game->player->getLocation()->getDirection(CMD_BACK)){ // Obstruction exists preventing game->player going any direction but the one they came from
			if(game->player->hasLight())
				Terminal::wrpro("The " + ob->getShortname() + " will not let you go that way.");
			else
				Terminal::wrpro("Some obstruction at this location will not let you go that way.");
		}
		else if(!next->hasAir() && !game->player->hasInInventoryWithAttribute(CTRL_ITEM_GIVES_AIR)){ // Refuse to move if there is no air
			Terminal::wrpro("There is no breathable atmosphere in that direction. I refuse to proceed.");
		}
		else if(command->is(CMD_UP) && game->player->hasGravity() && current->hasAttribute(CTRL_LOC_NEEDSNO_GRAVITY)){ // Gravity preventing player from going up
			Terminal::wrpro("You cannot currently reach the ceiling.");
		}
		else if(command->is(CMD_DOWN) && game->player->hasGravity() && !current->hasAttribute(CTRL_LOC_HAS_FLOOR)){ // Too far down
			Terminal::wrpro("Attempting such a downward journey would kill you.");
		}
		else{ // Movement to be attempted

			if(!game->player->hasLight() && !next->hasLight()){ // Player moving from dark space to dark space, may trip and fall...
				Terminal::wrpro(game->general->get(STR_STUMBLE));
				int life = rand()%DEATH_CHANCE;
				if(!life){ // Player has tripped and smashed their head
					Terminal::wrpro(game->general->get(STR_NOLIGHT));
					game->player->kill();
				}
			}

			if(!game->player->hasNosnomp() && !next->hasNosnomp()){ // Player moving from snomp space to snomp space, may get eaten...
				int life = rand()%DEATH_CHANCE;
				if(!life){ // Player has been caught by a snomp and eaten
					Terminal::wrpro("A snomp happens by and eats you.");
					game->player->kill();
				}
			}

			if(game->player->isAlive()){ // game->player was safe, or got away with it
				next->setDirection(CMD_BACK, game->player->getLocation());
				game->player->setLocation(next);
				Terminal::wrpro(game->player->getLocationArrival());

				if(!next->hasAir()){ // Check whether next location has ambient air
					Terminal::wrpro("Just a friendly warning: there is no ambient atmosphere at this location.");
				}


			}

		}

	}

}

/*
 *	Dispatch no-arg commands
 */
void Game::Dispatcher::dispatchNoArg(Game* game, Command* command){

	switch(command->getCode()){
		case CMD_ABRACADA: game->executor.execAbracada(game); break;
		case CMD_AVNARAND: game->executor.execAvnarand(game); break;
		case CMD_CHIMBU: game->executor.execChimbu(game); break;
		case CMD_COMMANDS: game->executor.execCommands(game); break;
		case CMD_CSB: game->executor.execCsb(game); break;
		case CMD_HELP: game->executor.execHelp(game); break;
		case CMD_INVENTOR: game->executor.execInventor(game); break;
		case CMD_LOOK: game->executor.execLook(game); break;
		case CMD_PLUGH: game->executor.execPlugh(game); break;
		case CMD_QUIT: game->executor.execQuit(game); break;
		case CMD_SCORE: game->executor.execScore(game); break;
		case CMD_SLEEP: game->executor.execSleep(game); break;
		case CMD_TEZAZZLE: game->executor.execTezazzle(game); break;
		case CMD_VERSION: game->executor.execVersion(game); break;
		case CMD_XYRO: game->executor.execXyro(game); break;
		case CMD_XYZZY: game->executor.execXyzzy(game); break;
		case CMD_ZIQUA: game->executor.execZiqua(game); break;
	}

}

/*
 *	Dispatch any-arg commands
 */
void Game::Dispatcher::dispatchAnyArg(Game* game, Command* command, uint64_t arg){

	switch(command->getCode()){
		case CMD_CALL: game->executor.execCall(game, arg); break;
		case CMD_HINT: game->executor.execHint(game, arg); break;
		case CMD_SAY: game->executor.execSay(game, arg); break;
	}

}

/*
 *	Dispatch present-arg commands
 */
void Game::Dispatcher::dispatchPresentArg(Game* game, Command* command, Item* item){

	if(!item->isPresent(game->player->getLocation()) && !game->player->hasInInventory(item->getCode())){ // Argument item is neither present at current location nor in inventory
		Terminal::wrpro("I see no " + item->getShortname() + " here to " + command->toString() + ", or I cannot get at it.");
		return;
	}

	switch(command->getCode()){

		case CMD_ATTACK: game->executor.execAttack(game, item); break;
		case CMD_BURN: game->executor.execBurn(game, item); break;
		case CMD_DESCRIBE: game->executor.execDescribe(game, item); break;

		case CMD_EMPTY:{
			if(!item->hasAttribute(CTRL_ITEM_CONTAINER)) // Item is not a container
				Terminal::wrpro("I do not know how to empty such a thing.");
			else // Item is a container
				game->executor.execEmpty(game, (Container*) item);
			break;
		}
		case CMD_IGNORE: game->executor.execIgnore(game, item); break;
		case CMD_LIGHT:{
			if(!item->hasAttribute(CTRL_ITEM_SWITCHABLE)) // Item is not a switchable item
				Terminal::wrpro("I do not know how to switch such a thing on.");
			else // Item is a switchable item
				game->executor.execLight(game, (SwitchableItem*) item);
			break;
		}
		case CMD_PLAY: game->executor.execPlay(game, item); break;
		case CMD_PUSH: game->executor.execPush(game, item); break;
		case CMD_QUENCH:{
			if(!item->hasAttribute(CTRL_ITEM_SWITCHABLE)) // Item is not a switchable item
				Terminal::wrpro("I do not know how to switch such a thing off.");
			else // Item is a switchable item
				game->executor.execQuench(game, (SwitchableItem*) item);
			break;
		}
		case CMD_REPAIR: game->executor.execRepair(game, item); break;
		case CMD_READ: game->executor.execRead(game, item); break;
		case CMD_ROB: game->executor.execRob(game, item); break;
		case CMD_ROLL: game->executor.execRoll(game, item); break;
		case CMD_RUB: game->executor.execRub(game, item); break;
		case CMD_TAKE: game->executor.execTake(game, item); break;

	}

}

/*
 *	Dispatch inventory-arg commands
 */
void Game::Dispatcher::dispatchInventoryArg(Game* game, Command* command, Item* item){

	if(!game->player->hasInInventory(item->getCode())){ // Argument item is not in inventory
		Terminal::wrpro("You do not have any " + item->getShortname() + DOT);
		return;
	}

	switch(command->getCode()){
		case CMD_DROP: game->executor.execDrop(game, item); break;
		case CMD_FREE: game->executor.execFree(game, item); break;
		case CMD_GIVE: game->executor.execGive(game, item); break;
		case CMD_INSERT: game->executor.execInsert(game, item); break;
		case CMD_THROW: game->executor.execThrow(game, item); break;
	}

}
