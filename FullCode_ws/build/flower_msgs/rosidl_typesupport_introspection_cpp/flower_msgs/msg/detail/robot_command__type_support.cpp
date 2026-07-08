// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from flower_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "flower_msgs/msg/detail/robot_command__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace flower_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void RobotCommand_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) flower_msgs::msg::RobotCommand(_init);
}

void RobotCommand_fini_function(void * message_memory)
{
  auto typed_message = static_cast<flower_msgs::msg::RobotCommand *>(message_memory);
  typed_message->~RobotCommand();
}

size_t size_function__RobotCommand__servo_angles(const void * untyped_member)
{
  (void)untyped_member;
  return 5;
}

const void * get_const_function__RobotCommand__servo_angles(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::array<int8_t, 5> *>(untyped_member);
  return &member[index];
}

void * get_function__RobotCommand__servo_angles(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::array<int8_t, 5> *>(untyped_member);
  return &member[index];
}

void fetch_function__RobotCommand__servo_angles(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const int8_t *>(
    get_const_function__RobotCommand__servo_angles(untyped_member, index));
  auto & value = *reinterpret_cast<int8_t *>(untyped_value);
  value = item;
}

void assign_function__RobotCommand__servo_angles(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<int8_t *>(
    get_function__RobotCommand__servo_angles(untyped_member, index));
  const auto & value = *reinterpret_cast<const int8_t *>(untyped_value);
  item = value;
}

size_t size_function__RobotCommand__led_colours_hex(const void * untyped_member)
{
  (void)untyped_member;
  return 5;
}

const void * get_const_function__RobotCommand__led_colours_hex(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::array<uint32_t, 5> *>(untyped_member);
  return &member[index];
}

void * get_function__RobotCommand__led_colours_hex(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::array<uint32_t, 5> *>(untyped_member);
  return &member[index];
}

void fetch_function__RobotCommand__led_colours_hex(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const uint32_t *>(
    get_const_function__RobotCommand__led_colours_hex(untyped_member, index));
  auto & value = *reinterpret_cast<uint32_t *>(untyped_value);
  value = item;
}

void assign_function__RobotCommand__led_colours_hex(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<uint32_t *>(
    get_function__RobotCommand__led_colours_hex(untyped_member, index));
  const auto & value = *reinterpret_cast<const uint32_t *>(untyped_value);
  item = value;
}

size_t size_function__RobotCommand__led_colours_brightness(const void * untyped_member)
{
  (void)untyped_member;
  return 5;
}

const void * get_const_function__RobotCommand__led_colours_brightness(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::array<int16_t, 5> *>(untyped_member);
  return &member[index];
}

void * get_function__RobotCommand__led_colours_brightness(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::array<int16_t, 5> *>(untyped_member);
  return &member[index];
}

void fetch_function__RobotCommand__led_colours_brightness(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const int16_t *>(
    get_const_function__RobotCommand__led_colours_brightness(untyped_member, index));
  auto & value = *reinterpret_cast<int16_t *>(untyped_value);
  value = item;
}

void assign_function__RobotCommand__led_colours_brightness(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<int16_t *>(
    get_function__RobotCommand__led_colours_brightness(untyped_member, index));
  const auto & value = *reinterpret_cast<const int16_t *>(untyped_value);
  item = value;
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember RobotCommand_message_member_array[5] = {
  {
    "servo_angles",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    true,  // is array
    5,  // array size
    false,  // is upper bound
    offsetof(flower_msgs::msg::RobotCommand, servo_angles),  // bytes offset in struct
    nullptr,  // default value
    size_function__RobotCommand__servo_angles,  // size() function pointer
    get_const_function__RobotCommand__servo_angles,  // get_const(index) function pointer
    get_function__RobotCommand__servo_angles,  // get(index) function pointer
    fetch_function__RobotCommand__servo_angles,  // fetch(index, &value) function pointer
    assign_function__RobotCommand__servo_angles,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "n20_pwm",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(flower_msgs::msg::RobotCommand, n20_pwm),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "n20_target_rotations",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(flower_msgs::msg::RobotCommand, n20_target_rotations),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "led_colours_hex",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    true,  // is array
    5,  // array size
    false,  // is upper bound
    offsetof(flower_msgs::msg::RobotCommand, led_colours_hex),  // bytes offset in struct
    nullptr,  // default value
    size_function__RobotCommand__led_colours_hex,  // size() function pointer
    get_const_function__RobotCommand__led_colours_hex,  // get_const(index) function pointer
    get_function__RobotCommand__led_colours_hex,  // get(index) function pointer
    fetch_function__RobotCommand__led_colours_hex,  // fetch(index, &value) function pointer
    assign_function__RobotCommand__led_colours_hex,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "led_colours_brightness",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    true,  // is array
    5,  // array size
    false,  // is upper bound
    offsetof(flower_msgs::msg::RobotCommand, led_colours_brightness),  // bytes offset in struct
    nullptr,  // default value
    size_function__RobotCommand__led_colours_brightness,  // size() function pointer
    get_const_function__RobotCommand__led_colours_brightness,  // get_const(index) function pointer
    get_function__RobotCommand__led_colours_brightness,  // get(index) function pointer
    fetch_function__RobotCommand__led_colours_brightness,  // fetch(index, &value) function pointer
    assign_function__RobotCommand__led_colours_brightness,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers RobotCommand_message_members = {
  "flower_msgs::msg",  // message namespace
  "RobotCommand",  // message name
  5,  // number of fields
  sizeof(flower_msgs::msg::RobotCommand),
  RobotCommand_message_member_array,  // message members
  RobotCommand_init_function,  // function to initialize message memory (memory has to be allocated)
  RobotCommand_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t RobotCommand_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &RobotCommand_message_members,
  get_message_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace flower_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<flower_msgs::msg::RobotCommand>()
{
  return &::flower_msgs::msg::rosidl_typesupport_introspection_cpp::RobotCommand_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, flower_msgs, msg, RobotCommand)() {
  return &::flower_msgs::msg::rosidl_typesupport_introspection_cpp::RobotCommand_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
