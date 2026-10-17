#!/usr/bin/env python3
"""
Test script for NLM CLI functionality.
This script demonstrates the new CLI interface and verifies basic functionality.
"""

import subprocess
import sys
import os

def run_command(cmd):
    """Run a command and return output"""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        print(f"Exit code: {result.returncode}")
        if result.stdout:
            print(f"STDOUT:\n{result.stdout}")
        if result.stderr:
            print(f"STDERR:\n{result.stderr}")
        return result.returncode == 0
    except Exception as e:
        print(f"Error: {e}")
        return False

def test_cli_help():
    """Test CLI help functionality"""
    print("\n=== Testing CLI Help ===")
    return run_command("./build/nlm --help")

def test_cli_version():
    """Test CLI version functionality"""
    print("\n=== Testing CLI Version ===")
    return run_command("./build/nlm --version")

def test_cli_demo():
    """Test CLI demo mode"""
    print("\n=== Testing CLI Demo Mode ===")
    return run_command("./build/nlm --mode demo --steps 50 --num-steps 10")

def test_cli_benchmark():
    """Test CLI benchmark mode"""
    print("\n=== Testing CLI Benchmark Mode ===")
    return run_command("./build/nlm --mode benchmark --steps 100")

def test_cli_config():
    """Test CLI with configuration file"""
    print("\n=== Testing CLI with Config File ===")
    return run_command("./build/nlm --config configs/cli.cfg --mode demo --steps 20 --num-steps 5")

def main():
    print("NLM CLI Test Suite")
    print("==================")
    
    # Check if build directory exists
    if not os.path.exists("./build"):
        print("Build directory not found. Please run 'mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4' first.")
        return 1
    
    # Check if CLI executable exists
    if not os.path.exists("./build/nlm"):
        print("CLI executable not found. Please build the project first.")
        return 1
    
    tests = [
        test_cli_help,
        test_cli_version,
        test_cli_demo,
        test_cli_benchmark,
        test_cli_config,
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                print(f"Test {test.__name__} failed")
        except Exception as e:
            print(f"Test {test.__name__} failed with exception: {e}")
    
    print(f"\n=== Test Results ===")
    print(f"Passed: {passed}/{total}")
    
    if passed == total:
        print("All tests passed!")
        return 0
    else:
        print("Some tests failed.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
