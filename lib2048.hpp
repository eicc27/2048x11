#include <iostream>
#include <random>
#include <functional>
#include <vector>
#include <algorithm>
#include "curses.h"
using namespace std;
typedef vector<vector<int>> v2;
class Player {
public:
	friend class Game;
private:
	int score = 0;
	string name;
};
class Game {
public:
	Game(int);
	friend class Player;
	enum gamemode { singleplayer = 1, multiplayer };
	enum direction { up = 'w', down = 's', left = 'a', right = 'd' };
	void SP(Player& player0), MP(Player& player0, Player& player1);
private:
	enum direction_int { _up = 0, _down, _left, _right };
	enum num_dir { iter_l = -1, iter_r = 1 };
	int iter_start = 0, iter_end = 0;
	void Cheat(), _Play(), LockedPlay();
	void NumGen(double p0), PrintMapFrame(), PrintNum();
	void PrintScore(Player& player0, Player& player1), PrintScore(Player& player0);
	bool isGameOver(), isGameWin();
	v2 vacant, map;
	bool cheatable = true, raisingcheat = false, canmove = false, input_check = true, isVertical, isLeft;
	int dscore = 0, steps = 0, end = 2048, i, j, k, size, gamemodekey;
	string gameinputkey;
	num_dir dir;
	vector<char> availdirs = {};
	void transpose(v2&& map) {
		for (i = 0; i < size; ++i)
			for (j = 0; j < i; ++j)
				swap(map[i][j], map[j][i]);
	} // Reduced reference-value binding to more atomic operations.
	template<num_dir dir>
	void compress(v2&& map, bool&& canmove) {
		for (i = 0; i < size; ++i) {
			j = iter_start;
			while (j != iter_end) {
				if (map[i][j]) break;
				j += dir;
			}
			while (j != iter_end + dir) {
				if (map[i][j]) { j += dir; continue; }
				map[i].erase(map[i].begin() + j);
				map[i].insert(iter_start ? map[i].end() : map[i].begin(), 0);
				canmove = 1;
				j += dir;
			}
		}
	}
	template<num_dir dir>
	void combine(v2&& map, bool&& canmove) {
		for (i = 0; i < size; ++i) {
			j = iter_end;
			while (j != iter_start) {
				if (map[i][j] != map[i][j + dir]) { j += dir; continue; }
				map[i][j] *= 2;
				dscore += map[i][j];
				map[i][j + dir] = 0;
				canmove = 1;
				j += dir;
			}
		}
	}
	template<char d>
	bool _Move(v2& map, bool& canmove) {
		dscore = canmove = 0;
		isVertical = (d == up or d == down);
		isLeft = (d == up or d == left);
		if (isLeft) { iter_end = 0; iter_start = size - 1; }
		else { iter_end = size - 1; iter_start = 0; }
		if (isVertical) { transpose(move(map)); };
		if (isLeft) { compress<iter_l>(move(map), move(canmove)); combine<iter_r>(move(map), move(canmove)); compress<iter_l>(move(map), move(canmove)); }
		else { compress<iter_r>(move(map), move(canmove)); combine<iter_l>(move(map), move(canmove)); compress<iter_r>(move(map), move(canmove)); }
		if (isVertical) { transpose(move(map)); };
		return canmove;
	} //At checking time, bind it.
	vector<function<bool()>> CpyMove = {};
	function<void()> Move = [&]() {if (gameinputkey.c_str()[0] == up) _Move<up>(map, canmove);
	if (gameinputkey.c_str()[0] == down) _Move<down>(map, canmove);
	if (gameinputkey.c_str()[0] == left) _Move<left>(map, canmove);
	if (gameinputkey.c_str()[0] == right) _Move<right>(map, canmove);
	}, GetAvailDirs = [&]() {availdirs = {};
	if (CpyMove[_up]())	availdirs.push_back(up);
	if (CpyMove[_down]()) availdirs.push_back(down);
	if (CpyMove[_left]()) availdirs.push_back(left);
	if (CpyMove[_right]()) availdirs.push_back(right);
	}, GetCpyMove = [&]()
	{CpyMove = { bind(&Game::_Move<up>, this, map, canmove), bind(&Game::_Move<down>, this, map, canmove), bind(&Game::_Move<left>, this, map, canmove), bind(&Game::_Move<right>, this, map, canmove) }; };
};
