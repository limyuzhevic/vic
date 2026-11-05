#!/usr/bin/env python3
"""
Test script for NLM Python bindings

This script can be run directly to test the pynlm Python bindings.
"""

import sys
import os

def main():
    print("=== NLM Python Bindings Test ===")
    print()
    
    # Add tests directory to path
    tests_dir = os.path.dirname(os.path.abspath(__file__))
    sys.path.insert(0, os.path.join(tests_dir, 'tests'))
    
    # Import and run test_pynlm
    try:
        from test_pynlm import run_all_tests
        return run_all_tests()
    except ImportError as e:
        print(f"ERROR: Cannot import test_pynlm: {e}")
        print("This usually means the pynlm Python module is not built.")
        print("Please build the Python bindings first:")
        print("  pip install .")
        return 1

if __name__ == "__main__":
    sys.exit(main())