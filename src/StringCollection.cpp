#include "StringCollection.h"

/*
 *	Constructor
 *	Takes an istream representing the file it is reading from
 */
StringCollection::StringCollection(BitFileReader& reader){

	string line = reader.getline();

	while(line.compare(FILE_DELIMITER_SECTION)){

		vector<string> tokens = Tokeniser::splitToVector(line, REGEX_FILE);

		uint64_t key = Statics::strToCode(tokens.at(INDEX_SKEY));
		string value = tokens.at(INDEX_SVALUE);
		this->strings.insert(pair<uint64_t, string>(key, value));

		//cerr << "Creating new string; code: |" << std::hex << key << std::dec << "|\tvalue: |" << value << "|" << endl;
	
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