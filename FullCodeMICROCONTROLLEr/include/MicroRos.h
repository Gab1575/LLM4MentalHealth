// MicroRos.h - Declarations for the embedded ROS bridge (see MicroRos.cpp).
//
// Defines the shared `Data` struct that every subsystem reads its target
// state from, plus the micro-ROS entity handles and setup/teardown/debug
// function prototypes used to connect the ESP32 to the host's ROS 2 graph.

#ifndef MICROROS_H
#define MICROROS_H

#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <stdio.h>
#include <stdarg.h> // Added for variadic debug arguments

#include "MicroRosWifiCredentials.h" // Wifi credentials held in gitignore file
#include "petal_lights.h" // LED petals double as a connection-status indicator while connecting
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <flower_msgs/msg/robot_command.h>
#include <std_msgs/msg/string.h>

// Shared command state, populated by subscription_callback() from the latest
// /flower_commands message and read every loop iteration by main.cpp to
// drive the servos, LEDs, and N20 motor.
struct Data {
  float servo_angles[4];          // Target angle per servo, degrees (-90 to 90)
  float servo_time[4];            // Commanded move duration per servo, seconds
  int16_t n20_pwm;                // N20 motor drive speed, 0-255
  float n20_target_rotations;     // N20 target position, rotations
  uint32_t led_colours_hex[5];    // Per-petal LED color, 0xRRGGBB
  int16_t led_colours_brightness[5]; // Per-petal LED brightness, 0-255
  bool n20_zero;                  // Toggling this re-zeroes the N20 position reference
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
extern char debug_buffer[200]; // Sized to fit a full relayed RobotCommand message

// --- Function Prototypes ---
void error_loop();                                                 // Halts on unrecoverable ROS setup failure
bool MicroRosSetup();                                              // Connects WiFi + initializes all ROS 2 entities
void timer_callback(rcl_timer_t * timer, int64_t last_call_time);
void subscription_callback(const void * msgin);                    // Applies an incoming RobotCommand to flowerData
void MicroRosRun();
bool WiFiSetup();                                                  // Connects to WiFi and resolves the agent via mDNS
void send_debug(const char *format, ...);                          // printf-style publish to /flower_debug
void MicroRosDestroy();                                            // Tears down ROS entities before a reconnect attempt

#endif // MICROROS_H