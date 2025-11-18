#include "GameController.h"
#include <Arduino.h>

String dominationTimeInput = "";
bool back = false;

void handleBuzzer(int totalSeconds);

unsigned long lastUpdate = 0;
unsigned long lastBeep = 0;
bool alarmDone = false;

void startDominationMode() {
    if (dominationGameMinutes < 1)
    {
        displayController.lcd.clear();
        displayController.lcd.setCursor(0, 0);
        displayController.lcd.print("Set valid time!");
        delay(2000);
        displayController.writeDominationMenu();

    }
    
    int totalSeconds = dominationGameMinutes * 60;

    while (totalSeconds >= 0)
    {
        unsigned long now = millis();

        if (now - lastUpdate >= 1000 && totalSeconds >= 0) {
            lastUpdate = now;

            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;

            displayController.lcd.clear();
            displayController.lcd.setCursor(0, 1);
            displayController.lcd.print("Time: ");
            if (minutes < 10) displayController.lcd.print("0");
            displayController.lcd.print(minutes);
            displayController.lcd.print(":");
            if (seconds < 10) displayController.lcd.print("0");
            displayController.lcd.print(seconds);
            displayController.lcd.print("  "); // Clear leftover chars
            handleBuzzer(totalSeconds);
            totalSeconds--;
        }
    }
}

void handleBuzzer(int totalSeconds) {
  if (totalSeconds > 0) {
    unsigned long now = millis();

    // Compute dynamic beep interval (accelerates toward zero)
    int minInterval = 30;   // fastest beep
    int maxInterval = 5000;  // slowest beep

    int interval = map(totalSeconds,
                       dominationGameMinutes * 60, 0,
                       maxInterval, minInterval);
    interval = constrain(interval, minInterval, maxInterval);

    // Time to beep?
    if (now - lastBeep >= (unsigned long)interval) {
      lastBeep = now;

      tone(BUZZER_PIN, 2000, 40);  // 40ms beep, non-blocking
    }
  }

  // Final rapid alarm when timer reaches zero
  if (totalSeconds == 0 && !alarmDone) {
    tone(BUZZER_PIN, 2500);  // Continuous
    delay(3000);          // for 3 seconds
    noTone(BUZZER_PIN);
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

