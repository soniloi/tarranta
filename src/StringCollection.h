/*
 *	Class representing a map of uint64_t to string
 *	These would be strings read in from file and mapped to a particular key, where string key is first converted to a long long
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
		StringCollection(FileReader& reader, int& linecount, int& puzzlecount);
		~StringCollection();
		string get(uint64_t key); // Fetches a string with a certain key
		void clear(uint64_t key); // Erases a string with a certain key

};