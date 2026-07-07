// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from flower_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice
#include "flower_msgs/msg/detail/robot_command__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
flower_msgs__msg__RobotCommand__init(flower_msgs__msg__RobotCommand * msg)
{
  if (!msg) {
    return false;
  }
  // servo_angles
  // n20_pwm
  // n20_target_rotations
  // led_colours_hex
  // led_colours_brightness
  return true;
}

void
flower_msgs__msg__RobotCommand__fini(flower_msgs__msg__RobotCommand * msg)
{
  if (!msg) {
    return;
  }
  // servo_angles
  // n20_pwm
  // n20_target_rotations
  // led_colours_hex
  // led_colours_brightness
}

bool
flower_msgs__msg__RobotCommand__are_equal(const flower_msgs__msg__RobotCommand * lhs, const flower_msgs__msg__RobotCommand * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // servo_angles
  for (size_t i = 0; i < 5; ++i) {
    if (lhs->servo_angles[i] != rhs->servo_angles[i]) {
      return false;
    }
  }
  // n20_pwm
  if (lhs->n20_pwm != rhs->n20_pwm) {
    return false;
  }
  // n20_target_rotations
  if (lhs->n20_target_rotations != rhs->n20_target_rotations) {
    return false;
  }
  // led_colours_hex
  for (size_t i = 0; i < 5; ++i) {
    if (lhs->led_colours_hex[i] != rhs->led_colours_hex[i]) {
      return false;
    }
  }
  // led_colours_brightness
  for (size_t i = 0; i < 5; ++i) {
    if (lhs->led_colours_brightness[i] != rhs->led_colours_brightness[i]) {
      return false;
    }
  }
  return true;
}

bool
flower_msgs__msg__RobotCommand__copy(
  const flower_msgs__msg__RobotCommand * input,
  flower_msgs__msg__RobotCommand * output)
{
  if (!input || !output) {
    return false;
  }
  // servo_angles
  for (size_t i = 0; i < 5; ++i) {
    output->servo_angles[i] = input->servo_angles[i];
  }
  // n20_pwm
  output->n20_pwm = input->n20_pwm;
  // n20_target_rotations
  output->n20_target_rotations = input->n20_target_rotations;
  // led_colours_hex
  for (size_t i = 0; i < 5; ++i) {
    output->led_colours_hex[i] = input->led_colours_hex[i];
  }
  // led_colours_brightness
  for (size_t i = 0; i < 5; ++i) {
    output->led_colours_brightness[i] = input->led_colours_brightness[i];
  }
  return true;
}

flower_msgs__msg__RobotCommand *
flower_msgs__msg__RobotCommand__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  flower_msgs__msg__RobotCommand * msg = (flower_msgs__msg__RobotCommand *)allocator.allocate(sizeof(flower_msgs__msg__RobotCommand), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(flower_msgs__msg__RobotCommand));
  bool success = flower_msgs__msg__RobotCommand__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
flower_msgs__msg__RobotCommand__destroy(flower_msgs__msg__RobotCommand * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    flower_msgs__msg__RobotCommand__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
flower_msgs__msg__RobotCommand__Sequence__init(flower_msgs__msg__RobotCommand__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  flower_msgs__msg__RobotCommand * data = NULL;

  if (size) {
    data = (flower_msgs__msg__RobotCommand *)allocator.zero_allocate(size, sizeof(flower_msgs__msg__RobotCommand), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = flower_msgs__msg__RobotCommand__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        flower_msgs__msg__RobotCommand__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
flower_msgs__msg__RobotCommand__Sequence__fini(flower_msgs__msg__RobotCommand__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      flower_msgs__msg__RobotCommand__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

flower_msgs__msg__RobotCommand__Sequence *
flower_msgs__msg__RobotCommand__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  flower_msgs__msg__RobotCommand__Sequence * array = (flower_msgs__msg__RobotCommand__Sequence *)allocator.allocate(sizeof(flower_msgs__msg__RobotCommand__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = flower_msgs__msg__RobotCommand__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
flower_msgs__msg__RobotCommand__Sequence__destroy(flower_msgs__msg__RobotCommand__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    flower_msgs__msg__RobotCommand__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
flower_msgs__msg__RobotCommand__Sequence__are_equal(const flower_msgs__msg__RobotCommand__Sequence * lhs, const flower_msgs__msg__RobotCommand__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!flower_msgs__msg__RobotCommand__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
flower_msgs__msg__RobotCommand__Sequence__copy(
  const flower_msgs__msg__RobotCommand__Sequence * input,
  flower_msgs__msg__RobotCommand__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(flower_msgs__msg__RobotCommand);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    flower_msgs__msg__RobotCommand * data =
      (flower_msgs__msg__RobotCommand *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!flower_msgs__msg__RobotCommand__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          flower_msgs__msg__RobotCommand__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!flower_msgs__msg__RobotCommand__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
