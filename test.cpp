#include "lib2048.hpp"
int main(int argc, char** argv)
{
	int _size = 3;
/* if (strlen(argv[1]) == 2 and argv[1][0] == '-' and argv[1][1] == 's') {
		if (strlen(argv[2]) == 1) {
			switch (argv[2][0]) {
			case '3': _size = 3; break;
			case '4':_size = 4; break;
			case '5': _size = 5; break;
			default: cout << "Invalid argument: " << argv[2] << ".\nSize must be from 3 to 5." << endl; return -1;
			}
		}
	}
	else {
		cout << "Invalid argument: " << argv[1] << ".\nOnly -s is accepted." << endl; return -1;
	}*/
	Game game(_size);
	cout << "Game board size has been set to " << _size << '.' << endl;
	cout<< "Curses is ready to initialize. You can only change the command line window size NOW!" << endl;
	Player player0, player1;
	getchar(); getchar(); initscr();
	game.SP(player0); game.MP(player0, player1);
	endwin();
	cout << "Game on Curses has been terminated." << endl;
}