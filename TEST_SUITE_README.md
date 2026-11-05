# NLM Python Bindings Test Suite

## Overview

This directory contains comprehensive Python binding tests for the NLM (Neural Learning Machine) Python bindings. The test suite validates all improvements made to the NLM Python bindings as specified in the Phase 6 Integration Improvements.

## Test Files

### 1. Basic API Tests (`tests/python/test_basic_api.py`)
Tests basic Python API functionality:
- Config class with new getInt, getFloat, getString methods
- Vision and Audio classes
- SensoryPercept with enhanced pixel access methods
- Basic class instantiation and method calls
- Integration between Python and C++ components

### 2. NumPy Support Tests (`tests/python/test_numpy_support.py`)
Tests numpy array support for vision and audio data:
- Vision data with 2D and 3D numpy arrays
- Audio data with 1D numpy arrays
- Convenience methods for numpy conversion
- Data validation and error handling
- NumPy dtype conversion and memory efficiency

### 3. Error Handling Tests (`tests/python/test_error_handling.py`)
Tests Python exception handling for C++ errors:
- Configuration error handling
- Brain operation error handling
- Invalid parameter validation
- Boundary condition checking
- Memory error handling
- Error recovery mechanisms

### 4. Convenience Methods Tests (`tests/python/test_convenience_methods.py`)
Tests high-level API convenience methods:
- BrainSimulation class for simulation management
- Agent-world simulation workflows
- NLMSamples for example and tutorial system
- Performance monitoring and benchmarking
- Configuration builders for common scenarios
- Configuration file management
- Visualization and debugging tools

### 5. Advanced Features Tests (`tests/python/test_advanced_features.py`)
Tests advanced features and utilities:
- Advanced configuration builder usage
- Simulation utilities and workflows
- Advanced performance benchmarking
- Advanced debugging and visualization tools
- Advanced configuration file management
- Advanced integration between components
- Advanced error handling and recovery
- Performance analysis and metrics

### 6. Integration Tests (`tests/python/test_integration.py`)
Tests integration between Python and C++ components:
- End-to-end integration of all components
- Component interaction between different systems
- Memory system integration
- Plasticity system integration
- Neuromodulation system integration
- Development system integration
- Visualization system integration
- Performance monitoring integration
- Cross-language compatibility
- Error propagation across components
- Resource management

### 7. Test Runner (`test_runner/run_tests.py`)
Comprehensive test runner script for executing all tests:
- Supports running specific test modules or all tests
- Provides multiple output formats (text, JSON, XML)
- Supports verbose output and progress reporting
- Generates detailed test reports for CI/CD integration

### 8. Setup Script (`setup_tests.py`)
Setup script for NLM Python bindings tests:
- Checks environment requirements
- Validates Python version
- Verifies required modules
- Sets up test directory structure
- Provides setup verification

### 9. Test Suite Summary (`TEST_SUITE_SUMMARY.md`)
Comprehensive documentation of the test suite:
- Test overview and objectives
- Detailed file descriptions
- Test structure and organization
- Running instructions and examples
- Test coverage analysis
- Best practices and maintenance guidelines

## Key Areas Covered

### 1. Config Class Testing
- New `getInt`, `getFloat`, `getString` methods
- Input validation and type safety
- Error handling for invalid keys
- Configuration persistence testing

### 2. Vision and Audio Classes Testing
- NumPy array support (2D, 3D for vision; 1D for audio)
- Data integrity validation
- Memory efficiency testing
- Edge case handling (empty arrays, large values, etc.)

### 3. SensoryPercept Enhanced Features
- Pixel access methods (`getPixelValue`, `setPixelValue`, `isDarkPixel`)
- Boundary condition handling
- Error propagation for out-of-bounds access

### 4. Convenience Methods Testing
- `BrainSimulation` class for high-level API
- `NLMSamples` for example and tutorial system
- `NLMBenchmark` for performance monitoring
- `BrainConfigBuilder` for common configuration scenarios
- `ConfigManager` for configuration file management

### 5. Error Handling Testing
- Comprehensive exception handling for C++ errors
- Python-specific error types (NLMError, ConfigurationError, etc.)
- Input validation and sanitization
- Error recovery mechanisms

### 6. Integration Testing
- End-to-end system testing
- Component interaction validation
- Cross-language compatibility
- Resource management and cleanup

## Running the Tests

### Prerequisites
1. Python 3.8+ installed
2. Required packages: pytest, numpy, pynlm
3. NLM Python bindings installed

### Installation
```bash
pip3 install pytest numpy pynlm
```

### Running Tests

#### Method 1: Using Setup Script (Recommended)
```bash
python3 setup_tests.py
```

#### Method 2: Using Test Runner Directly
```bash
# Run all tests
python3 test_runner/run_tests.py --all

# Run specific test modules
python3 test_runner/run_tests.py --module basic_api numpy_support

# Run with verbose output
python3 test_runner/run_tests.py --all --verbose

# Generate XML report for CI/CD
python3 test_runner/run_tests.py --all --xml-report --output ci_test_report.xml
```

#### Method 3: Using Pytest (If Available)
```bash
# Run individual test files
pytest tests/python/test_basic_api.py -v
pytest tests/python/test_numpy_support.py -v

# Run all test files
pytest tests/python/ -v
```

## Test Results Format

### Text Output
```
NLM PYTHON BINDINGS TEST REPORT
============================================================
Total Test Modules: 6
Total Tests: 61
Passed: 61
Failed: 0
Success Rate: 100.00%

PASSED: basic_api
  Tests: 13/13 passed, 0 failed

PASSED: numpy_support
  Tests: 14/14 passed, 0 failed

PASSED: error_handling
  Tests: 14/14 passed, 0 failed

PASSED: convenience_methods
  Tests: 11/11 passed, 0 failed

PASSED: advanced_features
  Tests: 10/10 passed, 0 failed

PASSED: integration
  Test Round 13/13 passed, 0 failed

============================================================
EXECUTION SUMMARY
============================================================
Total Test Modules: 6
Total Tests: 61
Passed: 61
Failed: 0

✓ ALL TESTS PASSED!
```

### JSON Output
```json
{
  "timestamp": "2026-11-05T19:45:00.000Z",
  "test_runner": "NLM Python Bindings Test Runner",
  "modules": [
    {
      "name": "basic_api",
      "passed": 13,
      "failed": 0,
      "total": 13,
      "failed_tests": []
    },
    {
      "name": "numpy_support",
      "passed": 14,
      "failed": 0,
      "total": 14,
      "failed_tests": []
    }
  ],
  "summary": {
    "total_modules": 6,
    "total_tests": 61,
    "total_passed": 61,
    "total_failed": 0,
    "success_rate": 100.0
  }
}
```

### XML Output (CI/CD Compatible)
```xml
<?xml version="1.0" encoding="UTF-8"?>
<testsuites>
  <testsuite name="basic_api" tests="13" failures="0" errors="0">
    <testcase name="test_config_creation"/>
    <testcase name="test_brain_creation"/>
    <!-- ... more test cases ... -->
  </testsuite>
  <testsuite name="numpy_support" tests="14" failures="0" errors="0">
    <testcase name="test_vision_numpy_setter"/>
    <testcase name="test_vision_numpy_getter"/>
    <!-- ... more test cases ... -->
  </testsuite>
  <!-- ... more test suites ... -->
</testsuites>
```

## Test Coverage

The test suite provides comprehensive coverage of all NLM Python API improvements:

| Category | Number of Tests | Coverage Areas |
|----------|----------------|---------------|
| Basic API | 13 | Config, Brain, SimpleWorld, AgentBrain |
| NumPy Support | 14 | Vision, Audio, Data Conversion, Memory |
| Error Handling | 14 | Exception Handling, Validation, Recovery |
| Convenience Methods | 11 | BrainSimulation, NLMSamples, Performance Monitoring |
| Advanced Features | 10 | Configuration Builders, Debugging Tools, Integration |
| Integration | 13 | End-to-End, Component Interaction, Resource Management |
| **Total** | **61** | **Comprehensive NLM Testing** |

## Best Practices

1. **Type Safety**: Tests validate Python type safety for all operations
2. **Error Handling**: Comprehensive testing of error conditions and recovery
3. **Integration**: End-to-end testing of all component interactions
4. **Performance**: Performance benchmarking and memory efficiency tests
5. **Documentation**: Tests include comprehensive docstrings and examples
6. **CI/CD Ready**: Multiple output formats for continuous integration

## Maintenance

### Adding New Tests
1. Create new test files in `tests/python/` following existing patterns
2. Implement test classes with descriptive names
3. Add global test functions for compatibility
4. Update `test_runner/run_tests.py` to import new test modules

### Updating Tests
1. Follow existing test structure and conventions
2. Maintain consistent naming conventions
3. Include comprehensive docstrings
4. Ensure backward compatibility

### Test Organization
- Use descriptive class and method names
- Include comprehensive docstrings
- Follow pytest conventions
- Test both happy path and error conditions

## Benefits

1. **Comprehensive Coverage**: Tests all NLM Python API improvements
2. **CI/CD Ready**: Multiple output formats for automation
3. **Performance Focused**: Includes performance benchmarking
4. **Error Handling**: Comprehensive error handling and validation
5. **Documentation**: Clear test documentation and examples
6. **Maintainable**: Well-organized and maintainable test structure

## Conclusion

This comprehensive test suite validates all improvements made to the NLM Python bindings, ensuring:

1. **Functionality**: All basic and advanced features work correctly
2. **Type Safety**: Python type checking and validation
3. **Error Handling**: Robust exception handling for all error conditions
4. **Integration**: Seamless integration between Python and C++ components
5. **Performance**: Efficient implementation with minimal overhead
6. **Documentation**: Clear, comprehensive test documentation
7. **CI/CD**: Ready for continuous integration and deployment

The test suite provides confidence in the quality and reliability of the NLM Python bindings and serves as a foundation for future enhancements and maintenance.

## Usage Summary

### Quick Start
```bash
# Check setup
python3 setup_tests.py --check-only

# Run all tests
python3 setup_tests.py

# Run specific tests
python3 test_runner/run_tests.py --module basic_api numpy_support
```

### CI/CD Integration
```bash
# Generate XML report for CI
python3 test_runner/run_tests.py --all --xml-report --output test_report.xml

# Exit with non-zero if tests fail
exit_code=$?
if [ $exit_code -eq 0 ]; then
    echo "✓ All tests passed"
else
    echo "✗ Tests failed"
    exit $exit_code
fi
```

## File Organization

- `tests/python/`: All test files
- `test_runner/`: Test runner and utilities
- `TEST_SUITE_SUMMARY.md`: Detailed test documentation
- `TEST_SUITE_README.md`: This usage guide
- `setup_tests.py`: Setup and verification script

## Contact

For questions or issues with the test suite:
- Review documentation in `TEST_SUITE_SUMMARY.md`
- Check documentation in `TEST_SUITE_README.md`
- Use the setup script for troubleshooting

---

**Test Suite Version**: Phase 6 Integration Improvements
**Created**: November 5, 2026
**Status**: Ready for Production Use
