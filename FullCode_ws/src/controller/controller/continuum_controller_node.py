import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PointStamped, Float32MultiArray
import message_filters
import numpy as np

# --- Robot Parameters ---
L1 = 10.0  # Length of Stage 1
L2 = 10.0  # Length of Stage 2
BALL_OFFSET = 3.0  # Distance from the end of Stage 2 (or tilt joint) to the center of the ball assembly

class ContinuumController(Node):
    def __init__(self):
        super().__init__('continuum_controller')
        
        # Latest known joint configurations from GUI
        # [theta1, phi1, theta2, phi2, tilt]
        self.current_q = np.zeros(5)
        
        # 1. Subscribe to GUI or joint state topic
        self.state_sub = self.create_subscription(Float32MultiArray, '/robot/joint_states', self.joint_state_callback, 10)

        # 2. Create message_filter subscribers for vision tracking
        self.red_sub = message_filters.Subscriber(self, PointStamped, '/vision/red_ball')
        self.blue_sub = message_filters.Subscriber(self, PointStamped, '/vision/blue_ball')

        # 3. Synchronize vision inputs (waits up to 0.1s for matching timestamps)
        self.ts = message_filters.ApproximateTimeSynchronizer(
            [self.red_sub, self.blue_sub], queue_size=10, slop=0.1)
        self.ts.registerCallback(self.control_loop_callback)
        
        # Target state [X, Y, Z, Pitch, Yaw]
        self.target_state = np.array([0.0, 0.0, 30.0, 0.0, 0.0])
        
        self.get_logger().info('Continuum Controller Node Initialized')

    def joint_state_callback(self, msg):
        """Callback to store the latest joint angles coming from your GUI/system."""
        if len(msg.data) >= 5:
            self.current_q = np.array(msg.data[:5])

    def control_loop_callback(self, red_msg, blue_msg):
        # --- STEP A: Process Vision Data ---
        P_red = np.array([red_msg.point.x, red_msg.point.y, red_msg.point.z])
        P_blue = np.array([blue_msg.point.x, blue_msg.point.y, blue_msg.point.z])

        # 1. Calculate the midpoint between the red and blue balls
        P_center = (P_red + P_blue) / 2.0

        # 2. Build the forward-pointing unit vector from red to blue
        vector = P_blue - P_red
        norm = np.linalg.norm(vector)
        if norm == 0:
            return
        v_hat = vector / norm

        # 3. Shift center point forward to find the actual flower head tip
        flower_head_pos = P_center + (v_hat * BALL_OFFSET)

        # 4. Extract actual Pitch and Yaw from the unit vector
        yaw = np.arctan2(v_hat[0], v_hat[2])
        pitch = np.arctan2(-v_hat[1], np.sqrt(v_hat[0]**2 + v_hat[2]**2))

        actual_state = np.array([flower_head_pos[0], flower_head_pos[1], flower_head_pos[2], pitch, yaw])

        # --- STEP B: Run Forward Kinematics Model ---
        model_tip_pos, model_orientation = self.run_forward_kinematics(self.current_q)

        # --- STEP C: Error Calculation & Control ---
        error = self.target_state - actual_state
        
        # Placeholder for Jacobian Inverse Control mapping
        # q_dot = J_inv @ (Kp @ error)
        # self.publish_motor_commands(q_dot)

    def run_forward_kinematics(self, q):
        """Computes the full forward kinematics using your configuration vector q."""
        th1, ph1, th2, ph2, tilt = q
        
        # 1. Calculate Stage 1 (Global frame)
        x1, y1, z1 = self.calc_segment(th1, ph1, L1)
        tip1_pos = np.array([x1[-1], y1[-1], z1[-1]])
        R1 = self.get_rotation_matrix(th1, ph1)
        
        # 2. Calculate Stage 2 (Local frame of stage 1 tip)
        x2_local, y2_local, z2_local = self.calc_segment(th2, ph2, L2)
        pts2_local = np.vstack((x2_local, y2_local, z2_local))
        
        # 3. Transform Stage 2 to Global frame
        pts2_global = R1 @ pts2_local + tip1_pos.reshape(3, 1)
        tip2_pos = np.array([pts2_global[0, -1], pts2_global[1, -1], pts2_global[2, -1]])
        R2 = R1 @ self.get_rotation_matrix(th2, ph2)
        
        # 4. Account for the 1DOF tilt joint at the top
        c_t, s_t = np.cos(tilt), np.sin(tilt)
        R_tilt = np.array([[1, 0, 0], [0, c_t, -s_t], [0, s_t, c_t]])
        R_final = R2 @ R_tilt
        
        # Final tip position including the ball offset
        # The forward vector in local coordinates is the Z-axis vector [0, 0, 1] transformed
        local_forward = np.array([0, 0, 1])
        global_forward = R_final @ local_forward
        flower_tip = tip2_pos + (global_forward * BALL_OFFSET)
        
        # Extract model pitch/yaw from global_forward vector
        model_yaw = np.arctan2(global_forward[0], global_forward[2])
        model_pitch = np.arctan2(-global_forward[1], np.sqrt(global_forward[0]**2 + global_forward[2]**2))
        
        model_state = np.array([flower_tip[0], flower_tip[1], flower_tip[2], model_pitch, model_yaw])
        return flower_tip, model_state

    def calc_segment(self, theta, phi, L, num_points=50):
        """Calculates the local 3D curve of a segment starting at origin."""
        s = np.linspace(0, L, num_points)
        if theta < 1e-6:
            return np.zeros_like(s), np.zeros_like(s), s
        r = L / theta
        x = r * (1 - np.cos(theta * s / L)) * np.cos(phi)
        y = r * (1 - np.cos(theta * s / L)) * np.sin(phi)
        z = r * np.sin(theta * s / L)
        return x, y, z    

    def get_rotation_matrix(self, theta, phi):
        """Calculates the rotation matrix at the tip of a segment."""
        c_p, s_p = np.cos(phi), np.sin(phi)
        c_t, s_t = np.cos(theta), np.sin(theta)
        
        Rz_phi = np.array([[c_p, -s_p, 0], [s_p, c_p, 0], [0, 0, 1]])
        Ry_th = np.array([[c_t, 0, s_t], [0, 1, 0], [-s_t, 0, c_t]])
        Rz_mphi = np.array([[c_p, s_p, 0], [-s_p, c_p, 0], [0, 0, 1]])
        
        return Rz_phi @ Ry_th @ Rz_mphi

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