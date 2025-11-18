#include "GameController.h"
#include <Arduino.h>

String dominationTimeInput = "";
bool back = false;

void startDominationMode() {
    if (dominationGameMinutes < 1)
    {
        displayController.lcd.clear();
        displayController.lcd.setCursor(0, 0);
        displayController.lcd.print("Set valid time!");
        delay(2000);
        displayController.writeDominationMenu();

    }
    

}

void enterGameTime()
{
    displayController.setTimeMenu();
        while (back == false)
        {
            char key = customKeyPad.getKey();
            if (key >= '0' && key <= '9') {
                dominationTimeInput += key;
                displayController.lcd.setCursor(dominationTimeInput.length() - 1, 2);
                displayController.lcd.print(key);
            }
            if (key == 'A')
            {
                dominationGameMinutes = dominationTimeInput.toInt();
                dominationTimeInput = "";
                back = true;
            }
            
        }
        back = false;
        displayController.writeDominationMenu();
}

