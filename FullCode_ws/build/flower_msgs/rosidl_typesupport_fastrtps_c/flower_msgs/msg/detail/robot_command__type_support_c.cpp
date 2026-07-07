// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from flower_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice
#include "flower_msgs/msg/detail/robot_command__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "flower_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "flower_msgs/msg/detail/robot_command__struct.h"
#include "flower_msgs/msg/detail/robot_command__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif


// forward declare type support functions


using _RobotCommand__ros_msg_type = flower_msgs__msg__RobotCommand;

static bool _RobotCommand__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _RobotCommand__ros_msg_type * ros_message = static_cast<const _RobotCommand__ros_msg_type *>(untyped_ros_message);
  // Field name: servo_angles
  {
    size_t size = 5;
    auto array_ptr = ros_message->servo_angles;
    cdr.serializeArray(array_ptr, size);
  }

  // Field name: n20_pwm
  {
    cdr << ros_message->n20_pwm;
  }

  // Field name: n20_target_rotations
  {
    cdr << ros_message->n20_target_rotations;
  }

  // Field name: led_colours_hex
  {
    size_t size = 5;
    auto array_ptr = ros_message->led_colours_hex;
    cdr.serializeArray(array_ptr, size);
  }

  // Field name: led_colours_brightness
  {
    size_t size = 5;
    auto array_ptr = ros_message->led_colours_brightness;
    cdr.serializeArray(array_ptr, size);
  }

  return true;
}

static bool _RobotCommand__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _RobotCommand__ros_msg_type * ros_message = static_cast<_RobotCommand__ros_msg_type *>(untyped_ros_message);
  // Field name: servo_angles
  {
    size_t size = 5;
    auto array_ptr = ros_message->servo_angles;
    cdr.deserializeArray(array_ptr, size);
  }

  // Field name: n20_pwm
  {
    cdr >> ros_message->n20_pwm;
  }

  // Field name: n20_target_rotations
  {
    cdr >> ros_message->n20_target_rotations;
  }

  // Field name: led_colours_hex
  {
    size_t size = 5;
    auto array_ptr = ros_message->led_colours_hex;
    cdr.deserializeArray(array_ptr, size);
  }

  // Field name: led_colours_brightness
  {
    size_t size = 5;
    auto array_ptr = ros_message->led_colours_brightness;
    cdr.deserializeArray(array_ptr, size);
  }

  return true;
}  // NOLINT(readability/fn_size)

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_flower_msgs
size_t get_serialized_size_flower_msgs__msg__RobotCommand(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _RobotCommand__ros_msg_type * ros_message = static_cast<const _RobotCommand__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name servo_angles
  {
    size_t array_size = 5;
    auto array_ptr = ros_message->servo_angles;
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name n20_pwm
  {
    size_t item_size = sizeof(ros_message->n20_pwm);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name n20_target_rotations
  {
    size_t item_size = sizeof(ros_message->n20_target_rotations);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name led_colours_hex
  {
    size_t array_size = 5;
    auto array_ptr = ros_message->led_colours_hex;
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name led_colours_brightness
  {
    size_t array_size = 5;
    auto array_ptr = ros_message->led_colours_brightness;
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

static uint32_t _RobotCommand__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_flower_msgs__msg__RobotCommand(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_flower_msgs
size_t max_serialized_size_flower_msgs__msg__RobotCommand(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

  // member: servo_angles
  {
    size_t array_size = 5;

    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }
  // member: n20_pwm
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint16_t);
    current_alignment += array_size * sizeof(uint16_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint16_t));
  }
  // member: n20_target_rotations
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }
  // member: led_colours_hex
  {
    size_t array_size = 5;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }
  // member: led_colours_brightness
  {
    size_t array_size = 5;

    last_member_size = array_size * sizeof(uint16_t);
    current_alignment += array_size * sizeof(uint16_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint16_t));
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = flower_msgs__msg__RobotCommand;
    is_plain =
      (
      offsetof(DataType, led_colours_brightness) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static size_t _RobotCommand__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_flower_msgs__msg__RobotCommand(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_RobotCommand = {
  "flower_msgs::msg",
  "RobotCommand",
  _RobotCommand__cdr_serialize,
  _RobotCommand__cdr_deserialize,
  _RobotCommand__get_serialized_size,
  _RobotCommand__max_serialized_size
};

static rosidl_message_type_support_t _RobotCommand__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_RobotCommand,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, flower_msgs, msg, RobotCommand)() {
  return &_RobotCommand__type_support;
}

#if defined(__cplusplus)
}
#endif
