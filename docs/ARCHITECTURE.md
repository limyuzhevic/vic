# NLM - Neural Learning Machine
## Advanced AI Development Platform

**Version 2.0 - Phase 2 Enhanced**

This document provides comprehensive information about the NLM (熙然) project, an experimental artificial developmental brain platform. This version includes significant improvements in configuration, memory management, performance optimization, debugging, and extensibility.

## Table of Contents

1. Overview
2. Configuration System Improvements
3. Memory Management and Performance Optimizations
4. Debugging and Profiling Tools
5. Command-Line Interface Enhancements
6. Extensibility Features
7. Installation and Usage
8. Development Guide
9. API Reference
10. Examples

## 1. Overview

NLM is an experimental artificial developmental brain project designed to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Real Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Memory Systems**: Working memory, episodic memory, semantic memory, procedural memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error systems
- **Prediction Systems**: Temporal sequence learning and action consequence prediction
- **Cognitive Mechanisms**: Attention, planning, concept formation, and self-model
- **Development System**: Stage-based plasticity modulation
- **Performance Infrastructure**: Memory pools, SIMD optimization, event queues, and parallel processing

### Architecture Philosophy

NLM follows a modular, layered architecture where each component can be developed and tested independently while maintaining integration through well-defined interfaces.

## 2. Configuration System Improvements

### Enhanced Configuration Capabilities

The configuration system has been completely redesigned to support:

#### JSON/YAML Format Support
- Advanced JSON/YAML configuration files
- Rich data types (arrays, objects, nested structures)
- Validation and schema support
- Configuration inheritance and merging

#### Improved Type Detection and Validation
- Automatic type detection from string values
- Configuration validation with detailed error messages
- Parameter range checking
- Dependency validation

#### Advanced Command-Line Interface
- Multiple configuration file support
- Inline configuration setting
- Configuration validation and comparison
- Environment variable integration

### Configuration File Examples

#### Basic Configuration (key=value format)
```
# Basic NLM Configuration
neuron_count = 1000
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

#### JSON Configuration
```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true
}
```

### Configuration Loading and Merging

Configurations are loaded in the following order (lowest to highest priority):

1. Default configuration
2. Configuration file (JSON, YAML, or key=value format)
3. Command-line arguments
4. Runtime overrides

### Configuration Validation

The configuration system validates:
- Parameter ranges and dependencies
- Required parameters for specific modes
- Compatibility between different configuration options
- Resource availability and system constraints

## 3. Memory Management and Performance Optimizations

### Advanced Memory Pool System

NLM implements a sophisticated memory pool system for efficient memory management:

#### Neuron Memory Pool
- Cache-aligned neuron state storage
- Structure-of-Arrays (SoA) layout for SIMD optimization
- Lock-free allocation and deallocation
- Pre-allocated memory blocks for reduced overhead

#### Synapse Memory Pool
- Specialized synapse storage with fast lookup
- Precomputed source/destination neuron indices
- Eligibility trace storage
- Short-term plasticity state storage

### Performance Optimizations

#### SIMD Vectorization
- AVX2 and AVX512 optimized neural computations
- Vectorized synaptic weight updates
- Parallel spike processing
- Memory bandwidth optimization

#### Event-Driven Processing
- Lock-free event queues for spike events
- Time-bucketed delayed spike processing
- Priority queues for time-sensitive events
- Batch event processing for throughput

#### Parallel Processing
- Multi-threaded neural population updates
- Thread-local storage optimization
- Synchronized spike processing
- Load balancing across cores

### Performance Monitoring

The system includes comprehensive performance monitoring:

#### Performance Statistics
- Allocation/deallocation tracking
- Memory usage monitoring
- Event processing timing
- Computational throughput measurement

#### Profiling Tools
- Time profiling for critical functions
- Memory allocation profiling
- Event flow tracing
- Performance bottleneck identification

## 4. Debugging and Profiling Tools

### Advanced Debugging Infrastructure

NLM provides comprehensive debugging capabilities:

#### Memory Leak Detection
- Real-time memory allocation tracking
- Deallocation verification
- Leak detection and reporting
- Type-specific leak tracking

#### Time Profiling
- Function call timing
- Critical section profiling
- Performance bottleneck identification
- Report generation (text, JSON)

#### Event Flow Tracing
- Event recording and replay
- Thread synchronization analysis
- Time-series event analysis
- Pattern detection

### Interactive Debugging Interface

#### Breakpoints and Watchpoints
- Conditional breakpoints
- Expression evaluation
- Variable inspection
- State management

#### Debugging Commands
- Step over, step into, step out
- Continue execution
- Variable evaluation
- Object inspection

### Visual Debugging Tools

#### Network Visualization
- Neural network topology display
- Connection strength visualization
- Activity heatmaps
- Pattern analysis

#### Performance Visualization
- Memory usage graphs
- Performance timelines
- Event flow diagrams
- Interactive HTML reports

## 5. Command-Line Interface Enhancements

### Advanced CLI Options

NLM now supports a comprehensive command-line interface:

#### Basic Options
```
--help, -h                    Show basic help
--advanced-help               Show all available options
--version, -v                 Show version information
```

#### Configuration Options
```
--config file, -c file        Load configuration from file
--set key=value              Set configuration key-value pair
--set-defaults               Use default configuration
--validate-config            Validate configuration
--show-config                Display current configuration
```

#### Performance Options
```
--perf-profile                 Enable performance profiling
--debug                        Enable debug mode
--analyze-memory              Analyze memory usage
--analyze-plasticity          Analyze plasticity effects
--analyze-learning            Analyze learning patterns
```

#### Output Options
```
--quiet                        Suppress non-error output
--verbose                      Enable verbose output
--format TEXT/JSON/CSV/HTML     Output format
--json-output FILE             Save results as JSON
--csv-output FILE              Save results as CSV
--report HTML                  Generate HTML report
```

#### Experiment Options
```
--experiment test1              Run test 1: Basic connectivity
--experiment test2              Run test 2: Plasticity learning
--experiment test3              Run test 3: STDP verification
--experiment all               Run all experiments
```

### Usage Examples

#### Basic Usage
```bash
# Run with default configuration
./nlm

# Run with custom configuration
./nlm --config my_config.json

# Run specific experiment
./nlm --experiment test2 --log-level DEBUG
```

#### Advanced Configuration
```bash
# Set multiple parameters
./nlm --set neuron_count=2000 --set connection_probability=0.2
./nlm --set --stdp_ltp_weight=0.03 --set --stdp_ltd_weight=0.028

# Save and load brain state
./nlm --save-state brain.bin
# ... later ...
./nlm --load-state brain.bin
```

#### Performance Analysis
```bash
# Enable performance profiling
./nlm --perf-profile --analyze-memory --analyze-plasticity

# Generate detailed report
./nlm --json-output results.json --csv-output data.csv --report html
```

## 6. Extensibility Features

### Plugin Architecture

NLM supports easy addition of new components:

#### Component Addition
- New neuron dynamics implementations
- Custom plasticity rules
- Additional neuromodulators
- Novel memory systems

#### Interface Extensions
- Custom configuration schemas
- New output formats
- Additional debugging tools
- Visualization modules

### Development Guidelines

#### Coding Standards
- Consistent with existing codebase patterns
- Comprehensive documentation
- Error handling and validation
- Performance considerations

#### Testing and Validation
- Unit tests for all components
- Integration testing for system interactions
- Performance benchmarking
- Memory leak detection

## 7. Installation and Usage

### Build Instructions

#### Prerequisites
```bash
# Required dependencies
g++ 9.0 or higher
CMake 3.16 or higher
Python 3.7 or higher (for tools)
```

#### Build Process
```bash
# Create build directory
mkdir build
cd build

# Configure with options
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_PERFORMANCE_ENABLED=ON \
         -DNLM_SIMD_AVX512=ON \
         -DNLM_THREAD_COUNT=8

# Build
make -j4
```

#### Install
```bash
# Install system-wide (optional)
sudo make install

# Install locally
sudo make install PREFIX=/usr/local
```

### Running Examples

#### Phase 2 Demo
```bash
./nlm --help
./nlm --advanced-help
./nlm --version
```

#### Configuration Examples
```bash
# Use default configuration
./nlm

# Use custom configuration
./nlm --config configs/my_config.json

# Override configuration with command line
./nlm --neuron-count 1000 --timestep 0.0001
```

## 8. Development Guide

### Developing New Components

#### Neuron Dynamics
```cpp
// Example: Custom neuron dynamics
class CustomNeuron : public Neuron {
public:
    void stepLIF(Timestamp currentTime, TimestepDuration dt) override {
        // Implement custom dynamics
    }
};
```

#### Plasticity Rules
```cpp
// Example: Custom plasticity rule
class CustomPlasticity : public PlasticityRule {
public:
    void applyPlasticity(Synapse* synapse, float preSpikeTime, float postSpikeTime) override {
        // Implement custom plasticity
    }
};
```

#### Memory Systems
```cpp
// Example: Custom memory system
class CustomMemory : public NeuralWorkingMemory {
public:
    void store(const NeuralPattern& pattern) override {
        // Implement custom storage
    }
    
    NeuralPattern retrieve() override {
        // Implement custom retrieval
    }
};
```

### Debugging Development Code

#### Using Debugging Tools
```bash
# Enable debugging
./nlm --debug --perf-profile

# Set breakpoints
# (In your code)
debugger.addBreakpoint("neuron_count > 1000", "Check neuron count limit");

# Inspect variables
std::string neuronInfo = debugger.inspectVariable("currentNeuron");
```

#### Performance Analysis
```bash
# Profile your code
./nlm --perf-profile --analyze-memory

# Check for bottlenecks
./nlm --debug --perf-profile --analyze-plasticity
```

## 9. API Reference

### Core Classes

#### Brain
```cpp
class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    bool initialize();
    void step(SimulationStep currentStep);
    void receiveSensoryInput(const SensoryInput& input);
    float getAverageFiringRate() const;
    size_t getTotalSpikeCount() const;
    // ... more methods
};
```

#### Neuron
```cpp
class Neuron {
public:
    Neuron(NeuronId id);
    void setType(NeuronType type);
    void injectCurrent(MembranePotential current);
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    bool checkThreshold() const;
    // ... more methods
};
```

#### Config
```cpp
class Config {
public:
    Config();
    bool loadFromFile(const std::string& filepath);
    bool loadFromJson(const std::string& jsonContent);
    bool loadFromArgs(int argc, char** argv);
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool has(const std::string& key) const;
    std::string summary() const;
};
```

### Performance Classes

#### MemoryPool
```cpp
template<typename T>
class MemoryPool {
public:
    MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4);
    T* allocate();
    void deallocate(T* obj);
    size_t allocatedCount() const;
    size_t availableCount() const;
};
```

#### PerformanceMonitor
```cpp
class PerformanceMonitor {
public:
    void reset();
    void recordStep(size_t steps = 1);
    void recordMemoryAllocation(size_t bytes);
    void recordMemoryDeallocation(size_t bytes);
    double getStepsPerSecond() const;
    double getMemoryAllocationRate() const;
    void printReport(std::ostream& out = std::cout) const;
};
```

### Debugging Classes

#### MemoryTracker
```cpp
class MemoryTracker {
public:
    static MemoryTracker& getInstance();
    void trackAllocation(void* address, size_t size, const std::string& type,
                         const std::string& file = "", int line = 0);
    void trackDeallocation(void* address);
    std::vector<AllocationInfo> checkForLeaks() const;
    void clear();
};
```

#### TimeProfiler
```cpp
class TimeProfiler {
public:
    static TimeProfiler& getInstance();
    void startProfile(const std::string& functionName);
    void stopProfile(const std::string& functionName);
    const ProfileData* getProfile(const std::string& functionName) const;
    void printReport(std::ostream& out = std::cout) const;
};
```

## 10. Examples

### Complete Example

```cpp
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/debug/Debug.hpp"
#include <iostream>
#include <memory>

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Starting NLM simulation...");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile("configs/default.cfg")) {
        NLM_LOG_ERROR("Failed to load configuration!");
        return 1;
    }
    
    // Override with command line arguments
    config->loadFromArgs(1, nullptr); // Simplified for example
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Get sensory input
        SensoryInput input;
        brain->receiveSensoryInput(input);
        
        // Step brain
        brain->step(step, step * 0.001);
        
        // Log progress
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        ", Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        ", Firing: " + std::to_string(brain->getAverageFiringRate()));
        }
    }
    
    // Print final statistics
    NLM_LOG_INFO("=== FINAL STATISTICS ===");
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("Neuron count: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Synapse count: " + std::to_string(brain->getTotalSynapseCount()));
    
    return 0;
}
```

### Debugging Example

```cpp
#include "src/debug/Debug.hpp"

void debugFunction() {
    auto& debugger = Debugger::getInstance();
    
    // Set breakpoints
    debugger.addBreakpoint("neuron_count > 1000", "Check neuron count limit");
    debugger.addWatchpoint("currentSpikeCount", "Monitor spike count");
    
    // Start debugging
    debugger.continueExecution();
    
    // Your code here...
    // When conditions are met, debugger will automatically pause
    
    // Inspect variables
    std::string neuronInfo = debugger.inspectVariable("currentNeuron");
    std::cout << "Neuron info: " << neuronInfo << std::endl;
    
    // Step through code
    debugger.stepOver();
}
```

### Performance Monitoring Example

```cpp
#include "src/performance/PerformanceMonitor.hpp"

void computeHeavyFunction() {
    // Your heavy computation here...
    // Simulate some work
    volatile double result = 0.0;
    for (int i = 0; i < 1000000; ++i) {
        result += std::sin(i * 0.01);
    }
}

int main() {
    auto& monitor = PerformanceMonitor::getInstance();
    monitor.reset();
    
    // Run performance test
    double avgTime = monitor.benchmarkFunction(computeHeavyFunction, 100);
    
    std::cout << "Average time per call: " << avgTime * 1000 << " ms" << std::endl;
    
    // Print detailed report
    monitor.printReport();
    
    return 0;
}
```

## References

1. `src/core/Config/Config.hpp` - Configuration system
2. `src/brain/Neuron.hpp` - Neuron implementation
3. `src/performance/MemoryPool.hpp` - Memory pool system
4. `src/debug/Debug.hpp` - Debugging tools
5. `README.md` - Basic usage information
6. `easy_usage.md` - Quick start guide

## License

MIT License

---

*This document is generated automatically as part of the NLM codebase improvements.*
