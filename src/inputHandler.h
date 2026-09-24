#pragma once
#include <vector>

struct InputEvent {
    int keyPressed;
};

enum InputKeyCode {
    ARROW_KEY_UP      = 14,
    ARROW_KEY_DOWN    = 15,
    ARROW_KEY_LEFT    = 16,
    ARROW_KEY_RIGHT   = 17,
};

void initTerminalInput();
void resetTerminalInput();

std::vector<InputEvent> pollTerminalInputEvents();