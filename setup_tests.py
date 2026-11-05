#!/usr/bin/env python3
"""
NLM Python Bindings Test Setup Script

This script sets up the test environment for NLM Python bindings.
"""

import sys
import os
import subprocess
import argparse

def check_python_version():
    """Check Python version"""
    print(f"Python version: {sys.version}")
    
    # Check Python version (3.8+)
    major, minor, _ = sys.version_info[:3]
    if major < 3 or (major == 3 and minor < 8):
        print("ERROR: Python 3.8 or higher is required")
        return False
    
    print("✓ Python version check passed")
    return True

def check_required_modules():
    """Check required modules are available"""
    required_modules = ['pynlm', 'numpy', 'pytest']
    missing_modules = []
    
    for module in required_modules:
        try:
            __import__(module)
            print(f"✓ Module '{module}' is available")
        except ImportError:
            missing_modules.append(module)
            print(f"✗ Module '{module}' is not available")
    
    if missing_modules:
        print(f"\nMissing modules: {missing_modules}")
        print("Please install them using: pip install {missing_modules}")
        return False
    
    print("✓ All required modules are available")
    return True

def setup_test_directory():
    """Setup test directory structure"""
    test_dir = os.path.join(os.path.dirname(__file__), 'tests', 'python')
    
    if not os.path.exists(test_dir):
        print(f"Creating test directory: {test_dir}")
        os.makedirs(test_dir, exist_ok=True)
    
    print("✓ Test directory setup complete")

def create_test_runner():
    """Create test runner script"""
    test_runner_path = os.path.join(os.path.dirname(__file__), 'test_runner', 'run_tests.py')
    
    if not os.path.exists(test_runner_path):
        print(f"Test runner will be available at: {test_runner_path}")
    
    print("✓ Test runner setup complete")

def run_tests_directly():
    """Run tests directly using the test runner"""
    test_runner = os.path.join(os.path.dirname(__file__), 'test_runner', 'run_tests.py')
    
    if os.path.exists(test_runner):
        print(f"\nRunning tests using: {test_runner}")
        
        # Run with all tests and verbose output
        result = subprocess.run([
            sys.executable, test_runner,
            '--all',
            '--verbose',
            '--format', 'text'
        ], capture_output=True, text=True)
        
        print("Test output:")
        print(result.stdout)
        
        if result.stderr:
            print("Errors:")
            print(result.stderr)
        
        return result.returncode
    else:
        print(f"ERROR: Test runner not found at {test_runner}")
        return 1

def main():
    """Main function"""
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings Test Setup",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Setup script for NLM Python bindings tests.

The script checks the environment and runs the comprehensive test suite
for NLM Python bindings.

Usage examples:
  python3 setup_tests.py                    # Setup and run all tests
  python3 setup_tests.py --check-only       # Only check requirements
"""
    )
    
    parser.add_argument(
        '--check-only',
        action='store_true',
        help='Only check requirements, do not run tests'
    )
    
    args = parser.parse_args()
    
    print("NLM Python Bindings Test Setup")
    print("=" * 50)
    
    # Setup checks
    setup_checks = [
        check_python_version,
        check_required_modules,
        setup_test_directory,
        create_test_runner,
    ]
    
    # Run all setup checks
    all_passed = True
    for check in setup_checks:
        if not check():
            all_passed = False
        print()
    
    if not all_passed:
        print("ERROR: Setup checks failed")
        return 1
    
    if args.check_only:
        print("✓ All setup checks passed (check-only mode)")
        return 0
    
    # Run tests
    print("\nStarting test execution...")
    result = run_tests_directly()
    
    if result == 0:
        print("\n✓ All tests completed successfully")
    else:
        print(f"\n✗ Tests completed with errors (exit code: {result})")
    
    return result

if __name__ == "__main__":
    sys.exit(main())
