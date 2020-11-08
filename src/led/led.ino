#include "led.h"

LED* led = nullptr;

void setup() {
  led = new LED();
}


void loop() {
  led->loop();
}
