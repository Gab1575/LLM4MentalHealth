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

  //-- microROS message saying everything is ready --
}

void loop() {
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
  Serial.println(flowerData.servo_angles[0]);
  Serial.println(flowerData.servo_angles[1]);
  Serial.println(flowerData.servo_angles[2]);
  Serial.println(flowerData.servo_angles[3]);
  Serial.println(flowerData.servo_angles[4]);
  Serial.println(flowerData.n20_pwm);
  Serial.println(flowerData.n20_target_rotations);
  Serial.println(flowerData.led_colours_hex[0]);
  Serial.println(flowerData.led_colours_brightness[0]);
  Serial.println(flowerData.led_colours_hex[1]);
  Serial.println(flowerData.led_colours_brightness[1]);
  Serial.println(flowerData.led_colours_hex[2]);
  Serial.println(flowerData.led_colours_brightness[2]);
  Serial.println(flowerData.led_colours_hex[3]);
  Serial.println(flowerData.led_colours_brightness[3]);   
}
