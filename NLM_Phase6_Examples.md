# NLM Phase 6 Examples and Tutorials
# Neural Learning Machine - Getting Started with Phase 6

This document provides practical examples and tutorials for using NLM Phase 6. It covers:

- Basic usage and getting started
- Advanced configuration examples
- Experiment setup and execution
- Plugin development examples
- Troubleshooting and common issues

## Table of Contents

1. [Getting Started](#getting-started)
2. [Basic Examples](#basic-examples)
3. [Advanced Configuration](#advanced-configuration)
4. [Experiment Examples](#experiment-examples)
5. [Plugin Development](#plugin-development)
6. [Performance Optimization](#performance-optimization)
7. [Troubleshooting](#troubleshooting)
8. [Reference Examples](#reference-examples)

## Getting Started

### Installation

NLM Phase 6 is typically built and installed using CMake:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

After building, you should have the following executables:

- `nlm` - Main NLM Phase 6 executable (demo mode by default)
- `nlm_phase6_demo` - Phase 6 demo executable
- `nlm_phase3_demo` - Phase 3 demo executable
- `nlm_phase4_demo` - Phase 4 demo executable

### Basic Usage

The simplest way to start NLM Phase 6 is to run the demo mode:

```bash
nlm demo
```

This will run a comprehensive Phase 6 integration demo with default settings (1000 neurons, 1000 steps).

### First Steps

1. **Run the basic demo:**
   ```bash
   nlm demo
   ```

2. **Check the help:**
   ```bash
   nlm --help
   ```

3. **Run a debug session:**
   ```bash
   nlm debug --verbose
   ```

4. **Run integration tests:**
   ```bash
   nlm integration
   ```

## Basic Examples

### Example 1: Basic Demo

This example runs a simple Phase 6 demo with default settings:

```bash
nlm demo
```

**What it does:**
- Creates a brain with 1000 neurons
- Initializes all integrated systems
- Runs basic integration tests
- Demonstrates memory, neuromodulation, and cognition systems
- Shows checkpointing and replay capabilities

**Expected output:**
- Integration verification results
- Individual system test results
- Final experiment results
- Complete system status report

### Example 2: Custom Configuration

This example uses a custom configuration file:

```bash
# Create a custom config file: myconfig.cfg
cat > myconfig.cfg << EOF
neuron_count = 500
region_count = 2
simulation_timestep = 0.001
random_seed = 123
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
enable_development = true
enable_replay = true
EOF

# Run demo with custom config
nlm demo --config myconfig.cfg
```

**Configuration options used:**
- 500 neurons across 2 regions
- Custom random seed for reproducibility
- Enhanced STDP parameters
- Development and replay enabled

### Example 3: Debug Session

This example runs a debug session to understand brain behavior:

```bash
nlm debug --verbose --steps 500
```

**What it does:**
- Creates a brain with 100 neurons (debug mode)
- Enables verbose logging
- Runs 500 simulation steps with detailed output
- Shows step-by-step progress
- Displays memory traces and system status

### Example 4: Integration Test

This example runs comprehensive integration verification:

```bash
nlm integration
```

**What it does:**
- Verifies all systems are properly connected
- Tests memory integration
- Tests neuromodulation integration
- Tests checkpointing functionality
- Tests replay functionality
- Reports any integration issues

## Advanced Configuration

### Example 1: Large-Scale Experiment

This example runs a large-scale experiment with multiple regions:

```bash
# Create a large config file: large_config.cfg
cat > large_config.cfg << EOF
neuron_count = 5000
region_count = 4
connection_probability = 0.05
simulation_timestep = 0.001
random_seed = 42
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
enable_development = true
enable_replay = true
working_memory_capacity = 500
episodic_memory_max_episodes = 2000
EOF

# Run large-scale demo
nlm demo --config large_config.cfg --steps 5000 --checkpoint /tmp/large_brain.bin
```

**Configuration highlights:**
- 5000 neurons across 4 regions
- Lower connection probability for sparser connectivity
- Development and replay enabled
- Larger memory capacities

### Example 2: Performance Benchmark

This example benchmarks the performance of NLM Phase 6:

```bash
nlm benchmark --neurons 2000 --steps 10000
```

**What it does:**
- Creates a brain with 2000 neurons
- Runs 10,000 simulation steps
- Measures performance metrics
- Reports steps per second
- Calculates neurons processed per second
- Shows firing rate statistics

### Example 3: Custom Plugin Development

This example demonstrates how to develop custom plugins:

```bash
# Create a simple plugin: myplugin.cpp
cat > myplugin.cpp << EOF
#include "brain/Brain.hpp"
#include <iostream>

class MyPlugin {
public:
    virtual ~MyPlugin() {}
    virtual void initialize(nlm::Brain* brain) = 0;
    virtual void update(nlm::Brain* brain, double dt) = 0;
    virtual void shutdown() = 0;
};

class MyCustomPlugin : public MyPlugin {
public:
    void initialize(nlm::Brain* brain) override {
        std::cout << "MyPlugin initialized" << std::endl;
        // Store brain reference
        brain_ = brain;
    }
    
    void update(nlm::Brain* brain, double dt) override {
        // Implement custom plugin logic here
        if (brain_->getDopamine() && brain_->getDopamine()->getLevel() > 0.5f) {
            // Apply custom neuromodulation
            brain_->injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
        }
    }
    
    void shutdown() override {
        std::cout << "MyPlugin shutdown" << std::endl;
    }
private:
    nlm::Brain* brain_ = nullptr;
};

// Plugin factory function
extern "C" MyPlugin* create_plugin() {
    return new MyCustomPlugin();
}

extern "C" void destroy_plugin(MyPlugin* plugin) {
    delete plugin;
}
EOF

# Compile the plugin
nlm-demo --plugin myplugin.cpp
```

## Experiment Examples

### Example 1: Learning Experiment

This example demonstrates learning through experience:

```bash
# Create experiment configuration
cat > learning_experiment.cfg << EOF
neuron_count = 1000
region_count = 2
simulation_timestep = 0.001
random_seed = 12345
connection_probability = 0.1
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
enable_development = false
enable_replay = false
EOF

# Run learning experiment
nlm demo --config learning_experiment.cfg --steps 2000
```

**What it does:**
- Runs a learning experiment with correlated activity
- Demonstrates STDP-based learning
- Shows synaptic weight changes over time
- Demonstrates memory formation

### Example 2: Memory Experiment

This example focuses on memory formation and recall:

```bash
# Create memory experiment configuration
cat > memory_experiment.cfg << EOF
neuron_count = 500
region_count = 1
simulation_timestep = 0.001
random_seed = 999
connection_probability = 0.2
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
enable_development = true
enable_replay = true
working_memory_capacity = 100
episodic_memory_max_episodes = 50
EOF

# Run memory experiment
nlm demo --config memory_experiment.cfg --steps 1500 --replay
```

**What it does:**
- Tests memory formation and storage
- Demonstrates working memory dynamics
- Shows episodic memory encoding and replay
- Tests memory consolidation

## Plugin Development

### Basic Plugin Structure

Here's a template for creating NLM Phase 6 plugins:

```cpp
// myplugin.hpp
#pragma once

#include "brain/Brain.hpp"
#include <string>

class MyPlugin {
public:
    virtual ~MyPlugin() {}
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual void initialize(nlm::Brain* brain) = 0;
    virtual void update(nlm::Brain* brain, double dt) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
};
```

```cpp
// myplugin.cpp
#include "myplugin.hpp"
#include <iostream>

class MyCustomPlugin : public MyPlugin {
public:
    std::string getName() const override { return "MyCustomPlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void initialize(nlm::Brain* brain) override {
        std::cout << "MyCustomPlugin initialized" << std::endl;
        brain_ = brain;
        // Initialize plugin-specific state
    }
    
    void update(nlm::Brain* brain, double dt) override {
        // Implement plugin logic here
        if (brain_ && isEnabled_) {
            // Access brain systems
            auto* workingMemory = brain_->getWorkingMemory();
            auto* dopamine = brain_->getDopamine();
            
            if (workingMemory && dopamine) {
                // Plugin implementation
                // For example, modify dopamine based on working memory
                float dopamineLevel = dopamine->getLevel();
                float memoryFactor = workingMemory->getMemoryActivity();
                dopamine->setLevel(dopamineLevel * (1.0f + memoryFactor * 0.1f));
            }
        }
    }
    
    void shutdown() override {
        std::cout << "MyCustomPlugin shutdown" << std::endl;
        brain_ = nullptr;
    }
    
    bool isEnabled() const override {
        return isEnabled_;
    }
    
    void enable() override {
        isEnabled_ = true;
        std::cout << "MyCustomPlugin enabled" << std::endl;
    }
    
    void disable() override {
        isEnabled_ = false;
        std::cout << "MyCustomPlugin disabled" << std::endl;
    }
    
private:
    nlm::Brain* brain_ = nullptr;
    bool isEnabled_ = false;
};
```

### Plugin Development Guidelines

1. **Plugin Interface**: Implement the required plugin interfaces
2. **Brain Access**: Use the provided brain accessor methods
3. **Thread Safety**: Ensure thread-safe access to shared resources
4. **Error Handling**: Implement proper error handling
5. **Resource Management**: Properly manage plugin resources
6. **Lifecycle**: Implement proper initialization and cleanup

## Performance Optimization

### Example 1: Performance Tuning

This example optimizes performance by adjusting configuration:

```bash
# Performance-optimized configuration
nlm demo --neurons 2000 --steps 5000 \
    --memory-capacity 200 \
    --plasticity-mod 0.8 \
    --sensory-gain 0.5 \
    --attention-strength 0.6
```

### Example 2: Memory Optimization

This example optimizes memory usage:

```bash
# Memory-optimized configuration
nlm demo --neurons 1000 --steps 3000 \
    --memory-capacity 100 \
    --replay \
    --develop
```

### Example 3: Large-Scale Processing

This example processes large networks efficiently:

```bash
nlm benchmark --neurons 5000 --steps 10000
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: "Failed to initialize brain"**

**Solution:**
1. Check that all required libraries are installed
2. Verify that the system has sufficient memory
3. Check configuration file format and syntax
4. Ensure that CMake was configured with the right options

**Issue 2: "Integration verification failed"**

**Solution:**
1. Check that all systems are properly linked
2. Verify that component dependencies are satisfied
3. Review integration test output for specific failures
4. Check system logs for error details

**Issue 3: "Performance is slow"**

**Solution:**
1. Reduce neuron count for testing
2. Check for memory leaks
3. Optimize configuration parameters
4. Consider using a more powerful machine
5. Check for inefficient code in brain.cpp or related files

**Issue 4: "Memory usage is high"**

**Solution:**
1. Reduce working memory capacity
2. Disable unnecessary systems
3. Monitor memory usage with system tools
4. Check for memory leaks in the code

### Getting Additional Help

If you encounter issues:

1. **Use the help system:**
   ```bash
   nlm --help
   ```

2. **Check documentation:**
   - Look at the NLM_Phase6_Documentation.md file
   - Check the docs directory for additional documentation

3. **Submit an issue:**
   - Visit the NLM project website
   - Submit issues to the NLM GitHub repository
   - Include configuration and error output

4. **Get support:**
   - Join the NLM community forums
   - Contact the NLM development team
   - Check the NLM project wiki

## Reference Examples

### Complete Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `nlm demo` | Run basic Phase 6 demo | `nlm demo --neurons 500 --steps 1000` |
| `nlm debug` | Debug session with verbose output | `nlm debug --verbose --steps 500` |
| `nlm integration` | Integration verification tests | `nlm integration` |
| `nlm benchmark` | Performance benchmarking | `nlm benchmark --neurons 2000 --steps 5000` |
| `nlm --help` | Show help information | `nlm --help` |
| `nlm --version` | Show version information | `nlm --version` |

### Configuration File Examples

**Minimal Configuration:**
```
neuron_count = 100
region_count = 1
random_seed = 42
```

**Advanced Configuration:**
```
neuron_count = 1000
region_count = 4
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 12345
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
enable_development = true
enable_replay = true
working_memory_capacity = 200
episodic_memory_max_episodes = 1000
```

### Output File Examples

**Results Output:**
```bash
# Save experiment results
nlm demo --output /tmp/experiment_results.txt

# Save benchmark results
nlm benchmark --output /tmp/benchmark_results.txt
```

**Checkpoint Files:**
```bash
# Save brain state for later loading
nlm demo --checkpoint /tmp/brain_state.bin

# Load brain state from checkpoint
nlm debug --checkpoint /tmp/brain_state.bin
```

### Expected Output Examples

**Demo Mode Output:**
```
=== NLM Phase 6 Demo Mode ===
Configuration:
  Neurons: 1000
  Regions: 1
  Steps: 1000
  Seed: 42
  Checkpoint: Disabled
  Replay: Disabled
  Development: Disabled

=== Integration Verification ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
Checkpointing: WORKING

=== DEMO RESULTS ===
Total reward: 0.123456
Avg firing rate: 12.345678
Episodes stored: 23.000000
Dopamine level: 0.567890
Wall clock time: 5.234567s
```

**Debug Mode Output:**
```
=== NLM Phase 6 Debug Mode ===
Verbose mode enabled
Brain initialized successfully
Running debug simulation...
Step 0: Spikes: 0 Active: 10 WorkingMem: 0
Step 10: Spikes: 5 Active: 15 WorkingMem: 2
Step 20: Spikes: 12 Active: 23 WorkingMem: 5
...
Debug mode complete
```

**Benchmark Mode Output:**
```
=== NLM Phase 6 Benchmark Mode ===
Running benchmark with 2000 neurons, 10000 steps
Step 0/10000
Step 1000/10000
...
Step 9000/10000

=== BENCHMARK RESULTS ===
Total steps: 10000
Duration: 45.678901s
Steps per second: 218.896789
Neurons processed per second: 437.793578
Average firing rate: 8.123456
Total spikes: 123456
```

## Quick Reference

### Essential Commands

**Start NLM Phase 6:**
```bash
nlm demo
```

**Get help:**
```bash
nlm --help
```

**Check version:**
```bash
nlm --version
```

**Run integration tests:**
```bash
nlm integration
```

**Performance benchmark:**
```bash
nlm benchmark --neurons 1000 --steps 1000
```

**Debug session:**
```bash
nlm debug --verbose
```

### Configuration Tips

1. **Start small:** Use 100 neurons for initial testing
2. **Enable development:** `nlm demo --develop`
3. **Enable replay:** `nlm demo --replay`
4. **Save checkpoints:** `nlm demo --checkpoint /path/to/file`
5. **Increase steps:** `nlm demo --steps 5000`

### Common Configurations

**Small System:**
```bash
nlm demo --neurons 100 --steps 500 --develop --replay
```

**Medium System:**
```bash
nlm demo --neurons 1000 --steps 2000 --develop --replay --checkpoint /tmp/brain.bin
```

**Large System:**
```bash
nlm demo --neurons 5000 --steps 10000 --regions 4 --develop --replay
```

### Next Steps

1. **Explore the documentation:** Read the NLM_Phase6_Documentation.md file
2. **Run examples:** Try the provided example commands
3. **Customize:** Modify the examples to suit your needs
4. **Debug:** Use debug mode for troubleshooting
5. **Optimize:** Use benchmark mode to optimize performance
6. **Extend:** Develop plugins to extend functionality

---

*For more information, visit the NLM project website or consult the NLM_Phase6_Documentation.md file.*
