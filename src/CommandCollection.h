/*
 *	Class representing collection of all game commands
 */

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "Command.h"
//#include "Constants.h"
#include "BitFileReader.h"
#include "Statics.h"

using namespace std;

class CommandCollection{

	private:
		map<uint64_t, Command*> commands;

	public:
		CommandCollection(BitFileReader& reader);
		~CommandCollection();
		Command* get(uint64_t cd);
		string listNonSecret();

};
