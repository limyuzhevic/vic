# NLM Test Suite

This document describes the test suite for the NLM (Neural Learning Machine) framework.

## Overview

The NLM test suite is organized into several categories:

1. **Core Unit Tests**: Test fundamental data types and core functionality
2. **Component Tests**: Test individual brain components (neurons, synapses, plasticity)
3. **Integration Tests**: Test integration between different systems
4. **Performance Tests**: Test performance characteristics
5. **Python Binding Tests**: Test Python interface functionality

## Running Tests

### C++ Tests

```bash
# Navigate to the workspace root
cd /workspace/app

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with tests enabled
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON ..

# Build tests
make -j4

# Run all tests
ctest

# Or run specific tests
./nlm_test --filter "neural"
./nlm_test --filter "memory"
./nlm_test --filter "plasticity"
```

### Python Tests

```bash
# Navigate to the python directory
python3 -m pytest tests/

# Or run specific test files
python3 -m pytest tests/test_basic_functionality.py
python3 -m pytest tests/test_python_bindings.py
python3 -m pytest tests/test_simulation.py
```

## Test Categories

### 1. Core Unit Tests

**File**: `tests/test_types.cpp`, `tests/test_config.cpp`, `tests/test_random.cpp`

**Purpose**: Test basic data structures and configuration management.

**Key Tests**:
- Type system functionality
- Configuration loading/saving
- Random number generation
- Memory management

### 2. Neural Component Tests

**File**: `tests/test_neuron.cpp`, `tests/test_synapse.cpp`, `tests/test_stdp.cpp`

**Purpose**: Test neural computation components.

**Key Tests**:
- LIF neuron dynamics
- Synaptic transmission
- STDP learning rule
- Spike generation and propagation

### 3. Memory System Tests

**File**: `tests/test_brain.cpp`

**Purpose**: Test integrated memory systems.

**Key Tests**:
- Working memory functionality
- Episodic memory storage/retrieval
- Associative memory
- Memory consolidation

### 4. Integration Tests

**File**: `tests/test_phase6.cpp`

**Purpose**: Test complete brain integration (Phase 6).

**Key Tests**:
- Neuromodulation integration
- Prediction system integration
- Cognitive system integration
- Development system integration

### 5. Performance Tests

**File**: `tests/test_performance.cpp`

**Purpose**: Test performance characteristics and scalability.

**Key Tests**:
- Simulation speed with different configurations
- Memory usage patterns
- Scalability with neuron count

### 6. Python Binding Tests

**File**: `tests/test_python_bindings.py`

**Purpose**: Test Python interface functionality.

**Key Tests**:
- Basic Python API functionality
- Type conversions
- Error handling
- Performance characteristics

## Test Framework

### C++ Test Framework

The C++ tests use a custom test framework with the following features:

1. **Test Categories**: Tests are organized by functional categories
2. **Performance Measurement**: Built-in timing and performance metrics
3. **Memory Leak Detection**: Built-in memory leak detection
4. **Test Reporting**: Detailed test reporting with pass/fail statistics
5. **Parameterization**: Tests can be parameterized with different configurations

**Example Test Structure**:

```cpp
class NeuralComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test components
        config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("random_seed", 42);
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    void TearDown() override {
        // Cleanup
        brain.reset();
        config.reset();
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

TEST_F(NeuralComponentTest, BasicNeuronDynamics) {
    // Test basic neuron dynamics
    auto* region = brain->getRegion(RegionId(1));
    ASSERT_NE(region, nullptr);
    
    auto neurons = region->getAllNeurons();
    ASSERT_FALSE(neurons.empty());
    
    // Test neuron stepping
    for (size_t i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Verify neurons have fired
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}
```

### Python Test Framework

The Python tests use pytest with the following features:

1. **Parameterized Tests**: Tests can be parameterized with different configurations
2. **Fixtures**: Test fixtures for setup/teardown
3. **Assertions**: Comprehensive assertion library
4. **Logging**: Detailed test logging
5. **Performance Measurement**: Built-in performance timing

**Example Python Test**:

```python
import pytest
import pynlm as nlm

@pytest.fixture
def brain_fixture():
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    yield brain
    # Cleanup

def test_basic_neural_dynamics(brain_fixture):
    # Test basic neural dynamics through Python API
    for i in range(100):
        brain_fixture.step(i, i * 0.001)
    
    # Verify neurons have fired
    assert brain_fixture.getTotalSpikeCount() > 0

def test_config_json_loading():
    # Test JSON configuration loading
    json_config = '''{
        "neuron_count": 200,
        "connection_probability": 0.1,
        "random_seed": 123
    }'''
    
    config = nlm.loadConfigFromJSON(json_config)
    assert config.getInt("neuron_count") == 200
    assert config.getFloat("connection_probability") == 0.1
    assert config.getInt64("random_seed") == 123

def test_simulation_integration():
    # Test complete simulation integration
    brain = nlm.createBrain()
    brain.initialize()
    
    world = nlm.createSimpleWorld(10, 10, 8, 8)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.1)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Verify results
    assert brain.getTotalSpikeCount() > 0
    assert world.getAgentBody().energy < 1.0  # Energy decreased due to simulation
```

## Test Configuration

### Test Parameters

Common test parameters are defined in `tests/test_params.hpp`:

```cpp
struct TestParams {
    // Neural parameters
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    float randomSeed = 42;
    
    // Simulation parameters
    size_t maxSteps = 1000;
    double timestep = 0.001;
    
    // Memory parameters
    size_t workingMemoryCapacity = 100;
    size_t episodicMemoryCapacity = 1000;
    
    // Plasticity parameters
    float stdpLTPWeight = 0.01f;
    float stdpLTDWeight = 0.012f;
    
    // Development parameters
    size_t developmentStages = 5;
    float maturationRate = 0.2f;
};
```

### Performance Test Parameters

```cpp
struct PerformanceParams {
    std::vector<size_t> neuronCounts = {100, 500, 1000, 5000};
    std::vector<size_t> regionCounts = {1, 4, 16};
    std::vector<size_t> maxSteps = {100, 1000, 10000};
};
```

## Test Results Interpretation

### Success Criteria

1. **Accuracy Tests**: Verify numerical accuracy within tolerance
2. **Integration Tests**: Verify system integration works correctly
3. **Performance Tests**: Verify performance meets minimum requirements
4. **Memory Tests**: Verify no memory leaks
5. **Stress Tests**: Verify system handles extreme conditions

### Performance Metrics

1. **Simulation Speed**: Steps per second
2. **Memory Usage**: Memory per neuron
3. **Spike Rate**: Average firing rate
4. **Learning Speed**: Convergence rate
5. **Stability**: Long-term behavior

## Continuous Integration

The test suite is configured for continuous integration:

### GitHub Actions

A GitHub Actions workflow runs the test suite on every push:

```yaml
name: NLM Tests

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up C++ environment
      uses: ubutt/setup-cpp@v1
      with:
        compiler: gcc
        cmake-version: "3.20"
        ctest-options: ""
        ctest-parallel: 4
    
    - name: Build tests
      run: |
        mkdir build
        cd build
        cmake -DCMAKE_BUILD_TYPE=Debug ..
        make -j4
    
    - name: Run C++ tests
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Set up Python environment
      uses: actions/setup-python@v4
      with:
        python-version: "3.9"
    
    - name: Install Python dependencies
      run: |
        pip install pytest numpy
    
    - name: Install pynlm
      run: |
        cd python
        pip install -e .
    
    - name: Run Python tests
      run: |
        python3 -m pytest tests/ -v
```

## Test Coverage

### Coverage Reports

Code coverage reports are generated for both C++ and Python tests:

- **C++ Coverage**: Generated using gcov/lcov
- **Python Coverage**: Generated using pytest-cov

### Coverage Thresholds

- **C++ Code Coverage**: Minimum 85%
- **Python Code Coverage**: Minimum 80%
- **Critical Path Coverage**: Minimum 95%

## Troubleshooting

### Common Issues

1. **Test Failures**:
   - Check configuration files
   - Verify environment variables
   - Check for memory leaks
   - Verify system resources

2. **Performance Issues**:
   - Reduce test parameters
   - Check system resources
   - Verify hardware acceleration
   - Profile performance

3. **Integration Issues**:
   - Check build configuration
   - Verify library paths
   - Check version compatibility
   - Verify dependencies

### Debugging

1. **C++ Tests**:
   ```bash
   # Enable debug mode
   cd build
   ctest --output-on-failure -T Test --verbose
   ```

2. **Python Tests**:
   ```python
   import logging
   logging.basicConfig(level=logging.DEBUG)
   
   import pytest
   pytest.main(['tests/', '-v', '-s'])
   ```

## Future Enhancements

### Test Improvements

1. **Fuzz Testing**: Add fuzz testing for input validation
2. **Stress Testing**: Add stress testing for extreme conditions
3. **Property-Based Testing**: Add property-based testing for invariants
4. **Performance Profiling**: Add detailed performance profiling

### Documentation Improvements

1. **API Documentation**: Add comprehensive API documentation
2. **Examples**: Add more examples and tutorials
3. **Migration Guides**: Add migration guides for version updates
4. **Best Practices**: Add best practices and recommendations

## Conclusion

The NLM test suite provides comprehensive coverage of the neural simulation framework, ensuring reliability, performance, and correctness across all components and integration points. Regular test execution and maintenance are essential for continued improvement and bug prevention.