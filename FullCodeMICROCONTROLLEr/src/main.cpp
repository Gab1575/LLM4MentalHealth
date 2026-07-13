#include <Arduino.h>
#include "petal_lights.h"
#include "servo_control.h"
#include "N20.h"
#include "MicroRos.h"


void setup() {
  Serial.begin(115200);
  
  MicroRosSetup();
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
  Serial.println("Micro-ROS setup complete");
  send_debug("Flower Connected to Host");
  delay(1000);

  // 2. Initialize LEDs (Starts them completely off)
  petalLightsBegin();
  send_debug("Lights initilized");
  delay(250); 
  
  // 3. Initialize I2C and PCA9685 for Servos
  servoControlBegin();
  send_debug("Servo's initilized");
  delay(250);
  
  // 4. Initialize the N20 Motor (Ensures it is stopped)
  n20MotorBegin();
  send_debug("N20 motor initilized");
  Serial.println("N20 motor initilized");
  delay(250);

  // send_debug("All system setup sucsess");
  Serial.println("All system setup sucsess");
}

void loop() {
  // 1. Let micro-ROS process incoming network traffic
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));

  // 2. Throttle LED and Servo updates to ~33 Hz (every 30ms)
  static unsigned long last_hardware_update = 0;
  if (millis() - last_hardware_update > 30) {
    last_hardware_update = millis();

    for (int i = 0; i < 5; i++) {
      petalLightsUpdate(i, flowerData.led_colours_hex[i], flowerData.led_colours_brightness[i]);
      servoControlSet(i, flowerData.servo_angles[i]);
    }
    // // Interrupts are only disabled during this brief call now!
    petalLightsShow(); 
  }

  // 3. Motor control must run constantly to be smooth
  n20MotorPosition();
  n20MotorControl(flowerData.n20_target_rotations, flowerData.n20_pwm);

  //4. Debug printing
  static unsigned long last_print = 0;
  if (millis() - last_print > 100) {
    last_print = millis();
    // Your debug code here
  }
}