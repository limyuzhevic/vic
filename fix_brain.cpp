#!/usr/bin/env python3
import sys

file_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_cbecf828-21d5-4c17-af4f-407acbacd46e/src/brain/Brain.cpp"

# Read the file
with open(file_path, 'r') as f:
    lines = f.readlines()

# Remove duplicate COGNITION SYSTEM ACCESSORS section
# Find all COGNITION SYSTEM ACCESSORS sections
cog_indices = []
for i, line in enumerate(lines):
    if line.strip() == "// ========== COGNITION SYSTEM ACCESSORS ==========":
        cog_indices.append(i)

# Remove all but the first COGNITION SYSTEM ACCESSORS section
for i in range(len(cog_indices) - 1, 0, -1):
    start_idx = cog_indices[i]
    # Find the end of this section
    end_idx = start_idx
    for j in range(start_idx + 1, len(lines)):
        if lines[j].strip().startswith("// ==========") and j > start_idx + 10:
            end_idx = j - 1
            break
        if j > start_idx + 30:
            end_idx = j
            break
    print(f"Removing COGNITION SYSTEM ACCESSORS section at line {start_idx + 1} to {end_idx + 1}")
    del lines[start_idx:end_idx + 1]

# Remove duplicate DEVELOPMENT SYSTEM section
# Find all DEVELOPMENT SYSTEM sections
dev_indices = []
for i, line in enumerate(lines):
    if line.strip() == "// ========== DEVELOPMENT SYSTEM ==========":
        dev_indices.append(i)

# Remove all but the first DEVELOPMENT SYSTEM section
for i in range(len(dev_indices) - 1, 0, -1):
    start_idx = dev_indices[i]
    # Find the end of this section
    end_idx = start_idx
    for j in range(start_idx + 1, len(lines)):
        if lines[j].strip().startswith("// ==========") and j > start_idx + 10:
            end_idx = j - 1
            break
        if j > start_idx + 50:
            end_idx = j
            break
    print(f"Removing DEVELOPMENT SYSTEM section at line {start_idx + 1} to {end_idx + 1}")
    del lines[start_idx:end_idx + 1]

# Write the fixed content back
with open(file_path, 'w') as f:
    f.writelines(lines)

print("Successfully fixed Brain.cpp - removed duplicate code sections")