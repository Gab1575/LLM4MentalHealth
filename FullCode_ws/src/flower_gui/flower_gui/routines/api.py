"""api.py - Extensible "API" routine: a scaffold for plugging in a
decision-maker that chooses what the flower does moment to
moment."""

from flower_msgs.msg import RobotCommand
import time
import math


# ---------------------------------------------------------------------------
# ACTION LIBRARY
# ---------------------------------------------------------------------------
# Each function takes (routine_msg, elapsed, initial_state)
# routine_msg is the single RobotCommand object that will be published
# elapsed is how long this action will run (seconds)
# initial_state is a snapshot of all states prior to the routine being started. 
# It can be used to restore or adjust based off current position.

"""ADD ACTIONS!!!! ... oh .. and make sure to put them in the ACTIONS dictionary

    POSSIBLE COMMANDS FIELDS:
    - servo_angles[]            List of angles (0-180) for each motor (1 to 4)                                                float32[4] 
    - servo_time[]              List of times associated with how long it takes for the selected servo to move (seconds)      float32[4]
    - n20_pwm                   PWM speed for petals opening and closing (0-255)                                              int16
    - n20_target_rotations      Target position for petals (0.0 (Closed) - 4.5 (Open))                                        float32
    - N20_zero                  Sets the current petal position to the current target position                                bool   
    - led_colours_hex[]         List of 5 hex colours associated with each LED (0x000000-0xFFFFFF)                            uint32[5]
    - led_colours_brightness[]  List of 5 brightness values associated with each LED (0-255)                                  int16[5]


"""

def _idle(routine_msg, elapsed, initial_state):
    """Soft white breathing pulse, petals held still"""
    routine_msg.led_colours_hex = [0xFFFFFF] * 5
    brightness = int(60 + 55 * (1 + math.sin(elapsed * (2 * math.pi / 6.0))))
    routine_msg.led_colours_brightness = [max(0, min(255, brightness))] * 5


def _pulse_blue(routine_msg, elapsed, initial_state):
    """Calm blue pulse"""
    routine_msg.led_colours_hex = [0x2050FF] * 5
    brightness = int(60 + 90 * (1 + math.sin(elapsed * (2 * math.pi / 3.0))))
    routine_msg.led_colours_brightness = [max(0, min(255, brightness))] * 5


def _pulse_red(routine_msg, elapsed, initial_state):
    """Warmer, faster pulse"""
    routine_msg.led_colours_hex = [0xFF3030] * 5
    brightness = int(80 + 100 * (1 + math.sin(elapsed * (2 * math.pi / 1.5))))
    routine_msg.led_colours_brightness = [max(0, min(255, brightness))] * 5


def _open_petals(routine_msg, elapsed, initial_state):
    """Opens the petals at max speed and then holds open."""
    routine_msg.n20PWM = 255
    routine_msg.n20_target_rotations = 4.5


def _close_petals(routine_msg, elapsed, initial_state):
    """Closes the petals at max speed and then holds closed."""
    routine_msg.n20PWM = 255
    routine_msg.n20_target_rotations = 0


# Keys in ACTIONS must match the action_name returned by decide_next_action() 
ACTIONS = {
    "idle": _idle,
    "pulse_blue": _pulse_blue,
    "pulse_red": _pulse_red,
    "open_petals": _open_petals,
    "close_petals": _close_petals,
}


# ---------------------------------------------------------------------------
def decide_next_action(context):
    """Chooses the next action name + how long to run it.

    This code MUST:
      - Return (action_name, duration_seconds).
      - Return an action_name that is a key in ACTIONS
      - Stay bounded in time

    Returns:
        (action_name: str, duration_seconds: float)
    """

    # --- EXAMPLE of what implementation might look like -----------
    #
    # import anthropic
    # client = context["llm_client"]  # create once, outside the loop, and
    #                                 # pass it in via context
    # try:
    #     response = client.messages.create(
    #         model="claude-sonnet-5",
    #         max_tokens=16,
    #         timeout=3.0,
    #         messages=[{
    #             "role": "user",
    #             "content": (
    #                 f"Session so far: {context['history']}. "
    #                 f"Pick exactly one of {list(ACTIONS)} for what the "
    #                 f"flower should do next and reply with just that word."
    #             ),
    #         }],
    #     )
    #     choice = response.content[0].text.strip()
    #     if choice not in ACTIONS:
    #         choice = "idle"
    #     return choice, 5.0
    # except Exception:
    #     return "idle", 5.0
    # -----------------------------------------------------------------------

    return "idle", 5.0


def API(publisher, stop_event, initial_state, logger):
    """Runs the API routine loop until stop_event is set.

    Repeatedly asks decide_next_action() what to do and for how long, then
    runs the action for the duration or until stop_event
    """
    logger.info("Starting API routine loop...")

    routine_msg = RobotCommand()
    routine_msg.servo_angles = initial_state.servo_angles.copy()
    routine_msg.servo_time = initial_state.servo_time.copy()
    routine_msg.n20_pwm = 0
    routine_msg.n20_target_rotations = initial_state.n20_target_rotations

    context = {"step": 0, "history": []}

    while not stop_event.is_set():
        action_name, duration = decide_next_action(context)
        if action_name not in ACTIONS:
            logger.warning(f"decide_next_action() returned unknown action {action_name!r}, using 'idle'")
            action_name = "idle"
        step_fn = ACTIONS[action_name]

        logger.info(f"API routine: running '{action_name}' for {duration:.1f}s")
        context["history"].append((action_name, duration))
        context["step"] += 1

        start_time = time.monotonic()
        while time.monotonic() - start_time < duration:
            elapsed = time.monotonic() - start_time
            step_fn(routine_msg, elapsed, initial_state)
            publisher.publish(routine_msg)

            # break out for stop
            if stop_event.wait(timeout=0.05):
                break

    # --- STOPPED STATE (RESTORE) ---
    logger.info("API routine stopped. Restoring original GUI state...")
    publisher.publish(initial_state)
