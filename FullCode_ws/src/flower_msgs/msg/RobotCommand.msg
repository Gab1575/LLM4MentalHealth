# RobotCommand.msg - The full command state for the flower robot.
#
# Published (in some combination) by flower_gui, continuum_controller_node,
# and flower_mux; the merged /flower_commands stream is what the embedded
# firmware (FullCodeMICROCONTROLLEr) actually subscribes to. See the
# FullCode_ws README for how each field is populated per topic.

float32[4] servo_angles           # Target angle per servo, degrees (-90 to 90)
float32[4] servo_time             # Commanded move duration per servo, seconds
int16 n20_pwm                     # N20 motor drive speed, 0-255
float32 n20_target_rotations      # N20 target position, rotations
bool n20_zero                     # Toggling this re-zeroes the N20 position reference
uint32[5] led_colours_hex         # Per-petal LED color, 0xRRGGBB
int16[5] led_colours_brightness   # Per-petal LED brightness, 0-255
