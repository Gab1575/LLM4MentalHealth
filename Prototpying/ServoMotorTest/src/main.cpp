#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Initialize the PCA9685 using the default I2C address (0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Standard servo pulse lengths
#define SERVOMIN  200
#define SERVOMAX  500

#define currentDelay 2 // Delay in milliseconds between servo position updates

void setup() {
  Serial.begin(9600);
  
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);  // Analog/digital servos operate at 50Hz
}

void loop() {        
    
  Serial.println("Sweeping Forward...");

  for (uint16_t pulselen = SERVOMIN; pulselen <= SERVOMAX; pulselen++) {
    pwm.setPWM(0, 0, pulselen);
    pwm.setPWM(1, 0, pulselen);
    pwm.setPWM(2, 0, pulselen);
        
    delay(currentDelay); 
  }
    
  delay(500); // Brief mechanical pause at 180 degrees
    
  Serial.println("Sweeping Reverse...");
  for (uint16_t pulselen = SERVOMAX; pulselen >= SERVOMIN; pulselen--) {
    pwm.setPWM(0, 0, pulselen);
    pwm.setPWM(1, 0, pulselen);
    pwm.setPWM(2, 0, pulselen);
    
    delay(currentDelay);
  }
}
