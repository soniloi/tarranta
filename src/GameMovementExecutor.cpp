#include "Game.h"

/*
 *	Constructor
 */
Game::MovementExecutor::MovementExecutor(Game* game){
	this->game = game;
}

/*
 *	Execute a movement command
 */
void Game::MovementExecutor::execMovement(Location* current, Location* next){

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
