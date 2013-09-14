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
			void parseInput(Game* game, string line);
		};

		class Dispatcher{
		public:
			void dispatchMovement(Game* game, Command* command);
			void dispatchNoArg(Game* game, Command* command);
			void dispatchAnyArg(Game* game, Command* command, uint64_t arg);
			void dispatchPresentArg(Game* game, Command* command, Item* item);
			void dispatchInventoryArg(Game* game, Command* command, Item* item);
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

			void execMovement(Game* game, Location* current, Location* next);
			void execAbracada(Game* game);
			void execAvnarand(Game* game);
			void execChimbu(Game* game);
			void execCommands(Game* game);
			void execCsb(Game* game);
			void execFish(Game* game);
			void execHelp(Game* game);
			void execInventor(Game* game);
			void execKnit(Game* game);
			void execLook(Game* game);
			void execPlugh(Game* game);
			void execQuit(Game* game);
			void execScore(Game* game, bool final);
			void execSleep(Game* game);
			void execTezazzle(Game* game);
			void execVersion(Game* game);
			void execXyro(Game* game);
			void execXyzzy(Game* game);
			void execZiqua(Game* game);
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
			void execGive(Game* game, Item* item);
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
			void execFree(Game* game, Item* item);
			void execInsert(Game* game, Item* item, Container* container);
			void execPour(Game* game, Item* item);
			void execThrow(Game* game, Item* item);
		};

		Game::Parser parser;
		Game::Dispatcher dispatcher;
		Game::Executor executor;

	public:
		Game(string filename);
		~Game();
		void play();


};
