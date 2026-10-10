#!/usr/bin/env python3
"""
NLM Build and Installation Script

This script provides a unified interface for building, installing, and managing
the NLM (Neural Learning Machine) project.

Usage:
    ./build.sh build      # Build the project
    ./build.sh install    # Install to system directories
    ./build.sh test       # Run tests
    ./build.sh clean      # Clean build artifacts
    ./build.sh help       # Show help
"""

import argparse
import os
import subprocess
import sys
from pathlib import Path

def run_command(cmd, cwd=None):
    """Run a command and return success status."""
    print(f"Running: {cmd}")
    try:
        subprocess.run(cmd, shell=True, cwd=cwd, check=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Command failed with exit code {e.returncode}: {e.cmd}")
        return False

def build_project():
    """Build the NLM project."""
    print("Building NLM project...")
    
    # Check if build directory exists
    build_dir = Path("build")
    if not build_dir.exists():
        build_dir.mkdir(exist_ok=True)
    
    # Configure with CMake
    cmake_cmd = ""
    if sys.platform == "win32":
        cmake_cmd = "cmake .. -G \"Visual Studio 17 2022\" -A x64"
    else:
        cmake_cmd = "cmake .. -DCMAKE_BUILD_TYPE=Release"
    
    if not run_command(cmake_cmd, cwd=build_dir):
        return False
    
    # Build all targets
    if sys.platform == "win32":
        build_cmd = "cmake --build . --config Release --parallel"
    else:
        build_cmd = "make -j$(nproc)"
    
    if not run_command(build_cmd, cwd=build_dir):
        return False
    
    print("Build completed successfully!")
    return True

def install_project():
    """Install the NLM project to system directories."""
    print("Installing NLM project...")
    
    build_dir = Path("build")
    if not build_dir.exists():
        print("Build directory not found. Running build first...")
        if not build_project():
            return False
    
    # Install to standard locations
    if sys.platform == "win32":
        install_cmd = "cmake --install . --config Release"
    else:
        install_cmd = "cmake --install . --prefix /usr/local"
    
    if not run_command(install_cmd, cwd=build_dir):
        return False
    
    print("Installation completed successfully!")
    return True

def run_tests():
    """Run NLM tests."""
    print("Running NLM tests...")
    
    build_dir = Path("build")
    if not build_dir.exists():
        print("Build directory not found. Building first...")
        if not build_project():
            return False
    
    # Run tests
    if sys.platform == "win32":
        test_cmd = "ctest --output-on-failure -C Release"
    else:
        test_cmd = "ctest --output-on-failure"
    
    if not run_command(test_cmd, cwd=build_dir):
        return False
    
    print("Tests completed!")
    return True

def clean_project():
    """Clean build artifacts."""
    print("Cleaning build artifacts...")
    
    # Remove build directory
    import shutil
    if Path("build").exists():
        shutil.rmtree("build")
        print("Removed build directory")
    
    # Remove Python build artifacts
    for pattern in ["*.egg-info", "dist", "__pycache__"]:
        for path in Path(".").glob(pattern):
            if path.is_dir():
                shutil.rmtree(path)
            else:
                path.unlink()
            print(f"Removed {path}")
    
    print("Clean completed!")
    return True

def main():
    parser = argparse.ArgumentParser(description="NLM Build and Installation Script")
    parser.add_argument("action", choices=["build", "install", "test", "clean", "help"],
                       help="Action to perform")
    parser.add_argument("--verbose", "-v", action="store_true", help="Enable verbose output")
    parser.add_argument("--build-dir", "-b", default="build", help="Build directory")
    
    args = parser.parse_args()
    
    if args.action == "help":
        print(__doc__)
        return 0
    
    # Set up verbose output
    if args.verbose:
        print("Verbose mode enabled")
    
    # Execute the requested action
    if args.action == "build":
        return 0 if build_project() else 1
    elif args.action == "install":
        return 0 if install_project() else 1
    elif args.action == "test":
        return 0 if run_tests() else 1
    elif args.action == "clean":
        return 0 if clean_project() else 1

if __name__ == "__main__":
    sys.exit(main())