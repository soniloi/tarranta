#include "Game.h"

/*
 *	Parse user input
 */
void Game::Parser::parseInput(Game* game, vector<uint64_t> input){

		Command* command = NULL;
		Item* item = NULL;

		Command* tentativeCommand = game->commands->get(input[ZERO]);
		uint64_t tentativeArg = 0;

		//	Attempt to find the command in this input

		//	First try the order verb-noun
		if(tentativeCommand){

			if(tentativeCommand->isCompound() && tentativeCommand->isMovement()){ // If a compound command e.g. "go north"

				Command* suppcommand = NULL;
				if(input.size() < TWO){
					vector<uint64_t> supparg = Terminal::readCodes(game->general->get(STR_WHICHWAY));
					if(supparg.size() > ZERO)
						suppcommand = game->commands->get(supparg[ZERO]);
				}
				else
					suppcommand = game->commands->get(input[ONE]);
					
				if(!suppcommand || !suppcommand->isMovement() || suppcommand->isCompound()){
					command = NULL;
				}
				else
					command = suppcommand;
			}

			else{ // Command is normal, and in the usual order i.e. verb-noun or whatever
				command = tentativeCommand;
				if(input.size() > ONE)
					tentativeArg = input[ONE];
			}
		}

		// That didn't work, so try noun-verb
		else if(input.size() > ONE){
			tentativeCommand = game->commands->get(input[ONE]);
			if(tentativeCommand && tentativeCommand->isInvertible()){ // Not all commands may be phrased as noun-verb, don't bother with the ones that cannot
				command = tentativeCommand;
				tentativeArg = input[ZERO];
			}
			else
				command = NULL;
		}

		if(!command)
			Terminal::wrpro(game->general->get(STR_NOTUIGIN)); // Input doesn't contain a command, we tell them we don't understand and leave it at that

		else{

			// First check for testing commands, where applicable; we send these directly to the executor without going through the dispatcher
			#ifdef TESTING_READ
				#ifdef TESTING_WRITE // WRITE commands do not exist when READ ones do not
				if(command->isTestingWrite()){
					if(command->is(CTRL_COMMAND_TESTING_FLASH))
						game->testingexec->execFlash(tentativeArg);
					else if(command->is(CTRL_COMMAND_TESTING_GRAB))
						game->testingexec->execGrab(tentativeArg);
				}
				#endif
			if(command->isTestingRead()){
				if(command->is(CTRL_COMMAND_TESTING_AIMSIGH))
					game->testingexec->execAimsigh(tentativeArg);
				else if(command->is(CTRL_COMMAND_TESTING_NODE))
					game->testingexec->execNode();
			}
			#endif

			// Check for movement commands
			if(command->isMovement()){
				if(tentativeArg) // A movement command shouldn't have an argument, but this gave one anyway
					Terminal::wrpro(game->general->get(STR_NOTUIGIN));
				else
					game->dispatcher.dispatchMovement(game, command);
			}

			// Next check for commands that take exactly zero arguments
			else if(!command->takesArg()){
				if(tentativeArg) // A no-arg command shouldn't have an argument, but this gave one anyway
					Terminal::wrpro(game->general->get(STR_NOTUIGIN));
				else
					game->dispatcher.dispatchNoArg(game, command);
			}

			// Every remaining command will take an argument
			else{

				if(!tentativeArg){ // Argument required, but not given
					vector<uint64_t> supparg = Terminal::readCodes("What is it you want me to " + command->toString() + "? ");
					if(supparg.size() > ZERO)
						tentativeArg = supparg[ZERO];
				}

				// Next check for commands that take an argument, where that argument doesn't represent a particular item or anything
				if(!command->takesPresentArg())
					game->dispatcher.dispatchAnyArg(game, command, tentativeArg);
				
				// Every remaining arg-taking command requires a named item
				else{
					item = game->items->get(tentativeArg);
					if(item == game->items->get(ITEM_NULL)) // Whatever they input as argument isn't anything we know as an item
						Terminal::wrpro(game->general->get(STR_NONOWHAT));

					// First handle items that just have to be in player's vicinity, may-or-may-not be in inventory
					else if(!command->takesInventoryArg())
						game->dispatcher.dispatchPresentArg(game, command, item);

					// Lastly, deal with items that must be in the player's inventory
					else
						game->dispatcher.dispatchInventoryArg(game, command, item);
				}

			}
			
		}


}
