#!/usr/bin/env python3
"""
NLM Python Test Runner

This script runs the comprehensive test suite for NLM Python bindings.
It provides a unified interface for running all tests and reporting results.

Features:
- Run all tests with a single command
- Detailed progress reporting
- Test results summary
- Error handling and reporting
- Support for specific test categories
- Performance reporting
"""

import sys
import os
import time
import argparse
import traceback
from pathlib import Path

# Add the tests directory to the path
sys.path.insert(0, str(Path(__file__).parent / "python" / "tests"))

# Import test modules
from test_context_managers import test_all_context_managers
from test_convenience_methods import test_all_convenience_methods
from test_type_hints_and_error_handling import test_all_type_hints_and_error_handling
from test_factory_methods import test_all_factory_methods
from test_serialization import test_all_serialization
from test_visualization_and_debugging import test_all_visualization_and_debugging
from test_batch_operations import test_all_batch_operations
from test_integration import test_all_integration
from test_performance import test_all_performance
from test_edge_cases import test_all_edge_cases
from test_backward_compatibility import test_all_backward_compatibility
from test_example import test_all_examples

def run_tests_with_timing(test_function, test_name):
    """Run tests with timing and error handling."""
    print(f"\n{'='*60}")
    print(f"Running {test_name}")
    print(f"{'='*60}")
    
    start_time = time.time()
    passed_tests = 0
    total_tests = 0
    
    try:
        # Call the test function
        test_function()
        passed_tests = total_tests  # If we get here, all tests passed
        
    except Exception as e:
        print(f"❌ Test failed: {e}")
        traceback.print_exc()
        sys.exit(1)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"\n{'='*60}")
    print(f"{test_name} Summary")
    print(f"{'='*60}")
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print(f"Execution time: {elapsed:.2f} seconds")
    print(f"{'='*60}")
    
    if passed_tests == total_tests:
        print(f"✓ {test_name} completed successfully!")
    else:
        print(f"❌ {test_name} had failures!")
        sys.exit(1)

def main():
    """Main function to run the test suite."""
    parser = argparse.ArgumentParser(
        description="NLM Python Test Suite Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        "--test-category",
        choices=["all", "context", "convenience", "types", "factory",
                 "serialization", "visualization", "batch", "integration",
                 "performance", "edge", "backward", "example"],
        default="all",
        help="Specify which test category to run (default: all)"
    )
    
    parser.add_argument(
        "--output-file",
        help="File to write test results to"
    )
    
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    args = parser.parse_args()
    
    # Map test categories to test functions
    test_mapping = {
        "all": [
            (test_all_context_managers, "Context Manager Tests"),
            (test_all_convenience_methods, "Convenience Method Tests"),
            (test_all_type_hints_and_error_handling, "Type Hints and Error Handling Tests"),
            (test_all_factory_methods, "Factory Method Tests"),
            (test_all_serialization, "Serialization Tests"),
            (test_all_visualization_and_debugging, "Visualization and Debugging Tests"),
            (test_all_batch_operations, "Batch Operation Tests"),
            (test_all_integration, "Integration Tests"),
            (test_all_performance, "Performance Tests"),
            (test_all_edge_cases, "Edge Case Tests"),
            (test_all_backward_compatibility, "Backward Compatibility Tests"),
            (test_all_examples, "Example Tests"),
        ],
        "context": [(test_all_context_managers, "Context Manager Tests")],
        "convenience": [(test_all_convenience_methods, "Convenience Method Tests")],
        "types": [(test_all_type_hints_and_error_handling, "Type Hints and Error Handling Tests")],
        "factory": [(test_all_factory_methods, "Factory Method Tests")],
        "serialization": [(test_all_serialization, "Serialization Tests")],
        "visualization": [(test_all_visualization_and_debugging, "Visualization and Debugging Tests")],
        "batch": [(test_all_batch_operations, "Batch Operation Tests")],
        "integration": [(test_all_integration, "Integration Tests")],
        "performance": [(test_all_performance, "Performance Tests")],
        "edge": [(test_all_edge_cases, "Edge Case Tests")],
        "backward": [(test_all_backward_compatibility, "Backward Compatibility Tests")],
        "example": [(test_all_examples, "Example Tests")],
    }
    
    # Determine which tests to run
    if args.test_category == "all":
        test_functions = test_mapping["all"]
    else:
        test_functions = test_mapping[args.test_category]
    
    # Run tests
    all_passed = True
    for test_function, test_name in test_functions:
        try:
            run_tests_with_timing(test_function, test_name)
        except Exception as e:
            print(f"❌ {test_name} failed: {e}")
            all_passed = False
            break
    
    # Write results to file if requested
    if args.output_file and all_passed:
        with open(args.output_file, "w") as f:
            f.write(f"All tests passed successfully at {time.ctime()}\n")
            f.write(f"Tests run: {', '.join([name.split()[0] for _, name in test_functions])}\n")
        print(f"Test results written to {args.output_file}")
    
    if all_passed:
        print(f"\n{'='*60}")
        print("🎉 ALL TESTS PASSED! 🎉")
        print(f"{'='*60}")
        print(f"Tests run: {len(test_functions)} categories")
        print(f"All tests completed successfully!")
        print(f"{'='*60}")
        return 0
    else:
        print(f"\n{'='*60}")
        print("❌ TESTS FAILED! ❌")
        print(f"{'='*60}")
        return 1

if __name__ == "__main__":
    sys.exit(main())