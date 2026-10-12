#!/usr/bin/env python3
"""
Simple Test Runner for NLM Python Bindings

Streamlined test runner for the consolidated NLM Python bindings test suite.
"""

import sys
import os


def main():
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    print()
    print("This test suite provides comprehensive testing for the NLM Python bindings")
    print("with the following focus areas:")
    print()
    print("Core Functionality Tests:")
    print("  - Basic object creation and initialization")
    print("  - Enum value validation")
    print("  - Configuration management")
    print()
    print("Integration Tests:")
    print("  - Complete brain simulation workflows")
    print("  - Agent-world interactions")
    print("  - Brain serialization/deserialization")
    print()
    print("Performance Tests:")
    print("  - Brain creation performance")
    print("  - Simulation step performance")
    print()
    print("Error Handling:")
    print("  - Input validation")
    print("  - Exception propagation")
    print()
    print("Architecture:")
    print("  - Single consolidated test file: test_pynlm_basic.py")
    print("  - Common fixtures: conftest.py")
    print("  - Reduced from 9 files (~4,600 lines) to 1 file (~900 lines)")
    print()


def run_tests():
    """Run the consolidated test suite."""
    try:
        import pytest
        import sys

        # Run pytest on the consolidated test file
        test_args = [
            "test_pynlm_basic.py",
            "-v",
            "--tb=short",
            "--timeout=300",
            "-x",  # Stop on first failure
        ]

        print("Running tests...")
        print("=" * 50)
        return pytest.main(test_args)

    except ImportError:
        print("pytest not found. Please install pytest:")
        print("  pip install pytest")
        return 1


if __name__ == "__main__":
    # Show help if --help is provided
    if "--help" in sys.argv or "-h" in sys.argv:
        main()
        sys.exit(0)

    # Run tests normally
    exit_code = run_tests()
    sys.exit(exit_code)