#include "BitFileReader.h"

/*
 *	Constructor
 */
BitFileReader::BitFileReader(string filename){

  this->file.open(filename.c_str());
  if(!file.good()){
    cout << "Error opening file, exiting." << endl;
    exit(1);
  }

	this->bufferIndex = 0;
	this->eof = file.eof();

	if(!eof)
	  this->buffer = file.get();
	else{
		cout << "Input file is empty, exiting" << endl;
		exit(1);
	}

}

/*
 *	Destructor
 */
BitFileReader::~BitFileReader(){

	//cerr << "closing input bitfile" << endl;
	if(this->file.good())
		this->file.close();

}

/*
 * Get a bit from the file
 */
int BitFileReader::get(){

	int bit = (buffer >> (BITS_PER_BYTE - bufferIndex - ONE)) & ONE;
	bufferIndex++;

	if(bufferIndex == BITS_PER_BYTE){

		buffer = file.get();
		bufferIndex = 0;

		file.get(); // Check the following character to test for EOF
		if(file.eof())
			this->eof = true;
		else
			file.seekg(-ONE, ios::cur);

	}

	return bit;

}

/*
 *	Get a line from the file
 *	This is formatted in a specific way:
 *		we read in 7-bit ascii and decompress it to 8-bit characters
 */
string BitFileReader::getline(){

	string result;
	if(this->eof)
		return result;

	bool found = false;
	while(!found){
		unsigned char current = '\0';
		for(int i=BITS_PER_ASCII-ONE ; i>=0 ; i--){
			int bit = this->get();
			current |= (bit << i);
		}
		if(file.eof()){
			found = true;
			this->eof = true;
		}
		else if(current=='\n')
			found = true;
		else
			result += current;
	}

	return result;
}

/*
 *	Return whether the file is empty
 *	Requires that both the end of the input file stream has been reached,
 *	and that the buffer is empty
 */
bool BitFileReader::isEof(){
	return this->eof;
}
