#include "Statics.h"

/*
 * Convert a string to an integer
 * Return true if successful
 * Return false in the case of invalid input (non-digit characters, empty strings, ... )
 * Will use decimal base by default, but another may be specified
 */
bool Statics::strToInt(int &dest, string str, int radix){

	dest = 0;

	if(str[0] == '\0')
		return false; // Empty string

	if(str[0] != '-'){
		int offset = 0;
		if(str[0] >= '0' && str[0] <= '9')
			offset = ASCII_NUMERAL_OFFSET;
		else
			offset = ASCII_ALPHA_OFFSET;
		dest = str[0] - offset;
	}

	else if(str[1] == '\0')
		return false; // String consisted of "-" only

	if(dest > radix)
		return false; // First character was something other than '-' or a digit

	int i;

	for(i=1 ; str[i]!='\0' ; i++){

		int offset = 0;

		if(str[i] >= '0' && str[i] <= '9')
			offset = ASCII_NUMERAL_OFFSET;

		else
			offset = ASCII_ALPHA_OFFSET;

		int digit = str[i] - offset;

		if((unsigned) digit > (unsigned) radix)
			return false;

		dest *= radix;
		dest += digit;

	}

	if(str[0] == '-')
		dest *= -1; // Negate if necessary

	return true;

}

bool Statics::strToInt(int& dest, string str){

	return Statics::strToInt(dest, str, DEFAULT_RADIX);

}

/*
 *	Simple power calculation
 *	This returns only integer powers, and as such the exponent must be positive
 */
int Statics::power(int num, int exp){

	int pow = ONE;
	int p = exp;
	while(p > 0){
		pow *= num;
		p--;
	}

	return pow;

}

/*
 *	'Convert' string into its packed eight-character integer code
 *	Returned as type uint64_t, which is capable of being shifted more than 31 bits safely
 */
uint64_t Statics::strToCode(string str){

	uint64_t code = 0;

	unsigned int bound = str.length();
	for(unsigned int i=0 ; i<CHARS_PER_CODE && i<bound; i++){
		unsigned int shiftby = BITS_PER_BYTE*(CHARS_PER_CODE-i-ONE);
		code |= ((uint64_t) str[i] << shiftby);	
	}

	return code;

}

/*
 *	Get string representation of a code
 *	Note that this may not perfectly reverse the above, as any long strings will have been truncated to 8 chars
 */
string Statics::codeToStr(uint64_t code){

	string result;
	for(unsigned int i=0 ; i<CHARS_PER_CODE ; i++){
		unsigned int shiftby = BITS_PER_BYTE*(CHARS_PER_CODE-i-ONE);
		result += ((char) (code >> shiftby));
	}

	// NOTE!!!! MAY NEED TO DO A LENGTH CHECK HERE!!!!!!!!!!! NOT SURE HOW C++ STRING HANDLES NULLS!!!!!!!
	return result;

}