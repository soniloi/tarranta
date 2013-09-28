#include "Command.h"

/*
 *	Constructor
 */
Command::Command(int stat, uint64_t cd){
	this->code = cd;
	this->status = stat;
}	

/*
 *	ostream overloading; may not be the most suitable, but I needed a way to be
 *	able to read the codes quicker than trying to learn off the ASCII table
 */
ostream& operator <<(ostream& output, const Command& comm){
	for(int i=CHARS_PER_CODE-ONE ; i >=0 ; i--){
		char current = (char) (comm.code >> (BITS_PER_BYTE*i));
		output << current;
	}
	return output;
}

#if defined (TESTING_READ)
bool Command::isTestingRead(){
	return (this->status & CTRL_COMMAND_TESTING_READ);
}
#endif

#if defined (TESTING_READ) && defined (TESTING_WRITE)
bool Command::isTestingWrite(){
	return (this->status & CTRL_COMMAND_TESTING_WRITE);
}
#endif
/*
 *	Matches this command to another
 */
bool Command::is(uint64_t cmdcode){
	return (this->code == cmdcode);
}

/*
 *	Assorted accessors revealing various pieces of information about the nature of the command
 */
bool Command::isCompound(){
	return (this->status & CTRL_COMMAND_COMPOUND);
}

bool Command::isMovement(){
	return (this->status & CTRL_COMMAND_MOVEMENT);
}

bool Command::isInvertible(){
	return (this->status & CTRL_COMMAND_INVERTIBLE);
}

bool Command::isSecret(){
	return (this->status & CTRL_COMMAND_SECRET);
}

bool Command::takesArg(){
	return (this->status & CTRL_COMMAND_ARGS);
}

bool Command::takesPresentArg(){
	return (this->takesArg() && (this->status & CTRL_COMMAND_PRESENT));
}

bool Command::takesInventoryArg(){
	return (this->takesArg() && (this->status & CTRL_COMMAND_INVENTORY));
}

uint64_t Command::getCode(){
	return this->code;
}

/*
 *	Return string representation of (most common form of) this command
 */
string Command::toString(){
	return Statics::codeToStr(this->code);
}