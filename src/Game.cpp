#include "Game.h"

/*
 *	Constructor
 */
Game::Game(string filename){

	srand(time(0)); // Seed for random number generator

	int linecount = ONE; // Lines in datafile are one-indexed
	int puzzlecount = ZERO; // Number of strings found that represent solved puzzled

	FileReader filein(filename);

	this->commands = new CommandCollection(filein, linecount);
	linecount++; // Skip section divider
	this->station = new Station(filein, linecount);
	linecount++; // Skip section divider
 	this->items = new ItemCollection(filein, linecount, station);
 	linecount++; // Skip section divider
 	this->hints = new StringCollection(filein, linecount, puzzlecount);
 	linecount++; // Skip section divider
 	this->explanations = new StringCollection(filein, linecount, puzzlecount);
 	linecount++; // Skip section divider
 	this->general = new StringCollection(filein, linecount, puzzlecount);
 	linecount++; // Skip section divider
 	this->randomevents = new StringCollection(filein, linecount, puzzlecount);
 	linecount++; // Skip section divider

 	this->player = new Player(this->station->get(LOCATION_START_0), this->station->get(LOCATION_SAFE_0));

 	this->maxpoints = (puzzlecount*SCORE_PUZZLE);
 	this->maxpoints += (this->items->countItemsWithAttribute(CTRL_ITEM_TREASURE)*SCORE_TREASURE);
 	this->maxpoints += SCORE_ESCAPE;

 	for(map<uint64_t, string>::iterator it = this->randomevents->strings.begin() ; it != this->randomevents->strings.end() ; it++){
		int move;
		do{
			move = rand()%MAX_MOVES_EVENT;
		}while(this->eventturns.find(move) != this->eventturns.end()); // Prevent more than one event occurring on a given move
 		this->eventturns[move] = it->first; // Allocate turn to random event
 	}

 	this->on = false;
 	this->escaped = false;

	this->movementexec = new MovementExecutor(this);
	this->noargexec = new NoargExecutor(this);
	this->anyargexec = new AnyargExecutor(this);
	this->presentargexec = new PresentargExecutor(this);
	this->inventoryargexec = new InventoryargExecutor(this);

	#if defined (TESTING_READ)
	this->testingexec = new TestingExecutor(this);
	#endif

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
	delete this->explanations;
	delete this->general;
	delete this->randomevents;

	delete this->movementexec;
	delete this->noargexec;
	delete this->anyargexec;
	delete this->presentargexec;
	delete this->inventoryargexec;

	#if defined (TESTING_READ)
	delete this->testingexec;
	#endif

	Terminal::reset();
}

/*
 *	Calculate the player's score
 *	This is taken both from the player's standing score from puzzles solved,
 *		together with the number of treasures they currently have in the right place
 *	If player has not flown away, only treasures from shuttle are counted, at half value
 *	If player has flown away but without tethering shuttle to ship, treasures from ship	and
 *		inventory are counted at full value, and treasures from shuttle are counted at half value
 *	If player has flown away and ship is tethered, treasures from shuttle, ship, and inventory
 *		are counted at full value
 */
int Game::calculateScore(){
	int score = this->player->getScore(); // Start with the player's achievement-based scores

	Location* trove = this->station->get(LOCATION_TREASURESTORE);
	int troveTreasureCount = trove->countItemsWithAttribute(CTRL_ITEM_TREASURE);

	if(this->escaped){
		Location* ship = this->station->get(LOCATION_SHIP);
		int shipTreasureCount = ship->countItemsWithAttribute(CTRL_ITEM_TREASURE);
		score += (shipTreasureCount * SCORE_TREASURE); // Add the ship's treasure-based scores at full value for escaping with them

		if(trove->getDirection(CMD_SOUTH) == ship)
			score += (troveTreasureCount * SCORE_TREASURE); // Add the treasure-based scores at full value for
		else
			score += (troveTreasureCount * (SCORE_TREASURE >> ONE)); // Add the treasure-based scores at half value for not escaping with them
	}

	else
		score += (troveTreasureCount * (SCORE_TREASURE >> ONE)); // Add the treasure-based scores at half value for not escaping with them

	if(score < 0)
		score = 0; // Don't be mean

	return score;
}

/*
 *	Call this to play this instance of a game
 */
void Game::play(){

	/*
	 *	When testing at WRITE level, give player some automatic starting items
	 */
	#if defined (TESTING_READ) && defined (TESTING_WRITE)
	this->testingexec->execGrab(ITEM_LAMP); // Grab lamp
	this->presentargexec->execLight((SwitchableItem*) this->items->get(ITEM_LAMP)); // Switch lamp on
	#endif

	Terminal::wrpro(general->get(STR_INITIAL));

	if(player->hasLight())
		Terminal::wrpro(player->getLocationArrival());
	else
		Terminal::wrpro(general->get(STR_CANTSEE));

	this->on = true;
	while(this->on){

		vector<uint64_t> input = Terminal::readCodesLocation(player->getLocation()->getStub());

		if(!input.empty()){

			this->parser.parseInput(this, input);
			
			this->player->incrementMoves();

			if(!player->hasAir()){ // Something player did means they no longer have air; instant death
				Terminal::wrpro(this->general->get(STR_NOAIR));
				player->kill();
			}

			else if(!player->hasLand()){ // Player has ended up in open water
				Terminal::wrpro(this->general->get(STR_DROWN));
				player->kill();
			}

			else if(!player->hasGravity() && !player->getLocation()->hasAttribute(CTRL_LOC_HAS_CEILING) && 
				player->getLocation()->getDirection(CMD_UP) == station->get(LOCATION_NOWHERE)){ // Player drifts away because no gravity and outdoors
				Terminal::wrpro(this->general->get(STR_NOGRAV));
				player->kill();
			}

			if(!player->isAlive()){ // Something in this move killed the player, see whether they want to live again or quit now
				Terminal::wrpro(this->general->get(STR_DESREINC));
				if(this->confirm(general->get(STR_ASKREINC))){
					this->player->reincarnate();
					this->forgetReturnLocation();
					Terminal::wrpro(general->get(STR_DOREINC));
					Terminal::wrtab(player->getLocationArrival());
				}
				else
					this->noargexec->execQuit();
			}

			else if(this->on){ // Perform the turn-based checks, but only if the game is still going

				if(!player->hasLight())
					Terminal::wrpro(general->get(STR_LAMPNO));

				int move = this->player->getMoves();

				if(move >= MAX_MOVES){
					Terminal::wrpro(this->general->get(STR_EXHAUST));
					this->noargexec->execScore(true);
					this->on = false;
				}

				else if(this->eventturns.find(move) != this->eventturns.end()){ // If there is a random event here to print, show it
					Terminal::wrpro(this->randomevents->get(this->eventturns[move]));
				}

			}

		}

	}

}
