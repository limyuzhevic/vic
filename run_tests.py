# Test runner script to run all tests

import pytest
import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Run tests with different options
def main():
    # Parse command line arguments
    args = sys.argv[1:]
    
    # Default test options
    test_options = [
        "-v",  # Verbose output
        "--tb=short",  # Short traceback
        "--disable-warnings",  # Disable warnings
    ]
    
    # Add custom arguments if provided
    if "--no-cov" not in args:
        test_options.extend(["--cov=pynlm", "--cov-report=term-missing"])
    
    # Run pytest
    exit_code = pytest.main(test_options + args)
    sys.exit(exit_code)

if __name__ == "__main__":
    main()