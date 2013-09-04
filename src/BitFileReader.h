/*
 *	Class representing file reader able to read single bits
 */
#ifndef BITFILEREADER_H
#define BITFILEREADER_H

#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "Tokeniser.h"

using namespace std;

class BitFileReader{

	private:
		ifstream file;
		unsigned char buffer;
		int bufferIndex;
		bool eof;

	public:
		BitFileReader(string filename);
		~BitFileReader();
		int get();
		string getline();
		bool isEof();

};

#endif
