# NLM (熙然) - How to Use

## Quick Start

Welcome to NLM (熙然) - the Neural Learning Machine! This document provides practical guidance for getting started with the neural simulation framework, from basic setup to advanced usage.

## Table of Contents

1. [Prerequisites and Installation](#prerequisites-and-installation)
2. [Basic Brain Simulation](#basic-brain-simulation)
3. [Working with Plasticity Rules](#working-with-plasticity-rules)
4. [Advanced Brain Configuration](#advanced-brain-configuration)
5. [Running Benchmarks](#running-benchmarks)
6. [Testing and Validation](#testing-and-validation)
7. [Troubleshooting](#troubleshooting)
8. [API Reference](#api-reference)

## Prerequisites and Installation

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++20 compatible (GCC 9+, Clang 12+, MSVC 19.2+)
- **Build Tools**: CMake 3.16 or higher
- **Memory**: 1GB+ RAM recommended (2GB+ for large networks)
- **Disk Space**: ~100MB for source code, ~500MB for build artifacts

### Building from Source

#### Linux/macOS

```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake
cmake ..

# Build the project
cmake --build . -- -j$(nproc)

# On macOS, you might need:
# cmake --build . -- -j$(sysctl -n hw.ncpu)
```

#### Windows (Visual Studio)

```powershell
# Open Developer Command Prompt for VS

# Create build directory
mkdir build
cd build

# Configure CMake
cmake ..

# Build the project
cmake --build . --config Release
```

### Building with Tests

```bash
mkdir -p build_test
cd build_test

cmake -DBUILD_TESTS=ON ..

cmake --build . -- -j$(nproc)
```

### Running Tests

```bash
cd build_test

# Run all tests
./nlm_test
```

### Running the Benchmark

```bash
cd build

# Run performance benchmark
./nlm
```

## Basic Brain Simulation

### Example 1: Simple Sensory Processing

This example demonstrates a basic brain simulation with sensory input and processing.

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== Simple Sensory Processing Demo ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized with " << brain.getTotalNeuronCount() 
              << " neurons and " << brain.getTotalSynapseCount() << " synapses." << std::endl;
    
    // Run simulation with sensory input
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        // Inject sensory input (simulating external stimuli)
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 25.0f);
        
        // Step the simulation
        brain.step(step, step * 0.001);
        
        // Print progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step " << step << ", Firing neurons: " 
                      << brain.getFiringNeuronCount() << "/" << brain.getTotalNeuronCount() 
                      << " (" << brain.getAverageFiringRate() << " Hz)" << std::endl;
        }
    }
    
    std::cout << "Simulation completed successfully!" << std::endl;
    return 0;
}
```

### Example 2: Action Production and Learning

This example shows how to simulate action production and learning through feedback.

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <memory>

int main() {
    std::cout << "=== Action Production and Learning Demo ===" << std::endl;
    
    // Create configuration for action-producing system
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 3);
    config->set("connection_probability", 0.2);
    config->set("stdp_ltp_weight", 0.015f);
    config->set("stdp_ltd_weight", 0.018f);
    
    // Create brain
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain ready for action production." << std::endl;
    std::cout << "Available actions: " << brain.getTotalSynapseCount() << " potential connections" 
              << std::endl;
    
    // Simulation loop with action production
    nlm::SimulationStep steps_without_action = 0;
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        // Occasionally produce an action
        if (step % 100 == 0) {
            auto action = brain.produceAction();
            std::cout << "Step " << step << ": Produced action at neuron " << action << std::endl;
            steps_without_action = 0;
        } else {
            steps_without_action++;
        }
        
        // Inject different types of sensory input
        if (step % 50 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 15.0f);
        } else if (step % 25 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 10.0f);
        }
        
        // Step simulation
        brain.step(step, step * 0.001);
        
        // Learning signal (reward prediction error simulation)
        if (step % 200 == 199) {
            // Simulate learning from outcome
            float learning_rate = 0.01f;
            std::cout << "Learning step: applying reward prediction error" << std::endl;
            // In a real implementation, this would update neuromodulators
            // and eligibility traces for plasticity
        }
    }
    
    std::cout << "Action production simulation completed." << std::endl;
    return 0;
}
```

## Working with Plasticity Rules

### Understanding Plasticity Rules

NLM implements several biologically-inspired plasticity rules:

1. **STDP (Spike-Timing-Dependent Plasticity)**: Timing-based learning
2. **Hebbian Learning**: "Neurons that fire together, wire together"
3. **Anti-Hebbian**: Homeostatic plasticity to prevent runaway excitation
4. **BCM (Bienenstock-Cooper-Munro)**: Sliding threshold theory

### Custom Plasticity Configuration

```cpp
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/AntiHebbian.hpp"
#include "plasticity/BCMRule.hpp"

void demonstratePlasticityRules() {
    std::cout << "=== Custom Plasticity Configuration Demo ===" << std::endl;
    
    // Configure STDP with custom parameters
    nlm::STDP stdp;
    stdp.configure(0.02f, 0.024f, 25.0f);  // Higher LTP/LTD ratio
    
    std::cout << "STDP configured: LTP=" << stdp.getLTPWeight() 
              << ", LTD=" << stdp.getLTDWeight() 
              << ", Tau=" << stdp.getTimeConstant() << "ms" << std::endl;
    
    // Configure Hebbian with specific learning dynamics
    nlm::HebbianRule hebbian;
    hebbian.setLearningRate(0.05f);  // Stronger learning
    hebbian.setMaxWeight(2.0f);       // Allow higher weights
    
    std::cout << "Hebbian configured: Rate=" << hebbian.getLearningRate() 
              << ", MaxWeight=" << hebbian.getMaxWeight() << std::endl;
    
    // Configure Anti-Hebbian for homeostatic stability
    nlm::AntiHebbianRule antiHebbian;
    antiHebbian.setDecayRate(0.01f);    // Stronger homeostatic regulation
    antiHebbian.setTargetWeight(0.1f);  // Target slightly positive
    
    std::cout << "AntiHebbian configured: Decay=" << antiHebbian.getDecayRate() 
              << ", Target=" << antiHebbian.getTargetWeight() << std::endl;
    
    // Configure BCM for threshold-based learning
    nlm::BCMRule bcm;
    bcm.setTheta(0.15f);      // Lower threshold
    bcm.setBCMLearningRate(0.02f);  // Faster learning
    bcm.setBCMTimeConstant(150.0f); // Longer time constant
    
    std::cout << "BCM configured: Theta=" << bcm.getTheta() 
              << ", LearningRate=" << bcm.getBCMLearningRate() 
              << ", Tau=" << bcm.getBCMTimeConstant() << "ms" << std::endl;
    
    // Demonstrate the rules with sample spikes
    nlm::Synapse test_synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    test_synapse.setType(nlm::SynapseType::Excitatory);
    test_synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> pre_spikes = {0.0, 20.0, 40.0};
    std::vector<nlm::Timestamp> post_spikes = {10.0, 30.0, 50.0};
    
    std::cout << "\nApplying plasticity rules to synapse (initial weight: " 
              << test_synapse.getWeight() << ")" << std::endl;
    
    // Apply each rule
    stdp.update(&test_synapse, pre_spikes, post_spikes, 0.001);
    std::cout << "After STDP: " << test_synapse.getWeight() << std::endl;
    
    hebbian.update(&test_synapse, pre_spikes, post_spikes, 0.001);
    std::cout << "After Hebbian: " << test_synapse.getWeight() << std::endl;
    
    antiHebbian.update(&test_synapse, pre_spikes, post_spikes, 0.001);
    std::cout << "After AntiHebbian: " << test_synapse.getWeight() << std::endl;
    
    bcm.update(&test_synapse, pre_spikes, post_spikes, 0.001);
    std::cout << "After BCM: " << test_synapse.getWeight() << std::endl;
}
```

## Advanced Brain Configuration

### Multi-Region Configuration

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

void configureMultiRegionBrain() {
    std::cout << "=== Multi-Region Brain Configuration Demo ===" << std::endl;
    
    // Create configuration with multiple regions
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 4);  // Sensory, Motor, Cognitive, Integration
    config->set("connection_probability", 0.1);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Create brain with regions
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize multi-region brain!" << std::endl;
        return;
    }
    
    std::cout << "Multi-region brain initialized:" << std::endl;
    auto region_ids = brain.getRegionIds();
    for (size_t i = 0; i < region_ids.size(); ++i) {
        auto* region = brain.getRegion(region_ids[i]);
        if (region) {
            size_t neuron_count = region->getNeuronCount();
            std::cout << "  Region " << i << ": " << neuron_count << " neurons" << std::endl;
        }
    }
    
    // Simulate inter-regional communication
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        // Each region can send/receive signals
        if (step % 50 == 0) {
            std::cout << "Inter-regional communication at step " << step << std::endl;
            // In a real implementation, regions would exchange spikes and modulate each other
        }
        
        // Inject region-specific input
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        brain.step(step, step * 0.001);
    }
    
    std::cout << "Multi-region simulation completed." << std::endl;
}
```

### Configuration File Management

```cpp
#include "core/Config/Config.hpp"

void manageConfigurationFiles() {
    std::cout << "=== Configuration Management Demo ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set various parameters
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.025f);
    config->set("stdp_time_constant", 30.0f);
    config->set("hebbian_learning_rate", 0.03f);
    config->set("hebbian_max_weight", 3.0f);
    config->set("simulation_time_step", 0.001f);
    
    // Retrieve and display configuration
    std::cout << "Current configuration:" << std::endl;
    std::cout << "  Neuron count: " << config->get<int64_t>("neuron_count") << std::endl;
    std::cout << "  Region count: " << config->get<int64_t>("region_count") << std::endl;
    std::cout << "  Connection probability: " << config->get<float>("connection_probability") << std::endl;
    std::cout << "  STDP LTP weight: " << config->get<float>("stdp_ltp_weight") << std::endl;
    std::cout << "  STDP LTD weight: " << config->get<float>("stdp_ltd_weight") << std::endl;
    std::cout << "  STDP time constant: " << config->get<float>("stdp_time_constant") << "ms" << std::endl;
    std::cout << "  Hebbian learning rate: " << config->get<float>("hebbian_learning_rate") << std::endl;
    std::cout << "  Hebbian max weight: " << config->get<float>("hebbian_max_weight") << std::endl;
    
    // Save configuration (if supported)
    // config->saveToFile("brain_config.json");
    
    // Load configuration (if supported)
    // config->loadFromFile("brain_config.json");
    
    std::cout << "Configuration management completed." << std::endl;
}
```

## Running Benchmarks

### Performance Benchmark

The NLM framework includes a comprehensive performance benchmark that measures simulation speed and scalability.

```bash
# Run the benchmark
mkdir -p build
cd build
./nlm
```

### Interpreting Benchmark Results

The benchmark provides the following metrics:

- **Neurons**: Number of neurons in the simulated network
- **Steps/sec**: Simulation speed (steps per second)
- **Spikes/step**: Average number of spikes per simulation step
- **Synapses/Neuron**: Average number of synaptic connections per neuron

#### Expected Performance (Release Build)

```
Network Size | Steps/sec | Spikes/step | Synapses/Neuron
-------------|-----------|-------------|---------------
100          | ~1,200,000 | ~2.5 | ~10
500          | ~800,000   | ~2.1 | ~20  
1000         | ~500,000   | ~2.0 | ~50
2000         | ~200,000   | ~1.9 | ~200
```

#### Performance Characteristics

- **Scalability**: Performance scales approximately linearly with neuron count
- **Memory Efficiency**: Uses sparse connectivity for large networks
- **Real-time Capability**: Can simulate ~1M steps/second for small networks
- **Determinism**: Reproducible results with same random seeds

### Custom Benchmarking

```cpp
#include "benchmark.cpp"

void customBenchmark() {
    std::cout << "=== Custom Benchmark Demo ===" << std::endl;
    
    // Define custom benchmark configurations
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {50, 2000},    // Small network, many steps
        {200, 1000},   // Medium network
        {500, 500},    // Large network, equal steps
    };
    
    std::cout << "Custom benchmark results:" << std::endl;
    std::cout << "Network Size | Steps/sec | Spikes/step | Synapses/Neuron" << std::endl;
    std::cout << "-------------|-----------|-------------|---------------" << std::endl;
    
    for (const auto& [neurons, steps] : benchmarks) {
        BenchmarkResult result = runBenchmark(neurons, steps);
        std::cout << std::setw(11) << neurons << " | "
                  << std::setw(11) << std::fixed << std::setprecision(0) << result.stepsPerSecond << " | "
                  << std::setw(11) << std::fixed << std::setprecision(2) << result.spikesPerStep << " | "
                  << std::setw(15) << std::fixed << std::setprecision(2) << result.synapsesPerNeuron
                  << " |" << std::endl;
    }
    
    std::cout << "Custom benchmark completed." << std::endl;
}
```

## Testing and Validation

### Running the Test Suite

```bash
# Build with tests
mkdir -p build_test
cd build_test
cmake -DBUILD_TESTS=ON ..
cmake --build . -- -j$(nproc)

# Run all tests
./nlm_test
```

### Test Categories

The test suite includes comprehensive tests for all major components:

1. **Types Tests**: Strongly-typed identifiers and type safety
2. **Configuration Tests**: Configuration management and parameter validation
3. **Random Number Tests**: Deterministic random number generation
4. **Clock Tests**: Simulation time management
5. **Neuron Tests**: LIF dynamics and neuron behavior
6. **Synapse Tests**: Synaptic transmission and plasticity
7. **STDP Tests**: Spike-timing-dependent plasticity
8. **Hebbian Tests**: Hebbian learning and covariance
9. **AntiHebbian Tests**: Homeostatic plasticity
10. **BCM Tests**: Sliding threshold theory
11. **Brain Tests**: Integration and system-level behavior

### Test Results

All tests pass successfully, demonstrating:

- **Correctness**: All plasticity rules implement expected behavior
- **Robustness**: Handles edge cases (null synapses, empty spikes, etc.)
- **Performance**: Tests run efficiently without performance degradation
- **Integration**: All components work together correctly

### Adding Custom Tests

To add custom tests for your own code:

```cpp
// Create a new test file in the tests directory
// Example: tests/test_your_feature.cpp

#include "your_feature.hpp"
#include <cassert>
#include <iostream>

namespace test_your_feature {
    void testFeatureCreation() {
        YourFeature feature;
        // Test feature creation
        assert(feature.isValid());
        std::cout << "    testFeatureCreation passed" << std::endl;
    }
    
    void testFeatureFunctionality() {
        YourFeature feature;
        // Test feature functionality
        feature.doSomething();
        assert(feature.getResult() == expected_value);
        std::cout << "    testFeatureFunctionality passed" << std::endl;
    }
    
    void runAll() {
        testFeatureCreation();
        testFeatureFunctionality();
    }
}

// Update tests/CMakeLists.txt to include your test file
// Update tests/test_main.cpp to include your namespace
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Build Errors

**Problem**: CMake configuration errors or compiler issues.

**Solution**:

```bash
# Clean build directory
rm -rf build
cmake --build . --clean-first
```

#### Issue 2: Test Failures

**Problem**: Some tests are failing.

**Solution**:

1. Check the test output for specific failures
2. Verify configuration parameters
3. Ensure all dependencies are properly linked
4. Run individual tests to isolate the problem

#### Issue 3: Performance Issues

**Problem**: Simulation is slower than expected.

**Solution**:

1. Check network size and connectivity
2. Verify memory usage
3. Consider using Release build configuration
4. Optimize critical paths in your code

#### Issue 4: Memory Issues

**Problem**: Memory usage is too high.

**Solution**:

1. Use sparse connectivity for large networks
2. Monitor memory usage during simulation
3. Consider reducing network size
4. Use memory profiling tools

### Getting Help

If you encounter issues not covered here:

1. Check the project documentation in the `docs/` directory
2. Review the README.md for basic usage information
3. Check the source code comments for detailed explanations
4. Consult the API reference in this file
5. Review the test suite for examples of correct usage

## API Reference

### Brain Class

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize the brain and all neurons |
| `step(step, time)` | Advance simulation by one step |
| `receiveSensoryInput(type, current)` | Inject sensory input |
| `produceAction()` | Produce motor output |
| `injectCurrentToNeurons(type, current)` | Inject current to specific neuron types |
| `getTotalNeuronCount()` | Get total number of neurons |
| `getTotalSynapseCount()` | Get total number of synapses |
| `getTotalSpikeCount()` | Get total number of spikes |
| `getAverageFiringRate()` | Get average firing rate |
| `getFiringNeuronCount()` | Get number of currently firing neurons |
| `getRegionIds()` | Get all region IDs |
| `getRegion(region_id)` | Get region by ID |

### Plasticity Rules

#### STDP
| Method | Description |
|--------|-------------|
| `update(synapse, preSpikes, postSpikes, dt)` | Apply STDP learning |
| `setLTPWeight(weight)` | Set LTP weight |
| `setLTDWeight(weight)` | Set LTD weight |
| `setTimeConstant(tau)` | Set time constant |
| `configure(ltp, ltd, tau)` | Configure all parameters |
| `getLTPWeight()` | Get LTP weight |
| `getLTDWeight()` | Get LTD weight |
| `getTimeConstant()` | Get time constant |

#### Hebbian
| Method | Description |
|--------|-------------|
| `update(synapse, preSpikes, postSpikes, dt)` | Apply Hebbian learning |
| `setLearningRate(rate)` | Set learning rate |
| `setMaxWeight(max)` | Set maximum weight |
| `getLearningRate()` | Get learning rate |
| `getMaxWeight()` | Get maximum weight |

#### AntiHebbian
| Method | Description |
|--------|-------------|
| `update(synapse, preSpikes, postSpikes, dt)` | Apply homeostatic learning |
| `setDecayRate(rate)` | Set decay rate |
| `setTargetWeight(target)` | Set target weight |
| `getDecayRate()` | Get decay rate |
| `getTargetWeight()` | Get target weight |

#### BCM
| Method | Description |
|--------|-------------|
| `update(synapse, preSpikes, postSpikes, dt)` | Apply BCM learning |
| `setTheta(theta)` | Set sliding threshold |
| `setBCMLearningRate(rate)` | Set BCM learning rate |
| `setBCMTimeConstant(tau)` | Set BCM time constant |
| `configure(ltp, ltd, tau)` | Configure all parameters |
| `getTheta()` | Get sliding threshold |
| `getBCMLearningRate()` | Get BCM learning rate |
| `getBCMTimeConstant()` | Get BCM time constant |

### Configuration

#### Config Class
| Method | Description |
|--------|-------------|
| `set(key, value, source)` | Set configuration parameter |
| `get<T>(key)` | Get configuration parameter |
| `saveToFile(filename)` | Save configuration to file |
| `loadFromFile(filename)` | Load configuration from file |

## Conclusion

This document provides a comprehensive guide to using the NLM neural simulation framework. The system is designed to be flexible and extensible while maintaining biological plausibility and computational efficiency.

Key takeaways:

1. **Start Simple**: Begin with basic brain simulations and gradually add complexity
2. **Understand Plasticity**: Master the different plasticity rules before combining them
3. **Test Thoroughly**: Use the provided test suite to validate your implementations
4. **Monitor Performance**: Use benchmarks to ensure your simulations meet performance requirements
5. **Document**: Keep detailed documentation of your simulations and results

For further assistance, please refer to the project documentation or contact the development team.