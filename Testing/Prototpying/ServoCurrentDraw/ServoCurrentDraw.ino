#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Initialize the PCA9685 using the default I2C address (0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Standard servo pulse lengths
#define SERVOMIN  150 // Roughly 0 degrees
#define SERVOMAX  600 // Roughly 180 degrees

void setup() {
  Serial.begin(9600);
  Serial.println("--- PCA9685 Dynamic Speed / Amperage Test ---");

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);  // Analog/digital servos operate at 50Hz

  Serial.println("Starting in 3 seconds. Get ready to watch the Ammeter...");
  delay(3000); 
}

void loop() {
  // Iterate from a 5ms delay down to 0ms
  for (int currentDelay = 5; currentDelay >= 0; currentDelay--) {
    
    Serial.print("\n>>> STARTING SWEEP WITH DELAY: ");
    Serial.print(currentDelay);
    Serial.println(" ms <<<");
    
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

    Serial.println("Sweep Complete. Log your peak amperage now.");
    Serial.println("Holding for 4 seconds before the next speed test...");
    delay(4000); // 4 seconds to write down your data
  }

  Serial.println("\n--- ALL TESTS COMPLETE ---");
  Serial.println("Reset the Arduino board to run the test sequence again.");
  
  // Infinite loop to halt execution so the test doesn't auto-restart
  while (true) {
    delay(1000);
  }
}
