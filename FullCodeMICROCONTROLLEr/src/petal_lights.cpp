#include "petal_lights.h"
#include <Arduino.h>

CRGB leds[NUM_LEDS];

void petalLightsBegin() {
  FastLED.addLeds<WS2812B, dataPin, GRB>(leds, NUM_LEDS);
  petalLightsClear(); // Ensure they start off
}

void petalLightsUpdate(int LEDIndex, uint32_t hexColor, int brightness) {
  // 1. Convert the HEX integer directly into a FastLED CRGB object
  CRGB color = hexColor; 
  
  // 2. Scale the individual LED's brightness (0-255)
  color.nscale8_video(brightness); 
  
  // 3. Apply to array, BUT DO NOT SHOW YET
  leds[LEDIndex] = color;
}

// 4. Create a dedicated show function
void petalLightsShow() {
    FastLED.show();
}

void petalLightsClear() {
  memset(leds, 0, sizeof(leds));
  FastLED.show();
}