import tkinter as tk
import serial
import time

# --- Setup Serial Connection ---
# Change 'COM3' to match your Arduino's port
try:
    arduino = serial.Serial('COM3', 9600, timeout=1)
    time.sleep(2) # Give Arduino a moment to reset after connecting
    print("Connected to Arduino!")
except Exception as e:
    print(f"Error connecting to Arduino: {e}")
    arduino = None

# Track the current PWM position of each servo (Starts at SERVO_CENTER = 295)
current_pwms = [295, 295, 295, 295]

# --- Command Functions ---
def send_command(command):
    if arduino:
        full_command = command + "\n"
        arduino.write(full_command.encode('utf-8'))
        print(f"Sent: {command}")
    else:
        print(f"Test Mode: Sent -> {command}")


def move_all_servos():
    """Reads all sliders, calculates the required delta, and sends the commands."""
    for i in range(4):
        # Get the slider value in degrees (-90 to 90)
        target_deg = sliders[i].get()
        
        # Map degrees (-90 to 90) to PWM (80 to 510)
        # 0 degrees = 295 PWM. +/- 90 degrees = +/- 215 PWM.
        target_pwm = int(295 + (target_deg / 90.0) * 215)
        
        # Calculate the delta required for the Arduino code
        delta_pwm = target_pwm - current_pwms[i]
        
        # Only send a command if the servo actually needs to move
        if delta_pwm != 0:
            send_command(f"servo {i} {delta_pwm}")
            # Update our tracking list so it knows where it is for next time
            current_pwms[i] = target_pwm
            
            # Tiny delay so the Arduino Serial buffer doesn't get overwhelmed
            time.sleep(0.05) 


def reset_servos():
    """Resets the Arduino and the Python GUI back to center."""
    stop_sweep()
    send_command("reset")
    for i in range(4):
        sliders[i].set(0)
        current_pwms[i] = 295


# Sweep control state
sweep_running = False
sweep_state = {
    "all": False,
    "servo": 2,
    "lower": -45,
    "upper": 45,
    "duration_ms": 1000,
    "direction": 1,
}

# --- Sweep Helpers ---
def degrees_to_pwm(degrees):
    return int(295 + (degrees / 90.0) * 215)


def move_servo_to_degree(index, target_deg):
    target_pwm = degrees_to_pwm(target_deg)
    delta_pwm = target_pwm - current_pwms[index]
    if delta_pwm != 0:
        send_command(f"servo {index} {delta_pwm}")
        current_pwms[index] = target_pwm


def toggle_sweep_all():
    if sweep_all_var.get():
        spinbox_servo.config(state=tk.DISABLED)
    else:
        spinbox_servo.config(state=tk.NORMAL)


def start_sweep():
    global sweep_running, sweep_state
    if sweep_running:
        return

    try:
        all_servos = sweep_all_var.get()
        servo_index = int(sweep_servo_var.get()) if not all_servos else None
        lower_bound = int(entry_lower.get())
        upper_bound = int(entry_upper.get())
        duration = float(entry_time.get())
    except ValueError:
        print("Invalid sweep input. Please enter integer angles and numeric time.")
        return

    if not all_servos and (servo_index < 0 or servo_index > 3):
        print("Servo index must be 0-3.")
        return
    if lower_bound < -90 or upper_bound > 90 or lower_bound >= upper_bound:
        print("Bounds must be -90 to 90 and lower < upper.")
        return
    if duration <= 0:
        print("Time must be greater than zero.")
        return

    sweep_state["all"] = all_servos
    sweep_state["servo"] = servo_index if servo_index is not None else 0
    sweep_state["lower"] = lower_bound
    sweep_state["upper"] = upper_bound
    sweep_state["duration_ms"] = int(duration * 1000)
    sweep_state["direction"] = 1
    sweep_running = True
    btn_start_sweep.config(state=tk.DISABLED)
    btn_stop_sweep.config(state=tk.NORMAL)

    if all_servos:
        for i in range(4):
            move_servo_to_degree(i, lower_bound)
            sliders[i].set(lower_bound)
    else:
        move_servo_to_degree(servo_index, lower_bound)
        sliders[servo_index].set(lower_bound)

    root.after(sweep_state["duration_ms"], perform_sweep_step)


def perform_sweep_step():
    global sweep_running, sweep_state
    if not sweep_running:
        return

    target = sweep_state["upper"] if sweep_state["direction"] == 1 else sweep_state["lower"]
    if sweep_state["all"]:
        for i in range(4):
            move_servo_to_degree(i, target)
            sliders[i].set(target)
    else:
        servo_index = sweep_state["servo"]
        move_servo_to_degree(servo_index, target)
        sliders[servo_index].set(target)

    sweep_state["direction"] *= -1
    root.after(sweep_state["duration_ms"], perform_sweep_step)


def stop_sweep():
    global sweep_running
    sweep_running = False
    btn_start_sweep.config(state=tk.NORMAL)
    btn_stop_sweep.config(state=tk.DISABLED)

# --- Build the GUI ---
root = tk.Tk()
root.geometry("520x520")

# List to hold the slider widgets
sliders = []

# Generate 4 sliders dynamically
for i in range(4):
    frame = tk.Frame(root)
    frame.pack(pady=5)
    
    tk.Label(frame, text=f"Servo {i}: ", width=8, font=("Arial", 10)).pack(side=tk.LEFT)
    
    slider = tk.Scale(frame, from_=-90, to=90, orient=tk.HORIZONTAL, length=300, tickinterval=45)
    slider.pack(side=tk.LEFT)
    sliders.append(slider)

# Frame for the buttons
btn_frame = tk.Frame(root)
btn_frame.pack(pady=20)

# Execute button
btn_move = tk.Button(btn_frame, text="Update All Servos", width=20, bg="green", fg="white", 
                     font=("Arial", 10, "bold"), command=move_all_servos)
btn_move.pack(side=tk.LEFT, padx=10)

# Reset Button
btn_reset = tk.Button(btn_frame, text="Reset to Center", width=15, bg="red", fg="white", 
                      font=("Arial", 10, "bold"), command=reset_servos)
btn_reset.pack(side=tk.LEFT, padx=10)

# Sweep controls
sweep_frame = tk.LabelFrame(root, text="Sweep Control", padx=10, pady=10, font=("Arial", 10, "bold"))
sweep_frame.pack(pady=10, fill="x", padx=10)

sweep_servo_var = tk.StringVar(value="2")
sweep_all_var = tk.BooleanVar(value=False)
row = tk.Frame(sweep_frame)
row.pack(anchor="w", pady=5)
tk.Label(row, text="Servo index (0-3):", width=18, anchor="w").pack(side=tk.LEFT)
spinbox_servo = tk.Spinbox(row, from_=0, to=3, width=5, textvariable=sweep_servo_var)
spinbox_servo.pack(side=tk.LEFT)

row = tk.Frame(sweep_frame)
row.pack(anchor="w", pady=5)
tk.Checkbutton(row, text="Sweep all servos", variable=sweep_all_var, command=toggle_sweep_all).pack(side=tk.LEFT)

row = tk.Frame(sweep_frame)
row.pack(anchor="w", pady=5)
tk.Label(row, text="Lower bound (-90 to 90):", width=18, anchor="w").pack(side=tk.LEFT)
entry_lower = tk.Entry(row, width=6)
entry_lower.insert(0, "-45")
entry_lower.pack(side=tk.LEFT)

row = tk.Frame(sweep_frame)
row.pack(anchor="w", pady=5)
tk.Label(row, text="Upper bound (-90 to 90):", width=18, anchor="w").pack(side=tk.LEFT)
entry_upper = tk.Entry(row, width=6)
entry_upper.insert(0, "45")
entry_upper.pack(side=tk.LEFT)

row = tk.Frame(sweep_frame)
row.pack(anchor="w", pady=5)
tk.Label(row, text="Time to each target (s):", width=18, anchor="w").pack(side=tk.LEFT)
entry_time = tk.Entry(row, width=6)
entry_time.insert(0, "1.0")
entry_time.pack(side=tk.LEFT)

sweep_btn_frame = tk.Frame(sweep_frame)
sweep_btn_frame.pack(pady=10)
btn_start_sweep = tk.Button(sweep_btn_frame, text="Start Sweep", width=15, bg="#0078D7", fg="white",
                           font=("Arial", 10, "bold"), command=start_sweep)
btn_start_sweep.pack(side=tk.LEFT, padx=10)
btn_stop_sweep = tk.Button(sweep_btn_frame, text="Stop Sweep", width=15, bg="#555555", fg="white",
                          font=("Arial", 10, "bold"), command=stop_sweep, state=tk.DISABLED)
btn_stop_sweep.pack(side=tk.LEFT, padx=10)

# Start the application
root.mainloop()
