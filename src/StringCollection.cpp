#include "StringCollection.h"

/*
 *	Constructor
 *	Takes an istream representing the file it is reading from
 */
StringCollection::StringCollection(FileReader& reader){

	string line = reader.getline();

	while(line.compare(FILE_DELIMITER_SECTION)){

		vector<string> tokens = Tokeniser::splitToVector(line, REGEX_FILE);

		uint64_t key = Statics::strToCode(tokens.at(INDEX_SKEY));
		string value = tokens.at(INDEX_SVALUE);
		this->strings.insert(pair<uint64_t, string>(key, value));

		line = reader.getline();
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