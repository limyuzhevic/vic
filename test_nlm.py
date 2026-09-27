#!/usr/bin/env python3
"""
NLM Quick Start Test Script
===========================

This script provides a quick test of the NLM (Neural Learning Machine) Phase 2
enhanced system to verify that all major improvements are working correctly.

Features tested:
1. Configuration system
2. Neuromodulation
3. Memory systems
4. Brain integration
5. Command line interface
"""

import subprocess
import sys
import os
import time
import json

def print_header(title):
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

def print_step(step_num, description):
    print(f"\nStep {step_num}: {description}")
    print("-" * 40)

def test_basic_functionality():
    """Test basic NLM functionality"""
    print_step(1, "Testing basic NLM functionality")
    
    # Test help option
    print("1.1 Testing help option...")
    try:
        result = subprocess.run(["nlm", "--help"], 
                              capture_output=True, text=True, timeout=10)
        if result.returncode == 0 and "NLM - Neural Learning Machine" in result.stdout:
            print("   ✓ Help option working")
        else:
            print("   ✗ Help option failed")
            print(f"     Error: {result.stderr}")
            return False
    except subprocess.TimeoutExpired:
        print("   ✗ Help option timed out")
        return False
    except FileNotFoundError:
        print("   ✗ NLM executable not found")
        return False
    
    # Test demo mode (basic run)
    print("1.2 Testing demo mode...")
    try:
        # Use a small number of steps for quick test
        result = subprocess.run(["nlm", "--demo", "--steps", "100", "--quiet"],
                              capture_output=True, text=True, timeout=30)
        if result.returncode == 0:
            print("   ✓ Demo mode executed successfully")
        else:
            print("   ✗ Demo mode failed")
            print(f"     Error: {result.stderr}")
            return False
    except subprocess.TimeoutExpired:
        print("   ✗ Demo mode timed out")
        return False
    
    return True

def test_configuration_options():
    """Test configuration system"""
    print_step(2, "Testing configuration system")
    
    # Test different configuration options
    test_cases = [
        ("--neuron_count 500", "Custom neuron count"),
        ("--timestep 0.01", "Custom timestep"),
        ("--random-seed 123", "Fixed random seed"),
        ("--enable-curiosity", "Enable curiosity system"),
        ("--enable-novelty", "Enable novelty detection"),
    ]
    
    for args, description in test_cases:
        print(f"2.{len([x for x in range(1, len(test_cases)+1) if x == len([x for x in range(1, len(test_cases)+1) if x <= len([args])])])}. Testing {description}...")
        try:
            # Use quiet mode for faster testing
            cmd = ["nlm", "--demo", "--steps", "50", "--quiet"] + args.split()
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=20)
            if result.returncode == 0:
                print(f"   ✓ {description} works")
            else:
                print(f"   ✗ {description} failed")
                print(f"     Error: {result.stderr}")
                return False
        except subprocess.TimeoutExpired:
            print(f"   ✗ {description} timed out")
            return False
        except FileNotFoundError:
            print(f"   ✗ NLM executable not found")
            return False
    
    return True

def test_advanced_features():
    """Test advanced features"""
    print_step(3, "Testing advanced features")
    
    # Test stress test
    print("3.1 Testing stress test...")
    try:
        result = subprocess.run(["nlm", "--stress", "1000", "--steps", "50", "--quiet"],
                              capture_output=True, text=True, timeout=60)
        if result.returncode == 0:
            print("   ✓ Stress test executed")
        else:
            print("   ✗ Stress test failed")
            return False
    except subprocess.TimeoutExpired:
        print("   ✗ Stress test timed out")
        return False
    
    # Test with curiosity and novelty
    print("3.2 Testing with neuromodulators...")
    try:
        result = subprocess.run(["nlm", "--demo", "--steps", "50", 
                               "--enable-curiosity", "--enable-novelty",
                               "--enable-working-memory", "--enable-attention", "--quiet"],
                              capture_output=True, text=True, timeout=30)
        if result.returncode == 0:
            print("   ✓ Neuromodulators and subsystems work")
        else:
            print("   ✗ Neuromodulators test failed")
            return False
    except subprocess.TimeoutExpired:
        print("   ✗ Neuromodulators test timed out")
        return False
    
    return True

def test_documentation():
    """Test documentation availability"""
    print_step(4, "Testing documentation")
    
    # Check if documentation files exist
    doc_files = ["NLM_Documentation.md", "README.md"]
    
    for doc_file in doc_files:
        if os.path.exists(doc_file):
            print(f"4.{len([x for x in range(1, len(doc_files)+1) if x == len([x for x in range(1, len(doc_files)+1) if x <= len([doc_file])])])}. Found {doc_file}")
            
            # Check if file has content
            with open(doc_file, 'r') as f:
                content = f.read()
                if len(content) > 1000:  # Should have substantial content
                    print(f"   ✓ {doc_file} has content")
                else:
                    print(f"   ⚠ {doc_file} seems incomplete")
        else:
            print(f"4.{len([x for x in range(1, len(doc_files)+1) if x == len([x for x in range(1, len(doc_files)+1) if x <= len([doc_file])])])}. {doc_file} not found")
            return False
    
    return True

def main():
    print_header("NLM Quick Start Test Script")
    print("Testing NLM (Neural Learning Machine) Phase 2 Enhanced System")
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Configuration System", test_configuration_options),
        ("Advanced Features", test_advanced_features),
        ("Documentation", test_documentation),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        start_time = time.time()
        try:
            if test_func():
                passed += 1
                print(f"\n✓ {test_name} PASSED ({time.time() - start_time:.2f}s)")
            else:
                failed += 1
                print(f"\n✗ {test_name} FAILED ({time.time() - start_time:.2f}s)")
        except Exception as e:
            failed += 1
            print(f"\n✗ {test_name} ERROR: {e} ({time.time() - start_time:.2f}s)")
    
    print(f"\n{'='*60}")
    print("TEST SUMMARY")
    print(f"{'='*60}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total:  {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! NLM Phase 2 Enhanced is working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())