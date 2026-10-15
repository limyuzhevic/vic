#!/usr/bin/env python3
"""
Build script for NLM Python bindings.

This script provides convenient build and test commands for the NLM Python bindings.
"""

import subprocess
import sys
import os

def run_command(cmd, cwd=None):
    """Run a command and return the result."""
    print(f"Running: {cmd}")
    result = subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True)
    
    if result.returncode != 0:
        print(f"Error: {result.stderr}")
        return False
    
    print(result.stdout)
    return True

def build():
    """Build the Python bindings."""
    print("Building NLM Python bindings...")
    
    # Check if we have the required tools
    if not run_command("which cmake"):
        print("CMake not found. Please install CMake.")
        return False
    
    if not run_command("which python3"):
        print("Python3 not found. Please install Python3.")
        return False
    
    # Create build directory
    if not run_command("rm -rf build && mkdir build && cd build"):
        return False
    
    # Configure with CMake
    if not run_command("cmake .. -DCMAKE_BUILD_TYPE=Release"):
        return False
    
    # Build
    if not run_command("make -j$(nproc)"):
        return False
    
    # Install
    if not run_command("make install"):
        return False
    
    print("Build completed successfully!")
    return True

def test():
    """Run tests."""
    print("Running tests...")
    
    # Check if module is installed
    try:
        import pynlm
        print(f"pynlm version: {pynlm.__version__}")
    except ImportError:
        print("pynlm not installed. Please install it first.")
        return False
    
    # Run pytest
    if not run_command("python -m pytest python/tests/ -v"):
        return False
    
    print("All tests passed!")
    return True

def install():
    """Install the Python module."""
    print("Installing NLM Python bindings...")
    
    # Build first
    if not build():
        return False
    
    # Install in development mode
    if not run_command("pip install -e python"):
        return False
    
    print("Installation completed!")
    return True

def help():
    """Print help information."""
    print("Usage: ./build_and_test.py [command]")
    print()
    print("Commands:")
    print("  build      Build the Python bindings")
    print("  test       Run tests")
    print("  install    Build and install the module")
    print("  help       Show this help message")
    print()
    print("Examples:")
    print("  ./build_and_test.py install")
    print("  ./build_and_test.py test")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        help()
        sys.exit(1)
    
    command = sys.argv[1].lower()
    
    if command == "build":
        success = build()
    elif command == "test":
        success = test()
    elif command == "install":
        success = install()
    elif command == "help":
        help()
        success = True
    else:
        print(f"Unknown command: {command}")
        help()
        success = False
    
    sys.exit(0 if success else 1)
