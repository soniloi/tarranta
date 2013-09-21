#include "Tarranta.h"

/*
 *	Main method
 */
int main(){

	cout << "\033[H\033[2J\n"; // Clear console

	#if defined (TESTING_WRITE) && defined (TESTING_READ)
	cout << " *** Running at testing level WRITE ***" << endl << endl;
	#elif defined (TESTING_READ)
	cout << " *** Running at testing level READ ***" << endl << endl;
	#endif

	Game game(DATAFILE);
	game.play();

	return 0;

}
