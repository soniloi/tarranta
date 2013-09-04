#include "Station.h"

/*
 *	Constructor
 */
Station::Station(BitFileReader& reader){

	vector<int*> links;

	string line = reader.getline();

	while(line.compare(FILE_DELIMITER_SECTION)){

		vector<string> tokens = Tokeniser::splitToVector(line, REGEX_FILE);

		/*
		 *	Parse elements needed for constructor
		 */
		int id;
		Statics::strToInt(id, tokens.at(INDEX_ID));
		int status;
		Statics::strToInt(status, tokens.at(INDEX_STATUS), HEX);

		string shortn = tokens.at(INDEX_SHORT);
		string longn = tokens.at(INDEX_LONG);
		string fulln = tokens.at(INDEX_FULL);

		Location* loc = new Location(id, status, shortn, longn, fulln); // Create new Location from these data

		/*
		 *	Parse elements needed for cross-referencing
		 */
		int* link = new int[MAX_DIRECTIONS];
		int dir;

		Statics::strToInt(dir, tokens.at(INDEX_N));
		link[INDEX_N-OFFSET_DIRECTION] = dir;

		Statics::strToInt(dir, tokens.at(INDEX_S));
		link[INDEX_S-OFFSET_DIRECTION] = dir;

		Statics::strToInt(dir, tokens.at(INDEX_E));
		link[INDEX_E-OFFSET_DIRECTION] = dir;

		Statics::strToInt(dir, tokens.at(INDEX_W));
		link[INDEX_W-OFFSET_DIRECTION] = dir;

		Statics::strToInt(dir, tokens.at(INDEX_NE));
		link[INDEX_NE-OFFSET_DIRECTION] = dir;

		Statics::strToInt(dir, tokens.at(INDEX_SW));
		link[INDEX_SW-OFFSET_DIRECTION] = dir;

		Statics::strToInt(dir, tokens.at(INDEX_SE));
		link[INDEX_SE-OFFSET_DIRECTION] = dir;

		Statics::strToInt(dir, tokens.at(INDEX_NW));
		link[INDEX_NW-OFFSET_DIRECTION] = dir;

		Statics::strToInt(dir, tokens.at(INDEX_U));
		link[INDEX_U-OFFSET_DIRECTION] = dir;

		Statics::strToInt(dir, tokens.at(INDEX_D));
		link[INDEX_D-OFFSET_DIRECTION] = dir;

		this->locations.push_back(loc); // Add new location to vector
		links.push_back(link); // Add link data for this location to corresponding links vector

		line = reader.getline();

	}

/*
	// DEBUGGING !!!!!!!!!!!!!!!!
	for(vector<int*>::iterator b=links.begin() ; b!=links.end() ; b++){
		for(int i=0 ; i<10 ; i++)
			cerr << (*b)[i] << '\t';
		cerr << endl;
	}
*/

	
	/*
	 *	Cross-reference all locations
	 */

	vector<int*>::iterator jt = links.begin();

	for(vector<Location*>::iterator it=locations.begin() ; it!=locations.end() ; it++){
		Location* n = locations.at((*jt)[INDEX_N-OFFSET_DIRECTION]);
		Location* s = locations.at((*jt)[INDEX_S-OFFSET_DIRECTION]);
		Location* e = locations.at((*jt)[INDEX_E-OFFSET_DIRECTION]);
		Location* w = locations.at((*jt)[INDEX_W-OFFSET_DIRECTION]);
		Location* ne = locations.at((*jt)[INDEX_NE-OFFSET_DIRECTION]);
		Location* sw = locations.at((*jt)[INDEX_SW-OFFSET_DIRECTION]);
		Location* se = locations.at((*jt)[INDEX_SE-OFFSET_DIRECTION]);
		Location* nw = locations.at((*jt)[INDEX_NW-OFFSET_DIRECTION]);
		Location* u = locations.at((*jt)[INDEX_U-OFFSET_DIRECTION]);
		Location* d = locations.at((*jt)[INDEX_D-OFFSET_DIRECTION]);
		Location* noout = locations.at(LOCATION_NOWHERE);
		(*it)->crossReference(n, s, e, w, ne, sw, se, nw, u, d, noout);
		(*it)->directions[CMD_BACK] = this->locations.at(LOCATION_NOWHERE); // At the outset, there is no 'back'; N.B. do *not* use the accessor setDirection, as this will re-check (and disable) the 'out' direction also
		jt++;
	}

	// Free all the memory that was allocated for the links
	for(vector<int*>::iterator it=links.begin() ; it!=links.end() ; it++){
		delete[] (*it);
	}

}

/*
 *	Destructor
 */
Station::~Station(){

	// Free all Locations
	for(vector<Location*>::iterator it=locations.begin() ; it!=locations.end() ; it++){
		delete (*it);
	}

}

/*
 *	Return the number of locations in the collection
 */
int Station::getLocationCount(){
	return this->locations.size();
}

/*
 *	Return the Location at a particular index in the vector 
 */
Location* Station::get(unsigned int index){
	return this->locations.at(index);
}

/*
 *	Fix a fixture at a certain location
 */
void Station::fix(int index, Item* item){
	this->locations.at(index)->fix(item);
}

/*
 *	Deposit an item at a certain location
 */
void Station::leave(int index, Item* item){
	this->locations.at(index)->deposit(item);
}
