#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

#include <Arduino.h>
#include <Config.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <LCD.h>

LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x3F dette er adressen vi har findet i "Scanner koden"

class DisplayController {
public:
  DisplayController() {}

    void initializeDisplay() {
    lcd.setBacklightPin(3,POSITIVE);
    lcd.setBacklight(HIGH); // NOTE: You can turn the backlight off by setting it to LOW instead of HIGH
    lcd.begin(20, 4);
    lcd.clear();
    writeTestMessage();
  }

  void writeMenuScreen() {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Main Menu:");
    lcd.setCursor(0, 1);
    lcd.print("1. Select Gamemode");
    lcd.setCursor(0, 2);
    lcd.print("2. Admin Menu");
  }

    void writeGameMenu() {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Game Menu:");
    lcd.setCursor(0, 1);
    lcd.print("1. Domination");
    lcd.setCursor(0, 2);
    lcd.print("2. Defuse the Bomb");
    lcd.setCursor(0, 3);
    lcd.print("3. Sabotage");

    }

private:
    void writeTestMessage() {
    lcd.setCursor(0, 0);
    lcd.print("Airsoft Bomb Sim");
    lcd.setCursor(0, 1);
    lcd.print("Initialized...");
    }
};

#endif