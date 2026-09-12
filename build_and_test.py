#!/usr/bin/env python3
import subprocess
import sys
import os

def build_and_test():
    # Configure build
    print("Configuring CMake build...")
    result = subprocess.run(
        ["cmake", "-S", ".", "-B", "build"],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("CMake configure failed:")
        print(result.stdout)
        print(result.stderr)
        return False
    print("CMake configuration successful")
    
    # Build
    print("Building...")
    result = subprocess.run(
        ["cmake", "--build", "build"],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("Build failed:")
        print(result.stdout)
        print(result.stderr)
        return False
    print("Build successful")
    
    # Find Python module
    pybind_dir = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ce8e060f-5529-43c9-8300-85293ba18a5a/build"
    for root, dirs, files in os.walk(pybind_dir):
        for file in files:
            if file.endswith(".so") or file.endswith(".pyd"):
                print(f"Found Python module: {os.path.join(root, file)}")
                return True
    
    print("No Python module found in build directory")
    return False

if __name__ == "__main__":
    success = build_and_test()
    sys.exit(0 if success else 1)