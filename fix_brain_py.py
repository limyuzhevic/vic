#!/usr/bin/env python3
import sys

file_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_cbecf828-21d5-4c17-af4f-407acbacd46e/src/brain/Brain.cpp"

# Read the file
with open(file_path, 'r') as f:
    content = f.read()

# First, let's remove the duplicate COGNITION SYSTEM ACCESSORS section
# We'll use a simpler approach - find the pattern and replace it

# Find the problematic section that starts around line 1046
# This section includes:
# // ========== COGNITION SYSTEM ACCESSORS ==========
# NeuralPlanner* Brain::getPlanner() { ... }
# ConceptFormation* Brain::getConceptFormation() { ... }
# AttentionalSelection* Brain::getAttention() { ... }
# // ========== DEVELOPMENT SYSTEM ==========
# DevelopmentSystem* Brain::getDevelopmentSystem() { ... }
# DevelopmentalStage Brain::getDevelopmentalStage() const { ... }
# void Brain::setDevelopmentalStage(DevelopmentalStage stage) { ... }
# // ========== NEUROMODULATION SYSTEMS ==========
# ... (rest of the section)
# Then another COGNITION SYSTEM ACCESSORS section which is the one we want to remove

# Let's split by lines and process
lines = content.split('\n')

# Find the first COGNITION SYSTEM ACCESSORS
first_cognition_idx = -1
for i, line in enumerate(lines):
    if "// ========== COGNITION SYSTEM ACCESSORS" in line:
        first_cognition_idx = i
        break

if first_cognition_idx != -1:
    # Find the second COGNITION SYSTEM ACCESSORS (the one after DEVELOPMENT SYSTEM)
    second_cognition_idx = -1
    for i in range(first_cognition_idx + 1, len(lines)):
        if "// ========== COGNITION SYSTEM ACCESSORS" in line:
            second_cognition_idx = i
            break
    
    if second_cognition_idx != -1:
        # Now we need to remove everything from second_cognition_idx up to
        # the line before "// ========== NEUROMODULATION SYSTEMS" that appears
        # after the duplicate COGNITION section
        
        # First, let's find where the duplicate DEVELOPMENT SYSTEM starts
        dev_start = -1
        for i in range(second_cognition_idx + 1, len(lines)):
            if "// ========== DEVELOPMENT SYSTEM" in lines[i]:
                dev_start = i
                break
        
        if dev_start != -1:
            # Find where NEUROMODULATION SYSTEMS starts
            neuromod_start = -1
            for i in range(dev_start, len(lines)):
                if "// ========== NEUROMODULATION SYSTEMS" in lines[i]:
                    neuromod_start = i
                    break
            
            if neuromod_start != -1:
                # Remove everything from second_cognition_idx to neuromod_start-1
                # But we need to keep the NEUROMODULATION SYSTEMS section
                del lines[second_cognition_idx:neuromod_start]
                print(f"Removed duplicate COGNITION SYSTEM ACCESSORS section (lines {second_cognition_idx+1} to {neuromod_start})")

# Write the fixed content back
with open(file_path, 'w') as f:
    f.write('\n'.join(lines))

print("Fixed Brain.cpp - removed duplicate code sections")