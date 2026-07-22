import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider

# --- 1. Robot Parameters ---
L1 = 10.0  # Length of Stage 1
L2 = 10.0  # Length of Stage 2
d = 0.5    # Tendon offset distance from center

# --- 2. Setup Figure and 3D Axis ---
fig = plt.figure(figsize=(12, 9))
# Increase bottom margin to make room for 4 sliders and the text box
plt.subplots_adjust(left=0.1, bottom=0.45) 

ax = fig.add_subplot(111, projection='3d')
ax.set_title("2-Stage 3D Continuum Robot Controller", fontsize=14)

# Fix axis limits so the entire 2-stage workspace fits
max_reach = L1 + L2
ax.set_xlim([-max_reach, max_reach])
ax.set_ylim([-max_reach, max_reach])
ax.set_zlim([0, max_reach])
ax.set_xlabel('X Axis')
ax.set_ylabel('Y Axis')
ax.set_zlabel('Z Axis (Height)')

# Draw a base plane for reference
xx, yy = np.meshgrid([-max_reach, max_reach], [-max_reach, max_reach])
ax.plot_surface(xx, yy, np.zeros_like(xx), alpha=0.1, color='gray')

# Initialize the lines for Stage 1, Stage 2, and the tips
stage1_line, = ax.plot([], [], [], lw=6, color='teal', label='Stage 1')
stage2_line, = ax.plot([], [], [], lw=5, color='darkorange', label='Stage 2')
tip_marker1, = ax.plot([], [], [], 'bo', markersize=6)
tip_marker2, = ax.plot([], [], [], 'ro', markersize=8, label='Final Tip')
ax.legend(loc='upper left')

# --- 3. Setup GUI Sliders and Text ---
ax_th1 = plt.axes([0.25, 0.35, 0.5, 0.02])
ax_ph1 = plt.axes([0.25, 0.31, 0.5, 0.02])
ax_th2 = plt.axes([0.25, 0.25, 0.5, 0.02])
ax_ph2 = plt.axes([0.25, 0.21, 0.5, 0.02])

ax_text = plt.axes([0.1, 0.02, 0.8, 0.15])
ax_text.axis('off') 

s_th1 = Slider(ax_th1, 'Stage 1 Theta', 0.0, np.pi, valinit=np.radians(45))
s_ph1 = Slider(ax_ph1, 'Stage 1 Phi', 0.0, 2 * np.pi, valinit=np.radians(30))
s_th2 = Slider(ax_th2, 'Stage 2 Theta', 0.0, np.pi, valinit=np.radians(60))
s_ph2 = Slider(ax_ph2, 'Stage 2 Phi', 0.0, 2 * np.pi, valinit=np.radians(120))

tendon_text = ax_text.text(0.5, 0.5, '', transform=ax_text.transAxes, 
                           ha='center', va='center', fontsize=11, family='monospace',
                           bbox=dict(facecolor='lightgray', alpha=0.5, boxstyle='round,pad=1'))

# --- 4. Kinematics Helper Functions ---
def calc_segment(theta, phi, L, num_points=50):
    """Calculates the local 3D curve of a segment starting at origin."""
    s = np.linspace(0, L, num_points)
    if theta < 1e-6:
        return np.zeros_like(s), np.zeros_like(s), s
    r = L / theta
    x = r * (1 - np.cos(theta * s / L)) * np.cos(phi)
    y = r * (1 - np.cos(theta * s / L)) * np.sin(phi)
    z = r * np.sin(theta * s / L)
    return x, y, z

def get_rotation_matrix(theta, phi):
    """Calculates the rotation matrix at the tip of a segment."""
    c_p, s_p = np.cos(phi), np.sin(phi)
    c_t, s_t = np.cos(theta), np.sin(theta)
    
    # Standard constant curvature rotation: Rz(phi) * Ry(theta) * Rz(-phi)
    Rz_phi = np.array([[c_p, -s_p, 0], [s_p, c_p, 0], [0, 0, 1]])
    Ry_th = np.array([[c_t, 0, s_t], [0, 1, 0], [-s_t, 0, c_t]])
    Rz_mphi = np.array([[c_p, s_p, 0], [-s_p, c_p, 0], [0, 0, 1]])
    
    return Rz_phi @ Ry_th @ Rz_mphi

# --- 5. Main Update Function ---
def update(val):
    th1, ph1 = s_th1.val, s_ph1.val
    th2, ph2 = s_th2.val, s_ph2.val
    
    # 1. Calculate Stage 1 (Global frame)
    x1, y1, z1 = calc_segment(th1, ph1, L1)
    tip1_pos = np.array([x1[-1], y1[-1], z1[-1]])
    R1 = get_rotation_matrix(th1, ph1)
    
    # 2. Calculate Stage 2 (Local frame)
    x2_local, y2_local, z2_local = calc_segment(th2, ph2, L2)
    pts2_local = np.vstack((x2_local, y2_local, z2_local))
    
    # 3. Transform Stage 2 to Global frame (Rotate then Translate)
    pts2_global = R1 @ pts2_local + tip1_pos.reshape(3, 1)
    x2, y2, z2 = pts2_global[0, :], pts2_global[1, :], pts2_global[2, :]
    
    # Update visual lines
    stage1_line.set_data(x1, y1)
    stage1_line.set_3d_properties(z1)
    tip_marker1.set_data([x1[-1]], [y1[-1]])
    tip_marker1.set_3d_properties([z1[-1]])
    
    stage2_line.set_data(x2, y2)
    stage2_line.set_3d_properties(z2)
    tip_marker2.set_data([x2[-1]], [y2[-1]])
    tip_marker2.set_3d_properties([z2[-1]])
    
    # 4. Calculate Tendon Lengths (8 tendons total)
    # Stage 1 tendons (only affected by Stage 1)
    s1_x_bend = d * th1 * np.cos(ph1)
    s1_y_bend = d * th1 * np.sin(ph1)
    S1_x0, S1_x1 = L1 - s1_x_bend, L1 + s1_x_bend
    S1_y0, S1_y1 = L1 - s1_y_bend, L1 + s1_y_bend
    
    # Stage 2 tendons (pass through S1, so they are affected by BOTH stages)
    s2_x_bend = d * (th1 * np.cos(ph1) + th2 * np.cos(ph2))
    s2_y_bend = d * (th1 * np.sin(ph1) + th2 * np.sin(ph2))
    S2_x0, S2_x1 = (L1+L2) - s2_x_bend, (L1+L2) + s2_x_bend
    S2_y0, S2_y1 = (L1+L2) - s2_y_bend, (L1+L2) + s2_y_bend
    
    # Format and display text
    t_str = (
        f"STAGE 1 TENDONS (Length: {L1:.1f})\n"
        f"XZ(+X): {S1_x0:5.2f} | XZ(-X): {S1_x1:5.2f} | YZ(+Y): {S1_y0:5.2f} | YZ(-Y): {S1_y1:5.2f}\n"
        f"------------------------------------------------------------------------\n"
        f"STAGE 2 TENDONS (Length: {L1+L2:.1f} - Routed through Stage 1)\n"
        f"XZ(+X): {S2_x0:5.2f} | XZ(-X): {S2_x1:5.2f} | YZ(+Y): {S2_y0:5.2f} | YZ(-Y): {S2_y1:5.2f}"
    )
    tendon_text.set_text(t_str)
    fig.canvas.draw_idle()

# Link sliders and initialize
s_th1.on_changed(update)
s_ph1.on_changed(update)
s_th2.on_changed(update)
s_ph2.on_changed(update)
update(0)

plt.show()