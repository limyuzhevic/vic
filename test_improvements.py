#!/usr/bin/env python3
"""
Test script for NLM Common Utilities and improved demos.
This script tests the new command-line interface, configuration file support,
and common utilities for NLM demos.
"""

import subprocess
import os
import sys
from pathlib import Path

def run_command(cmd, cwd=None):
    """Run a command and return result."""
    try:
        result = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, shell=True)
        return result.returncode, result.stdout, result.stderr
    except Exception as e:
        print(f"Error running command: {e}")
        return None

def test_help_command():
    """Test the help command."""
    print("Testing help command...")
    returncode, stdout, stderr = run_command("./nlm_phase3_demo --help")
    
    if returncode == 0 and "Usage:" in stdout and "Options:" in stdout:
        print("✓ Help command works correctly")
        return True
    else:
        print("✗ Help command failed")
        print(f"Return code: {returncode}")
        print(f"Stdout: {stdout[:200]}...")
        print(f"Stderr: {stderr[:200]}...")
        return False

def test_version_command():
    """Test the version command."""
    print("Testing version command...")
    returncode, stdout, stderr = run_command("./nlm_phase3_demo --version")
    
    if returncode == 0 and "NLM (熙然)" in stdout:
        print("✓ Version command works correctly")
        return True
    else:
        print("✗ Version command failed")
        print(f"Return code: {returncode}")
        print(f"Stdout: {stdout[:200]}...")
        print(f"Stderr: {stderr[:200]}...")
        return False

def test_custom_parameters():
    """Test custom parameters."""
    print("Testing custom parameters...")
    returncode, stdout, stderr = run_command("./nlm_phase3_demo --episodes 2 --steps 50")
    
    if returncode == 0:
        print("✓ Custom parameters work correctly")
        return True
    else:
        print("✗ Custom parameters failed")
        return False

def test_config_file():
    """Test configuration file."""
    print("Testing configuration file...")
    returncode, stdout, stderr = run_command("./nlm_phase3_demo --config ./configs/fast_test.cfg")
    
    if returncode == 0:
        print("✓ Configuration file works correctly")
        return True
    else:
        print("✗ Configuration file failed")
        return False

def test_all_demos():
    """Test all demo executables."""
    demos = [
        "./nlm_phase3_demo",
        "./nlm_phase4_demo",
        "./nlm_phase5_demo",
        "./nlm_phase6_demo",
        "./nlm_phase6_text_demo",
        "./nlm_benchmark"
    ]
    
    print("Testing all demo executables...")
    all_passed = True
    
    for demo in demos:
        if os.path.exists(demo):
            returncode, stdout, stderr = run_command(f"timeout 5 {demo} --help")
            if returncode == 0 and ("Usage:" in stdout or "Options:" in stdout):
                print(f"✓ {demo} works correctly")
            else:
                print(f"✗ {demo} failed")
                all_passed = False
        else:
            print(f"✗ {demo} not found")
            all_passed = False
    
    return all_passed

def main():
    print("=" * 60)
    print("NLM Common Utilities and Improvements Test")
    print("=" * 60)
    
    # Check if we're in the right directory
    if not os.path.exists("./nlm_phase3_demo"):
        print("ERROR: nlm_phase3_demo not found. Please run from build directory.")
        return 1
    
    tests = [
        test_help_command,
        test_version_command,
        test_custom_parameters,
        test_config_file,
        test_all_demos
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        if test():
            passed += 1
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed}/{total} tests passed")
    print("=" * 60)
    
    if passed == total:
        print("All tests passed! ✓")
        return 0
    else:
        print("Some tests failed. ✗")
        return 1

if __name__ == "__main__":
    sys.exit(main())