#!/usr/bin/env python3
"""
Python bindings test for NLM Unified Demo System
Tests the Python bindings and provides examples of how to use them
"""

import sys
import os
import subprocess
import json
from pathlib import Path

def test_python_bindings():
    """Test if Python bindings are available"""
    print("Testing Python bindings for NLM...")
    
    # Try to import pynlm
    try:
        import pynlm
        print("✓ pynlm module imported successfully")
        return True
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False

def test_unified_demo_system():
    """Test the unified demo system"""
    print("\nTesting unified demo system...")
    
    # Test help command
    try:
        result = subprocess.run(['nlm', '--help'], 
                              capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            print("✓ Unified demo system help command works")
            if 'NLM Unified Demo System' in result.stdout:
                print("✓ Help contains unified demo system branding")
            return True
        else:
            print(f"✗ Help command failed: {result.stderr}")
            return False
    except subprocess.TimeoutExpired:
        print("✗ Help command timed out")
        return False
    except FileNotFoundError:
        print("✗ nlm executable not found")
        return False

def test_phase_selection():
    """Test phase selection functionality"""
    print("\nTesting phase selection...")
    
    phases = ['2', '3', '5', '6', '7', 'benchmark']
    all_passed = True
    
    for phase in phases:
        try:
            result = subprocess.run(['nlm', '--phase', phase, '--help'],
                                  capture_output=True, text=True, timeout=10)
            if result.returncode == 0:
                print(f"✓ Phase {phase} selection works")
            else:
                print(f"✗ Phase {phase} selection failed: {result.stderr}")
                all_passed = False
        except subprocess.TimeoutExpired:
            print(f"✗ Phase {phase} selection timed out")
            all_passed = False
        except Exception as e:
            print(f"✗ Phase {phase} selection error: {e}")
            all_passed = False
    
    return all_passed

def test_configuration_features():
    """Test configuration system features"""
    print("\nTesting configuration system...")
    
    # Test configuration file options
    test_cases = [
        ['nlm', '--phase', '3', '--help'],
        ['nlm', '--version'],
        ['nlm', '--help-advanced']
    ]
    
    all_passed = True
    
    for test_case in test_cases:
        try:
            result = subprocess.run(test_case, 
                                  capture_output=True, text=True, timeout=10)
            if result.returncode == 0:
                print(f"✓ Configuration feature works: {' '.join(test_case)}")
            else:
                print(f"✗ Configuration feature failed: {' '.join(test_case)}")
                print(f"  Error: {result.stderr}")
                all_passed = False
        except subprocess.TimeoutExpired:
            print(f"✗ Configuration feature timed out: {' '.join(test_case)}")
            all_passed = False
        except Exception as e:
            print(f"✗ Configuration feature error: {' '.join(test_case)} - {e}")
            all_passed = False
    
    return all_passed

def test_python_bindings_integration():
    """Test Python bindings integration if available"""
    print("\nTesting Python bindings integration...")
    
    # Check if pynlm module exists
    try:
        import pynlm
        print("✓ pynlm module is available")
        
        # Test basic functionality
        if hasattr(pynlm, 'createDefaultConfig'):
            print("✓ createDefaultConfig function available")
        
        if hasattr(pynlm, 'createBrain'):
            print("✓ createBrain function available")
        
        if hasattr(pynlm, 'createSimpleWorld'):
            print("✓ createSimpleWorld function available")
        
        if hasattr(pynlm, 'createAgentBrain'):
            print("✓ createAgentBrain function available")
        
        return True
        
    except ImportError:
        print("✗ pynlm module not available")
        return False

def run_demo_tests():
    """Run actual demo tests"""
    print("\nRunning demo tests...")
    
    # Test Phase 3 demo with minimal settings
    print("Testing Phase 3 demo...")
    try:
        result = subprocess.run([
            'nlm', '--phase', '3', '--episodes', '1', '--steps', '10'
        ], capture_output=True, text=True, timeout=30)
        
        if result.returncode == 0:
            print("✓ Phase 3 demo runs successfully")
            if 'Phase 3 demonstration complete' in result.stdout:
                print("✓ Phase 3 demo completed expected output")
            return True
        else:
            print(f"✗ Phase 3 demo failed: {result.stderr}")
            return False
            
    except subprocess.TimeoutExpired:
        print("✗ Phase 3 demo timed out")
        return False

def create_test_script():
    """Create a test script for users"""
    test_script = '''#!/usr/bin/env python3
"""
NLM Unified Demo System Test Script
This script tests all major features of the NLM Unified Demo System
"""

import subprocess
import sys
import os

def run_test(description, command):
    """Run a single test"""
    print(f"\\n=== Testing: {description} ===")
    print(f"Command: {' '.join(command)}")
    
    try:
        result = subprocess.run(command, capture_output=True, text=True, timeout=60)
        
        if result.returncode == 0:
            print("✓ PASSED")
            if result.stdout:
                print(f"Output preview: {result.stdout[:200]}...")
            return True
        else:
            print("✗ FAILED")
            print(f"Error: {result.stderr}")
            return False
            
    except subprocess.TimeoutExpired:
        print("✗ FAILED (Timeout)")
        return False
    except Exception as e:
        print(f"✗ FAILED (Error: {e})")
        return False

def main():
    print("NLM Unified Demo System Test Suite")
    print("=" * 50)
    
    tests = [
        ("System Help", ['nlm', '--help']),
        ("Version Info", ['nlm', '--version']),
        ("Phase Selection (Phase 3)", ['nlm', '--phase', '3', '--help']),
        ("Phase Selection (Phase 6)", ['nlm', '--phase', '6', '--help']),
        ("Phase Selection (Benchmark)", ['nlm', '--benchmark', '--help']),
        ("Advanced Options", ['nlm', '--help-advanced']),
        ("Minimal Phase 3 Demo", ['nlm', '--phase', '3', '--episodes', '1', '--steps', '10']),
    ]
    
    passed = 0
    total = len(tests)
    
    for description, command in tests:
        if run_test(description, command):
            passed += 1
    
    print(f"\\n=== Test Summary ===")
    print(f"Passed: {passed}/{total}")
    
    if passed == total:
        print("✓ All tests passed!")
        return 0
    else:
        print("✗ Some tests failed")
        return 1

if __name__ == '__main__':
    sys.exit(main())
'''
    
    with open('test_nlm_unified.py', 'w') as f:
        f.write(test_script)
    
    # Make it executable
    os.chmod('test_nlm_unified.py', 0o755)
    
    print("✓ Test script created: test_nlm_unified.py")

def main():
    print("NLM Unified Demo System - Python Bindings Test Suite")
    print("=" * 60)
    
    results = []
    
    # Test Python bindings
    results.append(("Python Bindings", test_python_bindings()))
    
    # Test unified demo system
    results.append(("Unified Demo System", test_unified_demo_system()))
    
    # Test phase selection
    results.append(("Phase Selection", test_phase_selection()))
    
    # Test configuration features
    results.append(("Configuration Features", test_configuration_features()))
    
    # Test Python bindings integration
    results.append(("Python Bindings Integration", test_python_bindings_integration()))
    
    # Run demo tests
    results.append(("Demo Tests", run_demo_tests()))
    
    # Create test script
    create_test_script()
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    all_passed = True
    for test_name, passed in results:
        status = "✓ PASSED" if passed else "✗ FAILED"
        print(f"{test_name:30} {status}")
        if not passed:
            all_passed = False
    
    if all_passed:
        print("\n🎉 All tests passed! The NLM Unified Demo System is working correctly.")
    else:
        print("\n⚠️  Some tests failed. Please check the output above for details.")
        print("This may be expected if Python bindings are not fully configured.")
    
    return 0 if all_passed else 1

if __name__ == '__main__':
    sys.exit(main())
