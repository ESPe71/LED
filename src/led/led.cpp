// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 µF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

// see https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/

#include "led.h"

LED::LED(int ledCount, int pin) {
  strip = new Adafruit_NeoPixel(ledCount, pin, NEO_BRG + NEO_KHZ800);
  strip->begin();
  strip->setBrightness(brightness);
  setAll(0,0,0); // Set all black

  randomSeed(random(326874522));
  nextEffect();
}

LED::~LED() {
  if(strip != nullptr) {
    delete strip;
    strip = nullptr;
  }
}

void LED::loop() {
  static long time = 0;
  static long duration = 15000;

  if(millis() - time > duration) {
    nextEffect();
    time = millis();
  }
  switch(effect) {
    case  0: SnowSparkle(0x10, 0x10, 0x10, 20, 200);
             break;
    case  1: Sparkle(random(255), random(255), random(255), 0);
             break;
    case  2: TwinkleRandom(20, 100, false);
             break;
    case  3: Twinkle(random(255), random(255), random(255), 10, 100, false);
             break;
    case  4: rainbow(velocity / 5);
             break;
    case  5: if(random(100) < 50) {
               Strobe(0xff, 0xff, 0xff, 10, 50, 0);
             }
             meteorRain(0xff,0xff,0xff,10, 64, true, 30);
             if(random(100) < 30) {
               Strobe(0xff, 0xff, 0xff, 10, 50, 0);
             }
             break;
    case  6: RunningLights(random(255), random(255), random(255), 50);
             break;
    case  7: NewKITT(random(255), random(255), random(255), 8, 10, 50);
             break;
    case  8: FadeInOut(random(255), random(255), random(255));
             break;
    case  9: colorWipe(strip->Color(random(255), random(255), random(255)), velocity);
             break;
    case 10: theaterChase(strip->Color(random(255), random(255), random(255)), velocity);
             break;
    case 11: theaterChaseRainbow(velocity);
             break;
    default: nextEffect();
  }
  
}

void LED::nextEffect() {
  if(!staticEffect) {
    if(randomEffect) {
      effect = random(numEffects);
    }
    else {
      effect++;
    }
  }
  if(effect > numEffects ||
     effect < 0) {
    if(!staticEffect) {
      effect = -1;
      nextEffect();
    }
    else {
      effect = 0;
    }
  }
}


// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip->Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void LED::colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip->numPixels(); i++) { // For each pixel in strip...
    strip->setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip->show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip->Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void LED::theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip->clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip->numPixels(); c += 3) {
        strip->setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip->show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void LED::rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip->numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip->numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip->numPixels());
      // strip->ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip->gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip->setPixelColor(i, strip->gamma32(strip->ColorHSV(pixelHue)));
    }
    strip->show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void LED::theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip->clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip->numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip->numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip->numPixels();
        uint32_t color = strip->gamma32(strip->ColorHSV(hue)); // hue -> RGB
        strip->setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip->show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
void LED::RGBLoop(){
  for(int j = 0; j < 3; j++ ) {
    // Fade IN
    for(int k = 0; k < 256; k++) {
      switch(j) {
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      strip->show();
      delay(3);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) {
      switch(j) {
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      strip->show();
      delay(3);
    }
  }
}

void LED::FadeInOut(byte red, byte green, byte blue) {
  float r, g, b;

  for(int k = 0; k < 256; k=k+1) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    strip->show();
    delay(3);
  }
  delay(10);

  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    strip->show();
    delay(3);
  }
}

void LED::meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);

  int numLeds = strip->numPixels();
  for(int i = 0; i < numLeds+numLeds; i++) {
   
   
    // fade brightness all LEDs one step
    for(int j=0; j<numLeds; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <numLeds) && (i-j>=0) ) {
        strip->setPixelColor(i-j, red, green, blue);
      }
    }
   
    strip->show();
    delay(SpeedDelay);
  }
}

void LED::Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause) {
  for(int j = 0; j < StrobeCount; j++) {
    setAll(red,green,blue);
    strip->show();
    delay(FlashDelay);
    setAll(0,0,0);
    strip->show();
    delay(FlashDelay);
  }
 
 delay(EndPause);
}

void LED::HalloweenEyes(byte red, byte green, byte blue,
                   int EyeWidth, int EyeSpace,
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause) {
  randomSeed(analogRead(0));
 
  int i;
  int StartPoint  = random( 0, strip->numPixels() - (2*EyeWidth) - EyeSpace );
  int Start2ndEye = StartPoint + EyeWidth + EyeSpace;
 
  for(i = 0; i < EyeWidth; i++) {
    strip->setPixelColor(StartPoint + i, red, green, blue);
    strip->setPixelColor(Start2ndEye + i, red, green, blue);
  }
 
  strip->show();
 
  if(Fade==true) {
    float r, g, b;
 
    for(int j = Steps; j >= 0; j--) {
      r = j*(red/Steps);
      g = j*(green/Steps);
      b = j*(blue/Steps);
     
      for(i = 0; i < EyeWidth; i++) {
        strip->setPixelColor(StartPoint + i, r, g, b);
        strip->setPixelColor(Start2ndEye + i, r, g, b);
      }
     
      strip->show();
      delay(FadeDelay);
    }
  }
 
  setAll(0,0,0); // Set all black
 
  delay(EndPause);
}

void LED::CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  int numLeds = strip->numPixels();

  for(int i = 0; i < numLeds-EyeSize-2; i++) {
    setAll(0,0,0);
    strip->setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      strip->setPixelColor(i+j, red, green, blue);
    }
    strip->setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    strip->show();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for(int i = numLeds-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    strip->setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      strip->setPixelColor(i+j, red, green, blue);
    }
    strip->setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    strip->show();
    delay(SpeedDelay);
  }
 
  delay(ReturnDelay);
}

void LED::NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void LED::CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  int numLeds = strip->numPixels();
  for(int i =((numLeds-EyeSize)/2); i>=0; i--) {
    setAll(0,0,0);
   
    strip->setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      strip->setPixelColor(i+j, red, green, blue);
    }
    strip->setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
   
    strip->setPixelColor(numLeds-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      strip->setPixelColor(numLeds-i-j, red, green, blue);
    }
    strip->setPixelColor(numLeds-i-EyeSize-1, red/10, green/10, blue/10);
   
    strip->show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LED::OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  int numLeds = strip->numPixels();
  for(int i = 0; i<=((numLeds-EyeSize)/2); i++) {
    setAll(0,0,0);
   
    strip->setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      strip->setPixelColor(i+j, red, green, blue);
    }
    strip->setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
   
    strip->setPixelColor(numLeds-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      strip->setPixelColor(numLeds-i-j, red, green, blue);
    }
    strip->setPixelColor(numLeds-i-EyeSize-1, red/10, green/10, blue/10);
   
    strip->show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LED::LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  int numLeds = strip->numPixels();
  for(int i = 0; i < numLeds-EyeSize-2; i++) {
    setAll(0,0,0);
    strip->setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      strip->setPixelColor(i+j, red, green, blue);
    }
    strip->setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    strip->show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LED::RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  int numLeds = strip->numPixels();
  for(int i = numLeds-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    strip->setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      strip->setPixelColor(i+j, red, green, blue);
    }
    strip->setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    strip->show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LED::Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
//  Serial.printf("Start LED:Twinkle(%d, %d, %d, %d, %d, %s)\n", red, green, blue, Count, SpeedDelay, OnlyOne?"true":"false");
  setAll(0,0,0);
 
  for (int i=0; i<Count; i++) {
     strip->setPixelColor(random(strip->numPixels()),red,green,blue);
     strip->show();
     delay(SpeedDelay);
     if(OnlyOne) {
       setAll(0,0,0);
     }
   }
 
  delay(SpeedDelay);
//  Serial.printf("End LED:Twinkle\n");
}

void LED::TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
//  Serial.printf("Start LED:TwinkleRandom(%d, %d, %s)\n", Count, SpeedDelay, OnlyOne?"true":"false");
  setAll(0,0,0);
 
  for (int i=0; i<Count; i++) {
     strip->setPixelColor(random(strip->numPixels()),random(0,255),random(0,255),random(0,255));
     strip->show();
     delay(SpeedDelay);
     if(OnlyOne) {
       setAll(0,0,0);
     }
   }
 
  delay(SpeedDelay);
//  Serial.printf("End LED:TwinkleRandom\n");
}

void LED::Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
//  Serial.printf("Start LED:Sparkle(%d, %d, %d, %d)\n", red, green, blue, SpeedDelay);
  int Pixel = random(strip->numPixels());
  strip->setPixelColor(Pixel,red,green,blue);
  strip->show();
  delay(SpeedDelay);
  strip->setPixelColor(Pixel,0,0,0);
//  Serial.printf("End LED:Sparkle\n");
}

void LED::SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
//  Serial.printf("Start LED:SnowSparkle(%d, %d, %d, %d, %d)\n", red, green, blue, SparkleDelay, SpeedDelay);
  setAll(red,green,blue);
 
  int Pixel = random(strip->numPixels());
  strip->setPixelColor(Pixel,0xff,0xff,0xff);
  strip->show();
  delay(SparkleDelay);
  strip->setPixelColor(Pixel,red,green,blue);
  strip->show();
  delay(SpeedDelay);
//  Serial.printf("End LED:SnowSparkle\n");
}

void LED::RunningLights(byte red, byte green, byte blue, int WaveDelay) {
//  Serial.printf("Start LED::RunningLights(%d, %d, %d, %d)\n", red, green, blue, WaveDelay);
  int Position=0;
  int numLeds = strip->numPixels();
 
  for(int j=0; j<numLeds*2; j++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<numLeds; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        strip->setPixelColor(i,((sin(i+Position) * 127 + 128)/255)*red,
                               ((sin(i+Position) * 127 + 128)/255)*green,
                               ((sin(i+Position) * 127 + 128)/255)*blue);
      }
     
      strip->show();
      delay(WaveDelay);
  }
//  Serial.println("End LED::RunningLights\n");
}

void LED::Fire(int Cooling, int Sparking, int SpeedDelay) {
  const int numLeds = strip->numPixels();
  byte heat[numLeds];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < numLeds; i++) {
    cooldown = random(0, ((Cooling * 10) / numLeds) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= numLeds - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < numLeds; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  strip->show();
  delay(SpeedDelay);
}

void LED::setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    strip->setPixelColor(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    strip->setPixelColor(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    strip->setPixelColor(Pixel, heatramp, 0, 0);
  }
}

void LED::fadeToBlack(int ledNo, byte fadeValue) {
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;
 
  oldColor = strip->getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
  g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
  b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
 
  strip->setPixelColor(ledNo, r,g,b);
}

void LED::setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < strip->numPixels(); i++ ) {
    strip->setPixelColor(i, strip->Color(red, green, blue));
  }
  strip->show();
}
