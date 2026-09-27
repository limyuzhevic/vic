#!/usr/bin/env python3
"""
NLM Python Test Runner

This script runs the Python test suite for NLM bindings.
It can be used to run tests independently of the C++ build system.
"""

import sys
import os
import unittest
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent / 'python'))
sys.path.insert(0, str(Path(__file__).parent / 'tests'))
def main():
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    # Check if pynlm module is available
    try:
        import pynlm
        print("✓ pynlm module loaded successfully")
        HAS_PYBINDINGS = True
    except ImportError as e:
        print(f"✗ Failed to load pynlm module: {e}")
        print("Python binding tests cannot be run without the module.")
        print("Please ensure the NLM C++ library is built and installed.")
        HAS_PYBINDINGS = False
    
    print("\nRunning Python tests...")
    print("-" * 60)
    
    if HAS_PYBINDINGS:
        # Import and run tests
        from tests.python.test_bindings import (
            TestConfigBindings,
            TestBrainBindings,
            TestAgentBrainBindings,
            TestWorldBindings,
            TestVisualizationBindings,
            TestHelperFunctions,
            TestErrorHandling
        )
        
        # Create test suite
        loader = unittest.TestLoader()
        suite = unittest.TestSuite()
        
        # Add test classes
        test_classes = [
            TestConfigBindings,
            TestBrainBindings,
            TestAgentBrainBindings,
            TestWorldBindings,
            TestVisualizationBindings,
            TestHelperFunctions,
            TestErrorHandling
        ]
        
        for test_class in test_classes:
            tests = loader.loadTestsFromTestCase(test_class)
            suite.addTests(tests)
        
        # Run tests
        runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
        result = runner.run(suite)
        
        print("\n" + "=" * 60)
        if result.wasSuccessful():
            print("✓ All tests passed!")
            return 0
        else:
            print("✗ Some tests failed!")
            print(f"\nFailures: {len(result.failures)}")
            print(f"Errors: {len(result.errors)}")
            return 1
    else:
        print("Skipping tests - pynlm module not available")
        return 0
if __name__ == '__main__':
    sys.exit(main())