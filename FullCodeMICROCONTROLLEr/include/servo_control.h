// servo_control.h - Declarations for the stem's PCA9685-driven servo subsystem
// (see servo_control.cpp). Servos move on a smoothed, time-based S-curve
// rather than jumping straight to a new angle.

#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN       102   // PCA9685 pulse count at -90 degrees
#define SERVOMAX       512   // PCA9685 pulse count at +90 degrees
#define SERVO_CENTER   307   // PCA9685 pulse count at 0 degrees
#define NUM_SERVOS     16    // PCA9685 channel count (only 4 are wired/used)

#define SDA_PIN 19
#define SCL_PIN 21

// Per-servo motion state used to interpolate from startPulse to targetPulse
// over delta_T_ms, driven by servoControlUpdate() every loop iteration.
struct ServoState {
    bool isMoving = false;
    unsigned long startTime;   // millis() timestamp the current move began
    float delta_T_ms;          // Commanded move duration, milliseconds
    float startPulse;          // PCA9685 pulse count at move start
    float targetPulse;         // PCA9685 pulse count at move end
    float currentPulse;        // Last pulse count written to the driver
    float targetAngle;         // Commanded angle, degrees (for de-duplicating repeat commands)
    int hardwareIndex;         // PCA9685 channel this servo is wired to
};

void servoControlBegin();                                              // Initializes I2C/PCA9685 and centers all servos
void servoControlSet(int servoIndex, float target_angle, float delta_T); // Starts a new eased move toward target_angle
void servoControlUpdate();                                              // Advances any in-progress moves; call every loop

#endif // SERVO_CONTROL_H
