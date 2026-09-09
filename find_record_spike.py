#!/bin/bash

# Find the recordSpike function in Neuron.cpp
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_1b1029e3-884e-454b-8f93-8f7f5a317f49

# Read the file and find recordSpike function
python3 << 'EOF'
with open('src/brain/Neuron.cpp', 'r') as f:
    content = f.read()
    
# Find the recordSpike function
import re
pattern = r'(\d+:\s*void Neuron::recordSpike\(Timestamp timestamp\)\s*\n.*?\n\}\s*\n)'
match = re.search(pattern, content, re.DOTALL)

if match:
    print("Found recordSpike function:")
    print(match.group(1))
else:
    print("recordSpike function not found")
    # Try to find it line by line
    lines = content.split('\n')
    for i, line in enumerate(lines):
        if 'void Neuron::recordSpike' in line:
            print(f"Line {i+1}: {line}")
            # Print the next 20 lines
            for j in range(i, min(i+20, len(lines))):
                print(f"{j+1}: {lines[j]}")
            break
EOF