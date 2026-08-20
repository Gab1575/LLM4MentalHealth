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

void petalLightsConnectionSpin(uint32_t hexColor, unsigned long stepDelayMs) {
  static uint8_t spinPos = 0;
  static unsigned long lastStep = 0;

  unsigned long now = millis();
  if (now - lastStep < stepDelayMs) return; // throttle: callers can invoke this freely
  lastStep = now;

  CRGB color = hexColor;
  CRGB trail = color;
  trail.nscale8_video(60); // dim trailing petal so the spin direction is visible

  for (int i = 0; i < NUM_LEDS; i++) {
    if (i == spinPos) {
      leds[i] = color;
    } else if (i == (spinPos + NUM_LEDS - 1) % NUM_LEDS) {
      leds[i] = trail;
    } else {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();

  spinPos = (spinPos + 1) % NUM_LEDS;
}
