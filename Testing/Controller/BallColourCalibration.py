import cv2
import numpy as np

def empty_callback(value):
    pass

def main():
    cap = cv2.VideoCapture(0) # Change to your camera index if needed
    
    # Create a control panel window
    cv2.namedWindow("HSV Controls")
    cv2.resizeWindow("HSV Controls", 400, 250)
    
    # Create trackbars for H, S, and V (Min and Max)
    # Hue maxes out at 179 in OpenCV, Sat/Val max at 255
    cv2.createTrackbar("Hue Min", "HSV Controls", 0, 179, empty_callback)
    cv2.createTrackbar("Hue Max", "HSV Controls", 179, 179, empty_callback)
    cv2.createTrackbar("Sat Min", "HSV Controls", 0, 255, empty_callback)
    cv2.createTrackbar("Sat Max", "HSV Controls", 255, 255, empty_callback)
    cv2.createTrackbar("Val Min", "HSV Controls", 0, 255, empty_callback)
    cv2.createTrackbar("Val Max", "HSV Controls", 255, 255, empty_callback)

    print("Adjust the sliders in the 'HSV Controls' window.")
    print("TIP: For RED, set 'Hue Min' higher than 'Hue Max' (e.g., Min 160, Max 10) to trigger wraparound.")
    print("Press 'q' to quit and print your final values.")

    while True:
        ret, frame = cap.read()
        if not ret:
            break
            
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        
        # Read the current position of all trackbars
        h_min = cv2.getTrackbarPos("Hue Min", "HSV Controls")
        h_max = cv2.getTrackbarPos("Hue Max", "HSV Controls")
        s_min = cv2.getTrackbarPos("Sat Min", "HSV Controls")
        s_max = cv2.getTrackbarPos("Sat Max", "HSV Controls")
        v_min = cv2.getTrackbarPos("Val Min", "HSV Controls")
        v_max = cv2.getTrackbarPos("Val Max", "HSV Controls")
        
        # Check if Hue wraps around (Min is greater than Max)
        if h_min <= h_max:
            # Standard single mask
            lower_bound = np.array([h_min, s_min, v_min])
            upper_bound = np.array([h_max, s_max, v_max])
            mask = cv2.inRange(hsv, lower_bound, upper_bound)
        else:
            # Wraparound double mask for colors like Red
            lower_bound1 = np.array([h_min, s_min, v_min])
            upper_bound1 = np.array([179, s_max, v_max])
            mask1 = cv2.inRange(hsv, lower_bound1, upper_bound1)
            
            lower_bound2 = np.array([0, s_min, v_min])
            upper_bound2 = np.array([h_max, s_max, v_max])
            mask2 = cv2.inRange(hsv, lower_bound2, upper_bound2)
            
            mask = cv2.bitwise_or(mask1, mask2)
        
        # Apply mask to the frame
        result = cv2.bitwise_and(frame, frame, mask=mask)
        
        # Show the windows
        cv2.imshow("Camera", frame)
        cv2.imshow("Mask", mask)
        cv2.imshow("Color Result", result)
        
        # Quit on 'q' press
        if cv2.waitKey(1) & 0xFF == ord('q'):
            print("\n--- Copy these into your ROS Node ---")
            if h_min <= h_max:
                print(f"lower_bound = np.array([{h_min}, {s_min}, {v_min}])")
                print(f"upper_bound = np.array([{h_max}, {s_max}, {v_max}])")
            else:
                print("# Wraparound Detected (e.g., Red)")
                print(f"lower_bound1 = np.array([{h_min}, {s_min}, {v_min}])")
                print(f"upper_bound1 = np.array([179, {s_max}, {v_max}])")
                print(f"lower_bound2 = np.array([0, {s_min}, {v_min}])")
                print(f"upper_bound2 = np.array([{h_max}, {s_max}, {v_max}])")
                print("mask = cv2.bitwise_or(mask1, mask2) # Make sure to merge them!")
            print("---------------------------------------")
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    main()