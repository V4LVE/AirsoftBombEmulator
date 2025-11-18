#ifndef DOMINATIONGAMECONTROLLER_H
#define DOMINATIONGAMECONTROLLER_H

#include <Arduino.h>
#include "DisplayController.h"
#include <Config.h>
#include <keypad.h>

extern DisplayController displayController;
extern Keypad customKeyPad;

extern int dominationGameMinutes;

extern byte bar1[8];
extern byte bar2[8];
extern byte bar3[8];
extern byte bar4[8];
extern byte bar5[8];

void startDominationMode();
void enterGameTime();

#endif;