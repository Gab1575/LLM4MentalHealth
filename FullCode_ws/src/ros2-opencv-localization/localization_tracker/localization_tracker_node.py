import rclpy 
from rclpy.node import Node 
import cv2 
import numpy as np 

from geometry_msgs.msg import PointStamped

KNOWN_DIAMETER = 1 # cm dia of balls.
FOCAL_LENGTH = 615.0 # calibrated focal length of the camera lens, measured in pixels.

# Assuming a standard 640x480 webcam. If you use 1080p, change these to 960 and 540.
CX = 320.0 
CY = 240.0 

class LocalizationTrackerNode(Node): 
    def __init__(self): 
        super().__init__('localization_tracker') 
        self.get_logger().info('Localization Tracker Node Started') 

        self.red_pub = self.create_publisher(PointStamped, '/vision/red_ball', 10)
        self.blue_pub = self.create_publisher(PointStamped, '/vision/blue_ball', 10)
        
        self.cap = cv2.VideoCapture(0)  
        
        # HSV threshold parameters
        # Red
        self.lower_red1 = np.array([147, 124, 116])
        self.upper_red1 = np.array([179, 255, 255])
        self.lower_red2 = np.array([0,124,116])
        self.upper_red2 = np.array([8,255,255])

        # Blue 
        self.lower_blue = np.array([92, 215, 46])
        self.upper_blue = np.array([140, 255, 219])

        self.timer = self.create_timer(1.0 / 30.0, self.process_frame)

    def find_ball(self, mask, frame, color_name):
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 
        if contours: 
            c = max(contours, key=cv2.contourArea) 
            hull = cv2.convexHull(c)
            area = cv2.contourArea(hull)
            
            if area > 40: 
                perimeter = cv2.arcLength(hull, True)
                if perimeter == 0:
                    return None
                
                circularity = (4 * np.pi * area) / (perimeter * perimeter)
                
                if circularity > 0.6:
                    ((x, y), radius) = cv2.minEnclosingCircle(hull) 
                    if radius > 5:
                        apparent_width = radius * 2 
                        
                        # 3D Math Calculation
                        Z = (KNOWN_DIAMETER * FOCAL_LENGTH) / apparent_width 
                        X = (x - CX) * Z / FOCAL_LENGTH
                        Y = (y - CY) * Z / FOCAL_LENGTH
                        
                        # Draw tracking graphics
                        cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 0), 2) 
                        
                        # Return both the real 3D coordinates and the 2D pixel coordinates
                        return {"X": X, "Y": Y, "Z": Z, "px": int(x), "py": int(y)}
                        
        return None

    def process_frame(self): 
        ret, frame = self.cap.read() 
        if not ret: 
            return 

        blurred = cv2.GaussianBlur(frame, (11,11), 0)
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV) 

        # Red mask
        mask_red1 = cv2.inRange(hsv, self.lower_red1, self.upper_red1)
        mask_red2 = cv2.inRange(hsv, self.lower_red2, self.upper_red2) 
        mask_red = cv2.bitwise_or(mask_red1, mask_red2)

        # Blue mask
        mask_blue = cv2.inRange(hsv, self.lower_blue, self.upper_blue) 

        # Call the updated function
        red_data = self.find_ball(mask_red, frame, "Red") 
        blue_data = self.find_ball(mask_blue, frame, "Blue") 

        # If both are found, draw the line using the pixel coordinates
        if red_data and blue_data: 
            cv2.line(frame, (red_data["px"], red_data["py"]), (blue_data["px"], blue_data["py"]), (255, 0, 0), 2) 

        # Publish only if found. Skip publishing if lost so the robot doesn't jerk.
        if red_data:
            self.publish_point(self.red_pub, 'camera_link', red_data["X"], red_data["Y"], red_data["Z"])
            
        if blue_data:
            self.publish_point(self.blue_pub, 'camera_link', blue_data["X"], blue_data["Y"], blue_data["Z"])

        cv2.imshow("Localization Tracker", frame) 
        cv2.imshow("red mask", mask_red)
        cv2.imshow("blue mask", mask_blue)
        cv2.waitKey(1) 

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