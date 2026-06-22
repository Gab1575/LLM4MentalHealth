#ifndef PETAL_LIGHTS_H
#define PETAL_LIGHTS_H

#include <Arduino.h>
#include <FastLED.h>

#define dataPin 15      //Set the data pin for the LEDs
#define NUM_LEDS 5      //Set the number of LEDs

void petalLightsBegin();
void petalLightsUpdate(int LEDIndex, CRGB color);
void petalLightsSetBrightness(int brightness);
void petalLightsClear();


#endif // PETAL_LIGHTS_H
