#pragma once
#include<string>

enum Direction{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

std::string getDirectionChar(Direction in, Direction out);

Direction operator-(const Direction &d);