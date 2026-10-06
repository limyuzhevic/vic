# PYNLM Python Bindings Test Suite

## Overview

This is a comprehensive test suite for the pynlm (Python bindings for NLM) neural simulation framework. The test suite provides comprehensive validation of the Python bindings through multiple test categories.

## Test Categories

### 1. Basic Bindings Test (`test_basic_bindings.py`)

**Purpose**: Test that all major classes can be created and basic methods work.

**Test Cases**:
- ID classes creation (NeuronId, SynapseId, RegionId, PopulationId)
- Enum types exist and have expected values
- Config class basic functionality
- Sensory input classes (Vision, Audio, InternalSignals)
- Action class functionality
- Factory functions for creating main objects
- Module constants accessibility

**Key Methods Tested**:
- Object constructors and parameter initialization
- Basic getters and setters
- Object cloning and copying
- Equality and comparison operations

### 2. Integration Tests (`test_integration.py`)

**Purpose**: Test that brain, world, and agent work together.

**Test Cases**:
- Brain-world integration with configuration and initialization
- Agent brain integration with world interaction
- Complete sensory-motor processing loop
- Simulation step integration
- Multiple component creation and interaction

**Key Integration Points**:
- Brain initialization with configuration
- World configuration and agent positioning
- Agent brain initialization and sensory processing
- Motor action production and application

### 3. Memory System Tests (`test_memory_systems.py`)

**Purpose**: Test that memory systems are accessible via Python bindings.

**Test Cases**:
- Working memory access and region management
- Neuromodulation system access
- Memory-related constants and invalid IDs
- Memory system type information
- Memory system statistics and metrics

**Memory Systems Tested**:
- Working memory (via brain regions)
- Episodic memory (via brain regions)
- Neuromodulation integration
- Memory statistics collection

### 4. Neuromodulation Tests (`test_neuromodulation.py`)

**Purpose**: Test that neuromodulation systems work.

**Test Cases**:
- Reward modulation system
- Curiosity neuromodulation
- Novelty detection neuromodulation
- Prediction error neuromodulation
- Plasticity modulation through neuromodulators
- All neuromodulators together

**Neuromodulators Tested**:
- Reward system
- Curiosity system
- Novelty detection
- Prediction error
- Plasticity modulation

### 5. Checkpoint Tests (`test_checkpoints.py`)

**Purpose**: Test save/load functionality.

**Test Cases**:
- Config save/load functionality
- Brain save/load functionality
- Checkpoint file persistence
- Checkpoint with different configurations
- Error handling in checkpoint operations

**Checkpoint Operations Tested**:
- File save operations
- File load operations
- State preservation across save/load cycles
- Error conditions and handling

### 6. Error Handling Tests (`test_error_handling.py`)

**Purpose**: Test error conditions and exception handling.

**Test Cases**:
- Invalid neuron ID operations
- Invalid file operations
- Brain error conditions
- World error conditions
- Agent brain error conditions
- Memory system errors
- Neuromodulation error conditions
- Checkpoint error conditions

**Error Conditions Tested**:
- Invalid ID operations
- Invalid file paths
- Uninitialized component operations
- Edge case value handling
- Corrupted file loading

### 7. Performance Tests (`test_performance.py`)

**Purpose**: Basic performance validation.

**Test Cases**:
- Object creation performance
- Brain initialization performance
- Config operations performance
- Basic memory usage characteristics
- Object lifecycle performance
- Concurrent operations simulation
- Performance consistency

**Performance Metrics Tested**:
- Object creation time
- Initialization time
- Operation execution time
- Memory allocation patterns
- Consistency across runs

## Running the Tests

### Prerequisites

1. Install pytest and numpy:
   ```bash
   pip3 install pytest numpy
   ```

2. Ensure the pynlm module is built and accessible:
   - The module should be built using scikit-build-core
   - The bindings.cpp file contains the pybind11 bindings

### Running Tests

Run the comprehensive test suite:

```bash
pytest tests/test_runner.py -v
```

Run specific test categories:

```bash
# Basic bindings tests
pytest tests/test_basic_bindings.py -v

# Integration tests
pytest tests/test_integration.py -v

# Memory system tests
pytest tests/test_memory_systems.py -v

# Neuromodulation tests
pytest tests/test_neuromodulation.py -v

# Checkpoint tests
pytest tests/test_checkpoints.py -v

# Error handling tests
pytest tests/test_error_handling.py -v

# Performance tests
pytest tests/test_performance.py -v
```

Run all tests at once:

```bash
pytest tests/ -v --tb=short
```

### Test Output Options

- `-v`: Verbose output with test names
- `--tb=short`: Short traceback format
- `--capture=no`: Show print statements in tests
- `--maxfail=5`: Stop after first 5 failures
- `-x`: Exit on first failure

## Test Structure

### Test Classes

Each test file contains test classes that follow pytest naming conventions:
- `TestClassName` for test cases
- `test_method_name` for individual test methods

### Test Fixtures

No fixtures are currently defined, but the structure supports future fixture introduction for:
- Test environment setup
- Configuration management
- Resource cleanup

### Error Handling

Tests are designed to handle both expected failures and exceptions:
- Missing modules are skipped with `pytest.skip()`
- Invalid operations are caught and handled gracefully
- Edge cases are tested with appropriate error conditions

## Test Data

### Test Objects Created

The test suite creates various test objects:

**ID Objects**:
- NeuronId instances (with and without parameters)
- SynapseId instances
- RegionId instances
- PopulationId instances

**Config Objects**:
- Default configuration
- Configurations with test data
- Modified configurations

**Brain Objects**:
- Brains with different configurations
- Brains with added regions

**World Objects**:
- SimpleWorld instances
- Configured worlds with dimensions
- Worlds with agent positioning

**Agent Objects**:
- AgentBrain instances
- Integrated brain-world agents

### Test Constants

The test suite uses various constants:
- Invalid ID constants (INVALID_NEURON_ID, etc.)
- Enum values (NeuronType.Excitatory, etc.)
- Action types (ActionType.MoveForward, etc.)
- Developmental stages (DevelopmentalStage.Initial, etc.)

## Test Quality Metrics

### Coverage

The test suite provides coverage for:
- All major pynlm classes
- Factory functions
- Basic methods and operations
- Error conditions
- Performance characteristics

### Robustness

Tests are designed to be:
- Self-contained (no external dependencies beyond pynlm)
- Isolated (each test should work independently)
- Reproducible (consistent results across runs)
- Scalable (can handle different object counts)

### Performance

Performance tests ensure:
- Object creation is reasonably fast
- Initialization doesn't cause excessive delays
- Operations complete within acceptable timeframes
- Memory usage is reasonable

## Integration with C++ Tests

This Python test suite complements the existing C++ tests (`test_*.cpp` files):

- **C++ tests**: Test the underlying C++ implementation
- **Python tests**: Test the Python bindings interface
- **Combined**: Provide full coverage of the NLM system

The Python tests verify that:
- The pybind11 bindings are correctly exposed
- Python objects can be created and manipulated
- The Python API matches the expected interface
- Integration between Python and C++ components works

## Future Enhancements

### Additional Test Categories

Consider adding:
- Stress tests with large numbers of objects
- Memory leak detection
- Thread safety tests
- Serialization/deserialization tests
- Custom action type tests

### Test Improvements

Potential improvements:
- Add pytest fixtures for common test setup
- Include performance benchmarks
- Add property-based testing with hypothesis
- Include integration tests with external systems
- Add documentation for test scenarios

## License

MIT License - See LICENSE file for details.

## Authors

NLM Development Team
Phase 6: Final Integration

## Version

1.0.0
