#include <Arduino.h>

// Define the analog pin
const int batteryPin = 32;

// Voltage divider resistors (in ohms or kilohms, just keep units consistent)
const float R1 = 47000.0; 
const float R2 = 10000.0; 

// The ESP32's theoretical maximum voltage reading
// Note: You may need to tweak this slightly (e.g., to 3.28 or 3.32) 
// based on your specific board's actual 3.3V pin output.
const float referenceVoltage = 3.3; 

void setup() {
  Serial.begin(115200);
  
  // Set the resolution to 12-bit (0-4095), which is default on ESP32
  analogReadResolution(12);
}

void loop() {
  // 1. Read the raw analog value (0 to 4095)
  int rawADC = analogRead(batteryPin);
  
  // 2. Calculate the voltage at the GPIO pin
  float pinVoltage = (rawADC / 4095.0) * referenceVoltage;
  
  // 3. Calculate the actual battery voltage using the voltage divider formula
  // V_batt = V_pin * ((R1 + R2) / R2)
  float voltageMultiplier = (R1 + R2) / R2;
  float batteryVoltage = pinVoltage * voltageMultiplier;
  
  // 4. Print the results
  Serial.print("Raw ADC: ");
  Serial.print(rawADC);
  Serial.print(" | Pin Voltage: ");
  Serial.print(pinVoltage, 2); // Print with 2 decimal places
  Serial.print("V | Battery Voltage: ");
  Serial.print(batteryVoltage, 2);
  Serial.println("V");
  
  // Wait a second before reading again
  delay(1000);
}