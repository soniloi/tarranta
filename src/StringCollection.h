/*
 *	Class representing a map of uint64_t to string
 *	These would be strings read in from file and mapped to a particular key
 */
#include <string>
#include <map>
#include <inttypes.h>
#include "BitFileReader.h"
using namespace std;

class StringCollection{

	private:
		map<uint64_t, string> strings;

	public:
		StringCollection(BitFileReader& reader);
		~StringCollection();
		string get(uint64_t key);

};