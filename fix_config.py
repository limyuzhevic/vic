#!/usr/bin/env python3

import os

config_path = '/workspace/app/configs/default.cfg'

# Read the entire file content
with open(config_path, 'r') as f:
    content = f.read()

# Remove duplicate dopamine_baseline line by replacing the problematic section
old_section = """# Neuromodulation settings
dopamine_baseline = 0.0
dopamine_baseline = 0.1

# Reward settings"""

new_section = """# Neuromodulation settings
dopamine_baseline = 0.0

# Reward settings"""

# Replace the old section with the new one
fixed_content = content.replace(old_section, new_section)

# Write the fixed content back to the file
with open(config_path, 'w') as f:
    f.write(fixed_content)

print("Successfully fixed duplicate dopamine_baseline configuration")
print("Removed duplicate dopamine_baseline = 0.1 line")
print("Kept dopamine_baseline = 0.0 line")