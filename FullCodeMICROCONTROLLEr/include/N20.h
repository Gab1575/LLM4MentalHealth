#ifndef N20_H
#define N20_H

#include <Arduino.h>
#include "MicroRos.h"

// N20 Motor pin definitions
#define N20_PWM_PIN 12     // PWM pin for speed control
#define N20_IN1_PIN 13     // Direction control pin 1
#define N20_IN2_PIN 18     // Direction control pin 2
#define N20_ENC1_PIN 34      // Encoder pin 1 (Interrupt)
#define N20_ENC2_PIN 35      // Encoder pin 2 (Direction)

// Motor speed constants
#define N20_MIN_SPEED 0
#define N20_MAX_SPEED 255

// Motor direction constants
#define N20_FORWARD 1
#define N20_BACKWARD 2
#define N20_STOP 0

// Encoder Constants
// 7 PPR motor shaft * 100 gearbox ratio = 700 ticks per output shaft rev
#define N20_TICKS_PER_REV 700

// --- Global Variables for Tracking ---
extern volatile long n20EncoderTicks;
extern float n20VelocityRadS;

// --- Function Prototypes ---
void n20MotorBegin();
void n20MotorSetSpeed(int speed);
void n20MotorSetDirection(int direction);
void n20MotorControl(float target_position, int16_t speed);
void n20MotorStop();

// Encoder Function
void n20MotorPosition(); 

#endif // N20_H