"""face_tracker_node.py - Publishes face pan/tilt angles

Reads a dedicated USB camera, runs RetinaFace detection each frame, picks the
largest (closest) detected face, and publishes its nose landmark's angular
offset from the frame center to /face_tracking_angles as a geometry_msgs/Point
(x = pan, y = tilt, degrees). """

import rclpy
from rclpy.node import Node
import cv2
import math
from retinaface import RetinaFace

# 1. Import the Point message
from geometry_msgs.msg import Point

class DirectFaceTracker(Node):
    def __init__(self):
        super().__init__('direct_face_tracker')

        # 2. Create the Publisher
        # Topic name is 'face_tracking_angles', queue size is 10
        self.angle_pub = self.create_publisher(Point, 'face_tracking_angles', 10)

        self.cap = cv2.VideoCapture(2)

        self.cam_width = 640
        self.cam_height = 480
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.cam_width)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.cam_height)

        # Frame center, used as the pan/tilt zero point
        self.cx = self.cam_width / 2.0
        self.cy = self.cam_height / 2.0

        # Approximate focal length (pixels) for this camera; used to convert a
        # pixel offset from center into an angular error via atan(offset/f).
        self.fx = 554.0
        self.fy = 554.0

        self.timer = self.create_timer(1.0 / 30.0, self.track_face)

        self.get_logger().info('Tracker active! Publishing to /face_tracking_angles...')

    def track_face(self):
        """Per-frame callback: detects faces, tracks the largest (closest)
        one, publishes its pan/tilt angle, and shows a debug preview window."""
        ret, frame = self.cap.read()
        if not ret:
            return

        faces = RetinaFace.detect_faces(frame)

        if isinstance(faces, dict):
            closest_face = None
            max_area = 0
            
            # Loop through all detected faces
            for key, face in faces.items():
                facial_area = face["facial_area"]
                
                # Draw a blue rectangle around EVERY detected face
                cv2.rectangle(frame, (facial_area[0], facial_area[1]), (facial_area[2], facial_area[3]), (255, 0, 0), 2)
                
                # Calculate area to find the closest one
                face_w = facial_area[2] - facial_area[0]
                face_h = facial_area[3] - facial_area[1]
                
                area = face_w * face_h
                
                if area > max_area:
                    max_area = area
                    closest_face = face
            
            # Only process tracking math and data publishing for the closest face
            if closest_face is not None:
                facial_area = closest_face["facial_area"]
                
                nose_x = closest_face["landmarks"]["nose"][0]
                nose_y = closest_face["landmarks"]["nose"][1]

                error_x = nose_x - self.cx
                error_y = self.cy - nose_y 
                
                pan_angle_rad = math.atan(error_x / self.fx)
                tilt_angle_rad = math.atan(error_y / self.fy)
                
                pan_angle_deg = math.degrees(pan_angle_rad)
                tilt_angle_deg = math.degrees(tilt_angle_rad)
                
                # 3. Create the message, populate it, and publish
                msg = Point()
                msg.x = float(pan_angle_deg)
                msg.y = float(tilt_angle_deg)
                msg.z = 0.0 # Unused, but required by the Point message structure
                
                self.angle_pub.publish(msg)
                                
                # Draw a green rectangle and text over the closest face to indicate active tracking
                score = closest_face["score"]
                cv2.rectangle(frame, (facial_area[0], facial_area[1]), (facial_area[2], facial_area[3]), (0, 255, 0), 2)
                cv2.putText(frame, f"Closest: {score * 100:.1f}%", (facial_area[0], facial_area[1] - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
                cv2.circle(frame, (int(nose_x), int(nose_y)), 5, (0, 0, 255), -1)

        cv2.imshow("RetinaFace", frame)
        cv2.waitKey(1) 

def main(args=None):
    """Entry point: initializes ROS 2, spins the tracker node, and releases
    the camera cleanly on shutdown or Ctrl+C."""
    rclpy.init(args=args)
    tracker = DirectFaceTracker()

    try:
        rclpy.spin(tracker)
    except KeyboardInterrupt:
        pass
    finally:
        tracker.cap.release()
        cv2.destroyAllWindows()
        tracker.destroy_node()
        
        if rclpy.ok():
            rclpy.shutdown()

if __name__ == '__main__':
    main()