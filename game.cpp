#include "lib2048.hpp"
Game::Game(int _size) :size(_size) {
	vector<int> temp(_size, 0);
	for (i = 0; i < _size; ++i)
		map.push_back(temp); // default values
	while (1) {
		cout << "Single player [1] or multi player [2] ?" << endl;
		cin >> gameinputkey;
		if ((gameinputkey[0] == singleplayer + 48 or gameinputkey[0] == multiplayer + 48) and gameinputkey.size() == 1)
			break;
		cout << "Input ERROR: Please type '1' or '2'." << endl;
	}
	gamemodekey = int(gameinputkey[0]) - 48;
}
bool Game::isGameOver() {
	GetAvailDirs();
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j)
			if (map[i][j] == 0)
				return 0;
	if (!availdirs.size())
		return 1;
	return 0;
}
bool Game::isGameWin() {
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j)
			if (map[i][j] == end)
				return true;
	return false;
}
void Game::NumGen(double p0) {
	vacant = {};
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j)
			if (map[i][j] == 0)
				vacant.push_back({ i,j });
	if (vacant.size() == 0)
		return;
	time_t t0 = clock();
	default_random_engine eng0(time(&t0));
	uniform_real_distribution<double> r(0, 1);
	int nnum = r(eng0) <= p0 ? 2 : 4;
	time_t t1 = clock();
	std::default_random_engine eng1(time(&t1));
	std::uniform_int_distribution<unsigned> p(0, vacant.size() - 1);
	int index = p(eng1);
	map[vacant[index][0]][vacant[index][1]] = nnum;
}
void Game::PrintMapFrame() {
	move(0, 0);
	for (i = 0; i <= size * 2; ++i) {
		if (i % 2 == 0)
			for (j = 0; j <= size * 6; ++j)
				j % 6 == 0 ? waddch(stdscr, '+') : waddch(stdscr, '-');
		else
			for (j = 0; j <= size * 6; ++j)
				j % 6 == 0 ? waddch(stdscr, '|') : waddch(stdscr, ' ');
		waddch(stdscr, '\n');
	}
	refresh();
}
void Game::PrintNum() {
	for (i = 0; i < size; ++i) {
		for (j = 0; j < size; ++j) {
			mvaddstr(2 * i + 1, 6 * j + 1, "     ");
			if (map[i][j] == 0)
				continue;
			else if (map[i][j] < 100)
				mvprintw(2 * i + 1, 6 * j + 3, "%d", map[i][j]);
			else if (map[i][j] < 10000)
				mvprintw(2 * i + 1, 6 * j + 2, "%d", map[i][j]);
			else if (map[i][j] < 100000)
				mvprintw(2 * i + 1, 6 * j + 1, "%d", map[i][j]);
		}
	}
	refresh();
}
void Game::PrintScore(Player& player0, Player& player1) {
	steps % 2 == 0 ? player0.score += dscore : player1.score += dscore;
	mvprintw(0, 6 * size + 4, "%s's score is %d,\n", player0.name.c_str(), player0.score);
	mvprintw(1, 6 * size + 4, "%s's score is %d,\n", player1.name.c_str(), player1.score);
	refresh();
}
void Game::PrintScore(Player& player0) {
	player0.score += dscore;
	mvprintw(0, 6 * size + 4, "Current score: %d", player0.score); refresh();
}
void Game::Cheat() {
	++steps; raisingcheat = true;
	string cheatmsg = "";
	for (i = 0; i < strlen(gameinputkey.c_str()); ++i)
		cheatmsg += gameinputkey.c_str()[i];//GetAvailDirs();
	mvprintw(2 * size + 2, 0, "%s\tYou must choose a following direction to move: ", cheatmsg.c_str());
	for (i = 0; i < availdirs.size(); ++i)
		printw("%c ", availdirs[i]);
}
void Game::_Play() {
	while (1)
	{
		mvscanw(2 * size + 1, 0, "%[^\n]", gameinputkey.c_str());
		if (gameinputkey.c_str()[1] != '\0') {
			if (gameinputkey.c_str()[1] == 't' and gameinputkey.c_str()[0] == '-') {
				end = 64;
				mvprintw(2 * size + 2, 0, "Endgame rule has been changed.\n");
				return;
			}
		}
		else if (gameinputkey.c_str()[0] == up or gameinputkey.c_str()[0] == down or gameinputkey.c_str()[0] == left or gameinputkey.c_str()[0] == right) {
			Move(); ++steps;
			NumGen(0.9); PrintNum(); GetCpyMove();
			return;
		}
		if (gamemodekey == multiplayer and (gameinputkey.c_str()[0] == 'c' and gameinputkey.c_str()[1] == ' '))
			return;
		mvprintw(2 * size + 3, 0, "Input ERROR: Unrecognized command.\n"); refresh();
	}
} //Without player parameters // No loop
void Game::LockedPlay() {
	raisingcheat = false; cheatable = false;
	int sz = availdirs.size(); //if (!sz) { mvprintw(2 * size + 3, 0, "Warning: No available moves!"); return; };
	while (1)
	{
		mvscanw(2 * size + 1, 0, "%[^\n]", gameinputkey.c_str());
		if (gameinputkey.c_str()[1] == '\0')
		{
			for (i = 0; i < sz; ++i)
				if (gameinputkey.c_str()[0] == availdirs[i])
					break;
			if (i < sz)
				break;
		}
		mvprintw(2 * size + 3, 0, "You have to choose one available direction.\n");
	}
	Move(); NumGen(0.9); PrintNum();
}
void Game::SP(Player& player0) {
	if (gamemodekey == multiplayer) return;
	PrintMapFrame(); NumGen(0.9); PrintNum(); GetCpyMove();
	while ((not isGameOver()) and (not isGameWin())) {
		_Play();
		PrintScore(player0);
	}
	mvprintw(2 * size + 4, 0, "Game Over.\n"); refresh(); getch();
}
void Game::MP(Player& player0, Player& player1) {
	if (gamemodekey == singleplayer) return;
	printw("Input the first player's name:"); scanw("%[^\n]", player0.name.c_str());
	printw("Input the second player's name:"); scanw("%[^\n]", player1.name.c_str());
	PrintMapFrame(); NumGen(0.9); PrintNum(); GetCpyMove();
	while ((not isGameOver()) and (not isGameWin())) {
		mvprintw(2, 6 * size + 4, "It's %s's turn.\n", steps % 2 == 0 ? player0.name.c_str() : player1.name.c_str());
		(raisingcheat and cheatable) ? LockedPlay() : _Play();
		PrintScore(player0, player1);
		if (gameinputkey.c_str()[0] == 'c' and gameinputkey.c_str()[1] == ' ')
		{
			if (cheatable) Cheat();
			else mvprintw(2 * size + 3, 0, "No more cheating!\n");
		}
	}
	mvprintw(2 * size + 4, 0, "Game Over.\n"); refresh(); getch();
}
