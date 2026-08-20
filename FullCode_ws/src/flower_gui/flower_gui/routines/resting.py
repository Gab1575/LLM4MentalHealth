"""resting.py - Calm idle routine.

A calm idle state: petals stay put at whatever position they were in when the
routine started, N20 stays off, and the LEDs sit on a slow, gentle brightness
pulse (soft white) instead of a scripted breathing pattern.

Runs on the GUI's routine worker thread (see flower_gui_node.execute_routine)
and publishes directly to /manual_commands until stop_event is set.
"""

from flower_msgs.msg import RobotCommand
import time
import math

def Resting(publisher, stop_event, initial_state):
    """Runs the resting idle state in a loop until stop_event is set.

    Args:
        publisher: ROS 2 publisher for RobotCommand, e.g. /manual_commands.
        stop_event: threading.Event; setting it ends the routine and restores
            initial_state.
        initial_state: RobotCommand snapshot of the GUI's state when the
            routine was started; servo_angles and n20_target_rotations are
            held fixed at these values for the whole routine.
    """
    print("Starting resting routine...")

    # Create ONE message object to use for the entire routine
    routine_msg = RobotCommand()

    # Baseline values that never change during the routine - petals stay exactly
    # where they were when "Resting" was pressed.
    routine_msg.servo_angles = initial_state.servo_angles
    routine_msg.n20_pwm = 0
    routine_msg.n20_target_rotations = initial_state.n20_target_rotations
    routine_msg.led_colours_hex = [0xFFFFFF] * 5
    routine_msg.servo_time[3] = 10

    start_time = time.monotonic()
    while not stop_event.is_set():
        elapsed = time.monotonic() - start_time

        # Slow (~6s period) gentle pulse between dim and soft brightness
        brightness = int(60 + 55 * (1 + math.sin(elapsed * (2 * math.pi / 6.0))))
        routine_msg.led_colours_brightness = [brightness] * 5

        # S4 flips between 20 and -20 every 10 seconds
        position = int(elapsed // 10) % 2
        routine_msg.servo_angles[3] = 20 if position == 0 else -20

        publisher.publish(routine_msg)

        if stop_event.wait(timeout=0.05):
            break

    # --- STOPPED STATE (RESTORE) ---
    print("Resting routine stopped. Restoring original GUI state...")
    publisher.publish(initial_state)
