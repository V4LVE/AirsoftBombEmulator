#include <Arduino.h>
#include "Application.h"

namespace {
Application application;
}

void setup() { application.begin(); }
void loop() { application.update(); }

