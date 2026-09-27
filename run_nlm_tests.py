#!/usr/bin/env python3
"""
Test runner for NLM Python bindings comprehensive test suite.
"""

import unittest
import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

# Import test classes
from test_nlm_bindings import (
    TestNLMTypes,
    TestConfig, 
    TestBrainUtils,
    TestAdvancedConfig,
    TestBrainStatistics,
    TestProgressTracking,
    TestErrorHandling,
    TestPerformanceMonitoring,
    TestIntegration
)

def main():
    print("Running comprehensive NLM Python bindings test suite...")
    print("=" * 60)
    
    # Create test suite
    loader = unittest.TestLoader()
    
    test_suite = unittest.TestSuite()
    test_suite.addTests(loader.loadTestsFromTestCase(TestNLMTypes))
    test_suite.addTests(loader.loadTestsFromTestCase(TestConfig))
    test_suite.addTests(loader.loadTestsFromTestCase(TestBrainUtils))
    test_suite.addTests(loader.loadTestsFromTestCase(TestAdvancedConfig))
    test_suite.addTests(loader.loadTestsFromTestCase(TestBrainStatistics))
    test_suite.addTests(loader.loadTestsFromTestCase(TestProgressTracking))
    test_suite.addTests(loader.loadTestsFromTestCase(TestErrorHandling))
    test_suite.addTests(loader.loadTestsFromTestCase(TestPerformanceMonitoring))
    test_suite.addTests(loader.loadTestsFromTestCase(TestIntegration))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
    result = runner.run(test_suite)
    
    # Print summary
    print("\n" + "=" * 60)
    if result.wasSuccessful():
        print("✅ All tests passed successfully!")
        print(f"   Tests run: {result.testsRun}")
        print(f"   Failures: {len(result.failures)}")
        print(f"   Errors: {len(result.errors)}")
        print("=" * 60)
        return True
    else:
        print("❌ Some tests failed!")
        print(f"   Tests run: {result.testsRun}")
        print(f"   Failures: {len(result.failures)}")
        print(f"   Errors: {len(result.errors)}")
        
        if result.failures:
            print("\nFailures:")
            for test, traceback in result.failures:
                print(f"   - {test}: {traceback}")
        
        if result.errors:
            print("\nErrors:")
            for test, traceback in result.errors:
                print(f"   - {test}: {traceback}")
        
        print("=" * 60)
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)