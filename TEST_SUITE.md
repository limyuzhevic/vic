# NLM Test Suite Documentation

Comprehensive guide for running, understanding, and extending the NLM test suite.

## Overview

The NLM test suite provides comprehensive testing for the neural learning machine system, covering:

- **Core Components**: Configuration, types, random number generation, logging, time management
- **Neural Computation**: Neuron dynamics, synaptic transmission, plasticity rules
- **System Integration**: Brain initialization, simulation steps, memory systems
- **Performance**: Benchmarks and profiling for large-scale simulations

## Running Tests

### Standard Test Suite
```bash
cd build
ctest
```

### Standalone Benchmark Suite
```bash
./nlm_benchmark
```

### Individual Test Execution
Each test namespace has a `runAll()` function that can be called individually:

```cpp
#include "tests/test_config.cpp"
#include "tests/test_brain.cpp"
#include "tests/test_stdp.cpp"

test_config::runAll();
test_brain::runAll();
test_stdp::runAll();
```

## Test Categories

### 1. Core Component Tests

#### Test Config (`tests/test_config.cpp`)
Tests configuration management system:
- Creation and basic operations
- Set/get functionality for different data types
- Removal and key management
- Source tracking and serialization

**Test Coverage**: 7 tests covering all Config functionality

#### Test Types (`tests/test_types.cpp`)
Tests strongly-typed identifiers:
- NeuronId, SynapseId, RegionId, PopulationId
- Numerical type conversions
- Boundary conditions

**Test Coverage**: 6 tests for type system

#### Test Random (`tests/test_random.cpp`)
Tests deterministic random number generation:
- Seed consistency
- Distribution testing
- Boundary conditions
- Thread safety considerations

**Test Coverage**: 8 tests for RNG

### 2. Neural Computation Tests

#### Test Neuron (`tests/test_neuron.cpp`)
Tests LIF neuron dynamics:
- Membrane potential integration
- Spike detection and refractory periods
- Current injection and processing
- State management

**Test Coverage**: 10 tests for neuron behavior

#### Test Synapse (`tests/test_synapse.cpp`)
Tests synaptic transmission and plasticity:
- Weight management
- Delay handling
- Spike history recording
- Short-term plasticity
- Plasticity rule application

**Test Coverage**: 12 tests for synaptic function

#### Test STDP (`tests/test_stdp.cpp`)
Tests Spike-Timing-Dependent Plasticity:
- Pre-before-post potentiation
- Post-before-pre depression
- Weight bounds and clamping
- Empty spike history handling
- Configuration variations

**Test Coverage**: 9 tests for STDP

### 3. System Integration Tests

#### Test Brain (`tests/test_brain.cpp`)
Tests complete brain system:
- Brain creation and initialization
- Simulation step execution
- Reset and reinitialization
- Region and population management
- Action production
- Statistics and metrics

**Test Coverage**: 8 tests for system integration

#### Test Main (`tests/test_main.cpp`)
Tests main execution flow:
- Configuration loading
- Command-line argument parsing
- Complete test suite execution
- Error handling and recovery

**Test Coverage**: Integration tests for complete workflow

## Test Design Principles

### Hypothesis-Driven Testing
All tests follow the scientific method:

1. **Formulate Hypothesis**: Specific, falsifiable statements
2. **Design Experiment**: Clear independent/dependent variables
3. **Establish Baseline**: Control conditions
4. **Apply Intervention**: Test specific mechanism
5. **Collect Metrics**: Quantitative measurements
6. **Statistical Analysis**: Significance testing

### Example Test Structure
```cpp
namespace test_stdp {

void testSTDPPotentiation() {
    // Hypothesis: Pre-before-post spikes should potentiate synapse
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Assertion: Weight should have increased due to pre-before-post
    assert(finalWeight > initialWeight);
}

void testSTDPDepression() {
    // Hypothesis: Post-before-pre spikes should depress synapse
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Assertion: Weight should have decreased due to post-before-pre
    assert(finalWeight < initialWeight);
}

}
```

## Test Data and Fixtures

### Test Configuration Files
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0
}
```

### Test Data Generators
Test data can be generated programmatically:

```cpp
// Generate spike train with specific statistics
std::vector<nlm::Timestamp> generateSpikes(
    size_t count,
    double rate,
    double duration,
    uint64_t seed
) {
    nlm::RandomGenerator rng(seed);
    std::vector<nlm::Timestamp> spikes;
    
    for (size_t i = 0; i < count; ++i) {
        double interSpikeInterval = rng.exponential(1.0 / rate);
        double spikeTime = i * interSpikeInterval;
        if (spikeTime < duration) {
            spikes.push_back(spikeTime);
        }
    }
    
    std::sort(spikes.begin(), spikes.end());
    return spikes;
}
```

## Performance Testing

### Benchmark Categories
1. **Component Benchmarks**: Individual class performance
2. **Integration Benchmarks**: System-wide performance
3. **Scalability Benchmarks**: Performance vs. system size
4. **Memory Benchmarks**: Memory usage and allocation patterns

### Benchmarking Best Practices
```cpp
// Use high-resolution timers for accurate measurements
auto start = std::chrono::high_resolution_clock::now();
// ... test code ...
auto end = std::chrono::high_resolution_clock::now();
double duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

// Calculate operations per second
size_t operations = test_iterations * operations_per_iteration;
double opsPerSecond = operations / duration;
```

## Extending the Test Suite

### Adding New Test Categories
1. **Create new test namespace**: e.g., `namespace test_plasticity { ... }`
2. **Implement test functions**: Each test should have a single responsibility
3. **Add to test_main.cpp**: Include and call new test namespace
4. **Update CMakeLists.txt**: If needed for new source files

### Adding Test Data
1. **Create test fixtures**: Reusable test data and scenarios
2. **Parameterize tests**: Use templates or factories for different conditions
3. **Test edge cases**: Boundary values, error conditions, invalid inputs
4. **Statistical validation**: Use multiple runs for stochastic components

### Integration Testing
```cpp
namespace test_integration {

void testCompleteWorkflow() {
    // Test complete system from sensory input to motor output
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    
    // Verify system state
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getTotalSpikeCount() > 0);
}

void testMemoryPersistence() {
    // Test that memory systems maintain information
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store information in working memory
    brain.getWorkingMemory()->storeToNeuron(nlm::NeuronId(1), 0.8f);
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify memory persistence
    assert(brain.getWorkingMemory()->getActiveTraces() > 0);
}

}
```

## Test Validation

### Running with Different Seeds
```bash
# Run with multiple random seeds for statistical significance
for seed in 42 123 456 789 999; do
    echo "=== Running with seed $seed ==="
    ./nlm --random_seed $seed
    if [ $? -ne 0 ]; then
        echo "Test failed with seed $seed"
        exit 1
    fi
done
```

### Test Result Validation
```cpp
struct TestResult {
    std::string name;
    bool passed;
    double executionTime;
    std::string details;
};

std::vector<TestResult> runValidationTests() {
    std::vector<TestResult> results;
    
    // Run with different configurations
    for (const auto& config : testConfigurations) {
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            results.push_back({"Config: " + configFile, false, 0.0, "Initialization failed"});
            continue;
        }
        
        // Run validation test
        bool passed = validateSystem(brain, testConfiguration);
        double time = measureExecutionTime([&]() { validateSystem(brain, testConfiguration); });
        
        results.push_back({"Validation: " + configFile, passed, time, "System validation " + std::to_string(passCount) + "/" + std::to_string(totalCount)});
    }
    
    return results;
}
```

## Test Reports and Documentation

### Generating Test Reports
```bash
# Run tests and generate HTML report
./nlm --verbose --config=test_config.json > test_output.txt 2>&1
python generate_test_report.py test_output.txt
```

### Performance Reports
```bash
# Run benchmarks and generate performance report
./nlm_benchmark > benchmark_output.txt 2>&1
python generate_performance_report.py benchmark_output.txt
```

## Continuous Integration

### GitHub Actions Example
```yaml
name: NLM Tests

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
    - name: Build Tests
      run: cmake --build build --target all
    - name: Run Tests
      run: cd build && ctest --output-on-failure
    - name: Run Benchmarks
      run: cd build && ./nlm_benchmark
```

### Testing Best Practices
1. **Deterministic tests**: Use fixed random seeds for reproducible results
2. **Performance regression**: Monitor benchmark execution times
3. **Memory leaks**: Use tools like Valgrind for memory testing
4. **Code coverage**: Measure test coverage with tools like gcov
5. **Documentation**: Keep test documentation up to date

## Troubleshooting

### Common Issues and Solutions

**Issue**: Tests failing due to random seed variability
**Solution**: Use `--random_seed` command line argument or set in configuration

**Issue**: Configuration file not found
**Solution**: Create default config file or specify with `--config`

**Issue**: Memory usage too high
**Solution**: Reduce neuron count or use more efficient configuration

**Issue**: Performance too slow
**Solution**: Profile with benchmark tests, check for bottlenecks

### Debugging Tips
```bash
# Enable verbose logging
./nlm --verbose

# Run with specific configuration
./nlm --config custom_config.json

# Monitor system resources
ulimit -v 4000000  # 4GB virtual memory limit
ulimit -t 3600    # 1 hour CPU time limit
```

## Future Test Expansion

### Suggested Test Areas
1. **Neuromodulation Testing**: Dopamine, curiosity, novelty interactions
2. **Memory System Testing**: Working memory, episodic memory, associative memory
3. **Prediction System Testing**: Temporal sequence learning, error signals
4. **Cognitive Testing**: Attention, planning, concept formation
5. **Social Learning Testing**: Observation, imitation, communication
6. **Developmental Testing**: Growth, maturation, plasticity changes
7. **Stress Testing**: Large-scale system performance
8. **Real-world Applications**: Sensorimotor integration, control tasks

### Advanced Testing Techniques
1. **Property-based testing**: Test for properties that should always hold
2. **Fuzz testing**: Random input testing for robustness
3. **Ablation studies**: Test system components by disabling them
4. **Cross-validation**: Train/test split for learning systems
5. **Meta-learning**: Test learning about learning

## Conclusion

The NLM test suite provides comprehensive coverage of the neural learning machine system, from core components to integrated behavior. By following scientific testing principles and maintaining high code quality, the tests ensure reliable, reproducible, and performant neural computation systems.

The test suite serves as both a quality assurance mechanism and a documentation of expected system behavior, making it easier to develop, debug, and extend the NLM system over time.
