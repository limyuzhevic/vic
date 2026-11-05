#!/usr/bin/env python3
"""
Simple test runner for NLM Python bindings

This script runs the Python test suite for pynlm bindings.
"""

import sys
import os

def main():
    print("=== NLM Python Bindings Test Runner ===")
    print()
    
    # Check if pynlm is available
    try:
        import pynlm
        print("✓ pynlm module found")
        PYNLM_AVAILABLE = True
    except ImportError as e:
        print(f"✗ pynlm module not available: {e}")
        print("Please build the Python bindings first using: pip install .")
        return 1
    
    # Add tests directory to path
    tests_dir = os.path.join(os.path.dirname(__file__), 'tests')
    if not os.path.exists(tests_dir):
        print("✗ Tests directory not found")
        return 1
    
    # Try to run the test file
    test_file = os.path.join(tests_dir, 'test_pynlm.py')
    if not os.path.exists(test_file):
        print("✗ test_pynlm.py not found")
        return 1
    
    print(f"✓ Found test file: {test_file}")
    print()
    
    # Run the test file
    import importlib.util
    spec = importlib.util.spec_from_file_location("test_pynlm", test_file)
    test_module = importlib.util.module_from_spec(spec)
    
    # Execute the module
    spec.loader.exec_module(test_module)
    
    return 0

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)