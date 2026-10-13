# NLM Test Suite

## Overview

This directory contains comprehensive tests for the NLM (Neural Learning Machine) neural simulation framework, including both C++ unit tests and Python integration tests.

## Test Categories

### 1. C++ Unit Tests

These test the core C++ components of NLM and run during the build process.

#### Current Test Files:

- `test_neuron.cpp` - Tests neuron functionality, LIF dynamics, spike detection
- `test_stdp.cpp` - Tests STDP plasticity rules and weight changes
- `test_synapse.cpp` - Tests synaptic transmission and dynamics
- `test_brain.cpp` - Tests brain coordination and integration
- `test_config.cpp` - Tests configuration system and validation
- `test_clock.cpp` - Tests simulation timing and scheduling
- `test_random.cpp` - Tests random number generation
- `test_main.cpp` - Tests main application functionality

#### Test Structure:
- Each test file contains namespace-scoped test functions
- Tests use simple assertions and output status messages
- Tests can be run standalone or integrated into a test suite
- C++ tests focus on low-level functionality and correctness

### 2. Python Integration Tests

These test the Python bindings and the enhanced Python API functionality.

#### Current Test File:

- `test_python_bindings.py` - Tests all new Python functionality including:
  - Pythonic property access (`brain.region_count`, `brain.total_neuron_count`)
  - Factory methods for common use cases (`create_demo_brain`, `create_small_brain`)
  - Simulation helpers (`run_brain_simulation`, `benchmark_brain`)
  - Configuration validation (`validate_brain_config`)
  - Feature comparison (`compare_brains`)
  - Enhanced agent functionality (all enable methods)
  - Complete integration scenarios

#### Python Test Features:
- Tests the enhanced Python bindings API
- Validates all new Pythonic interface features
- Tests error handling and validation
- Benchmarks and compares brain performance
- Tests complete agent-world interaction scenarios
- Uses `unittest`-compatible structure

## How to Run Tests

### Running C++ Tests

If the project was built with testing enabled, C++ tests can typically be run using:

```bash
# Using CTest (if available)
ctest

# Or by compiling and running specific test executables
# Example (test names vary based on build system):
./nlm_tests
./test_neuron
./test_stdp
./test_brain
```

**Prerequisites:**
- NLM must be built with tests enabled
- Build directory with test executables
- Required C++ build tools

### Running Python Tests

The Python tests can be run directly with Python:

```bash
# Run all Python integration tests
cd tests
python3 test_python_bindings.py

# Run with verbose output for detailed results
python3 test_python_bindings.py -v
```

**Prerequisites:**
- Python 3.8+ installed
- `pynlm` Python package installed (in development mode)
- Required Python packages (usually automatic)

## New Python API Features Being Tested

The Python integration tests cover these enhanced features:

### 1. Pythonic Property Access
```python
# Instead of brain.getTotalNeuronCount():
print(brain.region_count)           # Pythonic alternative
print(brain.total_neuron_count)     # Cleaner syntax
print(brain.total_synapse_count)    # More readable
```

### 2. Factory Methods
```python
# Instead of manual config setup:
brain = pynlm.createBrain(config)

# More convenient alternatives:
brain = pynlm.create_demo_brain()           # With all learning systems
brain = pynlm.create_small_brain(200)      # Optimized for testing
brain = pynlm.create_experimental_brain(500, 3)  # Multi-region setup
```

### 3. Simulation Helpers
```python
# Instead of manual simulation setup:
for i in range(100):
    brain.step(i)

# More powerful alternatives:
spikes = pynlm.run_brain_simulation(brain, 100)
spikes, rate, duration = pynlm.benchmark_brain(brain, 200)
```

### 4. Configuration Validation
```python
# Safe configuration with validation:
config = pynlm.createDefaultConfig()
is_valid, message = config.validate()
if is_valid:
    # Use configuration safely
    config.set("neuron_count", 1000)
```

### 5. Feature Comparison
```python
# Compare two brains:
spikes1, spikes2, ratio, rate_ratio = pynlm.compare_brains(brain1, brain2, 200)
```

### 6. Enhanced Agent Interface
```python
# Enable all learning systems easily:
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Query system levels:
neuromod_level = agent.get_neuromodulation_level()
curiosity_level = agent.get_curiosity_level()
```

## Test Coverage

### C++ Tests Coverage:
- **Neuron Tests:** Creation, typing, membrane potential, firing states, refractory periods, synaptic input, spike history, random initialization, reset operations
- **STDP Tests:** Weight configuration, potentiation/depression, empty spike handling, weight bounds
- **Brain Tests:** Integration testing across multiple components

### Python Integration Tests Coverage:
- **Basic API:** Factory methods and property access
- **Pythonic Interface:** Clean property access and context management
- **Validation:** Configuration validation and error handling
- **Simulation Helpers:** Running, benchmarking, and comparing simulations
- **Agent Enhancements:** All enable methods and level queries
- **Integration:** Complete agent-world interaction scenarios
- **Property Access:** Comprehensive Pythonic interface testing

## Testing Philosophy

### C++ Tests:
- Focus on **correctness** and **low-level functionality**
- Test individual components in isolation
- Validate mathematical correctness of neural dynamics
- Ensure robustness of core algorithms

### Python Tests:
- Focus on **usability** and **developer experience**
- Test the **enhanced Python API** features
- Validate **integration scenarios** and **end-to-end functionality**
- Benchmark **performance** of new features

## Best Practices Demonstrated

### Code Quality:
- **Pythonic interface:** Clean, readable property access
- **Error handling:** Proper validation and meaningful error messages
- **Performance:** Benchmarks and comparisons
- **Integration:** Complete end-to-end scenarios

### Testing:
- **Comprehensive coverage:** All new features tested
- **Integration testing:** Python bindings thoroughly exercised
- **Real-world scenarios:** Complete agent-world simulations
- **Performance testing:** Benchmarking of new API features

## Future Test Expansion

The test suite can be expanded to include:

1. **More C++ unit tests:** For additional plasticity rules and neural components
2. **Advanced Python scenarios:** Complex learning environments and multi-agent systems
3. **Performance regression tests:** Benchmark comparisons across versions
4. **Stress tests:** Large-scale brain simulations and memory usage
5. **Cross-language tests:** Ensure consistency between C++ and Python implementations

## Conclusion

This test suite ensures that NLM's enhanced Python bindings are **robust**, **well-tested**, and **ready for production use**. The comprehensive testing validates both the **correctness** of core functionality and the **usability** of the enhanced Python API.

The tests demonstrate that NLM has successfully transitioned from a primarily C++ framework to one with **excellent Python support**, maintaining the computational power of the C++ core while providing **powerful, Pythonic interfaces** for developers and researchers.

**Key Achievement:** The enhanced Python bindings provide both **low-level access** to NLM's computational power and **high-level convenience** for rapid prototyping and research.

---

*Last updated: 2026-10-13*
*NLM Project: Neural Learning Machine (熙然)*