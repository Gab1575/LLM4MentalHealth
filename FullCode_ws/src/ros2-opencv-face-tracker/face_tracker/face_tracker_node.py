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
        
        self.cap = cv2.VideoCapture(0)
        
        self.cam_width = 640
        self.cam_height = 480
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.cam_width)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.cam_height)
        
        self.cx = self.cam_width / 2.0
        self.cy = self.cam_height / 2.0
        
        self.fx = 554.0 
        self.fy = 554.0
        
        self.timer = self.create_timer(1.0 / 30.0, self.track_face)
        
        self.get_logger().info('Tracker active! Publishing to /face_tracking_angles...')

    def track_face(self):
        ret, frame = self.cap.read()
        if not ret:
            return
            
        faces = RetinaFace.detect_faces(frame)

        if isinstance(faces, dict):
            closest_face = None
            max_area = 0
            
            for key, face in faces.items():
                facial_area = face["facial_area"]
                face_w = facial_area[2] - facial_area[0]
                face_h = facial_area[3] - facial_area[1]
                
                area = face_w * face_h
                
                if area > max_area:
                    max_area = area
                    closest_face = face
            
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
                                
                score = closest_face["score"]
                cv2.rectangle(frame, (facial_area[0], facial_area[1]), (facial_area[2], facial_area[3]), (0, 255, 0), 2)
                cv2.putText(frame, f"Closest: {score * 100:.1f}%", (facial_area[0], facial_area[1] - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
                cv2.circle(frame, (int(nose_x), int(nose_y)), 5, (0, 0, 255), -1)

        cv2.imshow("RetinaFace", frame)
        cv2.waitKey(1) 

def main(args=None):
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