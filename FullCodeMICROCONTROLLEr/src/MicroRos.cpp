#include "MicroRos.h"

// Instantiate the global micro-ROS objects
rcl_node_t node;
rcl_subscription_t subscriber;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;

Data flowerData; // Global instance to hold the latest data received from micro-ROS


void error_loop(){
  while(1){
    delay(100);
  }
}

// 2. Define the RCCHECK macros
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// Instantiate the custom message global
flower_msgs__msg__RobotCommand sub_msg;

// --- Callback for the subscriber ---
void subscription_callback(const void * msgin) {  
  // Cast the incoming void pointer to your custom message type
  const flower_msgs__msg__RobotCommand * msg = (const flower_msgs__msg__RobotCommand *)msgin;
  
  // 1. Extract Servo Angles
  for(int i = 0; i < 5; i++) {
    flowerData.servo_angles[i] = msg->servo_angles[i];
  }

  // 2. Extract N20 Motor Data
  flowerData.n20_pwm = msg->n20_pwm;
  flowerData.n20_target_rotations = msg->n20_target_rotations;
  
  // 3. Extract LED Data
  for(int i = 0; i < 5; i++) {
    flowerData.led_colours_hex[i] = msg->led_colours_hex[i];
    flowerData.led_colours_brightness[i] = msg->led_colours_brightness[i];
  }
}

void MicroRosSetup() {
  // ... wifi, allocator, node initialization remains the same ...

  // Update the subscriber initialization to use your custom type
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(flower_msgs, msg, RobotCommand),
    "robot_command" // This is the topic name the host will publish to
  ));

  // ... timer initialization remains the same ...

  // Update the executor to use the new sub_msg
  RCCHECK(rclc_executor_add_subscription(
    &executor, 
    &subscriber, 
    &sub_msg, 
    &subscription_callback, 
    ON_NEW_DATA));
}