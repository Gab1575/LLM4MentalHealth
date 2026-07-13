#include "servo_control.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
int servoPositions[NUM_SERVOS];

void servoControlBegin() {
    Wire.begin(SDA_PIN, SCL_PIN); // Initialize I2C communication
    pwm.begin();
    pwm.setPWMFreq(50);  // Set frequency to 50 Hz for servos
      pwm.setOscillatorFrequency(27000000);
}

void servoControlReset() {
    for (int i = 0; i < NUM_SERVOS; i++) {
        servoPositions[i] = SERVO_CENTER; // Initialize all servos to center position
        pwm.setPWM(i, 0, servoPositions[i]);
    }
}

void servoControlMove(int servoIndex, float delta_pos) {
    if (servoIndex < 0 || servoIndex >= NUM_SERVOS) {
        Serial.println("Invalid servo index");
        return;
    }
    
    // Update the servo position based on the delta_theta
    float newPosition = servoPositions[servoIndex] + delta_pos;
    
    // Constrain the new position to be within the defined limits
    newPosition = constrain(newPosition, SERVOMIN, SERVOMAX);
    
    // Move the servo to the new position
    pwm.setPWM(servoIndex, 0, newPosition);
    
    // Update the stored position for the servo
    servoPositions[servoIndex] = newPosition;
}

void servoControlSet(int servoIndex, float target_pos) {
    
    switch(servoIndex) {
        case 0:
            servoIndex = 7;
            break;
        case 1:
            servoIndex = 8;
            break;
        case 2:
            servoIndex = 9;
            break;
        case 3:
            servoIndex = 10;
            break;
        case 4:
            servoIndex = 11;
            break;
    }

    if (servoIndex < 0 || servoIndex >= NUM_SERVOS) {
        Serial.println("Invalid servo index");
        return;
    }
    
    // Constrain the target position to be within the defined limits
    target_pos = SERVO_CENTER + target_pos; // Adjust target position relative to center
    target_pos = constrain(target_pos, SERVOMIN, SERVOMAX);
    
    // Move the servo to the target position
    pwm.setPWM(servoIndex, 0, (uint16_t)target_pos);
    
    // Update the stored position for the servo
    servoPositions[servoIndex] = target_pos;
}