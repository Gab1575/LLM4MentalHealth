// petal_lights.cpp - Drives the 5 WS2812B petal LEDs via FastLED. Frames are
// built into the `leds` buffer in memory (petalLightsUpdate) and only pushed
// to the hardware once the buffer is fully updated (petalLightsShow), so a
// full frame update is atomic from the LEDs' point of view. Also doubles as
// the connection-status indicator during WiFi/agent setup
// (petalLightsConnectionSpin).

#include "petal_lights.h"
#include <Arduino.h>

CRGB leds[NUM_LEDS];

// Registers the LED strip with FastLED and clears it so the petals start off.
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

// Turns every petal LED off immediately (bypasses the buffer-then-show
// pattern above since there's nothing to preserve).
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
