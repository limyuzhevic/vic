#!/usr/bin/env bash
# Fix the Brain.cpp initialization

# Create a backup
cp /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_2aacbc76-8a67-4084-b7ff-645ef4a196d0/src/brain/Brain.cpp /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_2aacbc76-8a67-4084-b7ff-645ef4a196d0/src/brain/Brain.cpp.bak

# Use sed to replace the problematic section
sed -i '118,132d' /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_2aacbc76-8a67-4084-b7ff-645ef4a196d0/src/brain/Brain.cpp
