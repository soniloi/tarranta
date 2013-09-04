#include "Tokeniser.h"

/*
 *	Split a string into a vector of strings according to some delimiting character
 *	Note that this will NOT return empty tokens, but will count ANY NUMBER of the specified delimiter to be a single delimiter
 */
vector<string> Tokeniser::splitToVector(string str, char delim){

	vector<string> result;

	int bound = str.length();
	int i = 0;
	string current;
	while(str[i]==delim)
		i++; // Ignore any leading delimiters
	while(i < bound){
		while(str[i] != delim && i != bound){
			current += str[i];
			i++;
		}
		result.push_back(current);
		current.erase(current.begin(), current.end());
		while(str[i] == delim)
			i++; // Treat any number of delimiter as a single delimiter
	}

	return result;

}

/*
 *	Split a string into a vector of strings according to some delimiting character,
 *	and convert all tokens to lowercase
 */
vector<string> Tokeniser::splitToLowercaseVector(string str, char delim){

	vector<string> result;

	int bound = str.length();
	int i = 0;
	string current;
	while(str[i]==delim)
		i++; // Ignore any leading delimiters
	while(i < bound){
		while(str[i] != delim && i != bound){
			current += tolower(str[i]);
			i++;
		}
		result.push_back(current);
		current.erase(current.begin(), current.end());
		while(str[i] == delim)
			i++; // Treat any number of delimiter as a single delimiter
	}

	return result;

}

/*
 *	Split a string into a vector of strings according to some delimiter,
 *	and convert (compact) all tokens into codes, represented as types uint64_t
 *	The returned vector will be of some specified size
 */
vector<uint64_t> Tokeniser::splitToCodes(string str, char delim){

	vector<uint64_t> result;

	int bound = str.length();
	int i = 0; // Controls position in input string
	int count = 0; // Controls number of tokens permissible in user input

	while(str[i]==delim)
		i++; // Ignore any leading delimiters

	while(i < bound && count < TOKENS_PER_COMMAND){

		uint64_t current = 0;

		int j = i;
		while(str[i] != delim && i != bound && (i-j) < CHARS_PER_CODE){
			unsigned int shiftby = BITS_PER_BYTE*(CHARS_PER_CODE-(i-j)-ONE);
			current |= ((uint64_t) str[i] << shiftby);
			i++;
		}

		result.push_back(current);
		count++;

		//cout << "\ttoken found: |" << std::hex << ((long long) current) << std::dec << "|" << endl;

		while(i < bound && str[i] != delim)
			i++; // Ignore any further characters in a word with more than 8 letters

		while(str[i] == delim)
			i++; // Treat any number of delimiter as a single delimiter
	}

	return result;

}



