#ifndef __LED_H__
#define __LED_H__

#include <Adafruit_NeoPixel.h>

class LED {

  public:
    LED();
    ~LED();
    
    void loop();

  private:
    Adafruit_NeoPixel* strip = nullptr;
    
    int brightness = 50; // Set BRIGHTNESS to about 1/5 (max = 255)
    int velocity = 50;

    void colorWipe(uint32_t color, int wait);
    void theaterChase(uint32_t color, int wait);
    void rainbow(int wait);
    void theaterChaseRainbow(int wait);
    void RGBLoop();
    void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
    
    void fadeToBlack(int ledNo, byte fadeValue);
    void setAll(byte red, byte green, byte blue);
};

#endif __LED_H__
