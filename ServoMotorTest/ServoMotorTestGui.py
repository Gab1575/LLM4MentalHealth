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
    send_command("reset")
    for i in range(4):
        sliders[i].set(0)
        current_pwms[i] = 295

# --- Build the GUI ---
root = tk.Tk()
root.geometry("450x380")

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

# Start the application
root.mainloop()