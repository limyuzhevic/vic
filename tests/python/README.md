# NLM Python Bindings Test Suite

This directory contains Python tests for the Neural Learning Machine (NLM) Python bindings.

## Overview

The test suite validates the functionality exposed through the `pybind11` Python bindings for the NLM C++ library. Tests cover:

- **Config class**: Configuration management, key-value operations, batch loading
- **Brain class**: Neural brain creation, initialization, and basic operations
- **AgentBrain class**: Agent brain interface with world interaction
- **SimpleWorld class**: World simulation environment
- **Helper functions**: Utility functions for creating NLM objects
- **Error handling**: Exception handling and error conditions

## Prerequisites

1. **NLM C++ Library**: The NLM library must be built with Python bindings enabled
2. **Python Environment**: Python 3.6 or higher
3. **Dependencies**: Standard Python libraries (unittest, etc.)

## Running Tests

### Method 1: Using the Test Runner Script

Run the provided test runner script:

```bash
python tests/python/run_tests.py
```

This will:
- Load the pynlm Python module
- Run all test cases
- Provide detailed output with pass/fail results
- Return appropriate exit codes (0 for success, 1 for failures)

### Method 2: Using Python's unittest Module

Run the tests directly using Python's unittest module:

```bash
cd tests/python
python -m unittest discover -v
```

### Method 3: Individual Test Classes

Run specific test classes:

```bash
python -m unittest tests.python.test_bindings.TestConfigBindings -v
python -m unittest tests.python.test_bindings.TestBrainBindings -v
python -m unittest tests.python.test_bindings.TestAgentBrainBindings -v
```

## Test Structure

Each test class inherits from `TestPythonBindings` (base class) and `unittest.TestCase`:

### Base Test Class
- `setUp()`: Initializes test fixtures (checks for pynlm module)
- `tearDown()`: Cleans up after tests
- `skipTest()`: Skips tests if Python bindings are unavailable

### Test Classes

#### TestConfigBindings
Tests all Config class methods:
- `testConfigCreation`: Basic config creation
- `testConfigSetGet`: Setting and getting values
- `testConfigGetOr`: Default value handling
- `testConfigHas`: Key existence checking
- `testConfigRemove`: Key removal
- `testConfigKeys`: Getting all keys
- `testConfigClear`: Clearing configuration
- `testConfigSummary`: Getting configuration summary
- `testConfigBatchSet`: Batch configuration loading
- `testConfigBatchRemove`: Batch key removal

#### TestBrainBindings
Tests Brain class functionality:
- `testBrainCreation`: Basic brain creation
- `testBrainInitialize`: Brain initialization
- `testBrainGetConfig`: Configuration access
- `testBrainGetRegionCount`: Region count retrieval
- `testBrainGetNeuronCount`: Neuron count retrieval
- `testBrainGetSynapseCount`: Synapse count retrieval

#### TestAgentBrainBindings
Tests AgentBrain functionality:
- `testAgentBrainCreation`: AgentBrain creation
- `testAgentBrainInitialize`: AgentBrain initialization
- `testAgentBrainProcessSensoryInput`: Sensory input processing

#### TestWorldBindings
Tests SimpleWorld functionality:
- `testWorldCreation`: World creation
- `testWorldConfigure`: World configuration
- `testWorldReset`: World reset
- `testWorldUpdate`: World updates

#### TestVisualizationBindings
Tests VisualizationInterface (if exposed):
- `testVisualizationCreation`: Visualization creation

#### TestHelperFunctions
Tests helper functions:
- `testCreateDefaultConfig`: Default config creation
- `testCreateBrain`: Brain creation helper
- `testCreateSimpleWorld`: World creation helper
- `testCreateAgentBrain`: AgentBrain creation helper

#### TestErrorHandling
Tests error handling:
- `testExceptionHandling`: Exception handling and conversion

## Test Features

### Config Tests
- Comprehensive testing of all Config methods
- Batch operations (if supported)
- Error condition testing
- Edge case testing (empty config, missing keys, etc.)

### Brain Tests
- Initialization with different configurations
- State validation after initialization
- Performance testing

### World Tests
- World configuration testing
- State management
- Update loop testing

### Integration Tests
- End-to-end testing of brain-world interaction
- Complete workflow testing
- Stress testing with different configurations

## Test Results Interpretation

### Successful Test Run
```
========================================================================
✓ All tests passed!
========================================================================
```

### Test Run with Failures
```
========================================================================
✗ Some tests failed!

Failures: 2
Errors: 1

========================================================================
```

### Individual Test Results
Each test case provides detailed output showing:
- Test name
- Pass/fail status
- Any error messages or stack traces

## Test Limitations

### Python Bindings Availability
- Tests are skipped if the `pynlm` module is not available
- This can happen if:
  - The NLM library is not built
  - Python bindings were not enabled during compilation
  - Module installation issues

### Feature Availability
- Some tests may be skipped based on available features
- New features added to the bindings will be automatically tested when added

### Performance Testing
- Performance tests are minimal
- For comprehensive performance testing, use dedicated benchmarking tools

## Adding New Tests

To add new tests to the suite:

1. **Create a new test class** inheriting from `TestPythonBindings` and `unittest.TestCase`
2. **Implement test methods** for each functionality you want to test
3. **Add to the test suite** in `run_tests.py` if needed
4. **Run the tests** to verify your implementation

Example test class:

```python
class TestNewFeature(TestPythonBindings, unittest.TestCase):
    def testNewFeatureCreation(self):
        # Test creation of new feature
        pass
    
    def testNewFeatureFunctionality(self):
        # Test new feature functionality
        pass
```

## Troubleshooting

### "pynlm module not found" Error

If you see this error, ensure that:

1. **Library built with Python bindings**:
   - NLM must be built with pybind11 support
   - Check CMake options during build

2. **Python module path**:
   - The module should be installable in your Python environment
   - Try: `python -c "import pynlm; print(pynlm.__version__)"`

3. **Build artifacts**:
   - The shared library (`.so` or `.pyd` file) must be in a directory in your `PYTHONPATH`

### Import Errors

If you get import errors in specific tests:

1. **Missing dependencies**: Ensure all required Python packages are installed
2. **Path issues**: Check that the test suite is being run from the correct directory
3. **Module structure**: Verify that the Python bindings module structure is correct

### Test Failures

If tests are failing:

1. **Check error messages**: Most failures will include detailed error messages
2. **Verify bindings**: Ensure your implementation matches the expected API
3. **Review test expectations**: Check if test assumptions need to be updated

## Integration with C++ Tests

The Python bindings tests complement the existing C++ unit tests (`tests/test_*.cpp` files). While the C++ tests focus on the core library functionality, the Python tests validate the user-facing API through the Python bindings.

Together, these tests provide comprehensive coverage of the NLM library:
- **C++ tests**: Core functionality, algorithms, performance
- **Python tests**: User API, ease of use, integration

## Conclusion

The NLM Python Bindings Test Suite provides comprehensive testing of the Python interface to the NLM neural simulation library. It ensures that the bindings are correctly implemented, handle edge cases properly, and provide a robust user experience.

Run the tests regularly to:
- Verify bindings functionality after changes
- Catch regressions early
- Ensure compatibility across different Python environments
- Maintain test coverage for all exposed APIs
