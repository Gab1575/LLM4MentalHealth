#include "petal_lights.h"
#include <Arduino.h>

CRGB leds[NUM_LEDS];

void petalLightsBegin() {
  FastLED.addLeds<WS2812B, dataPin, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);   //initial brightness of the LEDs to maximum
}

void petalLightsUpdate(int LEDIndex, CRGB color) {
  leds[LEDIndex] = color;
  FastLED.show();
}

void petalLightsSetBrightness(int brightness) {
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void petalLightsClear() {
  memset(leds, 0, sizeof(leds));
  FastLED.show();
}