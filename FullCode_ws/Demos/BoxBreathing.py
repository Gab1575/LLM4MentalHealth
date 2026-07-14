from flower_msgs.msg import RobotCommand
import time

"""
Breathing Routine: Box Breathing
    -> 4 Seconds Exhale 
        Flower petals opening and LED in blue brightness decreasing 
    -> 4 Seconds Hold (Empty)
        Flower petals stay open and LED's fill up around the petals one by one in red
    -> 4 Seconds Inhale
        Flower petals closing and LED in red brightness increasing red
    -> 4 Seconds Hold (Full)
        Flower petals stay closed and LED's fill up around the petals one by one in blue
"""

def BoxBreathing(publisher, stop_event, initial_state):
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
        # STEP 1: EXHALE (4s Fade Down Blue + Open Petals)
        # ==========================================
        print("Exhaling...")
        brightness = 255.0
        routine_msg.led_colours_brightness = [int(brightness)] * 5
        routine_msg.n20_target_rotations = 0.0
        routine_msg.led_colours_hex = [0x0000FF] * 5 
        routine_msg.n20_pwm = 185
        publisher.publish(routine_msg)
        
        start_time = time.monotonic() 
        while time.monotonic() - start_time < 4.0:
            routine_msg.led_colours_brightness = [int(max(0, brightness))] * 5
            routine_msg.servo_angles[4] += 0.4
            routine_msg.servo_angles[1] += 0.25
            publisher.publish(routine_msg)
            brightness -= (6.375/2) # Decreases over ~80 iterations (4s at 0.05s timeout)


            #LED flutter/heartbeat effect
            elapsed = time.monotonic() - start_time
            if (0.9 < elapsed < 1.0) or (1.9 < elapsed < 2.0) or (2.9 < elapsed < 3.0) or (3.9 < elapsed < 4.0):
                currentbrightness = brightness 
                routine_msg.led_colours_brightness = [int(min(currentbrightness + 30, 255))] * 5
                publisher.publish(routine_msg)

            if stop_event.wait(timeout=0.05):
                break
        
        if stop_event.is_set(): break
        
        # ==========================================
        # STEP 2: HOLD (Empty) (4s Fill up sequentially in Red)
        # ==========================================
        print("Holding (Empty)...")
        routine_msg.n20_pwm = 0
        routine_msg.led_colours_hex = [0x000000] * 5
        publisher.publish(routine_msg)
        
        start_time = time.monotonic() 
        while time.monotonic() - start_time < 4.0:
            elapsed = time.monotonic() - start_time
            
            # Corrected logic: Check smallest times first, NO breaks
            if elapsed < 0.8:
                routine_msg.led_colours_brightness = [255, 0, 0, 0, 0]
            elif elapsed < 1.6:
                routine_msg.led_colours_brightness = [255, 255, 0, 0, 0]
            elif elapsed < 2.4:
                routine_msg.led_colours_brightness = [255, 255, 255, 0, 0]
            elif elapsed < 3.2:
                routine_msg.led_colours_brightness = [255, 255, 255, 255, 0]
            else:
                routine_msg.led_colours_brightness = [255, 255, 255, 255, 255]
            
            routine_msg.led_colours_hex = [0xFF0000] * 5
            publisher.publish(routine_msg)

            if stop_event.wait(timeout=0.1): # 0.1s wait prevents CPU maxing
                break
        
        if stop_event.is_set(): break

        # ==========================================
        # STEP 3: INHALE (4s Fade Up Red + Close Petals)
        # ==========================================
        print("Inhaling...")
        brightness = 0.0 
        routine_msg.led_colours_brightness = [int(brightness)] * 5
        routine_msg.n20_pwm = 185
        routine_msg.n20_target_rotations = 4.0
        routine_msg.led_colours_hex = [0xFF0000] * 5  
        publisher.publish(routine_msg)
        
        start_time = time.monotonic() 
        while time.monotonic() - start_time < 4.0:
            routine_msg.led_colours_brightness = [int(max(0, brightness))] * 5
            routine_msg.servo_angles[4] -= 0.4
            routine_msg.servo_angles[1] -= 0.25

            publisher.publish(routine_msg)
            brightness += (6.375/2) # Decreases over ~80 iterations (4s at 0.05s timeout)

            # LED flutter/heartbeat effect
            elapsed = time.monotonic() - start_time
            if (0.9 < elapsed < 1.0) or (1.9 < elapsed < 2.0) or (2.9 < elapsed < 3.0) or (3.9 < elapsed < 4.0):
                currentbrightness = brightness 
                routine_msg.led_colours_brightness = [int(min(currentbrightness - 30, 255))] * 5
                publisher.publish(routine_msg)

            if stop_event.wait(timeout=0.05):
                break
        
        if stop_event.is_set(): break
        
        # ==========================================
        # STEP 4: HOLD (Full) (4s Fill up sequentially in Blue)
        # ==========================================
        print("Holding (Full)...")
        routine_msg.n20_pwm = 0 
        routine_msg.led_colours_hex = [0x000000] * 5 # Switch to blue
        publisher.publish(routine_msg)
        
        start_time = time.monotonic() 
        while time.monotonic() - start_time < 4.0:
            elapsed = time.monotonic() - start_time
            
            # Same sequential logic as Step 2, but in Blue
            if elapsed < 0.8:
                routine_msg.led_colours_brightness = [0, 0, 0, 0, 255]
            elif elapsed < 1.6:
                routine_msg.led_colours_brightness = [0, 0, 0, 255, 255]
            elif elapsed < 2.4:
                routine_msg.led_colours_brightness = [0, 0, 255, 255, 255]
            elif elapsed < 3.2:
                routine_msg.led_colours_brightness = [0, 255, 255, 255, 255]
            else:
                routine_msg.led_colours_brightness = [255, 255, 255, 255, 255]
                
            publisher.publish(routine_msg)
            routine_msg.led_colours_hex = [0x0000FF] * 5 # Switch to blue

            if stop_event.wait(timeout=0.1):
                break
        
        if stop_event.is_set(): break

    # --- STOPPED STATE (RESTORE) ---
    print("Routine stopped. Restoring original GUI state...")
    publisher.publish(initial_state)