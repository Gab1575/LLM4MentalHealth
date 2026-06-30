#include <Arduino.h>
#include "petal_lights.h"
#include "servo_control.h"
#include "N20.h"
#include "bluetooth.h"

void test2();
void test1();
void test3();

void setup() {
  Serial.begin(115200);
  
  // Initialize all control systems
  petalLightsBegin();
  servoControlBegin();
  servoControlReset();
  n20MotorBegin();
  petalLightsSetBrightness(200); // Clear all LEDs at startup
  
  Serial.println("All systems initialized!");

  // Initialize Bluetooth (ESP32)
  if (!btInit("FlowerPower")) {
    Serial.println("BT init failed");
  } else {
    Serial.println("Bluetooth initialized");
  }
}

void loop() {
    btPoll(); // Keep bluetooth alive
    n20EncoderUpdateVelocity();
    test3();
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
  for (int i=255; i>=50; i--){
    petalLightsSetBrightness(i); 
    delay(10);
  }
  for (int i=50; i<=255; i++){
    petalLightsSetBrightness(i); 
    delay(10);
  }
}


void test3() {
  // Static variables keep their values between loop() iterations
  static bool isInitialized = false;
  static bool movingForward = true;
  static long targetTicks = 0;
  
  const int TICKS_TO_MOVE = 1500;
  const int MOTOR_SPEED = 150;

  // Safely read the current encoder ticks
  noInterrupts();
  long currentTicks = n20EncoderTicks;
  interrupts();

  // 1. Initialization (Runs only the very first time test3 is called)
  if (!isInitialized) {
    petalLightsSetBrightness(255); // Set brightness to a visible level
    for (int i = 0; i < NUM_LEDS; i++) {
      petalLightsUpdate(i, CRGB::Yellow); // Indicate test3 is initializing
    }
    targetTicks = currentTicks + TICKS_TO_MOVE;
    n20MotorControl(N20_FORWARD, MOTOR_SPEED);
    btLog("Test3 Sweep Started. Target: ", targetTicks, " ticks");
    
    isInitialized = true;
    return; // Exit the function for this cycle to let the motor move
  }

  // 2. The Sweeping Logic
  if (movingForward) {
    // Check if we reached the forward target
    if (currentTicks >= targetTicks) {
      movingForward = false; // Swap state
      targetTicks = currentTicks - TICKS_TO_MOVE; // Calculate new backward target
      
      n20MotorControl(N20_BACKWARD, MOTOR_SPEED);
      btLog("Swapped direction: BACKWARD. Target: ", targetTicks, " ticks");
    }
  } 
  else { // Moving Backward
    // Check if we reached the backward target
    if (currentTicks <= targetTicks) {
      movingForward = true; // Swap state
      targetTicks = currentTicks + TICKS_TO_MOVE; // Calculate new forward target
      
      n20MotorControl(N20_FORWARD, MOTOR_SPEED);
      btLog("Swapped direction: FORWARD. Target: ", targetTicks, " ticks");
    }
  }
}