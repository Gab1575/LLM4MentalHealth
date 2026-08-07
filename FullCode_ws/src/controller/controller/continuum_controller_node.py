import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64MultiArray
from flower_msgs.msg import RobotCommand
import numpy as np

# --- Robot Parameters ---
# 2-stage continuum robot, antagonistic servo control -> 2 servos per stage (4 driven total).
NUM_SERVOS = 4

# Each stage's pair of servos can only push/pull along a single bending axis, so the
# (theta, phi) bend command from /kinematic_commands is projected onto that axis with
# cos(phi) before being split into a +/- antagonistic pair.
SERVO_ANGLE_LIMIT = 90.0   # deg, full physical throw of a servo either side of "straight up"
THETA_MAX = 180.0          # deg, max bend magnitude sent on /kinematic_commands
BEND_TO_SERVO_SCALE = SERVO_ANGLE_LIMIT / THETA_MAX  # maps full theta range onto full servo throw


class ContinuumController(Node):
    def __init__(self):
        super().__init__('continuum_controller')

        # Vision-based (red/blue ball) control is disabled for now - the robot is driven
        # purely open-loop off of /kinematic_commands.
        self.kinematic_sub = self.create_subscription(
            Float64MultiArray, '/kinematic_commands', self.kinematic_command_callback, 10)

        # Publishes final servo angles for the MUX, which forwards them as /flower_commands
        # whenever it is in "kinematic" mode.
        self.command_pub = self.create_publisher(RobotCommand, '/kinematic_calculated_commands', 10)

    def kinematic_command_callback(self, msg):
        # Expects [theta1, phi1, theta2, phi2] in degrees, matching the Flower GUI's publisher.
        if len(msg.data) < 4:
            self.get_logger().warn(
                f"Expected 4 values [theta1, phi1, theta2, phi2], got {len(msg.data)}")
            return

        theta1, phi1, theta2, phi2 = msg.data[:4]
        servo_angles = self.compute_servo_angles(theta1, phi1, theta2, phi2)
        self.publish_servo_command(servo_angles)

    def compute_servo_angles(self, theta1, phi1, theta2, phi2):
        # Converts each stage's (theta, phi) bend command into a pair of antagonistic
        # servo angles: one servo drives positive, the other mirrors it negative, with
        # 0 deg = straight up / no bend on both.
        bend1 = np.clip(theta1 * BEND_TO_SERVO_SCALE * np.cos(np.radians(phi1)),
                         -SERVO_ANGLE_LIMIT, SERVO_ANGLE_LIMIT)
        bend2 = np.clip(theta2 * BEND_TO_SERVO_SCALE * np.cos(np.radians(phi2)),
                         -SERVO_ANGLE_LIMIT, SERVO_ANGLE_LIMIT)

        servo_angles = [0.0] * NUM_SERVOS
        servo_angles[0] = float(bend1)    # Stage 1, servo A
        servo_angles[1] = float(-bend1)   # Stage 1, servo B (antagonistic)
        servo_angles[2] = float(bend2)    # Stage 2, servo A
        servo_angles[3] = float(-bend2)   # Stage 2, servo B (antagonistic)
        # servo_angles[4] left at 0.0 - unused by this 2-stage robot

        return servo_angles

    def publish_servo_command(self, servo_angles):
        msg = RobotCommand()
        msg.servo_angles = servo_angles
        self.command_pub.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = ContinuumController()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
