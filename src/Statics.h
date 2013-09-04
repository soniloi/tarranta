#ifndef STATICS_H
#define STATICS_H

#include <string>
#include <inttypes.h>
#include "Definitions.h"

#define DEFAULT_RADIX 0xA
#define ASCII_NUMERAL_OFFSET 0x30
#define ASCII_ALPHA_OFFSET 0x37
#define ONE 1

/*
 *	Class containing assorted static methods
 */

using namespace std;

class Statics{
	public:
		static int power(int num, int exp);
		static bool strToInt(int& dest, string str, int radix);
		static bool strToInt(int& dest, string str);
		static uint64_t strToCode(string str);
		static string codeToStr(uint64_t code);
};

#endif
