#!/bin/bash

# Fix the config file - remove duplicate dopamine_baseline line
INPUT_FILE="/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_14799b0a-b18e-4a9b-8479-669c579847f3/configs/default.cfg"

# Read the file and fix the duplicate line
awk '$0 ~ /dopamine_baseline = / {print "dopamine_baseline = 0.1"; next} {print}' "$INPUT_FILE" > temp.cfg && mv temp.cfg "$INPUT_FILE"

echo "Config file fixed: duplicate dopamine_baseline lines removed"