#ifndef __LED_H__
#define __LED_H__

#include <Adafruit_NeoPixel.h>

class LED {

  public:
    LED(int ledCount, int pin);
    ~LED();
    
    void loop();

    boolean randomEffect = true;
  private:
    Adafruit_NeoPixel* strip = nullptr;
    
    int brightness = 50; // (max = 255)
    int velocity = 50;

    const int numEffects = 11;
    int effect = -1;
    boolean staticEffect = false;

    void nextEffect();

    void colorWipe(uint32_t color, int wait);
    void theaterChase(uint32_t color, int wait);
    void rainbow(int wait);
    void theaterChaseRainbow(int wait);
    void RGBLoop();
    void FadeInOut(byte red, byte green, byte blue);
    void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
    void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause);
    void HalloweenEyes(byte red, byte green, byte blue, int EyeWidth, int EyeSpace, boolean Fade, int Steps, int FadeDelay, int EndPause);
    void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);

    void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);

    void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne);
    void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne);
    void Sparkle(byte red, byte green, byte blue, int SpeedDelay);
    void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay);
    
    void RunningLights(byte red, byte green, byte blue, int WaveDelay);

    void Fire(int Cooling, int Sparking, int SpeedDelay);
    void setPixelHeatColor (int Pixel, byte temperature);

    void fadeToBlack(int ledNo, byte fadeValue);
    void setAll(byte red, byte green, byte blue);
};

#endif __LED_H__
