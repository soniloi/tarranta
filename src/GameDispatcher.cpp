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
			Terminal::wrpro(game->general->get(STR_MOVNOREM));
		else if(command->is(CMD_OUT)) // game->player requested 'out', but out is ambiguous here
			Terminal::wrpro(game->general->get(STR_MOVNOOUT));
		else
			Terminal::wrpro(game->general->get(STR_MOVNOWAY));
	}

	else{ // Location exists in desired direction
		Item* ob = game->player->getLocation()->getObstruction(); // Search for obstructions at current location

		if(ob != NULL && next != game->player->getLocation()->getDirection(CMD_BACK)){ // Obstruction may exist preventing player going any direction but the one they came from
		
			uint64_t obcode = ob->getCode();			

			if(obcode == ITEM_CORSAIR){ // Obstruction is the corsair (blind pirate)
				if(game->player->hasInInventory(game->items->get(ITEM_BOOTS))){ // Player is wearing boots, so corsair will hear them approach
					Terminal::wrpro(game->general->get(STR_PIRATSN1));
					game->player->kill();
				}
				else // Just give a warning instead
					Terminal::wrpro(game->general->get(STR_PIRATVG1));
			}

			else if(obcode == ITEM_BUCCANEE){ // Obstruction is the buccaneer (deaf pirate)
				if(!game->player->isInvisible()) // Player is visible to buccaneer, so do not go near him
						Terminal::wrpro(game->general->get(STR_PIRATVG2));
				else{ // Player is invisible to deaf pirate, so can proceed
					Terminal::wrpro(game->general->get(STR_PIRATSN2));
					game->movementexec->execMovement(current, next);
				}
			}
			
			else if(game->player->hasLight())
				Terminal::wrpro("You cannot get past the " + ob->getShortname() + ".");
			else
				Terminal::wrpro(game->general->get(STR_MOVOBST));
		}
		else if(!next->hasAir() && !game->player->hasInInventoryWithAttribute(CTRL_ITEM_GIVES_AIR)){ // Refuse to move if there is no air
			Terminal::wrpro(game->general->get(STR_MOVNOAIR));
		}
		else if(!next->hasLand() && !game->player->hasInInventoryWithAttribute(CTRL_ITEM_GIVES_LAND)){ // Refuse to move if there is no land
			Terminal::wrpro(game->general->get(STR_MOVNOLAN));
		}
		else if(command->is(CMD_UP) && game->player->hasGravity() && current->hasAttribute(CTRL_LOC_NEEDSNO_GRAVITY)){ // Gravity preventing player from going up
			Terminal::wrpro(game->general->get(STR_MOVNOCEI));
		}
		else if(command->is(CMD_DOWN) && game->player->hasGravity() && !current->hasAttribute(CTRL_LOC_HAS_FLOOR)){ // Too far down
			Terminal::wrpro(game->general->get(STR_MOVNODOW));
		}
		else{ // Movement to be attempted
			game->movementexec->execMovement(current, next);
		}

	}

}

/*
 *	Dispatch no-arg commands
 */
void Game::Dispatcher::dispatchNoArg(Game* game, Command* command){

	switch(command->getCode()){
		case CMD_ABRACADA: game->noargexec->execAbracada(); break;
		case CMD_AVNARAND: game->noargexec->execAvnarand(); break;
		case CMD_CHIMBU: game->noargexec->execChimbu(); break;
		case CMD_COMMANDS: game->noargexec->execCommands(); break;
		case CMD_CSB: game->noargexec->execCsb(); break;
		case CMD_FISH: game->noargexec->execFish(); break;
		case CMD_HELP: game->noargexec->execHelp(); break;
		case CMD_INVENTOR: game->noargexec->execInventor(); break;
		case CMD_KNIT: game->noargexec->execKnit(); break;
		case CMD_LOOK: game->noargexec->execLook(); break;
		case CMD_PLUGH: game->noargexec->execPlugh(); break;
		case CMD_QUIT: game->noargexec->execQuit(); break;
		case CMD_SCORE: game->noargexec->execScore(false); break;
		case CMD_SLEEP: game->noargexec->execSleep(); break;
		case CMD_STARE: game->noargexec->execStare(); break;
		case CMD_TEZAZZLE: game->noargexec->execTezazzle(); break;
		case CMD_VERSION: game->noargexec->execVersion(); break;
		case CMD_XYRO: game->noargexec->execXyro(); break;
		case CMD_XYZZY: game->noargexec->execXyzzy(); break;
		case CMD_ZIQUA: game->noargexec->execZiqua(); break;
	}

}

/*
 *	Dispatch any-arg commands
 */
void Game::Dispatcher::dispatchAnyArg(Game* game, Command* command, uint64_t arg){

	switch(command->getCode()){
		case CMD_CALL: game->anyargexec->execCall(arg); break;
		case CMD_EXPLAIN: game->anyargexec->execExplain(arg); break;
		case CMD_FLY: game->anyargexec->execFly(arg); break;
		case CMD_HINT: game->anyargexec->execHint(arg); break;
		case CMD_SAY: game->anyargexec->execSay(arg); break;
		case CMD_TETHER: game->anyargexec->execTether(arg); break;
	}

}

/*
 *	Dispatch present-arg commands
 */
void Game::Dispatcher::dispatchPresentArg(Game* game, Command* command, Item* item){

	if(!game->player->hasInPresent(item)){
		Terminal::wrpro("I see no " + item->getShortname() + " here to " + command->toString() + ".");
		return;
	}

	switch(command->getCode()){

		case CMD_ATTACK: game->presentargexec->execAttack(item); break;
		case CMD_BURN: game->presentargexec->execBurn(item); break;
		case CMD_DESCRIBE: game->presentargexec->execDescribe(item); break;

		case CMD_EMPTY:{
			if(!item->hasAttribute(CTRL_ITEM_CONTAINER)) // Item is not a container
				Terminal::wrpro(game->general->get(STR_NONOEMPT));
			else // Item is a container
				game->presentargexec->execEmpty((Container*) item);
			break;
		}
		case CMD_IGNORE: game->presentargexec->execIgnore(item); break;
		case CMD_LIGHT:{
			if(!item->hasAttribute(CTRL_ITEM_SWITCHABLE)) // Item is not a switchable item
				Terminal::wrpro(game->general->get(STR_NONOSWON));
			else // Item is a switchable item
				game->presentargexec->execLight((SwitchableItem*) item);
			break;
		}
		case CMD_PLAY: game->presentargexec->execPlay(item); break;
		case CMD_PUSH: game->presentargexec->execPush(item); break;
		case CMD_QUENCH:{
			if(!item->hasAttribute(CTRL_ITEM_SWITCHABLE)) // Item is not a switchable item
				Terminal::wrpro(game->general->get(STR_NONOSWOF));
			else // Item is a switchable item
				game->presentargexec->execQuench((SwitchableItem*) item);
			break;
		}
		case CMD_REPAIR: game->presentargexec->execRepair(item); break;
		case CMD_READ: game->presentargexec->execRead(item); break;
		case CMD_ROB: game->presentargexec->execRob(item); break;
		case CMD_RUB: game->presentargexec->execRub(item); break;
		case CMD_TAKE: game->presentargexec->execTake(item); break;

		case CMD_WATER: {

			list<Container*> containers = game->player->getFullLiquidInventoryContainers(); // Retrieve list of container items that currently hold liquid

			if(containers.size() == ZERO){ // No available liquid
				Terminal::wrpro("You have no liquid to pour onto the " + item->getShortname() + ".");
			}

			else if(containers.size() == ONE){ // Exactly one available liquid
				if(game->confirm("Water "  + item->getShortname() + " with " + containers.front()->getItemWithin()->getShortname() + "? "))
					game->presentargexec->execWater(item, containers.front());
			}

			else{ // Multiple available liquids
				Terminal::wrpro("Water " + item->getShortname() + " with...");
				int i = ONE;
				for(list<Container*>::iterator it = containers.begin() ; it != containers.end() ; it++){
					Terminal::wrtab(game->listentry(i, (*it)->getItemWithin()->getShortname())); // List all available liquids
					i++;
				} 
				Terminal::wrtab("\t0. None of these");
				string choice = Terminal::rdstr(game->general->get(STR_CHOOSE));
				if(choice.length() > ONE)
					Terminal::wrpro(game->general->get(STR_NOTUIGSE));
				else if(choice[0] == '0') // If player wishes to cancel pour
					Terminal::wrpro(game->general->get(STR_OK));
				else if((unsigned) choice[0]-ASCII_OFFSET <= containers.size()){ // If player wishes to pour a liquid
					list<Container*>::iterator it = containers.begin();
					advance(it, choice[0]-ASCII_OFFSET-ONE); // Iterate to correct position in list
					game->presentargexec->execWater(item, (*it));
				}
				else // If player has selected an invalid option
					Terminal::wrpro(game->general->get(STR_NOOPTION));
			}

			break;
		}

	}

}

/*
 *	Dispatch inventory-arg commands
 */
void Game::Dispatcher::dispatchInventoryArg(Game* game, Command* command, Item* item){

	if(!game->player->hasInInventory(item)){ // Argument item is not in inventory
		Terminal::wrpro("You do not have any " + item->getShortname() + DOT);
		return;
	}

	switch(command->getCode()){
		case CMD_COOK: game->inventoryargexec->execCook(item); break;
		case CMD_DRINK: {
			if(!item->hasAttribute(CTRL_ITEM_LIQUID))
				Terminal::wrpro(game->general->get(STR_NONODRIN));
			else
				game->inventoryargexec->execDrink(item); 
			break;
		}
		case CMD_DROP: game->inventoryargexec->execDrop(item); break;
		case CMD_EXCHANGE: {
			if(game->player->hasInPresent(game->items->get(ITEM_MACHINE))){ // Player performing exchange at machine
				if(item->getCode() != ITEM_CARTRIDG) // Player attempting to exchange something other than cartridge at machine
					Terminal::wrpro(game->general->get(STR_MACHIREJ));
				else{
					vector<uint64_t> secondarg = Terminal::readCodes(game->general->get(STR_MACHIWAT));
					Item* request = game->items->get(secondarg[0]);
					if(request->getCode() == ITEM_NULL)
							Terminal::wrpro(game->general->get(STR_MACHINNO));
					else
						game->inventoryargexec->execExchange(item, request);
				}
			}
			else if(game->player->hasInPresent(game->items->get(ITEM_BUILDING))){ // Player performing exchange at farm
				if(!item->hasAttribute(CTRL_ITEM_TREASURE)) // Player attempting to exchange something other than a treasure for farm
					Terminal::wrpro(game->general->get(STR_NOTVALUE));
				else{
					Item* farm = game->items->get(ITEM_BUILDING);
					game->inventoryargexec->execExchange(item, farm);
				}
			}
			else
				Terminal::wrpro(game->general->get(STR_NOEXCHAN));
			break;
		}
		case CMD_FREE: game->inventoryargexec->execFree(item); break;
		case CMD_GIVE: {
			vector<uint64_t> secondarg = Terminal::readCodes("Who or what would you like to give the " + item->getShortname() + " to? ");
			Item* receiver = game->items->get(secondarg[0]);
			if(receiver == game->items->get(ITEM_NULL)) // No such item as that specified as receiver
				Terminal::wrpro(game->general->get(STR_NONOWHAT));
			else if(!game->player->hasInPresent(receiver)) // Item specified as receiver does not exist
				Terminal::wrpro("I see no " + receiver->getShortname() + " here to give it to.");
			else // Receiver exists, in player's vicinity
				game->inventoryargexec->execGive(item, receiver);
			break;
		}
		case CMD_INSERT: {
				
			list<Container*> containers = game->player->getSuitableContainers(item); // Retrieve list of container items capable of holding this item

			if(containers.size() == ZERO){ // No available container to place item into
				Terminal::wrpro(game->general->get(STR_NOCONTAI));
			}

			else if(item->hasAttribute(CTRL_ITEM_WORN)){
				Terminal::wrpro(game->general->get(STR_NOFITWEA));
			}

			else if(containers.size() == ONE){ // Exactly one available container
				if(game->confirm("Insert " + item->getShortname() + " into " + containers.front()->getShortname() + "? ")){
					if(item == containers.front()) // Smartarse wants to insert the item into itself
						Terminal::wrpro(game->general->get(STR_CONTRECU));
					else if(!containers.front()->isEmpty()) // Something is currently in container
						Terminal::wrpro(game->general->get(STR_CONTFULL));
					else
						game->inventoryargexec->execInsert(item, containers.front());
				}
			}

			else{ // Multiple available containers
				Terminal::wrpro("Insert " + item->getShortname() + " into...");
				int i = ONE;
				for(list<Container*>::iterator it = containers.begin() ; it != containers.end() ; it++){
					Terminal::wrtab(game->listentry(i, (*it)->getShortname())); // List all available containers
					i++;
				} 
				Terminal::wrtab("\t0. None of these");
				string choice = Terminal::rdstr(game->general->get(STR_CHOOSE));
				if(choice.length() > ONE)
					Terminal::wrpro(game->general->get(STR_NOTUIGSE));
				else if(choice[0] == '0'){ // If player wishes to cancel insert
					Terminal::wrpro(game->general->get(STR_OK));
				}
				else if((unsigned) choice[0]-ASCII_OFFSET <= containers.size()){ // If player wishes to insert into available container
					list<Container*>::iterator it = containers.begin();
					advance(it, choice[0]-ASCII_OFFSET-ONE); // Iterate to correct position in list
					if(item == (*it)) // Smartarse wants to insert the item into itself
						Terminal::wrpro(game->general->get(STR_CONTRECU));
					else if(!(*it)->isEmpty()) // Something is currently in container
						Terminal::wrpro(game->general->get(STR_CONTFULL));
					else{
						game->inventoryargexec->execInsert(item, (*it));
					}
				}
				else{ // If player has selected an invalid option
					Terminal::wrpro(game->general->get(STR_NOOPTION));
				}

			}

			break;
		}

		case CMD_POUR: {
			if(!item->hasAttribute(CTRL_ITEM_LIQUID))
				Terminal::wrpro(game->general->get(STR_NONOPOUR));
			else
				game->inventoryargexec->execDrop(item); 
			break;
		}
		case CMD_THROW: game->inventoryargexec->execThrow(item); break;
	}

}
