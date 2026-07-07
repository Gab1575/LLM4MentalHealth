#include "MicroRos.h"

// --- Instantiate the global micro-ROS objects ---rcl_node_t node;
rcl_node_t node;
rcl_subscription_t subscriber;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_publisher_t publisher;
rcl_timer_t timer;

Data flowerData; // Global instance to hold the latest data received from micro-ROS

// Infinite loop to halt program if critical ROS setup step fails
void error_loop(){
  while(1){
    delay(100);
  }
}

// 2. Define the RCCHECK macros
  // Macro to check the return code (rc) of a ROS function. If it's not OK (success), it traps the code in error_loop().
  #define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}

  // Macro for soft checking. If a function fails, it does nothing and allows the program to continue.
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

void WiFiSetup() {
  Serial.println("-----------------------------------------");
  WiFi.begin(WIFI_SSID, WIFI_PASS); // Start the WiFi connection using credentials from MicroRosWifiCredentials.h
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { //loop unitl connected
    delay(500);
    Serial.print(".");
  }
  Serial.println(""); // Move to a new line after the dots
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP()); // Print the ESP32's local IP address to the Serial Monitor for debugging purposes
  Serial.println("");
  
  // Initialize the mDNS responder. This registers the ESP32 on the network as "flower-node.local"
  if (!MDNS.begin("flower-node")) {
    while(1) { delay(1000); }
  }

  IPAddress agent_ip;
  
  // Loop continuously until a valid IP address is found (0.0.0.0 means "unassigned" or "failed")
  while (agent_ip.toString() == "0.0.0.0") {
    agent_ip = MDNS.queryHost(AGENT_HOSTNAME);
    
    if (agent_ip.toString() == "0.0.0.0") {
      delay(1000);
      Serial.println("Waiting for mDNS to resolve the agent's hostname");
    }
  }
  Serial.print("Resolved agent hostname "AGENT_HOSTNAME" to IP: ");
  Serial.println(agent_ip);
  Serial.println("-----------------------------------------");

  // Pass the credentials and the dynamically found IP directly to the micro-ROS transport layer
  set_microros_wifi_transports((char*)WIFI_SSID, (char*)WIFI_PASS, agent_ip, AGENT_PORT);
}

void MicroRosSetup() {
  WiFiSetup();  // Execute the Wi-Fi and mDNS connection sequence first
  // Assign the standard memory allocator to be used by micro-ROS for all subsequent setup functions
  allocator = rcl_get_default_allocator();
  // Initialize the micro-ROS support context, passing in the allocator
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  // Create the ROS 2 node, naming it "flower_node", and link it to the support context
  RCCHECK(rclc_node_init_default(&node, "flower_node", "", &support));

  // Update the subscriber initialization to use your custom type
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(flower_msgs, msg, RobotCommand),
    "robot_command" // This is the topic name the host will publish to
  ));

  // Initialize the executor to manage callbacks. '1' means it is configured to handle exactly one handle (your subscriber)
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  
  // Add your subscriber to the executor, linking it to the incoming message buffer (sub_msg) and the callback function
  RCCHECK(rclc_executor_add_subscription(
    &executor, 
    &subscriber, 
    &sub_msg, 
    &subscription_callback, 
    ON_NEW_DATA));//Only trigger the callback when a fresh message arrives
}
