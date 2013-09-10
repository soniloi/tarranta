#include "Game.h"

/*
 *	Constructor
 */
Game::Game(string filename){

	FileReader filein(filename);

	this->commands = new CommandCollection(filein);
	this->station = new Station(filein);
 	this->items = new ItemCollection(filein, station);
 	this->hints = new StringCollection(filein);
 	this->general = new StringCollection(filein);
 	this->randoms = new StringCollection(filein);
 	this->player = new Player(this->station->get(LOCATION_START_0), this->station->get(LOCATION_SAFE_0));
 	this->on = false;

}

/*
 *	Destructor
 */
Game::~Game(){
	delete this->commands;
	delete this->station;
	delete this->items;
	delete this->player;
	delete this->hints;
	delete this->general;
	delete this->randoms;
	Terminal::reset();
}

/*
 *	Calculate the player's score
 *	This is taken both from the player's standing score from puzzles solved,
 *		together with the number of treasures they currently have in the right place
 */
int Game::calculateScore(){
	int score = this->player->getScore(); // Start with the player's achievement-based scores

	int treasureCount = station->get(LOCATION_ESCAPE)->countTreasures();
	score += (treasureCount * SCORE_TREASURE); // Add the treasure-based scores

	return score;
}

/*
 *	Call this to play this instance of a game
 */
void Game::play(){

	/*
	 *	When testing at WRITE level, give player some automatic starting items
	 */
	#ifdef TESTING_WRITE

	this->executor.execGrab(this, ITEM_LAMP); // Grab lamp
	this->executor.execLight(this, (SwitchableItem*) this->items->get(ITEM_LAMP)); // Switch lamp on

	#endif

	Terminal::wrpro(general->get(STR_INITIAL));

	if(player->hasLight())
		Terminal::wrpro(player->getLocationArrival());
	else
		Terminal::wrpro(general->get(STR_CANTSEE));

	this->on = true;
	while(this->on){

		string line = Terminal::rdloc(player->getLocation());

		if(!line.empty()){

			this->parser.parseInput(this, line);
			
			this->player->incrementMoves();

			if(!player->hasAir()){ // Something player did means they no longer have air; instant death
				Terminal::wrpro("There is no air here to breathe. You suffocate.");
				player->kill();
			}

			if(!player->hasGravity() && !player->getLocation()->hasAttribute(CTRL_LOC_HAS_CEILING) && 
				player->getLocation()->getDirection(CMD_UP) == station->get(LOCATION_NOWHERE)){ // Player drifts away because no gravity and outdoors
				Terminal::wrpro("With no gravity to keep you here you float up, up, and away from the asteroid's surface, never to be seen again.");
				player->kill();
			}

			if(!player->isAlive()){ // Something in this move killed the player, see whether they want to live again or quit now
				Terminal::wrpro(this->general->get(STR_DESREINC));
				string reanimate = Terminal::rdstr(general->get(STR_ASKREINC));
				if(!reanimate.compare("y") || !reanimate.compare("yes")){
					this->player->reincarnate();
					this->forgetReturnLocation();
					Terminal::wrpro(general->get(STR_DOREINC));
					Terminal::wrtab(player->getLocationArrival());
				}
				else
					this->executor.execQuit(this);
			}

			else{ // Issue any warnings due, but only if player is not dead
				if(!player->hasLight())
					Terminal::wrpro(general->get(STR_LAMPNO));

			}


		}

	}

}
