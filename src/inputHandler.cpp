#include "inputHandler.h"
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <cstring>
    #include <fcntl.h>
    #include <sys/ioctl.h>
#endif
// this is specifically for their key code enums

bool initialized = false;

#ifdef _WIN32
static HANDLE currentStdIn;
static DWORD oldTerminalMode;

void initTerminalInput() {
    if (initialized) {
        return;
    }
    currentStdIn = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(currentStdIn, &oldTerminalMode);
    //disable input echoing
    SetConsoleMode(currentStdIn, (oldTerminalMode & ~(ENABLE_ECHO_INPUT)) | ENABLE_PROCESSED_INPUT);
    initialized = true;
}

void resetTerminalInput() {
    if (!initialized) {
        return;
    }
    SetConsoleMode(currentStdIn, oldTerminalMode);
    initialized = false;
}

std::vector<InputEvent> pollTerminalInputEvents() {
    if (!initialized) {
        fprintf(stderr,"WARNING: Attempted to poll terminal input events before initializing input!!!!!\n");
        return {};
    }
    DWORD consoleEventCount;
    GetNumberOfConsoleInputEvents(currentStdIn, &consoleEventCount);
    if (consoleEventCount == 0) {
        return {};
    }

    std::vector<INPUT_RECORD> records;
    records.resize(consoleEventCount);
    DWORD readCount;
    ReadConsoleInput(currentStdIn, records.data(), records.size(), &readCount);
    std::vector<InputEvent> events;
    for (DWORD i=0;i<readCount;i++) {
        INPUT_RECORD record = records[i];
        if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
            WORD virtualKey = record.Event.KeyEvent.wVirtualKeyCode;
            if (virtualKey == VK_UP) {
                events.emplace_back(ARROW_KEY_UP);
            }else if (virtualKey == VK_LEFT) {
                events.emplace_back(ARROW_KEY_LEFT);
            } else if (virtualKey == VK_RIGHT) {
                events.emplace_back(ARROW_KEY_RIGHT);
            } else if (virtualKey == VK_DOWN) {
                events.emplace_back(ARROW_KEY_DOWN);
            } else {
                events.emplace_back(record.Event.KeyEvent.uChar.AsciiChar);
            }
        }
    }
    return events;
}
#else
static termios oldTerminalMode{};

void initTerminalInput() {
    if (initialized) {
        return;
    }
    termios newTerminalMode{};
    tcgetattr(STDIN_FILENO, &oldTerminalMode);
    newTerminalMode = oldTerminalMode;
    //disable input echo and Canonical input (erase and kill processing)
    newTerminalMode.c_lflag &= ~(ICANON | ECHO);
    //apply the new mode
    tcsetattr(STDIN_FILENO, TCSANOW, &newTerminalMode);
    //add the non blocking flag to the stdin reader
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
    //flush standard out
    fflush(stdout);
    initialized = true;
}

void resetTerminalInput() {
    if (!initialized) {
        return;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalMode);
    initialized = false;
}

std::vector<InputEvent> pollTerminalInputEvents() {
    if (!initialized) {
        fprintf(stderr,"WARNING: Attempted to poll terminal input events before initializing input!!!!!\n");
        return {};
    }

    int bytesWaiting =0;
    if (ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting) == -1) {
        return {};
    }
    if (bytesWaiting == 0) {
        return {};
    }
    std::vector<int8_t> buffer;
    buffer.resize(bytesWaiting+1);
    size_t bytesRead = read(STDIN_FILENO, buffer.data(), buffer.size());
    if (bytesRead == 0) {
        return {};
    }
    const char *bufferPtr = reinterpret_cast<const char *>(buffer.data());
    //while pointer is not at the end of the buffer:
    std::vector<InputEvent> events;
    while (bufferPtr != reinterpret_cast<const char *>(&buffer.back())) {
        if (strncmp(bufferPtr, "\x1b[", 2) == 0) {//if it is an escape sequence
            if (bufferPtr[2] == 'A') {
                events.emplace_back(ARROW_KEY_UP);
                bufferPtr+=3;
            } else if (bufferPtr[2] == 'B') {
                events.emplace_back(ARROW_KEY_DOWN);
                bufferPtr+=3;
            } else if (bufferPtr[2] == 'C') {
                events.emplace_back(ARROW_KEY_RIGHT);
                bufferPtr+=3;
            } else if (bufferPtr[2] == 'D') {
                events.emplace_back(ARROW_KEY_LEFT);
                bufferPtr+=3;
            } else {
                //unknown escape sequence, just skip
                bufferPtr+=2;
            }
        } else {
            events.emplace_back(bufferPtr[0]);
            bufferPtr++;
        }
    }
    return events;
}

#endif
