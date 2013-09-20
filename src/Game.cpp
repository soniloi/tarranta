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

	Terminal::reset();
}

/*
 *	Calculate the player's score
 *	This is taken both from the player's standing score from puzzles solved,
 *		together with the number of treasures they currently have in the right place
 */
int Game::calculateScore(){
	int score = this->player->getScore(); // Start with the player's achievement-based scores

	Location* trove = this->station->get(LOCATION_TREASURESTORE);
	int treasureCount = trove->countItemsWithAttribute(CTRL_ITEM_TREASURE);

	if(this->escaped && (trove->getDirection(CMD_SOUTH)->getID() == LOCATION_SHIP))
		score += (treasureCount * SCORE_TREASURE); // Add the treasure-based scores at double value for escaping with them
	else
		score += (treasureCount * (SCORE_TREASURE >> ONE)); // Add the treasure-based scores at ordinary value for not escaping with them

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

		vector<uint64_t> input = Terminal::readCodesLocation(player->getLocation()->getStub());

		if(!input.empty()){

			this->parser.parseInput(this, input);
			
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
				vector<uint64_t> reanimate = Terminal::readCodes(general->get(STR_ASKREINC));
				if(reanimate.size() == ONE && (reanimate[0] == STR_Y || reanimate[0] == STR_YES)){
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
					Terminal::wrpro("You have been wandering around so long here that you die of exhaustion.");
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
