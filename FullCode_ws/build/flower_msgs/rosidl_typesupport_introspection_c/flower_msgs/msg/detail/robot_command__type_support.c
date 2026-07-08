// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from flower_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "flower_msgs/msg/detail/robot_command__rosidl_typesupport_introspection_c.h"
#include "flower_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "flower_msgs/msg/detail/robot_command__functions.h"
#include "flower_msgs/msg/detail/robot_command__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  flower_msgs__msg__RobotCommand__init(message_memory);
}

void flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_fini_function(void * message_memory)
{
  flower_msgs__msg__RobotCommand__fini(message_memory);
}

size_t flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__size_function__RobotCommand__servo_angles(
  const void * untyped_member)
{
  (void)untyped_member;
  return 5;
}

const void * flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_const_function__RobotCommand__servo_angles(
  const void * untyped_member, size_t index)
{
  const int8_t * member =
    (const int8_t *)(untyped_member);
  return &member[index];
}

void * flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_function__RobotCommand__servo_angles(
  void * untyped_member, size_t index)
{
  int8_t * member =
    (int8_t *)(untyped_member);
  return &member[index];
}

void flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__fetch_function__RobotCommand__servo_angles(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int8_t * item =
    ((const int8_t *)
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_const_function__RobotCommand__servo_angles(untyped_member, index));
  int8_t * value =
    (int8_t *)(untyped_value);
  *value = *item;
}

void flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__assign_function__RobotCommand__servo_angles(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int8_t * item =
    ((int8_t *)
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_function__RobotCommand__servo_angles(untyped_member, index));
  const int8_t * value =
    (const int8_t *)(untyped_value);
  *item = *value;
}

size_t flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__size_function__RobotCommand__led_colours_hex(
  const void * untyped_member)
{
  (void)untyped_member;
  return 5;
}

const void * flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_const_function__RobotCommand__led_colours_hex(
  const void * untyped_member, size_t index)
{
  const uint32_t * member =
    (const uint32_t *)(untyped_member);
  return &member[index];
}

void * flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_function__RobotCommand__led_colours_hex(
  void * untyped_member, size_t index)
{
  uint32_t * member =
    (uint32_t *)(untyped_member);
  return &member[index];
}

void flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__fetch_function__RobotCommand__led_colours_hex(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint32_t * item =
    ((const uint32_t *)
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_const_function__RobotCommand__led_colours_hex(untyped_member, index));
  uint32_t * value =
    (uint32_t *)(untyped_value);
  *value = *item;
}

void flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__assign_function__RobotCommand__led_colours_hex(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint32_t * item =
    ((uint32_t *)
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_function__RobotCommand__led_colours_hex(untyped_member, index));
  const uint32_t * value =
    (const uint32_t *)(untyped_value);
  *item = *value;
}

size_t flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__size_function__RobotCommand__led_colours_brightness(
  const void * untyped_member)
{
  (void)untyped_member;
  return 5;
}

const void * flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_const_function__RobotCommand__led_colours_brightness(
  const void * untyped_member, size_t index)
{
  const int16_t * member =
    (const int16_t *)(untyped_member);
  return &member[index];
}

void * flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_function__RobotCommand__led_colours_brightness(
  void * untyped_member, size_t index)
{
  int16_t * member =
    (int16_t *)(untyped_member);
  return &member[index];
}

void flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__fetch_function__RobotCommand__led_colours_brightness(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int16_t * item =
    ((const int16_t *)
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_const_function__RobotCommand__led_colours_brightness(untyped_member, index));
  int16_t * value =
    (int16_t *)(untyped_value);
  *value = *item;
}

void flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__assign_function__RobotCommand__led_colours_brightness(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int16_t * item =
    ((int16_t *)
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_function__RobotCommand__led_colours_brightness(untyped_member, index));
  const int16_t * value =
    (const int16_t *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_message_member_array[5] = {
  {
    "servo_angles",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    5,  // array size
    false,  // is upper bound
    offsetof(flower_msgs__msg__RobotCommand, servo_angles),  // bytes offset in struct
    NULL,  // default value
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__size_function__RobotCommand__servo_angles,  // size() function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_const_function__RobotCommand__servo_angles,  // get_const(index) function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_function__RobotCommand__servo_angles,  // get(index) function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__fetch_function__RobotCommand__servo_angles,  // fetch(index, &value) function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__assign_function__RobotCommand__servo_angles,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "n20_pwm",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(flower_msgs__msg__RobotCommand, n20_pwm),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "n20_target_rotations",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(flower_msgs__msg__RobotCommand, n20_target_rotations),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "led_colours_hex",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    5,  // array size
    false,  // is upper bound
    offsetof(flower_msgs__msg__RobotCommand, led_colours_hex),  // bytes offset in struct
    NULL,  // default value
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__size_function__RobotCommand__led_colours_hex,  // size() function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_const_function__RobotCommand__led_colours_hex,  // get_const(index) function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_function__RobotCommand__led_colours_hex,  // get(index) function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__fetch_function__RobotCommand__led_colours_hex,  // fetch(index, &value) function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__assign_function__RobotCommand__led_colours_hex,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "led_colours_brightness",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    5,  // array size
    false,  // is upper bound
    offsetof(flower_msgs__msg__RobotCommand, led_colours_brightness),  // bytes offset in struct
    NULL,  // default value
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__size_function__RobotCommand__led_colours_brightness,  // size() function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_const_function__RobotCommand__led_colours_brightness,  // get_const(index) function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__get_function__RobotCommand__led_colours_brightness,  // get(index) function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__fetch_function__RobotCommand__led_colours_brightness,  // fetch(index, &value) function pointer
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__assign_function__RobotCommand__led_colours_brightness,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_message_members = {
  "flower_msgs__msg",  // message namespace
  "RobotCommand",  // message name
  5,  // number of fields
  sizeof(flower_msgs__msg__RobotCommand),
  flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_message_member_array,  // message members
  flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_init_function,  // function to initialize message memory (memory has to be allocated)
  flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_message_type_support_handle = {
  0,
  &flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_flower_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, flower_msgs, msg, RobotCommand)() {
  if (!flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_message_type_support_handle.typesupport_identifier) {
    flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &flower_msgs__msg__RobotCommand__rosidl_typesupport_introspection_c__RobotCommand_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
