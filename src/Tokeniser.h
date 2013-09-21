/*
 *	Class containing assorted tokenisers
 */
#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>
#include <inttypes.h>
#include <iostream> // DEBUGGING !!!!!!!!!!!!!!!!!!!
#include "Statics.h"

using namespace std;

class Tokeniser{

	public:
		static vector<string> splitToVector(string str, char delim);
		static vector<string> splitToLowercaseVector(string str, char delim);
		static vector<uint64_t> splitToCodes(string str, char delim);

};

#endif
