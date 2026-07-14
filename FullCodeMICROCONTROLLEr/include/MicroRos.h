#ifndef MICROROS_H
#define MICROROS_H

#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <stdio.h>
#include <stdarg.h> // Added for variadic debug arguments

#include "MicroRosWifiCredentials.h" // Wifi credentials held in gitignore file
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <flower_msgs/msg/robot_command.h>
#include <std_msgs/msg/string.h>

struct Data {
  float servo_angles[5];
  int16_t n20_pwm;
  float n20_target_rotations;
  uint32_t led_colours_hex[5];
  int16_t led_colours_brightness[5];
}; 

extern Data flowerData; 

// --- ROS 2 Globals ---
extern rcl_publisher_t publisher;
extern rcl_subscription_t subscriber;
extern flower_msgs__msg__RobotCommand sub_msg;
extern rclc_executor_t executor;
extern rclc_support_t support;
extern rcl_allocator_t allocator;
extern rcl_node_t node;
extern rcl_timer_t timer;

// Debug globals
extern rcl_publisher_t debug_pub;
extern std_msgs__msg__String debug_msg; // Added the message struct
extern char debug_buffer[100];

// --- Function Prototypes ---
void error_loop();
bool MicroRosSetup();
void timer_callback(rcl_timer_t * timer, int64_t last_call_time); 
void subscription_callback(const void * msgin);                   
void MicroRosRun();
bool WiFiSetup();
void send_debug(const char *format, ...); // Added debug function prototype
void MicroRosDestroy();

#endif // MICROROS_H