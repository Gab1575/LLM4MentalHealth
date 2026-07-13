#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN       80
#define SERVOMAX       510
#define SERVO_CENTER   295
#define NUM_SERVOS     16

#define SDA_PIN 19
#define SCL_PIN 21

void servoControlBegin();
void servoControlReset();
void servoControlMove(int servoIndex, float delta_pos);   
void servoControlSet(int servoIndex, float target_pos);

#endif // SERVO_CONTROL_H
