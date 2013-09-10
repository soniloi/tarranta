/*
 *	Class representing file reader able to read the tarranta datafile
 */

#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "Tokeniser.h"

using namespace std;

class FileReader{

	private:
		ifstream file;
		unsigned char buffer;
		int bufferIndex;
		bool eof;

	public:
		FileReader(string filename);
		~FileReader();
		int get();
		string getline();
		bool isEof();

};

#endif
