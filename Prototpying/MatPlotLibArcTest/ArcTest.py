import matplotlib.pyplot as plt
from matplotlib.patches import Arc

fig, ax = plt.subplots()

# Create the arc
# xy=(0,0), width=2, height=2, theta1=0 (start), theta2=90 (end)
my_arc = Arc((0, 0), 2, 2, theta1=0, theta2=20, color='blue', lw=2)

ax.add_patch(my_arc)

# Adjust axes to see the arc properly
ax.set_xlim(-1.5, 1.5)
ax.set_ylim(-1.5, 1.5)
ax.set_aspect('equal')

plt.show()
