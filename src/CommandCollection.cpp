#include "CommandCollection.h"

/*
 *	Constructor
 *	Commands to be included depends on testing level; these are conditionally compiled
 */
CommandCollection::CommandCollection(FileReader& reader, int& linecount){

	string line = reader.getline();

	while(line.compare(FILE_DELIMITER_SECTION)){

		vector<string> tokens = Tokeniser::splitToVector(line, REGEX_FILE);

		if(tokens.size() < MIN_TOKENS_COMMAND){ // Basic range check
			cerr << "Bad or corrupt datafile: insufficient tokens on line " << linecount << endl;
			exit(EXIT_FAILURE);
		}

		int status;
		Statics::strToInt(status, tokens.at(INDEX_CSTATUS), HEX);

		uint64_t primary = Statics::strToCode(tokens.at(INDEX_CPRIMARY));

		if(this->commands.find(primary) != this->commands.end()){ // Command name is not unique
			cerr << "Bad or corrupt datafile: duplicate command name on line " << linecount << endl;
			exit(EXIT_FAILURE);
		}

		#if defined (TESTING_WRITE) && defined (TESTING_READ) // Testing at WRITE level; allow any commands
		Command* command = new Command(status, primary);
		this->commands.insert(pair<uint64_t, Command*>(primary, command)); // Add command to collection under primary key

		for(unsigned int i=(INDEX_CSTATUS+ONE) ; i<tokens.size() ; i++){
			uint64_t cd = Statics::strToCode(tokens.at(i));
			if(cd != primary && this->commands.find(cd) != this->commands.end()){ // Command key is not unique
				cerr << "Bad or corrupt datafile: duplicate command name on line " << linecount << endl;
				exit(EXIT_FAILURE);
			}			
			this->commands.insert(pair<uint64_t, Command*>(cd, command)); // Add command to collection under alias keys
		}

		#elif defined (TESTING_READ) // Testing at READ level; allow READ commands, but not WRITE commands
		if(!(status & CTRL_COMMAND_TESTING_WRITE)){
			Command* command = new Command(status, primary);
			this->commands.insert(pair<uint64_t, Command*>(primary, command)); // Add command to collection under primary key

			for(unsigned int i=(INDEX_CSTATUS+ONE) ; i<tokens.size() ; i++){
				uint64_t cd = Statics::strToCode(tokens.at(i));
				if(cd != primary && this->commands.find(cd) != this->commands.end()){ // Command key is not unique
					cerr << "Bad or corrupt datafile: duplicate command name on line " << linecount << endl;
					exit(EXIT_FAILURE);
				}
				this->commands.insert(pair<uint64_t, Command*>(cd, command)); // Add command to collection under alias keys
			}
		}

		#else // Not testing; do not allow either READ or WRITE commands
		if(!(status & CTRL_COMMAND_TESTING_READ) && !(status & CTRL_COMMAND_TESTING_WRITE)){
			Command* command = new Command(status, primary);
			this->commands.insert(pair<uint64_t, Command*>(primary, command)); // Add command to collection under primary key

			for(unsigned int i=(INDEX_CSTATUS+ONE) ; i<tokens.size() ; i++){
				uint64_t cd = Statics::strToCode(tokens.at(i));
				if(cd != primary && this->commands.find(cd) != this->commands.end()){ // Command key is not unique
					cerr << "Bad or corrupt datafile: duplicate command name on line " << linecount << endl;
					exit(EXIT_FAILURE);
				}
				this->commands.insert(pair<uint64_t, Command*>(cd, command)); // Add command to collection under alias keys
			}
		}
		#endif

		line = reader.getline();
		linecount++;
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