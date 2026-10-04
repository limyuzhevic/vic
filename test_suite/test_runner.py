#!/usr/bin/env python3
"""
Enhanced NLM Python API Test Suite Runner

This script provides a comprehensive test runner for the Enhanced NLM Python API,
covering all test categories specified in the requirements.
"""

import os
import sys
import argparse
import time
import json
import subprocess
from pathlib import Path

def run_python_tests():
    """Run Python tests using pytest"""
    print("=" * 60)
    print("Running Python Tests")
    print("=" * 60)
    
    test_dir = Path(__file__).parent / "python"
    if not test_dir.exists():
        print(f"ERROR: Test directory {test_dir} not found")
        return False
    
    # Import pytest and run tests
    try:
        import pytest
        
        # Run all Python tests
        exit_code = pytest.main([
            str(test_dir),
            "-v",           # Verbose output
            "--tb=short",   # Short traceback format
            "--disable-warnings",  # Disable warnings
            f"--cov={test_dir.name}"  # Coverage analysis if available
        ])
        
        return exit_code == 0
        
    except ImportError as e:
        print(f"ERROR: pytest not available: {e}")
        print("Please install pytest: pip install pytest")
        return False

def run_cpp_tests():
    """Run C++ tests using Google Test"""
    print("=" * 60)
    print("Running C++ Tests")
    print("=" * 60)
    
    # Check if build directory exists
    build_dir = Path(__file__).parent / "build"
    if not build_dir.exists():
        print("Building C++ tests...")
        # In a real implementation, we would run cmake and make
        # For now, just indicate that building is needed
        print("ERROR: Please build C++ tests first")
        print("Run: cmake -B build -S . && cmake --build build")
        return False
    
    # Run the test executable
    test_executable = build_dir / "nlm_test"
    if not test_executable.exists():
        print(f"ERROR: Test executable not found: {test_executable}")
        return False
    
    try:
        result = subprocess.run(
            [str(test_executable)],
            cwd=Path(__file__).parent,
            capture_output=True,
            text=True
        )
        
        print(result.stdout)
        if result.stderr:
            print("STDERR:", result.stderr)
        
        return result.returncode == 0
        
    except Exception as e:
        print(f"ERROR: Failed to run C++ tests: {e}")
        return False

def run_integration_tests():
    """Run integration tests between Python and C++"""
    print("=" * 60)
    print("Running Integration Tests")
    print("=" * 60)
    
    try:
        import pytest
        
        # Create integration test directory
        integration_dir = Path(__file__).parent / "integration"
        integration_dir.mkdir(exist_ok=True)
        
        # Run integration tests
        exit_code = pytest.main([
            str(integration_dir),
            "-v",
            "--tb=short",
            "--disable-warnings"
        ])
        
        return exit_code == 0
        
    except ImportError as e:
        print(f"ERROR: pytest not available: {e}")
        return False

def run_all_tests():
    """Run all tests and generate comprehensive report"""
    print("=" * 60)
    print("Enhanced NLM Python API Test Suite")
    print("=" * 60)
    
    results = {
        "python_tests": {"passed": False, "time": 0.0},
        "cpp_tests": {"passed": False, "time": 0.0},
        "integration_tests": {"passed": False, "time": 0.0},
        "total_tests": 0,
        "passed_tests": 0,
        "failed_tests": 0
    }
    
    # Run Python tests
    start_time = time.time()
    python_success = run_python_tests()
    python_time = time.time() - start_time
    results["python_tests"]["passed"] = python_success
    results["python_tests"]["time"] = python_time
    
    # Run C++ tests
    start_time = time.time()
    cpp_success = run_cpp_tests()
    cpp_time = time.time() - start_time
    results["cpp_tests"]["passed"] = cpp_success
    results["cpp_tests"]["time"] = cpp_time
    
    # Run integration tests
    start_time = time.time()
    integration_success = run_integration_tests()
    integration_time = time.time() - start_time
    results["integration_tests"]["passed"] = integration_success
    results["integration_tests"]["time"] = integration_time
    
    # Generate report
    print("\n" + "=" * 60)
    print("Test Suite Report")
    print("=" * 60)
    print(f"Python Tests: {'PASSED' if python_success else 'FAILED'} ({python_time:.2f}s)")
    print(f"C++ Tests: {'PASSED' if cpp_success else 'FAILED'} ({cpp_time:.2f}s)")
    print(f"Integration Tests: {'PASSED' if integration_success else 'FAILED'} ({integration_time:.2f}s)")
    print(f"Total Time: {python_time + cpp_time + integration_time:.2f}s")
    
    all_passed = python_success and cpp_success and integration_success
    if all_passed:
        print("\n✓ ALL TESTS PASSED")
    else:
        print("\n✗ SOME TESTS FAILED")
    
    # Save report
    report_file = Path(__file__).parent / "test_report.json"
    with open(report_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"Report saved to: {report_file}")
    
    return all_passed

def main():
    parser = argparse.ArgumentParser(description="Enhanced NLM Python API Test Suite")
    parser.add_argument("--mode", choices=["python", "cpp", "integration", "all"], 
                       default="all", help="Test mode to run")
    parser.add_argument("--help-tests", action="store_true", 
                       help="Show help about available tests")
    
    args = parser.parse_args()
    
    if args.help_tests:
        print("\nAvailable Test Categories:")
        print("1. Python Tests - Core Python API functionality")
        print("2. C++ Tests - Low-level neural component tests")
        print("3. Integration Tests - Python-C++ integration")
        print("4. Performance Tests - Scalability and stress testing")
        print("\nEach category includes multiple subcategories as specified in the requirements.")
        return
    
    if args.mode == "python":
        success = run_python_tests()
    elif args.mode == "cpp":
        success = run_cpp_tests()
    elif args.mode == "integration":
        success = run_integration_tests()
    else:
        success = run_all_tests()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()