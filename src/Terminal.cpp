#include "Terminal.h"

const int Terminal::console_width = 80;
const string Terminal::console_reset = "\033[0m";
const string Terminal::colour_out = "\033[32m";
const string Terminal::colour_in = "\033[0m";
const string Terminal::prompttab = "         > ";
const string Terminal::promptpro = "---------> ";
const string Terminal::promptend = " > ";

/*
 *	Reset the console to defaults
 */
void Terminal::reset(){
	cout << Terminal::console_reset;
}

/*
 *	Return the index of a newline within the allowable console width
 *	string segment is defined by str, with start index index
 *	If none exists, return -1
 */
int Terminal::getIndexOfNewlineWithinWidth(string str, int index){
	int bound1 = str.length();
	int bound2 = (Terminal::console_width - Terminal::prompttab.length() + index);
	for(int i=index ; i<bound1 && i<bound2 ; i++){
		if(str[i] == '\n')
			return i;
	}
	return -1;
}

/*
 *	Return index of the last space character found in a string segment
 *	This would be the last space in the allowable width of the segment
 *	string segment is defined by str, with start index index
 *	If none exists, return -1
 */
int Terminal::getIndexOfLastSpaceWithinWidth(string str, unsigned int index){
	unsigned int i = console_width - prompttab.length() + index;
	if(str.length() < i)
		i = str.length() - ONE;
	while(i > index){
		if(str[i] == SPACE)
			return i;
		i--;
	}
	return -1;
}

/*
 *	Return the index of the next newline in the string after the point index, regardless of whether in console-width
 *	If none exists, return -1
 */
int Terminal::getIndexOfNextNewline(string str, int index){
	int bound = str.length();
	for(int i=0 ; i<bound ; i++){
		if(str[i] == '\n')
			return i;
	}
	return -1;
}

/*
 *	Write a string (in a particular way) to cout
 */
void Terminal::wr(string str){
	cout << colour_out;
	cout << str;
}

/*
 *	Write a string with a newline
 */
 void Terminal::wrln(string str){
 	Terminal::wr(str);
 	cout << endl;
 }

/*
 *	Print to cout with a 'tabbed'-formatted output; public method
 */
void Terminal::wrtab(string str){
	Terminal::wrtab(str, ZERO);
}

/*
 *	Print to cout with a 'tabbed'-formatted output; private method
 */
void Terminal::wrtab(string str, int index){

	int bound = getIndexOfNewlineWithinWidth(str, index);

	if(bound >= 0){ // Where there is a newline somewhere within the first console-width of chars, we will wish to print it
		Terminal::wrln(prompttab + str.substr(index, (bound-index)));
		Terminal::wrtab(str, bound + ONE); // Add one to skip the actual newline character
	}

	else if((str.length() + prompttab.length() - index) > console_width){ // If it's longer than the permissible width, but without newlines
		int spacebound = getIndexOfLastSpaceWithinWidth(str, index);
		if(spacebound >= 0){ // If it can be split on space
			Terminal::wrln(prompttab + str.substr(index, (spacebound-index)));
			Terminal::wrtab(str, spacebound + ONE); // Add one to skip the space character
		}
		else if(int newlinebound = Terminal::getIndexOfNextNewline(str, index) > 0){ // No spaces available, split by remaining newlines
			Terminal::wrln(prompttab + str.substr(index, (newlinebound-index)));
			Terminal::wrtab(str, newlinebound + ONE);
		}
		else{ // There's literally no logical place to split, so just print as-is
			Terminal::wrln(prompttab + str.substr(index));
		}

	}

	else{ // There's no need to split it as it's within the allowable length and there are no newlines (sort of a base case)
		Terminal::wrln(prompttab + str.substr(index));
	}

}

/*
 *	Print to cout with a 'prompted'-formatted output
 */
void Terminal::wrpro(string str){

	int bound = getIndexOfNewlineWithinWidth(str, ZERO);

	if(bound >= 0){ // Where there is a newline somewhere within the first console-width of chars, we will wish to print it
		Terminal::wrln(promptpro + str.substr(0, bound));
		Terminal::wrtab(str, bound + ONE); // Add one to skip the actual newline character; print remainder in tabbed format
	}

	else if((str.length() + prompttab.length()) > console_width){ // If it's longer than the permissible width, but without newlines
		int spacebound = getIndexOfLastSpaceWithinWidth(str, ZERO);
		if(spacebound >= 0){ // If it can be split on space
			Terminal::wrln(promptpro + str.substr(0, spacebound));
			Terminal::wrtab(str, spacebound + ONE); // Add one to skip the space character; print remainder in tabbed format
		}
		else if(int newlinebound = Terminal::getIndexOfNextNewline(str, ZERO) > 0){ // No spaces available, split by remaining newlines
			Terminal::wrln(promptpro + str.substr(0, newlinebound));
			Terminal::wrtab(str, newlinebound + ONE); // Print remainder in tabbed format
		}
		else{ // There's literally no logical place to split, so just print as-is
			Terminal::wrln(promptpro + str);
		}

	}

	else{ // There's no need to split it as it's within the allowable length and there are no newlines (sort of a default case)
		Terminal::wrln(promptpro + str);
	}

}

/*
 *	Get string from cin after an appropriate prompt has been printed
 */
string Terminal::rd(){
	cout << colour_in;
	string result;
//	cin >> result;
	getline(std::cin, result);
	return result;
}

/*
 *	Get string from cin using tabbed prompt
 */
string Terminal::rdtab(string str){
	Terminal::wrtab(str, ZERO);
	Terminal::wrtab(prompttab);

	string result;
	cin >> result;
	return result;
}

/*
 *	Get string from cin using tabbed prompt
 */
string Terminal::rdtab(){
	Terminal::wr(prompttab);
	return Terminal::rd();
}

/*
 *	Get string from cin using full prompt
 */
string Terminal::rdpro(){
	Terminal::wr(promptpro);
	return Terminal::rd();
}

/*
 *	Get string from cin using something specified as a prompt
 *	Will align to other prompts by truncation
 */
string Terminal::rdstr(string str){

	string pr(promptpro);
	pr += str;
	pr += NL;
	pr += prompttab;

	Terminal::wr(pr);
	return Terminal::rd();
}

/*
 *	Get string from cin using a stub of a location string within a prompt
 */
string Terminal::rdloc(Location* location){

	string pr(location->getStub());
	while((pr.length() + promptend.length()) < promptpro.length())
		pr += SPACE;
	
	pr.resize(promptpro.length() - promptend.length());
	pr += promptend;

	Terminal::wr(pr);
	return Terminal::rd();

}

/*
 *	Same as above, but accepting stringstreams as well
 */
void Terminal::wrpro(stringstream ss){
	Terminal::wrpro(ss.str());
}

void Terminal::wrtab(stringstream ss){
	Terminal::wrtab(ss.str());
}


