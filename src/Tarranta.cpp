#include "Tarranta.h"

/*
 *	Main method
 */
int main(){

	cout << "\033[H\033[2J\n"; // Clear console
/*
	map<uint64_t, Item*> items;

	Container* container = new Container(0x626F74746C650000, 0x103, "a small bottle", "a small plastic bottle", "Ballygowan", 2);
	SwitchableItem* switchable = new SwitchableItem(0x6C616D7000000000, 0x01A, "a lamp", "a small electric lamp", "Genie", 3);

	items[container->getCode()] = container;
	items[switchable->getCode()] = switchable;

	container->insertItem(switchable);

	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
		cerr << endl << Statics::codeToStr((it->second)->getCode()) << "\tis\t" << typeid(*(it->second)).name() << "\tbytes allocated: " << sizeof(*(it->second)) << endl;
		cerr << (it->second)->getLongname() << endl;
	}


	cerr << endl << endl;

	for(map<uint64_t, Item*>::iterator it = items.begin() ; it != items.end() ; it++){
		cerr << endl;
		delete it->second;
	}
	*/

	#ifdef TESTING_READ
	cout << " *** Running with extended READ commands available ***" << endl << endl;
	#endif

	#ifdef TESTING_WRITE
	cout << " *** Running with extended WRITE commands available ***" << endl << endl;
	#endif

	Game game(DATAFILE);
	game.play();

	return 0;

}
