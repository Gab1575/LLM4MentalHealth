from flower_msgs.msg import RobotCommand

def BoxBreathing(publisher, stop_event):
    """
    Loops a sequence of movements and colors until stopped.
    """
    print("Starting routine loop...")
    
    # Keep looping as long as the stop_event has not been triggered
    while not stop_event.is_set():
        
        # --- STEP 1 ---
        msg1 = RobotCommand()
        msg1.servo_angles = [90, 90, 90, 90, 90]
        msg1.n20_pwm = 200
        msg1.n20_target_rotations = 2.0
        msg1.led_colours_hex = [0xFF0000] * 5  # All Red
        msg1.led_colours_brightness = [255] * 5
        
        publisher.publish(msg1)
        
        # Wait 2 seconds. If the stop event is triggered during this wait, it breaks instantly.
        if stop_event.wait(timeout=2.0):
            break 

        # --- STEP 2 ---
        msg2 = RobotCommand()
        msg2.servo_angles = [-90, -90, -90, -90, -90]
        msg2.n20_pwm = 150
        msg2.n20_target_rotations = 0.0
        msg2.led_colours_hex = [0x0000FF] * 5  # All Blue
        msg2.led_colours_brightness = [100] * 5
        
        publisher.publish(msg2)
        
        # Wait 2 seconds again.
        if stop_event.wait(timeout=2.0):
            break

    # --- STOPPED STATE ---
    # Optional: Send a neutral command when the routine is turned off
    print("Routine stopped. Returning control to GUI...")
    msg3 = RobotCommand()
    msg3.servo_angles = [0, 0, 0, 0, 0]
    msg3.n20_pwm = 0
    msg3.n20_target_rotations = 0.0
    msg3.led_colours_hex = [0x000000] * 5
    msg3.led_colours_brightness = [0] * 5
    publisher.publish(msg3)