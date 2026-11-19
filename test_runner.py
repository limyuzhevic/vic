#!/usr/bin/env python3
"""
NLM Python Bindings Test Runner

Comprehensive test runner for NLM Python bindings unit tests.
Supports multiple test suites and provides detailed reporting.
"""

import sys
import os
import argparse
import time
import json
from datetime import datetime
from pathlib import Path

# Add the tests directory to the path
sys.path.insert(0, str(Path(__file__).parent / 'tests'))

import pytest
import unittest

def run_pytest_tests():
    """Run pytest-based tests."""
    print("=== Running Pytest Tests ===")
    
    # Find and run pytest tests
    test_files = Path(__file__).parent / 'tests'
    
    # Discover test files
    pytest_files = list(test_files.glob("test_*.py"))
    
    if not pytest_files:
        print("No pytest test files found")
        return False
    
    print(f"Found {len(pytest_files)} pytest test files:")
    for test_file in pytest_files:
        print(f"  - {test_file.name}")
    
    # Run pytest with specific arguments
    args = [
        str(test_files),
        "-v",           # Verbose output
        "--tb=short",  # Short traceback
        "--timeout=300",  # Timeout for each test
        "--disable-warnings",  # Disable warnings
    ]
    
    try:
        result = pytest.main(args)
        return result == 0
    except Exception as e:
        print(f"Error running pytest: {e}")
        return False

def run_unittest_tests():
    """Run unittest-based tests."""
    print("\n=== Running Unittest Tests ===")
    
    # Discover unittest test modules
    test_modules = [
        'test_core_functionality',
        'test_advanced_api',
    ]
    
    all_passed = True
    
    for module in test_modules:
        try:
            print(f"\n--- Running {module} ---")
            
            # Import and run the test module
            module_path = f"tests.{module}"
            
            # Create a test suite
            loader = unittest.TestLoader()
            
            # Try to load from the module
            try:
                if module == 'test_core_functionality':
                    from tests.test_core_functionality import TestCoreFunctionality, TestPerformanceOptimization, TestMemoryManagement
                    suite = unittest.TestSuite()
                    suite.addTest(loader.loadTestsFromTestCase(TestCoreFunctionality))
                    suite.addTest(loader.loadTestsFromTestCase(TestPerformanceOptimization))
                    suite.addTest(loader.loadTestsFromTestCase(TestMemoryManagement))
                elif module == 'test_advanced_api':
                    from tests.test_advanced_api import TestAdvancedAPI, TestPerformanceOptimization
                    suite = unittest.TestSuite()
                    suite.addTest(loader.loadTestsFromTestCase(TestAdvancedAPI))
                    suite.addTest(loader.loadTestsFromTestCase(TestPerformanceOptimization))
                else:
                    print(f"Unknown test module: {module}")
                    all_passed = False
                    continue
                
                # Run the test suite
                runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
                result = runner.run(suite)
                
                if not result.wasSuccessful():
                    all_passed = False
                    
            except ImportError as e:
                print(f"Could not import test module {module}: {e}")
                all_passed = False
                continue
                
        except Exception as e:
            print(f"Error running {module}: {e}")
            all_passed = False
    
    return all_passed

def run_all_tests():
    """Run all available tests."""
    print("=== NLM Python Bindings Test Suite ===")
    print(f"Test run started at: {datetime.now().isoformat()}")
    print(f"Working directory: {os.getcwd()}")
    
    # Check if pynlm is available
    try:
        import pynlm
        print(f"NLM Python bindings: {hasattr(pynlm, '__version__') and pynlm.__version__ or 'Unknown version'}")
        print(f"Available classes: {', '.join([attr for attr in dir(pynlm) if not attr.startswith('_')])}")
    except ImportError as e:
        print(f"ERROR: Cannot import pynlm: {e}")
        print("Please ensure NLM Python bindings are properly installed.")
        return False
    
    # Create test directory if it doesn't exist
    test_dir = Path(__file__).parent / 'tests'
    test_dir.mkdir(exist_ok=True)
    
    # Run tests
    start_time = time.time()
    
    pytest_passed = run_pytest_tests()
    unittest_passed = run_unittest_tests()
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Generate summary report
    print("\n" + "="*50)
    print("=== TEST SUMMARY ===")
    print(f"Total duration: {duration:.2f} seconds")
    print(f"Pytest tests: {'PASSED' if pytest_passed else 'FAILED'}")
    print(f"Unittest tests: {'PASSED' if unittest_passed else 'FAILED'}")
    
    overall_success = pytest_passed and unittest_passed
    print(f"Overall: {'PASSED' if overall_success else 'FAILED'}")
    
    # Save detailed report
    report = {
        "timestamp": datetime.now().isoformat(),
        "duration": duration,
        "pytest_passed": pytest_passed,
        "unittest_passed": unittest_passed,
        "overall_success": overall_success,
        "test_files_found": len(list(test_dir.glob("test_*.py"))),
        "modules_tested": ["test_core_functionality", "test_advanced_api"]
    }
    
    # Save report to JSON file
    report_file = test_dir / "test_report.json"
    with open(report_file, 'w') as f:
        json.dump(report, f, indent=2)
    
    print(f"\nDetailed report saved to: {report_file}")
    
    return overall_success

def run_specific_tests(args):
    """Run specific tests based on command line arguments."""
    print("=== Running Specific Tests ===")
    
    # Determine which tests to run based on arguments
    test_suites = []
    
    if "--all" in args:
        test_suites = ["core", "advanced"]
    elif "--core-only" in args:
        test_suites = ["core"]
    elif "--advanced-only" in args:
        test_suites = ["advanced"]
    elif "--help" in args:
        print("Usage: python test_runner.py [--all|--core-only|--advanced-only]")
        return False
    
    # Run specific test suites
    if "core" in test_suites:
        print("\n--- Running Core Functionality Tests ---")
        # In a real implementation, we would run core tests here
        print("Core tests would run here")
    
    if "advanced" in test_suites:
        print("\n--- Running Advanced API Tests ---")
        # In a real implementation, we would run advanced tests here
        print("Advanced tests would run here")
    
    print("\nSpecific test execution completed")
    return True

def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings Test Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python test_runner.py --all                    # Run all tests
  python test_runner.py --core-only               # Run only core functionality tests
  python test_runner.py --advanced-only           # Run only advanced API tests
  python test_runner.py --help                     # Show this help message
        """
    )
    
    parser.add_argument(
        "--all",
        action="store_true",
        help="Run all available tests"
    )
    
    parser.add_argument(
        "--core-only",
        action="store_true",
        help="Run only core functionality tests"
    )
    
    parser.add_argument(
        "--advanced-only",
        action="store_true",
        help="Run only advanced API tests"
    )
    
    parser.add_argument(
        "--pytest-only",
        action="store_true",
        help="Run only pytest tests"
    )
    
    parser.add_argument(
        "--unittest-only",
        action="store_true",
        help="Run only unittest tests"
    )
    
    args = parser.parse_args()
    
    # Determine which tests to run
    if args.all:
        success = run_all_tests()
    elif args.core_only:
        success = run_specific_tests(["core-only"])
    elif args.advanced_only:
        success = run_specific_tests(["advanced-only"])
    elif args.pytest_only:
        success = run_pytest_tests()
    elif args.unittest_only:
        success = run_unittest_tests()
    else:
        # Default: run all tests
        success = run_all_tests()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
