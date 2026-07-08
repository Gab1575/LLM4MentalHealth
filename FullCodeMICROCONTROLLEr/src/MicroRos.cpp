#include "MicroRos.h"

// --- Instantiate the global micro-ROS objects ---
rcl_node_t node;
rcl_subscription_t subscriber;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_publisher_t publisher;
rcl_timer_t timer;

// Debug globals
rcl_publisher_t debug_pub;
std_msgs__msg__String debug_msg; // Added message struct
char debug_buffer[100]; 

Data flowerData; 

// 2. Define the RCCHECK macros
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// Infinite loop to halt program if critical ROS setup step fails
void error_loop(){
  while(1){
    delay(100);
  }
}

flower_msgs__msg__RobotCommand sub_msg;

// --- Callback for the subscriber ---
void subscription_callback(const void * msgin) {  
  const flower_msgs__msg__RobotCommand * msg = (const flower_msgs__msg__RobotCommand *)msgin;
  
  for(int i = 0; i < 5; i++) {
    flowerData.servo_angles[i] = msg->servo_angles[i];
  }

  flowerData.n20_pwm = msg->n20_pwm;
  flowerData.n20_target_rotations = msg->n20_target_rotations;
  
  for(int i = 0; i < 5; i++) {
    flowerData.led_colours_hex[i] = msg->led_colours_hex[i];
    flowerData.led_colours_brightness[i] = msg->led_colours_brightness[i];
  }
}

void WiFiSetup() {
  Serial.println("-----------------------------------------");
  WiFi.begin(WIFI_SSID, WIFI_PASS); 
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println(""); 
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP()); 
  Serial.println("");
  
  if (!MDNS.begin("flower-node")) {
    while(1) { delay(1000); }
  }

  IPAddress agent_ip;
  while (agent_ip.toString() == "0.0.0.0") {
    agent_ip = MDNS.queryHost(AGENT_HOSTNAME);
    if (agent_ip.toString() == "0.0.0.0") {
      delay(1000);
      Serial.println("Waiting for mDNS to resolve the agent's hostname");
    }
  }
  Serial.print("Resolved agent hostname " AGENT_HOSTNAME " to IP: ");
  Serial.println(agent_ip);
  Serial.println("-----------------------------------------");

  set_microros_wifi_transports((char*)WIFI_SSID, (char*)WIFI_PASS, agent_ip, AGENT_PORT);
}

void MicroRosSetup() {
  WiFiSetup();  
  
  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "flower_node", "", &support));

  // Initialize your custom subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(flower_msgs, msg, RobotCommand),
    "flower_commands" 
  ));

  // Initialize the debug string publisher
  RCCHECK(rclc_publisher_init_default(
    &debug_pub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "flower_debug" // Topic name to monitor on host
  ));

  // Link the micro-ROS message pointer to your static buffer
  debug_msg.data.data = debug_buffer;
  debug_msg.data.capacity = sizeof(debug_buffer);

  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  
  RCCHECK(rclc_executor_add_subscription(
    &executor, 
    &subscriber, 
    &sub_msg, 
    &subscription_callback, 
    ON_NEW_DATA));

    send_debug("ESP32 flower_node successfully initialized and ready");

  }

// --- New Debug Helper Function ---
// Works exactly like printf. Example: send_debug("Motor PWM is: %d", flowerData.n20_pwm);
void send_debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    // Safely format the string into the pre-allocated buffer
    vsnprintf(debug_buffer, sizeof(debug_buffer), format, args);
    va_end(args);
    
    // Update the message length and publish
    debug_msg.data.size = strlen(debug_msg.data.data);
    
    // Using RCSOFTCHECK so a failed debug message doesn't crash the ESP32
    RCSOFTCHECK(rcl_publish(&debug_pub, &debug_msg, NULL));
}