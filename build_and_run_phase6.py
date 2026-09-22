#!/usr/bin/env python3
"""
Build and run script for NLM Phase 6
Simplified build process focusing on Python bindings and integration fixes
"""

import os
import subprocess
import sys

def run_command(cmd, cwd=None):
    print(f"Running: {cmd}")
    print(f"In directory: {cwd or os.getcwd()}")
    result = subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error (exit code {result.returncode}):")
        print(result.stderr)
        return False
    if result.stdout:
        print(result.stdout)
    return True

def main():
    print("=== NLM Phase 6 Integration Build and Run ===\n")
    
    # Check if we're in the right directory
    if not os.path.exists("CMakeLists.txt"):
        print("Error: CMakeLists.txt not found. Please run from NLM root directory.")
        return 1
    
    # Step 1: Build C++ core with Phase 6 integration
    print("Step 1: Building C++ core with Phase 6 integration...")
    if not run_command("mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release"):
        return 1
    
    if not run_command("cd build && make -j4"):
        return 1
    
    # Step 2: Build Python bindings
    print("\nStep 2: Building Python bindings...")
    if not run_command("cd .. && pip install -e ."):
        return 1
    
    # Step 3: Run Phase 6 demo
    print("\nStep 3: Running Phase 6 demo...")
    if not run_command("cd build && ./nlm_phase6_demo"):
        return 1
    
    print("\n=== Build and run completed successfully! ===")
    return 0

if __name__ == "__main__":
    sys.exit(main())
