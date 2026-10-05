"""
NLM Testing Infrastructure Package

This package provides comprehensive testing infrastructure for the NLM
(Neural Learning Machine) project, including unit tests, integration tests,
performance benchmarks, and automated test reporting.

The testing infrastructure is designed to be:
- Comprehensive: Tests all major NLM components and systems
- Automated: Provides automated test execution and reporting
- Modular: Organized into focused test suites
- Extensible: Easy to add new tests and test scenarios
- Maintainable: Follows modern Python testing practices

Key Features:
- Unit testing for core NLM functionality
- Integration testing for system components
- Performance benchmarking
- Error handling and edge case testing
- Automated test reporting and analysis
- Cross-platform compatibility
- Comprehensive coverage analysis

Usage:
    # Run all tests
    python -m pytest tests/
    
    # Run comprehensive test suite
    python -m nlm_testing.comprehensive_test_runner
    
    # Run core functionality tests
    python -m nlm_testing.test_core_functionality
    
    # Run custom test files
    python custom_test_file.py

The testing infrastructure is designed to work with both the NLM C++ core
and the Python bindings, providing comprehensive test coverage across
the entire system.
"""

__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Comprehensive testing infrastructure for NLM"

# Core test modules
from .comprehensive_test_runner import (
    ComprehensiveTestRunner,
    run_all_tests,
    get_test_results,
    print_test_summary
)

from .test_core_functionality import TestNLMCoreFunctionality

# Export key testing components
__all__ = [
    # Test runners
    "ComprehensiveTestRunner",
    "run_all_tests",
    "get_test_results",
    "print_test_summary",
    
    # Test suites
    "TestNLMCoreFunctionality",
    
    # Test utilities
    "discover_test_files",
]

# Testing configuration
def get_test_config():
    """Get test configuration"""
    return {
        'test_paths': [
            'src/nlm_testing/',
            'tests/',
        ],
        'exclude_patterns': [
            '__pycache__',
            '*.pyc',
            '.pytest_cache',
            'build/',
            'dist/',
        ],
        'test_timeout': 300,  # seconds
        'coverage_threshold': 0.8,
        'max_complexity': 10,
        'performance_benchmarks': [
            'response_time',
            'memory_usage',
            'throughput',
            'scalability',
        ],
    }

# Test discovery utilities
def discover_test_files():
    """
    Discover test files in the project.
    
    Returns:
        list: List of test file paths
    """
    import os
    import glob
    
    test_files = []
    
    # Look for test files in standard locations
    for pattern in [
        'src/nlm_testing/test_*.py',
        'tests/test_*.py',
        'test_*.py',
    ]:
        test_files.extend(glob.glob(pattern, recursive=True))
    
    return sorted(test_files)

# Test execution utilities
def run_test_suite():
    """
    Run the NLM test suite.
    
    Returns:
        bool: True if all tests passed, False otherwise
    """
    return run_all_tests()

# Test reporting utilities
def generate_test_report():
    """
    Generate a comprehensive test report.
    
    Returns:
        str: Formatted test report
    """
    results = get_test_results()
    
    if not results:
        return "No test results available"
    
    report = "NLM TEST REPORT\n"
    report += "=" * 50 + "\n\n"
    
    total_tests = sum(r['total'] for r in results)
    passed_tests = sum(r['passed'] for r in results)
    failed_tests = sum(r['failed'] for r in results)
    
    report += f"Total test suites: {len(results)}\n"
    report += f"Total individual tests: {total_tests}\n"
    report += f"Tests passed: {passed_tests}\n"
    report += f"Tests failed: {failed_tests}\n"
    report += f"Success rate: {passed_tests/total_tests*100:.1f}%\n\n"
    
    report += "Individual suite results:\n"
    for result in results:
        status = "PASS" if result['failed'] == 0 else "FAIL"
        report += f"  {status} {result['suite']}: {result['passed']}/{result['total']} ({result['success_rate']:.1f}%)\n"
    
    return report

# Main execution
if __name__ == '__main__':
    print("NLM Testing Infrastructure")
    print("=" * 30)
    
    # Run comprehensive test suite
    success = run_test_suite()
    
    if success:
        print("\n✅ All tests passed!")
        sys.exit(0)
    else:
        print("\n❌ Some tests failed!")
        sys.exit(1)
        
# Import standard library modules
import sys