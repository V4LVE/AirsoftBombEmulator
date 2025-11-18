#ifndef DEFUSEGAMECONTROLLER_H
#define DEFUSEGAMECONTROLLER_H

#include <Arduino.h>
#include "DisplayController.h"
#include <Config.h>
#include <keypad.h>

extern DisplayController displayController;
extern Keypad customKeyPad;

extern int defuseGameMinutes;

extern byte bar1[8];
extern byte bar2[8];
extern byte bar3[8];
extern byte bar4[8];
extern byte bar5[8];

void startDefuseMode();
void enterGameTimeDefuse();

#endif