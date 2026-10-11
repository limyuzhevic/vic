#!/usr/bin/env python3
"""Test runner for enhanced NLM Python bindings."""
import sys
import os
import subprocess
import time

def run_tests_with_pytest():
    """Run tests using pytest."""
    print("Running enhanced NLM Python bindings tests with pytest...")
    print("=" * 70)
    
    # Change to the tests directory
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    # Run pytest on the tests directory
    cmd = [
        sys.executable, "-m", "pytest",
        "tests/",
        "-v",
        "--tb=short",
        "--disable-warnings"
    ]
    
    start_time = time.time()
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
        end_time = time.time()
        
        print("STDOUT:")
        print(result.stdout)
        
        if result.stderr:
            print("\nSTDERR:")
            print(result.stderr)
        
        print(f"\nExecution time: {end_time - start_time:.2f} seconds")
        print(f"Return code: {result.returncode}")
        
        return result.returncode == 0
        
    except Exception as e:
        print(f"Error running pytest: {e}")
        return False

def run_test_files_individually():
    """Run test files individually as Python scripts."""
    print("Running test files individually...")
    print("=" * 70)
    
    test_files = [
        "test_basic_functionality.py",
        "test_pythonic_features.py", 
        "test_error_handling.py",
        "test_advanced_features.py",
        "test_integration.py",
        "test_performance.py",
        "test_edge_cases.py",
        "test_memory_systems.py"
    ]
    
    passed = 0
    failed = 0
    
    for test_file in test_files:
        print(f"\n{'='*70}")
        print(f"Running: {test_file}")
        print(f"{'='*70}")
        
        try:
            # Find the test file in tests directory
            test_path = os.path.join("tests", test_file)
            if not os.path.exists(test_path):
                print(f"✗ {test_file} NOT FOUND")
                failed += 1
                continue
            
            result = subprocess.run(
                [sys.executable, test_path],
                capture_output=True,
                text=True,
                timeout=60
            )
            
            if result.returncode == 0:
                print(f"✓ {test_file} PASSED")
                passed += 1
            else:
                print(f"✗ {test_file} FAILED")
                print(f"STDOUT:\n{result.stdout}")
                if result.stderr:
                    print(f"STDERR:\n{result.stderr}")
                failed += 1
                
        except subprocess.TimeoutExpired:
            print(f"✗ {test_file} TIMEOUT")
            failed += 1
        except Exception as e:
            print(f"✗ {test_file} ERROR: {e}")
            failed += 1
    
    print(f"\n{'='*70}")
    print(f"Individual test results: {passed} passed, {failed} failed")
    print(f"{'='*70}")
    
    return failed == 0

def run_original_test_suite():
    """Run the original test_enhanced_api.py file."""
    print("\nRunning original test_enhanced_api.py...")
    print("=" * 70)
    
    try:
        # Import and run the test
        # Change to the tests directory
        original_cwd = os.getcwd()
        tests_dir = os.path.join(original_cwd, "tests")
        os.chdir(tests_dir)
        
        # Import the test functions
        from test_enhanced_api import (
            test_basic_functionality,
            test_error_handling,
            test_advanced_features,
            test_agent_features,
            test_world_features,
            test_utility_functions,
            test_error_context_manager,
            test_example_script,
            run_all_tests
        )
        
        # Run all tests
        success = run_all_tests()
        
        # Restore original directory
        os.chdir(original_cwd)
        
        if success:
            print("✓ Original test suite PASSED")
            return True
        else:
            print("✗ Original test suite FAILED")
            return False
            
    except Exception as e:
        # Restore original directory in case of error
        os.chdir(os.getcwd())
        print(f"Error running original test suite: {e}")
        return False

def create_test_summary():
    """Create a summary of all tests created."""
    print("\nTest Summary")
    print("=" * 70)
    
    test_files = [
        ("test_basic_functionality.py", "Basic functionality tests"),
        ("test_pythonic_features.py", "Pythonic properties and features"),
        ("test_error_handling.py", "Error handling and exceptions"),
        ("test_advanced_features.py", "Advanced features and utilities"),
        ("test_integration.py", "Integration and workflow tests"),
        ("test_performance.py", "Performance and benchmarking"),
        ("test_edge_cases.py", "Edge cases and boundary conditions"),
        ("test_memory_systems.py", "Memory systems integration")
    ]
    
    print("Created test files:")
    for filename, description in test_files:
        filepath = os.path.join("tests", filename)
        if os.path.exists(filepath):
            size = os.path.getsize(filepath)
            print(f"  ✓ {filename} ({size} bytes) - {description}")
        else:
            print(f"  ✗ {filename} - NOT FOUND")
    
    print(f"\nTotal test files: {len(test_files)}")
    print(f"Test directory structure: tests/")
    
    # Check for conftest.py
    conftest_path = os.path.join("tests", "conftest.py")
    if os.path.exists(conftest_path):
        print("✓ Configuration: conftest.py with pytest fixtures")
    
    # Check for test_enhanced_api.py
    original_test_path = os.path.join("tests", "test_enhanced_api.py")
    if os.path.exists(original_test_path):
        print("✓ Configuration: test_enhanced_api.py (original tests)")

def main():
    """Main test runner function."""
    print("Enhanced NLM Python Bindings - Test Suite")
    print("=" * 70)
    
    # Check if pytest is available
    try:
        import pytest
        pytest_available = True
        print("✓ pytest available")
    except ImportError:
        pytest_available = False
        print("✗ pytest not available")
    
    # Create test summary
    create_test_summary()
    
    # Run original test suite
    original_success = run_original_test_suite()
    
    # Run pytest if available
    if pytest_available:
        print("\n" + "=" * 70)
        pytest_success = run_tests_with_pytest()
    else:
        pytest_success = run_test_files_individually()
    
    # Final results
    print("\n" + "=" * 70)
    print("FINAL TEST RESULTS")
    print("=" * 70)
    
    tests_passed = 0
    tests_failed = 0
    
    if original_success:
        tests_passed += 1
        print("✓ Original test suite: PASSED")
    else:
        tests_failed += 1
        print("✗ Original test suite: FAILED")
    
    if pytest_success:
        tests_passed += 1
        print("✓ Pytest suite: PASSED")
    else:
        tests_failed += 1
        print("✗ Pytest suite: FAILED")
    
    print(f"\nOverall: {tests_passed} passed, {tests_failed} failed")
    
    if tests_failed == 0:
        print("\n🎉 All tests passed! Enhanced NLM Python bindings are working correctly.")
        return True
    else:
        print(f"\n❌ {tests_failed} test(s) failed. Please review the test output above.")
        return False

if __name__ == "__main__":
    try:
        success = main()
        sys.exit(0 if success else 1)
    except KeyboardInterrupt:
        print("\nTest execution interrupted by user.")
        sys.exit(1)
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)