// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from flower_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice

#ifndef FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__BUILDER_HPP_
#define FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "flower_msgs/msg/detail/robot_command__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace flower_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotCommand_led_colours_brightness
{
public:
  explicit Init_RobotCommand_led_colours_brightness(::flower_msgs::msg::RobotCommand & msg)
  : msg_(msg)
  {}
  ::flower_msgs::msg::RobotCommand led_colours_brightness(::flower_msgs::msg::RobotCommand::_led_colours_brightness_type arg)
  {
    msg_.led_colours_brightness = std::move(arg);
    return std::move(msg_);
  }

private:
  ::flower_msgs::msg::RobotCommand msg_;
};

class Init_RobotCommand_led_colours_hex
{
public:
  explicit Init_RobotCommand_led_colours_hex(::flower_msgs::msg::RobotCommand & msg)
  : msg_(msg)
  {}
  Init_RobotCommand_led_colours_brightness led_colours_hex(::flower_msgs::msg::RobotCommand::_led_colours_hex_type arg)
  {
    msg_.led_colours_hex = std::move(arg);
    return Init_RobotCommand_led_colours_brightness(msg_);
  }

private:
  ::flower_msgs::msg::RobotCommand msg_;
};

class Init_RobotCommand_n20_target_rotations
{
public:
  explicit Init_RobotCommand_n20_target_rotations(::flower_msgs::msg::RobotCommand & msg)
  : msg_(msg)
  {}
  Init_RobotCommand_led_colours_hex n20_target_rotations(::flower_msgs::msg::RobotCommand::_n20_target_rotations_type arg)
  {
    msg_.n20_target_rotations = std::move(arg);
    return Init_RobotCommand_led_colours_hex(msg_);
  }

private:
  ::flower_msgs::msg::RobotCommand msg_;
};

class Init_RobotCommand_n20_pwm
{
public:
  explicit Init_RobotCommand_n20_pwm(::flower_msgs::msg::RobotCommand & msg)
  : msg_(msg)
  {}
  Init_RobotCommand_n20_target_rotations n20_pwm(::flower_msgs::msg::RobotCommand::_n20_pwm_type arg)
  {
    msg_.n20_pwm = std::move(arg);
    return Init_RobotCommand_n20_target_rotations(msg_);
  }

private:
  ::flower_msgs::msg::RobotCommand msg_;
};

class Init_RobotCommand_servo_angles
{
public:
  Init_RobotCommand_servo_angles()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotCommand_n20_pwm servo_angles(::flower_msgs::msg::RobotCommand::_servo_angles_type arg)
  {
    msg_.servo_angles = std::move(arg);
    return Init_RobotCommand_n20_pwm(msg_);
  }

private:
  ::flower_msgs::msg::RobotCommand msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::flower_msgs::msg::RobotCommand>()
{
  return flower_msgs::msg::builder::Init_RobotCommand_servo_angles();
}

}  // namespace flower_msgs

#endif  // FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__BUILDER_HPP_
