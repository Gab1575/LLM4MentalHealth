import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

# Animation settings
TOTAL_LENGTH = 10.0
MAX_BEND_ANGLE_DEG_POS = 75
MAX_BEND_ANGLE_DEG_NEG = -75
FRAMES = 90             # number of animation frames for one sweep
INTERVAL_MS = 30        # interval between frames in milliseconds
POINTS = 200            # number of points to define the arc for smoothness
VERTEBRA_INTERVAL = 1.0 # distance between vertebrae along the rod
VERTEBRA_HALF_WIDTH = 0.75

# Set up the figure and axis
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=4)
vertebrae_line, = ax.plot([], [], lw=2, color='tab:gray')
string_line_a, = ax.plot([], [], lw=1, color='tab:blue', ls='--')
string_line_b, = ax.plot([], [], lw=1, color='tab:orange', ls='--')
length_text = ax.text(0.02, 0.98, '', transform=ax.transAxes, va='top', ha='left', fontsize=10)
ax.set_aspect('equal', 'box')

# Adjusted Y-limits to allow for the negative bend
ax.set_xlim(-TOTAL_LENGTH * 1.1, TOTAL_LENGTH * 1.1)
ax.set_ylim(-TOTAL_LENGTH * 0.1, TOTAL_LENGTH * 1.1)
ax.grid(True)
ax.set_xlabel('x')
ax.set_ylabel('y')

# Function to compute arc points based on total length and bend angle
def compute_arc(total_length, bend_angle_deg):
    theta = np.radians(bend_angle_deg)
    if np.isclose(theta, 0.0):  # Straight line case
        y = np.array([0.0, total_length])
        x = np.array([0.0, 0.0])
    else:
        R = total_length / theta    # Calculate radius to preserve total length
        t = np.linspace(0.0, theta, POINTS) # Generate points along the curve
        y = R * np.sin(t)      # y-coordinates of the arc
        x = R * (1.0 - np.cos(t))   # x-coordinates of the arc
    return x, y

def compute_vertebrae(total_length, bend_angle_deg, interval, half_width):
    theta = np.radians(bend_angle_deg)
    if np.isclose(theta, 0.0):
        s_values = np.arange(0.0, total_length + 1e-6, interval)
        x_verts = np.zeros_like(s_values)
        y_verts = s_values
        orientations = np.zeros_like(s_values)
    else:
        R = total_length / theta
        s_values = np.arange(0.0, total_length + 1e-6, interval)
        t_values = s_values / R
        x_verts = R * (1.0 - np.cos(t_values))
        y_verts = R * np.sin(t_values)
        orientations = -t_values

    xs = []
    ys = []
    string_a_x = []
    string_a_y = []
    string_b_x = []
    string_b_y = []
    string_offset = half_width * 0.8
    for x_center, y_center, orientation in zip(x_verts, y_verts, orientations):
        dx = half_width * np.cos(orientation)
        dy = half_width * np.sin(orientation)
        xs.extend([x_center - dx, x_center + dx, np.nan])
        ys.extend([y_center - dy, y_center + dy, np.nan])

        dx_s = string_offset * np.cos(orientation)
        dy_s = string_offset * np.sin(orientation)
        string_a_x.append(x_center - dx_s)
        string_a_y.append(y_center - dy_s)
        string_b_x.append(x_center + dx_s)
        string_b_y.append(y_center + dy_s)

    return (
        np.array(xs),
        np.array(ys),
        np.array(string_a_x),
        np.array(string_a_y),
        np.array(string_b_x),
        np.array(string_b_y),
    )


def compute_polyline_length(x, y):
    return np.sum(np.hypot(np.diff(x), np.diff(y)))

# Initialization function for the animation
def init():
    line.set_data([], [])
    vertebrae_line.set_data([], [])
    string_line_a.set_data([], [])
    string_line_b.set_data([], [])
    length_text.set_text('')
    return (line, vertebrae_line, string_line_a, string_line_b, length_text)

sweep_forward = np.linspace(MAX_BEND_ANGLE_DEG_NEG, MAX_BEND_ANGLE_DEG_POS, FRAMES)
sweep_backward = sweep_forward[::-1]    
angles = np.concatenate([sweep_forward, sweep_backward]) # Create a full sweep from negative to positive and back to negative

def update(angle):
    x, y = compute_arc(TOTAL_LENGTH, angle)
    line.set_data(x, y)
    xv, yv, xa, ya, xb, yb = compute_vertebrae(TOTAL_LENGTH, angle, VERTEBRA_INTERVAL, VERTEBRA_HALF_WIDTH)
    vertebrae_line.set_data(xv, yv)
    string_line_a.set_data(xa, ya)
    string_line_b.set_data(xb, yb)

    length_a = compute_polyline_length(xa, ya)
    length_b = compute_polyline_length(xb, yb)
    length_text.set_text(
        f'String A length: {length_a:.2f}\nString B length: {length_b:.2f}'
    )
    return (line, vertebrae_line, string_line_a, string_line_b, length_text)

ani = animation.FuncAnimation(
    fig,
    func=update,
    frames=angles, 
    init_func=init,
    interval=INTERVAL_MS,
    blit=True,
    repeat=True,
)

plt.show()