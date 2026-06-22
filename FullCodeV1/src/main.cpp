#include <Arduino.h>
#include "petal_lights.h"

void test2();
void test1();

void setup() {
  Serial.begin(115200);
  petalLightsBegin();
}

void loop() {
  test2();
}

void test1(){ //individual light and colour test
  // Example usage: Update LED at index 0 to red color
  petalLightsUpdate(0, CRGB::Red);
  delay(1000); // Wait for 1 second

  // Update LED at index 1 to green color
  petalLightsUpdate(1, CRGB::Green);
  delay(1000); // Wait for 1 second

  // Update LED at index 2 to blue color
  petalLightsUpdate(2, CRGB::Blue);
  delay(1000); // Wait for 1 second

  // Update LED at index 3 to yellow color
  petalLightsUpdate(3, CRGB::Yellow);
  delay(1000); // Wait for 1 second

  // Update LED at index 4 to purple color
  petalLightsUpdate(4, CRGB::Purple);
  delay(1000); // Wait for 1 second

  // Clear all LEDs
  petalLightsClear();
  delay(1000); // Wait for 1 second
}

void test2(){ //brightness sweep test
  for (int i = 0; i < NUM_LEDS; i++) {
    petalLightsUpdate(i, CRGB::Red);
  }
  for (int i=255; i>=1; i--){
    petalLightsSetBrightness(i); 
    delay(10);
  }
  for (int i=1; i<=255; i++){
    petalLightsSetBrightness(i); 
    delay(10);
  }
}