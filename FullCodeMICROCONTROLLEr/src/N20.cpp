#include "N20.h"
#include <math.h> 
#include "MicroRos.h"

// Initialize tracking variables
volatile long n20EncoderTicks = 0;
float n20VelocityRadS = 0.0;

float currentPosition = 0.0;
float startingPosition = 0.0;
bool currentZeroState;

const float POSITION_TOLERANCE = 0.2; // The motor stops when it is within 0.2 rotations of the target

// Change to -1 to flip encoder readings if the model moves opposite to the physical direction
const int ENCODER_MULTIPLIER = -1; 

// --- Interrupt Service Routine (ISR) ---
void IRAM_ATTR n20EncoderISR() {
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
    
    pinMode(N20_ENC1_PIN, INPUT_PULLUP);
    pinMode(N20_ENC2_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(N20_ENC1_PIN), n20EncoderISR, RISING);
    
    // Initialize to stop
    n20MotorStop();

    startingPosition = flowerData.n20_target_rotations; // Set the starting position to the initial target rotations
    currentZeroState = flowerData.n20_zero; // Initialize the current state based on the received data
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

void n20MotorControl(float targetPosition, int16_t speed) {
    // Calculate how far away we are from the target
    
    if(flowerData.n20_zero != currentZeroState) {
        // If the zero state has changed, reset the encoder ticks and update the starting position
        noInterrupts();
        n20EncoderTicks = 0;
        interrupts();
        
        startingPosition = targetPosition; // Reset starting position to the new target
        currentZeroState = flowerData.n20_zero; // Update the current state
        send_debug("Zero state changed, resetting encoder ticks and starting position");
    }
    
    float error = targetPosition - currentPosition;

    static unsigned long last_print = 0;
    if (millis() - last_print > 300) {
     last_print = millis();
     send_debug("error value: %f",error);
    }
    
    // GRACE AREA CHECK: Use fabs() for absolute float value
    if(fabs(error) <= POSITION_TOLERANCE) {
        n20MotorStop();
        return;
    }
    
    // If error is positive, we need to move forward
    if(error > 0.0) {
        n20MotorSetDirection(N20_FORWARD);
        n20MotorSetSpeed(speed);
    } 
    // If error is negative, we need to move backward
    else {
        n20MotorSetDirection(N20_BACKWARD);
        n20MotorSetSpeed(speed);
    }
}

void n20MotorStop() {
    n20MotorSetDirection(N20_STOP);
    n20MotorSetSpeed(N20_MIN_SPEED);
}

void n20MotorPosition() {
    // Read the volatile variable safely by temporarily disabling interrupts
    noInterrupts();
    long currentTicks = n20EncoderTicks;
    interrupts();

    // Update position instantly every time this function is called
    currentPosition = startingPosition + (float)currentTicks / (float)N20_TICKS_PER_REV;

    static unsigned long last_print = 0;
    if (millis() - last_print > 300) {
        last_print = millis();
        
        send_debug("Pos: %f, Ticks: %ld", currentPosition, currentTicks);
        Serial.print("encoder ticks ");
        Serial.println(currentTicks);    
    }
}
