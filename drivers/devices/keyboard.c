//
// Created by x3vikan on 2/18/18.
//
#include "keyboard.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C

int shift = 0;
int caps = 0;
bool ctrl = false;

#define MAX_SCANCODE 57
const char *scancodeToName[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
                          "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
                          "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
                          "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
                          "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                          "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char scancodeToAscii[] = { '?', '?', '1', '2', '3', '4', '5', '6',
                          '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
                          'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
                          'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
                          'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

static char input[256];

static void handleKeypress(state compState){
    uint8_t press = inportb(0x60);
    // if the key is released
    if (press>MAX_SCANCODE) handleSpecialPress(press);
    else if (press == BACKSPACE){
        if (input[0]=='\0')
            return;
        deleteChar(input, 1);
        backspace();
    }
    else if(press == ENTER){
        if (strlen(input)==0)
            return;
        kprint("\n");
        handleUserInput(input);
        memset(input, 0, 256);
    }
    else if (ctrl)
        handleCtrlPress(scancodeToAscii[(int) press]);
    else{
        char letter = scancodeToAscii[(int) press];
        if (letter=='?')
            handleSpecialPress(press);
        else {
            if ((shift||caps) && letter!=' ') letter = capitalize(letter);
            char str[2] = {letter, '\0'};
            appendStr(input, str);
            kprint(str);
        }
    }
    SUPPRESS_UNUSED_ERROR(compState);
}

void handleSpecialPress(uint8_t scancode){
    const char* name = scancodeToName[(int)(keyReleased(scancode)?scancode-0x80:scancode)];
    if (name == "LShift" || name=="RShift")
        shift = keyReleased(scancode) ? 0 : 1;
    else if (name == "Lctrl")
        ctrl = keyReleased(scancode)? false:true;
    else if (scancode==0x3A)
        caps = (caps==0?1:0);
}

void initKeyboard(){
    addInterruptHandler(INTERRUPTSERVICEROUTINE1, handleKeypress);
}