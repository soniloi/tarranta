#include "Tarranta.h"

/*
 *	Main method
 */
int main(){

	cout << "\033[H\033[2J\n"; // Clear console

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
