#!/usr/bin/env python3
"""
Test script for NLM Unified Main Entry Point

This script tests the new unified main entry point functionality
and configuration system improvements.
"""

import subprocess
import sys
import os
import tempfile

def run_command(cmd):
    """Run a command and return its output."""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result.returncode, result.stdout, result.stderr
    except Exception as e:
        return -1, "", str(e)

def test_help_command():
    """Test that help command works."""
    print("Testing help command...")
    returncode, stdout, stderr = run_command("./nlm --help")
    
    if returncode != 0:
        print(f"  FAILED: Help command returned {returncode}")
        print(f"  STDERR: {stderr}")
        return False
    
    if "NLM (熙然)" in stdout and "USAGE:" in stdout:
        print("  PASSED: Help command shows expected output")
        return True
    else:
        print("  FAILED: Help command output not as expected")
        print(f"  STDOUT: {stdout[:500]}...")
        return False

def test_list_modes_command():
    """Test that list-modes command works."""
    print("Testing list-modes command...")
    returncode, stdout, stderr = run_command("./nlm --list-modes")
    
    if returncode != 0:
        print(f"  FAILED: List-modes command returned {returncode}")
        print(f"  STDERR: {stderr}")
        return False
    
    if "AVAILABLE NLM MODES:" in stdout and "PHASE 2:" in stdout:
        print("  PASSED: List-modes command shows expected output")
        return True
    else:
        print("  FAILED: List-modes command output not as expected")
        print(f"  STDOUT: {stdout[:500]}...")
        return False

def test_auto_mode():
    """Test auto mode selection."""
    print("Testing auto mode selection...")
    # Test with 600 neurons (should select phase6)
    returncode, stdout, stderr = run_command("./nlm --mode auto --neurons 600 --steps 100")
    
    if returncode != 0:
        print(f"  FAILED: Auto mode with 600 neurons returned {returncode}")
        print(f"  STDERR: {stderr}")
        return False
    
    print("  PASSED: Auto mode with 600 neurons works")
    
    # Test with 300 neurons (should select phase4)
    returncode, stdout, stderr = run_command("./nlm --mode auto --neurons 300 --steps 100")
    
    if returncode != 0:
        print(f"  FAILED: Auto mode with 300 neurons returned {returncode}")
        print(f"  STDERR: {stderr}")
        return False
    
    print("  PASSED: Auto mode with 300 neurons works")
    
    # Test with 150 neurons (should select phase3)
    returncode, stdout, stderr = run_command("./nlm --mode auto --neurons 150 --steps 100")
    
    if returncode != 0:
        print(f"  FAILED: Auto mode with 150 neurons returned {returncode}")
        print(f"  STDERR: {stderr}")
        return False
    
    print("  PASSED: Auto mode with 150 neurons works")
    
    # Test with 50 neurons (should select phase2)
    returncode, stdout, stderr = run_command("./nlm --mode auto --neurons 50 --steps 100")
    
    if returncode != 0:
        print(f"  FAILED: Auto mode with 50 neurons returned {returncode}")
        print(f"  STDERR: {stderr}")
        return False
    
    print("  PASSED: Auto mode with 50 neurons works")
    
    return True

def test_custom_settings():
    """Test custom settings via command line."""
    print("Testing custom settings...")
    returncode, stdout, stderr = run_command("./nlm --mode phase2 --neurons 500 --steps 100 --plasticity.learning_rate=0.01")
    
    if returncode != 0:
        print(f"  FAILED: Custom settings test returned {returncode}")
        print(f"  STDERR: {stderr}")
        return False
    
    print("  PASSED: Custom settings test works")
    return True

def test_config_generation():
    """Test configuration generation."""
    print("Testing configuration generation...")
    
    # Generate a configuration file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        config_file = f.name
        f.write("# Test configuration\n")
        f.write("neuron_count = 100\n")
        f.write("region_count = 1\n")
        f.write("connection_probability = 0.1\n")
    
    try:
        # Test that we can load the config file
        returncode, stdout, stderr = run_command(f"./nlm --config {config_file} --mode phase2 --steps 100")
        
        if returncode != 0:
            print(f"  FAILED: Config file loading returned {returncode}")
            print(f"  STDERR: {stderr}")
            return False
        
        print("  PASSED: Configuration file loading works")
        return True
    finally:
        # Clean up
        os.unlink(config_file)

def test_phase_selection():
    """Test explicit phase selection."""
    print("Testing explicit phase selection...")
    
    phases = ["phase2", "phase3", "phase4", "phase5", "phase6", "benchmark"]
    
    for phase in phases:
        returncode, stdout, stderr = run_command(f"./nlm --mode {phase} --neurons 100 --steps 50")
        
        if returncode != 0:
            print(f"  FAILED: Phase {phase} selection returned {returncode}")
            print(f"  STDERR: {stderr}")
            return False
        
        print(f"  PASSED: Phase {phase} selection works")
    
    return True

def main():
    """Run all tests."""
    print("NLM Unified Main Entry Point Test Suite")
    print("=" * 50)
    
    tests = [
        test_help_command,
        test_list_modes_command,
        test_auto_mode,
        test_custom_settings,
        test_config_generation,
        test_phase_selection,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"  ERROR: Test {test.__name__} raised exception: {e}")
            failed += 1
        
        print()
    
    print("=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed > 0:
        print("Some tests failed. Please check the output above.")
        return 1
    else:
        print("All tests passed!")
        return 0

if __name__ == "__main__":
    sys.exit(main())