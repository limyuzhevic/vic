// NLM Documentation Index
// Comprehensive documentation for the Neural Learning Machine (NLM) framework
// This document provides detailed information about the codebase, architecture,
// and usage patterns for developers working with NLM.

# NLM Documentation Index

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [System Components](#system-components)
3. [Code Structure](#code-structure)
4. [Configuration Management](#configuration-management)
5. [Performance Optimization](#performance-optimization)
6. [Testing and Validation](#testing-and-validation)
7. [Development Guidelines](#development-guidelines)
8. [API Reference](#api-reference)
9. [Performance Monitoring](#performance-monitoring)
10. [Extensibility and Plugins](#extensibility-and-plugins)
11. [Migration Guide](#migration-guide)
12. [Troubleshooting](#troubleshooting)

---

# 1. Architecture Overview

## 1.1 Core Architecture

The NLM framework implements a multi-layered architecture for neural simulation:

```
┌─────────────────────────────────────────────────────────────────┐
│                    USER INTERFACE & API                          │
├─────────────────────────────────────────────────────────────────┤
│                    COMMAND LINE INTERFACE                        │
│                    - Advanced CLI options                        │
│                    - Configuration management                   │
│                    - Batch processing tools                     │
├─────────────────────────────────────────────────────────────────┤
│                    BACKEND SERVICES                              │
├─────────────────────────────────────────────────────────────────┤
│                    CONFIGURATION MANAGER                         │
│                    - Hierarchical configuration                 │
│                    - Validation and schema                     │
│                    - Environment-specific settings             │
├─────────────────────────────────────────────────────────────────┤
│                    NEURAL SIMULATION ENGINE                      │
├─────────────────────────────────────────────────────────────────┤
│                    BRAIN CORE                                     │
│                    - Neural regions and populations              │
│                    - Neuron models (LIF, adaptive, etc.)        │
│                    - Synapse models and plasticity              │
│                    - Spike propagation and delays               │
├─────────────────────────────────────────────────────────────────┤
│                    INTEGRATED SYSTEMS                            │
├─────────────────────────────────────────────────────────────────┤
│                    MEMORY SYSTEMS                                │
│                    - Working memory (active traces)             │
│                    - Episodic memory (experience storage)       │
│                    - Associative memory (pattern associations)   │
├─────────────────────────────────────────────────────────────────┤
│                    PREDICTION SYSTEM                              │
│                    - Forward models and predictions             │
│                    - Prediction errors                           │
│                    - Bayesian inference                         │
├─────────────────────────────────────────────────────────────────┤
│                    COGNITION SYSTEMS                             │
│                    - Neural planning                           │
│                    - Concept formation                        │
│                    - Attentional selection                    │
├─────────────────────────────────────────────────────────────────┤
│                    DEVELOPMENT SYSTEM                            │
│                    - Developmental stages                      │
│                    - Structural plasticity                    │
│                    - Critical periods                           │
├─────────────────────────────────────────────────────────────────┤
│                    NEUROMODULATION SYSTEMS                        │
│                    - Dopamine (reward)                          │
│                    - Curiosity (exploration)                   │
│                    - Novelty detection                         │
│                    - Acetylcholine (attention)                  │
├─────────────────────────────────────────────────────────────────┤
│                    COMPUTATIONAL DYNAMICS                        │
│                    - Neural dynamics (spiking models)           │
│                    - Plasticity rules (STDP, Hebbian, etc.)     │
│                    - Neuromodulatory effects                   │
├─────────────────────────────────────────────────────────────────┤
│                    PHYSICS & DYNAMICS                             │
│                    - Leak dynamics                             │
│                    - Refractory periods                        │
│                    - Adaptation mechanisms                    │
├─────────────────────────────────────────────────────────────────┤
│                    ENVIRONMENT & WORLD                           │
│                    - Simulation environment                    │
│                    - Sensory inputs                             │
│                    - Motor outputs                              │
├─────────────────────────────────────────────────────────────────┤
│                    VISUALIZATION & ANALYSIS                      │
│                    - Real-time visualization                   │
│                    - Data export tools                        │
│                    - Statistical analysis                      │
└─────────────────────────────────────────────────────────────────┘
```

## 1.2 Design Principles

### 1.2.1 Modularity
- Each brain system is a separate, interchangeable component
- Components communicate through well-defined interfaces
- Easy to swap implementations without affecting other systems

### 1.2.2 Event-Driven Architecture
- Spike events drive the simulation
- Minimal synchronous computation
- Efficient handling of discrete events

### 1.2.3 Real-Time Optimization
- Optimized for time-critical applications
- Predictable execution times
- Memory-efficient data structures

### 1.2.4 Biological Plausibility
- Based on known neuroscience principles
- Realistic neuron and synapse models
- Validated against experimental data

## 2. System Components

### 2.1 Core Modules

#### 2.1.1 Brain Module (`src/brain/`)
```cpp
// Main neural computation engine
// Contains: Neurons, Synapses, Regions, Populations, Spike system
```

**Key Files:**
- `Brain.hpp/cpp` - Main brain coordination
- `Brain.cpp` - Implementation details
- `Neuron.hpp/cpp` - LIF neuron implementation
- `Synapse.hpp/cpp` - Synaptic transmission
- `NeuralRegion.hpp/cpp` - Brain region management
- `NeuralPopulation.hpp/cpp` - Neuron population management

#### 2.1.2 Memory Systems (`src/memory/`)
```cpp
// Integrated memory architectures
// Working memory: Active information maintenance
// Episodic memory: Experience recording and replay
// Associative memory: Pattern completion and prediction
```

**Key Files:**
- `NeuralWorkingMemory.hpp/cpp` - Working memory implementation
- `NeuralEpisodicMemory.hpp/cpp` - Episodic memory implementation
- `NeuralAssociativeMemory.hpp/cpp` - Associative memory implementation

#### 2.1.3 Neuromodulation (`src/neuromodulation/`)
```cpp
// Biological neuromodulators
// Dopamine: Reward prediction error
// Curiosity: Exploration motivation
// Novelty: Change detection
// Acetylcholine: Attention modulation
```

**Key Files:**
- `Dopamine.hpp/cpp` - Reward system
- `Curiosity.hpp/cpp` - Exploration system
- `Novelty.hpp/cpp` - Change detection
- `Neuromodulator.hpp/cpp` - Base neuromodulator class

#### 2.1.4 Plasticity (`src/plasticity/`)
```cpp
// Synaptic plasticity rules
// STDP: Spike-timing dependent plasticity
// Hebbian: Correlation-based learning
// Structural: Synaptogenesis and pruning
```

**Key Files:**
- `STDP.hpp/cpp` - Spike-timing dependent plasticity
- `Hebbian.hpp/cpp` - Hebbian learning
- `StructuralPlasticity.hpp/cpp` - Structural changes
- `PlasticityRule.hpp/cpp` - Base plasticity class

### 2.2 Supporting Modules

#### 2.2.1 Sensory and Motor (`src/sensory/`, `src/motor/`)
```cpp
// Perception and action systems
// Vision, audio, proprioception
// Movement planning and execution
```

#### 2.2.2 Development (`src/development/`)
```cpp
// Developmental processes
// Synaptogenesis: Formation of new connections
// Pruning: Elimination of connections
// Maturation: Refinement of neural properties
```

#### 2.2.3 Prediction (`src/prediction/`)
```cpp
// Forward models and prediction
// Inverse models and control
// Bayesian inference
```

#### 2.2.4 Cognition (`src/cognition/`)
```cpp
// Higher cognitive functions
// Planning and decision making
// Concept formation
// Attentional selection
```

#### 2.2.5 Environment (`src/environment/`)
```cpp
// Simulation environment
// World model
// Sensory input processing
```

#### 2.2.6 Visualization (`src/visualization/`)
```cpp
// Visualization and analysis
// Real-time display
// Data export
// Statistical analysis
```

### 2.3 Performance and Utility Modules

#### 2.3.1 Performance (`src/performance/`)
```cpp
// Performance optimization and monitoring
// Memory pools, event queues, parallel processing
// Profiling and benchmarking
```

#### 2.3.2 Core Utilities (`src/core/`)
```cpp
// Core system utilities
// Configuration management
// Random number generation
// Logging and monitoring
// Simulation clock
```

#### 2.3.3 Experiments (`src/experiments/`)
```cpp
// Experiment frameworks
// Phase demonstrations
// Validation and testing
// Benchmarks and comparisons
```

## 3. Code Structure

### 3.1 Directory Organization
```
/workspace/app
├── CMakeLists.txt                    # Build configuration
├── pyproject.toml                    # Python package configuration
├── README.md                         # Project documentation
├── src/                             # Main source code
│   ├── brain/                       # Neural computation core
│   ├── memory/                      # Memory systems
│   ├── neuromodulation/              # Neuromodulators
│   ├── plasticity/                  # Plasticity rules
│   ├── development/                 # Developmental systems
│   ├── prediction/                  # Prediction systems
│   ├── cognition/                   # Cognitive systems
│   ├── sensory/                     # Sensory processing
│   ├── motor/                       # Motor systems
│   ├── environment/                 # Environment interface
│   ├── visualization/               # Visualization tools
│   ├── performance/                 # Performance optimization
│   ├── core/                        # Core utilities
│   └── experiments/                 # Experiment frameworks
├── tests/                           # Test suite
│   ├── test_*.cpp                  # Unit tests
│   └── test_main.cpp               # Test runner
├── configs/                         # Configuration files
│   └── *.cfg                       # Configuration templates
├── docs/                            # Documentation
│   └── *.md                        # Documentation files
└── examples/                       # Usage examples
    └── *.cpp                       # Code examples
```

### 3.2 Build System
```cmake
# CMake configuration for NLM
project(NLM VERSION 2.0.0 LANGUAGES CXX)

# Core libraries
set(NEURAL_CORE src/brain/ src/core/)
set(MEMORY_SYSTEMS src/memory/)
set(NEUROMODULATION src/neuromodulation/)
set(PLASTICITY src/plasticity/)
set(DEVELOPMENT src/development/)
set(PREDICTION src/prediction/)
set(COGNITION src/cognition/)
set(SENSORY_MOTOR src/sensory/ src/motor/)
set(ENVIRONMENT src/environment/)
set(VISUALIZATION src/visualization/)
set(PERFORMANCE src/performance/)

# Create executable
add_executable(nlm_main src/main.cpp)
target_link_libraries(nlm_main PRIVATE ${NEURAL_CORE} ${MEMORY_SYSTEMS} ${NEUROMODULATION})

# Create tests
add_executable(nlm_tests tests/test_main.cpp)
target_link_libraries(nlm_tests PRIVATE ${NEURAL_CORE})
```

## 4. Configuration Management

### 4.1 Configuration System
```cpp
// Hierarchical configuration with validation
class Config {
public:
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get values with defaults
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
};
```

### 4.2 Configuration Schema
```yaml
# NLM Configuration Schema
nlm:
  version: 2.0.0
  
  simulation:
    timestep: 0.001                    # Simulation timestep (seconds)
    max_steps: 10000                   # Maximum simulation steps
    random_seed: 42                    # Random seed for reproducibility
    
  network:
    neuron_count: 1000                # Total number of neurons
    region_count: 2                    # Number of brain regions
    connection_probability: 0.1        # Connection probability
    
  plasticity:
    stdp_ltp_weight: 0.01              # STDP LTP weight
    stdp_ltd_weight: 0.012             # STDP LTD weight
    stdp_tau: 20.0                     # STDP time constant
    hebbian_learning_rate: 0.01        # Hebbian learning rate
    synaptogenesis_rate: 0.0001         # Synaptogenesis rate
    pruning_rate: 0.00001              # Pruning rate
    
  memory:
    working_memory_capacity: 100       # Working memory capacity
    episodic_memory_max_episodes: 1000 # Maximum episodes
    associative_memory_pattern_count: 500 # Pattern count
    
  neuromodulation:
    dopamine_baseline: 0.0             # Baseline dopamine level
    curiosity_strength: 0.1            # Curiosity influence
    novelty_threshold: 0.5              # Novelty detection threshold
    
  development:
    developmental_stage: "initial"     # Current developmental stage
    critical_period_start: 10          # Critical period start
    critical_period_end: 100           # Critical period end
    
  environment:
    name: "GridWorld"                  # Environment name
    width: 10                          # Environment width
    height: 10                         # Environment height
    
  logging:
    level: "INFO"                      # Log level
    to_file: false                     # Log to file
    filename: "nlm.log"                # Log filename
    
  visualization:
    enabled: false                     # Enable visualization
    update_rate: 30.0                  # Update rate (Hz)
    
  checkpointing:
    enabled: true                      # Enable checkpointing
    interval: 1000                     # Checkpoint interval
    path: "./checkpoints"              # Checkpoint directory
    compression: "balanced"            # Compression level
```

## 5. Performance Optimization

### 5.1 Performance Characteristics
```cpp
// Performance metrics and measurements
struct PerformanceMetrics {
    double simulation_speed;           // Steps per second
    size_t memory_usage;               // Memory usage (bytes)
    size_t active_neuron_count;        # Active neurons
    size_t spike_count;                # Total spikes processed
    double average_firing_rate;        # Average firing rate (Hz)
    size_t memory_allocations;         # Memory allocations
    double cpu_usage;                  # CPU usage (%)
    std::chrono::duration<double> step_time; # Average step time
};
```

### 5.2 Optimization Strategies
```cpp
// Performance optimization techniques
class PerformanceOptimizer {
public:
    // Memory optimization
    void optimizeMemoryUsage();
    void enableMemoryPools();
    void optimizeDataLayout();
    
    // Computational optimization
    void enableSIMD();
    void enableParallelProcessing();
    void optimizeAlgorithmicComplexity();
    
    // I/O optimization
    void optimizeFileIO();
    void enableAsynchronousOperations();
    void compressData();
};
```

## 6. Testing and Validation

### 6.1 Test Framework
```cpp
// Comprehensive test framework
class TestFramework {
public:
    // Unit testing
    void runUnitTests();
    
    // Integration testing
    void runIntegrationTests();
    
    // Performance testing
    void runPerformanceTests();
    
    // Stress testing
    void runStressTests();
    
    // Validation testing
    void runValidationTests();
};
```

### 6.2 Test Categories
```cpp
// Test organization by category
namespace test_types {
    void runAll();                      // Type system tests
}

namespace test_config {
    void runAll();                      // Configuration tests
}

namespace test_random {
    void runAll();                      // Random number tests
}

namespace test_clock {
    void runAll();                      // Clock tests
}

namespace test_neuron {
    void runAll();                      // Neuron tests
}

namespace test_synapse {
    void runAll();                      // Synapse tests
}

namespace test_stdp {
    void runAll();                      // STDP tests
}

namespace test_brain {
    void runAll();                      // Brain integration tests
}
```

## 7. Development Guidelines

### 7.1 Coding Standards
```cpp
// Modern C++ coding standards
// - Use C++17 features
// - RAII for resource management
// - Smart pointers for memory management
// - noexcept where appropriate
// - Minimal casting
// - Descriptive variable names
// - Comprehensive error handling
```

### 7.2 Documentation Standards
```cpp
// Doxygen-compatible documentation
/**
 * @class ClassName
 * @brief Brief description of the class
 * 
 * Detailed explanation of the class purpose and functionality.
 * 
 * @note Important notes about the class.
 * @see Related classes and functions.
 * @warning Potential issues or limitations.
 * @todo Future improvements and features.
 * 
 * @param param_name Description of parameter.
 * @return Description of return value.
 * 
 * @code
 * // Example usage
 * ClassName obj;
 * obj.method();
 * @endcode
 */
class ClassName {
    /// Public member documentation
    /// Multiple lines for detailed descriptions
    ReturnType publicMethod(ParameterType param) {
        // Implementation
    }
    
private:
    /// Private implementation details
    ImplementationType privateMember;
};
```

### 7.3 Error Handling
```cpp
// Comprehensive error handling
enum class ErrorCode {
    SUCCESS = 0,
    INVALID_ARGUMENT = 1,
    FILE_NOT_FOUND = 2,
    MEMORY_ALLOCATION_FAILED = 3,
    INITIALIZATION_FAILED = 4,
    SIMULATION_ERROR = 5,
    VALIDATION_FAILED = 6
};

class NLMError : public std::runtime_error {
public:
    NLMError(ErrorCode code, const std::string& message);
    
    ErrorCode getCode() const { return code_; }
    const std::string& getMessage() const { return message_; }
    
private:
    ErrorCode code_;
    std::string message_;
};
```

## 8. API Reference

### 8.1 Core API
```cpp
namespace nlm {
    // Brain management
    class Brain;
    class Config;
    
    // Neural components
    class Neuron;
    class Synapse;
    class NeuralRegion;
    class NeuralPopulation;
    
    // Memory systems
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralAssociativeMemory;
    
    // Plasticity rules
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    
    // Neuromodulation
    class Dopamine;
    class Curiosity;
    class Novelty;
    class Neuromodulator;
    
    // Development
    class DevelopmentSystem;
    class Synaptogenesis;
    class Pruning;
    
    // Cognition
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    
    // Environment
    class Environment;
    class SensoryInput;
    class MotorCommand;
    
    // Prediction
    class PredictionSystem;
    class NeuralPrediction;
    
    // Performance
    class PerformanceMonitor;
    class MemoryPool;
    class EventQueue;
    
    // Core utilities
    class Logger;
    class RandomGenerator;
    class SimulationClock;
}
```

### 8.2 Key Function Signatures
```cpp
// Brain API
Brain::Brain(std::shared_ptr<Config> config);
bool Brain::initialize();
void Brain::step(SimulationStep step);
void Brain::step(SimulationStep step, Timestamp time);
void Brain::reset();
bool Brain::save(const std::string& filepath) const;
bool Brain::load(const std::string& filepath);

// Configuration API
Config::Config();
bool Config::loadFromFile(const std::string& filepath);
bool Config::loadFromArgs(int argc, char** argv);
template<typename T>
std::optional<T> Config::get(const std::string& key) const;
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const;
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);

// Neuron API
Neuron::Neuron(NeuronId id);
void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt);
void Neuron::injectCurrent(MembranePotential current);
void Neuron::recordSpike(Timestamp timestamp);
bool Neuron::checkThreshold() const;
void Neuron::reset();

// Synapse API
Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination);
void Synapse::step(Timestamp currentTime);
void Synapse::recordPreSpike(Timestamp timestamp);
void Synapse::recordPostSpike(Timestamp timestamp);
void Synapse::reset();

// Memory API
NeuralWorkingMemory::NeuralWorkingMemory();
void NeuralWorkingMemory::initialize(Brain* brain);
void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation);
float NeuralWorkingMemory::getActivation(NeuronId neuron) const;
size_t NeuralWorkingMemory::getActiveTraces() const;

// Neuromodulation API
Dopamine::Dopamine();
void Dopamine::update(double timestep);
float Dopamine::getLevel() const;
float Dopamine::getPlasticityFactor() const;

// Plasticity API
STDP::STDP();
void STDP::configure(float ltpWeight, float ltdWeight, float tau);
void STDP::update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes, double timestep);
```

## 9. Performance Monitoring

### 9.1 Monitoring Infrastructure
```cpp
class PerformanceMonitor {
public:
    // Real-time metrics collection
    struct SystemMetrics {
        double cpuUsage;                // CPU usage (%)
        size_t memoryUsage;             // Memory usage (bytes)
        size_t activeNeurons;           // Active neurons
        size_t spikeEvents;             // Spike events processed
        double simulationSpeed;         // Steps per second
        double averageFiringRate;       // Average firing rate (Hz)
        size_t memoryAllocations;       // Memory allocations
        size_t memoryDeallocations;     // Memory deallocations
        std::chrono::duration<double> stepTime; // Step processing time
    };
    
    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Metrics access
    SystemMetrics getCurrentMetrics() const;
    std::vector<SystemMetrics> getHistoricalMetrics(size_t count) const;
    
    // Performance analysis
    double calculateEfficiency() const;
    double calculateScalability() const;
    std::string generateReport() const;
};
```

### 9.2 Profiling Tools
```cpp
class PerformanceProfiler {
public:
    // Function profiling
    void startProfile(const std::string& functionName);
    void stopProfile(const std::string& functionName);
    double getProfileTime(const std::string& functionName) const;
    
    // Memory profiling
    void startMemoryProfile();
    void stopMemoryProfile();
    size_t getPeakMemoryUsage() const;
    
    // Timeline analysis
    void startTimeline();
    void stopTimeline();
    std::vector<TimedEvent> getTimeline() const;
    
    // Bottleneck identification
    std::vector<std::string> identifyBottlenecks() const;
    std::vector<std::string> suggestOptimizations() const;
};
```

## 10. Extensibility and Plugins

### 10.1 Plugin Architecture
```cpp
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Plugin initialization
    virtual bool initialize(const Config& config) = 0;
    
    // Plugin cleanup
    virtual void cleanup() = 0;
    
    // Get plugin information
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Extension points
    virtual void onSimulationStart(Brain& brain) = 0;
    virtual void onSimulationStep(Brain& brain, SimulationStep step) = 0;
    virtual void onSimulationEnd(Brain& brain) = 0;
    virtual void onError(const std::exception& error) = 0;
};
```

### 10.2 Plugin Examples
```cpp
// Custom neuron model plugin
class CustomNeuronPlugin : public PluginInterface {
public:
    bool initialize(const Config& config) override;
    void cleanup() override;
    std::string getName() const override { return "CustomNeuron"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Custom LIF neuron model"; }
    
    void onSimulationStart(Brain& brain) override;
    void onSimulationStep(Brain& brain, SimulationStep step) override;
    void onSimulationEnd(Brain& brain) override;
    void onError(const std::exception& error) override;
};

// Custom plasticity rule plugin
class CustomPlasticityPlugin : public PluginInterface {
public:
    bool initialize(const Config& config) override;
    void cleanup() override;
    std::string getName() const override { return "CustomPlasticity"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Custom synaptic plasticity rule"; }
    
    void onSimulationStart(Brain& brain) override;
    void onSimulationStep(Brain& brain, SimulationStep step) override;
    void onSimulationEnd(Brain& brain) override;
    void onError(const std::exception& error) override;
};
```

## 11. Migration Guide

### 11.1 From Previous Versions
```cpp
// Migration from NLM 1.0 to 2.0

// Old API (1.0)
Brain brain(1000, 2);  // Direct parameters
brain.step();

// New API (2.0)
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("region_count", 2);
Brain brain(config);
brain.initialize();
brain.step(0);
```

### 11.2 Configuration Migration
```yaml
# Old configuration format (1.0)
neuron_count: 1000
region_count: 2
simulation_timestep: 0.001

# New configuration format (2.0)
nlm:
  simulation:
    timestep: 0.001
    max_steps: 10000
    random_seed: 42
  
  network:
    neuron_count: 1000
    region_count: 2
    connection_probability: 0.1
  
  plasticity:
    stdp_ltp_weight: 0.01
    stdp_ltd_weight: 0.012
    stdp_tau: 20.0
```

## 12. Troubleshooting

### 12.1 Common Issues

#### 12.1.1 Simulation Crashes
```
// Issue: Simulation crashes with segmentation fault
// Cause: Null pointer dereference or invalid memory access
// Solution: Enable validation and debugging

// Add validation
auto* region = brain->getRegion(RegionId(1));
if (!region) {
    NLM_LOG_ERROR("Region not found");
    return 1;
}

// Add null checks
if (synapse && synapse->getSourceNeuron().isValid()) {
    synapse->enablePlasticity(true, true, false);
}
```

#### 12.1.2 Performance Issues
```
// Issue: Simulation is too slow
// Cause: Inefficient algorithms or memory usage
// Solution: Enable performance optimizations

// Enable SIMD
config->set("enable_simd", true);

// Enable parallel processing
config->set("enable_parallel", true);

// Optimize memory
config->set("enable_memory_pools", true);
```

#### 12.1.3 Memory Issues
```
// Issue: Out of memory
// Cause: Memory leak or insufficient memory
// Solution: Monitor memory usage and fix leaks

// Monitor memory
PerformanceMonitor monitor;
monitor.startMonitoring();

// Check for memory leaks
if (monitor.getPeakMemoryUsage() > memoryLimit) {
    NLM_LOG_ERROR("Memory usage exceeds limit");
    // Fix memory leaks
}
```

### 12.2 Debugging Tools
```cpp
// Enable comprehensive debugging
config->set("log_level", "DEBUG");
config->set("log_to_file", true);
config->set("log_filename", "nlm_debug.log");

// Enable validation
config->set("validate_config", true);

// Enable profiling
PerformanceProfiler profiler;
profiler.startProfile("simulation");
profiler.startTimeline();
```

## Conclusion

This comprehensive documentation provides a complete guide to the NLM framework, covering:

1. **Architecture and Design Principles** - Understanding the system's structure and philosophy
2. **Code Structure and Organization** - Navigating the codebase
3. **Configuration Management** - Managing simulation parameters
4. **Performance Optimization** - Improving simulation efficiency
5. **Testing and Validation** - Ensuring reliability
6. **Development Guidelines** - Best practices for contributions
7. **API Reference** - Complete API documentation
8. **Performance Monitoring** - Tracking and analyzing performance
9. **Extensibility and Plugins** - Extending the system
10. **Migration Guide** - Upgrading from previous versions
11. **Troubleshooting** - Solving common issues

The NLM framework is a production-ready neural simulation system that combines:

- **Biological Plausibility** - Accurate neuron and synapse models
- **Computational Efficiency** - Optimized algorithms and data structures
- **Extensibility** - Plugin architecture for customization
- **Real-Time Capability** - Suitable for time-critical applications
- **Comprehensive Monitoring** - Detailed performance tracking
- **Robust Error Handling** - Comprehensive error management

This documentation serves as both a reference guide and a development handbook, enabling developers to understand, use, and extend the NLM framework effectively.