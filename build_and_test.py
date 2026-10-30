#!/usr/bin/env python3
"""
Build and test script for NLM project
Compiles the C++ code and runs tests
"""

import subprocess
import sys
import os
import json

def run_command(cmd, cwd=None):
    """Run a command and return success status"""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"ERROR: Command failed: {cmd}")
            print(f"STDOUT: {result.stdout}")
            print(f"STDERR: {result.stderr}")
            return False
        else:
            print(f"SUCCESS: {result.stdout}")
            return True
    except Exception as e:
        print(f"EXCEPTION: {e}")
        return False

def main():
    print("=== NLM Build and Test Script ===")
    
    # Change to workspace directory
    workspace_dir = "/workspace/app"
    if not os.path.exists(workspace_dir):
        print(f"ERROR: Workspace directory {workspace_dir} does not exist")
        return 1
    
    os.chdir(workspace_dir)
    
    # Check if CMake is available
    if not run_command("cmake --version"):
        print("ERROR: CMake not found. Please install CMake.")
        return 1
    
    # Check if compiler is available
    if not run_command("g++ --version"):
        print("ERROR: g++ not found. Please install g++.")
        return 1
    
    # Create build directory
    if not os.path.exists("build"):
        print("Creating build directory...")
        os.makedirs("build")
    
    # Configure CMake
    print("\n=== Configuring CMake ===")
    cmake_cmd = "cmake .. -DCMAKE_BUILD_TYPE=Release"
    if not run_command(cmake_cmd, "build"):
        print("ERROR: CMake configuration failed")
        return 1
    
    # Build the project
    print("\n=== Building project ===")
    build_cmd = "make -j4"
    if not run_command(build_cmd, "build"):
        print("ERROR: Build failed")
        return 1
    
    # Check if binaries were created
    binaries = ["nlm", "nlm_phase3_demo", "nlm_phase4_demo"]
    found_binaries = []
    
    for binary in binaries:
        binary_path = f"build/{binary}"
        if os.path.exists(binary_path):
            found_binaries.append(binary)
            print(f"✓ Found binary: {binary_path}")
        else:
            print(f"✗ Binary not found: {binary_path}")
    
    if not found_binaries:
        print("ERROR: No binaries were created")
        return 1
    
    # Run the main demo
    if "nlm" in found_binaries:
        print("\n=== Running main demo ===")
        demo_cmd = f"cd build && ./nlm"
        if not run_command(demo_cmd):
            print("WARNING: Main demo failed, but binaries were created")
    
    # Run tests if available
    test_dir = "tests"
    if os.path.exists(test_dir):
        print("\n=== Running tests ===")
        # Look for test executables
        test_files = ["test_neuron", "test_synapse", "test_stdp", "test_config", 
                     "test_random", "test_clock", "test_brain", "test_main", "test_types"]
        
        for test in test_files:
            test_path = f"build/{test}"
            if os.path.exists(test_path):
                print(f"\nRunning {test}...")
                if not run_command(f"cd build && ./{test}"):
                    print(f"WARNING: {test} failed")
            else:
                print(f"  Test not found: {test_path}")
    
    print("\n=== Build and test complete ===")
    print("✓ Binaries created successfully")
    print(f"✓ Available binaries: {', '.join(found_binaries)}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
