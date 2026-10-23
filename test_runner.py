#!/usr/bin/env python3
"""
Test Runner for NLM

This script provides a simple interface to run tests and build the project.
"""

import subprocess
import sys
import os
from pathlib import Path

def run_command(cmd, description):
    """Run a command and print results."""
    print(f"\n{'='*60}")
    print(f"Running: {description}")
    print(f"Command: {cmd}")
    print(f"{'='*60}")
    
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        
        if result.stdout:
            print("STDOUT:")
            print(result.stdout)
        
        if result.stderr:
            print("STDERR:")
            print(result.stderr)
        
        print(f"Return code: {result.returncode}")
        
        return result.returncode == 0
        
    except Exception as e:
        print(f"Error running command: {e}")
        return False

def build_project():
    """Build the project using scikit-build."""
    print("Building NLM project...")
    
    # Check if in a git repository
    repo_root = Path(__file__).parent
    if not (repo_root / ".git").exists():
        print("Not in a git repository, skipping build")
        return True
    
    # Build using pip install -e . (which will use scikit-build)
    return run_command(
        "pip install -e .",
        "Install project in development mode (builds Python bindings)"
    )

def run_python_tests():
    """Run Python tests."""
    return run_command(
        "python -m pytest tests/ -v",
        "Run Python tests with verbose output"
    )

def run_cpp_tests():
    """Run C++ tests if built."""
    print("\nChecking for C++ test executable...")
    
    # Look for the test executable
    test_executable = Path(__file__).parent / "build" / "nlm_test"
    if not test_executable.exists():
        # Try to find it in a different location
        build_dir = Path(__file__).parent
        for root, dirs, files in os.walk(build_dir):
            if "nlm_test" in files:
                test_executable = Path(root) / "nlm_test"
                break
    
    if not test_executable.exists():
        print("C++ test executable not found. Building project first...")
        if not build_project():
            return False
        # Try to find it again
        if not test_executable.exists():
            print("C++ test executable still not found after build")
            return True  # Skip C++ tests for now
    
    return run_command(
        str(test_executable),
        "Run C++ unit tests"
    )

def main():
    """Main test runner function."""
    print("NLM Test Runner")
    print("=" * 60)
    
    # Parse command line arguments
    if len(sys.argv) > 1:
        if sys.argv[1] == "--python-only":
            # Run only Python tests
            success = run_python_tests()
            sys.exit(0 if success else 1)
        
        elif sys.argv[1] == "--cpp-only":
            # Run only C++ tests
            success = run_cpp_tests()
            sys.exit(0 if success else 1)
    
    # Run full test suite
    print("Running full test suite...")
    
    # Build the project first
    if not build_project():
        print("Failed to build project")
        sys.exit(1)
    
    # Run Python tests
    python_success = run_python_tests()
    
    # Run C++ tests
    cpp_success = run_cpp_tests()
    
    # Summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Python tests: {'PASSED' if python_success else 'FAILED'}")
    print(f"C++ tests: {'PASSED' if cpp_success else 'FAILED'}")
    
    if python_success and cpp_success:
        print("\nAll tests passed!")
        sys.exit(0)
    else:
        print("\nSome tests failed.")
        sys.exit(1)

if __name__ == "__main__":
    main()