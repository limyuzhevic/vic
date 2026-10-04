#!/usr/bin/env python3
"""
NLM Enhanced Python API Test Suite
Comprehensive test suite for NLM Python bindings with C++ integration

This test suite provides:
1. Core Python API tests
2. C++ unit tests
3. Integration tests
4. Performance and stress tests
5. Python-specific tests
6. Configuration tests

Usage:
    python -m pytest test_suite/ -v
    python -m pytest test_suite/python/ -v
    python -m pytest test_suite/cpp_integration/ -v
"""

__version__ = "1.0.0"
__author__ = "NLM Test Suite"

import sys

# Add current directory to path for imports
sys.path.insert(0, __file__.replace("__init__.py", ""))

if __name__ == "__main__":
    print("NLM Enhanced Python API Test Suite")
    print("Run with: python -m pytest test_suite/ -v")