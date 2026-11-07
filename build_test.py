import subprocess
import sys
import os

# Try to build using pip install
print("Building pynlm using pip install...")
try:
    # First, let's see what's in the current directory
    result = subprocess.run(["ls", "-la", "/workspace/app"], capture_output=True, text=True)
    print("Files in /workspace/app:", result.stdout)
    
    # Try to install with pip in development mode
    print("\nAttempting pip install -e . in /workspace/app...")
    result = subprocess.run(["pip3", "install", "-e", "."], capture_output=True, text=True, cwd="/workspace/app")
    print("pip install output:", result.stdout)
    if result.stderr:
        print("pip install stderr:", result.stderr)
    
    # Try to import pynlm
    print("\nAttempting to import pynlm...")
    result = subprocess.run(["python3", "-c", "import pynlm; print('Successfully imported pynlm'); print(dir(pynlm))"], 
                          capture_output=True, text=True)
    print("Import result:", result.stdout)
    if result.stderr:
        print("Import stderr:", result.stderr)
    
except Exception as e:
    print(f"Error: {e}")
