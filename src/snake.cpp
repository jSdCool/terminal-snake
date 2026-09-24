#include <iostream>
//apparently the windows only version only lets you set foreground or background at a time, not necessarily both
#define RUTIL_USE_ANSI
#include "rogueutil.h"
#include "stopHandler.h"
#include <vector>
#include "directional.h"
#include "inputHandler.h"
#include "timeing.h"

using namespace std;
using namespace rogueutil;

static int width;
static int height;
static volatile bool gameRunning=true;
static bool paused=false;

struct Position{
	int x;
	int y;
	Direction in;
	Direction out;
};

struct ScreenData {
	string glyph = " ";
	int color = WHITE;
	int bgColor = BLACK;

	bool operator==(const ScreenData &other) const {
		return other.glyph == glyph && other.color == color && other.bgColor == bgColor;
	}
	bool operator!=(const ScreenData &other) const {
		return !(*this == other);
	}
};

void handleStop(){
	gameRunning=false;
	//handle stop here
}

void render(ScreenData current[] , ScreenData prev[]);

#ifdef _WIN32
	DWORD WINAPI winThread(LPVOID params);
#endif

int main() {
	width = tcols();
	height = trows();
	//terminal size check;
	if(width < 30 || height < 30){
		cout << "Terminal Window too Small!!" << endl;
		return EXIT_FAILURE;
	}
	saveDefaultColor();//save current terminal colors
	stopHandler::setContrlCHandler(&handleStop,false);//register the handler for ctrl c
	setConsoleTitle("SNAKE!!");

	initTerminalInput();

	ScreenData * prevScreen = new ScreenData[height*width];
	ScreenData * screen = new ScreenData[height*width];

	Direction heading = UP;
	Direction headingLastFrame = UP;

	Position apple{};
	apple.x=5;
	apple.y=5;
	vector<Position> snake;
	Position p{};
	p.x=width/2;
	p.y=height/2;
	snake.push_back(p);

	//add the initial apple to the screen
	screen[apple.y*width+apple.x] = {" ",WHITE,RED};
	//add the initial snake to the screen
	screen[p.y*width+p.x] = {getDirectionChar(DOWN,UP), GREEN, BLACK};
	enableAlternateBuffer();
	hidecursor();

	shared_ptr<TimeStamp> startOfLastFrame = getNowTimeStamp();

	while(gameRunning) {
		if(paused){
			msleep(20);
		}

		//process user inputs
		vector<InputEvent> events = pollTerminalInputEvents();
		for (InputEvent &event: events) {
			int key = event.keyPressed;
			bool changedDirection =false;
			if((key == ARROW_KEY_UP || key =='w' || key == 'W') && headingLastFrame != DOWN){
				heading=UP;
				changedDirection = true;
			}
			if((key == ARROW_KEY_RIGHT || key =='d' || key == 'D')  && headingLastFrame != LEFT){
				heading=RIGHT;
				changedDirection = true;
			}
			if((key == ARROW_KEY_DOWN || key =='s' || key == 'S') && headingLastFrame != UP){
				heading=DOWN;
				changedDirection = true;
			}
			if((key == ARROW_KEY_LEFT || key =='a' || key == 'A')  && headingLastFrame != RIGHT){
				heading=LEFT;
				changedDirection = true;
			}
			if(key == 'p' || key == 'P'){
				paused = !paused;
			}
			if(key == 'q' || key == 'Q'){
				gameRunning=false;
			}
			if(key == 'h' || key == 'H'){
				resetColor();
				gotoxy(1,1);
				cout << "Arrow Keys / WASD - change direction" << endl << "P - pause" << endl <<"Q - quit"<<endl<<"H - display this message";
			}
			if (changedDirection) {
				//update the snake head to point in the correct direction
				snake[0].out = heading;
				screen[snake[0].y*width+snake[0].x] = {getDirectionChar(snake[0].in,snake[0].out),LIGHTGREEN,BLACK};
			}
		}

		render(screen,prevScreen);
		long long timeSinceLastFrame = msSince(startOfLastFrame);
		if ( timeSinceLastFrame >= ((snake[0].in == UP || snake[0].in == DOWN)?60l:35l) && !paused) {
			startOfLastFrame = getNowTimeStamp();

			headingLastFrame = heading;

			//Remove the old snake from the screen
			for(auto & i : snake){
				screen[i.y*width+i.x] = {};
			}

			//Calculate the new snake
			Position sp{};
			Position tmp{};
			switch(heading){
				case UP:
					sp.x = snake[0].x;
					sp.y = snake[0].y-1;
					break;
				case RIGHT:
					sp.x = snake[0].x+1;
					sp.y = snake[0].y;
					break;
				case DOWN:
					sp.x = snake[0].x;
					sp.y = snake[0].y+1;
					break;
				case LEFT:
					sp.x = snake[0].x-1;
					sp.y = snake[0].y;
					break;
				default:
					break;
			}
			snake[0].out = heading;
			sp.in = -heading;
			sp.out = heading;
			//check to see if the snake has collided to its self
			for(auto & i : snake){
				if(i.x == sp.x && i.y == sp.y){
					gameRunning=false;
					break;
				}
			}

			//if the head is on the apple
			if(snake[0].x == apple.x && snake[0].y == apple.y){
				Position np{};
				snake.push_back(np);
				bool notValid = true;
				while(notValid){
					apple.x = rand()%(width-5)+3;
					apple.y = rand()%(height-5)+3;
					notValid=false;
					for(auto & i : snake){
						if(i.x == apple.x && i.y == apple.y){
							notValid=true;
							break;
						}
					}
				}
				screen[apple.y*width+apple.x]={" ",WHITE,RED};
			}

			tmp = snake[0];
			snake[0] = sp;
			for(size_t i=1;i<snake.size();i++){
				Position tmp2 = tmp;
				tmp = snake[i];
				snake[i]=tmp2;
			}



			//add the new snake to the screen
			if(sp.x <=0 || sp.x >=width || sp.y <= 0 || sp.y >= height){
				gameRunning = false;
			}
			if (gameRunning) {
				for(auto & i : snake){
					screen[i.y*width+i.x] = {getDirectionChar(i.in,i.out),LIGHTGREEN,BLACK};
				}
			}
		}

		msleep(1);
	}

	showcursor();
	resetColor();
	disableAlternateBuffer();
	cout << "GAME OVER!! Score:" <<snake.size() << endl;
	cout.flush();
	cout << endl;
	resetTerminalInput();

	delete[] screen;
	delete[] prevScreen;
}

void render(ScreenData current[] , ScreenData prev[]){
	int fgColor = 30;
	int bgColor = 30;
	for(int y=0;y<height;y++) {
		for(int x=0;x<width;x++) {
			int index = x + y*width;
			if(current[index] != prev[index]) {
				prev[index] = current[index];
				gotoxy(x,y);
				if (current[index].bgColor != bgColor) {
					setBackgroundColor(current[index].bgColor);
					bgColor = current[index].bgColor;
				}
				if (current[index].color != fgColor) {
					setColor(current[index].color);
				}
				cout << current[index].glyph;
			}
		}
	}
	cout.flush();
}