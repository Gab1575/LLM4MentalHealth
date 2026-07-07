import math

# --- Physical Robot Constants ---
S = 80.0       # Backbone length in mm (adjust to your hardware)
r = 7.5        # Routing radius in mm (distance from center to tendon)
alpha = [0, 2*math.pi/3, 4*math.pi/3] # Tendon locations (0, 120, 240 degrees in radians)

# Piecewise constants from the paper [cite: 470, 552, 553]
k = [0.4887, 0.4232, 0.3040]
b = [0, 0.0343, 0.1592]

def calculate_tendon_changes(p_current, phi_current, p_target, phi_target):
    
    # Calculate differentials 
    dp = p_target - p_current
    dphi = phi_target - phi_current
    
    # Handle quadrant wrap-around for smooth rotation [cite: 569-571]
    if dphi < -math.pi:
        dphi += 2 * math.pi
    elif dphi > math.pi:
        dphi -= 2 * math.pi
        
    # 2. Determine Piecewise Subset based on current bend ratio (p/S) [cite: 552-554]
    ratio = p_current / S
    if ratio < 0.2559:
        subset = 0
    elif ratio < 0.4775:
        subset = 1
    else:
        subset = 2
        
    k_j = k[subset]
    b_j = b[subset]
    
    # 3. Calculate Cable Displacements using the Master Equation 
    d_delta_L = [0.0, 0.0, 0.0]
    
    for i in range(3):
        term1 = -(math.cos(alpha[i] - phi_current) / S) * dp
        term2 = ((p_current / S) - b_j) * math.sin(alpha[i] - phi_current) * dphi
        
        # Calculate raw change in cable length (mm)
        raw_change = (r / k_j) * (term1 + term2)
        
        # 4. Open-Loop Tension Supervision (The Bias Hack)
        # In the paper, a positive d_delta_L means releasing cable[cite: 582].
        if raw_change > 0:
            # We are releasing cable (passive tendon). 
            # Multiply by a bias < 1.0 so it under-releases slightly, 
            # keeping tension against the active pulling tendon.
            d_delta_L[i] = raw_change * 0.90 
        else:
            # We are pulling cable (active tendon). Execute exactly.
            d_delta_L[i] = raw_change

    # Return the required length changes for the 3 tendons
    return d_delta_L