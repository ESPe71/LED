// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include "led.h"

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    D6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 100

LED::LED() {
  strip = new Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_BRG + NEO_KHZ800);  
  strip->begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip->show();            // Turn OFF all pixels ASAP
  strip->setBrightness(brightness); // Set BRIGHTNESS (max = 255)
}

LED::~LED() {
  if(strip != nullptr) {
    delete strip;
    strip = nullptr;
  }
}

void LED::loop() {
  RGBLoop();
  meteorRain(0xff,0xff,0xff,10, 64, true, 30);
  
  // Fill along the length of the strip in various colors...
  colorWipe(strip->Color(255,   0,   0), velocity); // Red
  colorWipe(strip->Color(  0, 255,   0), velocity); // Green
  colorWipe(strip->Color(  0,   0, 255), velocity); // Blue
  colorWipe(strip->Color(  0,   255, 255), velocity);
  colorWipe(strip->Color(  255,   0, 255), velocity);
  colorWipe(strip->Color(  255,   255, 0), velocity);
  colorWipe(strip->Color(  255,   255, 255), velocity);

  // Do a theater marquee effect in various colors...
  theaterChase(strip->Color(127, 127, 127), velocity); // White, half brightness
  theaterChase(strip->Color(127,   0,   0), velocity); // Red, half brightness
  theaterChase(strip->Color(  0,   0, 127), velocity); // Blue, half brightness
  theaterChase(strip->Color(  0,   127, 127), velocity);
  theaterChase(strip->Color(  127,   0, 127), velocity);
  theaterChase(strip->Color(  127,   127, 0), velocity);

  rainbow(velocity / 5);         // Flowing rainbow cycle along the whole strip
  theaterChaseRainbow(velocity); // Rainbow-enhanced theaterChase variant
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
