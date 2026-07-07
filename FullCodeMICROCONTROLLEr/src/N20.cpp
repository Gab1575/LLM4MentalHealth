#include "N20.h"

// Initialize tracking variables
volatile long n20EncoderTicks = 0;
float n20VelocityRadS = 0.0;

// Change to -1 to flip encoder readings if the model moves opposite to the physical direction
const int ENCODER_MULTIPLIER = -1; 

// --- Interrupt Service Routine (ISR) ---
// This runs automatically in the background every time ENC1 goes HIGH
void IRAM_ATTR n20EncoderISR() {
    // Read the other phase to determine direction
    if (digitalRead(N20_ENC2_PIN) > 0) {
        n20EncoderTicks += ENCODER_MULTIPLIER;
    } else {
        n20EncoderTicks -= ENCODER_MULTIPLIER;
    }
}

void n20MotorBegin() {
    pinMode(N20_PWM_PIN, OUTPUT);
    pinMode(N20_IN1_PIN, OUTPUT);
    pinMode(N20_IN2_PIN, OUTPUT);
    
    // ESP32 Pins 34 & 35 are INPUT ONLY and lack internal pull-ups.
    // Standard N20 encoder breakout boards usually have physical pull-up resistors on them.
    pinMode(N20_ENC1_PIN, INPUT);
    pinMode(N20_ENC2_PIN, INPUT);

    // Attach the hardware interrupt to ENC1
    attachInterrupt(digitalPinToInterrupt(N20_ENC1_PIN), n20EncoderISR, RISING);
    
    // Initialize to stop
    n20MotorStop();
}


void n20MotorSetSpeed(int speed) {
    speed = constrain(speed, N20_MIN_SPEED, N20_MAX_SPEED);
    analogWrite(N20_PWM_PIN, speed);
}

void n20MotorSetDirection(int direction) {
    switch(direction) {
        case N20_FORWARD:
            digitalWrite(N20_IN1_PIN, HIGH);
            digitalWrite(N20_IN2_PIN, LOW);
            break;
        case N20_BACKWARD:
            digitalWrite(N20_IN1_PIN, LOW);
            digitalWrite(N20_IN2_PIN, HIGH);
            break;
        case N20_STOP:
            digitalWrite(N20_IN1_PIN, LOW);
            digitalWrite(N20_IN2_PIN, LOW);
            break;
        default:
            Serial.println("Invalid direction");
            break;
    }
}

void n20MotorControl(int direction, int speed) {
    n20MotorSetDirection(direction);
    n20MotorSetSpeed(speed);
}

void n20MotorStop() {
    n20MotorSetDirection(N20_STOP);
    n20MotorSetSpeed(N20_MIN_SPEED);
}

// --- Velocity Calculation ---
// Call this repeatedly in your main loop() to keep the Rad/s updated
void n20EncoderUpdateVelocity() {
    static unsigned long lastTime = 0;
    static long lastTicks = 0;
    
    unsigned long currentTime = micros(); // Use microseconds for higher precision
    
    // Calculate velocity every 50 milliseconds (20Hz)
    if (currentTime - lastTime >= 50000) {
        
        // Briefly pause interrupts to safely copy the volatile tick count
        noInterrupts();
        long currentTicks = n20EncoderTicks;
        interrupts();

        // Calculate delta ticks and delta time (in seconds)
        long deltaTicks = currentTicks - lastTicks;
        float dt = (currentTime - lastTime) / 1000000.0; 

        // Calculate Radians per Second
        // Formula: (Delta Ticks / Ticks Per Rev) * (2 * PI) / dt
        float revolutions = (float)deltaTicks / N20_TICKS_PER_REV;
        n20VelocityRadS = (revolutions * 2.0 * PI) / dt;

        // Save current values for the next loop
        lastTicks = currentTicks;
        lastTime = currentTime;
    }
}