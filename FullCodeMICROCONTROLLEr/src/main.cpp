#include <Arduino.h>
#include "petal_lights.h"
#include "servo_control.h"
#include "N20.h"
#include "MicroRos.h"


void setup() {
  Serial.begin(115200);
  
  // 1. Initialize Wi-Fi first, as it has a heavy power spike
  WiFiSetup();
  delay(500); // Let power stabilize
  
  // 2. Initialize LEDs (Starts them completely off)
  petalLightsBegin();
  delay(250); 
  
  // 3. Initialize I2C and PCA9685 for Servos
  servoControlBegin();
  delay(250);
  
  // 4. Initialize the N20 Motor (Ensures it is stopped)
  n20MotorBegin();
  delay(250);
  
  // 5. Finally, initialize micro-ROS now that hardware is stable
  MicroRosSetup();
}

void loop() {
  // 1. Let micro-ROS process incoming network traffic
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));

  for (int i = 0; i < 5; i++) {
    petalLightsUpdate(i, flowerData.led_colours_hex[i], flowerData.led_colours_brightness[i]);
    servoControlSet(i, flowerData.servo_angles[i]);
  }
  petalLightsShow();
  n20MotorPosition();
  n20MotorControl(flowerData.n20_target_rotations, flowerData.n20_pwm);

  static unsigned long last_print = 0;
  if (millis() - last_print > 100) {
    last_print = millis();

    // // Format the output into a single, clean line
    // Serial.printf("Servos: [%d, %d, %d, %d, %d] | ", 
    //   flowerData.servo_angles[0], flowerData.servo_angles[1], 
    //   flowerData.servo_angles[2], flowerData.servo_angles[3], 
    //   flowerData.servo_angles[4]);
    // send_debug("Servos: [%d, %d, %d, %d, %d] | N20: [PWM: %d, Target: %.2f] | LED0: [HEX: %X, Brightness: %d]", 
    //   flowerData.servo_angles[0], flowerData.servo_angles[1], 
    //   flowerData.servo_angles[2], flowerData.servo_angles[3], 
    //   flowerData.servo_angles[4],
    //   flowerData.n20_pwm, flowerData.n20_target_rotations,
    //   flowerData.led_colours_hex[0], flowerData.led_colours_brightness[0]);
      
    // Serial.printf("N20: [PWM: %d, Target: %.2f] | ", 
    //   flowerData.n20_pwm, flowerData.n20_target_rotations);
    // send_debug("N20: [PWM: %d, Target: %.2f]", 
    //   flowerData.n20_pwm, flowerData.n20_target_rotations);

    // Serial.printf("LEDs: [HEX: %X, %X, %X, %X, %X] Brightness: %d, %d, %d, %d, %d]\n", 
    //   flowerData.led_colours_hex[0], flowerData.led_colours_hex[1], 
    //   flowerData.led_colours_hex[2], flowerData.led_colours_hex[3], 
    //   flowerData.led_colours_hex[4], flowerData.led_colours_brightness[0], flowerData.led_colours_brightness[1], flowerData.led_colours_brightness[2], flowerData.led_colours_brightness[3], flowerData.led_colours_brightness[4] );
    
    // send_debug("LEDs: [HEX: %X, %X, %X, %X, %X] Brightness: [%d, %d, %d, %d, %d]", 
    //   flowerData.led_colours_hex[0], flowerData.led_colours_hex[1], 
    //   flowerData.led_colours_hex[2], flowerData.led_colours_hex[3], 
    //   flowerData.led_colours_hex[4], flowerData.led_colours_brightness[0], flowerData.led_colours_brightness[1], flowerData.led_colours_brightness[2], flowerData.led_colours_brightness[3], flowerData.led_colours_brightness[4] );
  
    }
}
