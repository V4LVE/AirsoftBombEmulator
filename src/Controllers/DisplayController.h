#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

extern byte bar1[8];
extern byte bar2[8];
extern byte bar3[8];
extern byte bar4[8];
extern byte bar5[8];

#include <Arduino.h>
#include <Config.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <LCD.h>

extern int dominationGameMinutes;
extern int defuseGameMinutes;
extern long defuseCodeValue;

class DisplayController {
public:
  DisplayController() {}

    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,2,1,0,4,5,6,7);

    void initializeDisplay() {
    lcd.setBacklightPin(3,POSITIVE);
    lcd.setBacklight(HIGH); // NOTE: You can turn the backlight off by setting it to LOW instead of HIGH
    lcd.begin(20, 4);
    lcd.clear();
    lcd.createChar(0, bar1);
    lcd.createChar(1, bar2);
    lcd.createChar(2, bar3);
    lcd.createChar(3, bar4);
    lcd.createChar(4, bar5);
    writeTestMessage();
  }

  void writeMenuScreen() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("--Airsoftelite.dk--");
    lcd.setCursor(0, 1);
    lcd.print("1. Select Gamemode");
    lcd.setCursor(0, 3);
    lcd.print("V 1.0.0");
  }

    void writeGameMenu() {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Game Menu:");
    lcd.setCursor(0, 1);
    lcd.print("1. Domination");
    lcd.setCursor(0, 2);
    lcd.print("2. Defuse the Bomb");
    lcd.setCursor(17, 3);
    lcd.print("B >");
    }

    void writeDominationMenu() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Domination Mode");
    lcd.setCursor(0, 1);
    lcd.print("1. Start Game");
    lcd.setCursor(0, 2);
    lcd.print("2. Set Time");
    lcd.setCursor(0, 3);
    lcd.print("Time " + String(dominationGameMinutes) + " mins");
    lcd.setCursor(17, 3);
    lcd.print("B >");
    }

    void writeDefuseMenu() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Defuse Mode");
    lcd.setCursor(0, 1);
    lcd.print("1. Start Game");
    lcd.setCursor(0, 2);
    lcd.print("2. Time 3. Code");
    lcd.setCursor(0, 3);
    lcd.print(String(defuseGameMinutes) + " mins" + "/Code " + String(defuseCodeValue));
    lcd.setCursor(18, 3);
    lcd.print("B>");
    }

    void setTimeMenu() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set Game Time:");
    lcd.setCursor(0, 1);
    lcd.print("Time in minutes:");
    lcd.setCursor(0, 3);
    lcd.print("Press A to set");
    }

    void setCodeMenu() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set Plant Code:");
    lcd.setCursor(0, 1);
    lcd.print("Code:");
    lcd.setCursor(0, 3);
    lcd.print("Press A to set");
    }

private:
    void writeTestMessage() {
    lcd.setCursor(0, 0);
    lcd.print("--Airsoftelite.dk--");
    lcd.setCursor(0, 1);
    lcd.print("Initialized...");
    }


};

#endif;