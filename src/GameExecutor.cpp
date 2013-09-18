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

/*
 *	Execute a movement command
 */
void Game::Executor::execMovement(Game* game, Location* current, Location* next){

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
			Terminal::wrpro(game->general->get(STR_SNOMPEAT));
			game->player->kill();
		}
	}

	if(game->player->isAlive()){ // game->player was safe, or got away with it

		if(next->hasRouteTo(current)) // Player will only be able to use back command if there is actually a way they could
			next->setDirection(CMD_BACK, game->player->getLocation());
		else
			next->setDirection(CMD_BACK, game->station->get(LOCATION_NOWHERE));
		game->player->setLocation(next);
		Terminal::wrpro(game->player->getLocationArrival());

	}

}

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
		Terminal::wrpro(game->general->get(STR_NOEQUIP));
		return;
	}
	Item* glint = game->items->get(ITEM_GLINT);
	if(glint->getLocation() != game->player->getLocation()){
		Terminal::wrpro(game->general->get(STR_NOFISH));
		return;
	}
	glint->getLocation()->extract(glint);
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
 *	Execute command to knit
 */
void Game::Executor::execKnit(Game* game){
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

	vector<uint64_t> confirm = Terminal::readCodes("Are you sure? ");
	if(confirm.size() == ONE && (confirm[0] != STR_Y && confirm[0] != STR_YES))
		return;

	execScore(game, true);
	game->on = false;

}

/*
 *	Execute command to display current score to player
 */
void Game::Executor::execScore(Game* game, bool final){
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
	stringstream ss;
	ss << game->general->get(STR_VERSION) << " (" << game->maxpoints << " points).";
	Terminal::wrpro(ss.str());
}

/*
 *	Execute XYRO magic word command
 */
void Game::Executor::execXyro(Game* game){
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
void Game::Executor::execExplain(Game* game, uint64_t arg){
	string answer = game->explanations->get(arg);
	if(answer.empty())
		answer = game->explanations->get(HINT_DEFAULT);
	Terminal::wrpro(answer);
}

/*
 *	Execute command to fly
 */
void Game::Executor::execFly(Game* game, uint64_t arg){
	if(arg == ITEM_SHIP){
		if(game->player->getLocation()->getID() == LOCATION_SHIP){
			Terminal::wrpro("You steal the pirate ship and fly away in it. Congratulations, you have successfully escaped from Asterbase Tarranta.");
			game->player->incrementScore(SCORE_ESCAPE);
			game->on = false;
			game->escaped = true;
			execScore(game, true);
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
void Game::Executor::execHint(Game* game, uint64_t arg){
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
void Game::Executor::execSay(Game* game, uint64_t arg){
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
void Game::Executor::execTether(Game* game, uint64_t arg){
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

/*
 *	Execute command to attack something
 */
void Game::Executor::execAttack(Game* game, Item* item){
	uint64_t code = item->getCode();

	if(code == ITEM_DRAGON || code == ITEM_LION || code == ITEM_WOLF)
		Terminal::wrpro(game->general->get(STR_NOWISEAT));
	
	else if(code == ITEM_BOULDER){
		if(!game->player->isStrong()){
			Terminal::wrpro(game->general->get(STR_NOSTRONG));
		}
		else{
			Location* loc = game->player->getLocation();
			loc->setDirection(CMD_DOWN, game->station->get(LOCATION_CELLAR)); // Link location to cellar
			game->destroyItem(item);
			loc->deposit(game->items->get(ITEM_DUST)); // Boulder is replaced with dust
			game->player->setStrong(false);
			game->player->incrementScore(SCORE_PUZZLE);
			Terminal::wrpro(game->general->get(STR_BOULDPUL));
		}
	}

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
	else{
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
void Game::Executor::execGive(Game* game, Item* item, Item* receiver){
	uint64_t itemcode = item->getCode();
	uint64_t reccode = receiver->getCode();

	if(reccode == ITEM_LION){ // Player wishes to feed lion
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

	else if(reccode == ITEM_SKELETON){
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

	else
		Terminal::wrpro("The " + Statics::codeToStr(reccode) + " is not interested in your " + Statics::codeToStr(itemcode));
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
 *	Execute command to launch an item
 */
void Game::Executor::execLaunch(Game* game, Item* item){
	uint64_t code = item->getCode();
	Location* loc = game->player->getLocation();

	if(code == ITEM_DINGHY && loc == game->station->get(LOCATION_LAUNCH)){
		Terminal::wrpro("You launch the dinghy into the River Amethyst. Almost immediately, the powerful current sweeps you downstream; there is no going back now.");
		game->player->setLocation(game->station->get(LOCATION_RIVERBRIDGE));
	}
	else{
		Terminal::wrpro(game->general->get(STR_NONOHOW));
	}
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
		vector<uint64_t> tune = Terminal::readCodes("What would you like to play? ");
		if(tune.empty())
			Terminal::wrpro("Do not waste my time, s'il vous plait");
		else{
			Item* listener = game->player->getLocation()->get(ITEM_LION);
			if(listener != NULL && tune.size() == ONE && tune[0] == STR_CABBAGE){
				Terminal::wrpro(game->general->get(STR_LIONTUNE));
				listener->setAttribute(CTRL_ITEM_OBSTRUCTION, false); // Lion will no longer obstruct the player
				game->player->incrementScore(SCORE_PUZZLE);
			}
			else
				Terminal::wrpro("You play \"" + Statics::codeToStr(tune[0]) + "\" on the whistle.");
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
			Terminal::wrpro("It is now in the off position.");
			if(code == ITEM_BUTTON){ // If button is pressed, turn gravity on in the anteroom
				game->station->get(LOCATION_ANTEROOM)->setAttribute(CTRL_LOC_HAS_GRAVITY, true);
				Terminal::wrpro(game->general->get(STR_NOHAPPEN));
			}
		}
		else{ // Switch had been off, so turn on
			switchable->setOn(true);
			Terminal::wrpro("It is now in the on position.");
			if(code == ITEM_BUTTON){ // If button is pressed, turn gravity off in the anteroom
				game->station->get(LOCATION_ANTEROOM)->setAttribute(CTRL_LOC_HAS_GRAVITY, false);
				Terminal::wrpro(game->general->get(STR_NOHAPPEN));
			}
		}
	}

	else
		Terminal::wrpro(game->general->get(STR_NONOHOW));

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
			Item* wire = game->items->get(ITEM_WIRE);
			if(!game->player->hasInPresent(wire))
				Terminal::wrpro("You do not have the component needed to repair the console.");
			else{
				game->destroyItem(wire); // Consume wire
				game->destroyItem(item); // Remove console

				Item* newpanel = game->items->get(ITEM_PANEL);
				newpanel->setLocation(item->getLocation());
				item->getLocation()->deposit(newpanel); // Insert panel

				game->player->incrementScore(SCORE_PUZZLE);
				Terminal::wrpro(game->general->get(STR_CONSOLE));
			}
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

	if(code == ITEM_CORSAIR){ // Player wishes to rob the corsair
		Item* key = game->items->get(ITEM_KEY);
		if(key->getLocation()->getID() != LOCATION_NOWHERE) // Corsair's item has already been stolen
			Terminal::wrpro(game->general->get(STR_PIRATNO));
		else if(game->player->hasInInventory(game->items->get(ITEM_BOOTS))){ // Player is walking too loudly and corsair hears
			Terminal::wrpro(game->general->get(STR_PIRATBOO));
			game->player->kill();
		}
		else{ // Player successfully robs corsair
			Terminal::wrpro(game->general->get(STR_PIRATRB1));
			game->player->addToInventory(key);
			key->setLocation(game->player->getLocation());
			game->player->incrementScore(SCORE_PUZZLE);
		}	
	}

	else if(code == ITEM_BUCCANEE){ // Player wishes to rob the buccaneer
		Item* medallion = game->items->get(ITEM_MEDALLIO);
		if(medallion->getLocation()->getID() != LOCATION_NOWHERE) // Buccaneer's item has already been stolen
			Terminal::wrpro(game->general->get(STR_PIRATNO));
		else if(!game->player->isInvisible()){ // Player can be seen by buccaneer
			Terminal::wrpro(game->general->get(STR_PIRATSEE));
			game->player->kill();
		}
		else{ // Player successfully robs buccanee
			Terminal::wrpro(game->general->get(STR_PIRATRB2));
			game->player->addToInventory(medallion);
			medallion->setLocation(game->player->getLocation());
			game->player->incrementScore(SCORE_PUZZLE);
		}
	}

	else
		Terminal::wrpro(game->general->get(STR_NONOHOW));
}

/*
 *	Execute command to rub an item
 */
void Game::Executor::execRub(Game* game, Item* item){
	uint64_t code = item->getCode();
	if(code == ITEM_DRAGON){ // Player wishes to rub dragon
		game->player->getLocation()->extract(item);
		item->setLocation(LOCATION_NOWHERE);
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
	else if(!item->hasAttribute(CTRL_ITEM_MOBILE) | item->hasAttribute(CTRL_ITEM_OBSTRUCTION)) // Item is not portable, or is an obstruction
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
			vector<uint64_t> confirm = Terminal::readCodes("Take " + item->getShortname() + " in " + containers.front()->getShortname() + "? ");
			if(confirm.size() == ONE && (confirm[0] == STR_Y || confirm[0] == STR_YES)){
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
				Terminal::wrpro(game->general->get(STR_NOOPTION));
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
 *	Execute command to water something with something else
 *	container is the container holding the liquid that is to be poured
 */
void Game::Executor::execWater(Game* game, Item* item, Container* container){
	uint64_t code = item->getCode();

	if(code == ITEM_BEAN){

		Item* liquid = container->extractItemWithin();
		uint64_t liqcode = liquid->getCode();
		Location* loc = game->player->getLocation();

		if(liqcode == ITEM_POTION){ // Player waters the bean with potion

			liquid->setLocation(game->station->get(LOCATION_NOWHERE)); // Liquid that was poured is consumed

			Item* plant = game->items->get(ITEM_PLANT);

			if(game->player->hasInInventory(item)){ // Bean had been in inventory, so plant goes in inventory
				game->player->extractFromInventory(item);
				game->player->addToInventory(plant);
				plant->setLocation(game->station->get(LOCATION_INVENTORY));
			}
			else{ // Bean had been at location, so plant goes to location
				loc->extract(item);
				loc->deposit(plant);
				plant->setLocation(loc);
			}

			item->setLocation(game->station->get(LOCATION_NOWHERE)); // Bean is removed
			Terminal::wrpro("You water the bean and it instantly grows into a plant. The air is more pleasant to breathe now.");
			
		}

		else if(liqcode == ITEM_WATER && loc->getID() == LOCATION_HOT){ // Player pours water on the bean where there is heat

			Item* beanstalk = game->items->get(ITEM_BEANSTAL);
			Item* blossom = game->items->get(ITEM_BLOSSOM);
			game->destroyItem(item);
			loc->deposit(beanstalk);
			loc->deposit(blossom); // Remove bean and replace with beanstalk bearing asterium blossom
			game->player->incrementScore(SCORE_PUZZLE);

			Terminal::wrpro(game->general->get(STR_BEANSTAL));

		}

		else
			Terminal::wrpro("You pour the " + liquid->getShortname() + " onto the bean, but nothing happens.");

	}

	else if(code == ITEM_PLANT){

		Item* liquid = container->extractItemWithin();
		uint64_t liqcode = liquid->getCode();

		if(liqcode == ITEM_POTION){ // Player waters the plant with potion

			liquid->setLocation(game->station->get(LOCATION_NOWHERE)); // Liquid that was poured is consumed

			Item* bean = game->items->get(ITEM_BEAN);

			if(game->player->hasInInventory(item)){ // Plant had been in inventory, so bean goes in inventory
				game->player->extractFromInventory(item);
				game->player->addToInventory(bean);
				bean->setLocation(game->station->get(LOCATION_INVENTORY));
			}
			else{ // Plant had been at location, so bean goes to location
				Location* loc = game->player->getLocation();
				loc->extract(item);
				loc->deposit(bean);
				bean->setLocation(loc);
			}

			item->setLocation(game->station->get(LOCATION_NOWHERE)); // Plant is removed
			Terminal::wrpro("You water the plant and it turns back into a bean. The air feels thinner now.");
			
		}
		else
			Terminal::wrpro("You pour the " + liquid->getShortname() + " onto the plant, but nothing happens.");

	}

	else{
		Terminal::wrpro("I see no point in pouring liquid on such a thing.");
	}


}

/*
 *	Execute command to cook an item
 */
void Game::Executor::execCook(Game* game, Item* item){
	if(!game->player->hasInPresent(game->items->get(ITEM_CAULDRON))){
		Terminal::wrpro("You have nothing to cook it in.");
		return;
	}
	uint64_t code = item->getCode();
	switch(code){
		case ITEM_RADISHES:{
			game->destroyItem(item);
			Terminal::wrpro("You cook the radishes and this produces an elixir. A large quantity of it now sits in the cauldron.");
			game->player->getLocation()->deposit(game->items->get(ITEM_ELIXIR)); // Remember that we do not set liquid to point to its location
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
 *	Execute command to give Item item, and request that item with code request be dispensed in return
 *	Requires presence of machine, and the only item that may be exchanged is the cartridge
 *	request is an item that is not the null item
 */
void Game::Executor::execExchange(Game* game, Item* item, Item* request){

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
 *	Execute command to insert item into a container
 */
void Game::Executor::execInsert(Game* game, Item* item, Container* container){
	game->player->extractFromInventory(item); // Remove item from inventory
	item->setLocation(game->station->get(LOCATION_CONTAINER)); // Set item's location to "container"
	container->insertItem(item); // Insert item into desired container
	Terminal::wrpro(item->getShortname() + " inserted into " + container->getShortname() + ".");
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