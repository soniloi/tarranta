#include "Game.h"

/*
 *	Constructor
 */
Game::PresentargExecutor::PresentargExecutor(Game* game){
	this->game = game;
}

/*
 *	Execute command to attack something
 */
void Game::PresentargExecutor::execAttack(Item* item){
	uint64_t code = item->getCode();

	if(code == ITEM_DRAGON || code == ITEM_LION || code == ITEM_WOLF || code == ITEM_DOGS)
		Terminal::wrpro(game->general->get(STR_NOWISEAT));
	
	else if(code == ITEM_BOULDER){
		if(!game->player->isStrong()){
			Terminal::wrpro(game->general->get(STR_NOSTRONG));
		}
		else{
			Location* loc = game->player->getLocation();
			Location* cellar = game->station->get(LOCATION_CELLAR);
			loc->setDirection(CMD_DOWN, cellar); // Link location to cellar
			cellar->setDirection(CMD_UP, loc); // Link cellar to location
			game->retireItem(item);

			Item* dust = game->items->get(ITEM_DUST);
			dust->setLocation(loc);
			loc->deposit(dust); // Boulder is replaced with dust

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
void Game::PresentargExecutor::execBurn(Item* item){

	if(!game->player->hasInInventory(game->items->get(ITEM_MATCHES))){
		Terminal::wrpro(game->general->get(STR_NOMATCH));
		return;
	}

	uint64_t code = item->getCode();
	if(code == ITEM_BOOK)
		Terminal::wrpro(game->general->get(STR_PHILISTI));
	else if(code == ITEM_BREAD){ // Burn bread and replace with toast
		game->retireItem(item);
		Terminal::wrpro(game->general->get(STR_BREAD));
		Item* toast = game->items->get(ITEM_TOAST);
		if(!game->player->canAccept(toast)){ // Toast will not fit in inventory
			Location* loc = game->player->getLocation();
			loc->deposit(toast);
			toast->setLocation(loc);
		}
		else{ // Toast will fit in inventory
			game->player->addToInventory(toast);
			toast->setLocation(game->station->get(LOCATION_INVENTORY));
		}
	}
	else if(code == ITEM_TOAST){ // Burn toast and replace with ash
		game->retireItem(item);
		Terminal::wrpro(game->general->get(STR_TOAST));
		if(game->player->getLocation()->getID() == LOCATION_AIRLOCKE){
			Terminal::wrpro(game->general->get(STR_TOASTALM));
			Location* loc = game->player->getLocation();
			loc->setAttribute(CTRL_LOC_HAS_AIR, false); // Air has been blown out of the airlock
			loc->setDirection(CMD_SOUTHWES, game->station->get(LOCATION_AIRLOCKEOUT));
			game->player->incrementScore(SCORE_PUZZLE);
		}
		Terminal::wrpro(game->general->get(STR_ASHMOUSE));
	}
	else{
		Terminal::wrpro(game->general->get(STR_NONOHOW));
		
	}
}

/*
 *	Execute command to describe an item
 */
void Game::PresentargExecutor::execDescribe(Item* item){
	Terminal::wrpro("It is " + item->getFullname() + DOT);
}

/*
 *	Execute command to empty container
 */
void Game::PresentargExecutor::execEmpty(Container* container){

	Item* itemwithin = container->extractItemWithin();

	if(!itemwithin){ // Nothing in the container
		Terminal::wrpro("It is already empty.");
		return;
	}

	if(itemwithin->hasAttribute(CTRL_ITEM_LIQUID)){ // Liquids do not survive out of containers, and get poured away
		Terminal::wrpro("You pour the " + itemwithin->getShortname() + " out of the " + container->getShortname() + ". It falls to the ground and quickly seeps away.");
		itemwithin->setLocation(game->station->get(LOCATION_NOWHERE));
	}

	else{ // Non-liquids get taken into inventory, even if container had been at location; this prevents fragile item cheating
		if(!game->player->canAccept(itemwithin)) // Only exception is where they would not fit in inventory
			Terminal::wrpro("The item within is larger than you can carry right now. You will have to drop something else first.");
		else{
			game->player->addToInventory(itemwithin);
			itemwithin->setLocation(game->station->get(LOCATION_INVENTORY));
			Terminal::wrpro("You take the " + itemwithin->getLongname() + " out of the " + container->getShortname() + ".");
		}
	}

}

/*
 *	Execute command to ignore something or someone
 */
void Game::PresentargExecutor::execIgnore(Item* item){
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
void Game::PresentargExecutor::execLight(SwitchableItem* switchable){
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
void Game::PresentargExecutor::execPlay(Item* item){
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
void Game::PresentargExecutor::execPush(Item* item){

	uint64_t code = item->getCode();

	if(item->hasAttribute(CTRL_ITEM_SWITCHABLE)){
		SwitchableItem* switchable = (SwitchableItem*) item;
		if(switchable->isOn()){ // Switch had been on, so turn off
			switchable->setOn(false);
			Terminal::wrpro("It is now in the off position.");
			if(code == ITEM_BUTTON){ // If button is pressed, turn gravity on in the anteroom
				game->station->get(LOCATION_ANTEROOM)->setAttribute(CTRL_LOC_HAS_GRAVITY, true);
				game->station->get(LOCATION_OBSERVATORY)->setAttribute(CTRL_LOC_HAS_GRAVITY, true);
				Terminal::wrpro(game->general->get(STR_NOHAPPEN));
			}
		}
		else{ // Switch had been off, so turn on
			switchable->setOn(true);
			Terminal::wrpro("It is now in the on position.");
			if(code == ITEM_BUTTON){ // If button is pressed, turn gravity off in the anteroom
				game->station->get(LOCATION_ANTEROOM)->setAttribute(CTRL_LOC_HAS_GRAVITY, false);
				game->station->get(LOCATION_OBSERVATORY)->setAttribute(CTRL_LOC_HAS_GRAVITY, false);
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
void Game::PresentargExecutor::execQuench(SwitchableItem* switchable){
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
void Game::PresentargExecutor::execRead(Item* item){
	string writing = item->getWriting();
	if(writing.empty()) // Item has no writing on it
		Terminal::wrpro(game->general->get(STR_NOWRITIN));
	else // Item has writing on it
		Terminal::wrpro("It reads: \"" + writing + "\".");
}

/*
 *	Execute command to effect repairs to something
 */
void Game::PresentargExecutor::execRepair(Item* item){
		uint64_t code = item->getCode();

		if(code == ITEM_CONSOLE){
			Item* wire = game->items->get(ITEM_WIRE);
			if(!game->player->hasInPresent(wire))
				Terminal::wrpro("You do not have the component needed to repair the console.");
			else{
				Location* loc = item->getLocation();
				Item* newpanel = game->items->get(ITEM_PANEL);
				newpanel->setLocation(loc);
				loc->deposit(newpanel); // Insert panel

				game->retireItem(wire); // Consume wire
				game->retireItem(item); // Remove console

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
void Game::PresentargExecutor::execRob(Item* item){
	uint64_t code = item->getCode();

	if(code == ITEM_CORSAIR){ // Player wishes to rob the corsair
		Item* key = game->items->get(ITEM_KEY);
		if(key->getLocation()->getID() != LOCATION_NOWHERE) // Corsair's item has already been stolen
			Terminal::wrpro(game->general->get(STR_PIRATNO));
		else if(game->player->hasInInventory(game->items->get(ITEM_BOOTS))){ // Player is walking too loudly and corsair hears
			Terminal::wrpro(game->general->get(STR_PIRATBOO));
			game->player->kill();
		}
		else if(!game->player->canAccept(key)){ // No space in inventory
			Terminal::wrpro("You will not be able to carry what the corsair has. You must drop something first.");
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
		else if(!game->player->canAccept(medallion))
			Terminal::wrpro("You will not be able to carry what the buccaneer has. You must drop something first.");
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
void Game::PresentargExecutor::execRub(Item* item){
	uint64_t code = item->getCode();
	if(code == ITEM_LAMP) // Player wishes to rub lamp
		Terminal::wrpro(game->general->get(STR_GENIE));
	else if(code == ITEM_DRAGON){ // Player wishes to rub dragon
		game->player->getLocation()->extract(item);
		item->setLocation(LOCATION_NOWHERE);
		Terminal::wrpro(game->general->get(STR_DRAGON));
		game->player->incrementScore(SCORE_PUZZLE);

		Item* tooth = game->items->get(ITEM_TOOTH);
		game->player->getLocation()->deposit(tooth); // Dragon leaves tooth behind
		tooth->setLocation(game->player->getLocation());
	}
	else if(code == ITEM_PENDANT){ // Rubbing pendant sends rod treasure to Thor room in a flash of lightning
		Location* thor = game->station->get(LOCATION_THOR);
		Item* rod = game->items->get(ITEM_ROD);
		game->destroyItem(rod); // Remove rod from wherever it was
		rod->setLocation(thor);
		thor->deposit(rod);
		Terminal::wrpro(game->general->get(STR_PENDRUB));
	}

	else
		Terminal::wrpro(game->general->get(STR_NOINTERE));

}

/*
 *	Execute command to take item into inventory
 */
void Game::PresentargExecutor::execTake(Item* item){
	if(!game->player->hasLight()) // Player cannot see what they are doing
		Terminal::wrpro(game->general->get(STR_TAKENOSE));
	else if(game->player->hasInInventory(item)) // Item is already in inventory
		Terminal::wrpro(game->general->get(STR_TAKEALRE));
	else if(!item->hasAttribute(CTRL_ITEM_MOBILE) | item->hasAttribute(CTRL_ITEM_OBSTRUCTION)) // Item is not portable, or is an obstruction
		Terminal::wrpro(game->general->get(STR_TAKENOCA));
	else if(!game->player->canAccept(item)) // Player is already carrying too many things
		Terminal::wrpro("That item is larger than you can carry right now. You must drop something else first.");
	else if(item->hasAttribute(CTRL_ITEM_LIQUID)){ // Liquids cannot be carried directly, but must be in containers

		if(item->getLocation() == game->station->get(LOCATION_CONTAINER)){ // Liquid is currently in a container somewhere else, so we prevent player from taking more until that disappears
			Terminal::wrpro(game->general->get(STR_ENOUGH));
			return;
		}

		list<Container*> containers = game->player->getSuitableInventoryContainers(item); // Search for containers the player already has

		if(containers.size() == ZERO){ // No available container to place item into
			Terminal::wrpro(game->general->get(STR_NOCONTAI));
		}

		else if(containers.size() == ONE){ // Exactly one available container
			if(game->confirm("Take " + item->getShortname() + " in " + containers.front()->getShortname() + "? ")){
				if(item == containers.front()) // Smartarse wants to insert the item into itself
					Terminal::wrpro(game->general->get(STR_CONTRECU));
				else if(!containers.front()->isEmpty()) // Something is currently in container
					Terminal::wrpro(game->general->get(STR_CONTFULL));
				else{
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
		if(itemloc != NULL && !game->player->hasInInventory(item)){
			if(itemloc->getID() == LOCATION_CONTAINER){ // Item is inside a container at location
				Container* container = game->player->getLocation()->getParentContainer(item);
				if(container == NULL)
					Terminal::wrpro(game->general->get(STR_ERROR)); // This should not happen anyway
				else
					container->extractItemWithin();
			}
			else // Item is just normally at location
				itemloc->extract(item);
		}


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
void Game::PresentargExecutor::execWater(Item* item, Container* container){
	uint64_t code = item->getCode();

	if(code == ITEM_BEAN){

		Item* liquid = container->extractItemWithin();
		uint64_t liqcode = liquid->getCode();
		Location* loc = game->player->getLocation();

		if(liqcode == ITEM_POTION){ // Player waters the bean with potion

			liquid->setLocation(game->station->get(LOCATION_NOWHERE)); // Liquid that was poured is consumed

			Item* plant = game->items->get(ITEM_PLANT);

			if(game->player->hasInInventory(item)){ // Bean had been in inventory, so plant goes in inventory in preference
				game->player->extractFromInventory(item);
				if(!game->player->canAccept(plant)){ // Plant does not fit in inventory
					loc->deposit(plant);
					plant->setLocation(loc);
					Terminal::wrpro("The plant is larger than you can carry right now, so you set it down.");
				}
				else{ // Plant fits in inventory
					game->player->addToInventory(plant);
					plant->setLocation(game->station->get(LOCATION_INVENTORY));
				}
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
			game->retireItem(item);
			loc->deposit(beanstalk);
			loc->deposit(blossom); // Remove bean and replace with beanstalk bearing asterium blossom
			beanstalk->setLocation(loc);
			blossom->setLocation(loc);
			game->player->incrementScore(SCORE_PUZZLE);

			Terminal::wrpro(game->general->get(STR_BEANSTAL));

		}

		else
			Terminal::wrpro("You pour the " + liquid->getShortname() + " onto the bean, but nothing happens.");

	}

	else if(code == ITEM_PLANT){

		Item* liquid = container->extractItemWithin();
		uint64_t liqcode = liquid->getCode();
		Location* loc = game->player->getLocation();

		if(liqcode == ITEM_POTION){ // Player waters the plant with potion

			liquid->setLocation(game->station->get(LOCATION_NOWHERE)); // Liquid that was poured is consumed

			Item* bean = game->items->get(ITEM_BEAN);

			if(game->player->hasInInventory(item)){ // Plant had been in inventory, so bean goes in inventory in preference
				game->player->extractFromInventory(item);
				if(!game->player->canAccept(bean)){ // Bean does not fit in inventory
					loc->deposit(bean);
					bean->setLocation(loc);
					Terminal::wrpro("The bean is larger than you can carry right now, so you set it down.");
				}
				else{ // Bean fits in inventory
					game->player->addToInventory(bean);
					bean->setLocation(game->station->get(LOCATION_INVENTORY));
				}
			}
			else{ // Plant had been at location, so bean goes to location
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
