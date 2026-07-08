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
void servoControlMove(int servoIndex, int delta_pos);   
void servoControlSet(int servoIndex, int target_pos);
void servoControlSweep(int servoIndex, int end_pos, int step_delay);

#endif // SERVO_CONTROL_H
