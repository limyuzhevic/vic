# Advanced NLM Usage Guide

## Overview

This document provides comprehensive guidance for advanced users of the Neural Learning Machine (NLM) project. It covers the enhanced features, including the command-line interface, memory management, error handling, configuration system, and advanced usage patterns.

## Table of Contents

1. [Command-Line Interface](#command-line-interface)
    - [Basic Commands](#basic-commands)
    - [Experiment Commands](#experiment-commands)
    - [Configuration Commands](#configuration-commands)
    - [Utility Commands](#utility-commands)

2. [Memory Management](#memory-management)
    - [Memory Pools](#memory-pools)
    - [Performance Optimization](#performance-optimization)
    - [Memory Profiling](#memory-profiling)

3. [Error Handling](#error-handling)
    - [Error Categories](#error-categories)
    - [Error Recovery](#error-recovery)
    - [Custom Error Handlers](#custom-error-handlers)

4. [Configuration System](#configuration-system)
    - [Configuration Schema](#configuration-schema)
    - [Environment Variables](#environment-variables)
    - [Configuration File Formats](#configuration-file-formats)

5. [Advanced Features](#advanced-features)
    - [Interactive CLI](#interactive-cli)
    - [Batch Processing](#batch-processing)
    - [Performance Monitoring](#performance-monitoring)
    - [Extension Points](#extension-points)

6. [Python Bindings](#python-bindings)
    - [Installation](#installation)
    - [Basic Usage](#basic-usage)
    - [Advanced Features](#advanced-features-1)

## Command-Line Interface

### Basic Commands

The NLM CLI provides a comprehensive interface for running experiments, managing configuration, and monitoring the system.

#### Core Commands

**`nlm run`** - Run a simulation or experiment
```bash
nlm run --experiment phase6 --neurons 500 --duration 60 --output results.bench
```

**`nlm config`** - Manage configuration
```bash
nlm config list                    # List all configuration keys
nlm config get random_seed         # Get configuration value
nlm config set random_seed 42      # Set configuration value
nlm config reset                   # Reset to defaults
```

**`nlm experiment`** - Manage experiments
```bash
nlm experiment run --type phase6   # Run specific experiment
nlm experiment list               # List available experiments
nlm experiment status             # Check experiment status
```

**`nlm benchmark`** - Run performance benchmarks
```bash
nlm benchmark --neurons 1000 --duration 30 --output bench_results.txt
```

**`nlm info`** - Show system information
```bash
nlm info system                    # Show system information
nlm info config                    # Show configuration
nlm info version                   # Show version information
```

**`nlm test`** - Run unit tests
```bash
nlm test all                      # Run all tests
nlm test --neuron                 # Run neuron tests
```

**`nlm profile`** - Run performance profiling
```bash
nlm profile --duration 30 --neurons 500 --output profile.json
```

**`nlm replay`** - Replay saved experiment results
```bash
nlm replay --file experiment_result.bench
```

**`nlm save`** - Save current state
```bash
nlm save --file brain_state.bin --checkpoint
```

**`nlm load`** - Load saved state
```bash
nlm load --file brain_state.bin --checkpoint
```

#### Advanced Command Options

**Global Options**
- `-h, --help` - Show help message
- `-v, --version` - Show version information
- `-V, --verbose` - Enable verbose output
- `-q, --quiet` - Suppress non-error output
- `--log-file FILE` - Log output to file
- `--config FILE` - Load configuration from file
- `--key=value` - Set configuration key

#### Command Examples

**Running a complex experiment with custom configuration:**
```bash
nlm run \
  --experiment phase6 \
  --neurons 1000 \
  --duration 120 \
  --output experiment_1.bench \
  --config custom.cfg \
  --key=plasticity_enabled=true \
  --key=development_enabled=true
```

**Batch processing experiments:**
```bash
nlm batch <<EOF
run --experiment phase6 --neurons 500 --duration 60
config set random_seed 123
run --experiment phase4 --neurons 200 --duration 30
replay --file experiment_1.bench
EOF
```

**Interactive CLI mode:**
```bash
nlm interactive
```

## Memory Management

### Memory Pools

NLM implements advanced memory management with multiple memory pools optimized for different allocation patterns:

**Pool Types:**
- **Small**: < 1KB allocations (neuron data, small structures)
- **Medium**: 1KB - 64KB allocations (memory systems, large arrays)
- **Large**: 64KB - 1MB allocations (neural regions, big buffers)
- **Huge**: > 1MB allocations (large datasets, performance buffers)
- **Neuron**: Neuron-specific allocations (neuron states, spike data)
- **Synapse**: Synapse-specific allocations (synaptic weights, delays)
- **MemorySystem**: Memory system allocations (working memory, episodic memory)

**Benefits:**
- Reduced memory fragmentation
- Faster allocation/deallocation
- Better cache locality
- Lower memory overhead
- Improved performance for repeated allocations

### Performance Optimization

**Memory-Efficient Data Structures:**

```cpp
// Using memory-efficient vector with pool allocator
using NeuronVector = nlm::PoolVector<Neuron, nlm::MemoryPoolAllocator<Neuron>>;

// Pre-allocating memory for expected size
neuronVector.reserveExpected(1000);

// Using memory-efficient LRU cache for frequently accessed objects
using NeuronCache = nlm::MemoryEfficientLRUCache<NeuronId, Neuron>;

// Using string pool for interning string literals
using StringPool = nlm::StringPool;
```

**SIMD-Optimized Operations:**

```cpp
// Vectorized mathematical operations
float sum = nlm::SIMD::vectorizedSum(data, size);

// Vectorized compare and copy
nlm::SIMD::vectorizedCopy(src, dst, size, threshold);

// Vectorized normalization
nlm::SIMD::vectorizedNormalize(data, mean, stddev, size);
```

### Memory Profiling

**Monitoring Memory Usage:**

```bash
# Enable memory profiling
nlm run --experiment phase6 --profile-memory

# Generate memory report
nlm profile --output memory_report.json
```

**Memory Statistics:**

```cpp
// Get memory pool statistics
auto stats = nlm::MemoryManager::getInstance().getAllStats();
for (const auto& pair : stats) {
    std::cout << pair.first << ": " << pair.second.currentUsage << " bytes" << std::endl;
}

// Get memory usage for specific pool
auto neuronPool = nlm::MemoryManager::getInstance().getPool(nlm::MemoryPoolType::Neuron);
auto neuronStats = neuronPool->getStats();
```

## Error Handling

### Error Categories

NLM implements a comprehensive error classification system:

**Error Categories:**
- **Memory**: Memory allocation, access violations, corruption
- **Neural**: Neuron operations, synaptic computations, spike propagation
- **Plasticity**: STDP, Hebbian learning, structural plasticity
- **Development**: Synaptogenesis, pruning, maturation
- **Neuromodulation**: Dopamine, reward, prediction error
- **MemorySystem**: Working memory, episodic memory, associative memory
- **Prediction**: Prediction computation, error calculation
- **Cognition**: Concept formation, planning, attention
- **Sensory/Motor**: Sensory processing, motor commands
- **Environment**: Environment interaction, observation, action
- **Configuration**: Configuration loading, validation, saving
- **Logging**: Log file operations, output formatting
- **Simulation**: Simulation steps, reset, initialization

**Error Codes:**
- Memory errors: 1000-1999
- Neural errors: 2000-2999
- Plasticity errors: 3000-3999
- Development errors: 4000-4999
- Neuromodulation errors: 5000-5999
- Memory system errors: 6000-6999
- Prediction errors: 7000-7999
- Cognition errors: 8000-8999
- Sensory/Motor errors: 9000-9999
- Environment errors: 10000-10999
- Configuration errors: 11000-11999
- Logging errors: 12000-12999
- Simulation errors: 13000-13999

### Error Recovery

**Automatic Recovery:**

```cpp
// Safe execution with error recovery
try {
    auto result = nlm::ErrorRecovery::safeExecute(
        []() { return someOperation(); },
        defaultValue,
        "Some operation"
    );
} catch (const nlm::Error& e) {
    // Handle error
}
```

**Graceful Degradation:**

```cpp
// Check if error is recoverable
if (nlm::ErrorRecovery::isRecoverable(error)) {
    // Attempt recovery
    nlm::ErrorRecovery::resetSystemState();
    // Retry operation
} else {
    // Critical error - shutdown or emergency recovery
}
```

### Custom Error Handlers

**Implementing Custom Error Handler:**

```cpp
class CustomErrorHandler : public nlm::IErrorHandler {
public:
    void handleError(const nlm::Error& error) override {
        // Custom error handling logic
        if (error.getCategory() == nlm::ErrorCategory::Memory) {
            // Memory error handling
            handleMemoryError(error);
        } else {
            // General error handling
            logError(error);
        }
    }
    
    bool shouldThrow(const nlm::Error& error) const override {
        // Determine if error should be thrown
        return error.getCode() != nlm::ErrorCode::InvalidParameter;
    }
};
```

## Configuration System

### Configuration Schema

NLM uses a comprehensive configuration schema with validation:

**Configuration Definition:**
```cpp
nlm::ConfigSchema schema;
schema.addDefinition("random_seed", 42, "Random seed for reproducible experiments");
schema.addDefinition("simulation_timestep", 0.001, "Simulation time step in seconds");
schema.addDefinition("neuron_count", 500, "Number of neurons", "int", {nlm::RangeValidation(10, 100000)});
schema.addDefinition("connection_probability", 0.15, "Probability of connection between neurons", "float", {nlm::RangeValidation(0.0, 1.0)});
```

**Environment Variables:**
```bash
# Set configuration via environment variables
export NLM_RANDOM_SEED=42
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_NEURON_COUNT=500
export NLM_CONNECTION_PROBABILITY=0.15
```

**Command Line Configuration:**
```bash
# Set configuration via command line
nlm run --config=myconfig.cfg --key=random_seed=42 --key=neuron_count=1000
```

### Configuration File Formats

**JSON Configuration:**
```json
{
    "nlm": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 500,
        "connection_probability": 0.15,
        "plasticity_enabled": true,
        "development_enabled": true
    }
}
```

**YAML Configuration:**
```yaml
nlm:
  random_seed: 42
  simulation_timestep: 0.001
  neuron_count: 500
  connection_probability: 0.15
  plasticity_enabled: true
  development_enabled: true
```

**INI Configuration:**
```ini
[nlm]
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
connection_probability = 0.15
plasticity_enabled = true
development_enabled = true
```

## Advanced Features

### Interactive CLI

**Starting Interactive Mode:**
```bash
nlm interactive
```

**Interactive Commands:**
```
nlm> help              # Show help
nlm> config list       # List configuration
nlm> run --experiment phase6  # Run experiment
nlm> quit              # Exit interactive mode
```

**Features:**
- Command history
- Auto-completion
- Syntax highlighting
- Command templates
- Persistent configuration

### Batch Processing

**Batch File Format:**
```bash
# batch_commands.txt
# Comments start with #

run --experiment phase6 --neurons 500 --duration 60
config set random_seed 123
experiment run --type phase4
info system
profile --duration 30
replay --file previous_result.bench
save --file checkpoint.bin
```

**Execute Batch File:**
```bash
nlm batch --file batch_commands.txt
```

### Performance Monitoring

**Real-time Monitoring:**
```bash
# Start performance monitoring
nlm run --experiment phase6 --monitor --output monitor.json

# Generate performance report
nlm profile --output performance_report.html
```

**Monitoring Metrics:**
- Memory usage
- CPU utilization
- Simulation time
- Spike rate
- Learning progress

### Extension Points

**Adding Custom Commands:**

```cpp
class CustomCLI {
public:
    void registerCommand(const std::string& name, std::function<int(const std::vector<std::string>&)> callback) {
        // Register custom command
    }
};
```

**Adding Custom Plugins:**

```cpp
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    virtual void initialize(const nlm::Config& config) = 0;
    virtual void shutdown() = 0;
    virtual std::string getName() const = 0;
};
```

## Python Bindings

### Installation

**Prerequisites:**
```bash
# Install Python bindings
pip install -e .

# Build with Python bindings enabled
cmake -DNLM_ENABLE_PYTHON=ON ..
make -j4
```

**Build Options:**
```bash
# Disable Python bindings
cmake -DNLM_ENABLE_PYTHON=OFF ..

# Specify Python version
cmake -DPYTHON_EXECUTABLE=/usr/bin/python3 ..
```

### Basic Usage

**Import and Initialize:**
```python
import nlm

# Initialize NLM with default configuration
brain = nlm.Brain()
brain.initialize()

# Run a simple experiment
result = brain.run_experiment('phase6', neuron_count=500, duration=60)
print(f"Experiment result: {result}")
```

**Configuration:**
```python
import nlm

# Create custom configuration
config = nlm.Config()
config.set('random_seed', 42)
config.set('neuron_count', 1000)
config.set('plasticity_enabled', True)

# Initialize brain with custom configuration
brain = nlm.Brain(config)
brain.initialize()
```

**Advanced Features:**
```python
# Neuron management
neurons = brain.get_neurons()
for neuron in neurons:
    print(f"Neuron {neuron.get_id()}: type={neuron.get_type()}, potential={neuron.get_potential()}")

# Synaptic operations
for synapse in neurons[0].get_synapses():
    print(f"Synaptic weight: {synapse.get_weight()}")

# Memory management
memory = brain.get_memory()
working_memory = memory.get_working_memory()
print(f"Working memory size: {working_memory.size()}")

# Event handling
brain.add_event_listener('spike', handle_spike_event)
brain.add_event_listener('learning', handle_learning_event)
```

### Advanced Python Features

**Custom Python Modules:**
```python
# Create custom Python module for NLM extensions
import nlm
import numpy as np

class CustomModule(nlm.Module):
    def __init__(self, config):
        super().__init__(config)
    
    def process_neural_data(self, input_data):
        # Custom neural data processing
        processed = np.dot(input_data, self.weights)
        return self.activation_function(processed)
    
    def train(self, training_data, labels):
        # Custom training algorithm
        pass

# Register custom module
module = CustomModule(config)
nlm.register_module(module)
```

**Real-time Visualization:**
```python
import matplotlib.pyplot as plt
import nlm

class Visualizer(nlm.Module):
    def __init__(self, config):
        super().__init__(config)
        self.spike_data = []
    
    def visualize_spikes(self):
        plt.figure(figsize=(10, 6))
        plt.plot(self.spike_data)
        plt.title('Spike Activity')
        plt.xlabel('Time (ms)')
        plt.ylabel('Neuron Index')
        plt.show()
    
    def on_spike(self, event):
        self.spike_data.append(event.neuron_id)

# Add visualizer to brain
visualizer = Visualizer(config)
brain.add_module(visualizer)
```

## Migration Guide

### From Phase 5 to Phase 6

**Key Changes:**
1. Enhanced configuration system with validation
2. Improved error handling with custom error types
3. Advanced memory management with multiple pools
4. Comprehensive logging system with categories
5. New command-line interface with batch processing
6. Enhanced experiment framework with better integration

**Migration Steps:**
1. Update configuration files to use new schema
2. Add error handling for new error types
3. Enable memory pools for better performance
4. Configure logging with appropriate levels and categories
5. Migrate to new CLI interface
6. Update experiment code to use new integration APIs

### Performance Optimization

**Initial Performance Tuning:**
```bash
# Enable all optimizations
cmake -DNLM_ENABLE_OPTIMIZATION=ON -DNLM_ENABLE_ASSERTIONS=ON ..

# Build with maximum optimization
make -j4 -DNLM_ENABLE_PROFILING=ON

# Run with profiling
nlm profile --duration 60 --output profile.json
```

**Production Performance:**
```bash
# Use memory pools for better performance
export NLM_ENABLE_MEMORY_POOLS=ON

# Configure logging for production
export NLM_LOG_LEVEL=WARNING
export NLM_LOG_FILE=/var/log/nlm.log

# Set resource limits
ulimit -v 2097152  # 2GB virtual memory
ulimit -r 100    # 100 processes
```

## Troubleshooting

### Common Issues

**Memory Issues:**
- **Symptom:** Application crashes with out-of-memory errors
- **Solution:** Increase system memory or enable memory pools
- **Command:** `nlm run --experiment phase6 --memory-pool-size large`

**Configuration Issues:**
- **Symptom:** Application fails to start due to invalid configuration
- **Solution:** Validate configuration file with `nlm config validate <file>`
- **Command:** `nlm config list` to see all available configuration keys

**Performance Issues:**
- **Symptom:** Slow experiment execution
- **Solution:** Enable profiling and optimize configuration
- **Command:** `nlm profile --duration 30 --output profile.json`

**Error Handling Issues:**
- **Symptom:** Application crashes without helpful error messages
- **Solution:** Enable verbose logging and custom error handlers
- **Command:** `nlm run --verbose --error-handler custom`

### Debugging Tips

**Enable Debug Logging:**
```bash
export NLM_LOG_LEVEL=DEBUG
nlm run --experiment phase6 --log-file debug.log
```

**Check Memory Usage:**
```bash
nlm info system
nlm profile --memory-usage
```

**Validate Configuration:**
```bash
nlm config validate custom.cfg
nlm config list
```

**Run Tests:**
```bash
nlm test all
nlm test --neuron
nlm test --synapse
```

## Conclusion

The NLM project has been significantly enhanced with advanced features including:

1. **Comprehensive Logging System** with multiple levels and categories
2. **Advanced Configuration System** with validation and multiple file formats
3. **Robust Error Handling** with custom error types and recovery mechanisms
4. **Memory Management** with multiple memory pools and optimization
5. **Command-Line Interface** with interactive and batch processing modes
6. **Performance Monitoring** with profiling and benchmarking tools
7. **Extension Framework** for custom modules and plugins
8. **Python Bindings** for easy integration and scripting

These improvements make NLM more accessible to users with different skill levels, from beginners to advanced researchers, while maintaining the experimental nature of the project. The enhanced error handling, memory management, and configuration system provide a solid foundation for future development and scaling.

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [Python Bindings Documentation](https://nlm.readthedocs.io/python-bindings.html)
- [Advanced Configuration](https://nlm.readthedocs.io/configuration.html)
- [Memory Management](https://nlm.readthedocs.io/memory-management.html)
- [Error Handling](https://nlm.readthedocs.io/error-handling.html)

---

*Last updated: 2026-10-04*
*NLM Phase 6 Final Integration*
