import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64MultiArray
from geometry_msgs.msg import PointStamped
from flower_msgs.msg import RobotCommand
import numpy as np

# --- Robot Parameters ---
# 2-stage continuum robot, antagonistic servo control -> 2 servos per stage (4 driven total).
# Servo 0 (neck joint) has been removed; only S1-S4 exist, now at indices 0-3.
NUM_SERVOS = 4

# Each servo is wired antagonistically on its OWN tendon pair (rotating + pulls one
# tendon while releasing the other, - does the reverse), so it alone drives a full
# signed bend along a single plane through the stage - it is not mirrored by its
# stage-mate. Each stage's two servos cover two orthogonal bending planes: S1 (lower)
# and S2 (upper) act in the XZ plane, S4 (lower) and S3 (upper) act in the YZ plane.
# The (theta, phi) bend command is therefore resolved into XZ/YZ components with
# cos(phi)/sin(phi) rather than split into a +/- antagonistic pair.
SERVO_ANGLE_LIMIT = 90.0   # deg, full physical throw of a servo either side of "straight up"
THETA_MAX = 180.0          # deg, max bend magnitude sent on /kinematic_commands
BEND_TO_SERVO_SCALE = SERVO_ANGLE_LIMIT / THETA_MAX  # maps full theta range onto full servo throw

# Servo wiring: S1/S4 actuate the lower stage, S2/S3 actuate the upper stage. The upper
# stage is mechanically mounted 45 deg offset from the lower stage's bending planes, so
# that offset is subtracted from phi2 before resolving stage 2's bend onto its servos.
UPPER_STAGE_OFFSET_DEG = 45.0


class ContinuumController(Node):
    def __init__(self):
        super().__init__('continuum_controller')

        self.kinematic_sub = self.create_subscription(Float64MultiArray, '/kinematic_commands', self.kinematic_command_callback, 10)

        self.red_ball_sub = self.create_subscription(PointStamped, '/vision/red_ball', self.red_ball_callback, 10)
        self.blue_ball_sub = self.create_subscription(PointStamped, '/vision/blue_ball', self.blue_ball_callback, 10)

        # Publishes final servo angles for the MUX, which forwards them as /flower_commands
        # whenever it is in "kinematic" mode.
        self.command_pub = self.create_publisher(RobotCommand, '/kinematic_calculated_commands', 10)

        # Latest position received from each ball's callback (None until first seen).
        # check_ball_movement() needs both at once, but red/blue arrive on independent
        # callbacks, so each one is cached here and re-checked on every new message.
        self.latest_red = None
        self.latest_blue = None

        # check_ball_movement() state: last-sampled |position| for each ball and when
        # that sample was taken, used to detect movement between samples.
        self.last_red_abs = 0.0
        self.last_blue_abs = 0.0
        self.last_check_time = 0.0

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
        # Resolves each stage's (theta, phi) bend command into its two orthogonal
        # servo-plane components (XZ / YZ). Each servo is independently antagonistic
        # via its own tendon pair, so it directly takes the signed component for its
        # plane - no mirroring against the other servo on the stage.
        #
        # Stage 2 (upper) is mounted UPPER_STAGE_OFFSET_DEG rotated relative to stage 1
        # (lower)'s bending planes, so that offset is removed from phi2 first.
        phi1_rad = np.radians(phi1)
        phi2_rad = np.radians(phi2 - UPPER_STAGE_OFFSET_DEG)

        bend1_xz = np.clip(theta1 * BEND_TO_SERVO_SCALE * np.cos(phi1_rad),
                            -SERVO_ANGLE_LIMIT, SERVO_ANGLE_LIMIT)
        bend1_yz = np.clip(theta1 * BEND_TO_SERVO_SCALE * np.sin(phi1_rad),
                            -SERVO_ANGLE_LIMIT, SERVO_ANGLE_LIMIT)
        bend2_xz = np.clip(theta2 * BEND_TO_SERVO_SCALE * np.cos(phi2_rad),
                            -SERVO_ANGLE_LIMIT, SERVO_ANGLE_LIMIT)
        bend2_yz = np.clip(theta2 * BEND_TO_SERVO_SCALE * np.sin(phi2_rad),
                            -SERVO_ANGLE_LIMIT, SERVO_ANGLE_LIMIT)

        servo_angles = [0.0] * NUM_SERVOS
        servo_angles[0] = float(bend1_xz)  # S1 - lower stage, XZ plane
        servo_angles[1] = float(bend2_xz)  # S2 - upper stage, XZ plane
        servo_angles[2] = float(bend2_yz)  # S3 - upper stage, YZ plane
        servo_angles[3] = float(bend1_yz)  # S4 - lower stage, YZ plane

        # cos()/sin() of exact multiples of 90 deg aren't exactly 0.0 in floating point
        # (e.g. ~1e-15 instead), which is meaningless at servo resolution - round it away.
        servo_angles = [round(a, 6) for a in servo_angles]

        return servo_angles

    def publish_servo_command(self, servo_angles):
        msg = RobotCommand()
        msg.servo_angles = servo_angles
        self.command_pub.publish(msg)

    def red_ball_callback(self, msg):
        self.latest_red = (msg.point.x, msg.point.y, msg.point.z)
        self.check_ball_movement()

    def blue_ball_callback(self, msg):
        self.latest_blue = (msg.point.x, msg.point.y, msg.point.z)
        self.check_ball_movement()

    def check_ball_movement(self):
        # Red and blue arrive on independent callbacks, so wait until both have
        # reported at least once before trying to check anything.
        if self.latest_red is None or self.latest_blue is None:
            return

        r_x, r_y, r_z = self.latest_red
        b_x, b_y, b_z = self.latest_blue

        # Checks the balls for movement by calculating the absolute position
        red_abs = np.sqrt(r_x**2 + r_y**2 + r_z**2)
        blue_abs = np.sqrt(b_x**2 + b_y**2 + b_z**2)

        current_time = self.get_clock().now().nanoseconds / 1e9  # Convert to seconds

        # Compares the current abs position to a previous position from time (t-1) to determine if the balls are moving
        if current_time - self.last_check_time >= 0.2:  # Check per period
            if abs(red_abs - self.last_red_abs) > 0.3 or abs(blue_abs - self.last_blue_abs) > 0.3:
                self.get_logger().info("Balls are moving.")
            else:
                self.get_logger().info("Balls are stationary.")

            # Update last positions and time
            self.last_red_abs = red_abs
            self.last_blue_abs = blue_abs
            self.last_check_time = current_time

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