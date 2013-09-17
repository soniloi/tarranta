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
					game->executor.execMovement(game, current, next);
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
		else if(command->is(CMD_UP) && game->player->hasGravity() && current->hasAttribute(CTRL_LOC_NEEDSNO_GRAVITY)){ // Gravity preventing player from going up
			Terminal::wrpro(game->general->get(STR_MOVNOCEI));
		}
		else if(command->is(CMD_DOWN) && game->player->hasGravity() && !current->hasAttribute(CTRL_LOC_HAS_FLOOR)){ // Too far down
			Terminal::wrpro(game->general->get(STR_MOVNODOW));
		}
		else{ // Movement to be attempted

			game->executor.execMovement(game, current, next);

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
		case CMD_FISH: game->executor.execFish(game); break;
		case CMD_HELP: game->executor.execHelp(game); break;
		case CMD_INVENTOR: game->executor.execInventor(game); break;
		case CMD_KNIT: game->executor.execKnit(game); break;
		case CMD_LOOK: game->executor.execLook(game); break;
		case CMD_PLUGH: game->executor.execPlugh(game); break;
		case CMD_QUIT: game->executor.execQuit(game); break;
		case CMD_SCORE: game->executor.execScore(game, false); break;
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
		case CMD_EXPLAIN: game->executor.execExplain(game, arg); break;
		case CMD_FLY: game->executor.execFly(game, arg); break;
		case CMD_HINT: game->executor.execHint(game, arg); break;
		case CMD_SAY: game->executor.execSay(game, arg); break;
		case CMD_TETHER: game->executor.execTether(game, arg); break;
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
		case CMD_LAUNCH: game->executor.execLaunch(game, item); break;
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
		case CMD_RUB: game->executor.execRub(game, item); break;
		case CMD_TAKE: game->executor.execTake(game, item); break;

		case CMD_WATER: {

			list<Container*> containers = game->player->getFullLiquidInventoryContainers(); // Retrieve list of container items that currently hold liquid

			if(containers.size() == ZERO){ // No available liquid
				Terminal::wrpro("You have no liquid to pour onto the " + item->getShortname() + ".");
			}

			else if(containers.size() == ONE){ // Exactly one available liquid
				string input = Terminal::rdstr("Water "  + item->getShortname() + " with " + containers.front()->getItemWithin()->getShortname() + "? ");
				vector<uint64_t> confirm = Tokeniser::splitToCodes(input, SPACE);
				if(confirm.size() == ONE && (confirm[0] == STR_Y || confirm[0] == STR_YES)){
					game->executor.execWater(game, item, containers.front());
				}
			}

			else{ // Multiple available liquids
				Terminal::wrpro("Water " + item->getShortname() + " with...");
				int i = ONE;
				for(list<Container*>::iterator it = containers.begin() ; it != containers.end() ; it++){
					stringstream ss;
					ss << TAB << i << ". " << (*it)->getItemWithin()->getShortname();
					Terminal::wrtab(ss.str()); // List all available liquids
					i++;
				} 
				Terminal::wrtab("\t0. None of these");
				string choice = Terminal::rdstr("Please choose one: ");
				if(choice.length() > ONE)
					Terminal::wrpro("I do not understand that selection.");
				else if(choice[0] == '0') // If player wishes to cancel pour
					Terminal::wrpro(game->general->get(STR_OK));
				else if((unsigned) choice[0]-ASCII_OFFSET <= containers.size()){ // If player wishes to pour a liquid
					list<Container*>::iterator it = containers.begin();
					advance(it, choice[0]-ASCII_OFFSET-ONE); // Iterate to correct position in list
					game->executor.execWater(game, item, (*it));
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
		case CMD_COOK: game->executor.execCook(game, item); break;
		case CMD_DRINK: {
			if(!item->hasAttribute(CTRL_ITEM_LIQUID))
				Terminal::wrpro("I do not know how to drink something that is not liquid.");
			else
				game->executor.execDrink(game, item); 
			break;
		}
		case CMD_DROP: game->executor.execDrop(game, item); break;
		case CMD_EXCHANGE: {
			if(!game->player->hasInPresent(game->items->get(ITEM_MACHINE)))
				Terminal::wrpro("I see nowhere here that it can be exchanged.");
			else if(item->getCode() != ITEM_CARTRIDG)
				Terminal::wrpro("The machine rejects the item you offer in exchange.");
			else{
				string furtherinput = Terminal::rdstr("What would you like the machine to dispense in exchange? ");
				vector<uint64_t> secondarg = Tokeniser::splitToCodes(furtherinput, SPACE);
				Item* request = game->items->get(secondarg[0]);
				if(request->getCode() == ITEM_NULL)
						Terminal::wrpro("The machine does not know what that item is.");
				else
					game->executor.execExchange(game, item, request);
			}

			break;
		}
		case CMD_FREE: game->executor.execFree(game, item); break;
		case CMD_GIVE: {
			string furtherinput = Terminal::rdstr("Who or what would you like to give the " + item->getShortname() + " to? ");
			vector<uint64_t> secondarg = Tokeniser::splitToCodes(furtherinput, SPACE);
			Item* receiver = game->items->get(secondarg[0]);
			if(receiver == game->items->get(ITEM_NULL)) // No such item as that specified as receiver
				Terminal::wrpro("I do not know who or what that is.");
			else if(!game->player->hasInPresent(receiver)) // Item specified as receiver does not exist
				Terminal::wrpro("I see no " + receiver->getShortname() + " here to give it to.");
			else // Receiver exists, in player's vicinity
				game->executor.execGive(game, item, receiver);
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
				string input = Terminal::rdstr("Insert " + item->getShortname() + " into " + containers.front()->getShortname() + "? ");
				vector<uint64_t> confirm = Tokeniser::splitToCodes(input, SPACE);
				if(confirm.size() == ONE && (confirm[0] == STR_Y || confirm[0] == STR_YES)){
					if(item == containers.front()) // Smartarse wants to insert the item into itself
						Terminal::wrpro(game->general->get(STR_CONTRECU));
					else if(!containers.front()->isEmpty()) // Something is currently in container
						Terminal::wrpro(game->general->get(STR_CONTFULL));
					else{
						game->executor.execInsert(game, item, containers.front());
					}
				}
			}

			else{ // Multiple available containers
				Terminal::wrpro("Insert " + item->getShortname() + " into...");
				int i = ONE;
				for(list<Container*>::iterator it = containers.begin() ; it != containers.end() ; it++){
					stringstream ss;
					ss << TAB << i << ". " << (*it)->getShortname();
					Terminal::wrtab(ss.str()); // List all available containers
					i++;
				} 
				Terminal::wrtab("\t0. None of these");
				string choice = Terminal::rdstr("Please choose one: ");
				if(choice.length() > ONE)
					Terminal::wrpro("I do not understand that selection.");
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
						game->executor.execInsert(game, item, (*it));
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
				game->executor.execDrop(game, item); 
			break;
		}
		case CMD_THROW: game->executor.execThrow(game, item); break;
	}

}
