#include "StringCollection.h"

/*
 *	Constructor
 *	Takes an istream representing the file it is reading from
 *	linecount is the current line number in file
 *	puzzlecount is the number of strings representing solved puzzles that have been found
 */
StringCollection::StringCollection(FileReader& reader, int& linecount, int& puzzlecount){

	string line = reader.getline();

	while(line.compare(FILE_DELIMITER_SECTION)){

		vector<string> tokens = Tokeniser::splitToVector(line, REGEX_FILE);

		if(tokens.size() < MIN_TOKENS_STRING){ // Basic range check
			cerr << "Bad or corrupt datafile: insufficient tokens on line " << linecount << endl;
			exit(EXIT_FAILURE);
		}

		uint64_t key = Statics::strToCode(tokens.at(INDEX_SKEY));
		string value = tokens.at(INDEX_SVALUE);
		this->strings.insert(pair<uint64_t, string>(key, value));

		if(tokens.size() > MIN_TOKENS_STRING){ // If there is any other value, it represents a puzzle that has been solved
			puzzlecount++;
		}

		line = reader.getline();
		linecount++;
	}


}

/*
 *	Destructor
 */
StringCollection::~StringCollection(){
	
}

/*
 *	Return string with a particular key
 *	If no value with that key exists, return an empty string
 */
string StringCollection::get(uint64_t key){
	string result;
	map<uint64_t, string>::iterator it = this->strings.find(key);
	if(it != this->strings.end())
		result = it->second;

	return result;
}

/*
 *	Erase a string with a certain key
 */
void StringCollection::clear(uint64_t key){
	this->strings.erase(key);
}