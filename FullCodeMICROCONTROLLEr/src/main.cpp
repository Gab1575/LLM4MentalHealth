#include <Arduino.h>
#include "petal_lights.h"
#include "servo_control.h"
#include "N20.h"
#include "MicroRos.h"


void setup() {
  Serial.begin(115200);
  
  // Keep trying to connect until successful before moving on
  while (!MicroRosSetup()) {
    Serial.println("Initial Micro-ROS setup failed, retrying in 2 seconds...");
    delay(2000);
  }

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
  
  servoControlUpdate();

  // 2. Throttle LED and Servo updates to ~33 Hz (every 30ms)
  static unsigned long last_hardware_update = 0;
  if (millis() - last_hardware_update > 30) {
    last_hardware_update = millis();

    for (int i = 0; i < 5; i++) {
      petalLightsUpdate(i, flowerData.led_colours_hex[i], flowerData.led_colours_brightness[i]);
      servoControlSet(i, flowerData.servo_angles[i], flowerData.servo_time[i]);
    }
    petalLightsShow(); 
  }

  //3. N20 Motor Control
  n20MotorPosition();
  n20MotorControl(flowerData.n20_target_rotations, flowerData.n20_pwm);

// 4. Check Connection
  static unsigned long last_connected = 0;
  if (millis() - last_connected > 2000) {
    last_connected = millis();
    
    // If the ping FAILS
    if (rmw_uros_ping_agent(50, 1) != RMW_RET_OK) {
      Serial.println("Connection lost. Destroying entities...");
      MicroRosDestroy(); // Safe memory cleanup
      
      Serial.println("Attempting to reconnect...");
      if (!MicroRosSetup()) {
        // If the reconnect fails halfway through, we must clean up 
        // whatever partial memory was allocated before we try again next loop.
        MicroRosDestroy(); 
        Serial.println("Reconnect failed. Will try again in 2 seconds.");
      } else {
        Serial.println("Reconnected successfully!");
      }
    }
  }
}