import rclpy 
from rclpy.node import Node 
import cv2 
import numpy as np 

KNOWN_DIAMETER = 7.0 # The physical diameter of the ball we are tracking, in centimeters.
FOCAL_LENGTH = 615.0 # The calibrated focal length of the camera lens, measured in pixels.

class LocalizationTrackerNode(Node): 
    def __init__(self): 
        super().__init__('localization_tracker') # Initializes the parent Node class and names this node 'localization_tracker'.
        self.get_logger().info('Localization Tracker Node Started') 
        
        self.cap = cv2.VideoCapture(0)  # Opens a connection to the default webcam (index 0). WILL NEED TO CHANGE THIS TO THE CORRECT CAMERA INDEX OR PATH.
        
# HSV threshold parameters for STRICT bright colors
        
# Bright Red - Widened Hue to allow slight orange/pink, decreased min Saturation and Value for shadows
        self.lower_red1 = np.array([0, 150, 70])   # Lowered min Saturation to 150, min Value to 70
        self.upper_red1 = np.array([12, 255, 255]) # Widened max Hue from 8 to 12 (touches orange)
        self.lower_red2 = np.array([165, 150, 70]) # Widened min Hue from 172 to 165 (touches pink)
        self.upper_red2 = np.array([180, 255, 255]) 

        # Bright Blue - Widened Hue to allow slight cyan/purple, lowered min Sat and Val
        self.lower_blue = np.array([100, 150, 70]) # Hue shifted slightly towards cyan. Lowered min Sat to 150, Val to 70
        self.upper_blue = np.array([135, 255, 255]) # Hue shifted slightly towards purple.

        # Timer loop running at 30 FPS
        self.timer = self.create_timer(1.0 / 30.0, self.process_frame)

    def find_ball(self, mask, frame, color_name):
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) # Finds the outlines (contours) of all white blobs in the binary mask.
        if contours: # Checks if any contours were actually found in the image.
            c = max(contours, key=cv2.contourArea) # Grabs the largest contour by area, assuming the largest blob is our target ball.
            if cv2.contourArea(c) > 300: # Checks if the largest contour is big enough to be a ball (ignores small background noise).
                ((x, y), radius) = cv2.minEnclosingCircle(c) # Calculates the center coordinates (x, y) and radius of the smallest circle that fits around the contour.
                if radius > 10: # Ensures the radius is large enough to be considered a valid detection.
                    apparent_width = radius * 2 # Calculates the perceived width of the ball in pixels.
                    distance = (KNOWN_DIAMETER * FOCAL_LENGTH) / apparent_width # Uses the pinhole camera equation to calculate the distance to the ball in cm.
                    
                    cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 0), 2) # Draws a green circle outlining the detected ball on the original camera frame.
                    cv2.putText(frame, f"{color_name}: {distance:.1f}cm", # Formats a text string displaying the color and calculated distance to 1 decimal place.
                                (int(x) - 40, int(y) - int(radius) - 10), # Sets the (x, y) coordinates for where the text should be drawn above the ball.
                                cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2) # Draws the text using a specific font, scale, white color, and thickness.
                    return (x, y), distance # Returns the (x, y) center position and the calculated distance back to the caller.
        return None, None # Returns empty values if no ball was found or if it failed the size checks.

    def process_frame(self): # The main callback method that processes the camera feed 30 times a second.
        ret, frame = self.cap.read() # Grabs the latest image frame from the webcam. 'ret' is a boolean true/false indicating success.
        if not ret: # Checks if the frame failed to capture.
            return # Exits the function early if there's no frame, preventing errors.

        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV) # Converts the image from standard BGR (Blue-Green-Red) to HSV (Hue-Saturation-Value), which is much better for isolating colors.

        # Red mask
        mask_red1 = cv2.inRange(hsv, self.lower_red1, self.upper_red1) # Creates a black-and-white image where pixels within the first red range are white.
        mask_red2 = cv2.inRange(hsv, self.lower_red2, self.upper_red2) # Creates a black-and-white image where pixels within the second red range are white.
        mask_red = cv2.bitwise_or(mask_red1, mask_red2) # Merges the two red masks together to capture all shades of red.

        # Blue mask
        mask_blue = cv2.inRange(hsv, self.lower_blue, self.upper_blue) # Creates a black-and-white image where pixels within the blue range are white.

        red_pos, red_dist = self.find_ball(mask_red, frame, "Red") # Passes the red mask to the detection method and saves the resulting position and distance.
        blue_pos, blue_dist = self.find_ball(mask_blue, frame, "Blue") # Passes the blue mask to the detection method and saves the resulting position and distance.

        if red_pos and blue_pos: # Checks if BOTH the red ball and the blue ball are currently visible on screen.
            cv2.line(frame, (int(red_pos[0]), int(red_pos[1])), # Starts drawing a line from the center of the red ball...
                     (int(blue_pos[0]), int(blue_pos[1])), (255, 0, 0), 2) # ...to the center of the blue ball, colors it blue, and makes it 2 pixels thick.

        cv2.imshow("Localization Tracker", frame) # Opens a graphical window on your computer to display the live, annotated video frame.
        cv2.waitKey(1) # Pauses execution for 1 millisecond to give OpenCV time to refresh the graphical window.

    def destroy_node(self): # Overrides the default node shutdown behavior to clean up resources first.
        self.cap.release() # Turns off the webcam so other applications can use it again.
        cv2.destroyAllWindows() # Force-closes the graphical video window opened by cv2.imshow.
        super().destroy_node() # Calls the parent class's standard shutdown sequence to safely close the ROS node.

def main(args=None): # Defines the main entry point where the Python script actually starts executing.
    rclpy.init(args=args) # Initializes the ROS 2 communications network.
    node = LocalizationTrackerNode() # Creates a live instance of our custom tracking node class.
    try: # Starts a block of code that allows us to catch user interruptions cleanly.
        rclpy.spin(node) # Keeps the node alive, telling it to continuously check for timer events and run callbacks indefinitely.
    except KeyboardInterrupt: # Listens for the user pressing Ctrl+C in the terminal to stop the program.
        pass # If Ctrl+C is pressed, it does nothing here, allowing the code to naturally drop down into the 'finally' block.
    finally: 
        node.destroy_node()
        rclpy.shutdown() 

if __name__ == '__main__': 
    main() 