/*
 *	Class representing a map of uint64_t to string
 *	These would be strings read in from file and mapped to a particular key
 */
#include <string>
#include <map>
#include <inttypes.h>
#include "FileReader.h"
using namespace std;

class StringCollection{

	private:
		map<uint64_t, string> strings;

	public:
		friend class Game;
		StringCollection(FileReader& reader);
		~StringCollection();
		string get(uint64_t key);

};