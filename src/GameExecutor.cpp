#include "Game.h"

/*
 *	Testing methods (conditionally-compiled only)
 */

#ifdef TESTING_READ
/*
 *	Execute command to echo player's current location to terminal
 */
void Game::Executor::execNode(Game* game){
	Location* loc = game->player->getLocation();
	Location* prevloc = loc->getDirection(CMD_BACK);
	stringstream nodess;
	nodess << "You are currently at node: " << loc->getID() << ", \"" << loc->getLongname() << "\". ";
	nodess << "Your location previous to this was node: " << prevloc->getID() << ", \"" << prevloc->getLongname() << "\".";
	Terminal::wrpro(nodess.str());
}
#endif

#ifdef TESTING_WRITE
/*
 *	Execute command to move player to any desired location in game
 */
void Game::Executor::execFlash(Game* game, uint64_t arg){
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
void Game::Executor::execGrab(Game* game, uint64_t arg){
	Item* item = game->items->get(arg);
	if(!item){ // Whatever they input as argument isn't anything we know as an item
		Terminal::wrpro("I don't know what " + Statics::codeToStr(arg) + " is.");
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

/*
 *	Execute ABRACADA magic word command
 */
void Game::Executor::execAbracada(Game* game){
	Terminal::wrpro(game->general->get(STR_NOHAPPEN));
}

/*
 *	Execute AVNARAND magic word command
 */
void Game::Executor::execAvnarand(Game* game){
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
void Game::Executor::execChimbu(Game* game){
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

			game->player->incrementScore(TWO*SCORE_PUZZLE);
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
void Game::Executor::execCommands(Game* game){
	Terminal::wrpro(game->commands->listNonSecret());
}

/*
 *	Cool story, bro
 */
void Game::Executor::execCsb(Game* game){
	Terminal::wrpro(game->general->get(STR_IKR));
}

/*
 *	Execute command to fish for something
 */
void Game::Executor::execFish(Game* game){

	if(!game->player->hasInInventory(game->items->get(ITEM_NET))){
		Terminal::wrpro(game->general->get(STR_NONET));
		return;
	}
	Item* glint = game->items->get(ITEM_GLINT);
	if(glint->getLocation() != game->player->getLocation()){
		Terminal::wrpro(game->general->get(STR_NOFISH));
		return;
	}
	glint->getLocation()->extractFixture(glint);
	glint->setLocation(game->station->get(LOCATION_NOWHERE)); // Remove the glint the player is seeing

	Item* nugget = game->items->get(ITEM_NUGGET);
	game->player->addToInventory(nugget);
	nugget->setLocation(game->station->get(LOCATION_INVENTORY)); // Give nugget to player

	Terminal::wrpro(game->general->get(STR_NUGGGOOD));
	game->player->incrementScore(SCORE_PUZZLE);	
	
}

/*
 *	Execute command to give basic help to user
 */
void Game::Executor::execHelp(Game* game){
	Terminal::wrpro(game->general->get(STR_HELP));
}

/*
 *	Execute command to list contents of inventory
 */
void Game::Executor::execInventor(Game* game){
	Terminal::wrpro(game->player->inventoryToString());
}

/*
 *	Execute command to describe surroundings to user
 */
void Game::Executor::execLook(Game* game){
	Terminal::wrpro(game->player->getLocationAll());
}

/*
 *	Execute PLUGH magic word command
 */
void Game::Executor::execPlugh(Game* game){
	Terminal::wrpro(game->general->get(STR_PLUGH));
}

/*
 *	Execute command to end game
 */
void Game::Executor::execQuit(Game* game){

	string s = Terminal::rdstr("Are you sure?");
	if(s.compare("y") && s.compare("yes"))
		return;

	stringstream ss;
	ss << "Quitting game ...\nYour final score is " << game->calculateScore() << " points; you gave " << game->player->getMoves() << " instructions and died a total of " << game->player->getDeaths() << " times.";
	Terminal::wrpro(ss.str());
	game->on = false;

}

/*
 *	Execute command to display current score to player
 */
void Game::Executor::execScore(Game* game){
	stringstream ss;
	ss << "Your current score is " << game->calculateScore() << " points, and you have died " << game->player->getDeaths() << " times.";
	Terminal::wrpro(ss.str());
}

/*
 *	Execute command for player to go into/out of 'sleep' sequence
 */
void Game::Executor::execSleep(Game* game){

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
void Game::Executor::execTezazzle(Game* game){
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
void Game::Executor::execVersion(Game* game){
	Terminal::wrpro(game->general->get(STR_VERSION));
}

/*
 *	Execute XYRO magic word command
 */
void Game::Executor::execXyro(Game* game){
	Item* wiz = game->player->getLocation()->get(ITEM_WIZARD);
	if(wiz){
		Terminal::wrpro(game->general->get(STR_WIZARDED));
		game->player->kill();
	}
	else
		Terminal::wrpro(game->general->get(STR_SHMAGIC));
}

/*
 *	Execute XYZZY magic word command
 */
void Game::Executor::execXyzzy(Game* game){
	Terminal::wrpro(game->general->get(STR_OK));
}

/*
 *	Execute ZIQUA magic word command
 */
void Game::Executor::execZiqua(Game* game){
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

/*
 *	Execute command to call/summon something
 */
void Game::Executor::execCall(Game* game, uint64_t arg){
	if(game->player->getLocation()->getID() == LOCATION_COMMS && arg == STR_SHIP){
		// Change player's start & safe locations on death; they must now be west of the checkpoint rather than east
		game->player->setWakeLocation(game->station->get(LOCATION_START_1));
		game->player->setSafeLocation(game->station->get(LOCATION_SAFE_1));

		Location* loc;
		Location* loc1;
		Item* item;

		// Insert the bloodthirsty pirate at the checkpoint
		loc = game->station->get(LOCATION_CHECKPOINT);
		item = game->items->get(ITEM_PIRATE);
		loc->fix(item);
		item->setLocation(loc);

		// Insert the pirate ship (Item) into the docking bay
		loc = game->station->get(LOCATION_DOCKING);
		item = game->items->get(ITEM_SHIP);
		loc->fix(item);
		item->setLocation(loc);

		// Link the pirate ship (Location) to the docking bay
		loc = game->station->get(LOCATION_DOCKING);
		loc1 = game->station->get(LOCATION_SHIP);
		loc->setDirection(CMD_EAST, loc1);
		loc->setDirection(CMD_SOUTHEAS, loc1);

		// Unlink the shuttle from the southeast of the docking bay
		loc = game->station->get(LOCATION_SHUTTLE);
		loc->setDirection(CMD_SOUTH, game->station->get(LOCATION_NOWHERE));

		Terminal::wrpro("You send a distress call to the unknown ship.");
	}
	else
		Terminal::wrpro(game->general->get(STR_NONOHOW));

}

/*
 *	Execute command to display hint
 */
void Game::Executor::execHint(Game* game, uint64_t arg){
	string hint = game->hints->get(arg);
	if(hint.empty())
		hint = game->hints->get(HINT_DEFAULT);
	Terminal::wrpro(hint);
}

/*
 *	Execute command to say something
 */
void Game::Executor::execSay(Game* game, uint64_t arg){
	string statement = Statics::codeToStr(arg);
	Terminal::wrpro("You say \"" + statement + "\" aloud.");
}

/*
 *	Execute command to attack something
 */
void Game::Executor::execAttack(Game* game, Item* item){
	uint64_t code = item->getCode();

	if(code == ITEM_DRAGON || code == ITEM_LION || code == ITEM_WOLF)
		Terminal::wrpro(game->general->get(STR_NOWISEAT));
	else
		Terminal::wrpro(game->general->get(STR_NONOHOW));
}

/*
 *	Execute command to burn an item
 */
void Game::Executor::execBurn(Game* game, Item* item){

	if(!game->player->hasInInventory(game->items->get(ITEM_MATCHES))){
		Terminal::wrpro(game->general->get(STR_NOMATCH));
		return;
	}

	uint64_t code = item->getCode();
	if(code == ITEM_BOOK)
		Terminal::wrpro(game->general->get(STR_PHILISTI));
	else if(code == ITEM_BREAD){ // Burn bread and replace with toast
		game->destroyItem(item);
		Terminal::wrpro(game->general->get(STR_BREAD));
		game->player->addToInventory(game->items->get(ITEM_TOAST));
	}
	else if(code == ITEM_TOAST){ // Burn toast and replace with ash
		game->destroyItem(item);
		if(game->player->getLocation()->getID() == LOCATION_AIRLOCKE){
			Terminal::wrpro(game->general->get(STR_TOAST));
			Terminal::wrpro(game->general->get(STR_ASHMOUSE));
			Location* loc = game->player->getLocation();
			loc->setAttribute(CTRL_LOC_HAS_AIR, false); // Air has been blown out of the airlock
			loc->setDirection(CMD_SOUTHWES, game->station->get(LOCATION_AIRLOCKEOUT));
			game->player->incrementScore(SCORE_PUZZLE);
		}
	}
	else{ // Some other item or fixture, refuse to burn
		Terminal::wrpro(game->general->get(STR_NONOHOW));
		
	}
}

/*
 *	Execute command to describe an item
 */
void Game::Executor::execDescribe(Game* game, Item* item){
	Terminal::wrpro("It is " + item->getFullname() + DOT);
}

/*
 *	Execute command to empty container
 */
void Game::Executor::execEmpty(Game* game, Container* container){

	Item* itemwithin = container->extractItemWithin();
	if(!itemwithin){ // Nothing in the container
		Terminal::wrpro("It is already empty.");
		return;
	}

	if(itemwithin->hasAttribute(CTRL_ITEM_LIQUID)){
		Terminal::wrpro("You pour the " + itemwithin->getShortname() + " out of the " + container->getShortname() + ". It falls to the ground and quickly seeps away.");
		itemwithin->setLocation(game->station->get(LOCATION_NOWHERE));
	}

	else if(game->player->hasInInventory(container)){ // Item in container, which is in inventory
		game->player->addToInventory(itemwithin);
		itemwithin->setLocation(game->station->get(LOCATION_INVENTORY));
		Terminal::wrpro("The " + container->getShortname() + " has been emptied of " + itemwithin->getLongname() + ", which you are now holding.");
	}
	else{ // Item in container, which is at location but not in inventory
		game->player->getLocation()->deposit(itemwithin);
		itemwithin->setLocation(game->player->getLocation());
		Terminal::wrpro("The " + container->getShortname() + " has been emptied of " + itemwithin->getLongname() + ", which now sits on the ground.");
	}	
}

/*
 *	Execute command to give this item to someone at this location
 */
void Game::Executor::execGive(Game* game, Item* item){
	uint64_t code = item->getCode();
	Item* receiver;

	receiver = game->player->getLocation()->get(ITEM_LION);
	if(receiver){ // Player has chosen to feed the lion
		if(code == ITEM_RUTABAGA){
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
		else{
			Terminal::wrpro(game->general->get(STR_LIONYAWN)); // Lion is not interested
		}
		return;
	}

	receiver = game->player->getLocation()->get(ITEM_TROLL);
	if(receiver){
		if(item->hasAttribute(CTRL_ITEM_EDIBLE)){
			Terminal::wrpro(game->general->get(STR_TROLLED));
			game->player->extractFromInventory(item);
			item->setLocation(game->station->get(LOCATION_NOWHERE));
			game->player->kill();
		}
		else{
			Terminal::wrpro(game->general->get(STR_TROLYAWN));
		}
		return;
	}

	Terminal::wrpro(game->general->get(STR_GIVEYAWN));

}

/*
 *	Execute command to ignore something or someone
 */
void Game::Executor::execIgnore(Game* game, Item* item){
	uint64_t code = item->getCode();
	if(code == ITEM_TROLL){
		game->player->getLocation()->extract(item);
		Terminal::wrpro(game->general->get(STR_TROLL));
		game->player->incrementScore(SCORE_PUZZLE);
	}
	else
		Terminal::wrpro(game->general->get(STR_NOINTERE));
}

/*
 *	Execute command to turn a switchable item on
 */
void Game::Executor::execLight(Game* game, SwitchableItem* switchable){
	if(switchable->isOn())
		Terminal::wrpro(game->general->get(STR_ALREADON));
	else{
		switchable->setOn(true);
		Terminal::wrpro("The " + switchable->getShortname() + " is now on.");
	}
}

/*
 *	Execute command to play an item
 */
void Game::Executor::execPlay(Game* game, Item* item){
	uint64_t code = item->getCode();

	if(code == ITEM_WHISTLE){
		string tune = Terminal::rdstr("What would you like to play? ");
		if(tune.empty())
			Terminal::wrpro("Do not waste my time, s'il vous plait");
		else{
			Item* listener = game->player->getLocation()->get(ITEM_LION);
			for(unsigned int i = 0 ; i < tune.length() ; i++)
				tune[i] = tolower(tune[i]);
			if((listener != NULL) && (!tune.compare(game->general->get(STR_CABBAGE)))){
				stringstream ss;
				Terminal::wrpro(game->general->get(STR_LIONTUNE));
				listener->setAttribute(CTRL_ITEM_OBSTRUCTION, false); // Lion will no longer obstruct the player
				game->player->incrementScore(SCORE_PUZZLE);
			}
			else
				Terminal::wrpro("You play \"" + tune + "\" on the whistle.");
		}
	}

	else
		Terminal::wrpro(game->general->get(STR_NONOHOW));

}

/*
 *	Execute command to push an item
 *	This includes pushing a switch on a switchable item
 */
void Game::Executor::execPush(Game* game, Item* item){

	uint64_t code = item->getCode();

	if(item->hasAttribute(CTRL_ITEM_SWITCHABLE)){
		SwitchableItem* switchable = (SwitchableItem*) item;
		if(switchable->isOn()){ // Switch had been on, so turn off
			switchable->setOn(false);
			if(code == ITEM_BUTTON){ // If button is pressed, turn gravity on in the anteroom
				game->station->get(LOCATION_ANTEROOM)->setAttribute(CTRL_LOC_HAS_GRAVITY, true);
				Terminal::wrpro(game->general->get(STR_NOHAPPEN));
			}
			Terminal::wrpro("It is now in the off position.");
		}
		else{ // Switch had been off, so turn on
			switchable->setOn(true);
			if(code == ITEM_BUTTON){ // If button is pressed, turn gravity off in the anteroom
				game->station->get(LOCATION_ANTEROOM)->setAttribute(CTRL_LOC_HAS_GRAVITY, false);
				Terminal::wrpro(game->general->get(STR_NOHAPPEN));
			}
			Terminal::wrpro("It is now in the on position.");
		}
	}

}

/*
 *	Execute command to turn a switchable item off
 */
void Game::Executor::execQuench(Game* game, SwitchableItem* switchable){
	if(!switchable->isOn())
		Terminal::wrpro(game->general->get(STR_ALREADOF));
	else{
		switchable->setOn(false);
		Terminal::wrpro("The " + switchable->getShortname() + " is now off.");
	}
}

/*
 *	Execute command to read any writing on item
 */
void Game::Executor::execRead(Game* game, Item* item){
	string writing = item->getWriting();
	if(writing.empty()) // Item has no writing on it
		Terminal::wrpro(game->general->get(STR_NOWRITIN));
	else // Item has writing on it
		Terminal::wrpro("It reads: \"" + writing + "\".");
}

/*
 *	Execute command to effect repairs to something
 */
void Game::Executor::execRepair(Game* game, Item* item){
		uint64_t code = item->getCode();
		if(code == ITEM_CONSOLE){
			Terminal::wrpro(game->general->get(STR_CONSOLE));
			Item* newpanel = game->items->get(ITEM_PANEL);
			newpanel->setLocation(item->getLocation());
			item->getLocation()->fix(newpanel);
			game->destroyItem(item);
		}
		else if(code == ITEM_PANEL)
			Terminal::wrpro("The control panel has already been repaired.");
		else
			Terminal::wrpro(game->general->get(STR_NONOHOW));
}

/*
 *	Execute command to rob someone
 */
void Game::Executor::execRob(Game* game, Item* item){
	uint64_t code = item->getCode();
	if(code == ITEM_PIRATE){
		Item* key = game->items->get(ITEM_KEY);
		if(key->getLocation()->getID() == LOCATION_NOWHERE)
			Terminal::wrpro("The pirate has nothing more for you to rob.");
		else{
			Terminal::wrpro(game->general->get(STR_PIRATROB));
			game->player->addToInventory(game->items->get(ITEM_KEY));
		}
		
	}
	else
		Terminal::wrpro(game->general->get(STR_NONOHOW));
}

/*
 *	Execute command to roll something
 */
void Game::Executor::execRoll(Game* game, Item* item){

}


/*
 *	Execute command to rub an item
 */
void Game::Executor::execRub(Game* game, Item* item){
	uint64_t code = item->getCode();
	if(code == ITEM_DRAGON){ // Player wishes to rub dragon
		game->player->getLocation()->extract(item);
		Terminal::wrpro(game->general->get(STR_DRAGON));
		game->player->incrementScore(SCORE_PUZZLE);

		Item* tooth = game->items->get(ITEM_TOOTH);
		game->player->getLocation()->deposit(tooth); // Dragon leaves tooth behind
		tooth->setLocation(game->player->getLocation());
	}
	else if(code == ITEM_LAMP) // Player wishes to rub lamp
		Terminal::wrpro(game->general->get(STR_GENIE));
	else
		Terminal::wrpro(game->general->get(STR_NOINTERE));

}

/*
 *	Execute command to take item into inventory
 */
void Game::Executor::execTake(Game* game, Item* item){
	if(!game->player->hasLight()) // Player cannot see what they are doing
		Terminal::wrpro(game->general->get(STR_TAKENOSE));
	else if(game->player->hasInInventory(item)) // Item is already in inventory
		Terminal::wrpro(game->general->get(STR_TAKEALRE));
	else if(!item->hasAttribute(CTRL_ITEM_MOBILE)) // Item is not portable
		Terminal::wrpro(game->general->get(STR_TAKENOCA));
	else if(item->hasAttribute(CTRL_ITEM_LIQUID)){ // Liquids cannot be carried directly, but must be in containers

		if(item->getLocation() == game->station->get(LOCATION_CONTAINER	)){ // Liquid is currently in a container somewhere else, so we prevent player from taking more until that disappears
			Terminal::wrpro("I think you have had enough for now, do you not agree?");
			return;
		}

		list<Container*> containers = game->player->getSuitableInventoryContainers(item); // Search for containers the player already has

		if(containers.size() == ZERO){ // No available container to place item into
			Terminal::wrpro(game->general->get(STR_NOCONTAI));
		}

		else if(containers.size() == ONE){ // Exactly one available container
			string confirm = Terminal::rdstr("Take " + item->getShortname() + " in " + containers.front()->getShortname() + "? ");
			if(!confirm.compare("y") || !confirm.compare("yes")){
				if(item == containers.front()) // Smartarse wants to insert the item into itself
					Terminal::wrpro(game->general->get(STR_CONTRECU));
				else if(!containers.front()->isEmpty()) // Something is currently in container
					Terminal::wrpro(game->general->get(STR_CONTFULL));
				else{
					//game->player->getLocation()->extract(item); // Remove item from location
					item->setLocation(game->station->get(LOCATION_CONTAINER)); // Set item's location to "container"
					containers.front()->insertItem(item); // Insert item into desired container
					Terminal::wrpro(item->getShortname() + " taken in " + containers.front()->getShortname() + ".");
				}
			}

		}

		else{ // Multiple available containers
			Terminal::wrpro("Take " + item->getShortname() + " in...");
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
					//game->player->getLocation()->extract(item); // Remove item from location
					item->setLocation(game->station->get(LOCATION_CONTAINER)); // Set item's location to "container"
					(*it)->insertItem(item); // Insert item into desired container
					Terminal::wrpro(item->getShortname() + " taken in " + (*it)->getShortname() + ".");
				}
			}
			else{ // If player has selected an invalid option
				Terminal::wrpro("I gave no such option.");
			}
		}

	}
	else{ // Item is portable
		Location* itemloc = item->getLocation();
		if(itemloc != NULL && !game->player->hasInInventory(item))
			itemloc->extract(item);
		item->setLocation(game->station->get(LOCATION_INVENTORY));
		game->player->addToInventory(item);
		if(item->hasAttribute(CTRL_ITEM_WORN))
			Terminal::wrpro(game->general->get(STR_WEAR));
		else
			Terminal::wrpro(game->general->get(STR_TAKEGOOD));
	}
}

/*
 *	Execute command to drink a (liquid) item
 */
void Game::Executor::execDrink(Game* game, Item* item){
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
			game->player->setStrength(MAX_STRENGTH);
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
void Game::Executor::execDrop(Game* game, Item* item){
	
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
			Terminal::wrpro("Because there is no floor here, it falls down. You hear it hit the ground far below.");
			if(item->hasAttribute(CTRL_ITEM_FRAGILE)){
				Terminal::wrpro("Fragile as it was, you hear it break into pieces as it makes contact with a surface. You also hear a little cleaner bot come out to collect the remains.");
				game->destroyItem(item);
			}
			else{
				below->deposit(item);
				game->player->extractFromInventory(item); // Remove item from inventory
				item->setLocation(below);
			}
		}
	}

	else{
		Terminal::wrpro(game->general->get(STR_DROPGOOD));
		current->deposit(item); // Add item to current location's item list
		game->player->extractFromInventory(item); // Remove item from inventory
		item->setLocation(current); // Set item's location to current location
	}
}

/*
 *	Execute command to release an item from inventory
 */
void Game::Executor::execFree(Game* game, Item* item){
	uint64_t code = item->getCode();
	Location* current = game->player->getLocation();
	Item* attackee = current->get(ITEM_WOLF);

	this->execDrop(game, item);

	if((code == ITEM_LION) && (attackee != NULL)){ // Lion will attack and drive off wolf if released in same location
		current->extract(attackee); // Remove wolf from location
		Terminal::wrpro(game->general->get(STR_LIONWOLF));
		game->player->incrementScore(SCORE_PUZZLE);
	}
}

/*
 *	Execute command to insert item into an available container
 */
void Game::Executor::execInsert(Game* game, Item* item){

	list<Container*> containers = game->player->getSuitableContainers(item); // Retrieve list of container items capable of holding this item

	if(containers.size() == ZERO){ // No available container to place item into
		Terminal::wrpro(game->general->get(STR_NOCONTAI));
	}

	else if(item->hasAttribute(CTRL_ITEM_WORN)){
		Terminal::wrpro(game->general->get(STR_NOFITWEA));
	}

	else if(containers.size() == ONE){ // Exactly one available container
		string confirm = Terminal::rdstr("Insert " + item->getShortname() + " into " + containers.front()->getShortname() + "? ");
		if(!confirm.compare("y") || !confirm.compare("yes")){
			if(item == containers.front()) // Smartarse wants to insert the item into itself
				Terminal::wrpro(game->general->get(STR_CONTRECU));
			else if(!containers.front()->isEmpty()) // Something is currently in container
				Terminal::wrpro(game->general->get(STR_CONTFULL));
			else{
				game->player->extractFromInventory(item); // Remove item from inventory
				item->setLocation(game->station->get(LOCATION_CONTAINER)); // Set item's location to "container"
				containers.front()->insertItem(item); // Insert item into desired container
				Terminal::wrpro(item->getShortname() + " inserted into " + containers.front()->getShortname() + ".");
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
				game->player->extractFromInventory(item); // Remove item from inventory
				item->setLocation(game->station->get(LOCATION_CONTAINER)); // Set item's location to "container"
				(*it)->insertItem(item); // Insert item into desired container
				Terminal::wrpro(item->getShortname() + " inserted into " + (*it)->getShortname() + ".");
			}
		}
		else{ // If player has selected an invalid option
			Terminal::wrpro("I gave no such option.");
		}
	}

}

/*
 *	Execute command to throw item
 */
void Game::Executor::execThrow(Game* game, Item* item){
	Terminal::wrpro("You throw as hard as you can.");
	if(item->hasAttribute(CTRL_ITEM_FRAGILE) && game->player->getLocation()->hasAttribute(CTRL_LOC_HAS_GRAVITY)){
		Terminal::wrpro("Fragile as it is, it breaks into pieces on contact with a nearby surface.");
		Terminal::wrpro(game->general->get(STR_SHARDS));
		game->destroyItem(item);
	}
	else
		this->execDrop(game, item);
}