import rclpy 
from rclpy.node import Node 
import cv2 
import numpy as np 

KNOWN_DIAMETER = 1 #cm dia of balls.
FOCAL_LENGTH = 615.0 #calibrated focal length of the camera lens, measured in pixels.

class LocalizationTrackerNode(Node): 
    def __init__(self): 
        super().__init__('localization_tracker') # Initializes the parent Node class and names this node 'localization_tracker'.
        self.get_logger().info('Localization Tracker Node Started') 
        
        self.cap = cv2.VideoCapture(0)  # Opens a connection to the default webcam (index 0). WILL NEED TO CHANGE THIS TO THE CORRECT CAMERA INDEX OR PATH.
        
# HSV threshold parameters
        
        #Red
        self.lower_red1 = np.array([147, 124, 116])
        self.upper_red1 = np.array([179, 255, 255])

        self.lower_red2 = np.array([0,124,116])
        self.upper_red2 = np.array([8,255,255])

        #Blue 
        self.lower_blue = np.array([92, 215, 46])
        self.upper_blue = np.array([140, 255, 219])

        self.timer = self.create_timer(1.0 / 30.0, self.process_frame)

    def find_ball(self, mask, frame, color_name):
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 
            if contours: 
                # Grab the largest raw contour by area
                c = max(contours, key=cv2.contourArea) 
                
                # 1. CONVEX HULL: Wrap the contour to ignore missing glare chunks
                hull = cv2.convexHull(c)
                
                # Calculate Area based on the patched hull, not the broken contour
                area = cv2.contourArea(hull)
                
                if area > 40: 
                    # 2. CIRCULARITY: Calculate the perimeter of the hull
                    perimeter = cv2.arcLength(hull, True)
                    
                    # Prevent division by zero
                    if perimeter == 0:
                        return None, None
                    
                    # Calculate circularity using the formula
                    circularity = (4 * np.pi * area) / (perimeter * perimeter)
                    
                    # Filter: Only proceed if the object is reasonably circular
                    if circularity > 0.6:
                        ((x, y), radius) = cv2.minEnclosingCircle(hull) 
                        if radius > 5:
                            apparent_width = radius * 2 
                            distance = (KNOWN_DIAMETER * FOCAL_LENGTH) / apparent_width 
                            
                            # Draw tracking graphics
                            cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 0), 2) 
                            cv2.putText(frame, f"{color_name}: {distance:.1f}cm", (int(x) - 40, int(y) - int(radius) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2) 
                            
                            return (x, y), distance
                            
            return None, None

    def process_frame(self): 
        ret, frame = self.cap.read() # Grabs the latest image frame from the webcam.
        if not ret: 
            return 

        #adds GausianBlur
        blurred = cv2.GaussianBlur(frame, (11,11),0)

        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV) #Converts image from BGR to HSV

        # Red mask
        mask_red1 = cv2.inRange(hsv, self.lower_red1, self.upper_red1)
        mask_red2 = cv2.inRange(hsv, self.lower_red2, self.upper_red2) 
        mask_red = cv2.bitwise_or(mask_red1,mask_red2)

        # Blue mask
        mask_blue = cv2.inRange(hsv, self.lower_blue, self.upper_blue) 

        red_pos, red_dist = self.find_ball(mask_red, frame, "Red") 
        blue_pos, blue_dist = self.find_ball(mask_blue, frame, "Blue") 

        if red_pos and blue_pos: 
            cv2.line(frame, (int(red_pos[0]), int(red_pos[1])),(int(blue_pos[0]), int(blue_pos[1])), (255, 0, 0), 2) # Draw line between balls
        cv2.imshow("Localization Tracker", frame) # Opens Window
        cv2.imshow("red mask",mask_red)
        cv2.imshow("blue mask",mask_blue)
        #cv2.imshow("Gaussian",blurred)
        cv2.waitKey(1) 

    def destroy_node(self): 
        self.cap.release() 
        cv2.destroyAllWindows() 
        super().destroy_node() 

def main(args=None): 
    rclpy.init(args=args) # Initializes ROS 
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