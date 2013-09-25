/*
 *	Class representing an instance of the Tarranta game
 */

#include <sstream>
#include <algorithm>
#include "CommandCollection.h"
#include "Station.h"
#include "ItemCollection.h"
#include "StringCollection.h"
#include "Player.h"

class Game{

	private:
		CommandCollection* commands; // The collection of all possible game commands
		Station* station; // The collection of all game locations
		ItemCollection* items; // The collection of all game items and fixtures
		Player* player; // The player
		StringCollection* hints; // A collection of strings representing hints
		StringCollection* explanations; // A collection of strings representing explanations, or hints you can get for free
		StringCollection* general; // A collection of general strings
		StringCollection* randomevents; // A collection of strings representing random game events
		map<int, uint64_t> eventturns; // Maps turn numbers to the key for a random event
		int maxpoints;
		bool on; // Whether this game is currently running
		bool escaped; // Whether the player has escaped the asterbase

		int calculateScore();

		/*
		 *	Prevent player from going 'back'
		 */
		inline void forgetReturnLocation(){
			player->getLocation()->setDirection(CMD_BACK, station->get(LOCATION_NOWHERE)); // Set the return location to the nowhere place
		}

		/*
		 *	Send an item to graveyard; this represents 'normal' end of life for that item
		 */
		inline void retireItem(Item* item){
			if(!player->hasInInventory(item)) // Item was at location, not in inventory
				item->getLocation()->extract(item);
			else // Item was in inventory, not at location
				player->extractFromInventory(item);
			item->setLocation(station->get(LOCATION_GRAVEYARD));
		}

		/*
		 *	Send an item to nowhere; this represents destruction of that item
		 */
		inline void destroyItem(Item* item){
			if(!player->hasInInventory(item)) // Item was at location, not in inventory
				item->getLocation()->extract(item);
			else // Item was in inventory, not at location
				player->extractFromInventory(item);
			item->setLocation(station->get(LOCATION_NOWHERE));
		}

		/*
		 *	Generic confirmation sequence
		 */
		inline bool confirm(string prompt){
			for(int i = 0 ; i < MAX_RESPONSE_TRIES ; i++){
				vector<uint64_t> confirmation = Terminal::readCodes(prompt);
				if(confirmation.size() == ONE){
					if(confirmation[0] == STR_Y || confirmation[0] == STR_YES)
						return true;
					else if(confirmation[0] == STR_N || confirmation[0] == STR_NO){
						Terminal::wrpro(this->general->get(STR_OK));
						return false;
					}
					else
						Terminal::wrpro(this->general->get(STR_AMBIG));
				}
			}
			Terminal::wrpro(this->general->get(STR_ASSUMENO)); // Player has run out of attempts, default to no/false
			return false;
		}

		class Parser{
		public:
			void parseInput(Game* game, vector<uint64_t> input);
		};

		class Dispatcher{
		public:
			void dispatchMovement(Game* game, Command* command);
			void dispatchNoArg(Game* game, Command* command);
			void dispatchAnyArg(Game* game, Command* command, uint64_t arg);
			void dispatchPresentArg(Game* game, Command* command, Item* item);
			void dispatchInventoryArg(Game* game, Command* command, Item* item);
		};

		class MovementExecutor{ // Class handling movement commands
		private:
			Game* game;
		public:
			MovementExecutor(Game* game);
			void execMovement(Location* current, Location* next);
		};

		class NoargExecutor{ // Class handling commands that no user-input arguments
		private:
			Game* game;
		public:
			NoargExecutor(Game* game);
			void execAbracada();
			void execAvnarand();
			void execChimbu();
			void execCommands();
			void execCsb();
			void execFish();
			void execHelp();
			void execInventor();
			void execKnit();
			void execLook();
			void execPlugh();
			void execQuit();
			void execScore(bool final);
			void execSleep();
			void execTezazzle();
			void execVersion();
			void execXyro();
			void execXyzzy();
			void execZiqua();
		};

		class AnyargExecutor{ // Class handling commands that have one argument of unspecified type
		private:
			Game* game;
		public:
			AnyargExecutor(Game* game);
			void execCall(uint64_t arg);
			void execExplain(uint64_t arg);
			void execFly(uint64_t arg);
			void execHint(uint64_t arg);
			void execSay(uint64_t arg);
			void execTether(uint64_t arg);
		};

		class PresentargExecutor{ // Class handling commands that have one argument, an Item that must be present in player's vicinity
		private:
			Game* game;
		public:
			PresentargExecutor(Game* game);
			void execAttack(Item* item);
			void execBurn(Item* item);
			void execDescribe(Item* item);
			void execEmpty(Container* container);
			void execIgnore(Item* item);
			void execLight(SwitchableItem* switchable);
			void execOpen(Item* item);
			void execPlay(Item* item);
			void execPush(Item* item);
			void execQuench(SwitchableItem* switchable);
			void execRead(Item* item);
			void execRepair(Item* item);
			void execRob(Item* item);
			void execRub(Item* item);
			void execTake(Item* item);
			void execWater(Item* item, Container* container);
		};

		class InventoryargExecutor{ // Class handling commands that have one argument, an Item that the player is carrying
		private:
			Game* game;
		public:
			InventoryargExecutor(Game* game);
			void execCook(Item* item);
			void execDrink(Item* item);
			void execDrop(Item* item);
			void execEat(Item* item);
			void execExchange(Item* item, Item* request);
			void execFree(Item* item);
			void execGive(Item* item, Item* receiver);
			void execInsert(Item* item, Container* container);
			void execPour(Item* item);
			void execThrow(Item* item);
		};

		class TestingExecutor{ // Conditionally-compiled class used for various levels of testing
		private:
			Game* game;
		public:
			TestingExecutor(Game* game);
			#ifdef TESTING_READ
			void execNode();
			#endif

			#ifdef TESTING_WRITE
			void execFlash(uint64_t arg);
			void execGrab(uint64_t arg);
			#endif
		};

		Game::Parser parser;
		Game::Dispatcher dispatcher;

		Game::MovementExecutor* movementexec;
		Game::NoargExecutor* noargexec;
		Game::AnyargExecutor* anyargexec;
		Game::PresentargExecutor* presentargexec;
		Game::InventoryargExecutor* inventoryargexec;

		#ifdef TESTING_READ
		Game::TestingExecutor* testingexec;
		#endif

	public:
		Game(string filename);
		~Game();
		void play();

};
