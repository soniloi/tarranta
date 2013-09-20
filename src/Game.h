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

		inline void forgetReturnLocation(){
			player->getLocation()->setDirection(CMD_BACK, station->get(LOCATION_NOWHERE)); // Set the return location to the nowhere place
		}

		inline void destroyItem(Item* item){
			if(!player->hasInInventory(item)) // Item was at location, not in inventory
				item->getLocation()->extract(item);
			else // Item was in inventory, not at location
				player->extractFromInventory(item);
			item->setLocation(station->get(LOCATION_NOWHERE));
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

		class Executor{
		public:

			#ifdef TESTING_READ
			void execNode(Game* game);
			#endif

			#ifdef TESTING_WRITE
			void execFlash(Game* game, uint64_t arg);
			void execGrab(Game* game, uint64_t arg);
			#endif

			void execCall(Game* game, uint64_t arg);
			void execExplain(Game* game, uint64_t arg);
			void execFly(Game* game, uint64_t arg);
			void execHint(Game* game, uint64_t arg);
			void execSay(Game* game, uint64_t arg);
			void execTether(Game* game, uint64_t arg);
			void execAttack(Game* game, Item* item);
			void execBurn(Game* game, Item* item);
			void execDescribe(Game* game, Item* item);
			void execEmpty(Game* game, Container* container);
			void execGive(Game* game, Item* item, Item* receiver);
			void execIgnore(Game* game, Item* item);
			void execLaunch(Game* game, Item* item);
			void execLight(Game* game, SwitchableItem* switchable);
			void execLock(Game* game, Item* item);
			void execOpen(Game* game, Item* item);
			void execPlay(Game* game, Item* item);
			void execPush(Game* game, Item* item);
			void execQuench(Game* game, SwitchableItem* switchable);
			void execRead(Game* game, Item* item);
			void execRepair(Game* game, Item* item);
			void execRob(Game* game, Item* item);
			void execRub(Game* game, Item* item);
			void execTake(Game* game, Item* item);
			void execUnlock(Game* game, Item* item);
			void execWater(Game* game, Item* item, Container* container);
			void execCook(Game* game, Item* item);
			void execDrink(Game* game, Item* item);
			void execDrop(Game* game, Item* item);
			void execEat(Game* game, Item* item);
			void execExchange(Game* game, Item* item, Item* request);
			void execFree(Game* game, Item* item);
			void execInsert(Game* game, Item* item, Container* container);
			void execPour(Game* game, Item* item);
			void execThrow(Game* game, Item* item);
		};

		Game::Parser parser;
		Game::Dispatcher dispatcher;
		Game::Executor executor;

		Game::MovementExecutor* movementexec;
		Game::NoargExecutor* noargexec;

	public:
		Game(string filename);
		~Game();
		void play();


};
