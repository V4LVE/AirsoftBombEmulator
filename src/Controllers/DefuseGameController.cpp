#include "DefuseGameController.h"
#include <Arduino.h>

String defuseTimeInput = "";
bool backback = false;

void enterGameTimeDefuse()
{
    displayController.setTimeMenu();
        while (backback == false)
        {
            char key = customKeyPad.getKey();
            if (key >= '0' && key <= '9') {
                defuseTimeInput += key;
                displayController.lcd.setCursor(defuseTimeInput.length() - 1, 2);
                displayController.lcd.print(key);
            }
            if (key == 'A')
            {
                defuseGameMinutes = defuseTimeInput.toInt();
                defuseTimeInput = "";
                backback = true;
            }
            
        }
        backback = false;
        displayController.writeDefuseMenu();
}