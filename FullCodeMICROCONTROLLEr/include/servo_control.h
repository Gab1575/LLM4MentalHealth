#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN       102
#define SERVOMAX       512
#define SERVO_CENTER   307
#define NUM_SERVOS     16

#define SDA_PIN 19
#define SCL_PIN 21

void servoControlBegin();
void servoControlSet(int servoIndex, float target_angle, float delta_T);
void servoControlUpdate();

#endif // SERVO_CONTROL_H
