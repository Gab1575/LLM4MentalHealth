// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from flower_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice

#ifndef FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__STRUCT_H_
#define FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/RobotCommand in the package flower_msgs.
typedef struct flower_msgs__msg__RobotCommand
{
  uint8_t servo_angles[5];
  int16_t n20_pwm;
  float n20_target_rotations;
  uint32_t led_colours_hex[5];
  int16_t led_colours_brightness[5];
} flower_msgs__msg__RobotCommand;

// Struct for a sequence of flower_msgs__msg__RobotCommand.
typedef struct flower_msgs__msg__RobotCommand__Sequence
{
  flower_msgs__msg__RobotCommand * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} flower_msgs__msg__RobotCommand__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__STRUCT_H_
