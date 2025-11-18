#include <Arduino.h>
#include "Controllers/DisplayController.h"
#include "Controllers/GameController.h"
#include <keypad.h>
#include <Config.h>

Keypad customKeyPad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
DisplayController displayController;

int dominationGameMinutes = 0;

enum MenuState {
    MAIN_MENU,
    GAME_MENU,
    SETUP_GAME_MENU_DOMINATION,
    IN_GAME_DOMINATION,
    IN_GAME_DEFUSE,
    IN_GAME_SABOTAGE,
    SET_TIME_MENU
};

MenuState menustate = MAIN_MENU;

void keypadEvent(KeypadEvent key){
    switch (customKeyPad.getState()){
    case PRESSED:
        if (menustate == MAIN_MENU) { // main menu
          if (key == '1') {
            menustate = GAME_MENU;
            displayController.writeGameMenu();
          }
        } else if (menustate == GAME_MENU) { // game menu
          if (key == '1') {
            menustate = SETUP_GAME_MENU_DOMINATION;
            displayController.writeDominationMenu();
          } else if (key == '2') {
            //startTeamEliminationMode();
          } else if (key == '3') {
            //startCaptureMode();
          } else if (key == 'B') { // back to main menu
            menustate = MAIN_MENU;
            displayController.writeMenuScreen();
          }
        } else if (menustate == SETUP_GAME_MENU_DOMINATION) { // in domination game mode
          if (key == '1')
          {
            menustate = IN_GAME_DOMINATION;
            startDominationMode();
            menustate = SETUP_GAME_MENU_DOMINATION;
          }
          else if (key == '2')
          {
            menustate = SET_TIME_MENU;
            enterGameTime();
            menustate = SETUP_GAME_MENU_DOMINATION;
          }
          else if (key == 'B') { // back to game menu
            menustate = GAME_MENU;
            displayController.writeGameMenu();
          } 
        }
        break;
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Airsoft Bomb Emulator Starting...");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  customKeyPad.addEventListener( keypadEvent );

  displayController.initializeDisplay();
  displayController.writeMenuScreen();
}

void loop() {
    customKeyPad.getKey();
}

