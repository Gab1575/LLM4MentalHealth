"""localization_tracker_node.py - Camera-based 3D localization of the red and
blue target balls used for continuum-stem geometry sensing.

Captures from a calibrated USB camera, segments red/blue by HSV color,
shape-filters contours to reject glare/noise blobs, converts each ball's
apparent size to a 3D position via the camera's intrinsic matrix, and
publishes smoothed (EMA-filtered) positions to /vision/red_ball and
/vision/blue_ball.
"""

import rclpy
from rclpy.node import Node
import cv2
import numpy as np
import subprocess
import time

from geometry_msgs.msg import PointStamped

KNOWN_DIAMETER = 1.0 # cm dia of balls.

# --- Tracking smoothing ---
# Exponential moving average weight applied to each new raw detection (0 = ignore new
# samples entirely / infinite smoothing, 1 = no smoothing at all, raw passthrough).
EMA_ALPHA = 0.5
# If a color hasn't been seen for longer than this, don't blend the new detection into
# the old filtered value (which is now stale) - just snap straight to it.
FILTER_RESET_SEC = 0.5
# How many of the largest-by-area contours to check for a ball-like shape before
# giving up. Keeps the fallback search in find_ball() bounded/cheap.
MAX_CANDIDATES = 6

"""
Running Calibration instructions:
1. Launch the camera node:
ros2 run usb_cam usb_cam_node_exe
2. Run the camera calibration tool:
ros2 run camera_calibration cameracalibrator --size 9x6 --square 0.020 --ros-args -r image:=/image_raw 
-> Camera Calibration is set to use a 9x6 grid (Mesuring inner corners) with 2cm squares. Adjust the --size and --square parameters if you are using a different checkerboard.
"""

class LocalizationTrackerNode(Node): 
    def __init__(self): 
        super().__init__('localization_tracker') 
        self.get_logger().info('Localization Tracker Node Started') 

        self.red_pub = self.create_publisher(PointStamped, '/vision/red_ball', 10)
        self.blue_pub = self.create_publisher(PointStamped, '/vision/blue_ball', 10)
        
        # --- CALIBRATION MATRICES ---
        # Camera Matrix (K)
        self.camera_matrix = np.array([
            [861.053957, 0.000000,   358.471930],
            [0.000000,   858.673697, 228.178503],
            [0.000000,   0.000000,   1.000000]
        ])
        
        # Distortion Coefficients (D)
        self.dist_coeffs = np.array([0.195527, -0.750729, 0.000858, 0.000285, 0.000000])

        # Extract values for 3D math directly from the matrix
        self.fx = self.camera_matrix[0, 0]
        self.fy = self.camera_matrix[1, 1]
        self.cx = self.camera_matrix[0, 2]
        self.cy = self.camera_matrix[1, 2]
        
        self.cap = cv2.VideoCapture(0)  
        self.set_camera_hardware_settings()
        
        # HSV threshold parameters
        # Red
        self.lower_red1 = np.array([165, 121, 84])
        self.upper_red1 = np.array([179, 255, 255])
        self.lower_red2 = np.array([0, 121, 84])
        self.upper_red2 = np.array([7, 255, 255])

        # Blue
        self.lower_blue = np.array([96, 152, 23])
        self.upper_blue = np.array([112, 255, 255])

        # Kernel used to open (erode then dilate) each color mask, which strips out
        # small speckle noise before contour detection so a stray pixel-blob can't
        # win the "largest contour" pick away from the real ball.
        self.morph_kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))

        # Per-color EMA filter state (smoothed X/Y/Z) and last-detection timestamp,
        # used to damp frame-to-frame jitter in the published position - see EMA_ALPHA.
        self.filtered = {"Red": None, "Blue": None}
        self.last_seen = {"Red": None, "Blue": None}

        self.timer = self.create_timer(1.0 / 30.0, self.process_frame)

    def set_camera_hardware_settings(self):
        """Runs terminal commands to lock the camera settings."""
        commands = [
            # Brightness and contrast
            "v4l2-ctl -d /dev/video0 --set-ctrl=brightness=130",
            "v4l2-ctl -d /dev/video0 --set-ctrl=contrast=30",
            # White balance: disable auto, pin to a fixed color temperature
            "v4l2-ctl -d /dev/video0 --set-ctrl=white_balance_temperature_auto=0",
            "v4l2-ctl -d /dev/video0 --set-ctrl=white_balance_temperature=4600",
            # Exposure: disable auto, pin absolute value
            "v4l2-ctl -d /dev/video0 --set-ctrl=exposure_auto=1",
            "v4l2-ctl -d /dev/video0 --set-ctrl=exposure_absolute=250",
        ]

        failures = []
        for cmd in commands:
            try:
                subprocess.run(cmd, shell=True, check=True,
                                stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
            except subprocess.CalledProcessError as e:
                failures.append(cmd)
                self.get_logger().warn(f"Camera control failed ({cmd}): {e.stderr.decode().strip()}")

        if not failures:
            self.get_logger().info("Successfully locked camera hardware settings.")
        else:
            self.get_logger().warn(f"{len(failures)}/{len(commands)} camera controls failed to apply.")

    def find_ball(self, mask, frame, color_name):
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if not contours:
            return None

        # Check from largest contour down to the MAX_CANDIDATES-th largest, and return the first one that passes all shape tests.
        candidates = sorted(contours, key=cv2.contourArea, reverse=True)[:MAX_CANDIDATES]

        for c in candidates:
            # Circularity has to be judged on the RAW contour, not its convex hull -
            # convexHull() smooths away exactly the jagged/spiky edges that circularity is supposed to catch
            raw_area = cv2.contourArea(c)
            if raw_area <= 40:
                continue

            raw_perimeter = cv2.arcLength(c, True)
            if raw_perimeter == 0:
                continue

            circularity = (4 * np.pi * raw_area) / (raw_perimeter * raw_perimeter)
            if circularity <= 0.6:
                continue

            hull = cv2.convexHull(c)
            hull_area = cv2.contourArea(hull)
            if hull_area <= 0:
                continue

            # Solidity: how much of the convex hull the actual contour fills. A ball
            # is nearly as solid as its own hull; a spiky/branching reflection blob
            # has a hull much bigger than its true (jagged) area.
            solidity = raw_area / hull_area
            if solidity <= 0.85:
                continue

            ((cx_encl, cy_encl), radius) = cv2.minEnclosingCircle(hull)
            if radius <= 5:
                continue


            moments = cv2.moments(hull)
            if moments["m00"] > 0:
                x = moments["m10"] / moments["m00"]
                y = moments["m01"] / moments["m00"]
            else:
                x, y = cx_encl, cy_encl

            apparent_width = radius * 2

            # 3D Math Calculation using Matrix Parameters
            focal_length_avg = (self.fx + self.fy) / 2.0
            Z = (KNOWN_DIAMETER * focal_length_avg) / apparent_width

            # Use fx for X calculation and fy for Y calculation for maximum accuracy
            X = (x - self.cx) * Z / self.fx
            Y = (y - self.cy) * Z / self.fy

            # Draw tracking graphics
            cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 0), 2)

            # Return both the real 3D coordinates and the 2D pixel coordinates
            return {"X": X, "Y": Y, "Z": Z, "px": int(x), "py": int(y)}

        return None

    def process_frame(self): 
        ret, frame = self.cap.read() 
        if not ret: 
            return 

        # --- APPLY CALIBRATION HERE ---
        # Flatten the lens distortion before doing ANY color processing
        frame = cv2.undistort(frame, self.camera_matrix, self.dist_coeffs)

        blurred = cv2.GaussianBlur(frame, (11,11), 0)
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV) 

        # Red mask
        mask_red1 = cv2.inRange(hsv, self.lower_red1, self.upper_red1)
        mask_red2 = cv2.inRange(hsv, self.lower_red2, self.upper_red2)
        mask_red = cv2.bitwise_or(mask_red1, mask_red2)

        # Blue mask
        mask_blue = cv2.inRange(hsv, self.lower_blue, self.upper_blue)

        # Open both masks to strip speckle noise before contour detection
        mask_red = cv2.morphologyEx(mask_red, cv2.MORPH_OPEN, self.morph_kernel)
        mask_blue = cv2.morphologyEx(mask_blue, cv2.MORPH_OPEN, self.morph_kernel)

        # Call the updated function
        red_data = self.find_ball(mask_red, frame, "Red")
        blue_data = self.find_ball(mask_blue, frame, "Blue")

        # Throttled
        if not red_data:
            self.get_logger().warn("Red ball not found", throttle_duration_sec=2.0)
        if not blue_data:
            self.get_logger().warn("Blue ball not found", throttle_duration_sec=2.0)

        # If both are found, draw the line using the pixel coordinates
        if red_data and blue_data:
            cv2.line(frame, (red_data["px"], red_data["py"]), (blue_data["px"], blue_data["py"]), (255, 0, 0), 2)

        # Smooth (EMA) then publish only if found. Skip publishing if lost so the robot doesn't jerk.
        if red_data:
            X, Y, Z = self.smooth("Red", red_data["X"], red_data["Y"], red_data["Z"])
            self.publish_point(self.red_pub, 'camera_link', X, Y, Z)

        if blue_data:
            X, Y, Z = self.smooth("Blue", blue_data["X"], blue_data["Y"], blue_data["Z"])
            self.publish_point(self.blue_pub, 'camera_link', X, Y, Z)

        cv2.imshow("Localization Tracker", frame)
        cv2.imshow("red mask", mask_red)
        cv2.imshow("blue mask", mask_blue)
        cv2.waitKey(1)

    def smooth(self, color_name, X, Y, Z):
        """Exponential moving average over a color's published position.

        Blends each new raw detection with the previous filtered value. If the color hasn't been seen
        recently the filter is reset to the new sample, so reacquisition doesn't drag the old, stale
        position along with it."""
        now = time.monotonic()
        last_seen = self.last_seen[color_name]
        prev = self.filtered[color_name]

        if prev is None or last_seen is None or (now - last_seen) > FILTER_RESET_SEC:
            new_filtered = (X, Y, Z)
        else:
            new_filtered = (
                EMA_ALPHA * X + (1 - EMA_ALPHA) * prev[0],
                EMA_ALPHA * Y + (1 - EMA_ALPHA) * prev[1],
                EMA_ALPHA * Z + (1 - EMA_ALPHA) * prev[2],
            )

        self.filtered[color_name] = new_filtered
        self.last_seen[color_name] = now
        return new_filtered

    def publish_point(self, publisher, frame_id, X, Y, Z):
        msg = PointStamped()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = frame_id
        msg.point.x = float(X)
        msg.point.y = float(Y)
        msg.point.z = float(Z)
        publisher.publish(msg)

    def destroy_node(self): 
        self.cap.release() 
        cv2.destroyAllWindows() 
        super().destroy_node() 

def main(args=None): 
    rclpy.init(args=args) 
    node = LocalizationTrackerNode() 
    try:
        rclpy.spin(node) 
    except KeyboardInterrupt: 
        pass 
    finally: 
        node.destroy_node()
        rclpy.shutdown() 

if __name__ == '__main__': 
    main()