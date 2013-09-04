#include "CommandCollection.h"

/*
 *	Constructor
 */
CommandCollection::CommandCollection(BitFileReader& reader){

	string line = reader.getline();

	while(line.compare(FILE_DELIMITER_SECTION)){

		vector<string> tokens = Tokeniser::splitToVector(line, REGEX_FILE);

		int status;
		Statics::strToInt(status, tokens.at(INDEX_CSTATUS), HEX);

		uint64_t primary = Statics::strToCode(tokens.at(INDEX_CPRIMARY));

		/*
		 *	Write-enabled testing includes read-enabled testing
		 *	This arrangement prevents WRITE test commands from being compiled if READ test ones are not
		 */
		#ifdef TESTING_READ

			#ifdef TESTING_WRITE
			Command* command = new Command(status, primary); // If testing at WRITE level, allow any commands
			this->commands.insert(pair<uint64_t, Command*>(primary, command)); // Add command to collection under primary key

			for(unsigned int i=(INDEX_CSTATUS+ONE) ; i<tokens.size() ; i++){
				uint64_t cd = Statics::strToCode(tokens.at(i));
				this->commands.insert(pair<uint64_t, Command*>(cd, command)); // Add command to collection under alias keys
			}
			#endif

			#ifndef TESTING_WRITE
				if(!(status & CTRL_COMMAND_TESTING_WRITE)){ // If not testing at WRITE level, do not create WRITE test commands
					Command* command = new Command(status, primary); // Create new Command
					this->commands.insert(pair<uint64_t, Command*>(primary, command)); // Add command to collection under primary key

					for(unsigned int i=(INDEX_CSTATUS+ONE) ; i<tokens.size() ; i++){
						uint64_t cd = Statics::strToCode(tokens.at(i));
						this->commands.insert(pair<uint64_t, Command*>(cd, command)); // Add command to collection under alias keys
					}
				}
			#endif

		#endif

		#ifndef TESTING_READ
		if(!(status & CTRL_COMMAND_TESTING_READ)){ // If not testing at READ level, do not create READ test commands
			Command* command = new Command(status, primary); // Create new Command
			this->commands.insert(pair<uint64_t, Command*>(primary, command)); // Add command to collection under primary key

			for(unsigned int i=(INDEX_CSTATUS+ONE) ; i<tokens.size() ; i++){
				uint64_t cd = Statics::strToCode(tokens.at(i));
				this->commands.insert(pair<uint64_t, Command*>(cd, command)); // Add command to collection under alias keys
			}
		}
		#endif

		line = reader.getline();
	}

}

/*
 *	Destructor
 */
CommandCollection::~CommandCollection(){

	for(map<uint64_t, Command*>::iterator it = commands.begin() ; it != commands.end() ; it++){

		if(it->second != NULL){
			delete it->second;

			// Having deallocated the memory, we must now search for any other keys pointing to the same value to prevent double frees
			map<uint64_t, Command*>::iterator jt = it;
			jt++;

			while(jt != commands.end()){
				if(jt->second == it->second)
					commands[jt->first] = NULL; // NULL out any extra keys
				jt++;
			}
		}

	}

}

/*
 *	Return pointer to a command with a certain code
 *	Returns NULL if command does not exist
 */
Command* CommandCollection::get(uint64_t cd){

	map<uint64_t, Command*>::iterator it = this->commands.find(cd);

	if(it == this->commands.end()) // If no command by that code in the collection, return NULL
		return NULL;

	return it->second;

}

string CommandCollection::listNonSecret(){

	string result("I understand the following commands (and possibly others):\n");

	for(map<uint64_t, Command*>::iterator it = this->commands.begin() ; it != this->commands.end() ; it++){
		if(!it->second->isSecret())
			result += Statics::codeToStr(it->first) + ", ";
	}

	result.erase(result.length()-TWO); // Remove final ", "
	result += DOT;
	return result;

}