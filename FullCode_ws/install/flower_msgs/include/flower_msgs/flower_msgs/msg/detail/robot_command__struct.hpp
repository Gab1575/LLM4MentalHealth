// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from flower_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice

#ifndef FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__STRUCT_HPP_
#define FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__flower_msgs__msg__RobotCommand __attribute__((deprecated))
#else
# define DEPRECATED__flower_msgs__msg__RobotCommand __declspec(deprecated)
#endif

namespace flower_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct RobotCommand_
{
  using Type = RobotCommand_<ContainerAllocator>;

  explicit RobotCommand_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      std::fill<typename std::array<int8_t, 5>::iterator, int8_t>(this->servo_angles.begin(), this->servo_angles.end(), 0);
      this->n20_pwm = 0;
      this->n20_target_rotations = 0.0f;
      std::fill<typename std::array<uint32_t, 5>::iterator, uint32_t>(this->led_colours_hex.begin(), this->led_colours_hex.end(), 0ul);
      std::fill<typename std::array<int16_t, 5>::iterator, int16_t>(this->led_colours_brightness.begin(), this->led_colours_brightness.end(), 0);
    }
  }

  explicit RobotCommand_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : servo_angles(_alloc),
    led_colours_hex(_alloc),
    led_colours_brightness(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      std::fill<typename std::array<int8_t, 5>::iterator, int8_t>(this->servo_angles.begin(), this->servo_angles.end(), 0);
      this->n20_pwm = 0;
      this->n20_target_rotations = 0.0f;
      std::fill<typename std::array<uint32_t, 5>::iterator, uint32_t>(this->led_colours_hex.begin(), this->led_colours_hex.end(), 0ul);
      std::fill<typename std::array<int16_t, 5>::iterator, int16_t>(this->led_colours_brightness.begin(), this->led_colours_brightness.end(), 0);
    }
  }

  // field types and members
  using _servo_angles_type =
    std::array<int8_t, 5>;
  _servo_angles_type servo_angles;
  using _n20_pwm_type =
    int16_t;
  _n20_pwm_type n20_pwm;
  using _n20_target_rotations_type =
    float;
  _n20_target_rotations_type n20_target_rotations;
  using _led_colours_hex_type =
    std::array<uint32_t, 5>;
  _led_colours_hex_type led_colours_hex;
  using _led_colours_brightness_type =
    std::array<int16_t, 5>;
  _led_colours_brightness_type led_colours_brightness;

  // setters for named parameter idiom
  Type & set__servo_angles(
    const std::array<int8_t, 5> & _arg)
  {
    this->servo_angles = _arg;
    return *this;
  }
  Type & set__n20_pwm(
    const int16_t & _arg)
  {
    this->n20_pwm = _arg;
    return *this;
  }
  Type & set__n20_target_rotations(
    const float & _arg)
  {
    this->n20_target_rotations = _arg;
    return *this;
  }
  Type & set__led_colours_hex(
    const std::array<uint32_t, 5> & _arg)
  {
    this->led_colours_hex = _arg;
    return *this;
  }
  Type & set__led_colours_brightness(
    const std::array<int16_t, 5> & _arg)
  {
    this->led_colours_brightness = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    flower_msgs::msg::RobotCommand_<ContainerAllocator> *;
  using ConstRawPtr =
    const flower_msgs::msg::RobotCommand_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<flower_msgs::msg::RobotCommand_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<flower_msgs::msg::RobotCommand_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      flower_msgs::msg::RobotCommand_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<flower_msgs::msg::RobotCommand_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      flower_msgs::msg::RobotCommand_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<flower_msgs::msg::RobotCommand_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<flower_msgs::msg::RobotCommand_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<flower_msgs::msg::RobotCommand_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__flower_msgs__msg__RobotCommand
    std::shared_ptr<flower_msgs::msg::RobotCommand_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__flower_msgs__msg__RobotCommand
    std::shared_ptr<flower_msgs::msg::RobotCommand_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const RobotCommand_ & other) const
  {
    if (this->servo_angles != other.servo_angles) {
      return false;
    }
    if (this->n20_pwm != other.n20_pwm) {
      return false;
    }
    if (this->n20_target_rotations != other.n20_target_rotations) {
      return false;
    }
    if (this->led_colours_hex != other.led_colours_hex) {
      return false;
    }
    if (this->led_colours_brightness != other.led_colours_brightness) {
      return false;
    }
    return true;
  }
  bool operator!=(const RobotCommand_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct RobotCommand_

// alias to use template instance with default allocator
using RobotCommand =
  flower_msgs::msg::RobotCommand_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace flower_msgs

#endif  // FLOWER_MSGS__MSG__DETAIL__ROBOT_COMMAND__STRUCT_HPP_
