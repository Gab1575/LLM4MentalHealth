import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from flower_msgs.msg import RobotCommand

class FlowerMuxNode(Node):
    def __init__(self):
        super().__init__('flower_mux_node')
        
        self.current_mode = "manual" 
        self.latest_gui_state = RobotCommand()  # Cache to hold the Time, LEDs, and N20 data

        # Publisher to hardware
        self.hardware_pub = self.create_publisher(RobotCommand, '/flower_commands', 10)

        # Subscribers
        self.manual_sub = self.create_subscription(RobotCommand, '/manual_commands', self.manual_callback, 10)
        self.kine_sub = self.create_subscription(RobotCommand, '/kinematic_calculated_commands', self.kine_callback, 10)
        self.mode_sub = self.create_subscription(String, '/control_mode', self.mode_callback, 10)

        self.get_logger().info("Smart Mux Node Started. Default mode: manual")

    def mode_callback(self, msg):
        if msg.data in ["manual", "kinematic"]:
            self.current_mode = msg.data
            self.get_logger().info(f"Switched control mode to: {self.current_mode}")

    def manual_callback(self, msg):
        # Always cache the latest full message from the GUI so we don't lose the Time, N20, or LED values
        self.latest_gui_state = msg
        
        if self.current_mode == "manual":
            # Pass it straight through
            self.hardware_pub.publish(msg)

    def kine_callback(self, msg):
        if self.current_mode == "kinematic":
            # Merge the incoming calculated angles with the GUI's cached Time/N20/LED data
            combined_msg = RobotCommand()
            
            # Take just the angles from the IK calculation script
            combined_msg.servo_angles = msg.servo_angles
            
            # Take everything else from the GUI's cached state
            combined_msg.servo_time = self.latest_gui_state.servo_time
            combined_msg.n20_target_rotations = self.latest_gui_state.n20_target_rotations
            combined_msg.n20_pwm = self.latest_gui_state.n20_pwm
            combined_msg.n20_zero = self.latest_gui_state.n20_zero
            combined_msg.led_colours_hex = self.latest_gui_state.led_colours_hex
            combined_msg.led_colours_brightness = self.latest_gui_state.led_colours_brightness
            
            # Publish the full, merged message to the hardware
            self.hardware_pub.publish(combined_msg)

def main(args=None):
    rclpy.init(args=args)
    node = FlowerMuxNode()
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()