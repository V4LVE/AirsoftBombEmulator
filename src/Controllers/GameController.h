#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <Arduino.h>
#include "DisplayController.h"
#include <Config.h>
#include <keypad.h>

extern DisplayController displayController;
extern Keypad customKeyPad;

extern int dominationGameMinutes;

void startDominationMode();
void enterGameTime();

#endif;