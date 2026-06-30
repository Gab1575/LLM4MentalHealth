#include "BluetoothSerial.h"

// Check if Bluetooth is properly enabled in the board configuration
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

// --- Encoder Hardware ---
const int encoderPinA = 34;
const int encoderPinB = 35;

volatile long encoderCount = 0;
const int encoderDirection = 1; // Change to -1 to flip readings if needed

// Interrupt Service Routine
void IRAM_ATTR readEncoder() {
  if (digitalRead(encoderPinB) > 0) {
    encoderCount += encoderDirection;
  } else {
    encoderCount -= encoderDirection;
  }
}

void setup() {
  // Start the standard USB serial for debugging
  Serial.begin(115200);
  
  // Initialize Encoder Pins
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), readEncoder, RISING);

  // Start Bluetooth Serial with a custom device name
  SerialBT.begin("ESP32-Rover-Encoder"); 
  Serial.println("Bluetooth Started! Ready to pair.");
}

void loop() {
  // Use a non-blocking timer to send data at 10Hz (every 100ms)
  // This prevents flooding the Bluetooth serial buffer
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 100) { 
    lastTime = millis();
    
    // Briefly pause interrupts to safely copy the volatile variable
    noInterrupts();
    long currentCount = encoderCount;
    interrupts();

    // Send the reading to the paired laptop
    SerialBT.println(currentCount); 
  }
}