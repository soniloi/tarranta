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
#include "FileReader.h"
#include "Statics.h"

using namespace std;

class CommandCollection{

	private:
		map<uint64_t, Command*> commands;

	public:
		CommandCollection(FileReader& reader);
		~CommandCollection();
		Command* get(uint64_t cd);
		string listNonSecret();

};
