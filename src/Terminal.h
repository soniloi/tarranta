/*
 *	Class to act as a terminal wrapper
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "Tokeniser.h"
#include "Definitions.h"

using namespace std;

class Terminal{

 	private:
 		static int getIndexOfNewlineWithinWidth(string str, int index);
 		static int getIndexOfLastSpaceWithinWidth(string str, unsigned int index);
 		static int getIndexOfNextNewline(string str, int index);
 		static void wr(string str);
 		static void wrln(string str);
 		static void wrtab(string str, int index);
 		static void wrpro(string str, int index);
 		static string rd();

 	public:
 		static const int console_width;
		static const string console_reset;
 		static const string colour_out;
 		static const string colour_in;
 		static const string prompttab;
 		static const string promptpro;
 		static const string promptend;
		static void reset();
 		static void wrtab(string str);
 		static void wrpro(string str);
 		static string rdstr(string str);
 		static vector<uint64_t> readCodes(string prompt);
 		static vector<uint64_t> readCodesLocation(string locname);
};

#endif
