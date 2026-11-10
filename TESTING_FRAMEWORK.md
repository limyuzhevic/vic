# NLM Testing Framework

## Overview

This document describes the testing framework for NLM (Neural Learning Machine). The testing framework provides a comprehensive suite of tests to validate the correctness, performance, and reliability of the NLM system.

## Test Structure

### Test Categories

1. **Unit Tests**
   - Test individual components (neurons, synapses, plasticity rules)
   - Verify mathematical correctness of learning algorithms
   - Test memory management and data structures

2. **Integration Tests**
   - Test system interactions (memory, prediction, cognition)
   - Validate brain system integration
   - Test world-environment interactions

3. **Performance Tests**
   - Benchmark system performance and scalability
   - Test simulation speed with different configurations
   - Validate memory usage efficiency

4. **Learning Tests**
   - Verify learning mechanisms work correctly
   - Test pattern separation and selectivity
   - Validate reward-modulated learning

5. **Validation Tests**
   - Ensure system outputs are biologically plausible
   - Test convergence of learning algorithms
   - Validate parameter ranges and constraints

### Test Files

#### Core Unit Tests (`tests/`)

| Test File | Description |
|-----------|-------------|
| `test_brain.cpp` | Brain system integration tests |
| `test_neuron.cpp` | Neuron dynamics and LIF simulation |
| `test_synapse.cpp` | Synapse transmission and plasticity |
| `test_stdp.cpp` | Spike-timing-dependent plasticity |
| `test_clock.cpp` | Simulation clock and timing |
| `test_config.cpp` | Configuration management |
| `test_random.cpp` | Random number generation |
| `test_types.cpp` | Neural type system |

#### High-Level Tests

| Test | Purpose |
|------|---------|
| `nlm_phase3_demo` | Phase 3 world interaction demo |
| `nlm_phase6_demo` | Phase 6 integration test |
| `nlm_benchmark` | Performance benchmarks |

## Running Tests

### Using the Test Runner

The NLM test suite includes a convenient test runner script:

```bash
# Run all tests (requires building first)
chmod +x ./run_tests.sh
./run_tests.sh
```

### Individual Test Execution

Run specific test executables directly:

```bash
# Phase 2: Real Neural Computation
./nlm

# Phase 3: World Interaction
./nlm_phase3_demo [num_episodes] [steps_per_episode]

# Phase 6: Integration Test
./nlm_phase6_demo

# Performance Benchmarks
./nlm_benchmark
```

### Building Tests

If executables are not available, build the project first:

```bash
# Create build directory and build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run tests
../run_tests.sh
cd ..
```

## Test Results Interpretation

### Success Indicators

✅ **PASSED**: Test executed successfully with expected behavior
✅ **SKIPPED**: Test not applicable (executable not available)
✅ **FAILED**: Test execution failed or produced unexpected results

### Common Test Results

#### Neuron Dynamics Test (`./nlm`)
- **Expected**: Neural activity, spike generation, learning
- **Validation**: Check logs for spike counts, weight changes, learning progress
- **Success Criteria**: System shows activity-dependent synaptic modification

#### Phase 3 Demo Test (`./nlm_phase3_demo`)
- **Expected**: Agent explores world, interacts with objects, receives rewards
- **Validation**: Check reward accumulation, object consumption, navigation behavior
- **Success Criteria**: Reward > 0, agent learns to navigate and interact

#### Phase 6 Integration Test (`./nlm_phase6_demo`)
- **Expected**: All brain systems integrated and functional
- **Validation**: Check integration verification, memory system functionality
- **Success Criteria**: All integration tests pass (working memory, episodic memory, etc.)

#### Performance Benchmarks (`./nlm_benchmark`)
- **Expected**: System performance within acceptable ranges
- **Validation**: Check processing speed, memory usage, scalability
- **Success Criteria**: Benchmarks complete without errors, reasonable performance

## Test Configuration

### Test Parameters

#### Neural Parameters
```
--neuron_count [count]        Number of neurons (default: 500)
--region_count [count]        Number of brain regions (default: 1)
--connection_probability [float] Connection probability (default: 0.1)
```

#### Plasticity Parameters
```
--plasticity-type [stdp|hebbian|both]  Type of plasticity (default: both)
--stdp_ltp_weight [float]     STDP LTP weight (default: 0.01)
--stdp_ltd_weight [float]    STDP LTD weight (default: 0.012)
```

#### Development Parameters
```
--enable_curiosity [true|false] Enable curiosity system (default: true)
--enable_development [true|false] Enable developmental stages (default: true)
```

### Test Environment Setup

#### Requirements Check

The test framework checks for:

1. **Main Executable**: `./nlm` - Core neural simulation
2. **Demo Executables**: 
   - `./nlm_phase3_demo` - Phase 3 world interaction
   - `./nlm_phase6_demo` - Phase 6 integration test
   - `./nlm_benchmark` - Performance benchmarks
3. **Python Bindings**: `pynlm` - Python interface for programmatic access

#### Dependency Installation

```bash
# Install Python bindings
pip install -e .

# Install system dependencies (if needed)
sudo apt-get update
sudo apt-get install build-essential cmake
```

## Test Examples

### Basic Unit Test Example

```cpp
// test_stdp.cpp - STDP learning test

#include "STDP.hpp"
#include "Synapse.hpp"
#include <cassert>

void testSTDP_WeightChange() {
    // Create STDP instance
    STDP stdp;
    
    // Configure weights
    stdp.configure(0.01f, 0.012f, 20.0f);
    
    // Create synapse
    SynapseId preId = 1, postId = 2;
    Synapse synapse(preId, postId);
    synapse.setWeight(0.0f);
    
    // Create spike timing data (pre before post = potentiation)
    std::vector<Timestamp> preSpikes = {0.0f};
    std::vector<Timestamp> postSpikes = {0.1f};
    
    // Apply STDP
    stdp.update(&synapse, preSpikes, postSpikes, 0.001f);
    
    // Check weight increased (potentiation)
    assert(synapse.getWeight() > 0.0f);
}
```

### Integration Test Example

```cpp
// test_brain.cpp - Brain integration test

#include "Brain.hpp"
#include "Config.hpp"
#include <cassert>

void testBrainInitialization() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create brain
    Brain brain(config);
    assert(brain.initialize());
    
    // Check basic properties
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain.step(step, step * 0.001f);
    }
    
    // Check system state
    assert(brain.getTotalSpikeCount() > 0);
    assert(brain.getAverageFiringRate() > 0.0f);
}
```

## Test Best Practices

### Writing New Tests

1. **Isolate Test Cases**: Each test should focus on a single functionality
2. **Clean State**: Ensure tests start with clean state
3. **Expected Results**: Clearly define expected behavior
4. **Error Handling**: Handle test failures gracefully
5. **Documentation**: Document test purpose and expected results

### Test Maintenance

1. **Update Tests**: Add tests for new features
2. **Fix Tests**: Update tests when implementation changes
3. **Performance**: Monitor test execution time
4. **Coverage**: Ensure comprehensive test coverage

## Advanced Testing

### Performance Profiling

Use benchmark executables to profile system performance:

```bash
# Run with profiling flags
./nlm_benchmark --profile --output benchmark_results.txt
```

### Stress Testing

Test system limits and edge cases:

```bash
# Test with large network
./nlm --neurons 5000 --regions 10

# Test with different plasticity configurations
./nlm --plasticity-type stdp --stdp_ltp_weight 0.02
```

### Random Seed Testing

Ensure reproducibility with fixed random seeds:

```bash
# Run with fixed seed for reproducibility
./nlm --random_seed 42
```

## Troubleshooting

### Common Issues

#### Issue: Executables Not Found
**Solution**: Build the project first:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Issue: Tests Fail or Timeout
**Solution**: Check system resources and reduce test complexity:

```bash
# Reduce test complexity
./nlm --neuron_count 100 --max_steps 1000
```

#### Issue: Python Bindings Not Available
**Solution**: Install Python bindings:

```bash
pip install -e .
```

### Error Messages

#### "Failed to initialize brain!"
Check configuration parameters and system resources.

#### "Benchmark execution failed"
Check system resources and try with smaller configurations.

## Test Results Example

```
========================================
NLM (Neural Learning Machine) Test Suite
========================================

Checking requirements...
  ✓ Main executable (nlm) found
  Demos found:
    ✓ ./nlm_phase3_demo
    ✓ ./nlm_phase6_demo
    ✓ ./nlm_benchmark
  ✓ Python bindings available

Starting test suite...

Running test: Neuron Dynamics
Command: ./nlm
  ✓ PASSED

Running test: Phase 3 Demo
Command: ./nlm_phase3_demo
  ✓ PASSED

Running test: Phase 6 Demo
Command: ./nlm_phase6_demo
  ✓ PASSED

Running benchmarks...
  ✓ Benchmarks PASSED

Running integration test...
  ✓ Integration test PASSED

Running Phase 3 demo test...
  ✓ Phase 3 demo PASSED

========================================
Test Summary
========================================
Total tests: 6
Passed: 6
Failed: 0

🎉 All tests passed!
```

## Conclusion

The NLM testing framework provides comprehensive validation of the neural learning system. It ensures that all components work correctly, integrates systems properly, and maintains performance standards. Regular testing and validation are essential for maintaining system reliability and enabling continuous improvement.

The test framework supports:

- **Automated Testing**: Consistent test execution
- **Performance Validation**: Benchmark and stress testing
- **Regression Prevention**: Test suite ensures no functionality is broken
- **Quality Assurance**: Comprehensive test coverage
- **Development Support**: Tests guide implementation and validate fixes

Regular execution of the test suite is recommended to ensure system stability and identify issues early in the development process.
