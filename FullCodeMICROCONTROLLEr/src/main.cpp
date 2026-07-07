#include <Arduino.h>
#include "petal_lights.h"
#include "servo_control.h"
#include "N20.h"
#include "MicroRos.h"

void setup() {
  Serial.begin(115200);

  // Initialize all control systems
  petalLightsBegin();
  servoControlBegin();
  servoControlReset();
  n20MotorBegin();
  MicroRosSetup();
}

void loop() {
  // 1. Let micro-ROS process incoming network traffic
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));

  // 2. Print debug data only twice a second (every 500ms)
  static unsigned long last_print = 0;
  if (millis() - last_print > 500) {
    last_print = millis();

    // Format the output into a single, clean line
    Serial.printf("Servos: [%d, %d, %d, %d, %d] | ", 
      flowerData.servo_angles[0], flowerData.servo_angles[1], 
      flowerData.servo_angles[2], flowerData.servo_angles[3], 
      flowerData.servo_angles[4]);
      
    Serial.printf("N20: [PWM: %d, Target: %.2f] | ", 
      flowerData.n20_pwm, flowerData.n20_target_rotations);

    // Just printing the first LED as an example to keep the line readable
    Serial.printf("LED0: [HEX: %X, Brightness: %d]\n", 
      flowerData.led_colours_hex[0], flowerData.led_colours_brightness[0]);
  }
}
