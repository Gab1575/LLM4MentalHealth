// MicroRos.cpp - Embedded ROS bridge: WiFi/agent connection, micro-ROS entity
// setup and teardown, the /flower_commands subscription callback that feeds
// flowerData, and the /flower_debug publish helper used throughout the firmware.

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
char debug_buffer[200]; // Sized to fit a full relayed RobotCommand message

Data flowerData;

// Colours for the LED-petal connection indicator (see WiFiSetup/MicroRosSetup)
#define CONNECTING_WIFI_COLOUR  0xFFC800 // spins while joining the WiFi network
#define CONNECTING_AGENT_COLOUR 0x00FF00 // spins once on WiFi, while finding/pinging the ROS agent

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
// Fires on every /flower_commands message; copies the incoming RobotCommand
// into flowerData (read by main.cpp's loop) and periodically echoes it back
// out over /flower_debug so the sender can confirm what the ESP32 received.
void subscription_callback(const void * msgin) {
  const flower_msgs__msg__RobotCommand * msg = (const flower_msgs__msg__RobotCommand *)msgin;
  
  for(int i = 0; i < 4; i++) {
    flowerData.servo_angles[i] = msg->servo_angles[i];
    flowerData.servo_time[i] = msg->servo_time[i];
  }

  flowerData.n20_pwm = msg->n20_pwm;
  flowerData.n20_target_rotations = msg->n20_target_rotations;
  
  for(int i = 0; i < 5; i++) {
    flowerData.led_colours_hex[i] = msg->led_colours_hex[i];
    flowerData.led_colours_brightness[i] = msg->led_colours_brightness[i];
  }

  flowerData.n20_zero = msg->n20_zero;

  // Relay every incoming /flower_commands message back out over flower_debug
  // so the sender can confirm the ESP32 is receiving the values it expects.
  // Throttled so a fast-publishing sender (e.g. a GUI slider) doesn't flood the topic.
  static unsigned long last_relay = 0;
  if (millis() - last_relay > 300) {
    last_relay = millis();
    send_debug(
      "RX servo=[%.0f,%.0f,%.0f,%.0f] time=[%.2f,%.2f,%.2f,%.2f] pwm=%d rot=%.2f led=[%06X,%06X,%06X,%06X,%06X] bright=[%d,%d,%d,%d,%d] zero=%d",
      flowerData.servo_angles[0], flowerData.servo_angles[1], flowerData.servo_angles[2], flowerData.servo_angles[3],
      flowerData.servo_time[0], flowerData.servo_time[1], flowerData.servo_time[2], flowerData.servo_time[3],
      flowerData.n20_pwm, flowerData.n20_target_rotations,
      (unsigned int)flowerData.led_colours_hex[0], (unsigned int)flowerData.led_colours_hex[1], (unsigned int)flowerData.led_colours_hex[2], (unsigned int)flowerData.led_colours_hex[3], (unsigned int)flowerData.led_colours_hex[4],
      flowerData.led_colours_brightness[0], flowerData.led_colours_brightness[1], flowerData.led_colours_brightness[2], flowerData.led_colours_brightness[3], flowerData.led_colours_brightness[4],
      flowerData.n20_zero
    );
  }
}

// Connects to whichever of the two configured WiFi networks is reachable,
// resolves that network's matching micro-ROS agent host via mDNS, and pings
// the agent before returning - so a successful return means the transport
// is actually usable, not just that WiFi came up.
bool WiFiSetup() {
  Serial.println("--- Checking WiFi & mDNS ---");
  
  // Variables to track our active configuration
  char* active_ssid = (char*)WIFI_SSID_1;
  char* active_pass = (char*)WIFI_PASS_1;
  char* active_agent = (char*)AGENT_HOSTNAME_1;

  // 1A. Try connecting to WiFi Network 1
  WiFi.begin(WIFI_SSID_1, WIFI_PASS_1); 
  Serial.print("Connecting to WiFi 1");
  
  int wifi_retries = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_retries < 20) { // 10-second timeout
    petalLightsConnectionSpin(CONNECTING_WIFI_COLOUR);
    delay(500);
    Serial.print(".");
    wifi_retries++;
  }
  Serial.println();

  // 1B. If Network 1 fails, switch EVERYTHING to Network 2
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi 1 failed. Trying WiFi 2...");
    
    // Update credentials AND the target agent
    active_ssid = (char*)WIFI_SSID_2;
    active_pass = (char*)WIFI_PASS_2;
    active_agent = (char*)AGENT_HOSTNAME_2; 
    
    WiFi.disconnect(); 
    delay(500);
    WiFi.begin(WIFI_SSID_2, WIFI_PASS_2);
    
    wifi_retries = 0;
    while (WiFi.status() != WL_CONNECTED && wifi_retries < 20) {
      petalLightsConnectionSpin(CONNECTING_WIFI_COLOUR);
      delay(500);
      Serial.print(".");
      wifi_retries++;
    }
    Serial.println();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to ANY WiFi network.");
    return false;
  }
  
  WiFi.setSleep(false); // Disables Modem Sleep for hotspots
  
  Serial.print("Connected to: ");
  Serial.println(active_ssid);

  // 2. Initialize mDNS
  if (!MDNS.begin("flower-node")) {
    Serial.println("Failed to start mDNS.");
    return false;
  }

  // 3. Resolve ONLY the Agent that matches our active WiFi network
  IPAddress agent_ip;
  int mdns_retries = 0;
  
  Serial.print("Searching for Agent (");
  Serial.print(active_agent);
  Serial.print(")...");

  while (agent_ip.toString() == "0.0.0.0" && mdns_retries < 10) {
    petalLightsConnectionSpin(CONNECTING_AGENT_COLOUR);
    agent_ip = MDNS.queryHost(active_agent);
    if (agent_ip.toString() == "0.0.0.0") {
      delay(1000);
      Serial.print(".");
      mdns_retries++;
    }
  }
  Serial.println();

  if (agent_ip.toString() == "0.0.0.0") {
    Serial.println("Failed to resolve Agent IP.");
    return false;
  }

  Serial.print("Resolved agent to IP: ");
  Serial.println(agent_ip);

  // 4. Set the transport
  set_microros_wifi_transports(active_ssid, active_pass, agent_ip, AGENT_PORT);

  // 5. Wait for the ROS agent to actually be reachable over that transport before
  // handing off to the rclc entity setup, so the green "waiting for host" spin
  // covers the whole wait, not just the mDNS lookup.
  Serial.print("Pinging Agent...");
  int agent_ping_retries = 0;
  while (rmw_uros_ping_agent(200, 1) != RMW_RET_OK && agent_ping_retries < 25) { // ~5-second timeout
    petalLightsConnectionSpin(CONNECTING_AGENT_COLOUR);
    Serial.print(".");
    agent_ping_retries++;
  }
  Serial.println();

  if (agent_ping_retries >= 25) {
    Serial.println("Agent did not respond to ping.");
    return false;
  }

  return true;
}


// Brings up every micro-ROS entity: node, /flower_commands subscription,
// /flower_debug publisher, and the single-subscription executor. Returns
// false on the first failed step so the caller (main.cpp) can retry.
bool MicroRosSetup() {
  // If WiFi or mDNS fails, abort setup
  if (!WiFiSetup()) {
    return false;
  }
  
  allocator = rcl_get_default_allocator();
  rcl_ret_t rc;

  // Replace RCCHECK with safe boolean returns
  rc = rclc_support_init(&support, 0, NULL, &allocator);
  if (rc != RCL_RET_OK) return false;

  rc = rclc_node_init_default(&node, "flower_node", "", &support);
  if (rc != RCL_RET_OK) return false;

  rc = rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(flower_msgs, msg, RobotCommand),
    "flower_commands" 
  );
  if (rc != RCL_RET_OK) return false;

  rc = rclc_publisher_init_default(
    &debug_pub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "flower_debug"
  );
  if (rc != RCL_RET_OK) return false;

  debug_msg.data.data = debug_buffer;
  debug_msg.data.capacity = sizeof(debug_buffer);

  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &support.context, 1, &allocator);
  if (rc != RCL_RET_OK) return false;
  
  rc = rclc_executor_add_subscription(
    &executor, 
    &subscriber, 
    &sub_msg, 
    &subscription_callback, 
    ON_NEW_DATA
  );
  if (rc != RCL_RET_OK) return false;

  send_debug("ESP32 flower_node successfully initialized and ready");
  return true;
}

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

// Tears down every entity created in MicroRosSetup(), in reverse order, so a
// reconnect attempt starts from a clean slate instead of leaking handles.
void MicroRosDestroy() {
  // Tell the agent we are destroying the session (timeout 0 ensures it doesn't block)
  rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
  (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

  // Destroy entities in the reverse order they were created
  rclc_executor_fini(&executor);
  rcl_publisher_fini(&debug_pub, &node);
  rcl_subscription_fini(&subscriber, &node);
  rcl_node_fini(&node);
  rclc_support_fini(&support);
}