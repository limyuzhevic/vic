#!/usr/bin/env python3
import sys

file_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_cbecf828-21d5-4c17-af4f-407acbacd46e/src/brain/Brain.cpp"

with open(file_path, 'r') as f:
    content = f.read()

# Remove the duplicate COGNITION SYSTEM ACCESSORS section (lines 1046-1058 in current file)
# We'll look for the pattern
lines = content.split('\n')

# Find the start of the duplicate COGNITION SYSTEM ACCESSORS section
start_idx = -1
for i, line in enumerate(lines):
    if line.strip() == "// ========== COGNITION SYSTEM ACCESSORS ==========" and i > 1040:
        start_idx = i
        break

# Find the end of this section (right before "// ========== DEVELOPMENT SYSTEM ==========" or another section)
end_idx = -1
if start_idx != -1:
    for i in range(start_idx + 1, len(lines)):
        if lines[i].strip().startswith("// ==============") and i > start_idx + 5:
            end_idx = i - 1
            break
        if i > start_idx + 20:
            end_idx = i
            break

if start_idx != -1 and end_idx != -1:
    print(f"Removing duplicate section from line {start_idx+1} to {end_idx+1}")
    # Remove the duplicate section
    del lines[start_idx:end_idx + 1]
    
    # Also need to check if there are duplicate DEVELOPMENT SYSTEM sections
    # Find all DEVELOPMENT SYSTEM sections
    dev_starts = []
    for i, line in enumerate(lines):
        if line.strip() == "// ========== DEVELOPMENT SYSTEM ==========":
            dev_starts.append(i)
    
    # If we have more than one DEVELOPMENT SYSTEM section, remove all but the first one
    if len(dev_starts) > 1:
        print(f"Found {len(dev_starts)} DEVELOPMENT SYSTEM sections, keeping only the first")
        # Keep the first one, remove subsequent ones
        # We'll work backwards to not mess up indices
        for i in range(len(dev_starts) - 1, 0, -1):
            start = dev_starts[i]
            # Find the end of this section
            end = start
            for j in range(start + 1, len(lines)):
                if lines[j].strip().startswith("// ==========") and j > start + 5:
                    end = j - 1
                    break
                if j > start + 30:
                    end = j
                    break
            # Remove from start to end
            del lines[start:end + 1]
            # Update dev_starts list for next iteration
            dev_starts = [x if x < start else x - (end - start + 1) for x in dev_starts if x != start]

# Write back to file
with open(file_path, 'w') as f:
    f.write('\n'.join(lines))

print("Fixed Brain.cpp - removed duplicate code sections")