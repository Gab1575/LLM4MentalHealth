// N20.h - Declarations for the N20 geared motor / quadrature encoder
// position-control subsystem (see N20.cpp).

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
void n20MotorBegin();                                          // Configures pins/ISR and anchors the position reference
void n20MotorSetSpeed(int speed);                               // Sets PWM drive speed (0-255)
void n20MotorSetDirection(int direction);                       // Sets H-bridge direction (forward/backward/stop)
void n20MotorControl(float target_position, int16_t speed);     // Bang-bang position control toward target_position
void n20MotorStop();                                            // Stops the motor (direction + speed to zero)

// Encoder Function
void n20MotorPosition(); // Recomputes currentPosition from encoder ticks

#endif // N20_H