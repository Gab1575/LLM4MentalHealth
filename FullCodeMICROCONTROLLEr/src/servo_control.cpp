// servo_control.cpp - Drives the four servos that bend the continuum
// stem. 

#include "servo_control.h"
#include "MicroRos.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
ServoState servos[4]; 

// How long the intial move to current target angle takes (milliseconds)
#define INIT_EASE_MS 4000.0

// Initializes I2C and the PCA9685.
void servoControlBegin() {
    Wire.begin(SDA_PIN, SCL_PIN);
    pwm.begin();
    pwm.setPWMFreq(50);
    pwm.setOscillatorFrequency(27000000);

    servos[0].hardwareIndex = 0;
    servos[1].hardwareIndex = 1;
    servos[2].hardwareIndex = 2;
    servos[3].hardwareIndex = 3;

    unsigned long startTime = millis();

    for (int i = 0; i < 4; i++) {
        servos[i].targetAngle = flowerData.servo_angles[i];
        servos[i].targetPulse = map(flowerData.servo_angles[i], -90.0, 90.0, SERVOMIN, SERVOMAX);
        servos[i].currentPulse = servos[i].targetPulse; 
        servos[i].startPulse = servos[i].currentPulse;
        servos[i].delta_T_ms = INIT_EASE_MS;
        servos[i].startTime = startTime;
        servos[i].isMoving = false; // Stay put until a real command arrives
    }
}

// Starts a new move for one servo toward target_angle, to complete in
// delta_T_seconds.
void servoControlSet(int servoIndex, float target_angle, float delta_T_seconds) {
    if (servoIndex < 0 || servoIndex > 3) return;

    // Ignore redundant commands from the 30ms main loop
    if (abs(servos[servoIndex].targetAngle - target_angle) < 0.1 &&
        abs((servos[servoIndex].delta_T_ms / 1000.0) - delta_T_seconds) < 0.05) {
        return;
    }

    float target_pulse = SERVOMIN + ((target_angle - (-90.0)) / (90.0 - (-90.0))) * (SERVOMAX - SERVOMIN);
    target_pulse = constrain(target_pulse, SERVOMIN, SERVOMAX);

    // Start from where the servo physically is right now
    servos[servoIndex].startPulse = servos[servoIndex].currentPulse;
    servos[servoIndex].targetPulse = target_pulse;
    servos[servoIndex].targetAngle = target_angle;

    // Convert GUI seconds into Milliseconds
    servos[servoIndex].delta_T_ms = delta_T_seconds * 1000.0;
    servos[servoIndex].startTime = millis();
    servos[servoIndex].isMoving = true;
}

// Advances every servo currently mid-move by one tick: computes how far
// through its S-curve it should be given elapsed time, writes the
// interpolated pulse to the PCA9685, and marks the move finished once the
// commanded duration has elapsed. Intended to be called every loop().
void servoControlUpdate() {
    unsigned long currentMillis = millis();

    for (int i = 0; i < 4; i++) {
        if (!servos[i].isMoving) continue;

        unsigned long elapsed = currentMillis - servos[i].startTime;
        
        // Prevent Divide by Zero if GUI sends 0 seconds
        if (servos[i].delta_T_ms <= 0 || elapsed >= servos[i].delta_T_ms) {
            servos[i].currentPulse = servos[i].targetPulse; // Update location tracker
            pwm.setPWM(servos[i].hardwareIndex, 0, (uint16_t)servos[i].targetPulse);
            servos[i].isMoving = false;
        } else {
            // Your S-Curve logic 
            double progress = (double)elapsed / servos[i].delta_T_ms;
            double easedProgress = progress * progress * (3 - 2 * progress);
            
            // Keep track of exactly where we are
            servos[i].currentPulse = servos[i].startPulse + (servos[i].targetPulse - servos[i].startPulse) * easedProgress;
            pwm.setPWM(servos[i].hardwareIndex, 0, (uint16_t)servos[i].currentPulse);
        }
    }
}