#include <Arduino.h>
#include "Controllers/DisplayController.h"

DisplayController displayController;

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  displayController.initializeDisplay();
}

void loop() {
  displayController.writeMenuScreen();
  delay(2000);
  displayController.writeGameMenu();
  delay(2000);
}