#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// PCA9685 default I2C address is 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const uint8_t servoChannels[] = {0, 1, 2};
const uint16_t servoMin = 150; // Min pulse length out of 4096
const uint16_t servoMax = 600; // Max pulse length out of 4096

uint16_t angleToPulse(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, servoMin, servoMax);
}

void moveServo(uint8_t channel, int angle) {
  uint16_t pulse = angleToPulse(angle);
  pwm.setPWM(channel, 0, pulse);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  Serial.println("PCA9685 Servo Control Example");
  pwm.begin();
  pwm.setPWMFreq(50); // Standard servo frequency
  delay(10);

  for (uint8_t i = 0; i < sizeof(servoChannels); i++) {
    moveServo(servoChannels[i], 90);
  }
}

void loop() {
  moveServo(servoChannels[0], 150);
  moveServo(servoChannels[1], 150);
  moveServo(servoChannels[2], 150);
  Serial.println("Moved to 150 degrees");
  delay(2000);
  moveServo(servoChannels[0], 20);
  moveServo(servoChannels[1], 20);
  moveServo(servoChannels[2], 20);
  Serial.println("Moved to 20 degrees");
  delay(2000);
}
