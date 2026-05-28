import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

def plot_arc_rod(total_length, bend_angle_deg):
    """
    Plots a rod bending continuously as an arc.
    """
    theta = np.radians(bend_angle_deg)
    
    # If the angle is 0, it's just a straight line
    if np.isclose(theta, 0):
        x = [0, total_length]
        y = [0, 0]
    else:
        # Calculate radius to preserve total length
        R = total_length / theta
        
        # Generate points along the curve
        t = np.linspace(0, theta, 100)
        x = R * np.sin(t)
        y = R * (1 - np.cos(t))
        
    plt.plot(x, y, linewidth=4)
    plt.axis('equal')
    plt.grid(True)

for bend_angle in [60]:
    plt.figure()
    plot_arc_rod(total_length=10, bend_angle_deg=bend_angle)
    plt.title(f'Bend Angle: {bend_angle} degrees')
    plt.show()
