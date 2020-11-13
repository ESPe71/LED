#include "led.h"

#define LED_PIN    D6
#define LED_COUNT 100

LED* led = nullptr;

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\n\n");
  Serial.println("Setting up.");
  led = new LED(LED_COUNT, LED_PIN);
  Serial.println("Setup ready.");
}


void loop() {
//  Serial.println("loop started.");
  led->loop();
//  Serial.println("loop ended.");
}
