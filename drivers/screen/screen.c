#include "screen.h"

// declare screen buffer
unsigned char* screenBuffer = (unsigned char*)(VIDEO_ADDRESS);
int colors = 0x0f;
int charsOnLine = 0;
int colors;

// declarators to avoid pointless info messages and mislinking when compiling
int printChar(char c, int row, int col);
void scroll();

void scroll(){
    // shift every line left in the array and delete the last line
    for (int counter=MAX_COLS*2; counter<(MAX_COLS*MAX_ROWS*2); counter+=MAX_COLS*2)
        memcpy(VIDEO_ADDRESS+counter-(MAX_COLS*2), VIDEO_ADDRESS+counter, MAX_COLS*2);

    int line = (getLocation(MAX_ROWS-1, 0));
    for (int counter = 0; counter<MAX_COLS*2; counter+=2){
        screenBuffer[line+counter] = ' ';
        screenBuffer[line+counter+1] = colors;
    }
    //unsigned char* line = (unsigned char*)(getLocation(MAX_ROWS-1, 0)+VIDEO_ADDRESS);
    //for (int counter = 0; counter<MAX_COLS*2; counter+=2){
    //    line[counter] = ' ';
    //    line[counter+1] = colors;
    //}

}

int printChar(char character, int row, int col) {
    if(colors==0x00)
        return getCursorLocation();

    int location;

    location = col >= 0 && row >= 0?getLocation(row, col):getCursorLocation();

    if (row >= MAX_ROWS) {
        //clearScreen();
        scroll();
        location = getLocation(MAX_ROWS-1, 0);
        setCursorLocation(location);
    }

    if (character == '\n') {
        location = getLocation(getLocationRow(location) + 1, 0);
    }
    else if (character == '\t'){
        for (int counter = 0; counter<4; counter++){
            screenBuffer[location+counter*2] = ' ';
            screenBuffer[location+1+counter*2] = colors; 
        }
        location+=8;
    }
    else {
        screenBuffer[location] = character;
        screenBuffer[location+1] = colors;
        location += 2;
    }

    setCursorLocation(location);
    return location;
}

// Public Kernel Functions to print
void clearScreen() {
    int size = MAX_COLS * MAX_ROWS;
    for (int counter = 0; counter < size; counter++) {
        screenBuffer[counter*2] = ' ';
        screenBuffer[counter*2+1] = (char)(colors);
    }
    setCursorLocation(getLocation(0, 0));
}

void backspace(){
    int location = getCursorLocation()-2;
    printChar(' ', getLocationRow(location), getLocationCol(location));
    setCursorLocation(location);
}

int _kprintf(int centered, char* message, args_list list){
    if (!message)
        return 1;

    void (*printFunction)(char*);
    if (centered) printFunction = &kprintCentered;
    else printFunction = &kprint;
    char charToPrint[2];

    for (size_t index = 0; index<strlen(message); index++){
        if (message[index] == '%') {
            switch (message[index + 1]) {
                case 'c': {
                    char c = get_args(list, char);
                    (*printFunction)(c);
                    index++;
                    break;
                }
                case 's': {
                    char *c = get_args(list, char*);
                    (*printFunction)(c);
                    index++;
                    break;
                }
                case 'd':
                case 'i': {
                    int c = get_args (list, int);
                    char str[32] = {0};
                    itoa(c, 10, str);
                    (*printFunction)(str);
                    index++;
                    break;
                }
                case 'X':
                case 'x': {
                    int c = get_args (list, int);
                    char str[32] = {0};
                    itoa(c, 16, str);
                    (*printFunction)(str);
                    index++;
                    break;
                }
                default:
                    return 1;
            }
        }
        else{
            charToPrint[0] = message[index];
            charToPrint[1] = '\0';
            (*printFunction)(charToPrint);
        }
    }

    args_end(printArgs);
    return 0;
}

int kprintf(char* message, ...){
    args_list  printArgs;
    args_start(printArgs, message);
    return _kprintf(0, message, printArgs);
}

int kprintfCentered(char* message, ...){
    args_list printArgs;
    args_start(printArgs, message);
    return _kprintf(1, message, printArgs);
}


void kprintPos(char *message, int col, int row, int centered) {
    int location;
    if (col >= 0 && row >= 0)
        location = getLocation(row, col);
    else {
        location = getCursorLocation();
        row = getLocationRow(location);
        col = getLocationCol(location);
    }

    if (centered){
        // get message length
        int messageLength = strlen(message);
        // shift left the characters on the line so that the message is still centered with the new characters
        if (messageLength>1)memcpy(VIDEO_ADDRESS+getLocation(row, messageLength), VIDEO_ADDRESS+getLocation(row, 0), (MAX_COLS*2)-(messageLength));
        // if we are printing each character individually then only shift every other character
        else if (messageLength==1 && (charsOnLine+1)%2==0) memcpy(VIDEO_ADDRESS+getLocation(row, 0), VIDEO_ADDRESS+getLocation(row, 1), (MAX_COLS*2)-(1));
        // set initial value
        location = -1;
        // find where the start of our new addition is if we are printing a full string
        if (messageLength>1) {
            // will look if we have already printed on this line and yield where to print
            for (int counter = 0; counter < MAX_COLS; counter++)
                if (screenBuffer[getLocation(row, counter)] != ' ' &&
                    strlen(screenBuffer[getLocation(row, counter)] > 0)) {
                    location = getLocation(row, counter + strlen(screenBuffer[getLocation(row, counter)]));
                    counter += strlen(screenBuffer[getLocation(row, counter)]);
                }
        }
        // if this is the first addition then just set it as the center
       if (location == -1 && messageLength>1) location = getLocation(row, (MAX_COLS/2)-(messageLength/2));
       // special case for individual character from kprintf
       else if (location == -1) location = getLocation(row, (MAX_COLS/2)-(messageLength/2)+(charsOnLine/2));
        row = getLocationRow(location);
        col = getLocationCol(location);
    }
    int pos = 0;
    while (message[pos] != '\0') {
        charsOnLine++;
        if (message[pos]=='\n')
            charsOnLine=0;
        location = printChar(message[pos++], row, col);
        row = getLocationRow(location);
        col = getLocationCol(location);
    }
}

void _kprint(char *message, int centered) {
    int shift = 0;
    while (message[shift++]=='\n') {
        kprintPos("\n", -1, -1, centered);
        message++;
    }
    int numNewLines = count(message, "\n");
    if ((numNewLines>1 || (numNewLines==1 && message[strlen(message)-1]!='\n')) && strlen(message)>1 && centered){
        list_t* lines = split(message, "\n");
        foreach(val, lines){
                //appendStr(val, "\n");
                //kprintf("Printing string of length %d\n", strlen(val));
                kprintPos(val->value, -1, -1, centered);
                kprintPos("\n",-1,-1,centered);
            }
        list_free(lines);
    }
    else{
        //kprintf("Printing\n");
        kprintPos(message, -1, -1, centered);
    }
}

void kprint(char* message){
    _kprint(message, 0);
}

void kprintCentered(char* message){
    _kprint(message, 1);
}

void setColor(int newColors){
    colors = newColors;
}

void setBackground(int background){
    colors = (background*0x10)+(colors&0xf);
}

void setText(int text){
    colors = (colors&0xf0)+text;
}