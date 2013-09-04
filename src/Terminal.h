/*
 *	Class to act as a terminal wrapper
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <sstream>
#include <iostream>
#include "Location.h"
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
 		static string rdtab(string str);
 		static string rdtab();
 		static string rdpro();
 		static string rdstr(string str);
 		static string rdloc(Location* loc);
 		static void wrpro(stringstream ss);
 		static void wrtab(stringstream ss);
};

#endif
