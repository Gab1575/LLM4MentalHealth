#ifndef PETAL_LIGHTS_H
#define PETAL_LIGHTS_H

#include <Arduino.h>
#include <FastLED.h>

#define dataPin 27      //Set the data pin for the LEDs
#define NUM_LEDS 5      //Set the number of LEDs

void petalLightsBegin();
void petalLightsUpdate(int LEDIndex, uint32_t hexColor, int brightness);
void petalLightsClear();
void petalLightsShow();

// Connection status indicator: advances a single "comet" of hexColor one petal
// further around the ring each time it's called, no faster than stepDelayMs apart
// (safe to call every loop iteration - it throttles itself internally).
// Use while trying to connect: yellow for WiFi, green for the ROS agent/host.
void petalLightsConnectionSpin(uint32_t hexColor, unsigned long stepDelayMs = 120);


#endif // PETAL_LIGHTS_H
