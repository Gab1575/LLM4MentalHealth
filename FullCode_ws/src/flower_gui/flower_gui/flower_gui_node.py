"""flower_gui_node.py - Tkinter-based GUI.

Runs as a ROS 2 node (flower_gui_node) alongside a Tkinter main loop. Lets an
operator either drive the four stem servos and LEDs directly ("manual" mode)
or set a two-stage bend target via theta/phi sliders ("kinematic" mode, with
a live 3D preview of the resulting stem shape). Publishes:

- /manual_commands       (RobotCommand)         - full manual/base command state alonside LED and N20 settings
- /kinematic_commands    (Float64MultiArray)     - [theta1, phi1, theta2, phi2], degrees
- /control_mode          (String)                - "manual" or "kinematic"

GUI state is saved to ~/flower_gui_settings.json so the dashboard reopens
with its last configuration.
"""

import os
import sys
import json
import time
import threading
import signal
import math
import numpy as np

import rclpy
from rclpy.node import Node
import tkinter as tk
from tkinter import colorchooser

# Matplotlib integration for Tkinter
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

from flower_msgs.msg import RobotCommand
from std_msgs.msg import Float64MultiArray
from std_msgs.msg import String
from flower_gui.routines.box_breathing import BoxBreathing
from flower_gui.routines.resting import Resting
from flower_gui.routines.api import API

# --- Kinematics Helper Functions ---
# These mirror the constant-curvature bend math used by the continuum
# controller node, but purely to draw a live preview in the GUI

def calc_segment(theta, phi, L, num_points=50):
    """Samples a constant-curvature arc of length L bending by theta toward phi.
    """
    s = np.linspace(0, L, num_points)
    if theta < 1e-6:
        # Straight segment: avoid the r = L/theta singularity at theta -> 0.
        return np.zeros_like(s), np.zeros_like(s), s
    r = L / theta
    x = r * (1 - np.cos(theta * s / L)) * np.cos(phi)
    y = r * (1 - np.cos(theta * s / L)) * np.sin(phi)
    z = r * np.sin(theta * s / L)
    return x, y, z

def get_rotation_matrix(theta, phi):
    """Builds the rotation matrix mapping a second stage's local frame into
    the first stage's tip frame, given the first stage's (theta, phi) bend.

    Used to chain stage 2's arc (from calc_segment) onto stage 1's tip.
    """
    c_p, s_p = np.cos(phi), np.sin(phi)
    c_t, s_t = np.cos(theta), np.sin(theta)

    Rz_phi = np.array([[c_p, -s_p, 0], [s_p, c_p, 0], [0, 0, 1]])
    Ry_th = np.array([[c_t, 0, s_t], [0, 1, 0], [-s_t, 0, c_t]])
    Rz_mphi = np.array([[c_p, s_p, 0], [-s_p, c_p, 0], [0, 0, 1]])

    return Rz_phi @ Ry_th @ Rz_mphi

class FlowerDashboard(Node):
    """The operator dashboard: a Tkinter window driven by a ROS 2 node.

    GUI widgets/state and three independent polling loops scheduled
    via Tkinter's `after()`: ros_spin() to process ROS callbacks,
    publish_command() to publish the current command state at 10 Hz, and
    auto_save() to persist settings once a second.
    """

    def __init__(self, root):
        super().__init__('flower_gui_node')

        # ROS 2 Publishers
        self.manual_publisher = self.create_publisher(RobotCommand, '/manual_commands', 10)
        self.kinematic_publisher = self.create_publisher(Float64MultiArray, '/kinematic_commands', 10)
        self.mode_publisher = self.create_publisher(String, '/control_mode', 10)
        
        self.root = root
        self.root.title("Flower Control Dashboard & Kinematics")
        self.root.geometry("950x800") 
        
        self.config_file = os.path.expanduser('~/flower_gui_settings.json')

        # --- Robot Parameters (for Drawing) ---
        self.L1 = 10.0
        self.L2 = 10.0

        # --- Variables ---
        self.control_mode = tk.StringVar(value="manual")
        self.control_mode.trace_add('write', self.on_mode_changed)
        self.servo_vars = [tk.DoubleVar(value=0.0) for _ in range(4)]
        self.servo_time_vars = [tk.StringVar(value="0.0") for _ in range(4)]
        self.led_vars = [tk.IntVar(value=0) for _ in range(5)]
        self.led_hex_vars = [tk.StringVar(value="#000000") for _ in range(5)]
        self.led_color_btns = [] 
        
        self.master_led_var = tk.IntVar(value=0)
        self.use_master_led = tk.BooleanVar(value=False)
        self.use_master_color = tk.BooleanVar(value=False)
        self.master_hex_var = tk.StringVar(value="#000000")
        
        self.n20_pos_var = tk.DoubleVar(value=0.0)
        self.n20_speed_var = tk.IntVar(value=128)
        self.n20_zero_var = tk.BooleanVar(value=False)

        # Kinematic Variables
        self.th1_var = tk.DoubleVar(value=45.0)  
        self.ph1_var = tk.DoubleVar(value=30.0)
        self.th2_var = tk.DoubleVar(value=60.0)
        self.ph2_var = tk.DoubleVar(value=120.0)
        self.kin_time_var = tk.StringVar(value="0.5") 
        
        self.routine_running = False
        self.active_routine_btn = None
        self.stop_event = threading.Event()

        self.setup_ui()
        self.load_config()
        self.update_kinematics()

        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

        # Start loops
        self.on_mode_changed()
        self.root.after(10, self.ros_spin)          # Pump ROS callbacks
        self.root.after(100, self.publish_command)  # Publish current state at 10 Hz
        self.root.after(1000, self.auto_save)       # Persist settings once a second

    def setup_ui(self):
        """Builds every Tkinter widget: the mode selector, the kinematics
        preview/sliders on the left, and the manual servo/LED/N20 controls
        plus routine buttons on the right."""
        # --- Mode Selector (Top) ---
        mode_frame = tk.LabelFrame(self.root, text="Publishing Mode Control", font=("TkDefaultFont", 10, "bold"), fg="blue")
        mode_frame.pack(fill="x", padx=2, pady=2)
        
        tk.Radiobutton(mode_frame, text="Publish Servos to /manual_commands", 
                       variable=self.control_mode, value="manual", font=("TkDefaultFont", 9)).pack(side="left", padx=5, pady=0)
                       
        tk.Radiobutton(mode_frame, text="Publish Theta & Phi to /kinematic_commands", 
                       variable=self.control_mode, value="kinematic", font=("TkDefaultFont", 9)).pack(side="left", padx=5, pady=0)

        # Create Main Layout Frames
        main_frame = tk.Frame(self.root)
        main_frame.pack(fill="both", expand=True)

        left_frame = tk.Frame(main_frame)
        left_frame.pack(side="left", fill="both", expand=True, padx=2, pady=2)
        
        right_frame = tk.Frame(main_frame)
        right_frame.pack(side="right", fill="y", padx=2, pady=2)

        # ================= LEFT FRAME (KINEMATICS) =================
        kin_frame = tk.LabelFrame(left_frame)
        kin_frame.pack(fill="both", expand=True)

        self.fig = Figure(figsize=(3, 3), dpi=90)
        self.fig.subplots_adjust(left=0, right=1, bottom=0, top=1)
        self.ax = self.fig.add_subplot(111, projection='3d')
        max_reach = self.L1 + self.L2
        self.ax.set_xlim([-max_reach, max_reach])
        self.ax.set_ylim([-max_reach, max_reach])
        self.ax.set_zlim([0, max_reach])
        self.ax.set_xlabel('X', labelpad=-10)
        self.ax.set_ylabel('Y', labelpad=-10)
        self.ax.set_zlabel('Z', labelpad=-10)
        self.ax.tick_params(axis='both', which='major', labelsize=7, pad=-5)

        xx, yy = np.meshgrid([-max_reach, max_reach], [-max_reach, max_reach])
        self.ax.plot_surface(xx, yy, np.zeros_like(xx), alpha=0.1, color='gray')

        self.stage1_line, = self.ax.plot([], [], [], lw=4, color='teal', label='Stage 1')
        self.stage2_line, = self.ax.plot([], [], [], lw=4, color='darkorange', label='Stage 2')
        self.tip_marker1, = self.ax.plot([], [], [], 'bo', markersize=4)
        self.tip_marker2, = self.ax.plot([], [], [], 'ro', markersize=6, label='Final Tip')
        self.ax.legend(loc='upper left', fontsize=7)

        self.canvas = FigureCanvasTkAgg(self.fig, master=kin_frame)
        self.canvas.get_tk_widget().pack(fill="both", expand=True, padx=2, pady=2)

        slider_frame = tk.Frame(kin_frame)
        slider_frame.pack(fill="x", pady=2)
        
        tk.Scale(slider_frame, variable=self.th1_var, from_=0, to=180, orient="horizontal", label="Stg 1 Theta", width=10, sliderlength=15, command=self.update_kinematics).grid(row=0, column=0, sticky="ew", padx=2)
        tk.Scale(slider_frame, variable=self.ph1_var, from_=0, to=360, orient="horizontal", label="Stg 1 Phi", width=10, sliderlength=15, command=self.update_kinematics).grid(row=0, column=1, sticky="ew", padx=2)
        tk.Scale(slider_frame, variable=self.th2_var, from_=0, to=180, orient="horizontal", label="Stg 2 Theta", width=10, sliderlength=15, command=self.update_kinematics).grid(row=1, column=0, sticky="ew", padx=2)
        tk.Scale(slider_frame, variable=self.ph2_var, from_=0, to=360, orient="horizontal", label="Stg 2 Phi", width=10, sliderlength=15, command=self.update_kinematics).grid(row=1, column=1, sticky="ew", padx=2)
        
        slider_frame.columnconfigure(0, weight=1)
        slider_frame.columnconfigure(1, weight=1)

        time_frame_kin = tk.Frame(kin_frame)
        time_frame_kin.pack(pady=2)
        tk.Label(time_frame_kin, text="Move Time (s):", font=("TkDefaultFont", 9)).pack(side="left")
        tk.Entry(time_frame_kin, textvariable=self.kin_time_var, width=5).pack(side="left", padx=2)

        # ================= RIGHT FRAME (HARDWARE) =================
        servo_frame = tk.LabelFrame(right_frame, text="Servos (-90 to 90)", font=("TkDefaultFont", 9))
        servo_frame.pack(fill="x", padx=2, pady=0)
        for i in range(4):
            row = tk.Frame(servo_frame)
            row.pack(fill="x", padx=2, pady=0)

            tk.Scale(row, variable=self.servo_vars[i], from_=-90.0, to=90.0,
                     orient="horizontal", label=f"S{i + 1}", width=10, sliderlength=15).pack(side="left", expand=True, fill="x")
            
            time_frame = tk.Frame(row)
            time_frame.pack(side="right", padx=(2, 2))
            tk.Label(time_frame, text="Time:", font=("TkDefaultFont", 8)).pack(side="left")
            tk.Entry(time_frame, textvariable=self.servo_time_vars[i], width=4).pack(side="left")

        led_frame = tk.LabelFrame(right_frame, text="LEDs", font=("TkDefaultFont", 9))
        led_frame.pack(fill="x", padx=2, pady=0)
        
        master_row = tk.Frame(led_frame)
        master_row.pack(fill="x")
        tk.Checkbutton(master_row, text="Master Bright", variable=self.use_master_led, font=("TkDefaultFont", 8)).pack(side="left")
        tk.Scale(master_row, variable=self.master_led_var, from_=0, to=255, orient="horizontal", width=10, sliderlength=15).pack(side="left", expand=True, fill="x", padx=2)
        
        color_override_frame = tk.Frame(led_frame)
        color_override_frame.pack(fill="x")
        tk.Checkbutton(color_override_frame, text="Master Color", variable=self.use_master_color, font=("TkDefaultFont", 8)).pack(side="left")
        
        self.master_color_btn = tk.Button(color_override_frame, width=4, bg="#000000",
                                          command=lambda: self.choose_color(self.master_hex_var, self.master_color_btn))
        self.master_color_btn.pack(side="right", padx=5)
        
        for i in range(5):
            row = tk.Frame(led_frame)
            row.pack(fill="x", pady=0)
            tk.Label(row, text=f"L{i}:", font=("TkDefaultFont", 8)).pack(side="left")
            tk.Scale(row, variable=self.led_vars[i], from_=0, to=255, orient="horizontal", width=10, sliderlength=15).pack(side="left", expand=True, fill="x", padx=2)
            
            btn = tk.Button(row, width=4, bg="#000000",
                            command=lambda idx=i: self.choose_color(self.led_hex_vars[idx], self.led_color_btns[idx]))
            btn.pack(side="right", padx=2)
            self.led_color_btns.append(btn)

        n20_frame = tk.LabelFrame(right_frame, text="N20", font=("TkDefaultFont", 9))
        n20_frame.pack(fill="x", padx=2, pady=2)
        
        n20_row = tk.Frame(n20_frame)
        n20_row.pack(fill="x")
        tk.Scale(n20_row, variable=self.n20_pos_var, from_=0, to=4.5, resolution=0.1, 
                 orient="horizontal", label="Pos", width=10, sliderlength=15).pack(side="left", expand=True, fill="x", padx=2)
        tk.Scale(n20_row, variable=self.n20_speed_var, from_=0, to=255, 
                 orient="horizontal", label="Spd", font=("TkDefaultFont", 8), width=10, sliderlength=15).pack(side="left", expand=True, fill="x", padx=2)
        
        tk.Button(n20_frame, text="Zero", command=self.toggle_n20_zero, height=1, bd=1, relief="sunken").pack(fill="x", padx=2, pady=2)
        
        routine_frame = tk.Frame(right_frame)
        routine_frame.pack(pady=2)
        
        self.routine_btn = tk.Button(routine_frame, text="Run Box Breathing",
                                     command=lambda: self.start_routine(BoxBreathing, "Box Breathing", self.routine_btn),
                                     fg="grey", font=("TkDefaultFont", 10, "bold"))
        self.routine_btn.pack()

        self.resting_btn = tk.Button(routine_frame, text="Run Resting",
                                     command=lambda: self.start_routine(Resting, "Resting", self.resting_btn),
                                     fg="grey", font=("TkDefaultFont", 10, "bold"))
        self.resting_btn.pack()

        self.api_btn = tk.Button(routine_frame, text="Run API",
                                 command=lambda: self.start_routine(API, "API", self.api_btn),
                                 fg="grey", font=("TkDefaultFont", 10, "bold"))
        self.api_btn.pack()

    def toggle_n20_zero(self):
        """Toggles the N20 zero flag when the Zero button is pressed."""
        self.n20_zero_var.set(not self.n20_zero_var.get())
        self.get_logger().info(f"N20 zeroed")

    def on_mode_changed(self, *args):
        """Publishes the current control_mode ("manual"/"kinematic") to
        /control_mode. Bound as a trace on self.control_mode, so this fires
        automatically whenever the operator flips the mode radio buttons."""
        msg = String()
        msg.data = self.control_mode.get()
        self.mode_publisher.publish(msg)
        self.get_logger().info(f"Published mode change: {msg.data}")

    def update_kinematics(self, *args):
        """Redraws the 3D stem preview from the current theta/phi sliders."""

        th1 = np.radians(self.th1_var.get())
        ph1 = np.radians(self.ph1_var.get())
        th2 = np.radians(self.th2_var.get())
        ph2 = np.radians(self.ph2_var.get())
        
        x1, y1, z1 = calc_segment(th1, ph1, self.L1)
        tip1_pos = np.array([x1[-1], y1[-1], z1[-1]])
        R1 = get_rotation_matrix(th1, ph1)
        
        x2_local, y2_local, z2_local = calc_segment(th2, ph2, self.L2)
        pts2_local = np.vstack((x2_local, y2_local, z2_local))
        
        pts2_global = R1 @ pts2_local + tip1_pos.reshape(3, 1)
        x2, y2, z2 = pts2_global[0, :], pts2_global[1, :], pts2_global[2, :]
        
        self.stage1_line.set_data(x1, y1)
        self.stage1_line.set_3d_properties(z1)
        self.tip_marker1.set_data([x1[-1]], [y1[-1]])
        self.tip_marker1.set_3d_properties([z1[-1]])
        
        self.stage2_line.set_data(x2, y2)
        self.stage2_line.set_3d_properties(z2)
        self.tip_marker2.set_data([x2[-1]], [y2[-1]])
        self.tip_marker2.set_3d_properties([z2[-1]])
        self.canvas.draw_idle()

    def start_routine(self, routine_func, label, btn):
        """Starts or stops a guided routine (e.g. box breathing) on its button.

        A routine runs on its own daemon thread so it can publish on a tight
        timer without blocking the Tkinter main loop. Pressing the active
        routine's button again requests a stop via stop_event"""
        if not self.routine_running:
            self.routine_running = True
            self.active_routine_btn = btn
            self.stop_event.clear()
            btn.config(text="Stop Routine")
            threading.Thread(target=self.execute_routine, args=(routine_func, label, btn), daemon=True).start()
        elif btn is self.active_routine_btn:
            self.routine_running = False
            self.stop_event.set()
            btn.config(text=f"Run {label}")
        # else: a different routine is already running - ignore this button until it stops

    def execute_routine(self, routine_func, label, btn):
        """Runs one routine to completion (or until stopped) on the worker
        thread started by start_routine(). Snapshots the current GUI command
        state first so the routine can restore it when it exits, and resets
        the button label back on the Tkinter thread via root.after()."""
        try:
            initial_state = self.get_current_command()
            routine_func(self.manual_publisher, self.stop_event, initial_state, self.get_logger())
        except Exception as e:
            self.get_logger().error(f"Routine crashed: {e}")
        finally:
            if self.routine_running:
                self.routine_running = False
                self.root.after(0, lambda: btn.config(text=f"Run {label}"))

    def choose_color(self, string_var, button):
        """Opens the OS color picker and, if a color was chosen, updates the
        given StringVar (hex string) and button swatch to match."""
        color = colorchooser.askcolor(initialcolor=string_var.get(), title="Select LED Color")
        if color[1]:
            string_var.set(color[1])
            button.config(bg=color[1])

    def safe_float(self, val_str):
        """Parses val_str as a float, returning 0.0 instead of raising if the
        operator has typed something invalid into a time/duration entry."""
        try:
            return float(val_str)
        except ValueError:
            return 0.0

    def auto_save(self):
        """Persists GUI settings and reschedules itself for one second later."""
        self.save_config()
        self.root.after(1000, self.auto_save)

    def save_config(self):
        """Serializes all persisted GUI state to self.config_file as JSON."""
        config_data = {
            "mode": self.control_mode.get(),
            "servos": [v.get() for v in self.servo_vars],
            "servo_times": [self.safe_float(v.get()) for v in self.servo_time_vars],
            "led_brightness": [v.get() for v in self.led_vars],
            "led_hex": [v.get() for v in self.led_hex_vars],
            "master_brightness": self.master_led_var.get(),
            "use_master_led": self.use_master_led.get(),
            "master_hex": self.master_hex_var.get(),
            "use_master_color": self.use_master_color.get(),
            "n20_pos": self.n20_pos_var.get(),
            "n20_speed": self.n20_speed_var.get(),
            "n20_zero": self.n20_zero_var.get(),
            "th1": self.th1_var.get(),
            "ph1": self.ph1_var.get(),
            "th2": self.th2_var.get(),
            "ph2": self.ph2_var.get(),
            "kin_time": self.safe_float(self.kin_time_var.get())
        }
        try:
            with open(self.config_file, 'w') as f:
                json.dump(config_data, f, indent=4)
        except Exception as e:
            self.get_logger().error(f"Failed to save config: {e}")

    def validate_hex(self, hex_val):
        """Normalizes a loaded LED color value, falling back to black (off)
        for anything that isn't a well-formed "#RRGGBB" string"""
        if not isinstance(hex_val, str): return "#000000"
        if not hex_val.startswith('#'): hex_val = f"#{hex_val}"
        if len(hex_val) != 7: return "#000000"
        return hex_val

    def load_config(self):
        """Restores GUI state from self.config_file if it exists, falling
        back to each widget's built-in default for any field that's missing"""
        if os.path.exists(self.config_file):
            try:
                with open(self.config_file, 'r') as f:
                    config_data = json.load(f)
                
                self.control_mode.set(config_data.get("mode", "manual"))

                saved_servos = config_data.get("servos", [0.0]*4)
                for i in range(min(4, len(saved_servos))):
                    self.servo_vars[i].set(saved_servos[i])

                saved_times = config_data.get("servo_times", [0.0]*4)
                for i in range(min(4, len(saved_times))):
                    self.servo_time_vars[i].set(str(saved_times[i]))
                    
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
                self.n20_speed_var.set(config_data.get("n20_speed", 128))
                self.n20_zero_var.set(config_data.get("n20_zero", False))

                self.th1_var.set(config_data.get("th1", self.th1_var.get()))
                self.ph1_var.set(config_data.get("ph1", self.ph1_var.get()))
                self.th2_var.set(config_data.get("th2", self.th2_var.get()))
                self.ph2_var.set(config_data.get("ph2", self.ph2_var.get()))
                self.kin_time_var.set(str(config_data.get("kin_time", self.safe_float(self.kin_time_var.get()))))

            except Exception as e:
                self.get_logger().error(f"Failed to load config, starting fresh: {e}")

    def on_closing(self):
        """Window-close handler: saves settings one last time before quitting he Tkinter main loop."""
        self.save_config()
        self.root.quit()

    def ros_spin(self):
        """Processes any pending ROS 2 callbacks"""

        if not rclpy.ok():
            self.root.quit()
            return
        rclpy.spin_once(self, timeout_sec=0.01)
        self.root.after(10, self.ros_spin)

    def get_current_command(self):
        """Builds a RobotCommand from the current widget state: servo angles/
        times, N20 target/speed/zero, and LED colors/brightness"""
        msg = RobotCommand()
        msg.servo_angles = [float(v.get()) for v in self.servo_vars]
        msg.servo_time = [self.safe_float(v.get()) for v in self.servo_time_vars]
        msg.n20_target_rotations = float(self.n20_pos_var.get())
        msg.n20_pwm = int(self.n20_speed_var.get()) 
        msg.n20_zero = bool(self.n20_zero_var.get())
        
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
            
        return msg

    def publish_command(self):
        """Publishes the current command state at ~10 Hz"""
        if not rclpy.ok():
            return
        try:
            if not self.routine_running:
                
                # Fetch full base command (includes manual servos, LEDs, N20 data, and individual times)
                cmd_msg = self.get_current_command()
                
                # Override the time array if we are in kinematic mode
                if self.control_mode.get() == "kinematic":
                    kin_time = self.safe_float(self.kin_time_var.get())
                    cmd_msg.servo_time = [kin_time] * 4
                
                # Unconditionally publish to /manual_commands so the Mux has the latest Time/N20/LED states
                self.manual_publisher.publish(cmd_msg)
                
                # Unconditionally publish kinematics so you can still slide them and test them
                kin_msg = Float64MultiArray()
                kin_msg.data = [
                    float(self.th1_var.get()),
                    float(self.ph1_var.get()),
                    float(self.th2_var.get()),
                    float(self.ph2_var.get())
                ]
                self.kinematic_publisher.publish(kin_msg)
                
        except Exception as e:
            self.get_logger().error(f"Publish crashed: {e}")
        finally:
            if rclpy.ok():
                self.root.after(100, self.publish_command)

def main(args=None):
    """Entry point: initializes ROS 2 and Tkinter, then runs the GUI main
    loop. SIGINT handler so Ctrl+C cleanly tears down the ROS node
    and the Tkinter window instead of leaving either dangling."""
    rclpy.init(args=args)
    root = tk.Tk()
    app = FlowerDashboard(root)

    def handle_sigint(sig, frame):
        if rclpy.ok():
            try:
                app.destroy_node()
                rclpy.shutdown()
            except Exception:
                pass
        try:
            root.destroy()
        except Exception:
            pass
        sys.exit(0)

    signal.signal(signal.SIGINT, handle_sigint)
    root.mainloop()

if __name__ == '__main__':
    main()