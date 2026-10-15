#!/usr/bin/env python3
"""
NLM Test Suite Configuration
Enhanced Testing Framework for NLM (Neural Learning Machine)
"""

import pytest
import sys
import os
from pathlib import Path

# Add the project root to the Python path
sys.path.insert(0, str(Path(__file__).parent))

# Configure pytest plugins
pytest_plugins = [
    "pytest_asyncio",
    "pytest_benchmark",
]

# Pytest configuration
pytest_configure = """
[pytest]
testpaths = [
    "tests/pytest",
    "tests/integration",
    "tests/e2e",
    "tests/performance",
    "tests/validation",
    "tests/advanced"
]
python_files = [
    "test_*.py",
    "*_test.py"
]
python_classes = [
    "Test*",
    "*Test"
]
python_functions = [
    "test_*",
    "*_test"
]
python_path = [
    "src",
    "python"
]
markers = [
    "unit: Unit tests for individual components",
    "integration: Integration tests for cross-component functionality",
    "e2e: End-to-end simulation tests",
    "performance: Performance and load tests",
    "validation: Schema validation and consistency tests",
    "advanced: Advanced user feature tests",
    "slow: Tests that take significant time to run",
    "memory: Tests that check memory usage",
    "config: Configuration system tests",
    "bindings: Python bindings tests",
    "cli: CLI interface tests"
]
addopts = [
    "-v",
    "--tb=short",
    "--disable-warnings",
    "-x",  # Stop on first failure
]
"""

def pytest_collection_modifyitems(config, items):
    """Modify test collection to add markers"""
    for item in items:
        # Add category markers based on test file location
        if "config" in item.location[0]:
            item.add_marker(pytest.mark.config)
        elif "bindings" in item.location[0]:
            item.add_marker(pytest.mark.bindings)
        elif "cli" in item.location[0]:
            item.add_marker(pytest.mark.cli)
        elif "integration" in item.location[0]:
            item.add_marker(pytest.mark.integration)
        elif "e2e" in item.location[0]:
            item.add_marker(pytest.mark.e2e)
        elif "performance" in item.location[0]:
            item.add_marker(pytest.mark.performance)
        elif "validation" in item.location[0]:
            item.add_marker(pytest.mark.validation)
        elif "advanced" in item.location[0]:
            item.add_marker(pytest.mark.advanced)

def pytest_runtest_setup(item):
    """Setup for each test"""
    # Initialize test environment
    pass

def pytest_runtest_teardown(item, nextitem):
    """Teardown after each test"""
    # Cleanup test environment
    pass

def pytest_sessionstart(session):
    """Called before any tests are collected"""
    print("🚀 Starting NLM Enhanced Test Suite")
    print("📊 Test coverage targets:")
    print("   - Configuration system: 100%")
    print("   - Python bindings: 100%")
    print("   - Integration: 80%")
    print("   - Performance: Comprehensive")
    print("   - End-to-End: Multiple scenarios")
    print("   - Validation: Schema consistency")
    print("   - Advanced features: Power user capabilities")

def pytest_sessionfinish(session, exitstatus):
    """Called after all tests are finished"""
    print(f"✅ Test session finished with exit code: {exitstatus}")

if __name__ == "__main__":
    pytest.main(["-v", "--tb=short"])
