#include "led.h"

#define LED_PIN    D6
#define LED_COUNT 100

LED* led = nullptr;

void setup() {
  Serial.begin(9600);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
  led = new LED(LED_COUNT, LED_PIN);
}


void loop() {
  led->loop();
}
