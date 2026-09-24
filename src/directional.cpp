#include "directional.h"

static std::string directionChars[4][4];
static bool directionsInitialized = false;

//TODO unicode independent
// ║  ╗   ╝   ╚   ╔   ═
//-70 -69 -68 -56 -55 -51
static void initDirections() {
	directionsInitialized = true;
#ifdef _WIN32
	directionChars[UP][UP] = "x";
	directionChars[UP][UP][0] = -70;
	directionChars[UP][DOWN] = "x";
	directionChars[UP][DOWN][0] = -70;
	directionChars[UP][LEFT] = "x";
	directionChars[UP][LEFT][0] = -68;
	directionChars[UP][RIGHT] = "x";
	directionChars[UP][RIGHT][0] = -56;

	directionChars[DOWN][UP] = "x";
	directionChars[DOWN][UP][0] = -70;
	directionChars[DOWN][DOWN] = "x";
	directionChars[DOWN][DOWN][0] = -70;
	directionChars[DOWN][LEFT] = "x";
	directionChars[DOWN][LEFT][0] = -69;
	directionChars[DOWN][RIGHT] = "x";
	directionChars[DOWN][RIGHT][0] = -55;

	directionChars[LEFT][UP] = "x";
	directionChars[LEFT][UP][0] = -68;
	directionChars[LEFT][DOWN] = "x";
	directionChars[LEFT][DOWN][0] = -69;
	directionChars[LEFT][LEFT] = "x";
	directionChars[LEFT][LEFT][0] = -51;
	directionChars[LEFT][RIGHT] = "x";
	directionChars[LEFT][RIGHT][0] = -51;

	directionChars[RIGHT][UP] = "x";
	directionChars[RIGHT][UP][0] = -56;
	directionChars[RIGHT][DOWN] = "x";
	directionChars[RIGHT][DOWN][0] = -55;
	directionChars[RIGHT][LEFT] = "x";
	directionChars[RIGHT][LEFT][0] = -51;
	directionChars[RIGHT][RIGHT] = "x";
	directionChars[RIGHT][RIGHT][0] = -51;
#else
	directionChars[UP][UP] = "║";
	directionChars[UP][DOWN] = "║";
	directionChars[UP][LEFT] = "╝";
	directionChars[UP][RIGHT] = "╚";
	
	directionChars[DOWN][UP] = "║";
	directionChars[DOWN][DOWN] = "║";
	directionChars[DOWN][LEFT] = "╗";
	directionChars[DOWN][RIGHT] = "╔";
	
	directionChars[LEFT][UP] = "╝";
	directionChars[LEFT][DOWN] = "╗";
	directionChars[LEFT][LEFT] = "═";
	directionChars[LEFT][RIGHT] = "═";
	
	directionChars[RIGHT][UP] = "╚";
	directionChars[RIGHT][DOWN] = "╔";
	directionChars[RIGHT][LEFT] = "═";
	directionChars[RIGHT][RIGHT] = "═";
#endif
}

std::string getDirectionChar(Direction in, Direction out){
	if (!directionsInitialized) {
		initDirections();
	}
	return directionChars[in][out];
}