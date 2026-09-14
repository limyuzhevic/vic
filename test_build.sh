#!/usr/bin/env bash
# Quick test to check if the codebase builds and runs

cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_4f7f4f77-7560-467c-b125-9134932df059

# Check for build directory
if [ -d "build" ]; then
    echo "Build directory exists"
    cd build
    
    # Check if executable exists
    if [ -f "nlm" ]; then
        echo "Executable 'nlm' exists"
        ./nlm
        echo "Build successful!"
    else
        echo "Executable 'nlm' not found"
        ls -la
    fi
else
    echo "Build directory does not exist"
    ls -la
fi