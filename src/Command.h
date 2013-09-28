/*
 *	Class representing a command in the game
 *	A command is characterised by:
 *		a code of 64-bit type (primary string representing how it is called by user)
 *		together with a status integer (stores data about what type of command this is)
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <fstream>
#include <inttypes.h>
#include "Definitions.h"
#include "Statics.h"

class Command{
		friend ostream& operator <<(ostream&, const Command&);

	private:
		uint64_t code;
		int status;

	public:
		#if defined (TESTING_READ)
		bool isTestingRead();
		#endif
		#if defined (TESTING_READ) && defined (TESTING_WRITE)
		bool isTestingWrite();
		#endif
		friend class CommandCollection;
		Command(int stat, uint64_t cd);
		bool is(uint64_t cmdcode);
		bool isCompound();
		bool isMovement();
		bool isInvertible();
		bool isSecret();
		bool takesArg();
		bool takesPresentArg();
		bool takesInventoryArg();
		uint64_t getCode();
		string toString();

};

#endif
