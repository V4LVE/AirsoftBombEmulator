#ifndef KEYPADCONTROLLER_H
#define KEYPADCONTROLLER_H

#include <keypad.h>
#include <Config.h>

class KeypadController {

public:
    KeypadController() {}

    Keypad customKeyPad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

    char getKeyPress() {
        // Code to get a key press from the keypad
        return '\0'; // Placeholder return value
    }

};

#endif