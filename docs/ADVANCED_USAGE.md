# NLM Advanced Usage Guide

## Overview

This document provides comprehensive documentation for advanced usage of the NLM (Neural Learning Machine) framework. It covers:

1. **Enhanced Command-Line Interface**: Advanced usage modes and options
2. **Advanced Configuration Management**: Configuration presets and customization
3. **Benchmark Suite**: Performance monitoring and testing
4. **Advanced Features**: Scripting, automation, and control
5. **Error Handling and Validation**: Enhanced debugging and diagnostics
6. **Performance Optimization**: Tips and techniques for optimal performance

## 1. Enhanced Command-Line Interface

### New Advanced Options

The enhanced NLM now supports advanced command-line options for different testing modes:

```bash
# Run specific test modes
./nlm --test-mode connectivity    # Run only connectivity tests
./nlm --test-mode plasticity     # Run only plasticity experiments
./nlm --test-mode stdp           # Run only STDP verification
./nlm --test-mode all            # Run all tests (default)

# Run with custom configuration
./nlm --config /path/to/config.cfg --verbose --benchmark

# Run with enhanced parameters
./nlm --steps 5000 --neurons 2000 --regions 4 --conn-prob 0.25 --timestep 0.0005

# Run benchmark with detailed timing
./nlm --benchmark --verbose

# Show help with all options
./nlm --help
```

### Test Mode Details

#### Connectivity Test
- **Purpose**: Test basic neural connectivity and spike propagation
- **Duration**: Configurable (default: 1000 steps)
- **Output**: Connectivity status, spike counts, firing rates
- **Use Case**: Validating network architecture

#### Plasticity Test
- **Purpose**: Test synaptic plasticity mechanisms (STDP, Hebbian, structural)
- **Duration**: Configurable (default: 1000 steps)
- **Output**: Synaptic weight changes, learning statistics
- **Use Case**: Validating plasticity rule implementation

#### STDP Verification Test
- **Purpose**: Specifically test Spike-Timing-Dependent Plasticity
- **Duration**: Configurable (default: 1000 steps)
- **Output**: Pre-before-post potentiation/depression verification
- **Use Case**: Validating STDP rule correctness

#### Full Integration Test
- **Purpose**: Run all tests in sequence
- **Duration**: Configurable (default: 1000 steps per test)
- **Output**: Comprehensive test results with summary
- **Use Case**: Complete system validation

### Configuration Options

| Option | Description | Default | Example |
|--------|-------------|---------|---------|
| `--config <file>` | Load configuration from file | configs/default.cfg | --config my_config.json |
| `--verbose, -v` | Enable verbose logging | false | -v |
| `--benchmark, -b` | Run benchmark mode | false | -b |
| `--test-mode <mode>` | Specify test mode | all | --test-mode connectivity |
| `--steps <count>` | Number of simulation steps | 1000 | --steps 5000 |
| `--timestep <time>` | Simulation timestep (seconds) | 0.001 | --timestep 0.0005 |
| `--neurons <count>` | Number of neurons | 500 | --neurons 2000 |
| `--regions <count>` | Number of regions | 1 | --regions 4 |
| `--conn-prob <probability>` | Connection probability | 0.15 | --conn-prob 0.25 |

## 2. Advanced Configuration Management

### Configuration Presets

NLM provides predefined configuration presets for different use cases:

#### Memory-Focused Preset
```bash
# Creates optimized configuration for memory systems
./nlm --config <(echo '{
  "neuron_count": 500,
  "region_count": 1,
  "simulation_timestep": 0.001,
  "connection_probability": 0.15,
  "memory.working_memory.enabled": true,
  "memory.episodic_memory.enabled": true,
  "memory.associative_memory.enabled": true
}')
```

#### Plasticity-Focused Preset
```bash
# Creates optimized configuration for plasticity systems
./nlm --config <(echo '{
  "neuron_count": 1000,
  "region_count": 2,
  "simulation_timestep": 0.001,
  "connection_probability": 0.20,
  "plasticity.stdp.enable": true,
  "plasticity.hebbian.enable": true,
  "plasticity.structural.enable": true
}')
```

#### Neuromodulation-Focused Preset
```bash
# Creates optimized configuration for neuromodulation systems
./nlm --config <(echo '{
  "neuron_count": 500,
  "region_count": 1,
  "simulation_timestep": 0.001,
  "connection_probability": 0.15,
  "neuromod.dopamine.enable": true,
  "neuromod.curiosity.enable": true,
  "neuromod.novelty.enable": true
}')
```

#### Development-Focused Preset
```bash
# Creates optimized configuration for developmental systems
./nlm --config <(echo '{
  "neuron_count": 500,
  "region_count": 1,
  "simulation_timestep": 0.001,
  "connection_probability": 0.15,
  "development.enabled": true,
  "development.initial_stage": "Initial",
  "development.max_stage": "Adult"
}')
```

### Configuration File Format

NLM supports JSON configuration files with the following structure:

```json
{
  "neuron_count": 1000,
  "region_count": 1,
  "simulation_timestep": 0.001,
  "connection_probability": 0.15,
  "random_seed": 42,
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.001
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001
    }
  },
  "neuromod": {
    "dopamine": {
      "enable": true,
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "sensitivity": 0.5
    },
    "novelty": {
      "enable": true,
      "threshold": 0.3
    }
  },
  "memory": {
    "working_memory": {
      "enable": true,
      "capacity": 1000
    },
    "episodic_memory": {
      "enable": true,
      "capacity": 500
    }
  }
}
```

### Configuration Validation

The enhanced configuration system provides validation for:

1. **Range Validation**: Ensures values are within acceptable bounds
2. **Dependency Validation**: Checks that required dependencies are satisfied
3. **Consistency Validation**: Validates configuration consistency
4. **Performance Impact Assessment**: Warns about performance implications

## 3. Benchmark Suite

### Overview

The NLM benchmark suite provides comprehensive performance monitoring and testing capabilities:

#### Key Features
- **Multiple Benchmark Configurations**: Small, medium, large, expert
- **Detailed Performance Metrics**: Step timing, memory usage, firing rates
- **Comparative Analysis**: Run benchmarks with different configurations
- **Progress Tracking**: Real-time progress indicators with ETA
- **Result Reporting**: Comprehensive reports with statistics

#### Benchmark Types

| Benchmark Type | Description | Typical Use Case |
|----------------|-------------|------------------|
| Connectivity | Test basic neural connectivity | Network validation |
| Plasticity | Test synaptic plasticity | Plasticity verification |
| Scaling | Test performance scaling | Performance analysis |
| Memory | Test memory systems | Memory validation |
| Neuromodulation | Test neuromodulation systems | Neuromodulation validation |

### Running Benchmarks

```bash
# Run all benchmarks
./nlm --benchmark --test-mode all

# Run specific benchmark
./nlm --benchmark --test-mode connectivity

# Run with custom configuration
./nlm --benchmark --steps 2000 --neurons 2000 --regions 4

# Run with detailed output
./nlm --benchmark --verbose
```

### Benchmark Results Interpretation

**Performance Metrics:**
- **Steps per Second**: Higher is better (indicates faster execution)
- **Average Step Time**: Lower is better (indicates faster execution)
- **Firing Rate**: Depends on use case (higher may indicate more activity)
- **Memory Usage**: Lower is better (indicates memory efficiency)

**Statistical Analysis:**
- **Min/Max**: Range of step execution times
- **Mean**: Average step execution time
- **Std Dev**: Consistency of step execution times
- **95th/99th Percentile**: Worst-case performance

## 4. Advanced Features

### Advanced Scripting

NLM provides advanced scripting capabilities for automation and customization:

```python
#!/usr/bin/env python3
import nlm
import json

# Example script: Custom configuration generation
def generate_custom_config():
    # Load default configuration
    config = nlm.createDefaultConfig()
    
    # Apply custom settings
    config.set("neuron_count", 2000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.25)
    
    # Enable specific features
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.enable", True)
    
    # Save to file
    config.saveToFile("custom_config.json")
    print("Custom configuration saved to custom_config.json")

# Example script: Advanced experiment control
def run_advanced_experiment():
    # Create brain with custom configuration
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run custom simulation
    for step in range(1000):
        brain.step(step, step * 0.001)
        
        # Apply custom stimuli at specific steps
        if step % 100 == 0:
            # Inject current into specific neurons
            for i in range(10):
                brain.injectCurrent(nlm.NeuronId(i), 30.0f)
    
    # Save results
    brain.save("experiment_results.bin")
    print("Experiment results saved to experiment_results.bin")

if __name__ == "__main__":
    generate_custom_config()
    run_advanced_experiment()
```

### Advanced Brain Control

#### Neural Connectivity Manipulation
```cpp
// Apply small-world network topology
brainControl.applyConnectivityPattern("small_world", 1.0);

// Apply scale-free network topology
brainControl.applyConnectivityPattern("scale_free", 0.8);

// Apply clustered network topology
brainControl.applyConnectivityPattern("clustered", 1.2);
```

#### Plasticity Control
```cpp
// Enable/disable specific plasticity rules
std::map<std::string, bool> rules = {
    {"hebbian", true},
    {"stdp", true},
    {"structural", false},
    {"reward_modulated", true}
};
brainControl.setPlasticityRules(rules);

// Adjust plasticity parameters
std::map<std::string, double> parameters = {
    {"learning_rate", 0.002},
    {"time_constant", 25.0},
    {"max_weight", 1.5}
};
brainControl.adjustPlasticityParameters(parameters);
```

#### Neuromodulation Control
```cpp
// Apply different neuromodulation patterns
brainControl.applyNeuromodulationPattern("reward_based", 1.0);
brainControl.applyNeuromodulationPattern("exploration", 0.7);
brainControl.applyNeuromodulationPattern("novelty_detection", 0.5);
```

#### Developmental Control
```cpp
// Advance development to specific stage
brainControl.advanceDevelopment("critical_period");
brainControl.advanceDevelopment("maturation");
brainControl.advanceDevelopment("adult");

// Set developmental speed
brainControl.setDevelopmentalSpeed(2.0); // Faster development
brainControl.setDevelopmentalSpeed(0.5); // Slower development
```

### Advanced Agent Control

#### Subsystem Management
```cpp
// Enable specific agent subsystems
std::vector<std::string> subsystems = {
    "reward_modulation",
    "structural_plasticity",
    "development",
    "curiosity"
};
agentControl.enableAgentSubsystems(subsystems);

// Disable specific agent subsystems
agentControl.disableAgentSubsystems({"reward_modulation", "curiosity"});
```

#### Behavior Control
```cpp
// Set agent behavior
agentControl.setAgentBehavior("exploratory", {
    {"curiosity", 0.8},
    {"novelty_threshold", 0.3},
    {"exploration_drive", 1.0}
});

agentControl.setAgentBehavior("exploitive", {
    {"reward_sensitivity", 1.5},
    {"risk_tolerance", 0.2},
    {"memory_weight", 0.9}
});
```

#### World Environment Control
```cpp
// Create custom world objects
std::vector<WorldObject> objects = {
    WorldObject(5.0, 5.0, WorldObjectType::Resource, 10.0),
    WorldObject(15.0, 15.0, WorldObjectType::Hazard, 5.0),
    WorldObject(10.0, 20.0, WorldObjectType::Marker, 1.0)
};
agentControl.createWorldObjects(objects);

// Modify world environment
agentControl.modifyWorldEnvironment("complex"); // Larger world with more vision
agentControl.modifyWorldEnvironment("sparse");   // Sparse environment
agentControl.modifyWorldEnvironment("dense");    // Dense environment
```

## 5. Error Handling and Validation

### Enhanced Error Messages

The enhanced error handling provides detailed, actionable error messages:

#### Configuration Errors
```
ERROR: Invalid neuron_count: must be positive
  - Value: 0
  - Suggested fix: Set neuron_count to at least 1
  - Impact: Network cannot be initialized without neurons

ERROR: Invalid connection_probability: must be positive
  - Value: -0.1
  - Suggested fix: Set connection_probability to a positive value
  - Impact: Network connectivity will be incorrect
```

#### Runtime Errors
```
ERROR: No brain regions found for connectivity test!
  - Check: Configuration and brain initialization
  - Suggested action: Verify region_count in configuration
  - Test: Run with a valid configuration

ERROR: Failed to initialize brain!
  - Check: Configuration and available memory
  - Suggested action: Increase system resources or reduce network size
  - Test: Run with smaller configuration (--neurons 100)
```

#### Performance Warnings
```
WARNING: Neuron count 50000 is very large, this may impact performance.
  - Current value: 50000
  - Suggested maximum: 10000
  - Impact: Increased memory usage and longer execution time

WARNING: Unusual simulation timestep: 0.1s
  - Current value: 0.1
  - Suggested range: [0.0001, 0.01]s
  - Impact: May cause numerical instability
```

### Debugging Tools

The advanced debugging tools provide comprehensive analysis:

#### Neural Stability Analysis
```cpp
brainControl.performNeuralStabilityAnalysis();
// Analyzes network stability and detects potential issues
```

#### Network Topology Analysis
```cpp
brainControl.analyzeNetworkTopology("eigenvector");
// Analyzes network structure using eigenvector centrality
```

#### Critical Node Detection
```cpp
brainControl.detectCriticalNetworkNodes();
// Identifies critical nodes whose removal would disrupt network function
```

#### Performance Bottleneck Identification
```cpp
brainControl.identifyBottleneckAreas();
// Identifies areas of the network that may limit performance
```

#### Memory Usage Analysis
```cpp
brainControl.analyzeMemoryUsagePatterns();
// Analyzes memory usage patterns and suggests optimizations
```

#### Error Pattern Analysis
```cpp
brainControl.analyzeErrorPatterns("all");
// Analyzes error patterns and identifies systematic issues
```

## 6. Performance Optimization

### Configuration Optimization

The NLM provides configuration optimization for different performance goals:

#### Performance-Optimized Configuration
```cpp
auto perfConfig = std::make_shared<Config>();
perfConfig->set("neuron_count", 2000, ConfigSource::Runtime);
perfConfig->set("region_count", 2, ConfigSource::Runtime);
perfConfig->set("connection_probability", 0.20, ConfigSource::Runtime);
```

#### Memory-Optimized Configuration
```cpp
auto memConfig = std::make_shared<Config>();
memConfig->set("neuron_count", 500, ConfigSource::Runtime);
memConfig->set("region_count", 1, ConfigSource::Runtime);
memConfig->set("connection_probability", 0.10, ConfigSource::Runtime);
```

#### Accuracy-Optimized Configuration
```cpp
auto accConfig = std::make_shared<Config>();
accConfig->set("neuron_count", 3000, ConfigSource::Runtime);
accConfig->set("region_count", 3, ConfigSource::Runtime);
accConfig->set("connection_probability", 0.25, ConfigSource::Runtime);
```

### Performance Monitoring

Track key performance metrics during runtime:

```cpp
// Monitor simulation time
Timer timer;
double stepTime = timer.elapsedSeconds();

// Monitor memory usage
double memoryMB = brainControl.getMemoryUsage();

// Monitor neural activity
size_t activeNeurons = brain->getActiveNeuronCount();
size_t firingNeurons = brain->getFiringNeuronCount();
double firingRate = brain->getAverageFiringRate();
```

### Optimization Tips

1. **Network Size**: Start with smaller networks for debugging, then scale up
2. **Timestep**: Use smaller timesteps for stability, larger for performance
3. **Region Count**: Use multiple regions for better parallelization
4. **Connection Probability**: Balance between connectivity and sparsity
5. **Plasticity Parameters**: Adjust based on learning requirements

## 7. Advanced Examples

### Complete Advanced Usage Example

```cpp
#include "AdvancedFeatures.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Advanced NLM Usage Example ===\n" << std::endl;
    
    // 1. Create custom configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", 2000, ConfigSource::Default);
    config->set("region_count", 3, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("connection_probability", 0.25, ConfigSource::Default);
    
    // Enable advanced features
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    config->set("neuromod.dopamine.enable", true, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    config->set("memory.working_memory.enabled", true, ConfigSource::Default);
    config->set("memory.episodic_memory.enabled", true, ConfigSource::Default);
    
    // 2. Create brain and initialize
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // 3. Create advanced brain control
    AdvancedBrainControl brainControl(brain);
    
    // 4. Apply advanced connectivity pattern
    brainControl.applyConnectivityPattern("small_world", 1.0);
    
    // 5. Set advanced plasticity rules
    std::map<std::string, bool> rules = {
        {"hebbian", true},
        {"stdp", true},
        {"structural", true},
        {"reward_modulated", true}
    };
    brainControl.setPlasticityRules(rules);
    
    // 6. Apply neuromodulation patterns
    brainControl.applyNeuromodulationPattern("reward_based", 1.0);
    brainControl.applyNeuromodulationPattern("exploration", 0.7);
    
    // 7. Advance development
    brainControl.advanceDevelopment("critical_period");
    brainControl.setDevelopmentalSpeed(1.5);
    
    // 8. Run simulation with custom stimuli
    std::cout << "Running custom simulation..." << std::endl;
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Apply custom stimuli at specific steps
        if (step % 100 == 0) {
            // Inject current into sensory neurons
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                for (size_t i = 0; i < std::min(size_t(20), neurons.size()); ++i) {
                    if (neurons[i]->getType() == NeuronType::Sensory) {
                        brain->injectCurrent(neurons[i]->getId(), 50.0f);
                    }
                }
            }
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 200 steps
        if (step % 200 == 0) {
            std::cout << "Step " << step << ": " << brain->getTotalSpikeCount() 
                      << " spikes, " << brain->getFiringNeuronCount() 
                      << " firing neurons" << std::endl;
        }
    }
    
    // 9. Run benchmarks
    std::cout << "\nRunning performance benchmarks..." << std::endl;
    BenchmarkSuite benchmarkSuite;
    benchmarkSuite.runAllBenchmarks(config, 500);
    
    // 10. Generate comprehensive report
    std::string report = benchmarkSuite.generateFullReport();
    std::cout << "\n=== Benchmark Report ===\n" << report;
    
    // 11. Save results
    benchmarkSuite.saveResults("advanced_benchmark_results.json");
    
    std::cout << "\n=== Advanced Usage Complete ===\n" << std::endl;
    std::cout << "Key achievements:\n";
    std::cout << "✓ Custom configuration created\n";
    std::cout << "✓ Advanced connectivity pattern applied\n";
    std::cout << "✓ Enhanced plasticity rules configured\n";
    std::cout << "✓ Neuromodulation patterns applied\n";
    std::cout << "✓ Developmental processes controlled\n";
    std::cout << "✓ Custom simulation executed\n";
    std::cout << "✓ Performance benchmarks run\n";
    std::cout << "✓ Comprehensive report generated\n";
    
    return 0;
}
```

### Command-Line Advanced Usage

```bash
#!/bin/bash

# Example: Advanced command-line usage

# 1. Run with custom configuration and advanced options
./nlm \
  --config my_config.json \
  --verbose \
  --test-mode all \
  --steps 2000 \
  --neurons 2000 \
  --regions 4 \
  --conn-prob 0.25 \
  --timestep 0.0005

# 2. Run benchmark with specific configuration
./nlm \
  --benchmark \
  --test-mode connectivity \
  --steps 1000 \
  --neurons 1000 \
  --regions 2 \
  --conn-prob 0.15

# 3. Run plasticity benchmark
./nlm \
  --benchmark \
  --test-mode plasticity \
  --steps 500 \
  --neurons 500 \
  --regions 1 \
  --conn-prob 0.10

# 4. Run comprehensive test suite
./nlm \
  --test-mode all \
  --steps 1000 \
  --neurons 1000 \
  --regions 2 \
  --conn-prob 0.15 \
  --verbose
```

## 8. Troubleshooting

### Common Issues and Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| "No brain regions found" | Invalid region_count | Set region_count to at least 1 |
| "Failed to initialize brain" | Configuration error or memory issue | Check configuration and available memory |
| "No spikes detected" | Insufficient current or wrong neuron type | Increase current or use appropriate neuron types |
| "Performance too slow" | Network too large or inefficient configuration | Reduce network size or optimize configuration |
| "Memory usage high" | Large network or inefficient algorithms | Reduce network size or enable memory optimization |

### Debugging Commands

```bash
# Show detailed help
./nlm --help

# Show configuration validation
./nlm --config <(echo '{"neuron_count": 0}')

# Run minimal test
./nlm --test-mode connectivity --steps 100 --neurons 100

# Run with debug logging
./nlm --verbose --test-mode all

# Run benchmark
./nlm --benchmark --test-mode connectivity

# Show available test modes
./nlm --help | grep -A 20 "Test Mode"
```

### Performance Monitoring Commands

```bash
# Monitor system resources
./nlm --benchmark --test-mode all | tail -20

# Check benchmark results
./nlm --benchmark --test-mode all --steps 100 2>&1 | grep -E "(Time:|Steps:|Memory:|ERROR|WARNING)"

# Generate performance report
./nlm --benchmark --test-mode all --steps 100 --verbose > benchmark_report.txt
```

## 9. Migration Guide

### From Previous Versions

This guide helps migrating from previous NLM versions:

#### Command-Line Changes
- **Old**: `./nlm --config config.json`
- **New**: `./nlm --config config.json --test-mode all`

#### Configuration Changes
- **Old**: Limited configuration options
- **New**: Advanced configuration with validation and defaults

#### Test Mode Changes
- **Old**: Separate executables for each test type
- **New**: Unified command-line interface with test modes

### Upgrading Tips

1. **Update Configuration**: Review and update configuration files
2. **Test Modes**: Use appropriate test modes for your use case
3. **Benchmarks**: Run benchmarks to validate performance
4. **Documentation**: Read this guide for advanced features

## 10. Conclusion

The enhanced NLM framework provides:

1. **Advanced Command-Line Interface**: Flexible testing with multiple modes
2. **Comprehensive Configuration Management**: Presets, validation, and customization
3. **Benchmark Suite**: Performance monitoring and analysis
4. **Advanced Features**: Scripting, automation, and control
5. **Enhanced Error Handling**: Detailed diagnostics and suggestions
6. **Performance Optimization**: Configuration optimization and monitoring
7. **Advanced Examples**: Complete usage examples and tutorials

These improvements make NLM more powerful, flexible, and user-friendly for both beginners and advanced users.

---

*For more information, refer to the NLM documentation, examples, and help files.*
