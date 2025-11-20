#include "DefuseGameController.h"
#include <Arduino.h>

#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_G5  784
#define NOTE_C6  1047

String defuseTimeInput = "";
String defuseCodeInput = "";
bool backback = false;

bool bombBlown = false;
bool bombArmed = false;
bool bombDefused = false;
bool defusingInProgress = false;

unsigned long lastUpdateDefuse = 0;
unsigned long lastBeepDefuse = 0;
unsigned long timeElapsedBeforeDefuse = 0;
unsigned long holdStart = 0;
bool exitGameDefuse = false;
bool redHoldingDefuse = false;

// Forward declarations
void writeStartObjectiveScreen();
void writeDefuseObjectiveScreen();
void plantBomb();
void defuseBomb();
void writeBombBlownScreen();
void writeBombDefusedScreen();
void resetGameDefuse();
void playVictoryMelody();

void startDefuseGame()
{
    if (defuseGameMinutes < 1)
    {
        displayController.lcd.clear();
        displayController.lcd.setCursor(0, 0);
        displayController.lcd.print("Set valid time!");
        delay(2000);
        displayController.writeDefuseMenu();
        return;
    }

    resetGameDefuse();

    int totalSeconds = defuseGameMinutes * 60;

    writeStartObjectiveScreen();

    while (!bombBlown && totalSeconds >= 0 && !bombDefused && !exitGameDefuse)
    {
        unsigned long now = millis();

        char key = customKeyPad.getKey();
        if (key == 'B' && redHoldingDefuse) {
            exitGameDefuse  = true;
        }

        if (!bombArmed)
        {
            plantBomb();
        }

        if (now - lastUpdateDefuse >= 1000 && totalSeconds >= 0) {
            lastUpdateDefuse = now;
            timeElapsedBeforeDefuse++;
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;

            if (bombArmed && !bombDefused)
            {
                defuseBomb();
            }
            

            
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
    if (bombDefused)
    {
        writeBombDefusedScreen();
        playVictoryMelody();
    }
    else
    {
        bombBlown = true;
    }


    if (bombBlown)
    {
        writeBombBlownScreen();
    }
    

    bool continueLoop = true;
    while (continueLoop)
    {
        char key = customKeyPad.getKey();
        if (key == 'B') 
        {
            continueLoop = false;
        }


    }
}

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

void enterCodeDefuse()
{
    displayController.setCodeMenu();
        while (backback == false && defuseCodeInput.length() < 6)
        {
            char key = customKeyPad.getKey();
            if (key >= '0' && key <= '9') {
                defuseCodeInput += key;
                displayController.lcd.setCursor(defuseCodeInput.length() - 1, 2);
                displayController.lcd.print(key);
            }
            if (key == 'A')
            {

                backback = true;
            }
            
        }
                defuseCodeValue = defuseCodeInput.toInt();
                defuseCodeInput = "";

        backback = false;
        displayController.writeDefuseMenu();
}

void plantBomb()
{
    bool planted = false;
    long enteredCode = 0;

    while (!planted)
        {
            char key = customKeyPad.getKey();
            if (key >= '0' && key <= '9') {
                defuseCodeInput += key;
                displayController.lcd.setCursor(defuseCodeInput.length() - 1, 2);
                displayController.lcd.print(key);
            }
            if (key == '#')
            {
                enteredCode = defuseCodeInput.toInt();
                defuseCodeInput = "";
                if (enteredCode == defuseCodeValue) {
                    planted = true;
                    bombArmed = true;
                    writeDefuseObjectiveScreen();
                } else {
                    displayController.lcd.setCursor(0, 3);
                    displayController.lcd.print("Wrong Code!    ");
                    delay(2000);
                    writeStartObjectiveScreen();
                }
            }
        }
}

void defuseBomb()
{
    unsigned long now = millis();

  bool bluePressed = digitalRead(BUTTON_PIN_BLUE) == LOW; // LOW if pressed
  bool redPressed  = digitalRead(BUTTON_PIN_RED) == LOW;   // LOW if pressed

    if (!bluePressed || !redPressed)
    {
        drawProgressBar(0);
        writeDefuseObjectiveScreen();
    }
    
    if (redPressed)
    {
        redHoldingDefuse = true;
    }
    else
    {
        redHoldingDefuse = false;
    }
    
    if (bluePressed && redPressed) {

        if (!defusingInProgress) {   // Just started holding
            defusingInProgress = true;
            holdStart = now;
        }

        unsigned long heldFor = now - holdStart;
        float progress = min(1.0, heldFor / 10000.0);
        displayController.lcd.clear();
        displayController.lcd.setCursor(0, 2);
        displayController.lcd.print("Defusing...");
        drawProgressBar(progress);
        
        if (heldFor >= 10000) {
            drawProgressBar(1.0); // full bar
            bombArmed = false;
            bombDefused = true;
            defusingInProgress = false;
    }
  }
}

void writeStartObjectiveScreen()
{
    displayController.lcd.clear();
    displayController.lcd.setCursor(0, 0);
    displayController.lcd.print("Plant the Bomb!");
    displayController.lcd.setCursor(0, 1);
    displayController.lcd.print("Enter the code!");
    displayController.lcd.setCursor(7, 2);
    displayController.lcd.print("# to Plant");
    displayController.lcd.setCursor(0, 3);
    displayController.lcd.print("Bomb: Disarmed");
}

void writeDefuseObjectiveScreen()
{
    displayController.lcd.clear();
    displayController.lcd.setCursor(0, 0);
    displayController.lcd.print("BOMB IS ARMED!");
    displayController.lcd.setCursor(0, 2);
    displayController.lcd.print("To disarm the bomb");
    displayController.lcd.setCursor(0, 3);
    displayController.lcd.print("Hold red and blue");
}

void writeBombBlownScreen()
{
    displayController.lcd.clear();
    displayController.lcd.setCursor(0, 0);
    displayController.lcd.print("BOMB HAS EXPLODED!");
    displayController.lcd.setCursor(0, 1);
    displayController.lcd.print("Terrorists Win!");
    displayController.lcd.setCursor(0, 3);
    displayController.lcd.print("Press b to exit");

}

void writeBombDefusedScreen()
{

    int minutes = timeElapsedBeforeDefuse / 60;
    int seconds = timeElapsedBeforeDefuse % 60;

    displayController.lcd.clear();
    displayController.lcd.setCursor(0, 0);
    displayController.lcd.print("BOMB DEFUSED!");
    displayController.lcd.setCursor(0, 1);
    displayController.lcd.print("CT Wins!");
    displayController.lcd.setCursor(0, 2);
    displayController.lcd.print("Defuse Time: ");
    if (minutes < 10) displayController.lcd.print("0");
    displayController.lcd.print(minutes);
    displayController.lcd.print(":");
    if (seconds < 10) displayController.lcd.print("0");
    displayController.lcd.print(seconds);
    displayController.lcd.print("  "); // Clear leftover chars
    displayController.lcd.setCursor(0, 3);
    displayController.lcd.print("Press B to exit");

}

void resetGameDefuse()
{
    bombBlown = false;
    bombArmed = false;
    bombDefused = false;
    defusingInProgress = false;
    timeElapsedBeforeDefuse = 0;
}

void playVictoryMelody() {
  tone(BUZZER_PIN, NOTE_C5, 150);
  delay(180);

  tone(BUZZER_PIN, NOTE_E5, 150);
  delay(180);

  tone(BUZZER_PIN, NOTE_G5, 200);
  delay(250);

  tone(BUZZER_PIN, NOTE_C6, 300);
  delay(350);

  noTone(BUZZER_PIN);
}