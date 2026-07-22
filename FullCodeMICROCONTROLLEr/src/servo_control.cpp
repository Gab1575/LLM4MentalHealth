#include "servo_control.h"
#include "MicroRos.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
ServoState servos[5];

void servoControlBegin() {
    Wire.begin(SDA_PIN, SCL_PIN);
    pwm.begin();
    pwm.setPWMFreq(50);
    pwm.setOscillatorFrequency(27000000);

    servos[0].hardwareIndex = 7;
    servos[1].hardwareIndex = 8;
    servos[2].hardwareIndex = 9;
    servos[3].hardwareIndex = 10;
    servos[4].hardwareIndex = 11;
    
    for (int i = 0; i < 5; i++) {
        servos[i].targetAngle = flowerData.servo_angles[i];
        servos[i].startPulse = map(flowerData.servo_angles[i], -90.0, 90.0, SERVOMIN, SERVOMAX);
        servos[i].targetPulse = servos[i].startPulse;
        servos[i].currentPulse = servos[i].startPulse; // Initialize current location
        pwm.setPWM(servos[i].hardwareIndex, 0, (uint16_t)servos[i].targetPulse);
    }
}

void servoControlSet(int servoIndex, float target_angle, float delta_T_seconds) {
    if (servoIndex < 0 || servoIndex > 4) return;

    // 1. Ignore redundant commands from the 30ms main loop
    if (abs(servos[servoIndex].targetAngle - target_angle) < 0.1 && 
        abs((servos[servoIndex].delta_T_ms / 1000.0) - delta_T_seconds) < 0.05) {
        return; 
    }

    float target_pulse = SERVOMIN + ((target_angle - (-90.0)) / (90.0 - (-90.0))) * (SERVOMAX - SERVOMIN);
    target_pulse = constrain(target_pulse, SERVOMIN, SERVOMAX);
    
    // 2. Start from EXACTLY where the servo physically is right now
    servos[servoIndex].startPulse = servos[servoIndex].currentPulse; 
    servos[servoIndex].targetPulse = target_pulse;
    servos[servoIndex].targetAngle = target_angle;
    
    // 3. Convert GUI seconds into Milliseconds!
    servos[servoIndex].delta_T_ms = delta_T_seconds * 1000.0; 
    servos[servoIndex].startTime = millis();
    servos[servoIndex].isMoving = true;
}

void servoControlUpdate() {
    unsigned long currentMillis = millis();

    for (int i = 0; i < 5; i++) {
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