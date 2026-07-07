// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from flower_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice

#ifndef FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__TRAITS_HPP_
#define FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "flower_msgs/msg/detail/robot_command__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace flower_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RobotCommand & msg,
  std::ostream & out)
{
  out << "{";
  // member: servo_angles
  {
    if (msg.servo_angles.size() == 0) {
      out << "servo_angles: []";
    } else {
      out << "servo_angles: [";
      size_t pending_items = msg.servo_angles.size();
      for (auto item : msg.servo_angles) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: n20_pwm
  {
    out << "n20_pwm: ";
    rosidl_generator_traits::value_to_yaml(msg.n20_pwm, out);
    out << ", ";
  }

  // member: n20_target_rotations
  {
    out << "n20_target_rotations: ";
    rosidl_generator_traits::value_to_yaml(msg.n20_target_rotations, out);
    out << ", ";
  }

  // member: led_colours_hex
  {
    if (msg.led_colours_hex.size() == 0) {
      out << "led_colours_hex: []";
    } else {
      out << "led_colours_hex: [";
      size_t pending_items = msg.led_colours_hex.size();
      for (auto item : msg.led_colours_hex) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: led_colours_brightness
  {
    if (msg.led_colours_brightness.size() == 0) {
      out << "led_colours_brightness: []";
    } else {
      out << "led_colours_brightness: [";
      size_t pending_items = msg.led_colours_brightness.size();
      for (auto item : msg.led_colours_brightness) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const RobotCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: servo_angles
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.servo_angles.size() == 0) {
      out << "servo_angles: []\n";
    } else {
      out << "servo_angles:\n";
      for (auto item : msg.servo_angles) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: n20_pwm
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "n20_pwm: ";
    rosidl_generator_traits::value_to_yaml(msg.n20_pwm, out);
    out << "\n";
  }

  // member: n20_target_rotations
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "n20_target_rotations: ";
    rosidl_generator_traits::value_to_yaml(msg.n20_target_rotations, out);
    out << "\n";
  }

  // member: led_colours_hex
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.led_colours_hex.size() == 0) {
      out << "led_colours_hex: []\n";
    } else {
      out << "led_colours_hex:\n";
      for (auto item : msg.led_colours_hex) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: led_colours_brightness
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.led_colours_brightness.size() == 0) {
      out << "led_colours_brightness: []\n";
    } else {
      out << "led_colours_brightness:\n";
      for (auto item : msg.led_colours_brightness) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RobotCommand & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace flower_msgs

namespace rosidl_generator_traits
{

[[deprecated("use flower_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const flower_msgs::msg::RobotCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  flower_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use flower_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const flower_msgs::msg::RobotCommand & msg)
{
  return flower_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<flower_msgs::msg::RobotCommand>()
{
  return "flower_msgs::msg::RobotCommand";
}

template<>
inline const char * name<flower_msgs::msg::RobotCommand>()
{
  return "flower_msgs/msg/RobotCommand";
}

template<>
struct has_fixed_size<flower_msgs::msg::RobotCommand>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<flower_msgs::msg::RobotCommand>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<flower_msgs::msg::RobotCommand>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__TRAITS_HPP_
