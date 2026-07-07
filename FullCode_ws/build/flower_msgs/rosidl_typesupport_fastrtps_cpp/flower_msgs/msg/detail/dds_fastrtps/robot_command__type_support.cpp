// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
// with input from flower_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice
#include "flower_msgs/msg/detail/robot_command__rosidl_typesupport_fastrtps_cpp.hpp"
#include "flower_msgs/msg/detail/robot_command__struct.hpp"

#include <limits>
#include <stdexcept>
#include <string>
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_fastrtps_cpp/identifier.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp"
#include "fastcdr/Cdr.h"


// forward declaration of message dependencies and their conversion functions

namespace flower_msgs
{

namespace msg
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_flower_msgs
cdr_serialize(
  const flower_msgs::msg::RobotCommand & ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Member: servo_angles
  {
    cdr << ros_message.servo_angles;
  }
  // Member: n20_pwm
  cdr << ros_message.n20_pwm;
  // Member: n20_target_rotations
  cdr << ros_message.n20_target_rotations;
  // Member: led_colours_hex
  {
    cdr << ros_message.led_colours_hex;
  }
  // Member: led_colours_brightness
  {
    cdr << ros_message.led_colours_brightness;
  }
  return true;
}

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_flower_msgs
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  flower_msgs::msg::RobotCommand & ros_message)
{
  // Member: servo_angles
  {
    cdr >> ros_message.servo_angles;
  }

  // Member: n20_pwm
  cdr >> ros_message.n20_pwm;

  // Member: n20_target_rotations
  cdr >> ros_message.n20_target_rotations;

  // Member: led_colours_hex
  {
    cdr >> ros_message.led_colours_hex;
  }

  // Member: led_colours_brightness
  {
    cdr >> ros_message.led_colours_brightness;
  }

  return true;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_flower_msgs
get_serialized_size(
  const flower_msgs::msg::RobotCommand & ros_message,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Member: servo_angles
  {
    size_t array_size = 5;
    size_t item_size = sizeof(ros_message.servo_angles[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: n20_pwm
  {
    size_t item_size = sizeof(ros_message.n20_pwm);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: n20_target_rotations
  {
    size_t item_size = sizeof(ros_message.n20_target_rotations);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: led_colours_hex
  {
    size_t array_size = 5;
    size_t item_size = sizeof(ros_message.led_colours_hex[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: led_colours_brightness
  {
    size_t array_size = 5;
    size_t item_size = sizeof(ros_message.led_colours_brightness[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_flower_msgs
max_serialized_size_RobotCommand(
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


  // Member: servo_angles
  {
    size_t array_size = 5;

    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }

  // Member: n20_pwm
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint16_t);
    current_alignment += array_size * sizeof(uint16_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint16_t));
  }

  // Member: n20_target_rotations
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: led_colours_hex
  {
    size_t array_size = 5;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: led_colours_brightness
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
    using DataType = flower_msgs::msg::RobotCommand;
    is_plain =
      (
      offsetof(DataType, led_colours_brightness) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static bool _RobotCommand__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  auto typed_message =
    static_cast<const flower_msgs::msg::RobotCommand *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _RobotCommand__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<flower_msgs::msg::RobotCommand *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _RobotCommand__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const flower_msgs::msg::RobotCommand *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _RobotCommand__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_RobotCommand(full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}

static message_type_support_callbacks_t _RobotCommand__callbacks = {
  "flower_msgs::msg",
  "RobotCommand",
  _RobotCommand__cdr_serialize,
  _RobotCommand__cdr_deserialize,
  _RobotCommand__get_serialized_size,
  _RobotCommand__max_serialized_size
};

static rosidl_message_type_support_t _RobotCommand__handle = {
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_RobotCommand__callbacks,
  get_message_typesupport_handle_function,
};

}  // namespace typesupport_fastrtps_cpp

}  // namespace msg

}  // namespace flower_msgs

namespace rosidl_typesupport_fastrtps_cpp
{

template<>
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_EXPORT_flower_msgs
const rosidl_message_type_support_t *
get_message_type_support_handle<flower_msgs::msg::RobotCommand>()
{
  return &flower_msgs::msg::typesupport_fastrtps_cpp::_RobotCommand__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, flower_msgs, msg, RobotCommand)() {
  return &flower_msgs::msg::typesupport_fastrtps_cpp::_RobotCommand__handle;
}

#ifdef __cplusplus
}
#endif
