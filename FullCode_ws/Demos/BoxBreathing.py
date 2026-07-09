from flower_msgs.msg import RobotCommand
import time

def BoxBreathing(publisher, stop_event, initial_state):
    duration = 4.0
    print("Starting routine loop...")

    # Create ONE message object to use for the entire routine
    routine_msg = RobotCommand()
    
    # Set the baseline values that NEVER change during the routine
    routine_msg.n20_pwm = 185
    routine_msg.servo_angles = initial_state.servo_angles
    routine_msg.led_colours_brightness = [255] * 5

    # --- Get into Starting Position ---
    routine_msg.n20_target_rotations = 4.0
    routine_msg.led_colours_hex = [0xFF0000] * 5
    publisher.publish(routine_msg)

    # Keep looping as long as the stop_event has not been triggered
    while not stop_event.is_set():
        
        # ==========================================
        # STEP 1: EXHALE
        # ==========================================
        print("Exhaling...")
        brightness = 255.0
        routine_msg.led_colours_brightness = [int(brightness)] * 5
        routine_msg.n20_target_rotations = 0.0
        routine_msg.led_colours_hex = [0x0000FF] * 5 
        routine_msg.n20_pwm = 185
        publisher.publish(routine_msg)
        
        start_time = time.monotonic() # Reset timer for Exhale phase
        while time.monotonic() - start_time < duration:
            if stop_event.wait(timeout=0.1):
                break
        
        if stop_event.is_set():
            break
        
        # ==========================================
        # STEP 2: HOLD (Empty)
        # ==========================================
        print("Holding...")
        routine_msg.n20_pwm = 0
        publisher.publish(routine_msg)
        
        start_time = time.monotonic() # Reset timer for Hold phase
        while time.monotonic() - start_time < duration:
            # Fixed the underscore and cast brightness to an integer
            routine_msg.led_colours_brightness = [int(max(0, brightness))] * 5
            publisher.publish(routine_msg)
            brightness -= 6.375

            if stop_event.wait(timeout=0.1):
                break
        
        if stop_event.is_set():
            break

        # ==========================================
        # STEP 3: INHALE
        # ==========================================
        print("Inhaling...")
        brightness = 255.0
        routine_msg.led_colours_brightness = [int(brightness)] * 5
        routine_msg.n20_pwm = 185
        routine_msg.n20_target_rotations = 4.0
        routine_msg.led_colours_hex = [0xFF0000] * 5  
        publisher.publish(routine_msg)
        
        start_time = time.monotonic() # Reset timer for Inhale phase
        while time.monotonic() - start_time < duration:
            if stop_event.wait(timeout=0.1):
                break 
        
        if stop_event.is_set():
            break
        
        # ==========================================
        # STEP 4: HOLD (Full)
        # ==========================================
        print("Holding...")
        routine_msg.n20_pwm = 0 # Assuming you want the motor to stop while holding
        publisher.publish(routine_msg)
        
        start_time = time.monotonic() # Reset timer for Hold phase
        while time.monotonic() - start_time < duration:
            # Fixed the underscore and cast brightness to an integer
            routine_msg.led_colours_brightness = [int(max(0, brightness))] * 5
            publisher.publish(routine_msg)
            brightness -= 6.375

            if stop_event.wait(timeout=0.1):
                break
        
        if stop_event.is_set():
            break

    # --- STOPPED STATE (RESTORE) ---
    print("Routine stopped. Restoring original GUI state...")
    publisher.publish(initial_state)