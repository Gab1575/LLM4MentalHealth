import os
import sys
import json
import time
import threading

# Force Python to look inside your workspace's colcon install directory
workspace_install_path = os.path.expanduser('~/Desktop/llm4mentalhealth/FullCode_ws/install/flower_msgs/lib/python3.10/site-packages')
if workspace_install_path not in sys.path:
    sys.path.insert(0, workspace_install_path)

import rclpy
from rclpy.node import Node
import tkinter as tk
from tkinter import colorchooser
from flower_msgs.msg import RobotCommand
from Demos.BoxBreathing import BoxBreathing

class FlowerDashboard(Node):
    def __init__(self, root):
        super().__init__('flower_gui_node')
        self.publisher = self.create_publisher(RobotCommand, '/flower_commands', 10)
        self.root = root
        self.root.title("Flower Control Dashboard")
        self.root.geometry("700x700") # Slightly taller for the new button
        
        self.config_file = os.path.expanduser('~/flower_gui_settings.json')

        # --- Variables ---
        self.servo_vars = [tk.IntVar(value=0) for _ in range(5)]
        self.led_vars = [tk.IntVar(value=0) for _ in range(5)]
        self.led_hex_vars = [tk.StringVar(value="#000000") for _ in range(5)]
        self.led_color_btns = [] 
        
        self.master_led_var = tk.IntVar(value=0)
        self.use_master_led = tk.BooleanVar(value=False)
        self.use_master_color = tk.BooleanVar(value=False)
        self.master_hex_var = tk.StringVar(value="#000000")
        
        self.n20_pos_var = tk.DoubleVar(value=0.0)
        self.n20_speed_var = tk.IntVar(value=128)
        
        # --- Flag to control who is publishing ---
        self.routine_running = False
        self.stop_event = threading.Event() 

        self.setup_ui()
        self.load_config()
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

        # Start loops
        self.root.after(10, self.ros_spin)
        self.root.after(100, self.publish_command) 
        self.root.after(1000, self.auto_save)      

    def setup_ui(self):
        # Servos
        servo_frame = tk.LabelFrame(self.root, text="Servo Motors (-90 to 90)")
        servo_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
        for i in range(5):
            tk.Scale(servo_frame, variable=self.servo_vars[i], from_=-90, to=90, 
                     orient="horizontal", label=f"Servo {i}").pack(fill="x", padx=5)

        # LEDs
        led_frame = tk.LabelFrame(self.root, text="LEDs (Brightness & Hex)")
        led_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")
        
        tk.Checkbutton(led_frame, text="Master Brightness Override", variable=self.use_master_led).pack(pady=(5,0))
        tk.Scale(led_frame, variable=self.master_led_var, from_=0, to=255, 
                 orient="horizontal", label="Master Brightness").pack(fill="x", padx=5)
        
        color_override_frame = tk.Frame(led_frame)
        color_override_frame.pack(fill="x", pady=(0, 10))
        tk.Checkbutton(color_override_frame, text="Master Colour Override", variable=self.use_master_color).pack(side="left", padx=5)
        
        # Master Color Picker Button
        self.master_color_btn = tk.Button(color_override_frame, width=8, bg="#000000",
                                          command=lambda: self.choose_color(self.master_hex_var, self.master_color_btn))
        self.master_color_btn.pack(side="right", padx=15)
        
        for i in range(5):
            row = tk.Frame(led_frame)
            row.pack(fill="x", pady=2)
            tk.Label(row, text=f"L{i}:").pack(side="left", padx=(5,0))
            tk.Scale(row, variable=self.led_vars[i], from_=0, to=255, orient="horizontal").pack(side="left", expand=True, fill="x")
            
            # Individual LED Color Picker Buttons
            btn = tk.Button(row, width=8, bg="#000000",
                            command=lambda idx=i: self.choose_color(self.led_hex_vars[idx], self.led_color_btns[idx]))
            btn.pack(side="left", padx=5)
            self.led_color_btns.append(btn)

        # N20 Motor
        n20_frame = tk.LabelFrame(self.root, text="N20 Motor")
        n20_frame.grid(row=1, column=0, columnspan=2, padx=10, pady=10, sticky="nsew")
        
        tk.Scale(n20_frame, variable=self.n20_pos_var, from_=0, to=4.5, resolution=0.1, 
                 orient="horizontal", label="Position").pack(side="left", expand=True, fill="x", padx=15)
        
        tk.Scale(n20_frame, variable=self.n20_speed_var, from_=0, to=255, 
                 orient="horizontal", label="Speed", font=("TkDefaultFont", 8), length=120).pack(side="right", padx=15)

        # Routine Button
        routine_frame = tk.Frame(self.root)
        routine_frame.grid(row=2, column=0, columnspan=2, pady=15)
        
        self.routine_btn = tk.Button(routine_frame, text="Run Box Breathing", 
                                     command=self.start_routine, fg="grey", font=("TkDefaultFont", 12, "bold"))
        self.routine_btn.pack()

    # --- Routine Methods ---

    def start_routine(self):
            if not self.routine_running:
                # --- START THE ROUTINE ---
                self.routine_running = True
                self.stop_event.clear()  # Reset the stop signal
                
                # Turn button red and change text
                self.routine_btn.config(text="Stop Routine")
                
                # Start in background thread
                threading.Thread(target=self.execute_routine, daemon=True).start()
                
            else:
                # --- STOP THE ROUTINE ---
                self.routine_running = False
                self.stop_event.set()  # Send the stop signal to the routine loop
                
                # Turn button back to green
                self.routine_btn.config(text="Run Box Breathing")

    def execute_routine(self):
            try:
                # Pass BOTH the publisher and the stop_event
                BoxBreathing(self.publisher, self.stop_event)

            except Exception as e:
                self.get_logger().error(f"Routine crashed: {e}")
                
            finally:
                # In case the routine crashes or finishes on its own, reset GUI visually
                if self.routine_running: 
                    self.routine_running = False
                    self.root.after(0, lambda: self.routine_btn.config(text="Run Box Breathing"))

    # -----------------------

    def choose_color(self, string_var, button):
        color = colorchooser.askcolor(initialcolor=string_var.get(), title="Select LED Color")
        if color[1]: 
            string_var.set(color[1])
            button.config(bg=color[1]) 

    def auto_save(self):
        self.save_config()
        self.root.after(1000, self.auto_save)

    def save_config(self):
        config_data = {
            "servos": [v.get() for v in self.servo_vars],
            "led_brightness": [v.get() for v in self.led_vars],
            "led_hex": [v.get() for v in self.led_hex_vars],
            "master_brightness": self.master_led_var.get(),
            "use_master_led": self.use_master_led.get(),
            "master_hex": self.master_hex_var.get(),
            "use_master_color": self.use_master_color.get(),
            "n20_pos": self.n20_pos_var.get(),
            "n20_speed": self.n20_speed_var.get()
        }
        try:
            with open(self.config_file, 'w') as f:
                json.dump(config_data, f, indent=4)
        except Exception as e:
            self.get_logger().error(f"Failed to save config: {e}")

    def validate_hex(self, hex_val):
        if not isinstance(hex_val, str): return "#000000"
        if not hex_val.startswith('#'): hex_val = f"#{hex_val}"
        if len(hex_val) != 7: return "#000000"
        return hex_val

    def load_config(self):
        if os.path.exists(self.config_file):
            try:
                with open(self.config_file, 'r') as f:
                    config_data = json.load(f)
                
                saved_servos = config_data.get("servos", [0]*5)
                for i in range(min(5, len(saved_servos))):
                    self.servo_vars[i].set(saved_servos[i])
                    
                saved_brightness = config_data.get("led_brightness", [0]*5)
                for i in range(min(5, len(saved_brightness))):
                    self.led_vars[i].set(saved_brightness[i])
                    
                saved_hex = config_data.get("led_hex", ["#000000"]*5)
                for i in range(min(5, len(saved_hex))):
                    val = self.validate_hex(saved_hex[i]) 
                    self.led_hex_vars[i].set(val)
                    self.led_color_btns[i].config(bg=val)
                
                self.master_led_var.set(config_data.get("master_brightness", 0))
                self.use_master_led.set(config_data.get("use_master_led", False))
                
                m_hex = self.validate_hex(config_data.get("master_hex", "#000000"))
                self.master_hex_var.set(m_hex)
                self.master_color_btn.config(bg=m_hex)
                
                self.use_master_color.set(config_data.get("use_master_color", False))
                self.n20_pos_var.set(config_data.get("n20_pos", 0.0))
                
                saved_speed = config_data.get("n20_speed", config_data.get("n20_dir", 128))
                self.n20_speed_var.set(saved_speed)
                
            except Exception as e:
                self.get_logger().error(f"Failed to load config, starting fresh. Error: {e}")

    def on_closing(self):
        self.save_config()
        self.root.destroy()

    def ros_spin(self):
        rclpy.spin_once(self, timeout_sec=0.01)
        self.root.after(10, self.ros_spin)

    def publish_command(self):
        try:
            # If the routine is running, skip publishing the GUI values
            if self.routine_running:
                return
            
            msg = RobotCommand()
            msg.servo_angles = [int(v.get()) for v in self.servo_vars]
            msg.n20_target_rotations = float(self.n20_pos_var.get())
            msg.n20_pwm = int(self.n20_speed_var.get()) 
            
            if self.use_master_color.get():
                try:
                    val = int(self.master_hex_var.get().replace('#', ''), 16)
                except ValueError:
                    val = 0
                msg.led_colours_hex = [val] * 5
            else:
                hex_values = []
                for var in self.led_hex_vars:
                    try:
                        val = int(var.get().replace('#', ''), 16)
                        hex_values.append(val)
                    except ValueError:
                        hex_values.append(0)
                msg.led_colours_hex = hex_values
            
            if self.use_master_led.get():
                msg.led_colours_brightness = [int(self.master_led_var.get())] * 5
            else:
                msg.led_colours_brightness = [int(v.get()) for v in self.led_vars]
                
            self.publisher.publish(msg)
            
        except Exception as e:
            self.get_logger().error(f"Publish crashed: {e}")
            
        finally:
            self.root.after(100, self.publish_command)

def main(args=None):
    rclpy.init(args=args)
    root = tk.Tk()
    app = FlowerDashboard(root)
    
    try:
        root.mainloop()
    except KeyboardInterrupt:
        pass
    finally:
        app.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()