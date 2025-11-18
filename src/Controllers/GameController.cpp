#include "GameController.h"
#include <Arduino.h>

String dominationTimeInput = "";
bool back = false;

void handleBuzzer(int totalSeconds);
void handleAssetCapture();
void drawProgressBar(float progress);
void setCurrentTeamCapture();
void displayEndGameStats();

bool bombIsBlue = false;
bool bombIsRed = false;

unsigned long blueTime = 0;
unsigned long redTime = 0;

unsigned long blueHoldStart = 0;
unsigned long redHoldStart = 0;

bool blueHolding = false;
bool redHolding  = false;

const unsigned long neutralTime = 5000;  // 5 seconds
const unsigned long captureTime = 10000; // 10 seconds

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
    
    displayController.lcd.clear();
    int totalSeconds = dominationGameMinutes * 60;

    while (totalSeconds >= 0)
    {
        unsigned long now = millis();

        if (now - lastUpdate >= 1000 && totalSeconds >= 0) {
            lastUpdate = now;

            if (bombIsBlue && !bombIsRed) {
                blueTime += 1;   // Blue gains 1 second
            }
            else if (bombIsRed && !bombIsBlue) {
                redTime += 1;    // Red gains 1 second
            }

            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;

            
            displayController.lcd.setCursor(0, 1);
            displayController.lcd.print("Time: ");
            if (minutes < 10) displayController.lcd.print("0");
            displayController.lcd.print(minutes);
            displayController.lcd.print(":");
            if (seconds < 10) displayController.lcd.print("0");
            displayController.lcd.print(seconds);
            displayController.lcd.print("  "); // Clear leftover chars
            handleBuzzer(totalSeconds);
            handleAssetCapture();
            setCurrentTeamCapture();
            totalSeconds--;
        }
    }

    displayEndGameStats();

    bool exit = false;
    while (!exit) {
        char key = customKeyPad.getKey();
        if (key == 'B') {
            exit = true;
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

void handleAssetCapture() {
  unsigned long now = millis();

  bool bluePressed = digitalRead(BUTTON_PIN_BLUE) == LOW; // LOW if pressed
  bool redPressed  = digitalRead(BUTTON_PIN_RED) == LOW;   // LOW if pressed

  if (!blueHolding && !redHolding)
  {
    drawProgressBar(0);
  }
  

  // If both are pressed → ignore (prevents conflict)
  if (bluePressed && redPressed) {
    blueHolding = false;
    redHolding = false;
    return;
  }

  // -----------------------------
  // BLUE BUTTON LOGIC
  // -----------------------------
  if (bluePressed) {

    // If blue already owns the asset → do nothing
    if (bombIsBlue) {
      return;
    }

    if (!blueHolding) {   // Just started holding
      blueHolding = true;
      blueHoldStart = now;
    }

    unsigned long heldFor = now - blueHoldStart;

    float progress = min(1.0, heldFor / 10000.0);
    drawProgressBar(progress);

    // FIRST 5s: neutralize
    if (heldFor >= neutralTime && (bombIsRed || bombIsBlue)) {
      bombIsBlue = false;
      bombIsRed = false;
      setCurrentTeamCapture();
    }

    // AFTER 10s: capture for BLUE
    if (heldFor >= captureTime && !bombIsBlue && !bombIsRed) {
      bombIsBlue = true;
      bombIsRed = false;
      setCurrentTeamCapture();
      drawProgressBar(1.0); // full bar
    }
  }
  else {
    blueHolding = false; // Button released
    //drawProgressBar(0);
  }

  // -----------------------------
  // RED BUTTON LOGIC
  // -----------------------------
  if (redPressed) {

    // If red already owns the asset → do nothing
    if (bombIsRed) {
      return;
    }

    if (!redHolding) {
      redHolding = true;
      redHoldStart = now;
    }

    unsigned long heldFor = now - redHoldStart;
    float progress = min(1.0, heldFor / 10000.0);
    drawProgressBar(progress);

    // FIRST 5s: neutralize
    if (heldFor >= neutralTime && (bombIsRed || bombIsBlue)) {
      bombIsBlue = false;
      bombIsRed = false;
      setCurrentTeamCapture();
    }

    // AFTER 10s: capture for RED
    if (heldFor >= captureTime && !bombIsBlue && !bombIsRed) {
      bombIsRed = true;
      bombIsBlue = false;
      drawProgressBar(1.0);
      setCurrentTeamCapture();
    }
  }
  else {
    redHolding = false;
    //drawProgressBar(0);
  }
}

void drawProgressBar(float progress) {
    // progress range: 0.0 → 1.0
    int totalCells = 20;  
    int filledCells = progress * totalCells * 5; // convert to 1/5th segments

    displayController.lcd.setCursor(0, 0);

    for (int cell = 0; cell < totalCells; cell++) {
        int segment = filledCells - cell * 5;

        if (segment >= 5) {
            displayController.lcd.write(byte(4));  // full block
        } else if (segment >= 1) {
            displayController.lcd.write(byte(segment - 1));  // 1→4 block
        } else {
            displayController.lcd.print(" ");  // empty
        }
    }
}

void setCurrentTeamCapture()
{
    displayController.lcd.setCursor(0, 2);
    if (bombIsBlue && !bombIsRed) {
        displayController.lcd.print("Blue Team Has Bomb ");
    }
    else if (bombIsRed && !bombIsBlue) {
        displayController.lcd.print("Red Team Has Bomb  ");
    }
    else {
        displayController.lcd.print("Bomb Is Neutralized");
    }
}
 
void displayEndGameStats()
{
    unsigned long blueM = blueTime / 60;
    unsigned long blueS = blueTime % 60;

    unsigned long redM = redTime / 60;
    unsigned long redS = redTime % 60;

    displayController.lcd.clear();


    displayController.lcd.setCursor(0, 0);
    displayController.lcd.print("Game Over!");

    // BLUE LINE
    displayController.lcd.setCursor(0, 1);
    displayController.lcd.print("Blue: ");
    if (blueM < 10) displayController.lcd.print("0");
    displayController.lcd.print(blueM);
    displayController.lcd.print(":");
    if (blueS < 10) displayController.lcd.print("0");
    displayController.lcd.print(blueS);

    // RED LINE
    displayController.lcd.setCursor(0, 2);
    displayController.lcd.print("Red : ");
    if (redM < 10) displayController.lcd.print("0");
    displayController.lcd.print(redM);
    displayController.lcd.print(":");
    if (redS < 10) displayController.lcd.print("0");
    displayController.lcd.print(redS);

    displayController.lcd.setCursor(0, 3);
    displayController.lcd.print("Press B to Exit");
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

