#include "servo_control.h"
#include "MicroRos.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Create a structure to hold the trajectory state for each servo
struct ServoState {
    bool isMoving = false;
    unsigned long startTime;
    float delta_T;
    float startPulse;
    float targetPulse;
    int hardwareIndex;
};

// Array to track all 5 servos independently
ServoState servos[5];

void servoControlBegin() {
    Wire.begin(SDA_PIN, SCL_PIN);
    pwm.begin();
    pwm.setPWMFreq(50);
    pwm.setOscillatorFrequency(27000000);

    // Map your software indices (0-4) to hardware pins (7-11)
    servos[0].hardwareIndex = 7;
    servos[1].hardwareIndex = 8;
    servos[2].hardwareIndex = 9;
    servos[3].hardwareIndex = 10;
    servos[4].hardwareIndex = 11;
    
    // Initialize all to the last known position
    for (int i = 0; i < 5; i++) {
        servos[i].startPulse = map(flowerData.servo_angles[i], -90.0, 90.0, SERVOMIN, SERVOMAX);
        servos[i].targetPulse = servos[i].startPulse;
        pwm.setPWM(servos[i].hardwareIndex, 0, (uint16_t)servos[i].targetPulse);
    }
}

// This ONLY sets the targets, it does not wait for movement
void servoControlSet(int servoIndex, float target_angle, float delta_T) {
    if (servoIndex < 0 || servoIndex > 4) return;

    // Standard linear map formula for floats
    float target_pulse = SERVOMIN + ((target_angle - (-90.0)) / (90.0 - (-90.0))) * (SERVOMAX - SERVOMIN);
    target_pulse = constrain(target_pulse, SERVOMIN, SERVOMAX);
    
    // Record the trajectory data for this specific servo
    servos[servoIndex].startPulse = servos[servoIndex].targetPulse; // Start from current expected pos
    servos[servoIndex].targetPulse = target_pulse;
    servos[servoIndex].delta_T = delta_T;
    servos[servoIndex].startTime = millis();
    servos[servoIndex].isMoving = true;
}

void servoControlUpdate() {
    unsigned long currentMillis = millis();

    // Iterate through all 5 servos and update them simultaneously
    for (int i = 0; i < 5; i++) {
        if (!servos[i].isMoving) continue; // Skip servos that are stationary

        unsigned long elapsed = currentMillis - servos[i].startTime;
        
        if (elapsed >= servos[i].delta_T) {
            // Movement is finished, snap to exact target and stop
            pwm.setPWM(servos[i].hardwareIndex, 0, (uint16_t)servos[i].targetPulse);
            servos[i].isMoving = false;
        } else {
            // Calculate progress and ease it
            double progress = (double)elapsed / servos[i].delta_T;
            double easedProgress = progress * progress * (3 - 2 * progress);
            
            // Calculate current pulse and send it to the driver
            double currentPulse = servos[i].startPulse + (servos[i].targetPulse - servos[i].startPulse) * easedProgress;
            pwm.setPWM(servos[i].hardwareIndex, 0, (uint16_t)currentPulse);
        }
    }
}