#include <Arduino.h>

#define SERVO_PIN 13          // Change to your ESP32 GPIO pin
#define PWM_FREQ 50           // 50Hz frequency standard for RC servos
#define PWM_CHANNEL 0         // ESP32 LEDC Channel (0-15)
#define PWM_RES 16            // 16-bit resolution (0 to 65535)

// Servo Duty Cycle Calibration (16-bit equivalents for 0.5ms to 2.5ms pulses @ 50Hz)
#define MIN_DUTY 1638         // ~0.5ms pulse (0 degrees)
#define MAX_DUTY 8192         // ~2.5ms pulse (180 degrees)

// Global variable to remember where the servo is
double currentPosition = 0.0; 

void writeServoAngle(double angle) {
    //Constrain to 180-degree limit
    angle = constrain(angle, 0.0, 180.0);
    
    // 2. Map the 0-180 range to the MIN/MAX duty cycles
    uint32_t duty = MIN_DUTY + (uint32_t)((angle / 180.0) * (MAX_DUTY - MIN_DUTY));
    
    ledcWrite(PWM_CHANNEL, duty);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize ESP32 LEDC PWM (Core 2.x syntax)
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
    ledcAttachPin(SERVO_PIN, PWM_CHANNEL);
    
    // Set initial neutral position
    writeServoAngle(currentPosition);
}

void moveServoTo(double targetAngle, unsigned long durationMs) {
    // Start from wherever the servo currently is, not zero!
    double startAngle = currentPosition; 
    unsigned long startTime = millis();

    double deltaAngle = targetAngle - startAngle;
    double rate = deltaAngle / durationMs; // degrees per millisecond
    constrain(rate, -0.3, 0.3); // Limit the rate to avoid too fast movement
    
    while (millis() - startTime < durationMs) {
        unsigned long elapsed = millis() - startTime;
        double progress = (double)elapsed / durationMs;
        
        // S-curve easing function (using a simple cubic easing)
        double easedProgress = progress * progress * (3 - 2 * progress);
        
        double currentAngle = startAngle + (targetAngle - startAngle) * easedProgress;
        writeServoAngle(currentAngle);
        
        delay(10); // Small delay to avoid overwhelming the loop
    }
    
    // Ensure final position is set precisely
    writeServoAngle(targetAngle);
    
    // Update the global state so the next move starts from this new angle
    currentPosition = targetAngle; 
}

void loop(){
    moveServoTo(180.0, 600); // Move to 180 degrees smoothly over 2 seconds
    delay(1000);              // Wait for 1 second
    moveServoTo(0.0, 600);   // Move back to 0 degrees smoothly over 2 seconds
    delay(1000);              // Wait for 1 second
}